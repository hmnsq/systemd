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

#အကယ်၍ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#မဟုတ်ပါက
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

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, ကိန်းပြည့်);

ကိန်းပြည့် socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                အကယ်၍ (a->size != parimaa_na(sa_nracanaa sockaddr_in))
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (a->sockaddr.in.sin_port == 0)
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး 0;

        pak_sa AF_INET6:
                အကယ်၍ (a->size != parimaa_na(sa_nracanaa sockaddr_in6))
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (a->sockaddr.in6.sin6_port == 0)
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး 0;

        pak_sa AF_UNIX:
                အကယ်၍ (a->size < offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ပြန်ပေး -EINVAL;
                အကယ်၍ (a->size > parimaa_na(sa_nracanaa sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (a->size > offsetof(sa_nracanaa sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const var_na *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, parimaa_na(a->sockaddr.un.sun_path));
                        အကယ်၍ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                အကယ်၍ (a->size != offsetof(sa_nracanaa sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ပြန်ပေး -EINVAL;
                        } မဟုတ်ပါက {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                အကယ်၍ (!IN_SET(a->size, parimaa_na(a->sockaddr.un.sun_path), parimaa_na(a->sockaddr.un.sun_path)+1))
                                        ပြန်ပေး -EINVAL;
                        }
                }

                အကယ်၍ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး 0;

        pak_sa AF_NETLINK:

                အကယ်၍ (a->size != parimaa_na(sa_nracanaa sockaddr_nl))
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး 0;

        pak_sa AF_VSOCK:
                အကယ်၍ (a->size != parimaa_na(sa_nracanaa sockaddr_vm))
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး 0;

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}

ကိန်းပြည့် socket_address_print(const SocketAddress *a, var_na **ret) {
        ကိန်းပြည့် r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ var_na *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                အကယ်၍ (r < 0)
                        ပြန်ပေး -ENOMEM;

                ပြန်ပေး 0;
        }

        ပြန်ပေး sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        ပြန်ပေး
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        အကယ်၍ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ပြန်ပေး false;

        အကယ်၍ (a->type != b->type)
                ပြန်ပေး false;

        အကယ်၍ (socket_address_family(a) != socket_address_family(b))
                ပြန်ပေး false;

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                အကယ်၍ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ပြန်ပေး false;

                အကယ်၍ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ပြန်ပေး false;

                bha_nga;

        pak_sa AF_INET6:
                အကယ်၍ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, parimaa_na(a->sockaddr.in6.sin6_addr)) != 0)
                        ပြန်ပေး false;

                အကယ်၍ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ပြန်ပေး false;

                bha_nga;

        pak_sa AF_UNIX:
                အကယ်၍ (a->size <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    b->size <= offsetof(sa_nracanaa sockaddr_un, sun_path))
                        ပြန်ပေး false;

                အကယ်၍ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ပြန်ပေး false;

                အကယ်၍ (a->sockaddr.un.sun_path[0]) {
                        အကယ်၍ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ပြန်ပေး false;
                } မဟုတ်ပါက {
                        အကယ်၍ (a->size != b->size)
                                ပြန်ပေး false;

                        အကယ်၍ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ပြန်ပေး false;
                }

                bha_nga;

        pak_sa AF_NETLINK:
                အကယ်၍ (a->protocol != b->protocol)
                        ပြန်ပေး false;

                အကယ်၍ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ပြန်ပေး false;

                bha_nga;

        pak_sa AF_VSOCK:
                အကယ်၍ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ပြန်ပေး false;

                အကယ်၍ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ပြန်ပေး false;

                bha_nga;

        default:
                /* Cannot compare, so we assume the addresses are different */
                ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

const var_na* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        အကယ်၍ (socket_address_family(a) != AF_UNIX)
                ပြန်ပေး NULL;

        အကယ်၍ (a->sockaddr.un.sun_path[0] == 0)
                ပြန်ပေး NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(parimaa_na(sa_ngha sockaddr_union) >= parimaa_na(sa_nracanaa sockaddr_un)+1);
        ပြန်ပေး a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static ကိန်းပြည့် cached = -1;

        အကယ်၍ (cached < 0) {

                အကယ်၍ (access("/proc/net/if_inet6", F_OK) < 0) {

                        အကယ်၍ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ပြန်ပေး false;
                        }

                        cached = false;
                } မဟုတ်ပါက
                        cached = true;
        }

        ပြန်ပေး cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ var_na *v = NULL;
        ကိန်းပြည့် r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        အကယ်၍ (!socket_ipv6_is_supported())
                ပြန်ပေး false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        အကယ်၍ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ပြန်ပေး true;
        }

        r = parse_boolean(v);
        အကယ်၍ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ပြန်ပေး true;
        }

        ပြန်ပေး !r;
}

