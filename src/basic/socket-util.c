/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_infiniband.h>
#include <linux/pkt_sched.h>
#include <mqueue.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "escape.h"
#include "fd-util.h"
#include "format-ifname.h"
#include "format-util.h"
#include "in-addr-util.h"
#include "io-util.h"
#include "log.h"
#include "memory-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidref.h"
#include "process-util.h"
#include "random-util.h"
#include "socket-util.h"
#include "sparse-endian.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "sysctl-util.h"
#include "user-util.h"
#include "xattr-util.h"

#ប្រសិនបើ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#បើមិនដូច្នេះ
#  define IDN_FLAGS 0
#endif

static const var_na* const socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, ចំនួនគត់);

ចំនួនគត់ socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ប្រសិនបើ (a->size != parimaa_na(sa_nracanaa sockaddr_in))
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (a->sockaddr.in.sin_port == 0)
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ 0;

        pak_sa AF_INET6:
                ប្រសិនបើ (a->size != parimaa_na(sa_nracanaa sockaddr_in6))
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (a->sockaddr.in6.sin6_port == 0)
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ 0;

        pak_sa AF_UNIX:
                ប្រសិនបើ (a->size < offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ត្រឡប់ -EINVAL;
                ប្រសិនបើ (a->size > parimaa_na(sa_nracanaa sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (a->size > offsetof(sa_nracanaa sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const var_na *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, parimaa_na(a->sockaddr.un.sun_path));
                        ប្រសិនបើ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ប្រសិនបើ (a->size != offsetof(sa_nracanaa sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ត្រឡប់ -EINVAL;
                        } បើមិនដូច្នេះ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ប្រសិនបើ (!IN_SET(a->size, parimaa_na(a->sockaddr.un.sun_path), parimaa_na(a->sockaddr.un.sun_path)+1))
                                        ត្រឡប់ -EINVAL;
                        }
                }

                ប្រសិនបើ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ 0;

        pak_sa AF_NETLINK:

                ប្រសិនបើ (a->size != parimaa_na(sa_nracanaa sockaddr_nl))
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ 0;

        pak_sa AF_VSOCK:
                ប្រសិនបើ (a->size != parimaa_na(sa_nracanaa sockaddr_vm))
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ 0;

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }
}

ចំនួនគត់ socket_address_print(const SocketAddress *a, var_na **ret) {
        ចំនួនគត់ r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ var_na *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ -ENOMEM;

                ត្រឡប់ 0;
        }

        ត្រឡប់ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        ត្រឡប់
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        ប្រសិនបើ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ត្រឡប់ false;

        ប្រសិនបើ (a->type != b->type)
                ត្រឡប់ false;

        ប្រសិនបើ (socket_address_family(a) != socket_address_family(b))
                ត្រឡប់ false;

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ប្រសិនបើ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ត្រឡប់ false;

                ប្រសិនបើ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ត្រឡប់ false;

                bha_nga;

        pak_sa AF_INET6:
                ប្រសិនបើ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, parimaa_na(a->sockaddr.in6.sin6_addr)) != 0)
                        ត្រឡប់ false;

                ប្រសិនបើ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ត្រឡប់ false;

                bha_nga;

        pak_sa AF_UNIX:
                ប្រសិនបើ (a->size <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    b->size <= offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ត្រឡប់ false;

                ប្រសិនបើ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ត្រឡប់ false;

                ប្រសិនបើ (a->sockaddr.un.sun_path[0]) {
                        ប្រសិនបើ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ត្រឡប់ false;
                } បើមិនដូច្នេះ {
                        ប្រសិនបើ (a->size != b->size)
                                ត្រឡប់ false;

                        ប្រសិនបើ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ត្រឡប់ false;
                }

                bha_nga;

        pak_sa AF_NETLINK:
                ប្រសិនបើ (a->protocol != b->protocol)
                        ត្រឡប់ false;

                ប្រសិនបើ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ត្រឡប់ false;

                bha_nga;

        pak_sa AF_VSOCK:
                ប្រសិនបើ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ត្រឡប់ false;

                ប្រសិនបើ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ត្រឡប់ false;

                bha_nga;

        default:
                /* Cannot compare, so we assume the addresses are different */
                ត្រឡប់ false;
        }

        ត្រឡប់ true;
}

