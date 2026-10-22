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

#ถ้า ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#มิฉะนั้น
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

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, จำนวนเต็ม);

จำนวนเต็ม socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ถ้า (a->size != parimaa_na(sa_nracanaa sockaddr_in))
                        คืน -EINVAL;

                ถ้า (a->sockaddr.in.sin_port == 0)
                        คืน -EINVAL;

                ถ้า (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        คืน -EINVAL;

                คืน 0;

        pak_sa AF_INET6:
                ถ้า (a->size != parimaa_na(sa_nracanaa sockaddr_in6))
                        คืน -EINVAL;

                ถ้า (a->sockaddr.in6.sin6_port == 0)
                        คืน -EINVAL;

                ถ้า (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        คืน -EINVAL;

                คืน 0;

        pak_sa AF_UNIX:
                ถ้า (a->size < offsetof(sa_nracanaa sockaddr_un, sun_path))
                        คืน -EINVAL;
                ถ้า (a->size > parimaa_na(sa_nracanaa sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        คืน -EINVAL;

                ถ้า (a->size > offsetof(sa_nracanaa sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const var_na *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, parimaa_na(a->sockaddr.un.sun_path));
                        ถ้า (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ถ้า (a->size != offsetof(sa_nracanaa sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        คืน -EINVAL;
                        } มิฉะนั้น {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ถ้า (!IN_SET(a->size, parimaa_na(a->sockaddr.un.sun_path), parimaa_na(a->sockaddr.un.sun_path)+1))
                                        คืน -EINVAL;
                        }
                }

                ถ้า (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        คืน -EINVAL;

                คืน 0;

        pak_sa AF_NETLINK:

                ถ้า (a->size != parimaa_na(sa_nracanaa sockaddr_nl))
                        คืน -EINVAL;

                ถ้า (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        คืน -EINVAL;

                คืน 0;

        pak_sa AF_VSOCK:
                ถ้า (a->size != parimaa_na(sa_nracanaa sockaddr_vm))
                        คืน -EINVAL;

                ถ้า (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        คืน -EINVAL;

                คืน 0;

        default:
                คืน -EAFNOSUPPORT;
        }
}

จำนวนเต็ม socket_address_print(const SocketAddress *a, var_na **ret) {
        จำนวนเต็ม r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ถ้า (r < 0)
                คืน r;

        ถ้า (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ var_na *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ถ้า (r < 0)
                        คืน r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ถ้า (r < 0)
                        คืน -ENOMEM;

                คืน 0;
        }

        คืน sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        คืน
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        ถ้า (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                คืน false;

        ถ้า (a->type != b->type)
                คืน false;

        ถ้า (socket_address_family(a) != socket_address_family(b))
                คืน false;

        switch (socket_address_family(a)) {

        pak_sa AF_INET:
                ถ้า (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        คืน false;

                ถ้า (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        คืน false;

                bha_nga;

        pak_sa AF_INET6:
                ถ้า (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, parimaa_na(a->sockaddr.in6.sin6_addr)) != 0)
                        คืน false;

                ถ้า (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        คืน false;

                bha_nga;

        pak_sa AF_UNIX:
                ถ้า (a->size <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    b->size <= offsetof(sa_nracanaa sockaddr_un, sun_path))
                        คืน false;

                ถ้า ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        คืน false;

                ถ้า (a->sockaddr.un.sun_path[0]) {
                        ถ้า (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                คืน false;
                } มิฉะนั้น {
                        ถ้า (a->size != b->size)
                                คืน false;

                        ถ้า (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                คืน false;
                }

                bha_nga;

        pak_sa AF_NETLINK:
                ถ้า (a->protocol != b->protocol)
                        คืน false;

                ถ้า (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        คืน false;

                bha_nga;

        pak_sa AF_VSOCK:
                ถ้า (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        คืน false;

                ถ้า (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        คืน false;

                bha_nga;

        default:
                /* Cannot compare, so we assume the addresses are different */
                คืน false;
        }

        คืน true;
}

const var_na* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        ถ้า (socket_address_family(a) != AF_UNIX)
                คืน NULL;

        ถ้า (a->sockaddr.un.sun_path[0] == 0)
                คืน NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(parimaa_na(sa_ngha sockaddr_union) >= parimaa_na(sa_nracanaa sockaddr_un)+1);
        คืน a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static จำนวนเต็ม cached = -1;

        ถ้า (cached < 0) {

                ถ้า (access("/proc/net/if_inet6", F_OK) < 0) {

                        ถ้า (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                คืน false;
                        }

                        cached = false;
                } มิฉะนั้น
                        cached = true;
        }

        คืน cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ var_na *v = NULL;
        จำนวนเต็ม r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ถ้า (!socket_ipv6_is_supported())
                คืน false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ถ้า (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                คืน true;
        }

        r = parse_boolean(v);
        ถ้า (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                คืน true;
        }

        คืน !r;
}

bool socket_address_matches_fd(const SocketAddress *a, จำนวนเต็ม fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = parimaa_na(b.sockaddr);
        ถ้า (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                คืน false;

        ถ้า (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                คืน false;

        solen = parimaa_na(b.type);
        ถ้า (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                คืน false;

        ถ้า (b.type != a->type)
                คืน false;

        ถ้า (a->protocol != 0)  {
                solen = parimaa_na(b.protocol);
                ถ้า (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        คืน false;

                ถ้า (b.protocol != a->protocol)
                        คืน false;
        }

        คืน socket_address_equal(a, &b);
}

จำนวนเต็ม sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *ret_port) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                คืน 0;

        pak_sa AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                คืน 0;

        pak_sa AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                คืน 0;

        default:
                คืน -EAFNOSUPPORT;
        }
}

const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *_sa) {
        const sa_ngha sockaddr_union *sa = (const sa_ngha sockaddr_union*) _sa;

        ถ้า (!sa)
                คืน NULL;

        switch (sa->sa.sa_family) {

        pak_sa AF_INET:
                คืน (const sa_ngha in_addr_union*) &sa->in.sin_addr;

        pak_sa AF_INET6:
                คืน (const sa_ngha in_addr_union*) &sa->in6.sin6_addr;

        default:
                คืน NULL;
        }
}

จำนวนเต็ม sockaddr_set_in_addr(
                sa_ngha sockaddr_union *u,
                จำนวนเต็ม family,
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

                คืน 0;

        pak_sa AF_INET6:
                u->in6 = (sa_nracanaa sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                คืน 0;

        default:
                คืน -EAFNOSUPPORT;

        }
}

จำนวนเต็ม sockaddr_pretty(
                const sa_nracanaa sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                var_na **ret) {

        sa_ngha sockaddr_union *sa = (sa_ngha sockaddr_union*) _sa;
        var_na *p;
        จำนวนเต็ม r;

        assert(sa);
        assert(salen >= parimaa_na(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        pak_sa AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ถ้า (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                มิฉะนั้น
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ถ้า (r < 0)
                        คืน -ENOMEM;
                bha_nga;
        }

        pak_sa AF_INET6: {
                static const unsigned var_na ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ถ้า (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, parimaa_na(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ถ้า (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        มิฉะนั้น
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ถ้า (r < 0)
                                คืน -ENOMEM;
                } มิฉะนั้น {
                        const var_na *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ถ้า (include_port) {
                                ถ้า (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        คืน -ENOMEM;
                        } มิฉะนั้น {
                                ถ้า (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                มิฉะนั้น
                                        p = strdup(a);
                                ถ้า (!p)
                                        คืน -ENOMEM;
                        }
                }

                bha_nga;
        }

        pak_sa AF_UNIX:
                ถ้า (salen <= offsetof(sa_nracanaa sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(sa_nracanaa sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                มิฉะนั้น {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        var_na *path = (var_na*) sa->un_buffer + offsetof(sa_nracanaa sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(sa_nracanaa sockaddr_un, sun_path);

                        ถ้า (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ var_na *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                ถ้า (!e)
                                        คืน -ENOMEM;

                                p = strjoin("@", e);
                        } มิฉะนั้น {
                                ถ้า (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ถ้า (!p)
                        คืน -ENOMEM;

                bha_nga;

        pak_sa AF_VSOCK:
                ถ้า (include_port) {
                        ถ้า (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        มิฉะนั้น
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } มิฉะนั้น
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ถ้า (r < 0)
                        คืน -ENOMEM;
                bha_nga;

        default:
                คืน -EOPNOTSUPP;
        }

        *ret = p;
        คืน 0;
}

จำนวนเต็ม getpeername_pretty(จำนวนเต็ม fd, bool include_port, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);
        จำนวนเต็ม r;

        assert(fd >= 0);
        assert(ret);

        ถ้า (getpeername(fd, &sa.sa, &salen) < 0)
                คืน -errno;

        ถ้า (sa.sa.sa_family == AF_UNIX) {
                sa_nracanaa ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        คืน -ENOMEM;

                คืน 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        คืน sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

จำนวนเต็ม getsockname_pretty(จำนวนเต็ม fd, var_na **ret) {
        sa_ngha sockaddr_union sa;
        socklen_t salen = parimaa_na(sa);

        assert(fd >= 0);
        assert(ret);

        ถ้า (getsockname(fd, &sa.sa, &salen) < 0)
                คืน -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        คืน sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

จำนวนเต็ม socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **ret) {
        var_na host[NI_MAXHOST];
        จำนวนเต็ม r;

        assert(sa);
        assert(salen >= parimaa_na(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, parimaa_na(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        ถ้า (r != 0) {
                ถ้า (r == EAI_MEMORY)
                        คืน log_oom_debug();
                ถ้า (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                มิฉะนั้น
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                คืน sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        คืน strdup_to(ret, host);
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, จำนวนเต็ม, INT_MAX);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b) {
        assert(a);
        assert(b);

        ถ้า (a->sa.sa_family != b->sa.sa_family)
                คืน false;

        ถ้า (a->sa.sa_family == AF_INET)
                คืน a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ถ้า (a->sa.sa_family == AF_INET6)
                คืน memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, parimaa_na(a->in6.sin6_addr)) == 0;

        ถ้า (a->sa.sa_family == AF_VSOCK)
                คืน a->vm.svm_cid == b->vm.svm_cid;

        คืน false;
}

จำนวนเต็ม fd_set_sndbuf(จำนวนเต็ม fd, size_t n, bool increase) {
        จำนวนเต็ม r, value;
        socklen_t l = parimaa_na(value);

        ถ้า (n > INT_MAX)
                คืน -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ถ้า (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                คืน 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ถ้า (r < 0)
                คืน r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ถ้า (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                คืน 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ถ้า (r < 0)
                คืน r;

        คืน 1;
}

จำนวนเต็ม fd_set_rcvbuf(จำนวนเต็ม fd, size_t n, bool increase) {
        จำนวนเต็ม r, value;
        socklen_t l = parimaa_na(value);

        ถ้า (n > INT_MAX)
                คืน -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ถ้า (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                คืน 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ถ้า (r < 0)
                คืน r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = parimaa_na(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ถ้า (r >= 0 && l == parimaa_na(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                คืน 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ถ้า (r < 0)
                คืน r;

        คืน 1;
}

static const var_na* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, จำนวนเต็ม, 0xff);

bool ifname_valid_char(var_na a) {
        ถ้า ((unsigned var_na) a >= 127U)
                คืน false;

        ถ้า ((unsigned var_na) a <= 32U)
                คืน false;

        ถ้า (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                คืน false;

        คืน true;
}

bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        ถ้า (isempty(p))
                คืน false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ถ้า (parse_ifindex(p) >= 0)
                คืน flags & IFNAME_VALID_NUMERIC;

        ถ้า (flags & IFNAME_VALID_ALTERNATIVE) {
                ถ้า (strlen(p) >= ALTIFNAMSIZ)
                        คืน false;
        } มิฉะนั้น {
                ถ้า (strlen(p) >= IFNAMSIZ)
                        คืน false;
        }

        ถ้า (dot_or_dot_dot(p))
                คืน false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ถ้า (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                คืน false;

        สำหรับ (const var_na *t = p; *t; t++) {
                ถ้า (!ifname_valid_char(*t))
                        คืน false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ถ้า (numeric)
                คืน false;

        คืน true;
}

bool address_label_valid(const var_na *p) {

        POINTER_MAY_BE_NULL(p);

        ถ้า (isempty(p))
                คืน false;

        ถ้า (strlen(p) >= IFNAMSIZ)
                คืน false;

        ขณะ (*p) {
                ถ้า ((uint8_t) *p >= 127U)
                        คืน false;

                ถ้า ((uint8_t) *p <= 31U)
                        คืน false;
                p++;
        }

        คืน true;
}

จำนวนเต็ม getpeercred(จำนวนเต็ม fd, sa_nracanaa ucred *ucred) {
        socklen_t n = parimaa_na(sa_nracanaa ucred);
        sa_nracanaa ucred u;

        assert(fd >= 0);
        assert(ucred);

        ถ้า (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                คืน -errno;

        ถ้า (n != parimaa_na(sa_nracanaa ucred))
                คืน -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ถ้า (!pid_is_valid(u.pid))
                คืน -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        คืน 0;
}

จำนวนเต็ม getpeersec(จำนวนเต็ม fd, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        สำหรับ (;;) {
                s = new0(var_na, n+1);
                ถ้า (!s)
                        คืน -ENOMEM;

                ถ้า (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        bha_nga;
                }

                ถ้า (errno != ERANGE)
                        คืน -errno;

                s = mfree(s);
        }

        ถ้า (isempty(s))
                คืน -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        คืน 0;
}

จำนวนเต็ม getpeergroups(จำนวนเต็ม fd, gid_t **ret) {
        socklen_t n = parimaa_na(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        จำนวนเต็ม ngroups_max = sysconf_ngroups_max();
        ถ้า (ngroups_max > 0)
                n = MAX(n, parimaa_na(gid_t) * (socklen_t) ngroups_max);

        สำหรับ (;;) {
                d = malloc(n);
                ถ้า (!d)
                        คืน -ENOMEM;

                ถ้า (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        bha_nga;

                ถ้า (errno != ERANGE)
                        คืน -errno;

                d = mfree(d);
        }

        assert_se(n % parimaa_na(gid_t) == 0);
        n /= parimaa_na(gid_t);

        ถ้า (n > INT_MAX)
                คืน -E2BIG;

        *ret = TAKE_PTR(d);

        คืน (จำนวนเต็ม) n;
}

จำนวนเต็ม getpeerpidfd(จำนวนเต็ม fd) {
        socklen_t n = parimaa_na(จำนวนเต็ม);
        จำนวนเต็ม pidfd = -EBADF;

        assert(fd >= 0);

        ถ้า (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                คืน -errno;

        ถ้า (n != parimaa_na(จำนวนเต็ม))
                คืน -EIO;

        คืน pidfd;
}

จำนวนเต็ม getpeerpidref(จำนวนเต็ม fd, PidRef *ret) {
        จำนวนเต็ม r;

        assert(fd >= 0);
        assert(ret);

        จำนวนเต็ม pidfd = getpeerpidfd(fd);
        ถ้า (pidfd < 0) {
                ถ้า (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        คืน pidfd;

                sa_nracanaa ucred ucred;
                r = getpeercred(fd, &ucred);
                ถ้า (r < 0)
                        คืน r;

                คืน pidref_set_pid(ret, ucred.pid);
        }

        คืน pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                จำนวนเต็ม transport_fd,
                จำนวนเต็ม fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                จำนวนเต็ม flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(จำนวนเต็ม))) control = {};
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
        ถ้า (fd < 0 && !iov)
                คืน -EINVAL;

        ถ้า (fd >= 0) {
                sa_nracanaa cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = parimaa_na(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(parimaa_na(จำนวนเต็ม));
                memcpy(CMSG_DATA(cmsg), &fd, parimaa_na(จำนวนเต็ม));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ถ้า (k < 0)
                คืน (ssize_t) -errno;

        คืน k;
}

จำนวนเต็ม send_one_fd_sa(
                จำนวนเต็ม transport_fd,
                จำนวนเต็ม fd,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                จำนวนเต็ม flags) {

        assert(fd >= 0);

        คืน (จำนวนเต็ม) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                จำนวนเต็ม transport_fd,
                sa_nracanaa iovec *iov, size_t iovlen,
                จำนวนเต็ม flags,
                จำนวนเต็ม *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(parimaa_na(จำนวนเต็ม))) control;
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
        ถ้า (k < 0)
                คืน k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(parimaa_na(จำนวนเต็ม)));
        ถ้า (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ถ้า (k == 0)
                        คืน -EIO;
        }

        ถ้า (found)
                *ret_fd = *CMSG_TYPED_DATA(found, จำนวนเต็ม);
        มิฉะนั้น
                *ret_fd = -EBADF;

        คืน k;
}

จำนวนเต็ม receive_one_fd(จำนวนเต็ม transport_fd, จำนวนเต็ม flags) {
        จำนวนเต็ม fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        ถ้า (k == 0)
                คืน fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        คืน (จำนวนเต็ม) k;
}

ssize_t next_datagram_size_fd(จำนวนเต็ม fd) {
        ssize_t l;
        จำนวนเต็ม k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        ถ้า (l < 0) {
                ถ้า (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                คืน -errno;
        }
        ถ้า (l == 0)
                goto fallback;

        คืน l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ถ้า (ioctl(fd, FIONREAD, &k) < 0)
                คืน -errno;

        คืน (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

จำนวนเต็ม flush_accept(จำนวนเต็ม fd) {

        จำนวนเต็ม r, b;
        socklen_t l = parimaa_na(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ถ้า (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                คืน -errno;

        assert(l == parimaa_na(b));
        ถ้า (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                คืน -ENOTTY;

        สำหรับ (unsigned iteration = 0;; iteration++) {
                จำนวนเต็ม cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ถ้า (r == -EINTR)
                        continue;
                ถ้า (r <= 0)
                        คืน r;

                ถ้า (iteration >= MAX_FLUSH_ITERATIONS)
                        คืน log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ถ้า (cfd < 0) {
                        ถ้า (errno == EAGAIN)
                                คืน 0;

                        ถ้า (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        คืน -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(จำนวนเต็ม fd) {
        _cleanup_free_ var_na *buf = NULL;
        sa_nracanaa mq_attr attr;
        ssize_t count = 0;
        จำนวนเต็ม r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        สำหรับ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ถ้า (r == -EINTR)
                        continue;
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        คืน count;

                ถ้า (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ถ้า (mq_getattr(fd, &attr) < 0)
                                คืน -errno;

                        buf = malloc(attr.mq_msgsize);
                        ถ้า (!buf)
                                คืน -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                ถ้า (l < 0) {
                        ถ้า (errno == EINTR)
                                continue;

                        ถ้า (errno == EAGAIN)
                                คืน count;

                        คืน -errno;
                }

                count += l;
        }
}

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, จำนวนเต็ม level, จำนวนเต็ม type, socklen_t length) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                ถ้า (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        คืน cmsg;

        คืน NULL;
}

void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, จำนวนเต็ม level, จำนวนเต็ม type, void *buf, size_t buf_len) {
        sa_nracanaa cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ถ้า (!cmsg)
                คืน NULL;

        คืน memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const sa_nracanaa sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = parimaa_na(sa->sll_addr);

        ถ้า (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ถ้า (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        คืน offsetof(sa_nracanaa sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const sa_nracanaa sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        คืน offsetof(sa_nracanaa sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, parimaa_na(sa->sun_path)-1) :
                        strnlen(sa->sun_path, parimaa_na(sa->sun_path))+1);
}

size_t sockaddr_len(const sa_ngha sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        pak_sa AF_INET:
                คืน parimaa_na(sa_nracanaa sockaddr_in);
        pak_sa AF_INET6:
                คืน parimaa_na(sa_nracanaa sockaddr_in6);
        pak_sa AF_UNIX:
                คืน sockaddr_un_len(&sa->un);
        pak_sa AF_PACKET:
                คืน sockaddr_ll_len(&sa->ll);
        pak_sa AF_NETLINK:
                คืน parimaa_na(sa_nracanaa sockaddr_nl);
        pak_sa AF_VSOCK:
                คืน parimaa_na(sa_nracanaa sockaddr_vm);
        default:
                assert_not_reached();
        }
}

จำนวนเต็ม socket_ioctl_fd(void) {
        จำนวนเต็ม fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ถ้า (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ถ้า (fd < 0)
                คืน -errno;

        คืน fd;
}

จำนวนเต็ม sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa) {
        const var_na *p, * nul;

        assert(sa);

        ถ้า (sa->sun_family != AF_UNIX)
                คืน -EPROTOTYPE;

        ถ้า (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                คืน 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, parimaa_na(sa->sun_path));
        ถ้า (nul)
                p = sa->sun_path;
        มิฉะนั้น
                p = memdupa_suffix0(sa->sun_path, parimaa_na(sa->sun_path));

        ถ้า (unlink(p) < 0)
                คืน -errno;

        คืน 1;
}

จำนวนเต็ม sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ถ้า (l < 2)
                คืน -EINVAL;
        ถ้า (!IN_SET(path[0], '/', '@'))
                คืน -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ถ้า (l+1 > parimaa_na(ret->sun_path))
                คืน path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (sa_nracanaa sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ถ้า (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                คืน (จำนวนเต็ม) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } มิฉะนั้น {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                คืน (จำนวนเต็ม) (offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

จำนวนเต็ม getsockopt_int(จำนวนเต็ม fd, จำนวนเต็ม level, จำนวนเต็ม optname, จำนวนเต็ม *ret) {
        จำนวนเต็ม v;
        socklen_t sl = parimaa_na(v);

        assert(fd >= 0);
        assert(ret);

        ถ้า (getsockopt(fd, level, optname, &v, &sl) < 0)
                คืน negative_errno();
        ถ้า (sl != parimaa_na(v))
                คืน -EIO;

        *ret = v;
        คืน 0;
}

จำนวนเต็ม socket_bind_to_ifname(จำนวนเต็ม fd, const var_na *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        คืน RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

จำนวนเต็ม socket_bind_to_ifindex(จำนวนเต็ม fd, จำนวนเต็ม ifindex) {
        assert(fd >= 0);

        ถ้า (ifindex <= 0)
                /* Drop binding */
                คืน RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        คืน setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

จำนวนเต็ม socket_autobind(จำนวนเต็ม fd, var_na **ret_name) {
        _cleanup_free_ var_na *name = NULL;
        uint64_t random;
        จำนวนเต็ม r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        ถ้า (asprintf(&name, "@%" PRIu64, random) < 0)
                คืน -ENOMEM;

        sa_ngha sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < parimaa_na(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ถ้า (r < 0)
                คืน r;

        ถ้า (bind(fd, &sa.sa, r) < 0)
                คืน -errno;

        ถ้า (ret_name)
                *ret_name = TAKE_PTR(name);
        คืน 0;
}

ssize_t recvmsg_safe(จำนวนเต็ม sockfd, sa_nracanaa msghdr *msg, จำนวนเต็ม flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        ถ้า (n < 0)
                คืน -errno;

        ถ้า (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                คืน FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        คืน n;
}

จำนวนเต็ม socket_get_family(จำนวนเต็ม fd) {
        จำนวนเต็ม af;
        socklen_t sl = parimaa_na(af);

        ถ้า (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                คืน -errno;

        ถ้า (sl != parimaa_na(af))
                คืน -EINVAL;

        คืน af;
}

จำนวนเต็ม socket_set_recvpktinfo(จำนวนเต็ม fd, จำนวนเต็ม af, bool b) {

        ถ้า (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ถ้า (af < 0)
                        คืน af;
        }

        switch (af) {

        pak_sa AF_INET:
                คืน setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        pak_sa AF_INET6:
                คืน setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        pak_sa AF_NETLINK:
                คืน setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        pak_sa AF_PACKET:
                คืน setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                คืน -EAFNOSUPPORT;
        }
}

จำนวนเต็ม socket_set_unicast_if(จำนวนเต็ม fd, จำนวนเต็ม af, จำนวนเต็ม ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ถ้า (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ถ้า (af < 0)
                        คืน af;
        }

        switch (af) {

        pak_sa AF_INET:
                คืน RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        pak_sa AF_INET6:
                คืน RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, parimaa_na(ifindex_be)));

        default:
                คืน -EAFNOSUPPORT;
        }
}

จำนวนเต็ม socket_set_option(จำนวนเต็ม fd, จำนวนเต็ม af, จำนวนเต็ม opt_ipv4, จำนวนเต็ม opt_ipv6, จำนวนเต็ม val) {
        ถ้า (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ถ้า (af < 0)
                        คืน af;
        }

        switch (af) {

        pak_sa AF_INET:
                คืน setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        pak_sa AF_INET6:
                คืน setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                คืน -EAFNOSUPPORT;
        }
}

จำนวนเต็ม socket_get_mtu(จำนวนเต็ม fd, จำนวนเต็ม af, size_t *ret) {
        จำนวนเต็ม mtu, r;

        assert(ret);

        ถ้า (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ถ้า (af < 0)
                        คืน af;
        }

        switch (af) {

        pak_sa AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                bha_nga;

        pak_sa AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                bha_nga;

        default:
                คืน -EAFNOSUPPORT;
        }

        ถ้า (r < 0)
                คืน r;
        ถ้า (mtu <= 0)
                คืน -EINVAL;

        *ret = (size_t) mtu;
        คืน 0;
}

static จำนวนเต็ม connect_unix_path_simple(จำนวนเต็ม fd, const var_na *path) {
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
        คืน RET_NERRNO(connect(fd, &sa.sa, offsetof(sa_nracanaa sockaddr_un, sun_path) + l + 1));
}

static จำนวนเต็ม connect_unix_inode(จำนวนเต็ม fd, จำนวนเต็ม inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        คืน connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

จำนวนเต็ม connect_unix_path(จำนวนเต็ม fd, จำนวนเต็ม dir_fd, const var_na *path) {
        _cleanup_close_ จำนวนเต็ม inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ถ้า (!path) {
                ถ้า (dir_fd < 0)
                        คืน -EISDIR;

                คืน connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ถ้า (isempty(path))
                คืน -EINVAL;

        /* Shortcut for the simple case */
        ถ้า (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(sa_nracanaa sockaddr_un, sun_path))
                คืน connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ถ้า (dir_fd == XAT_FDROOT) {
                _cleanup_free_ var_na *j = strjoin("/", path);
                ถ้า (!j)
                        คืน -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } มิฉะนั้น
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ถ้า (inode_fd < 0)
                คืน -errno;

        คืน connect_unix_inode(fd, inode_fd);
}

จำนวนเต็ม socket_address_parse_unix(SocketAddress *ret_address, const var_na *s) {
        sa_nracanaa sockaddr_un un;
        จำนวนเต็ม r;

        assert(ret_address);
        assert(s);

        ถ้า (!IN_SET(*s, '/', '@'))
                คืน -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ถ้า (r < 0)
                คืน r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        คืน 0;
}

จำนวนเต็ม socket_address_equal_unix(const var_na *a, const var_na *b) {
        SocketAddress socket_a, socket_b;
        จำนวนเต็ม r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        ถ้า (r < 0)
                คืน r;

        r = socket_address_parse_unix(&socket_b, b);
        ถ้า (r < 0)
                คืน r;

        คืน sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

จำนวนเต็ม vsock_parse_port(const var_na *s, unsigned *ret) {
        จำนวนเต็ม r;

        assert(ret);

        ถ้า (!s)
                คืน -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ถ้า (r < 0)
                คืน r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ถ้า (u == VMADDR_PORT_ANY)
                คืน -EINVAL;

        *ret = u;
        คืน 0;
}

จำนวนเต็ม vsock_parse_cid(const var_na *s, unsigned *ret) {
        assert(ret);

        ถ้า (!s)
                คืน -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ถ้า (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        มิฉะนั้น ถ้า (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        มิฉะนั้น ถ้า (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        มิฉะนั้น ถ้า (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        มิฉะนั้น
                คืน safe_atou(s, ret);

        คืน 0;
}

จำนวนเต็ม socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ var_na *n = NULL;
        const var_na *e, *cid_start;
        unsigned port, cid;
        จำนวนเต็ม type, r;

        assert(ret_address);
        assert(s);

        ถ้า ((cid_start = startswith(s, "vsock:")))
                type = 0;
        มิฉะนั้น ถ้า ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        มิฉะนั้น ถ้า ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        มิฉะนั้น ถ้า ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        มิฉะนั้น
                คืน -EPROTO;

        e = strchr(cid_start, ':');
        ถ้า (!e)
                คืน -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ถ้า (r < 0)
                คืน r;

        n = strndup(cid_start, e - cid_start);
        ถ้า (!n)
                คืน -ENOMEM;

        ถ้า (isempty(n))
                cid = VMADDR_CID_ANY;
        มิฉะนั้น {
                r = vsock_parse_cid(n, &cid);
                ถ้า (r < 0)
                        คืน r;
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

        คืน 0;
}

จำนวนเต็ม netlink_socket_get_multicast_groups(จำนวนเต็ม fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        ถ้า (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                คืน -errno;

        ถ้า (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        ถ้า (!groups)
                คืน -ENOMEM;

        old_len = len;

        ถ้า (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                คืน -errno;

        ถ้า (old_len != len)
                คืน -EIO;

finalize:
        ถ้า (ret_len)
                *ret_len = len;
        ถ้า (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        คืน 0;
}

จำนวนเต็ม socket_get_cookie(จำนวนเต็ม fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = parimaa_na(cookie);
        ถ้า (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                คืน -errno;

        assert(cookie_len == parimaa_na(cookie));
        ถ้า (ret)
                *ret = cookie;

        คืน 0;
}

void cmsg_close_all(sa_nracanaa msghdr *mh) {
        assert(mh);

        sa_nracanaa cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ถ้า (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                ถ้า (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, จำนวนเต็ม),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / parimaa_na(จำนวนเต็ม));
                มิฉะนั้น ถ้า (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(parimaa_na(จำนวนเต็ม)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, จำนวนเต็ม));
                }
        }
}

จำนวนเต็ม tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        pak_sa IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        pak_sa IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                คืน TC_PRIO_CONTROL;

        pak_sa IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        pak_sa IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                คืน TC_PRIO_INTERACTIVE;

        pak_sa IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        pak_sa IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                คืน TC_PRIO_INTERACTIVE_BULK;

        pak_sa IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                คืน TC_PRIO_BULK;

        pak_sa IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                คืน TC_PRIO_BESTEFFORT;
        }
}

จำนวนเต็ม socket_xattr_supported(void) {
        จำนวนเต็ม r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static จำนวนเต็ม cached = -1;
        ถ้า (cached >= 0)
                คืน cached;

        _cleanup_close_ จำนวนเต็ม fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ถ้า (fd < 0)
                คืน -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        ถ้า (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                คืน (cached = false);
        ถ้า (r < 0)
                คืน log_debug_errno(r, "Failed to set test xattr on socket: %m");

        คืน (cached = true);
}
