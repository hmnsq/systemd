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

#ако ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#иначе
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

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, цяло);

цяло socket_address_verify(const SocketAddress *a, bool strict) {
        assert(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        switch (socket_address_family(a)) {

        case AF_INET:
                ако (a->size != sizeof(struct sockaddr_in))
                        върни -EINVAL;

                ако (a->sockaddr.in.sin_port == 0)
                        върни -EINVAL;

                ако (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        върни -EINVAL;

                върни 0;

        case AF_INET6:
                ако (a->size != sizeof(struct sockaddr_in6))
                        върни -EINVAL;

                ако (a->sockaddr.in6.sin6_port == 0)
                        върни -EINVAL;

                ако (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        върни -EINVAL;

                върни 0;

        case AF_UNIX:
                ако (a->size < offsetof(struct sockaddr_un, sun_path))
                        върни -EINVAL;
                ако (a->size > sizeof(struct sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        върни -EINVAL;

                ако (a->size > offsetof(struct sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        const char *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, sizeof(a->sockaddr.un.sun_path));
                        ако (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ако (a->size != offsetof(struct sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        върни -EINVAL;
                        } иначе {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ако (!IN_SET(a->size, sizeof(a->sockaddr.un.sun_path), sizeof(a->sockaddr.un.sun_path)+1))
                                        върни -EINVAL;
                        }
                }

                ако (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        върни -EINVAL;

                върни 0;

        case AF_NETLINK:

                ако (a->size != sizeof(struct sockaddr_nl))
                        върни -EINVAL;

                ако (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        върни -EINVAL;

                върни 0;

        case AF_VSOCK:
                ако (a->size != sizeof(struct sockaddr_vm))
                        върни -EINVAL;

                ако (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        върни -EINVAL;

                върни 0;

        default:
                върни -EAFNOSUPPORT;
        }
}

цяло socket_address_print(const SocketAddress *a, char **ret) {
        цяло r;

        assert(a);
        assert(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ако (r < 0)
                върни r;

        ако (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ char *sfamily = NULL;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ако (r < 0)
                        върни r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ако (r < 0)
                        върни -ENOMEM;

                върни 0;
        }

        върни sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(const SocketAddress *a) {
        assert(a);

        върни
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) {
        assert(a);
        assert(b);

        /* Invalid addresses are unequal to all */
        ако (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                върни false;

        ако (a->type != b->type)
                върни false;

        ако (socket_address_family(a) != socket_address_family(b))
                върни false;

        switch (socket_address_family(a)) {

        case AF_INET:
                ако (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        върни false;

                ако (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        върни false;

                break;

        case AF_INET6:
                ако (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, sizeof(a->sockaddr.in6.sin6_addr)) != 0)
                        върни false;

                ако (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        върни false;

                break;

        case AF_UNIX:
                ако (a->size <= offsetof(struct sockaddr_un, sun_path) ||
                    b->size <= offsetof(struct sockaddr_un, sun_path))
                        върни false;

                ако ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        върни false;

                ако (a->sockaddr.un.sun_path[0]) {
                        ако (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                върни false;
                } иначе {
                        ако (a->size != b->size)
                                върни false;

                        ако (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                върни false;
                }

                break;

        case AF_NETLINK:
                ако (a->protocol != b->protocol)
                        върни false;

                ако (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        върни false;

                break;

        case AF_VSOCK:
                ако (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        върни false;

                ако (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        върни false;

                break;

        default:
                /* Cannot compare, so we assume the addresses are different */
                върни false;
        }

        върни true;
}

const char* socket_address_get_path(const SocketAddress *a) {
        assert(a);

        ако (socket_address_family(a) != AF_UNIX)
                върни NULL;

        ако (a->sockaddr.un.sun_path[0] == 0)
                върни NULL;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(sizeof(union sockaddr_union) >= sizeof(struct sockaddr_un)+1);
        върни a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(void) {
        static цяло cached = -1;

        ако (cached < 0) {

                ако (access("/proc/net/if_inet6", F_OK) < 0) {

                        ако (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                върни false;
                        }

                        cached = false;
                } иначе
                        cached = true;
        }

        върни cached;
}

bool socket_ipv6_is_enabled(void) {
        _cleanup_free_ char *v = NULL;
        цяло r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ако (!socket_ipv6_is_supported())
                върни false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ако (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                върни true;
        }

        r = parse_boolean(v);
        ако (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                върни true;
        }

        върни !r;
}

bool socket_address_matches_fd(const SocketAddress *a, цяло fd) {
        SocketAddress b;
        socklen_t solen;

        assert(a);
        assert(fd >= 0);

        b.size = sizeof(b.sockaddr);
        ако (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                върни false;

        ако (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                върни false;

        solen = sizeof(b.type);
        ако (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                върни false;

        ако (b.type != a->type)
                върни false;

        ако (a->protocol != 0)  {
                solen = sizeof(b.protocol);
                ако (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        върни false;

                ако (b.protocol != a->protocol)
                        върни false;
        }

        върни socket_address_equal(a, &b);
}

цяло sockaddr_port(const struct sockaddr *_sa, unsigned *ret_port) {
        const union sockaddr_union *sa = (const union sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        assert(sa);
        assert(ret_port);

        switch (sa->sa.sa_family) {

        case AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                върни 0;

        case AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                върни 0;

        case AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                върни 0;

        default:
                върни -EAFNOSUPPORT;
        }
}

const union in_addr_union *sockaddr_in_addr(const struct sockaddr *_sa) {
        const union sockaddr_union *sa = (const union sockaddr_union*) _sa;

        ако (!sa)
                върни NULL;

        switch (sa->sa.sa_family) {

        case AF_INET:
                върни (const union in_addr_union*) &sa->in.sin_addr;

        case AF_INET6:
                върни (const union in_addr_union*) &sa->in6.sin6_addr;

        default:
                върни NULL;
        }
}

цяло sockaddr_set_in_addr(
                union sockaddr_union *u,
                цяло family,
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

                върни 0;

        case AF_INET6:
                u->in6 = (struct sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                върни 0;

        default:
                върни -EAFNOSUPPORT;

        }
}

цяло sockaddr_pretty(
                const struct sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                char **ret) {

        union sockaddr_union *sa = (union sockaddr_union*) _sa;
        char *p;
        цяло r;

        assert(sa);
        assert(salen >= sizeof(sa->sa.sa_family));
        assert(ret);

        switch (sa->sa.sa_family) {

        case AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ако (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                иначе
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ако (r < 0)
                        върни -ENOMEM;
                break;
        }

        case AF_INET6: {
                static const unsigned char ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ако (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, sizeof(ipv4_prefix)) == 0) {
                        const uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ако (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        иначе
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ако (r < 0)
                                върни -ENOMEM;
                } иначе {
                        const char *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ако (include_port) {
                                ако (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        върни -ENOMEM;
                        } иначе {
                                ако (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                иначе
                                        p = strdup(a);
                                ако (!p)
                                        върни -ENOMEM;
                        }
                }

                break;
        }

        case AF_UNIX:
                ако (salen <= offsetof(struct sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(struct sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                иначе {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        char *path = (char*) sa->un_buffer + offsetof(struct sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(struct sockaddr_un, sun_path);

                        ако (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ char *e = NULL;

                                e = cescape_length(path + 1, path_len - 1);
                                ако (!e)
                                        върни -ENOMEM;

                                p = strjoin("@", e);
                        } иначе {
                                ако (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ако (!p)
                        върни -ENOMEM;

                break;

        case AF_VSOCK:
                ако (include_port) {
                        ако (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        иначе
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } иначе
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ако (r < 0)
                        върни -ENOMEM;
                break;

        default:
                върни -EOPNOTSUPP;
        }

        *ret = p;
        върни 0;
}

цяло getpeername_pretty(цяло fd, bool include_port, char **ret) {
        union sockaddr_union sa;
        socklen_t salen = sizeof(sa);
        цяло r;

        assert(fd >= 0);
        assert(ret);

        ако (getpeername(fd, &sa.sa, &salen) < 0)
                върни -errno;

        ако (sa.sa.sa_family == AF_UNIX) {
                struct ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ако (r < 0)
                        върни r;

                ако (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        върни -ENOMEM;

                върни 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        върни sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

цяло getsockname_pretty(цяло fd, char **ret) {
        union sockaddr_union sa;
        socklen_t salen = sizeof(sa);

        assert(fd >= 0);
        assert(ret);

        ако (getsockname(fd, &sa.sa, &salen) < 0)
                върни -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        върни sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

цяло socknameinfo_pretty(const struct sockaddr *sa, socklen_t salen, char **ret) {
        char host[NI_MAXHOST];
        цяло r;

        assert(sa);
        assert(salen >= sizeof(sa_family_t));
        assert(ret);

        r = getnameinfo(sa, salen, host, sizeof(host), /* service= */ NULL, /* service_len= */ 0, IDN_FLAGS);
        ако (r != 0) {
                ако (r == EAI_MEMORY)
                        върни log_oom_debug();
                ако (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                иначе
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                върни sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        върни strdup_to(ret, host);
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, цяло, INT_MAX);

bool sockaddr_equal(const union sockaddr_union *a, const union sockaddr_union *b) {
        assert(a);
        assert(b);

        ако (a->sa.sa_family != b->sa.sa_family)
                върни false;

        ако (a->sa.sa_family == AF_INET)
                върни a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ако (a->sa.sa_family == AF_INET6)
                върни memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, sizeof(a->in6.sin6_addr)) == 0;

        ако (a->sa.sa_family == AF_VSOCK)
                върни a->vm.svm_cid == b->vm.svm_cid;

        върни false;
}

цяло fd_set_sndbuf(цяло fd, size_t n, bool increase) {
        цяло r, value;
        socklen_t l = sizeof(value);

        ако (n > INT_MAX)
                върни -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ако (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                върни 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ако (r < 0)
                върни r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = sizeof(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ако (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                върни 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ако (r < 0)
                върни r;

        върни 1;
}

цяло fd_set_rcvbuf(цяло fd, size_t n, bool increase) {
        цяло r, value;
        socklen_t l = sizeof(value);

        ако (n > INT_MAX)
                върни -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ако (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                върни 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ако (r < 0)
                върни r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = sizeof(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ако (r >= 0 && l == sizeof(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                върни 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ако (r < 0)
                върни r;

        върни 1;
}

static const char* const ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, цяло, 0xff);

bool ifname_valid_char(char a) {
        ако ((unsigned char) a >= 127U)
                върни false;

        ако ((unsigned char) a <= 32U)
                върни false;

        ако (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                върни false;

        върни true;
}

bool ifname_valid_full(const char *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        assert(!(flags & ~_IFNAME_VALID_ALL));

        ако (isempty(p))
                върни false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ако (parse_ifindex(p) >= 0)
                върни flags & IFNAME_VALID_NUMERIC;

        ако (flags & IFNAME_VALID_ALTERNATIVE) {
                ако (strlen(p) >= ALTIFNAMSIZ)
                        върни false;
        } иначе {
                ако (strlen(p) >= IFNAMSIZ)
                        върни false;
        }

        ако (dot_or_dot_dot(p))
                върни false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ако (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                върни false;

        за (const char *t = p; *t; t++) {
                ако (!ifname_valid_char(*t))
                        върни false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ако (numeric)
                върни false;

        върни true;
}

bool address_label_valid(const char *p) {

        POINTER_MAY_BE_NULL(p);

        ако (isempty(p))
                върни false;

        ако (strlen(p) >= IFNAMSIZ)
                върни false;

        докато (*p) {
                ако ((uint8_t) *p >= 127U)
                        върни false;

                ако ((uint8_t) *p <= 31U)
                        върни false;
                p++;
        }

        върни true;
}

цяло getpeercred(цяло fd, struct ucred *ucred) {
        socklen_t n = sizeof(struct ucred);
        struct ucred u;

        assert(fd >= 0);
        assert(ucred);

        ако (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                върни -errno;

        ако (n != sizeof(struct ucred))
                върни -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ако (!pid_is_valid(u.pid))
                върни -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        върни 0;
}

цяло getpeersec(цяло fd, char **ret) {
        _cleanup_free_ char *s = NULL;
        socklen_t n = 64;

        assert(fd >= 0);
        assert(ret);

        за (;;) {
                s = new0(char, n+1);
                ако (!s)
                        върни -ENOMEM;

                ако (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        break;
                }

                ако (errno != ERANGE)
                        върни -errno;

                s = mfree(s);
        }

        ако (isempty(s))
                върни -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        върни 0;
}

цяло getpeergroups(цяло fd, gid_t **ret) {
        socklen_t n = sizeof(gid_t) * 64U;
        _cleanup_free_ gid_t *d = NULL;

        assert(fd >= 0);
        assert(ret);

        цяло ngroups_max = sysconf_ngroups_max();
        ако (ngroups_max > 0)
                n = MAX(n, sizeof(gid_t) * (socklen_t) ngroups_max);

        за (;;) {
                d = malloc(n);
                ако (!d)
                        върни -ENOMEM;

                ако (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        break;

                ако (errno != ERANGE)
                        върни -errno;

                d = mfree(d);
        }

        assert_se(n % sizeof(gid_t) == 0);
        n /= sizeof(gid_t);

        ако (n > INT_MAX)
                върни -E2BIG;

        *ret = TAKE_PTR(d);

        върни (цяло) n;
}

цяло getpeerpidfd(цяло fd) {
        socklen_t n = sizeof(цяло);
        цяло pidfd = -EBADF;

        assert(fd >= 0);

        ако (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                върни -errno;

        ако (n != sizeof(цяло))
                върни -EIO;

        върни pidfd;
}

цяло getpeerpidref(цяло fd, PidRef *ret) {
        цяло r;

        assert(fd >= 0);
        assert(ret);

        цяло pidfd = getpeerpidfd(fd);
        ако (pidfd < 0) {
                ако (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        върни pidfd;

                struct ucred ucred;
                r = getpeercred(fd, &ucred);
                ако (r < 0)
                        върни r;

                върни pidref_set_pid(ret, ucred.pid);
        }

        върни pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                цяло transport_fd,
                цяло fd,
                const struct iovec *iov, size_t iovlen,
                const struct sockaddr *sa, socklen_t len,
                цяло flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(sizeof(цяло))) control = {};
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
        ако (fd < 0 && !iov)
                върни -EINVAL;

        ако (fd >= 0) {
                struct cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = sizeof(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(sizeof(цяло));
                memcpy(CMSG_DATA(cmsg), &fd, sizeof(цяло));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ако (k < 0)
                върни (ssize_t) -errno;

        върни k;
}

цяло send_one_fd_sa(
                цяло transport_fd,
                цяло fd,
                const struct sockaddr *sa, socklen_t len,
                цяло flags) {

        assert(fd >= 0);

        върни (цяло) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                цяло transport_fd,
                struct iovec *iov, size_t iovlen,
                цяло flags,
                цяло *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(sizeof(цяло))) control;
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
        ако (k < 0)
                върни k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(sizeof(цяло)));
        ако (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ако (k == 0)
                        върни -EIO;
        }

        ако (found)
                *ret_fd = *CMSG_TYPED_DATA(found, цяло);
        иначе
                *ret_fd = -EBADF;

        върни k;
}

цяло receive_one_fd(цяло transport_fd, цяло flags) {
        цяло fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, NULL, 0, flags, &fd);
        ако (k == 0)
                върни fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        assert(k < 0);
        върни (цяло) k;
}

ssize_t next_datagram_size_fd(цяло fd) {
        ssize_t l;
        цяло k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, NULL, 0, MSG_PEEK|MSG_TRUNC);
        ако (l < 0) {
                ако (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        goto fallback;

                върни -errno;
        }
        ако (l == 0)
                goto fallback;

        върни l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ако (ioctl(fd, FIONREAD, &k) < 0)
                върни -errno;

        върни (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

цяло flush_accept(цяло fd) {

        цяло r, b;
        socklen_t l = sizeof(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ако (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                върни -errno;

        assert(l == sizeof(b));
        ако (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                върни -ENOTTY;

        за (unsigned iteration = 0;; iteration++) {
                цяло cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ако (r == -EINTR)
                        continue;
                ако (r <= 0)
                        върни r;

                ако (iteration >= MAX_FLUSH_ITERATIONS)
                        върни log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ако (cfd < 0) {
                        ако (errno == EAGAIN)
                                върни 0;

                        ако (ERRNO_IS_ACCEPT_AGAIN(errno))
                                continue;

                        върни -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(цяло fd) {
        _cleanup_free_ char *buf = NULL;
        struct mq_attr attr;
        ssize_t count = 0;
        цяло r;

        assert(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        за (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ако (r == -EINTR)
                        continue;
                ако (r < 0)
                        върни r;
                ако (r == 0)
                        върни count;

                ако (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ако (mq_getattr(fd, &attr) < 0)
                                върни -errno;

                        buf = malloc(attr.mq_msgsize);
                        ако (!buf)
                                върни -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ NULL);
                ако (l < 0) {
                        ако (errno == EINTR)
                                continue;

                        ако (errno == EAGAIN)
                                върни count;

                        върни -errno;
                }

                count += l;
        }
}

struct cmsghdr* cmsg_find(struct msghdr *mh, цяло level, цяло type, socklen_t length) {
        struct cmsghdr *cmsg;

        assert(mh);

        CMSG_FOREACH(cmsg, mh)
                ако (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        върни cmsg;

        върни NULL;
}

void* cmsg_find_and_copy_data(struct msghdr *mh, цяло level, цяло type, void *buf, size_t buf_len) {
        struct cmsghdr *cmsg;

        assert(mh);
        assert(buf);
        assert(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ако (!cmsg)
                върни NULL;

        върни memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(const struct sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        assert(sa->sll_family == AF_PACKET);

        size_t mac_len = sizeof(sa->sll_addr);

        ако (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ако (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        върни offsetof(struct sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(const struct sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        assert(sa->sun_family == AF_UNIX);

        върни offsetof(struct sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, sizeof(sa->sun_path)-1) :
                        strnlen(sa->sun_path, sizeof(sa->sun_path))+1);
}

size_t sockaddr_len(const union sockaddr_union *sa) {
        assert(sa);

        switch (sa->sa.sa_family) {
        case AF_INET:
                върни sizeof(struct sockaddr_in);
        case AF_INET6:
                върни sizeof(struct sockaddr_in6);
        case AF_UNIX:
                върни sockaddr_un_len(&sa->un);
        case AF_PACKET:
                върни sockaddr_ll_len(&sa->ll);
        case AF_NETLINK:
                върни sizeof(struct sockaddr_nl);
        case AF_VSOCK:
                върни sizeof(struct sockaddr_vm);
        default:
                assert_not_reached();
        }
}

цяло socket_ioctl_fd(void) {
        цяло fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ако (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ако (fd < 0)
                върни -errno;

        върни fd;
}

цяло sockaddr_un_unlink(const struct sockaddr_un *sa) {
        const char *p, * nul;

        assert(sa);

        ако (sa->sun_family != AF_UNIX)
                върни -EPROTOTYPE;

        ако (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                върни 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, sizeof(sa->sun_path));
        ако (nul)
                p = sa->sun_path;
        иначе
                p = memdupa_suffix0(sa->sun_path, sizeof(sa->sun_path));

        ако (unlink(p) < 0)
                върни -errno;

        върни 1;
}

цяло sockaddr_un_set_path(struct sockaddr_un *ret, const char *path) {
        size_t l;

        assert(ret);
        assert(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ако (l < 2)
                върни -EINVAL;
        ако (!IN_SET(path[0], '/', '@'))
                върни -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ако (l+1 > sizeof(ret->sun_path))
                върни path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (struct sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ако (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                върни (цяло) (offsetof(struct sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } иначе {
                assert(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                върни (цяло) (offsetof(struct sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

цяло getsockopt_int(цяло fd, цяло level, цяло optname, цяло *ret) {
        цяло v;
        socklen_t sl = sizeof(v);

        assert(fd >= 0);
        assert(ret);

        ако (getsockopt(fd, level, optname, &v, &sl) < 0)
                върни negative_errno();
        ако (sl != sizeof(v))
                върни -EIO;

        *ret = v;
        върни 0;
}

цяло socket_bind_to_ifname(цяло fd, const char *ifname) {
        assert(fd >= 0);

        /* Call with NULL to drop binding */

        върни RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

цяло socket_bind_to_ifindex(цяло fd, цяло ifindex) {
        assert(fd >= 0);

        ако (ifindex <= 0)
                /* Drop binding */
                върни RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, NULL, 0));

        върни setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

цяло socket_autobind(цяло fd, char **ret_name) {
        _cleanup_free_ char *name = NULL;
        uint64_t random;
        цяло r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        assert(fd >= 0);

        random = random_u64();

        ако (asprintf(&name, "@%" PRIu64, random) < 0)
                върни -ENOMEM;

        union sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < sizeof(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ако (r < 0)
                върни r;

        ако (bind(fd, &sa.sa, r) < 0)
                върни -errno;

        ако (ret_name)
                *ret_name = TAKE_PTR(name);
        върни 0;
}

ssize_t recvmsg_safe(цяло sockfd, struct msghdr *msg, цяло flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        assert(sockfd >= 0);
        assert(msg);

        n = recvmsg(sockfd, msg, flags);
        ако (n < 0)
                върни -errno;

        ако (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                върни FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        върни n;
}

цяло socket_get_family(цяло fd) {
        цяло af;
        socklen_t sl = sizeof(af);

        ако (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                върни -errno;

        ако (sl != sizeof(af))
                върни -EINVAL;

        върни af;
}

цяло socket_set_recvpktinfo(цяло fd, цяло af, bool b) {

        ако (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ако (af < 0)
                        върни af;
        }

        switch (af) {

        case AF_INET:
                върни setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        case AF_INET6:
                върни setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        case AF_NETLINK:
                върни setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        case AF_PACKET:
                върни setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        default:
                върни -EAFNOSUPPORT;
        }
}

цяло socket_set_unicast_if(цяло fd, цяло af, цяло ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ако (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ако (af < 0)
                        върни af;
        }

        switch (af) {

        case AF_INET:
                върни RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, sizeof(ifindex_be)));

        case AF_INET6:
                върни RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, sizeof(ifindex_be)));

        default:
                върни -EAFNOSUPPORT;
        }
}

цяло socket_set_option(цяло fd, цяло af, цяло opt_ipv4, цяло opt_ipv6, цяло val) {
        ако (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ако (af < 0)
                        върни af;
        }

        switch (af) {

        case AF_INET:
                върни setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        case AF_INET6:
                върни setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        default:
                върни -EAFNOSUPPORT;
        }
}

цяло socket_get_mtu(цяло fd, цяло af, size_t *ret) {
        цяло mtu, r;

        assert(ret);

        ако (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ако (af < 0)
                        върни af;
        }

        switch (af) {

        case AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                break;

        case AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                break;

        default:
                върни -EAFNOSUPPORT;
        }

        ако (r < 0)
                върни r;
        ако (mtu <= 0)
                върни -EINVAL;

        *ret = (size_t) mtu;
        върни 0;
}

static цяло connect_unix_path_simple(цяло fd, const char *path) {
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
        върни RET_NERRNO(connect(fd, &sa.sa, offsetof(struct sockaddr_un, sun_path) + l + 1));
}

static цяло connect_unix_inode(цяло fd, цяло inode_fd) {
        assert(fd >= 0);
        assert(inode_fd >= 0);

        върни connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

цяло connect_unix_path(цяло fd, цяло dir_fd, const char *path) {
        _cleanup_close_ цяло inode_fd = -EBADF;

        assert(fd >= 0);
        assert(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ако (!path) {
                ако (dir_fd < 0)
                        върни -EISDIR;

                върни connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ако (isempty(path))
                върни -EINVAL;

        /* Shortcut for the simple case */
        ако (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(struct sockaddr_un, sun_path))
                върни connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ако (dir_fd == XAT_FDROOT) {
                _cleanup_free_ char *j = strjoin("/", path);
                ако (!j)
                        върни -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } иначе
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ако (inode_fd < 0)
                върни -errno;

        върни connect_unix_inode(fd, inode_fd);
}

цяло socket_address_parse_unix(SocketAddress *ret_address, const char *s) {
        struct sockaddr_un un;
        цяло r;

        assert(ret_address);
        assert(s);

        ако (!IN_SET(*s, '/', '@'))
                върни -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ако (r < 0)
                върни r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        върни 0;
}

цяло socket_address_equal_unix(const char *a, const char *b) {
        SocketAddress socket_a, socket_b;
        цяло r;

        assert(a);
        assert(b);

        r = socket_address_parse_unix(&socket_a, a);
        ако (r < 0)
                върни r;

        r = socket_address_parse_unix(&socket_b, b);
        ако (r < 0)
                върни r;

        върни sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

цяло vsock_parse_port(const char *s, unsigned *ret) {
        цяло r;

        assert(ret);

        ако (!s)
                върни -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ако (r < 0)
                върни r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ако (u == VMADDR_PORT_ANY)
                върни -EINVAL;

        *ret = u;
        върни 0;
}

цяло vsock_parse_cid(const char *s, unsigned *ret) {
        assert(ret);

        ако (!s)
                върни -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ако (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        иначе ако (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        иначе ако (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        иначе ако (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        иначе
                върни safe_atou(s, ret);

        върни 0;
}

цяло socket_address_parse_vsock(SocketAddress *ret_address, const char *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ char *n = NULL;
        const char *e, *cid_start;
        unsigned port, cid;
        цяло type, r;

        assert(ret_address);
        assert(s);

        ако ((cid_start = startswith(s, "vsock:")))
                type = 0;
        иначе ако ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        иначе ако ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        иначе ако ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        иначе
                върни -EPROTO;

        e = strchr(cid_start, ':');
        ако (!e)
                върни -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ако (r < 0)
                върни r;

        n = strndup(cid_start, e - cid_start);
        ако (!n)
                върни -ENOMEM;

        ако (isempty(n))
                cid = VMADDR_CID_ANY;
        иначе {
                r = vsock_parse_cid(n, &cid);
                ако (r < 0)
                        върни r;
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

        върни 0;
}

цяло netlink_socket_get_multicast_groups(цяло fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = NULL;
        socklen_t len = 0, old_len;

        assert(fd >= 0);

        ако (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, NULL, &len) < 0)
                върни -errno;

        ако (len == 0)
                goto finalize;

        groups = new0(uint32_t, len);
        ако (!groups)
                върни -ENOMEM;

        old_len = len;

        ако (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                върни -errno;

        ако (old_len != len)
                върни -EIO;

finalize:
        ако (ret_len)
                *ret_len = len;
        ако (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        върни 0;
}

цяло socket_get_cookie(цяло fd, uint64_t *ret) {
        assert(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = sizeof(cookie);
        ако (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                върни -errno;

        assert(cookie_len == sizeof(cookie));
        ако (ret)
                *ret = cookie;

        върни 0;
}

void cmsg_close_all(struct msghdr *mh) {
        assert(mh);

        struct cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ако (cmsg->cmsg_level != SOL_SOCKET)
                        continue;

                ако (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, цяло),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(цяло));
                иначе ако (cmsg->cmsg_type == SCM_PIDFD) {
                        assert(cmsg->cmsg_len == CMSG_LEN(sizeof(цяло)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, цяло));
                }
        }
}

цяло tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        switch (IPTOS_PREC(tos)) {
        case IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        case IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                върни TC_PRIO_CONTROL;

        case IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        case IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                върни TC_PRIO_INTERACTIVE;

        case IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        case IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                върни TC_PRIO_INTERACTIVE_BULK;

        case IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                върни TC_PRIO_BULK;

        case IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        default:
                върни TC_PRIO_BESTEFFORT;
        }
}

цяло socket_xattr_supported(void) {
        цяло r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        static цяло cached = -1;
        ако (cached >= 0)
                върни cached;

        _cleanup_close_ цяло fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ако (fd < 0)
                върни -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ NULL, AT_EMPTY_PATH, "user.testxxx", "1");
        ако (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                върни (cached = false);
        ако (r < 0)
                върни log_debug_errno(r, "Failed to set test xattr on socket: %m");

        върни (cached = true);
}