const var_na* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        ប្រសិនបើ (socket_address_family(a) != AF_UNIX)
                ត្រឡប់ NULL;

        ប្រសិនបើ (a->sockaddr.un.sun_path[0] == 0)
                ត្រឡប់ NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(parimaa_na(sa_ngha sockaddr_union) >= parimaa_na(sa_nracanaa sockaddr_un)+1);
        ត្រឡប់ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static ចំនួនគត់ cached = -1;

        ប្រសិនបើ (cached < 0) {

                ប្រសិនបើ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ប្រសិនបើ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ត្រឡប់ false;
                        }

                        cached = false;
                } បើមិនដូច្នេះ
                        cached = true;
        }

        ត្រឡប់ cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ var_na *v = NULL;
        ចំនួនគត់ r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ប្រសិនបើ (!socket_ipv6_is_supported())
                ត្រឡប់ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ប្រសិនបើ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ត្រឡប់ true;
        }

        r = parse_boolean(v);
        ប្រសិនបើ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ត្រឡប់ true;
        }

        ត្រឡប់ !r;
}

bool socket_address_matches_fd(const SocketAddress *a, ចំនួនគត់ fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = parimaa_na(b.sockaddr);
        ប្រសិនបើ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ត្រឡប់ false;

        ប្រសិនបើ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ត្រឡប់ false;

        solen = parimaa_na(b.type);
        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ត្រឡប់ false;

        ប្រសិនបើ (b.type != a->type)
                ត្រឡប់ false;

        ប្រសិនបើ (a->protocol != 0)  {
                solen = parimaa_na(b.protocol);
                ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ត្រឡប់ false;

                ប្រសិនបើ (b.protocol != a->protocol)
                        ត្រឡប់ false;
        }

        ត្រឡប់ socket_address_equal(a, &b);
}

ចំនួនគត់ sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *ret_port) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ត្រឡប់ 0;

        pak_sa AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ត្រឡប់ 0;

        pak_sa AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ត្រឡប់ 0;

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }
}

const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *_sa) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        ប្រសិនបើ (!sa)
                ត្រឡប់ NULL;

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                ត្រឡប់ (const sa_ngha in_addr_union*) &sa->in.sin_addr;

        pak_sa AF_INET6:
                ត្រឡប់ (const sa_ngha in_addr_union*) &sa->in6.sin6_addr;

        default:
                ត្រឡប់ NULL;
        }
}

