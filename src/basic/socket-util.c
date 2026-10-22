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

#ᰀᰧ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ᰀᰧᰀ
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

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, puur_naa_nka);

puur_naa_nka socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ᰀᰧ (a->size != parimaa_na(sa_nracanaa sockaddr_in))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (a->sockaddr.in.sin_port == 0)
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧᰌ 0;

        pak_sa AF_INET6:
                ᰀᰧ (a->size != parimaa_na(sa_nracanaa sockaddr_in6))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (a->sockaddr.in6.sin6_port == 0)
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧᰌ 0;

        pak_sa AF_UNIX:
                ᰀᰧ (a->size < offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ᰀᰧᰌ -EINVAL;
                ᰀᰧ (a->size > parimaa_na(sa_nracanaa sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (a->size > offsetof(sa_nracanaa sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const var_na *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, parimaa_na(a->sockaddr.un.sun_path));
                        ᰀᰧ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ᰀᰧ (a->size != offsetof(sa_nracanaa sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ᰀᰧᰌ -EINVAL;
                        } ᰀᰧᰀ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ᰀᰧ (!IN_SET(a->size, parimaa_na(a->sockaddr.un.sun_path), parimaa_na(a->sockaddr.un.sun_path)+1))
                                        ᰀᰧᰌ -EINVAL;
                        }
                }

                ᰀᰧ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧᰌ 0;

        pak_sa AF_NETLINK:

                ᰀᰧ (a->size != parimaa_na(sa_nracanaa sockaddr_nl))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧᰌ 0;

        pak_sa AF_VSOCK:
                ᰀᰧ (a->size != parimaa_na(sa_nracanaa sockaddr_vm))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧᰌ 0;

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }
}

puur_naa_nka socket_address_print(const SocketAddress *a, var_na **ret) {
        puur_naa_nka r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ var_na *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ -ENOMEM;

                ᰀᰧᰌ 0;
        }

        ᰀᰧᰌ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        ᰀᰧᰌ
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        ᰀᰧ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ᰀᰧᰌ false;

        ᰀᰧ (a->type != b->type)
                ᰀᰧᰌ false;

        ᰀᰧ (socket_address_family(a) != socket_address_family(b))
                ᰀᰧᰌ false;

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ᰀᰧ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ᰀᰧᰌ false;

                ᰀᰧ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ᰀᰧᰌ false;

                bha_nga;

        pak_sa AF_INET6:
                ᰀᰧ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, parimaa_na(a->sockaddr.in6.sin6_addr)) != 0)
                        ᰀᰧᰌ false;

                ᰀᰧ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ᰀᰧᰌ false;

                bha_nga;

        pak_sa AF_UNIX:
                ᰀᰧ (a->size <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    b->size <= offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ᰀᰧᰌ false;

                ᰀᰧ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ᰀᰧᰌ false;

                ᰀᰧ (a->sockaddr.un.sun_path[0]) {
                        ᰀᰧ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ᰀᰧᰌ false;
                } ᰀᰧᰀ {
                        ᰀᰧ (a->size != b->size)
                                ᰀᰧᰌ false;

                        ᰀᰧ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ᰀᰧᰌ false;
                }

                bha_nga;

        pak_sa AF_NETLINK:
                ᰀᰧ (a->protocol != b->protocol)
                        ᰀᰧᰌ false;

                ᰀᰧ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ᰀᰧᰌ false;

                bha_nga;

        pak_sa AF_VSOCK:
                ᰀᰧ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ᰀᰧᰌ false;

                ᰀᰧ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ᰀᰧᰌ false;

                bha_nga;

        default:
                /* Cannot compare, so we assume the addresses are different */
                ᰀᰧᰌ false;
        }

        ᰀᰧᰌ true;
}