bool socket_address_matches_fd(const SocketAddress *a, ကိန်းပြည့် fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = parimaa_na(b.sockaddr);
        အကယ်၍ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ပြန်ပေး false;

        အကယ်၍ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ပြန်ပေး false;

        solen = parimaa_na(b.type);
        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ပြန်ပေး false;

        အကယ်၍ (b.type != a->type)
                ပြန်ပေး false;

        အကယ်၍ (a->protocol != 0)  {
                solen = parimaa_na(b.protocol);
                အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ပြန်ပေး false;

                အကယ်၍ (b.protocol != a->protocol)
                        ပြန်ပေး false;
        }

        ပြန်ပေး socket_address_equal(a, &b);
}

ကိန်းပြည့် sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *ret_port) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ပြန်ပေး 0;

        pak_sa AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ပြန်ပေး 0;

        pak_sa AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ပြန်ပေး 0;

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}

const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *_sa) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        အကယ်၍ (!sa)
                ပြန်ပေး NULL;

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                ပြန်ပေး (const sa_ngha in_addr_union*) &sa->in.sin_addr;

        pak_sa AF_INET6:
                ပြန်ပေး (const sa_ngha in_addr_union*) &sa->in6.sin6_addr;

        default:
                ပြန်ပေး NULL;
        }
}

ကိန်းပြည့် sockaddr_set_in_addr(
                sa_ngha sockaddr_union *u,
                ကိန်းပြည့် family,
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

                ပြန်ပေး 0;

        pak_sa AF_INET6:
                u->in6 = (sa_nracanaa sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ပြန်ပေး 0;

        default:
                ပြန်ပေး -EAFNOSUPPORT;

        }
}