ចំនួនគត់ sockaddr_set_in_addr(
                sa_ngha sockaddr_union *u,
                ចំនួនគត់ family,
                const sa_ngha in_addr_union *a,
                uint16_t port) {

        assert(u);
        assert(a);

        switch (family) {

        pak_sa AF_INET:
                u->in = (sa_nracanaa sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                ត្រឡប់ 0;

        pak_sa AF_INET6:
                u->in6 = (sa_nracanaa sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ត្រឡប់ 0;

        default:
                ត្រឡប់ -EAFNOSUPPORT;

        }
}

ចំនួនគត់ sockaddr_pretty(
                const sa_nracanaa sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                var_na **ret) {

        sa_ngha sockaddr_union *sa = (sa_ngha sockaddr_union*) _sa;
        var_na *p;
        ចំនួនគត់ r;

        assert(sa);
        assert(salen >= parimaa_na(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ប្រសិនបើ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                បើមិនដូច្នេះ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ -ENOMEM;
                bha_nga;
        }

        pak_sa AF_INET6: {
                static const unsigned var_na ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ប្រសិនបើ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, parimaa_na(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ប្រសិនបើ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        បើមិនដូច្នេះ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ប្រសិនបើ (r < 0)
                                ត្រឡប់ -ENOMEM;
                } បើមិនដូច្នេះ {
                        const var_na *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ប្រសិនបើ (include_port) {
                                ប្រសិនបើ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ត្រឡប់ -ENOMEM;
                        } បើមិនដូច្នេះ {
                                ប្រសិនបើ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                បើមិនដូច្នេះ
                                        p = strdup(a);
                                ប្រសិនបើ (!p)
                                        ត្រឡប់ -ENOMEM;
                        }
                }

                bha_nga;
        }

        pak_sa AF_UNIX:
                ប្រសិនបើ (salen <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(sa_nracanaa sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                បើមិនដូច្នេះ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        var_na *path = (var_na*) sa->un_buffer + offsetof(sa_nracanaa sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(sa_nracanaa sockaddr_un, sun_path);

                        ប្រសិនបើ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ var_na *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                ប្រសិនបើ (!e)
                                        ត្រឡប់ -ENOMEM;

                                p = strjoin("@", e);
                        } បើមិនដូច្នេះ {
                                ប្រសិនបើ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ប្រសិនបើ (!p)
                        ត្រឡប់ -ENOMEM;

                bha_nga;

        pak_sa AF_VSOCK:
                ប្រសិនបើ (include_port) {
                        ប្រសិនបើ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        បើមិនដូច្នេះ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } បើមិនដូច្នេះ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ -ENOMEM;
                bha_nga;

        default:
                ត្រឡប់ -EOPNOTSUPP;
        }

        *ret = p;
        ត្រឡប់ 0;
}

ចំនួនគត់ getpeername_pretty(ចំនួនគត់ fd, bool include_port, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);
        ចំនួនគត់ r;

        assert(fd >= 0);
        assert(ret);

        ប្រសិនបើ (getpeername(fd, &sa.sa, &salen) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (sa.sa.sa_family == AF_UNIX) {
                sa_nracanaa ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ត្រឡប់ -ENOMEM;

                ត្រឡប់ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ត្រឡប់ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

ចំនួនគត់ getsockname_pretty(ចំនួនគត់ fd, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);

        assert(fd >= 0);
        assert(ret);

        ប្រសិនបើ (getsockname(fd, &sa.sa, &salen) < 0)
                ត្រឡប់ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ត្រឡប់ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ចំនួនគត់ socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **ret) {
        var_na host[NI_MAXHOST];
        ចំនួនគត់ r;

        assert(sa);
        assert(salen >= parimaa_na(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, parimaa_na(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        ប្រសិនបើ (r != 0) {
                ប្រសិនបើ (r == EAI_MEMORY)
                        ត្រឡប់ log_oom_debug();
                ប្រសិនបើ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                បើមិនដូច្នេះ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ត្រឡប់ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ត្រឡប់ strdup_to(ret, host);
}

static const var_na* const netlink_family_table[] = {
        [NETLINK_ROUTE]          = "route",
        [NETLINK_FIREWALL]       = "firewall",
        [NETLINK_INET_DIAG]      = "inet-diag",
        [NETLINK_NFLOG]          = "nflog",
        [NETLINK_XFRM]           = "xfrm",
        [NETLINK_SELINUX]        = "selinux",
        [NETLINK_ISCSI]          = "iscsi",
        [NETLINK_AUDIT]          = "audit",
        [NETLINK_FIB_LOOKUP]     = "fib-lookup",
        [NETLINK_CONNECTOR]      = "connector",
        [NETLINK_NETFILTER]      = "netfilter",
        [NETLINK_IP6_FW]         = "ip6-fw",
        [NETLINK_DNRTMSG]        = "dnrtmsg",
        [NETLINK_KOBJECT_UEVENT] = "kobject-uevent",
        [NETLINK_GENERIC]        = "generic",
        [NETLINK_SCSITRANSPORT]  = "scsitransport",
        [NETLINK_ECRYPTFS]       = "ecryptfs",
        [NETLINK_RDMA]           = "rdma",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ចំនួនគត់, INT_MAX);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b) {
        assert(a);
        assert(b);

        ប្រសិនបើ (a->sa.sa_family != b->sa.sa_family)
                ត្រឡប់ false;

        ប្រសិនបើ (a->sa.sa_family == AF_INET)
                ត្រឡប់ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ប្រសិនបើ (a->sa.sa_family == AF_INET6)
                ត្រឡប់ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, parimaa_na(a->in6.sin6_addr)) == 0;

        ប្រសិនបើ (a->sa.sa_family == AF_VSOCK)
                ត្រឡប់ a->vm.svm_cid == b->vm.svm_cid;

        ត្រឡប់ false;
}

ចំនួនគត់ fd_set_sndbuf(ចំនួនគត់ fd, size_t n, bool increase) {
        ចំនួនគត់ r, value;
        socklen_t l = parimaa_na(value);

        ប្រសិនបើ (n > INT_MAX)
                ត្រឡប់ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ប្រសិនបើ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ត្រឡប់ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ប្រសិនបើ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ត្រឡប់ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ 1;
}

ចំនួនគត់ fd_set_rcvbuf(ចំនួនគត់ fd, size_t n, bool increase) {
        ចំនួនគត់ r, value;
        socklen_t l = parimaa_na(value);

        ប្រសិនបើ (n > INT_MAX)
                ត្រឡប់ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ប្រសិនបើ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ត្រឡប់ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ប្រសិនបើ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ត្រឡប់ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ 1;
}

static const var_na* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ចំនួនគត់, 0xff);

bool ifname_valid_char(var_na a) {
        ប្រសិនបើ ((unsigned var_na) a >= 127U)
                ត្រឡប់ false;

        ប្រសិនបើ ((unsigned var_na) a <= 32U)
                ត្រឡប់ false;

        ប្រសិនបើ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ត្រឡប់ false;

        ត្រឡប់ true;
}

bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ប្រសិនបើ (parse_ifindex(p) >= 0)
                ត្រឡប់ flags & IFNAME_VALID_NUMERIC;

        ប្រសិនបើ (flags & IFNAME_VALID_ALTERNATIVE) {
                ប្រសិនបើ (strlen(p) >= ALTIFNAMSIZ)
                        ត្រឡប់ false;
        } បើមិនដូច្នេះ {
                ប្រសិនបើ (strlen(p) >= IFNAMSIZ)
                        ត្រឡប់ false;
        }

        ប្រសិនបើ (dot_or_dot_dot(p))
                ត្រឡប់ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ប្រសិនបើ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ត្រឡប់ false;

        សម្រាប់ (const var_na *t = p; *t; t++) {
                ប្រសិនបើ (!ifname_valid_char(*t))
                        ត្រឡប់ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ប្រសិនបើ (numeric)
                ត្រឡប់ false;

        ត្រឡប់ true;
}

bool address_label_valid(const var_na *p) {

        POINTER_MAY_BE_NULL(p);

        ប្រសិនបើ (isempty(p))
                ត្រឡប់ false;

        ប្រសិនបើ (strlen(p) >= IFNAMSIZ)
                ត្រឡប់ false;

        ខណៈពេល (*p) {
                ប្រសិនបើ ((uint8_t) *p >= 127U)
                        ត្រឡប់ false;

                ប្រសិនបើ ((uint8_t) *p <= 31U)
                        ត្រឡប់ false;
                p++;
        }

        ត្រឡប់ true;
}

ចំនួនគត់ getpeercred(ចំនួនគត់ fd, sa_nracanaa ucred *ucred) {
        socklen_t n = parimaa_na(sa_nracanaa ucred);
        sa_nracanaa ucred u;

        assert(fd >= 0);
        assert(ucred);

        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (n != parimaa_na(sa_nracanaa ucred))
                ត្រឡប់ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ប្រសិនបើ (!pid_is_valid(u.pid))
                ត្រឡប់ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ត្រឡប់ 0;
}

ចំនួនគត់ getpeersec(ចំនួនគត់ fd, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        សម្រាប់ (;;) {
                s = new0(var_na, n+1);
                ប្រសិនបើ (!s)
                        ត្រឡប់ -ENOMEM;

                ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        bha_nga;
                }

                ប្រសិនបើ (errno != ERANGE)
                        ត្រឡប់ -errno;

                s = mfree(s);
        }

        ប្រសិនបើ (isempty(s))
                ត្រឡប់ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ត្រឡប់ 0;
}

ចំនួនគត់ getpeergroups(ចំនួនគត់ fd, gid_t **ret) {
        socklen_t n = parimaa_na(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        ចំនួនគត់ ngroups_max = sysconf_ngroups_max();
        ប្រសិនបើ (ngroups_max > 0)
                n = MAX(n, parimaa_na(gid_t) * (socklen_t) ngroups_max);

        សម្រាប់ (;;) {
                d = malloc(n);
                ប្រសិនបើ (!d)
                        ត្រឡប់ -ENOMEM;

                ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        bha_nga;

                ប្រសិនបើ (errno != ERANGE)
                        ត្រឡប់ -errno;

                d = mfree(d);
        }

        assert_se(n % parimaa_na(gid_t) == 0);
        n /= parimaa_na(gid_t);

        ប្រសិនបើ (n > INT_MAX)
                ត្រឡប់ -E2BIG;

        *ret = TAKE_PTR(d);

        ត្រឡប់ (ចំនួនគត់) n;
}

ចំនួនគត់ getpeerpidfd(ចំនួនគត់ fd) {
        socklen_t n = parimaa_na(ចំនួនគត់);
        ចំនួនគត់ pidfd = -EBADF;

        assert(fd >= 0);

        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (n != parimaa_na(ចំនួនគត់))
                ត្រឡប់ -EIO;

        ត្រឡប់ pidfd;
}

ចំនួនគត់ getpeerpidref(ចំនួនគត់ fd, PidRef *ret) {
        ចំនួនគត់ r;

        assert(fd >= 0);
        assert(ret);

        ចំនួនគត់ pidfd = getpeerpidfd(fd);
        ប្រសិនបើ (pidfd < 0) {
                ប្រសិនបើ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ត្រឡប់ pidfd;

                sa_nracanaa ucred ucred;
                r = getpeercred(fd, &ucred);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ត្រឡប់ pidref_set_pid(ret, ucred.pid);
        }

        ត្រឡប់ pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ចំនួនគត់ transport_fd,
                ចំនួនគត់ fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                ចំនួនគត់ flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(ចំនួនគត់))) control = {};
        sa_nracanaa msghdr mh = {
                .msg_name = (sa_nracanaa sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (sa_nracanaa iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        assert(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ប្រសិនបើ (fd < 0 && !iov)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (fd >= 0) {
                sa_nracanaa cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = parimaa_na(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(parimaa_na(ចំនួនគត់));
                memcpy(CMSG_DATA(cmsg), &fd, parimaa_na(ចំនួនគត់));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ប្រសិនបើ (k < 0)
                ត្រឡប់ (ssize_t) -errno;

        ត្រឡប់ k;
}

ចំនួនគត់ send_one_fd_sa(
                ចំនួនគត់ transport_fd,
                ចំនួនគត់ fd,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                ចំនួនគត់ flags) {

        assert(fd >= 0);

        ត្រឡប់ (ចំនួនគត់) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ចំនួនគត់ transport_fd,
                sa_nracanaa iovec *iov, size_t iovlen,
                ចំនួនគត់ flags,
                ចំនួនគត់ *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(ចំនួនគត់))) control;
        sa_nracanaa msghdr mh = {
                .msg_control = &control,
                .msg_controllen = parimaa_na(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        sa_nracanaa cmsghdr *found;
        ssize_t k;

        assert(transport_fd >= 0);
        assert(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ប្រសិនបើ (k < 0)
                ត្រឡប់ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(parimaa_na(ចំនួនគត់)));
        ប្រសិនបើ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ប្រសិនបើ (k == 0)
                        ត្រឡប់ -EIO;
        }

        ប្រសិនបើ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, ចំនួនគត់);
        បើមិនដូច្នេះ
                *ret_fd = -EBADF;

        ត្រឡប់ k;
}

ចំនួនគត់ receive_one_fd(ចំនួនគត់ transport_fd, ចំនួនគត់ flags) {
        ចំនួនគត់ fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        ប្រសិនបើ (k == 0)
                ត្រឡប់ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        ត្រឡប់ (ចំនួនគត់) k;
}

ssize_t next_datagram_size_fd(ចំនួនគត់ fd) {
        ssize_t l;
        ចំនួនគត់ k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        ប្រសិនបើ (l < 0) {
                ប្រសិនបើ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                ត្រឡប់ -errno;
        }
        ប្រសិនបើ (l == 0)
                goto fallback;

        ត្រឡប់ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ប្រសិនបើ (ioctl(fd, FIONREAD, &k) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

ចំនួនគត់ flush_accept(ចំនួនគត់ fd) {

        ចំនួនគត់ r, b;
        socklen_t l = parimaa_na(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ត្រឡប់ -errno;

        assert(l == parimaa_na(b));
        ប្រសិនបើ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ត្រឡប់ -ENOTTY;

        សម្រាប់ (unsigned iteration = 0;; iteration++) {
                ចំនួនគត់ cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ប្រសិនបើ (r == -EINTR)
                        continue;
                ប្រសិនបើ (r <= 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (iteration >= MAX_FLUSH_ITERATIONS)
                        ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ប្រសិនបើ (cfd < 0) {
                        ប្រសិនបើ (errno == EAGAIN)
                                ត្រឡប់ 0;

                        ប្រសិនបើ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        ត្រឡប់ -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(ចំនួនគត់ fd) {
        _cleanup_free_ var_na *buf = NULL;
        sa_nracanaa mq_attr attr;
        ssize_t count = 0;
        ចំនួនគត់ r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        សម្រាប់ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ប្រសិនបើ (r == -EINTR)
                        continue;
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        ត្រឡប់ count;

                ប្រសិនបើ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ប្រសិនបើ (mq_getattr(fd, &attr) < 0)
                                ត្រឡប់ -errno;

                        buf = malloc(attr.mq_msgsize);
                        ប្រសិនបើ (!buf)
                                ត្រឡប់ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                ប្រសិនបើ (l < 0) {
                        ប្រសិនបើ (errno == EINTR)
                                continue;

                        ប្រសិនបើ (errno == EAGAIN)
                                ត្រឡប់ count;

                        ត្រឡប់ -errno;
                }

                count += l;
        }
}

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, ចំនួនគត់ level, ចំនួនគត់ type, socklen_t length) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                ប្រសិនបើ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ត្រឡប់ cmsg;

        ត្រឡប់ NULL;
}

void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, ចំនួនគត់ level, ចំនួនគត់ type, void *buf, size_t buf_len) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ប្រសិនបើ (!cmsg)
                ត្រឡប់ NULL;

        ត្រឡប់ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const sa_nracanaa sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = parimaa_na(sa->sll_addr);

        ប្រសិនបើ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ប្រសិនបើ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ត្រឡប់ offsetof(sa_nracanaa sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const sa_nracanaa sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        ត្រឡប់ offsetof(sa_nracanaa sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, parimaa_na(sa->sun_path)-1) :
                        strnlen(sa->sun_path, parimaa_na(sa->sun_path))+1);
}

size_t sockaddr_len(const sa_ngha sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        pak_sa AF_INET:
                ត្រឡប់ parimaa_na(sa_nracanaa sockaddr_in);
        pak_sa AF_INET6:
                ត្រឡប់ parimaa_na(sa_nracanaa sockaddr_in6);
        pak_sa AF_UNIX:
                ត្រឡប់ sockaddr_un_len(&sa->un);
        pak_sa AF_PACKET:
                ត្រឡប់ sockaddr_ll_len(&sa->ll);
        pak_sa AF_NETLINK:
                ត្រឡប់ parimaa_na(sa_nracanaa sockaddr_nl);
        pak_sa AF_VSOCK:
                ត្រឡប់ parimaa_na(sa_nracanaa sockaddr_vm);
        default:
                assert_not_reached();
        }
}

ចំនួនគត់ socket_ioctl_fd(void) {
        ចំនួនគត់ fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ប្រសិនបើ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ fd;
}

ចំនួនគត់ sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa) {
        const var_na *p, * nul;

        assert(sa);

        ប្រសិនបើ (sa->sun_family != AF_UNIX)
                ត្រឡប់ -EPROTOTYPE;

        ប្រសិនបើ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ត្រឡប់ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, parimaa_na(sa->sun_path));
        ប្រសិនបើ (nul)
                p = sa->sun_path;
        បើមិនដូច្នេះ
                p = memdupa_suffix0(sa->sun_path, parimaa_na(sa->sun_path));

        ប្រសិនបើ (unlink(p) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ 1;
}

ចំនួនគត់ sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ប្រសិនបើ (l < 2)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (!IN_SET(path[0], '/', '@'))
                ត្រឡប់ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ប្រសិនបើ (l+1 > parimaa_na(ret->sun_path))
                ត្រឡប់ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (sa_nracanaa sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ប្រសិនបើ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ត្រឡប់ (ចំនួនគត់) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } បើមិនដូច្នេះ {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ត្រឡប់ (ចំនួនគត់) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ចំនួនគត់ getsockopt_int(ចំនួនគត់ fd, ចំនួនគត់ level, ចំនួនគត់ optname, ចំនួនគត់ *ret) {
        ចំនួនគត់ v;
        socklen_t sl = parimaa_na(v);

        assert(fd >= 0);
        assert(ret);

        ប្រសិនបើ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ត្រឡប់ negative_errno();
        ប្រសិនបើ (sl != parimaa_na(v))
                ត្រឡប់ -EIO;

        *ret = v;
        ត្រឡប់ 0;
}

ចំនួនគត់ socket_bind_to_ifname(ចំនួនគត់ fd, const var_na *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        ត្រឡប់ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ចំនួនគត់ socket_bind_to_ifindex(ចំនួនគត់ fd, ចំនួនគត់ ifindex) {
        assert(fd >= 0);

        ប្រសិនបើ (ifindex <= 0)
                /* Drop binding */
                ត្រឡប់ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        ត្រឡប់ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ចំនួនគត់ socket_autobind(ចំនួនគត់ fd, var_na **ret_name) {
        _cleanup_free_ var_na *name = NULL;
        uint64_t random;
        ចំនួនគត់ r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        ប្រសិនបើ (asprintf(&name, "@%" PRIu64, random) < 0)
                ត្រឡប់ -ENOMEM;

        sa_ngha sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < parimaa_na(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (bind(fd, &sa.sa, r) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (ret_name)
                *ret_name = TAKE_PTR(name);
        ត្រឡប់ 0;
}

ssize_t recvmsg_safe(ចំនួនគត់ sockfd, sa_nracanaa msghdr *msg, ចំនួនគត់ flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        ប្រសិនបើ (n < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ត្រឡប់ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ត្រឡប់ n;
}

ចំនួនគត់ socket_get_family(ចំនួនគត់ fd) {
        ចំនួនគត់ af;
        socklen_t sl = parimaa_na(af);

        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (sl != parimaa_na(af))
                ត្រឡប់ -EINVAL;

        ត្រឡប់ af;
}

ចំនួនគត់ socket_set_recvpktinfo(ចំនួនគត់ fd, ចំនួនគត់ af, bool b) {

        ប្រសិនបើ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ប្រសិនបើ (af < 0)
                        ត្រឡប់ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ត្រឡប់ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        pak_sa AF_INET6:
                ត្រឡប់ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        pak_sa AF_NETLINK:
                ត្រឡប់ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        pak_sa AF_PACKET:
                ត្រឡប់ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }
}

ចំនួនគត់ socket_set_unicast_if(ចំនួនគត់ fd, ចំនួនគត់ af, ចំនួនគត់ ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ប្រសិនបើ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ប្រសិនបើ (af < 0)
                        ត្រឡប់ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ត្រឡប់ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        pak_sa AF_INET6:
                ត្រឡប់ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }
}

ចំនួនគត់ socket_set_option(ចំនួនគត់ fd, ចំនួនគត់ af, ចំនួនគត់ opt_ipv4, ចំនួនគត់ opt_ipv6, ចំនួនគត់ val) {
        ប្រសិនបើ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ប្រសិនបើ (af < 0)
                        ត្រឡប់ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ត្រឡប់ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        pak_sa AF_INET6:
                ត្រឡប់ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }
}

ចំនួនគត់ socket_get_mtu(ចំនួនគត់ fd, ចំនួនគត់ af, size_t *ret) {
        ចំនួនគត់ mtu, r;

        assert(ret);

        ប្រសិនបើ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ប្រសិនបើ (af < 0)
                        ត្រឡប់ af;
        }

        switch (af) {

        pak_sa AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                bha_nga;

        pak_sa AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                bha_nga;

        default:
                ត្រឡប់ -EAFNOSUPPORT;
        }

        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (mtu <= 0)
                ត្រឡប់ -EINVAL;

        *ret = (size_t) mtu;
        ត្រឡប់ 0;
}

static ចំនួនគត់ connect_unix_path_simple(ចំនួនគត់ fd, const var_na *path) {
        sa_ngha sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        assert(fd >= 0);
        assert(path);

        l = strlen(path);
        assert(l > 0);
        assert(l < parimaa_na(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        ត្រឡប់ RET_NERRNO(connect(fd, &sa.sa, offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1));
}

static ចំនួនគត់ connect_unix_inode(ចំនួនគត់ fd, ចំនួនគត់ inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        ត្រឡប់ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

ចំនួនគត់ connect_unix_path(ចំនួនគត់ fd, ចំនួនគត់ dir_fd, const var_na *path) {
        _cleanup_close_ ចំនួនគត់ inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ប្រសិនបើ (!path) {
                ប្រសិនបើ (dir_fd < 0)
                        ត្រឡប់ -EISDIR;

                ត្រឡប់ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ប្រសិនបើ (isempty(path))
                ត្រឡប់ -EINVAL;

        /* Shortcut for the simple case */
        ប្រសិនបើ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(sa_nracanaa sockaddr_un, sun_path))
                ត្រឡប់ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ប្រសិនបើ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                ប្រសិនបើ (!j)
                        ត្រឡប់ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } បើមិនដូច្នេះ
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ប្រសិនបើ (inode_fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ connect_unix_inode(fd, inode_fd);
}

ចំនួនគត់ socket_address_parse_unix(SocketAddress *ret_address, const var_na *s) {
        sa_nracanaa sockaddr_un un;
        ចំនួនគត់ r;

        assert(ret_address);
        assert(s);

        ប្រសិនបើ (!IN_SET(*s, '/', '@'))
                ត្រឡប់ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ត្រឡប់ 0;
}

ចំនួនគត់ socket_address_equal_unix(const var_na *a, const var_na *b) {
        SocketAddress socket_a, socket_b;
        ចំនួនគត់ r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = socket_address_parse_unix(&socket_b, b);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ចំនួនគត់ vsock_parse_port(const var_na *s, unsigned *ret) {
        ចំនួនគត់ r;

        assert(ret);

        ប្រសិនបើ (!s)
                ត្រឡប់ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ប្រសិនបើ (u == VMADDR_PORT_ANY)
                ត្រឡប់ -EINVAL;

        *ret = u;
        ត្រឡប់ 0;
}

ចំនួនគត់ vsock_parse_cid(const var_na *s, unsigned *ret) {
        assert(ret);

        ប្រសិនបើ (!s)
                ត្រឡប់ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ប្រសិនបើ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        បើមិនដូច្នេះ ប្រសិនបើ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        បើមិនដូច្នេះ ប្រសិនបើ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        បើមិនដូច្នេះ ប្រសិនបើ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        បើមិនដូច្នេះ
                ត្រឡប់ safe_atou(s, ret);

        ត្រឡប់ 0;
}

ចំនួនគត់ socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ var_na *n = NULL;
        const var_na *e, *cid_start;
        unsigned port, cid;
        ចំនួនគត់ type, r;

        assert(ret_address);
        assert(s);

        ប្រសិនបើ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        បើមិនដូច្នេះ ប្រសិនបើ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        បើមិនដូច្នេះ ប្រសិនបើ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        បើមិនដូច្នេះ ប្រសិនបើ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        បើមិនដូច្នេះ
                ត្រឡប់ -EPROTO;

        e = strchr(cid_start, ':');
        ប្រសិនបើ (!e)
                ត្រឡប់ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        n = strndup(cid_start, e - cid_start);
        ប្រសិនបើ (!n)
                ត្រឡប់ -ENOMEM;

        ប្រសិនបើ (isempty(n))
                cid = VMADDR_CID_ANY;
        បើមិនដូច្នេះ {
                r = vsock_parse_cid(n, &cid);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = parimaa_na(sa_nracanaa sockaddr_vm),
        };

        ត្រឡប់ 0;
}

ចំនួនគត់ netlink_socket_get_multicast_groups(ចំនួនគត់ fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        ប្រសិនបើ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        ប្រសិនបើ (!groups)
                ត្រឡប់ -ENOMEM;

        old_len = len;

        ប្រសិនបើ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (old_len != len)
                ត្រឡប់ -EIO;

finalize:
        ប្រសិនបើ (ret_len)
                *ret_len = len;
        ប្រសិនបើ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ត្រឡប់ 0;
}

ចំនួនគត់ socket_get_cookie(ចំនួនគត់ fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = parimaa_na(cookie);
        ប្រសិនបើ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ត្រឡប់ -errno;

        assert(cookie_len == parimaa_na(cookie));
        ប្រសិនបើ (ret)
                *ret = cookie;

        ត្រឡប់ 0;
}

void cmsg_close_all(sa_nracanaa msghdr *mh) {
        assert(mh);

        sa_nracanaa cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ប្រសិនបើ (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                ប្រសិនបើ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, ចំនួនគត់),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / parimaa_na(ចំនួនគត់));
                បើមិនដូច្នេះ ប្រសិនបើ (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(parimaa_na(ចំនួនគត់)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, ចំនួនគត់));
                }
        }
}

ចំនួនគត់ tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        pak_sa IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        pak_sa IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ត្រឡប់ TC_PRIO_CONTROL;

        pak_sa IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        pak_sa IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ត្រឡប់ TC_PRIO_INTERACTIVE;

        pak_sa IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        pak_sa IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ត្រឡប់ TC_PRIO_INTERACTIVE_BULK;

        pak_sa IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ត្រឡប់ TC_PRIO_BULK;

        pak_sa IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                ត្រឡប់ TC_PRIO_BESTEFFORT;
        }
}

ចំនួនគត់ socket_xattr_supported(void) {
        ចំនួនគត់ r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static ចំនួនគត់ cached = -1;
        ប្រសិនបើ (cached >= 0)
                ត្រឡប់ cached;

        _cleanup_close_ ចំនួនគត់ fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        ប្រសិនបើ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ត្រឡប់ (cached = false);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ត្រឡប់ (cached = true);
}