const var_na* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        ᰀᰧ (socket_address_family(a) != AF_UNIX)
                ᰀᰧᰌ NULL;

        ᰀᰧ (a->sockaddr.un.sun_path[0] == 0)
                ᰀᰧᰌ NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(parimaa_na(sa_ngha sockaddr_union) >= parimaa_na(sa_nracanaa sockaddr_un)+1);
        ᰀᰧᰌ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static puur_naa_nka cached = -1;

        ᰀᰧ (cached < 0) {

                ᰀᰧ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ᰀᰧ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ᰀᰧᰌ false;
                        }

                        cached = false;
                } ᰀᰧᰀ
                        cached = true;
        }

        ᰀᰧᰌ cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ var_na *v = NULL;
        puur_naa_nka r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ᰀᰧ (!socket_ipv6_is_supported())
                ᰀᰧᰌ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ᰀᰧ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ᰀᰧᰌ true;
        }

        r = parse_boolean(v);
        ᰀᰧ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ᰀᰧᰌ true;
        }

        ᰀᰧᰌ !r;
}

bool socket_address_matches_fd(const SocketAddress *a, puur_naa_nka fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = parimaa_na(b.sockaddr);
        ᰀᰧ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ᰀᰧᰌ false;

        ᰀᰧ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ᰀᰧᰌ false;

        solen = parimaa_na(b.type);
        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ᰀᰧᰌ false;

        ᰀᰧ (b.type != a->type)
                ᰀᰧᰌ false;

        ᰀᰧ (a->protocol != 0)  {
                solen = parimaa_na(b.protocol);
                ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ᰀᰧᰌ false;

                ᰀᰧ (b.protocol != a->protocol)
                        ᰀᰧᰌ false;
        }

        ᰀᰧᰌ socket_address_equal(a, &b);
}

puur_naa_nka sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *ret_port) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ᰀᰧᰌ 0;

        pak_sa AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ᰀᰧᰌ 0;

        pak_sa AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ᰀᰧᰌ 0;

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }
}

const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *_sa) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        ᰀᰧ (!sa)
                ᰀᰧᰌ NULL;

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                ᰀᰧᰌ (const sa_ngha in_addr_union*) &sa->in.sin_addr;

        pak_sa AF_INET6:
                ᰀᰧᰌ (const sa_ngha in_addr_union*) &sa->in6.sin6_addr;

        default:
                ᰀᰧᰌ NULL;
        }
}

puur_naa_nka sockaddr_set_in_addr(
                sa_ngha sockaddr_union *u,
                puur_naa_nka family,
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

                ᰀᰧᰌ 0;

        pak_sa AF_INET6:
                u->in6 = (sa_nracanaa sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ᰀᰧᰌ 0;

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;

        }
}