ကိန်းပြည့် sockaddr_pretty(
                const sa_nracanaa sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                var_na **ret) {

        sa_ngha sockaddr_union *sa = (sa_ngha sockaddr_union*) _sa;
        var_na *p;
        ကိန်းပြည့် r;

        assert(sa);
        assert(salen >= parimaa_na(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                အကယ်၍ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                မဟုတ်ပါက
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                အကယ်၍ (r < 0)
                        ပြန်ပေး -ENOMEM;
                bha_nga;
        }

        pak_sa AF_INET6: {
                static const unsigned var_na ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                အကယ်၍ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, parimaa_na(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        အကယ်၍ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        မဟုတ်ပါက
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး -ENOMEM;
                } မဟုတ်ပါက {
                        const var_na *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        အကယ်၍ (include_port) {
                                အကယ်၍ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ပြန်ပေး -ENOMEM;
                        } မဟုတ်ပါက {
                                အကယ်၍ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                မဟုတ်ပါက
                                        p = strdup(a);
                                အကယ်၍ (!p)
                                        ပြန်ပေး -ENOMEM;
                        }
                }

                bha_nga;
        }

        pak_sa AF_UNIX:
                အကယ်၍ (salen <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(sa_nracanaa sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                မဟုတ်ပါက {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        var_na *path = (var_na*) sa->un_buffer + offsetof(sa_nracanaa sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(sa_nracanaa sockaddr_un, sun_path);

                        အကယ်၍ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ var_na *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                အကယ်၍ (!e)
                                        ပြန်ပေး -ENOMEM;

                                p = strjoin("@", e);
                        } မဟုတ်ပါက {
                                အကယ်၍ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                အကယ်၍ (!p)
                        ပြန်ပေး -ENOMEM;

                bha_nga;

        pak_sa AF_VSOCK:
                အကယ်၍ (include_port) {
                        အကယ်၍ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        မဟုတ်ပါက
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } မဟုတ်ပါက
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                အကယ်၍ (r < 0)
                        ပြန်ပေး -ENOMEM;
                bha_nga;

        default:
                ပြန်ပေး -EOPNOTSUPP;
        }

        *ret = p;
        ပြန်ပေး 0;
}

ကိန်းပြည့် getpeername_pretty(ကိန်းပြည့် fd, bool include_port, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);
        ကိန်းပြည့် r;

        assert(fd >= 0);
        assert(ret);

        အကယ်၍ (getpeername(fd, &sa.sa, &salen) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (sa.sa.sa_family == AF_UNIX) {
                sa_nracanaa ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ပြန်ပေး -ENOMEM;

                ပြန်ပေး 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ပြန်ပေး sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

ကိန်းပြည့် getsockname_pretty(ကိန်းပြည့် fd, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);

        assert(fd >= 0);
        assert(ret);

        အကယ်၍ (getsockname(fd, &sa.sa, &salen) < 0)
                ပြန်ပေး -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ပြန်ပေး sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ကိန်းပြည့် socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **ret) {
        var_na host[NI_MAXHOST];
        ကိန်းပြည့် r;

        assert(sa);
        assert(salen >= parimaa_na(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, parimaa_na(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        အကယ်၍ (r != 0) {
                အကယ်၍ (r == EAI_MEMORY)
                        ပြန်ပေး log_oom_debug();
                အကယ်၍ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                မဟုတ်ပါက
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ပြန်ပေး sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ပြန်ပေး strdup_to(ret, host);
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ကိန်းပြည့်, INT_MAX);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b) {
        assert(a);
        assert(b);

        အကယ်၍ (a->sa.sa_family != b->sa.sa_family)
                ပြန်ပေး false;

        အကယ်၍ (a->sa.sa_family == AF_INET)
                ပြန်ပေး a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        အကယ်၍ (a->sa.sa_family == AF_INET6)
                ပြန်ပေး memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, parimaa_na(a->in6.sin6_addr)) == 0;

        အကယ်၍ (a->sa.sa_family == AF_VSOCK)
                ပြန်ပေး a->vm.svm_cid == b->vm.svm_cid;

        ပြန်ပေး false;
}

ကိန်းပြည့် fd_set_sndbuf(ကိန်းပြည့် fd, size_t n, bool increase) {
        ကိန်းပြည့် r, value;
        socklen_t l = parimaa_na(value);

        အကယ်၍ (n > INT_MAX)
                ပြန်ပေး -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        အကယ်၍ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ပြန်ပေး 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        အကယ်၍ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ပြန်ပေး 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1;
}

ကိန်းပြည့် fd_set_rcvbuf(ကိန်းပြည့် fd, size_t n, bool increase) {
        ကိန်းပြည့် r, value;
        socklen_t l = parimaa_na(value);

        အကယ်၍ (n > INT_MAX)
                ပြန်ပေး -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        အကယ်၍ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ပြန်ပေး 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        အကယ်၍ (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ပြန်ပေး 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1;
}

static const var_na* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ကိန်းပြည့်, 0xff);

bool ifname_valid_char(var_na a) {
        အကယ်၍ ((unsigned var_na) a >= 127U)
                ပြန်ပေး false;

        အကယ်၍ ((unsigned var_na) a <= 32U)
                ပြန်ပေး false;

        အကယ်၍ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        အကယ်၍ (parse_ifindex(p) >= 0)
                ပြန်ပေး flags & IFNAME_VALID_NUMERIC;

        အကယ်၍ (flags & IFNAME_VALID_ALTERNATIVE) {
                အကယ်၍ (strlen(p) >= ALTIFNAMSIZ)
                        ပြန်ပေး false;
        } မဟုတ်ပါက {
                အကယ်၍ (strlen(p) >= IFNAMSIZ)
                        ပြန်ပေး false;
        }

        အကယ်၍ (dot_or_dot_dot(p))
                ပြန်ပေး false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        အကယ်၍ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ပြန်ပေး false;

        အတွက် (const var_na *t = p; *t; t++) {
                အကယ်၍ (!ifname_valid_char(*t))
                        ပြန်ပေး false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        အကယ်၍ (numeric)
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool address_label_valid(const var_na *p) {

        POINTER_MAY_BE_NULL(p);

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        အကယ်၍ (strlen(p) >= IFNAMSIZ)
                ပြန်ပေး false;

        နေစဉ် (*p) {
                အကယ်၍ ((uint8_t) *p >= 127U)
                        ပြန်ပေး false;

                အကယ်၍ ((uint8_t) *p <= 31U)
                        ပြန်ပေး false;
                p++;
        }

        ပြန်ပေး true;
}

ကိန်းပြည့် getpeercred(ကိန်းပြည့် fd, sa_nracanaa ucred *ucred) {
        socklen_t n = parimaa_na(sa_nracanaa ucred);
        sa_nracanaa ucred u;

        assert(fd >= 0);
        assert(ucred);

        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (n != parimaa_na(sa_nracanaa ucred))
                ပြန်ပေး -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        အကယ်၍ (!pid_is_valid(u.pid))
                ပြန်ပေး -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် getpeersec(ကိန်းပြည့် fd, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        အတွက် (;;) {
                s = new0(var_na, n+1);
                အကယ်၍ (!s)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        bha_nga;
                }

                အကယ်၍ (errno != ERANGE)
                        ပြန်ပေး -errno;

                s = mfree(s);
        }

        အကယ်၍ (isempty(s))
                ပြန်ပေး -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ပြန်ပေး 0;
}

ကိန်းပြည့် getpeergroups(ကိန်းပြည့် fd, gid_t **ret) {
        socklen_t n = parimaa_na(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        ကိန်းပြည့် ngroups_max = sysconf_ngroups_max();
        အကယ်၍ (ngroups_max > 0)
                n = MAX(n, parimaa_na(gid_t) * (socklen_t) ngroups_max);

        အတွက် (;;) {
                d = malloc(n);
                အကယ်၍ (!d)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        bha_nga;

                အကယ်၍ (errno != ERANGE)
                        ပြန်ပေး -errno;

                d = mfree(d);
        }

        assert_se(n % parimaa_na(gid_t) == 0);
        n /= parimaa_na(gid_t);

        အကယ်၍ (n > INT_MAX)
                ပြန်ပေး -E2BIG;

        *ret = TAKE_PTR(d);

        ပြန်ပေး (ကိန်းပြည့်) n;
}

ကိန်းပြည့် getpeerpidfd(ကိန်းပြည့် fd) {
        socklen_t n = parimaa_na(ကိန်းပြည့်);
        ကိန်းပြည့် pidfd = -EBADF;

        assert(fd >= 0);

        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (n != parimaa_na(ကိန်းပြည့်))
                ပြန်ပေး -EIO;

        ပြန်ပေး pidfd;
}

ကိန်းပြည့် getpeerpidref(ကိန်းပြည့် fd, PidRef *ret) {
        ကိန်းပြည့် r;

        assert(fd >= 0);
        assert(ret);

        ကိန်းပြည့် pidfd = getpeerpidfd(fd);
        အကယ်၍ (pidfd < 0) {
                အကယ်၍ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ပြန်ပေး pidfd;

                sa_nracanaa ucred ucred;
                r = getpeercred(fd, &ucred);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                ပြန်ပေး pidref_set_pid(ret, ucred.pid);
        }

        ပြန်ပေး pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ကိန်းပြည့် transport_fd,
                ကိန်းပြည့် fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                ကိန်းပြည့် flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(ကိန်းပြည့်))) control = {};
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
        အကယ်၍ (fd < 0 && !iov)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (fd >= 0) {
                sa_nracanaa cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = parimaa_na(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(parimaa_na(ကိန်းပြည့်));
                memcpy(CMSG_DATA(cmsg), &fd, parimaa_na(ကိန်းပြည့်));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        အကယ်၍ (k < 0)
                ပြန်ပေး (ssize_t) -errno;

        ပြန်ပေး k;
}

ကိန်းပြည့် send_one_fd_sa(
                ကိန်းပြည့် transport_fd,
                ကိန်းပြည့် fd,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                ကိန်းပြည့် flags) {

        assert(fd >= 0);

        ပြန်ပေး (ကိန်းပြည့်) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ကိန်းပြည့် transport_fd,
                sa_nracanaa iovec *iov, size_t iovlen,
                ကိန်းပြည့် flags,
                ကိန်းပြည့် *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(ကိန်းပြည့်))) control;
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
        အကယ်၍ (k < 0)
                ပြန်ပေး k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(parimaa_na(ကိန်းပြည့်)));
        အကယ်၍ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                အကယ်၍ (k == 0)
                        ပြန်ပေး -EIO;
        }

        အကယ်၍ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, ကိန်းပြည့်);
        မဟုတ်ပါက
                *ret_fd = -EBADF;

        ပြန်ပေး k;
}

ကိန်းပြည့် receive_one_fd(ကိန်းပြည့် transport_fd, ကိန်းပြည့် flags) {
        ကိန်းပြည့် fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        အကယ်၍ (k == 0)
                ပြန်ပေး fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        ပြန်ပေး (ကိန်းပြည့်) k;
}

ssize_t next_datagram_size_fd(ကိန်းပြည့် fd) {
        ssize_t l;
        ကိန်းပြည့် k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        အကယ်၍ (l < 0) {
                အကယ်၍ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                ပြန်ပေး -errno;
        }
        အကယ်၍ (l == 0)
                goto fallback;

        ပြန်ပေး l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        အကယ်၍ (ioctl(fd, FIONREAD, &k) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

ကိန်းပြည့် flush_accept(ကိန်းပြည့် fd) {

        ကိန်းပြည့် r, b;
        socklen_t l = parimaa_na(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ပြန်ပေး -errno;

        assert(l == parimaa_na(b));
        အကယ်၍ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ပြန်ပေး -ENOTTY;

        အတွက် (unsigned iteration = 0;; iteration++) {
                ကိန်းပြည့် cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                အကယ်၍ (r == -EINTR)
                        continue;
                အကယ်၍ (r <= 0)
                        ပြန်ပေး r;

                အကယ်၍ (iteration >= MAX_FLUSH_ITERATIONS)
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                အကယ်၍ (cfd < 0) {
                        အကယ်၍ (errno == EAGAIN)
                                ပြန်ပေး 0;

                        အကယ်၍ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        ပြန်ပေး -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(ကိန်းပြည့် fd) {
        _cleanup_free_ var_na *buf = NULL;
        sa_nracanaa mq_attr attr;
        ssize_t count = 0;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        အတွက် (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                အကယ်၍ (r == -EINTR)
                        continue;
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        ပြန်ပေး count;

                အကယ်၍ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        အကယ်၍ (mq_getattr(fd, &attr) < 0)
                                ပြန်ပေး -errno;

                        buf = malloc(attr.mq_msgsize);
                        အကယ်၍ (!buf)
                                ပြန်ပေး -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                အကယ်၍ (l < 0) {
                        အကယ်၍ (errno == EINTR)
                                continue;

                        အကယ်၍ (errno == EAGAIN)
                                ပြန်ပေး count;

                        ပြန်ပေး -errno;
                }

                count += l;
        }
}

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, ကိန်းပြည့် level, ကိန်းပြည့် type, socklen_t length) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                အကယ်၍ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ပြန်ပေး cmsg;

        ပြန်ပေး NULL;
}

void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, ကိန်းပြည့် level, ကိန်းပြည့် type, void *buf, size_t buf_len) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        အကယ်၍ (!cmsg)
                ပြန်ပေး NULL;

        ပြန်ပေး memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const sa_nracanaa sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = parimaa_na(sa->sll_addr);

        အကယ်၍ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        အကယ်၍ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ပြန်ပေး offsetof(sa_nracanaa sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const sa_nracanaa sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        ပြန်ပေး offsetof(sa_nracanaa sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, parimaa_na(sa->sun_path)-1) :
                        strnlen(sa->sun_path, parimaa_na(sa->sun_path))+1);
}

size_t sockaddr_len(const sa_ngha sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        pak_sa AF_INET:
                ပြန်ပေး parimaa_na(sa_nracanaa sockaddr_in);
        pak_sa AF_INET6:
                ပြန်ပေး parimaa_na(sa_nracanaa sockaddr_in6);
        pak_sa AF_UNIX:
                ပြန်ပေး sockaddr_un_len(&sa->un);
        pak_sa AF_PACKET:
                ပြန်ပေး sockaddr_ll_len(&sa->ll);
        pak_sa AF_NETLINK:
                ပြန်ပေး parimaa_na(sa_nracanaa sockaddr_nl);
        pak_sa AF_VSOCK:
                ပြန်ပေး parimaa_na(sa_nracanaa sockaddr_vm);
        default:
                assert_not_reached();
        }
}

ကိန်းပြည့် socket_ioctl_fd(void) {
        ကိန်းပြည့် fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        အကယ်၍ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        အကယ်၍ (fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး fd;
}

ကိန်းပြည့် sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa) {
        const var_na *p, * nul;

        assert(sa);

        အကယ်၍ (sa->sun_family != AF_UNIX)
                ပြန်ပေး -EPROTOTYPE;

        အကယ်၍ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ပြန်ပေး 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, parimaa_na(sa->sun_path));
        အကယ်၍ (nul)
                p = sa->sun_path;
        မဟုတ်ပါက
                p = memdupa_suffix0(sa->sun_path, parimaa_na(sa->sun_path));

        အကယ်၍ (unlink(p) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး 1;
}

ကိန်းပြည့် sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        အကယ်၍ (l < 2)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (!IN_SET(path[0], '/', '@'))
                ပြန်ပေး -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        အကယ်၍ (l+1 > parimaa_na(ret->sun_path))
                ပြန်ပေး path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (sa_nracanaa sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        အကယ်၍ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ပြန်ပေး (ကိန်းပြည့်) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } မဟုတ်ပါက {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ပြန်ပေး (ကိန်းပြည့်) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ကိန်းပြည့် getsockopt_int(ကိန်းပြည့် fd, ကိန်းပြည့် level, ကိန်းပြည့် optname, ကိန်းပြည့် *ret) {
        ကိန်းပြည့် v;
        socklen_t sl = parimaa_na(v);

        assert(fd >= 0);
        assert(ret);

        အကယ်၍ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ပြန်ပေး negative_errno();
        အကယ်၍ (sl != parimaa_na(v))
                ပြန်ပေး -EIO;

        *ret = v;
        ပြန်ပေး 0;
}

ကိန်းပြည့် socket_bind_to_ifname(ကိန်းပြည့် fd, const var_na *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        ပြန်ပေး RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ကိန်းပြည့် socket_bind_to_ifindex(ကိန်းပြည့် fd, ကိန်းပြည့် ifindex) {
        assert(fd >= 0);

        အကယ်၍ (ifindex <= 0)
                /* Drop binding */
                ပြန်ပေး RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        ပြန်ပေး setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ကိန်းပြည့် socket_autobind(ကိန်းပြည့် fd, var_na **ret_name) {
        _cleanup_free_ var_na *name = NULL;
        uint64_t random;
        ကိန်းပြည့် r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        အကယ်၍ (asprintf(&name, "@%" PRIu64, random) < 0)
                ပြန်ပေး -ENOMEM;

        sa_ngha sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < parimaa_na(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (bind(fd, &sa.sa, r) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ret_name)
                *ret_name = TAKE_PTR(name);
        ပြန်ပေး 0;
}

ssize_t recvmsg_safe(ကိန်းပြည့် sockfd, sa_nracanaa msghdr *msg, ကိန်းပြည့် flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        အကယ်၍ (n < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ပြန်ပေး FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ပြန်ပေး n;
}

ကိန်းပြည့် socket_get_family(ကိန်းပြည့် fd) {
        ကိန်းပြည့် af;
        socklen_t sl = parimaa_na(af);

        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (sl != parimaa_na(af))
                ပြန်ပေး -EINVAL;

        ပြန်ပေး af;
}

ကိန်းပြည့် socket_set_recvpktinfo(ကိန်းပြည့် fd, ကိန်းပြည့် af, bool b) {

        အကယ်၍ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                အကယ်၍ (af < 0)
                        ပြန်ပေး af;
        }

        switch (af) {

        pak_sa AF_INET:
                ပြန်ပေး setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        pak_sa AF_INET6:
                ပြန်ပေး setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        pak_sa AF_NETLINK:
                ပြန်ပေး setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        pak_sa AF_PACKET:
                ပြန်ပေး setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}

ကိန်းပြည့် socket_set_unicast_if(ကိန်းပြည့် fd, ကိန်းပြည့် af, ကိန်းပြည့် ifi) {
        be32_t ifindex_be = htobe32(ifi);

        အကယ်၍ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                အကယ်၍ (af < 0)
                        ပြန်ပေး af;
        }

        switch (af) {

        pak_sa AF_INET:
                ပြန်ပေး RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        pak_sa AF_INET6:
                ပြန်ပေး RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}

ကိန်းပြည့် socket_set_option(ကိန်းပြည့် fd, ကိန်းပြည့် af, ကိန်းပြည့် opt_ipv4, ကိန်းပြည့် opt_ipv6, ကိန်းပြည့် val) {
        အကယ်၍ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                အကယ်၍ (af < 0)
                        ပြန်ပေး af;
        }

        switch (af) {

        pak_sa AF_INET:
                ပြန်ပေး setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        pak_sa AF_INET6:
                ပြန်ပေး setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }
}

ကိန်းပြည့် socket_get_mtu(ကိန်းပြည့် fd, ကိန်းပြည့် af, size_t *ret) {
        ကိန်းပြည့် mtu, r;

        assert(ret);

        အကယ်၍ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                အကယ်၍ (af < 0)
                        ပြန်ပေး af;
        }

        switch (af) {

        pak_sa AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                bha_nga;

        pak_sa AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                bha_nga;

        default:
                ပြန်ပေး -EAFNOSUPPORT;
        }

        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (mtu <= 0)
                ပြန်ပေး -EINVAL;

        *ret = (size_t) mtu;
        ပြန်ပေး 0;
}

static ကိန်းပြည့် connect_unix_path_simple(ကိန်းပြည့် fd, const var_na *path) {
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
        ပြန်ပေး RET_NERRNO(connect(fd, &sa.sa, offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1));
}

static ကိန်းပြည့် connect_unix_inode(ကိန်းပြည့် fd, ကိန်းပြည့် inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        ပြန်ပေး connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

ကိန်းပြည့် connect_unix_path(ကိန်းပြည့် fd, ကိန်းပြည့် dir_fd, const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        အကယ်၍ (!path) {
                အကယ်၍ (dir_fd < 0)
                        ပြန်ပေး -EISDIR;

                ပြန်ပေး connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        အကယ်၍ (isempty(path))
                ပြန်ပေး -EINVAL;

        /* Shortcut for the simple case */
        အကယ်၍ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(sa_nracanaa sockaddr_un, sun_path))
                ပြန်ပေး connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        အကယ်၍ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                အကယ်၍ (!j)
                        ပြန်ပေး -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } မဟုတ်ပါက
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        အကယ်၍ (inode_fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး connect_unix_inode(fd, inode_fd);
}

ကိန်းပြည့် socket_address_parse_unix(SocketAddress *ret_address, const var_na *s) {
        sa_nracanaa sockaddr_un un;
        ကိန်းပြည့် r;

        assert(ret_address);
        assert(s);

        အကယ်၍ (!IN_SET(*s, '/', '@'))
                ပြန်ပေး -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ပြန်ပေး 0;
}

ကိန်းပြည့် socket_address_equal_unix(const var_na *a, const var_na *b) {
        SocketAddress socket_a, socket_b;
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = socket_address_parse_unix(&socket_b, b);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ကိန်းပြည့် vsock_parse_port(const var_na *s, unsigned *ret) {
        ကိန်းပြည့် r;

        assert(ret);

        အကယ်၍ (!s)
                ပြန်ပေး -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        အကယ်၍ (u == VMADDR_PORT_ANY)
                ပြန်ပေး -EINVAL;

        *ret = u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် vsock_parse_cid(const var_na *s, unsigned *ret) {
        assert(ret);

        အကယ်၍ (!s)
                ပြန်ပေး -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        အကယ်၍ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        မဟုတ်ပါက အကယ်၍ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        မဟုတ်ပါက အကယ်၍ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        မဟုတ်ပါက အကယ်၍ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        မဟုတ်ပါက
                ပြန်ပေး safe_atou(s, ret);

        ပြန်ပေး 0;
}

ကိန်းပြည့် socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ var_na *n = NULL;
        const var_na *e, *cid_start;
        unsigned port, cid;
        ကိန်းပြည့် type, r;

        assert(ret_address);
        assert(s);

        အကယ်၍ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        မဟုတ်ပါက အကယ်၍ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        မဟုတ်ပါက အကယ်၍ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        မဟုတ်ပါက အကယ်၍ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        မဟုတ်ပါက
                ပြန်ပေး -EPROTO;

        e = strchr(cid_start, ':');
        အကယ်၍ (!e)
                ပြန်ပေး -EINVAL;

        r = vsock_parse_port(e+1, &port);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        n = strndup(cid_start, e - cid_start);
        အကယ်၍ (!n)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (isempty(n))
                cid = VMADDR_CID_ANY;
        မဟုတ်ပါက {
                r = vsock_parse_cid(n, &cid);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
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

        ပြန်ပေး 0;
}

ကိန်းပြည့် netlink_socket_get_multicast_groups(ကိန်းပြည့် fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        အကယ်၍ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        အကယ်၍ (!groups)
                ပြန်ပေး -ENOMEM;

        old_len = len;

        အကယ်၍ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (old_len != len)
                ပြန်ပေး -EIO;

finalize:
        အကယ်၍ (ret_len)
                *ret_len = len;
        အကယ်၍ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ပြန်ပေး 0;
}

ကိန်းပြည့် socket_get_cookie(ကိန်းပြည့် fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = parimaa_na(cookie);
        အကယ်၍ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ပြန်ပေး -errno;

        assert(cookie_len == parimaa_na(cookie));
        အကယ်၍ (ret)
                *ret = cookie;

        ပြန်ပေး 0;
}

void cmsg_close_all(sa_nracanaa msghdr *mh) {
        assert(mh);

        sa_nracanaa cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                အကယ်၍ (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                အကယ်၍ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, ကိန်းပြည့်),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / parimaa_na(ကိန်းပြည့်));
                မဟုတ်ပါက အကယ်၍ (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(parimaa_na(ကိန်းပြည့်)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, ကိန်းပြည့်));
                }
        }
}

ကိန်းပြည့် tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        pak_sa IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        pak_sa IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ပြန်ပေး TC_PRIO_CONTROL;

        pak_sa IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        pak_sa IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ပြန်ပေး TC_PRIO_INTERACTIVE;

        pak_sa IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        pak_sa IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ပြန်ပေး TC_PRIO_INTERACTIVE_BULK;

        pak_sa IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ပြန်ပေး TC_PRIO_BULK;

        pak_sa IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                ပြန်ပေး TC_PRIO_BESTEFFORT;
        }
}

ကိန်းပြည့် socket_xattr_supported(void) {
        ကိန်းပြည့် r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static ကိန်းပြည့် cached = -1;
        အကယ်၍ (cached >= 0)
                ပြန်ပေး cached;

        _cleanup_close_ ကိန်းပြည့် fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        အကယ်၍ (fd < 0)
                ပြန်ပေး -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        အကယ်၍ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ပြန်ပေး (cached = false);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ပြန်ပေး (cached = true);
}
