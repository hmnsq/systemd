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

#якщо ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#інакше
#  define IDN_FLAGS 0
#endif

static const char* const socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, ціле);

ціле socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        case AF_INET:
                якщо (a->size != sizeof(struct sockaddr_in))
                        повернути -EINVAL;

                якщо (a->sockaddr.in.sin_port == 0)
                        повернути -EINVAL;

                якщо (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        повернути -EINVAL;

                повернути 0;

        case AF_INET6:
                якщо (a->size != sizeof(struct sockaddr_in6))
                        повернути -EINVAL;

                якщо (a->sockaddr.in6.sin6_port == 0)
                        повернути -EINVAL;

                якщо (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        повернути -EINVAL;

                повернути 0;

        case AF_UNIX:
                якщо (a->size < offsetof(struct sockaddr_un, sun_path))
                        повернути -EINVAL;
                якщо (a->size > sizeof(struct sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        повернути -EINVAL;

                якщо (a->size > offsetof(struct sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const char *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, sizeof(a->sockaddr.un.sun_path));
                        якщо (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                якщо (a->size != offsetof(struct sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        повернути -EINVAL;
                        } інакше {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                якщо (!IN_SET(a->size, sizeof(a->sockaddr.un.sun_path), sizeof(a->sockaddr.un.sun_path)+1))
                                        повернути -EINVAL;
                        }
                }

                якщо (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        повернути -EINVAL;

                повернути 0;

        case AF_NETLINK:

                якщо (a->size != sizeof(struct sockaddr_nl))
                        повернути -EINVAL;

                якщо (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        повернути -EINVAL;

                повернути 0;

        case AF_VSOCK:
                якщо (a->size != sizeof(struct sockaddr_vm))
                        повернути -EINVAL;

                якщо (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        повернути -EINVAL;

                повернути 0;

        default:
                повернути -EAFNOSUPPORT;
        }
}

ціле socket_address_print(const SocketAddress *a, char **ret) {
        ціле r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        якщо (r < 0)
                повернути r;

        якщо (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ char *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                якщо (r < 0)
                        повернути r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                якщо (r < 0)
                        повернути -ENOMEM;

                повернути 0;
        }

        повернути sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        повернути
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        якщо (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                повернути false;

        якщо (a->type != b->type)
                повернути false;

        якщо (socket_address_family(a) != socket_address_family(b))
                повернути false;

        switch (socket_address_family(a)) {

        case AF_INET:
                якщо (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        повернути false;

                якщо (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        повернути false;

                break;

        case AF_INET6:
                якщо (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, sizeof(a->sockaddr.in6.sin6_addr)) != 0)
                        повернути false;

                якщо (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        повернути false;

                break;

        case AF_UNIX:
                якщо (a->size <= offsetof(struct sockaddr_un, sun_path) ||
                    b->size <= offsetof(struct sockaddr_un, sun_path))
                        повернути false;

                якщо ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        повернути false;

                якщо (a->sockaddr.un.sun_path[0]) {
                        якщо (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                повернути false;
                } інакше {
                        якщо (a->size != b->size)
                                повернути false;

                        якщо (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                повернути false;
                }

                break;

        case AF_NETLINK:
                якщо (a->protocol != b->protocol)
                        повернути false;

                якщо (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        повернути false;

                break;

        case AF_VSOCK:
                якщо (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        повернути false;

                якщо (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        повернути false;

                break;

        default:
                /* Cannot compare, so we assume the addresses are different */
                повернути false;
        }

        повернути true;
}

const char* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        якщо (socket_address_family(a) != AF_UNIX)
                повернути NULL;

        якщо (a->sockaddr.un.sun_path[0] == 0)
                повернути NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(sizeof(union sockaddr_union) >= sizeof(struct sockaddr_un)+1);
        повернути a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static ціле cached = -1;

        якщо (cached < 0) {

                якщо (access("/proc/net/if_inet6", F_OK) < 0) {

                        якщо (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                повернути false;
                        }

                        cached = false;
                } інакше
                        cached = true;
        }

        повернути cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ char *v = NULL;
        ціле r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        якщо (!socket_ipv6_is_supported())
                повернути false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        якщо (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                повернути true;
        }

        r = parse_boolean(v);
        якщо (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                повернути true;
        }

        повернути !r;
}

bool socket_address_matches_fd(const SocketAddress *a, ціле fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = sizeof(b.sockaddr);
        якщо (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                повернути false;

        якщо (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                повернути false;

        solen = sizeof(b.type);
        якщо (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                повернути false;

        якщо (b.type != a->type)
                повернути false;

        якщо (a->protocol != 0)  {
                solen = sizeof(b.protocol);
                якщо (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        повернути false;

                якщо (b.protocol != a->protocol)
                        повернути false;
        }

        повернути socket_address_equal(a, &b);
}

ціле sockaddr_port(const struct sockaddr *_sa, unsigned *ret_port) {
        const union sockaddr_union *sa = (const union sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        case AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                повернути 0;

        case AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                повернути 0;

        case AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                повернути 0;

        default:
                повернути -EAFNOSUPPORT;
        }
}

const union in_addr_union *sockaddr_in_addr(const struct sockaddr *_sa) {
        const union sockaddr_union *sa = (const union sockaddr_union*) _sa;

        якщо (!sa)
                повернути NULL;

        switch (sa->sa.sa_family) {

        case AF_INET:
                повернути (const union in_addr_union*) &sa->in.sin_addr;

        case AF_INET6:
                повернути (const union in_addr_union*) &sa->in6.sin6_addr;

        default:
                повернути NULL;
        }
}

ціле sockaddr_set_in_addr(
                union sockaddr_union *u,
                ціле family,
                const union in_addr_union *a,
                uint16_t port) {

        assert(u);
        assert(a);

        switch (family) {

        case AF_INET:
                u->in = (struct sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                повернути 0;

        case AF_INET6:
                u->in6 = (struct sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                повернути 0;

        default:
                повернути -EAFNOSUPPORT;

        }
}

ціле sockaddr_pretty(
                const struct sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                char **ret) {

        union sockaddr_union *sa = (union sockaddr_union*) _sa;
        char *p;
        ціле r;

        assert(sa);
        assert(salen >= sizeof(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        case AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                якщо (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                інакше
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                якщо (r < 0)
                        повернути -ENOMEM;
                break;
        }

        case AF_INET6: {
                static const unsigned char ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                якщо (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, sizeof(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        якщо (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        інакше
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        якщо (r < 0)
                                повернути -ENOMEM;
                } інакше {
                        const char *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        якщо (include_port) {
                                якщо (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        повернути -ENOMEM;
                        } інакше {
                                якщо (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                інакше
                                        p = strdup(a);
                                якщо (!p)
                                        повернути -ENOMEM;
                        }
                }

                break;
        }

        case AF_UNIX:
                якщо (salen <= offsetof(struct sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(struct sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                інакше {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        char *path = (char*) sa->un_buffer + offsetof(struct sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(struct sockaddr_un, sun_path);

                        якщо (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ char *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                якщо (!e)
                                        повернути -ENOMEM;

                                p = strjoin("@", e);
                        } інакше {
                                якщо (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                якщо (!p)
                        повернути -ENOMEM;

                break;

        case AF_VSOCK:
                якщо (include_port) {
                        якщо (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        інакше
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } інакше
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                якщо (r < 0)
                        повернути -ENOMEM;
                break;

        default:
                повернути -EOPNOTSUPP;
        }

        *ret = p;
        повернути 0;
}

ціле getpeername_pretty(ціле fd, bool include_port, char **ret) {
        union sockaddr_union sa;
        socklen_t salen = sizeof(sa);
        ціле r;

        assert(fd >= 0);
        assert(ret);

        якщо (getpeername(fd, &sa.sa, &salen) < 0)
                повернути -errno;

        якщо (sa.sa.sa_family == AF_UNIX) {
                struct ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                якщо (r < 0)
                        повернути r;

                якщо (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        повернути -ENOMEM;

                повернути 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        повернути sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

ціле getsockname_pretty(ціле fd, char **ret) {
        union sockaddr_union sa;
        socklen_t salen = sizeof(sa);

        assert(fd >= 0);
        assert(ret);

        якщо (getsockname(fd, &sa.sa, &salen) < 0)
                повернути -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        повернути sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ціле socknameinfo_pretty(const struct sockaddr *sa, socklen_t salen, char **ret) {
        char host[NI_MAXHOST];
        ціле r;

        assert(sa);
        assert(salen >= sizeof(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, sizeof(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        якщо (r != 0) {
                якщо (r == EAI_MEMORY)
                        повернути log_oom_debug();
                якщо (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                інакше
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                повернути sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        повернути strdup_to(ret, host);
}

static const char* const netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ціле, INT_MAX);

bool sockaddr_equal(const union sockaddr_union *a, const union sockaddr_union *b) {
        assert(a);
        assert(b);

        якщо (a->sa.sa_family != b->sa.sa_family)
                повернути false;

        якщо (a->sa.sa_family == AF_INET)
                повернути a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        якщо (a->sa.sa_family == AF_INET6)
                повернути memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, sizeof(a->in6.sin6_addr)) == 0;

        якщо (a->sa.sa_family == AF_VSOCK)
                повернути a->vm.svm_cid == b->vm.svm_cid;

        повернути false;
}

ціле fd_set_sndbuf(ціле fd, size_t n, bool increase) {
        ціле r, value;
        socklen_t l = sizeof(value);

        якщо (n > INT_MAX)
                повернути -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        якщо (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                повернути 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        якщо (r < 0)
                повернути r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = sizeof(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        якщо (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                повернути 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        якщо (r < 0)
                повернути r;

        повернути 1;
}

ціле fd_set_rcvbuf(ціле fd, size_t n, bool increase) {
        ціле r, value;
        socklen_t l = sizeof(value);

        якщо (n > INT_MAX)
                повернути -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        якщо (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                повернути 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        якщо (r < 0)
                повернути r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = sizeof(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        якщо (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                повернути 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        якщо (r < 0)
                повернути r;

        повернути 1;
}

static const char* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ціле, 0xff);

bool ifname_valid_char(char a) {
        якщо ((unsigned char) a >= 127U)
                повернути false;

        якщо ((unsigned char) a <= 32U)
                повернути false;

        якщо (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                повернути false;

        повернути true;
}

bool ifname_valid_full(const char *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        якщо (isempty(p))
                повернути false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        якщо (parse_ifindex(p) >= 0)
                повернути flags & IFNAME_VALID_NUMERIC;

        якщо (flags & IFNAME_VALID_ALTERNATIVE) {
                якщо (strlen(p) >= ALTIFNAMSIZ)
                        повернути false;
        } інакше {
                якщо (strlen(p) >= IFNAMSIZ)
                        повернути false;
        }

        якщо (dot_or_dot_dot(p))
                повернути false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        якщо (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                повернути false;

        для (const char *t = p; *t; t++) {
                якщо (!ifname_valid_char(*t))
                        повернути false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        якщо (numeric)
                повернути false;

        повернути true;
}

bool address_label_valid(const char *p) {

        POINTER_MAY_BE_NULL(p);

        якщо (isempty(p))
                повернути false;

        якщо (strlen(p) >= IFNAMSIZ)
                повернути false;

        поки (*p) {
                якщо ((uint8_t) *p >= 127U)
                        повернути false;

                якщо ((uint8_t) *p <= 31U)
                        повернути false;
                p++;
        }

        повернути true;
}

ціле getpeercred(ціле fd, struct ucred *ucred) {
        socklen_t n = sizeof(struct ucred);
        struct ucred u;

        assert(fd >= 0);
        assert(ucred);

        якщо (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                повернути -errno;

        якщо (n != sizeof(struct ucred))
                повернути -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        якщо (!pid_is_valid(u.pid))
                повернути -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        повернути 0;
}

ціле getpeersec(ціле fd, char **ret) {
        _cleanup_free_ char *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        для (;;) {
                s = new0(char, n+1);
                якщо (!s)
                        повернути -ENOMEM;

                якщо (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        break;
                }

                якщо (errno != ERANGE)
                        повернути -errno;

                s = mfree(s);
        }

        якщо (isempty(s))
                повернути -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        повернути 0;
}

ціле getpeergroups(ціле fd, gid_t **ret) {
        socklen_t n = sizeof(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        ціле ngroups_max = sysconf_ngroups_max();
        якщо (ngroups_max > 0)
                n = MAX(n, sizeof(gid_t) * (socklen_t) ngroups_max);

        для (;;) {
                d = malloc(n);
                якщо (!d)
                        повернути -ENOMEM;

                якщо (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        break;

                якщо (errno != ERANGE)
                        повернути -errno;

                d = mfree(d);
        }

        assert_se(n % sizeof(gid_t) == 0);
        n /= sizeof(gid_t);

        якщо (n > INT_MAX)
                повернути -E2BIG;

        *ret = TAKE_PTR(d);

        повернути (ціле) n;
}

ціле getpeerpidfd(ціле fd) {
        socklen_t n = sizeof(ціле);
        ціле pidfd = -EBADF;

        assert(fd >= 0);

        якщо (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                повернути -errno;

        якщо (n != sizeof(ціле))
                повернути -EIO;

        повернути pidfd;
}

ціле getpeerpidref(ціле fd, PidRef *ret) {
        ціле r;

        assert(fd >= 0);
        assert(ret);

        ціле pidfd = getpeerpidfd(fd);
        якщо (pidfd < 0) {
                якщо (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        повернути pidfd;

                struct ucred ucred;
                r = getpeercred(fd, &ucred);
                якщо (r < 0)
                        повернути r;

                повернути pidref_set_pid(ret, ucred.pid);
        }

        повернути pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ціле transport_fd,
                ціле fd,
                const struct iovec *iov, size_t iovlen,
                const struct sockaddr *sa, socklen_t len,
                ціле flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(sizeof(ціле))) control = {};
        struct msghdr mh = {
                .msg_name = (struct sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (struct iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        assert(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        якщо (fd < 0 && !iov)
                повернути -EINVAL;

        якщо (fd >= 0) {
                struct cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = sizeof(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(sizeof(ціле));
                memcpy(CMSG_DATA(cmsg), &fd, sizeof(ціле));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        якщо (k < 0)
                повернути (ssize_t) -errno;

        повернути k;
}

ціле send_one_fd_sa(
                ціле transport_fd,
                ціле fd,
                const struct sockaddr *sa, socklen_t len,
                ціле flags) {

        assert(fd >= 0);

        повернути (ціле) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ціле transport_fd,
                struct iovec *iov, size_t iovlen,
                ціле flags,
                ціле *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(sizeof(ціле))) control;
        struct msghdr mh = {
                .msg_control = &control,
                .msg_controllen = sizeof(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        struct cmsghdr *found;
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
        якщо (k < 0)
                повернути k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(sizeof(ціле)));
        якщо (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                якщо (k == 0)
                        повернути -EIO;
        }

        якщо (found)
                *ret_fd = *CMSG_TYPED_DATA(found, ціле);
        інакше
                *ret_fd = -EBADF;

        повернути k;
}

ціле receive_one_fd(ціле transport_fd, ціле flags) {
        ціле fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        якщо (k == 0)
                повернути fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        повернути (ціле) k;
}

ssize_t next_datagram_size_fd(ціле fd) {
        ssize_t l;
        ціле k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        якщо (l < 0) {
                якщо (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                повернути -errno;
        }
        якщо (l == 0)
                goto fallback;

        повернути l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        якщо (ioctl(fd, FIONREAD, &k) < 0)
                повернути -errno;

        повернути (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

ціле flush_accept(ціле fd) {

        ціле r, b;
        socklen_t l = sizeof(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        якщо (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                повернути -errno;

        assert(l == sizeof(b));
        якщо (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                повернути -ENOTTY;

        для (unsigned iteration = 0;; iteration++) {
                ціле cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                якщо (r == -EINTR)
                        continue;
                якщо (r <= 0)
                        повернути r;

                якщо (iteration >= MAX_FLUSH_ITERATIONS)
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                якщо (cfd < 0) {
                        якщо (errno == EAGAIN)
                                повернути 0;

                        якщо (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        повернути -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(ціле fd) {
        _cleanup_free_ char *buf = NULL;
        struct mq_attr attr;
        ssize_t count = 0;
        ціле r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        для (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                якщо (r == -EINTR)
                        continue;
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        повернути count;

                якщо (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        якщо (mq_getattr(fd, &attr) < 0)
                                повернути -errno;

                        buf = malloc(attr.mq_msgsize);
                        якщо (!buf)
                                повернути -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                якщо (l < 0) {
                        якщо (errno == EINTR)
                                continue;

                        якщо (errno == EAGAIN)
                                повернути count;

                        повернути -errno;
                }

                count += l;
        }
}

struct cmsghdr* cmsg_find(struct msghdr *mh, ціле level, ціле type, socklen_t length) {
        struct cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                якщо (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        повернути cmsg;

        повернути NULL;
}

void* cmsg_find_and_copy_data(struct msghdr *mh, ціле level, ціле type, void *buf, size_t buf_len) {
        struct cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        якщо (!cmsg)
                повернути NULL;

        повернути memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const struct sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = sizeof(sa->sll_addr);

        якщо (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        якщо (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        повернути offsetof(struct sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const struct sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        повернути offsetof(struct sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, sizeof(sa->sun_path)-1) :
                        strnlen(sa->sun_path, sizeof(sa->sun_path))+1);
}

size_t sockaddr_len(const union sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        case AF_INET:
                повернути sizeof(struct sockaddr_in);
        case AF_INET6:
                повернути sizeof(struct sockaddr_in6);
        case AF_UNIX:
                повернути sockaddr_un_len(&sa->un);
        case AF_PACKET:
                повернути sockaddr_ll_len(&sa->ll);
        case AF_NETLINK:
                повернути sizeof(struct sockaddr_nl);
        case AF_VSOCK:
                повернути sizeof(struct sockaddr_vm);
        default:
                assert_not_reached();
        }
}

ціле socket_ioctl_fd(void) {
        ціле fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        якщо (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        якщо (fd < 0)
                повернути -errno;

        повернути fd;
}

ціле sockaddr_un_unlink(const struct sockaddr_un *sa) {
        const char *p, * nul;

        assert(sa);

        якщо (sa->sun_family != AF_UNIX)
                повернути -EPROTOTYPE;

        якщо (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                повернути 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, sizeof(sa->sun_path));
        якщо (nul)
                p = sa->sun_path;
        інакше
                p = memdupa_suffix0(sa->sun_path, sizeof(sa->sun_path));

        якщо (unlink(p) < 0)
                повернути -errno;

        повернути 1;
}

ціле sockaddr_un_set_path(struct sockaddr_un *ret, const char *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        якщо (l < 2)
                повернути -EINVAL;
        якщо (!IN_SET(path[0], '/', '@'))
                повернути -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        якщо (l+1 > sizeof(ret->sun_path))
                повернути path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (struct sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        якщо (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                повернути (ціле) (offsetof(struct sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } інакше {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                повернути (ціле) (offsetof(struct sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ціле getsockopt_int(ціле fd, ціле level, ціле optname, ціле *ret) {
        ціле v;
        socklen_t sl = sizeof(v);

        assert(fd >= 0);
        assert(ret);

        якщо (getsockopt(fd, level, optname, &v, &sl) < 0)
                повернути negative_errno();
        якщо (sl != sizeof(v))
                повернути -EIO;

        *ret = v;
        повернути 0;
}

ціле socket_bind_to_ifname(ціле fd, const char *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        повернути RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ціле socket_bind_to_ifindex(ціле fd, ціле ifindex) {
        assert(fd >= 0);

        якщо (ifindex <= 0)
                /* Drop binding */
                повернути RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        повернути setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ціле socket_autobind(ціле fd, char **ret_name) {
        _cleanup_free_ char *name = NULL;
        uint64_t random;
        ціле r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        якщо (asprintf(&name, "@%" PRIu64, random) < 0)
                повернути -ENOMEM;

        union sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < sizeof(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        якщо (r < 0)
                повернути r;

        якщо (bind(fd, &sa.sa, r) < 0)
                повернути -errno;

        якщо (ret_name)
                *ret_name = TAKE_PTR(name);
        повернути 0;
}

ssize_t recvmsg_safe(ціле sockfd, struct msghdr *msg, ціле flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        якщо (n < 0)
                повернути -errno;

        якщо (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                повернути FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        повернути n;
}

ціле socket_get_family(ціле fd) {
        ціле af;
        socklen_t sl = sizeof(af);

        якщо (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                повернути -errno;

        якщо (sl != sizeof(af))
                повернути -EINVAL;

        повернути af;
}

ціле socket_set_recvpktinfo(ціле fd, ціле af, bool b) {

        якщо (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                якщо (af < 0)
                        повернути af;
        }

        switch (af) {

        case AF_INET:
                повернути setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        case AF_INET6:
                повернути setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        case AF_NETLINK:
                повернути setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        case AF_PACKET:
                повернути setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                повернути -EAFNOSUPPORT;
        }
}

ціле socket_set_unicast_if(ціле fd, ціле af, ціле ifi) {
        be32_t ifindex_be = htobe32(ifi);

        якщо (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                якщо (af < 0)
                        повернути af;
        }

        switch (af) {

        case AF_INET:
                повернути RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, sizeof(ifindex_be)));

        case AF_INET6:
                повернути RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, sizeof(ifindex_be)));

        default:
                повернути -EAFNOSUPPORT;
        }
}

ціле socket_set_option(ціле fd, ціле af, ціле opt_ipv4, ціле opt_ipv6, ціле val) {
        якщо (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                якщо (af < 0)
                        повернути af;
        }

        switch (af) {

        case AF_INET:
                повернути setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        case AF_INET6:
                повернути setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                повернути -EAFNOSUPPORT;
        }
}

ціле socket_get_mtu(ціле fd, ціле af, size_t *ret) {
        ціле mtu, r;

        assert(ret);

        якщо (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                якщо (af < 0)
                        повернути af;
        }

        switch (af) {

        case AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                break;

        case AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                break;

        default:
                повернути -EAFNOSUPPORT;
        }

        якщо (r < 0)
                повернути r;
        якщо (mtu <= 0)
                повернути -EINVAL;

        *ret = (size_t) mtu;
        повернути 0;
}

static ціле connect_unix_path_simple(ціле fd, const char *path) {
        union sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        assert(fd >= 0);
        assert(path);

        l = strlen(path);
        assert(l > 0);
        assert(l < sizeof(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        повернути RET_NERRNO(connect(fd, &sa.sa, offsetof(struct sockaddr_un, sun_path) + l + 1));
}

static ціле connect_unix_inode(ціле fd, ціле inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        повернути connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

ціле connect_unix_path(ціле fd, ціле dir_fd, const char *path) {
        _cleanup_close_ ціле inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        якщо (!path) {
                якщо (dir_fd < 0)
                        повернути -EISDIR;

                повернути connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        якщо (isempty(path))
                повернути -EINVAL;

        /* Shortcut for the simple case */
        якщо (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(struct sockaddr_un, sun_path))
                повернути connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        якщо (dir_fd == XAT_FDROOT) {
                _cleanup_free_ char *j = strjoin("/", path);
                якщо (!j)
                        повернути -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } інакше
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        якщо (inode_fd < 0)
                повернути -errno;

        повернути connect_unix_inode(fd, inode_fd);
}

ціле socket_address_parse_unix(SocketAddress *ret_address, const char *s) {
        struct sockaddr_un un;
        ціле r;

        assert(ret_address);
        assert(s);

        якщо (!IN_SET(*s, '/', '@'))
                повернути -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        якщо (r < 0)
                повернути r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        повернути 0;
}

ціле socket_address_equal_unix(const char *a, const char *b) {
        SocketAddress socket_a, socket_b;
        ціле r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        якщо (r < 0)
                повернути r;

        r = socket_address_parse_unix(&socket_b, b);
        якщо (r < 0)
                повернути r;

        повернути sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ціле vsock_parse_port(const char *s, unsigned *ret) {
        ціле r;

        assert(ret);

        якщо (!s)
                повернути -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        якщо (r < 0)
                повернути r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        якщо (u == VMADDR_PORT_ANY)
                повернути -EINVAL;

        *ret = u;
        повернути 0;
}

ціле vsock_parse_cid(const char *s, unsigned *ret) {
        assert(ret);

        якщо (!s)
                повернути -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        якщо (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        інакше якщо (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        інакше якщо (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        інакше якщо (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        інакше
                повернути safe_atou(s, ret);

        повернути 0;
}

ціле socket_address_parse_vsock(SocketAddress *ret_address, const char *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ char *n = NULL;
        const char *e, *cid_start;
        unsigned port, cid;
        ціле type, r;

        assert(ret_address);
        assert(s);

        якщо ((cid_start = startswith(s, "vsock:")))
                type = 0;
        інакше якщо ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        інакше якщо ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        інакше якщо ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        інакше
                повернути -EPROTO;

        e = strchr(cid_start, ':');
        якщо (!e)
                повернути -EINVAL;

        r = vsock_parse_port(e+1, &port);
        якщо (r < 0)
                повернути r;

        n = strndup(cid_start, e - cid_start);
        якщо (!n)
                повернути -ENOMEM;

        якщо (isempty(n))
                cid = VMADDR_CID_ANY;
        інакше {
                r = vsock_parse_cid(n, &cid);
                якщо (r < 0)
                        повернути r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = sizeof(struct sockaddr_vm),
        };

        повернути 0;
}

ціле netlink_socket_get_multicast_groups(ціле fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        якщо (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                повернути -errno;

        якщо (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        якщо (!groups)
                повернути -ENOMEM;

        old_len = len;

        якщо (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                повернути -errno;

        якщо (old_len != len)
                повернути -EIO;

finalize:
        якщо (ret_len)
                *ret_len = len;
        якщо (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        повернути 0;
}

ціле socket_get_cookie(ціле fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = sizeof(cookie);
        якщо (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                повернути -errno;

        assert(cookie_len == sizeof(cookie));
        якщо (ret)
                *ret = cookie;

        повернути 0;
}

void cmsg_close_all(struct msghdr *mh) {
        assert(mh);

        struct cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                якщо (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                якщо (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, ціле),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(ціле));
                інакше якщо (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(sizeof(ціле)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, ціле));
                }
        }
}

ціле tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        case IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        case IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                повернути TC_PRIO_CONTROL;

        case IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        case IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                повернути TC_PRIO_INTERACTIVE;

        case IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        case IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                повернути TC_PRIO_INTERACTIVE_BULK;

        case IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                повернути TC_PRIO_BULK;

        case IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                повернути TC_PRIO_BESTEFFORT;
        }
}

ціле socket_xattr_supported(void) {
        ціле r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static ціле cached = -1;
        якщо (cached >= 0)
                повернути cached;

        _cleanup_close_ ціле fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        якщо (fd < 0)
                повернути -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                повернути (cached = false);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to set test xattr on socket: %m");

        повернути (cached = true);
}