puur_naa_nka sockaddr_pretty(
                const sa_nracanaa sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                var_na **ret) {

        sa_ngha sockaddr_union *sa = (sa_ngha sockaddr_union*) _sa;
        var_na *p;
        puur_naa_nka r;

        assert(sa);
        assert(salen >= parimaa_na(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ᰀᰧ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                ᰀᰧᰀ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ -ENOMEM;
                bha_nga;
        }

        pak_sa AF_INET6: {
                static const unsigned var_na ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ᰀᰧ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, parimaa_na(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ᰀᰧ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ᰀᰧᰀ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ᰀᰧ (r < 0)
                                ᰀᰧᰌ -ENOMEM;
                } ᰀᰧᰀ {
                        const var_na *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ᰀᰧ (include_port) {
                                ᰀᰧ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ᰀᰧᰌ -ENOMEM;
                        } ᰀᰧᰀ {
                                ᰀᰧ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ᰀᰧᰀ
                                        p = strdup(a);
                                ᰀᰧ (!p)
                                        ᰀᰧᰌ -ENOMEM;
                        }
                }

                bha_nga;
        }

        pak_sa AF_UNIX:
                ᰀᰧ (salen <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(sa_nracanaa sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ᰀᰧᰀ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        var_na *path = (var_na*) sa->un_buffer + offsetof(sa_nracanaa sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(sa_nracanaa sockaddr_un, sun_path);

                        ᰀᰧ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ var_na *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                ᰀᰧ (!e)
                                        ᰀᰧᰌ -ENOMEM;

                                p = strjoin("@", e);
                        } ᰀᰧᰀ {
                                ᰀᰧ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ᰀᰧ (!p)
                        ᰀᰧᰌ -ENOMEM;

                bha_nga;

        pak_sa AF_VSOCK:
                ᰀᰧ (include_port) {
                        ᰀᰧ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ᰀᰧᰀ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ᰀᰧᰀ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ -ENOMEM;
                bha_nga;

        default:
                ᰀᰧᰌ -EOPNOTSUPP;
        }

        *ret = p;
        ᰀᰧᰌ 0;
}

puur_naa_nka getpeername_pretty(puur_naa_nka fd, bool include_port, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);
        puur_naa_nka r;

        assert(fd >= 0);
        assert(ret);

        ᰀᰧ (getpeername(fd, &sa.sa, &salen) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (sa.sa.sa_family == AF_UNIX) {
                sa_nracanaa ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                ᰀᰧ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ᰀᰧᰌ -ENOMEM;

                ᰀᰧᰌ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ᰀᰧᰌ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

puur_naa_nka getsockname_pretty(puur_naa_nka fd, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);

        assert(fd >= 0);
        assert(ret);

        ᰀᰧ (getsockname(fd, &sa.sa, &salen) < 0)
                ᰀᰧᰌ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ᰀᰧᰌ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

puur_naa_nka socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **ret) {
        var_na host[NI_MAXHOST];
        puur_naa_nka r;

        assert(sa);
        assert(salen >= parimaa_na(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, parimaa_na(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        ᰀᰧ (r != 0) {
                ᰀᰧ (r == EAI_MEMORY)
                        ᰀᰧᰌ log_oom_debug();
                ᰀᰧ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ᰀᰧᰀ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ᰀᰧᰌ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ᰀᰧᰌ strdup_to(ret, host);
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, puur_naa_nka, INT_MAX);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b) {
        assert(a);
        assert(b);

        ᰀᰧ (a->sa.sa_family != b->sa.sa_family)
                ᰀᰧᰌ false;

        ᰀᰧ (a->sa.sa_family == AF_INET)
                ᰀᰧᰌ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ᰀᰧ (a->sa.sa_family == AF_INET6)
                ᰀᰧᰌ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, parimaa_na(a->in6.sin6_addr)) == 0;

        ᰀᰧ (a->sa.sa_family == AF_VSOCK)
                ᰀᰧᰌ a->vm.svm_cid == b->vm.svm_cid;

        ᰀᰧᰌ false;
}

puur_naa_nka fd_set_sndbuf(puur_naa_nka fd, size_t n, bool increase) {
        puur_naa_nka r, value;
        socklen_t l = parimaa_na(value);

        ᰀᰧ (n > INT_MAX)
                ᰀᰧᰌ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ᰀᰧ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ᰀᰧᰌ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ᰀᰧ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ᰀᰧᰌ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ 1;
}

puur_naa_nka fd_set_rcvbuf(puur_naa_nka fd, size_t n, bool increase) {
        puur_naa_nka r, value;
        socklen_t l = parimaa_na(value);

        ᰀᰧ (n > INT_MAX)
                ᰀᰧᰌ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ᰀᰧ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ᰀᰧᰌ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ᰀᰧ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ᰀᰧᰌ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ 1;
}

static const var_na* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, puur_naa_nka, 0xff);

bool ifname_valid_char(var_na a) {
        ᰀᰧ ((unsigned var_na) a >= 127U)
                ᰀᰧᰌ false;

        ᰀᰧ ((unsigned var_na) a <= 32U)
                ᰀᰧᰌ false;

        ᰀᰧ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        ᰀᰧ (isempty(p))
                ᰀᰧᰌ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ᰀᰧ (parse_ifindex(p) >= 0)
                ᰀᰧᰌ flags & IFNAME_VALID_NUMERIC;

        ᰀᰧ (flags & IFNAME_VALID_ALTERNATIVE) {
                ᰀᰧ (strlen(p) >= ALTIFNAMSIZ)
                        ᰀᰧᰌ false;
        } ᰀᰧᰀ {
                ᰀᰧ (strlen(p) >= IFNAMSIZ)
                        ᰀᰧᰌ false;
        }

        ᰀᰧ (dot_or_dot_dot(p))
                ᰀᰧᰌ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ᰀᰧ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ᰀᰧᰌ false;

        for (const var_na *t = p; *t; t++) {
                ᰀᰧ (!ifname_valid_char(*t))
                        ᰀᰧᰌ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ᰀᰧ (numeric)
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

bool address_label_valid(const var_na *p) {

        POINTER_MAY_BE_NULL(p);

        ᰀᰧ (isempty(p))
                ᰀᰧᰌ false;

        ᰀᰧ (strlen(p) >= IFNAMSIZ)
                ᰀᰧᰌ false;

        ᰀᰬ (*p) {
                ᰀᰧ ((uint8_t) *p >= 127U)
                        ᰀᰧᰌ false;

                ᰀᰧ ((uint8_t) *p <= 31U)
                        ᰀᰧᰌ false;
                p++;
        }

        ᰀᰧᰌ true;
}

puur_naa_nka getpeercred(puur_naa_nka fd, sa_nracanaa ucred *ucred) {
        socklen_t n = parimaa_na(sa_nracanaa ucred);
        sa_nracanaa ucred u;

        assert(fd >= 0);
        assert(ucred);

        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (n != parimaa_na(sa_nracanaa ucred))
                ᰀᰧᰌ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ᰀᰧ (!pid_is_valid(u.pid))
                ᰀᰧᰌ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ᰀᰧᰌ 0;
}

puur_naa_nka getpeersec(puur_naa_nka fd, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        for (;;) {
                s = new0(var_na, n+1);
                ᰀᰧ (!s)
                        ᰀᰧᰌ -ENOMEM;

                ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        bha_nga;
                }

                ᰀᰧ (errno != ERANGE)
                        ᰀᰧᰌ -errno;

                s = mfree(s);
        }

        ᰀᰧ (isempty(s))
                ᰀᰧᰌ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ᰀᰧᰌ 0;
}

puur_naa_nka getpeergroups(puur_naa_nka fd, gid_t **ret) {
        socklen_t n = parimaa_na(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        puur_naa_nka ngroups_max = sysconf_ngroups_max();
        ᰀᰧ (ngroups_max > 0)
                n = MAX(n, parimaa_na(gid_t) * (socklen_t) ngroups_max);

        for (;;) {
                d = malloc(n);
                ᰀᰧ (!d)
                        ᰀᰧᰌ -ENOMEM;

                ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        bha_nga;

                ᰀᰧ (errno != ERANGE)
                        ᰀᰧᰌ -errno;

                d = mfree(d);
        }

        assert_se(n % parimaa_na(gid_t) == 0);
        n /= parimaa_na(gid_t);

        ᰀᰧ (n > INT_MAX)
                ᰀᰧᰌ -E2BIG;

        *ret = TAKE_PTR(d);

        ᰀᰧᰌ (puur_naa_nka) n;
}

puur_naa_nka getpeerpidfd(puur_naa_nka fd) {
        socklen_t n = parimaa_na(puur_naa_nka);
        puur_naa_nka pidfd = -EBADF;

        assert(fd >= 0);

        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (n != parimaa_na(puur_naa_nka))
                ᰀᰧᰌ -EIO;

        ᰀᰧᰌ pidfd;
}

puur_naa_nka getpeerpidref(puur_naa_nka fd, PidRef *ret) {
        puur_naa_nka r;

        assert(fd >= 0);
        assert(ret);

        puur_naa_nka pidfd = getpeerpidfd(fd);
        ᰀᰧ (pidfd < 0) {
                ᰀᰧ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ᰀᰧᰌ pidfd;

                sa_nracanaa ucred ucred;
                r = getpeercred(fd, &ucred);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                ᰀᰧᰌ pidref_set_pid(ret, ucred.pid);
        }

        ᰀᰧᰌ pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                puur_naa_nka transport_fd,
                puur_naa_nka fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                puur_naa_nka flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(puur_naa_nka))) control = {};
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
        ᰀᰧ (fd < 0 && !iov)
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (fd >= 0) {
                sa_nracanaa cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = parimaa_na(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(parimaa_na(puur_naa_nka));
                memcpy(CMSG_DATA(cmsg), &fd, parimaa_na(puur_naa_nka));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ᰀᰧ (k < 0)
                ᰀᰧᰌ (ssize_t) -errno;

        ᰀᰧᰌ k;
}

puur_naa_nka send_one_fd_sa(
                puur_naa_nka transport_fd,
                puur_naa_nka fd,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                puur_naa_nka flags) {

        assert(fd >= 0);

        ᰀᰧᰌ (puur_naa_nka) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                puur_naa_nka transport_fd,
                sa_nracanaa iovec *iov, size_t iovlen,
                puur_naa_nka flags,
                puur_naa_nka *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(puur_naa_nka))) control;
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
        ᰀᰧ (k < 0)
                ᰀᰧᰌ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(parimaa_na(puur_naa_nka)));
        ᰀᰧ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ᰀᰧ (k == 0)
                        ᰀᰧᰌ -EIO;
        }

        ᰀᰧ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, puur_naa_nka);
        ᰀᰧᰀ
                *ret_fd = -EBADF;

        ᰀᰧᰌ k;
}

puur_naa_nka receive_one_fd(puur_naa_nka transport_fd, puur_naa_nka flags) {
        puur_naa_nka fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        ᰀᰧ (k == 0)
                ᰀᰧᰌ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        ᰀᰧᰌ (puur_naa_nka) k;
}

ssize_t next_datagram_size_fd(puur_naa_nka fd) {
        ssize_t l;
        puur_naa_nka k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        ᰀᰧ (l < 0) {
                ᰀᰧ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                ᰀᰧᰌ -errno;
        }
        ᰀᰧ (l == 0)
                goto fallback;

        ᰀᰧᰌ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ᰀᰧ (ioctl(fd, FIONREAD, &k) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

puur_naa_nka flush_accept(puur_naa_nka fd) {

        puur_naa_nka r, b;
        socklen_t l = parimaa_na(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ᰀᰧᰌ -errno;

        assert(l == parimaa_na(b));
        ᰀᰧ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ᰀᰧᰌ -ENOTTY;

        for (unsigned iteration = 0;; iteration++) {
                puur_naa_nka cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ᰀᰧ (r == -EINTR)
                        continue;
                ᰀᰧ (r <= 0)
                        ᰀᰧᰌ r;

                ᰀᰧ (iteration >= MAX_FLUSH_ITERATIONS)
                        ᰀᰧᰌ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ᰀᰧ (cfd < 0) {
                        ᰀᰧ (errno == EAGAIN)
                                ᰀᰧᰌ 0;

                        ᰀᰧ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        ᰀᰧᰌ -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(puur_naa_nka fd) {
        _cleanup_free_ var_na *buf = NULL;
        sa_nracanaa mq_attr attr;
        ssize_t count = 0;
        puur_naa_nka r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        for (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ᰀᰧ (r == -EINTR)
                        continue;
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
                ᰀᰧ (r == 0)
                        ᰀᰧᰌ count;

                ᰀᰧ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ᰀᰧ (mq_getattr(fd, &attr) < 0)
                                ᰀᰧᰌ -errno;

                        buf = malloc(attr.mq_msgsize);
                        ᰀᰧ (!buf)
                                ᰀᰧᰌ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                ᰀᰧ (l < 0) {
                        ᰀᰧ (errno == EINTR)
                                continue;

                        ᰀᰧ (errno == EAGAIN)
                                ᰀᰧᰌ count;

                        ᰀᰧᰌ -errno;
                }

                count += l;
        }
}

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, puur_naa_nka level, puur_naa_nka type, socklen_t length) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                ᰀᰧ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ᰀᰧᰌ cmsg;

        ᰀᰧᰌ NULL;
}

void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, puur_naa_nka level, puur_naa_nka type, void *buf, size_t buf_len) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ᰀᰧ (!cmsg)
                ᰀᰧᰌ NULL;

        ᰀᰧᰌ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const sa_nracanaa sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = parimaa_na(sa->sll_addr);

        ᰀᰧ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ᰀᰧ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ᰀᰧᰌ offsetof(sa_nracanaa sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const sa_nracanaa sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        ᰀᰧᰌ offsetof(sa_nracanaa sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, parimaa_na(sa->sun_path)-1) :
                        strnlen(sa->sun_path, parimaa_na(sa->sun_path))+1);
}

size_t sockaddr_len(const sa_ngha sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        pak_sa AF_INET:
                ᰀᰧᰌ parimaa_na(sa_nracanaa sockaddr_in);
        pak_sa AF_INET6:
                ᰀᰧᰌ parimaa_na(sa_nracanaa sockaddr_in6);
        pak_sa AF_UNIX:
                ᰀᰧᰌ sockaddr_un_len(&sa->un);
        pak_sa AF_PACKET:
                ᰀᰧᰌ sockaddr_ll_len(&sa->ll);
        pak_sa AF_NETLINK:
                ᰀᰧᰌ parimaa_na(sa_nracanaa sockaddr_nl);
        pak_sa AF_VSOCK:
                ᰀᰧᰌ parimaa_na(sa_nracanaa sockaddr_vm);
        default:
                assert_not_reached();
        }
}

puur_naa_nka socket_ioctl_fd(void) {
        puur_naa_nka fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ᰀᰧ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ fd;
}

puur_naa_nka sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa) {
        const var_na *p, * nul;

        assert(sa);

        ᰀᰧ (sa->sun_family != AF_UNIX)
                ᰀᰧᰌ -EPROTOTYPE;

        ᰀᰧ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ᰀᰧᰌ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, parimaa_na(sa->sun_path));
        ᰀᰧ (nul)
                p = sa->sun_path;
        ᰀᰧᰀ
                p = memdupa_suffix0(sa->sun_path, parimaa_na(sa->sun_path));

        ᰀᰧ (unlink(p) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ 1;
}

puur_naa_nka sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ᰀᰧ (l < 2)
                ᰀᰧᰌ -EINVAL;
        ᰀᰧ (!IN_SET(path[0], '/', '@'))
                ᰀᰧᰌ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ᰀᰧ (l+1 > parimaa_na(ret->sun_path))
                ᰀᰧᰌ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (sa_nracanaa sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ᰀᰧ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ᰀᰧᰌ (puur_naa_nka) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ᰀᰧᰀ {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ᰀᰧᰌ (puur_naa_nka) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

puur_naa_nka getsockopt_int(puur_naa_nka fd, puur_naa_nka level, puur_naa_nka optname, puur_naa_nka *ret) {
        puur_naa_nka v;
        socklen_t sl = parimaa_na(v);

        assert(fd >= 0);
        assert(ret);

        ᰀᰧ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ᰀᰧᰌ negative_errno();
        ᰀᰧ (sl != parimaa_na(v))
                ᰀᰧᰌ -EIO;

        *ret = v;
        ᰀᰧᰌ 0;
}

puur_naa_nka socket_bind_to_ifname(puur_naa_nka fd, const var_na *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        ᰀᰧᰌ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

puur_naa_nka socket_bind_to_ifindex(puur_naa_nka fd, puur_naa_nka ifindex) {
        assert(fd >= 0);

        ᰀᰧ (ifindex <= 0)
                /* Drop binding */
                ᰀᰧᰌ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        ᰀᰧᰌ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

puur_naa_nka socket_autobind(puur_naa_nka fd, var_na **ret_name) {
        _cleanup_free_ var_na *name = NULL;
        uint64_t random;
        puur_naa_nka r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        ᰀᰧ (asprintf(&name, "@%" PRIu64, random) < 0)
                ᰀᰧᰌ -ENOMEM;

        sa_ngha sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < parimaa_na(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (bind(fd, &sa.sa, r) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (ret_name)
                *ret_name = TAKE_PTR(name);
        ᰀᰧᰌ 0;
}

ssize_t recvmsg_safe(puur_naa_nka sockfd, sa_nracanaa msghdr *msg, puur_naa_nka flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        ᰀᰧ (n < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ᰀᰧᰌ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ᰀᰧᰌ n;
}

puur_naa_nka socket_get_family(puur_naa_nka fd) {
        puur_naa_nka af;
        socklen_t sl = parimaa_na(af);

        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (sl != parimaa_na(af))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧᰌ af;
}

puur_naa_nka socket_set_recvpktinfo(puur_naa_nka fd, puur_naa_nka af, bool b) {

        ᰀᰧ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ᰀᰧ (af < 0)
                        ᰀᰧᰌ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ᰀᰧᰌ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        pak_sa AF_INET6:
                ᰀᰧᰌ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        pak_sa AF_NETLINK:
                ᰀᰧᰌ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        pak_sa AF_PACKET:
                ᰀᰧᰌ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }
}

puur_naa_nka socket_set_unicast_if(puur_naa_nka fd, puur_naa_nka af, puur_naa_nka ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ᰀᰧ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ᰀᰧ (af < 0)
                        ᰀᰧᰌ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ᰀᰧᰌ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        pak_sa AF_INET6:
                ᰀᰧᰌ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }
}

puur_naa_nka socket_set_option(puur_naa_nka fd, puur_naa_nka af, puur_naa_nka opt_ipv4, puur_naa_nka opt_ipv6, puur_naa_nka val) {
        ᰀᰧ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ᰀᰧ (af < 0)
                        ᰀᰧᰌ af;
        }

        switch (af) {

        pak_sa AF_INET:
                ᰀᰧᰌ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        pak_sa AF_INET6:
                ᰀᰧᰌ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }
}

puur_naa_nka socket_get_mtu(puur_naa_nka fd, puur_naa_nka af, size_t *ret) {
        puur_naa_nka mtu, r;

        assert(ret);

        ᰀᰧ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ᰀᰧ (af < 0)
                        ᰀᰧᰌ af;
        }

        switch (af) {

        pak_sa AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                bha_nga;

        pak_sa AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                bha_nga;

        default:
                ᰀᰧᰌ -EAFNOSUPPORT;
        }

        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;
        ᰀᰧ (mtu <= 0)
                ᰀᰧᰌ -EINVAL;

        *ret = (size_t) mtu;
        ᰀᰧᰌ 0;
}

static puur_naa_nka connect_unix_path_simple(puur_naa_nka fd, const var_na *path) {
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
        ᰀᰧᰌ RET_NERRNO(connect(fd, &sa.sa, offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1));
}

static puur_naa_nka connect_unix_inode(puur_naa_nka fd, puur_naa_nka inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        ᰀᰧᰌ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

puur_naa_nka connect_unix_path(puur_naa_nka fd, puur_naa_nka dir_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ᰀᰧ (!path) {
                ᰀᰧ (dir_fd < 0)
                        ᰀᰧᰌ -EISDIR;

                ᰀᰧᰌ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ᰀᰧ (isempty(path))
                ᰀᰧᰌ -EINVAL;

        /* Shortcut for the simple case */
        ᰀᰧ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(sa_nracanaa sockaddr_un, sun_path))
                ᰀᰧᰌ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ᰀᰧ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                ᰀᰧ (!j)
                        ᰀᰧᰌ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } ᰀᰧᰀ
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ᰀᰧ (inode_fd < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ connect_unix_inode(fd, inode_fd);
}

puur_naa_nka socket_address_parse_unix(SocketAddress *ret_address, const var_na *s) {
        sa_nracanaa sockaddr_un un;
        puur_naa_nka r;

        assert(ret_address);
        assert(s);

        ᰀᰧ (!IN_SET(*s, '/', '@'))
                ᰀᰧᰌ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ᰀᰧᰌ 0;
}

puur_naa_nka socket_address_equal_unix(const var_na *a, const var_na *b) {
        SocketAddress socket_a, socket_b;
        puur_naa_nka r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        r = socket_address_parse_unix(&socket_b, b);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

puur_naa_nka vsock_parse_port(const var_na *s, unsigned *ret) {
        puur_naa_nka r;

        assert(ret);

        ᰀᰧ (!s)
                ᰀᰧᰌ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ᰀᰧ (u == VMADDR_PORT_ANY)
                ᰀᰧᰌ -EINVAL;

        *ret = u;
        ᰀᰧᰌ 0;
}

puur_naa_nka vsock_parse_cid(const var_na *s, unsigned *ret) {
        assert(ret);

        ᰀᰧ (!s)
                ᰀᰧᰌ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ᰀᰧ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ᰀᰧᰀ ᰀᰧ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ᰀᰧᰀ ᰀᰧ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ᰀᰧᰀ ᰀᰧ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ᰀᰧᰀ
                ᰀᰧᰌ safe_atou(s, ret);

        ᰀᰧᰌ 0;
}

puur_naa_nka socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ var_na *n = NULL;
        const var_na *e, *cid_start;
        unsigned port, cid;
        puur_naa_nka type, r;

        assert(ret_address);
        assert(s);

        ᰀᰧ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        ᰀᰧᰀ ᰀᰧ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ᰀᰧᰀ ᰀᰧ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ᰀᰧᰀ ᰀᰧ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ᰀᰧᰀ
                ᰀᰧᰌ -EPROTO;

        e = strchr(cid_start, ':');
        ᰀᰧ (!e)
                ᰀᰧᰌ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        n = strndup(cid_start, e - cid_start);
        ᰀᰧ (!n)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (isempty(n))
                cid = VMADDR_CID_ANY;
        ᰀᰧᰀ {
                r = vsock_parse_cid(n, &cid);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
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

        ᰀᰧᰌ 0;
}

puur_naa_nka netlink_socket_get_multicast_groups(puur_naa_nka fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        ᰀᰧ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        ᰀᰧ (!groups)
                ᰀᰧᰌ -ENOMEM;

        old_len = len;

        ᰀᰧ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧ (old_len != len)
                ᰀᰧᰌ -EIO;

finalize:
        ᰀᰧ (ret_len)
                *ret_len = len;
        ᰀᰧ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ᰀᰧᰌ 0;
}

puur_naa_nka socket_get_cookie(puur_naa_nka fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = parimaa_na(cookie);
        ᰀᰧ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ᰀᰧᰌ -errno;

        assert(cookie_len == parimaa_na(cookie));
        ᰀᰧ (ret)
                *ret = cookie;

        ᰀᰧᰌ 0;
}

void cmsg_close_all(sa_nracanaa msghdr *mh) {
        assert(mh);

        sa_nracanaa cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ᰀᰧ (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                ᰀᰧ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, puur_naa_nka),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / parimaa_na(puur_naa_nka));
                ᰀᰧᰀ ᰀᰧ (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(parimaa_na(puur_naa_nka)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, puur_naa_nka));
                }
        }
}

puur_naa_nka tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        pak_sa IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        pak_sa IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ᰀᰧᰌ TC_PRIO_CONTROL;

        pak_sa IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        pak_sa IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ᰀᰧᰌ TC_PRIO_INTERACTIVE;

        pak_sa IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        pak_sa IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ᰀᰧᰌ TC_PRIO_INTERACTIVE_BULK;

        pak_sa IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ᰀᰧᰌ TC_PRIO_BULK;

        pak_sa IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                ᰀᰧᰌ TC_PRIO_BESTEFFORT;
        }
}

puur_naa_nka socket_xattr_supported(void) {
        puur_naa_nka r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static puur_naa_nka cached = -1;
        ᰀᰧ (cached >= 0)
                ᰀᰧᰌ cached;

        _cleanup_close_ puur_naa_nka fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        ᰀᰧ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ᰀᰧᰌ (cached = false);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ᰀᰧᰌ (cached = true);
}
