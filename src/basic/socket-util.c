/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <linux/if.h>
#ضمن <linux/if_arp.h>
#ضمن <linux/if_ether.h>
#ضمن <linux/if_infiniband.h>
#ضمن <linux/pkt_sched.h>
#ضمن <mqueue.h>
#ضمن <net/if.h>
#ضمن <netdb.h>
#ضمن <netinet/ip.h>
#ضمن <poll.h>
#ضمن <stdio.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "escape.h"
#ضمن "fd-util.h"
#ضمن "format-ifname.h"
#ضمن "format-util.h"
#ضمن "in-addr-util.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "random-util.h"
#ضمن "socket-util.h"
#ضمن "sparse-endian.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "sysctl-util.h"
#ضمن "user-util.h"
#ضمن "xattr-util.h"

#אם ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#אחרת
#  define IDN_FLAGS 0
#endif

ساكن ثابت حرف* ثابت socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, صحيح);

صحيح socket_address_verify(ثابت SocketAddress *a, bool strict) {
        أكد(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        بدل (socket_address_family(a)) {

        حالة AF_INET:
                אם (a->size != حجم(بنية sockaddr_in))
                        החזר -EINVAL;

                אם (a->sockaddr.in.sin_port == 0)
                        החזר -EINVAL;

                אם (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        החזר -EINVAL;

                החזר 0;

        حالة AF_INET6:
                אם (a->size != حجم(بنية sockaddr_in6))
                        החזר -EINVAL;

                אם (a->sockaddr.in6.sin6_port == 0)
                        החזר -EINVAL;

                אם (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        החזר -EINVAL;

                החזר 0;

        حالة AF_UNIX:
                אם (a->size < offsetof(بنية sockaddr_un, sun_path))
                        החזר -EINVAL;
                אם (a->size > حجم(بنية sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        החזר -EINVAL;

                אם (a->size > offsetof(بنية sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ثابت حرف *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, حجم(a->sockaddr.un.sun_path));
                        אם (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                אם (a->size != offsetof(بنية sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        החזר -EINVAL;
                        } אחרת {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                אם (!IN_SET(a->size, حجم(a->sockaddr.un.sun_path), حجم(a->sockaddr.un.sun_path)+1))
                                        החזר -EINVAL;
                        }
                }

                אם (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        החזר -EINVAL;

                החזר 0;

        حالة AF_NETLINK:

                אם (a->size != حجم(بنية sockaddr_nl))
                        החזר -EINVAL;

                אם (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        החזר -EINVAL;

                החזר 0;

        حالة AF_VSOCK:
                אם (a->size != حجم(بنية sockaddr_vm))
                        החזר -EINVAL;

                אם (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        החזר -EINVAL;

                החזר 0;

        افتراضي:
                החזר -EAFNOSUPPORT;
        }
}

صحيح socket_address_print(ثابت SocketAddress *a, حرف **ret) {
        صحيح r;

        أكد(a);
        أكد(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        אם (r < 0)
                החזר r;

        אם (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ حرف *sfamily = عدم;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                אם (r < 0)
                        החזר r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                אם (r < 0)
                        החזר -ENOMEM;

                החזר 0;
        }

        החזר sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ثابت SocketAddress *a) {
        أكد(a);

        החזר
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ثابت SocketAddress *a, ثابت SocketAddress *b) {
        أكد(a);
        أكد(b);

        /* Invalid addresses are unequal to all */
        אם (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                החזר false;

        אם (a->type != b->type)
                החזר false;

        אם (socket_address_family(a) != socket_address_family(b))
                החזר false;

        بدل (socket_address_family(a)) {

        حالة AF_INET:
                אם (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        החזר false;

                אם (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        החזר false;

                اكسر;

        حالة AF_INET6:
                אם (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, حجم(a->sockaddr.in6.sin6_addr)) != 0)
                        החזר false;

                אם (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        החזר false;

                اكسر;

        حالة AF_UNIX:
                אם (a->size <= offsetof(بنية sockaddr_un, sun_path) ||
                    b->size <= offsetof(بنية sockaddr_un, sun_path))
                        החזר false;

                אם ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        החזר false;

                אם (a->sockaddr.un.sun_path[0]) {
                        אם (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                החזר false;
                } אחרת {
                        אם (a->size != b->size)
                                החזר false;

                        אם (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                החזר false;
                }

                اكسر;

        حالة AF_NETLINK:
                אם (a->protocol != b->protocol)
                        החזר false;

                אם (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        החזר false;

                اكسر;

        حالة AF_VSOCK:
                אם (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        החזר false;

                אם (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        החזר false;

                اكسر;

        افتراضي:
                /* Cannot compare, so we assume the addresses are different */
                החזר false;
        }

        החזר true;
}

ثابت حرف* socket_address_get_path(ثابت SocketAddress *a) {
        أكد(a);

        אם (socket_address_family(a) != AF_UNIX)
                החזר عدم;

        אם (a->sockaddr.un.sun_path[0] == 0)
                החזר عدم;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(حجم(اتحاد sockaddr_union) >= حجم(بنية sockaddr_un)+1);
        החזר a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(فراغ) {
        ساكن صحيح cached = -1;

        אם (cached < 0) {

                אם (access("/proc/net/if_inet6", F_OK) < 0) {

                        אם (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                החזר false;
                        }

                        cached = false;
                } אחרת
                        cached = true;
        }

        החזר cached;
}

bool socket_ipv6_is_enabled(فراغ) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        אם (!socket_ipv6_is_supported())
                החזר false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        אם (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                החזר true;
        }

        r = parse_boolean(v);
        אם (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                החזר true;
        }

        החזר !r;
}

bool socket_address_matches_fd(ثابت SocketAddress *a, صحيح fd) {
        SocketAddress b;
        socklen_t solen;

        أكد(a);
        أكد(fd >= 0);

        b.size = حجم(b.sockaddr);
        אם (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                החזר false;

        אם (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                החזר false;

        solen = حجم(b.type);
        אם (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                החזר false;

        אם (b.type != a->type)
                החזר false;

        אם (a->protocol != 0)  {
                solen = حجم(b.protocol);
                אם (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        החזר false;

                אם (b.protocol != a->protocol)
                        החזר false;
        }

        החזר socket_address_equal(a, &b);
}

صحيح sockaddr_port(ثابت بنية sockaddr *_sa, unsigned *ret_port) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        أكد(sa);
        أكد(ret_port);

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                החזר 0;

        حالة AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                החזר 0;

        حالة AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                החזר 0;

        افتراضي:
                החזר -EAFNOSUPPORT;
        }
}

ثابت اتحاد in_addr_union *sockaddr_in_addr(ثابت بنية sockaddr *_sa) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        אם (!sa)
                החזר عدم;

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                החזר (ثابت اتحاد in_addr_union*) &sa->in.sin_addr;

        حالة AF_INET6:
                החזר (ثابت اتحاد in_addr_union*) &sa->in6.sin6_addr;

        افتراضي:
                החזר عدم;
        }
}

صحيح sockaddr_set_in_addr(
                اتحاد sockaddr_union *u,
                صحيح family,
                ثابت اتحاد in_addr_union *a,
                uint16_t port) {

        أكد(u);
        أكد(a);

        بدل (family) {

        حالة AF_INET:
                u->in = (بنية sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                החזר 0;

        حالة AF_INET6:
                u->in6 = (بنية sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                החזר 0;

        افتراضي:
                החזר -EAFNOSUPPORT;

        }
}

صحيح sockaddr_pretty(
                ثابت بنية sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                حرف **ret) {

        اتحاد sockaddr_union *sa = (اتحاد sockaddr_union*) _sa;
        حرف *p;
        صحيح r;

        أكد(sa);
        أكد(salen >= حجم(sa->sa.sa_family));
        أكد(ret);

        بدل (sa->sa.sa_family) {

        حالة AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                אם (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                אחרת
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                אם (r < 0)
                        החזר -ENOMEM;
                اكسر;
        }

        حالة AF_INET6: {
                ساكن ثابت unsigned حرف ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                אם (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, حجم(ipv4_prefix)) == 0) {
                        ثابت uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        אם (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        אחרת
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        אם (r < 0)
                                החזר -ENOMEM;
                } אחרת {
                        ثابت حرف *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        אם (include_port) {
                                אם (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        החזר -ENOMEM;
                        } אחרת {
                                אם (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                אחרת
                                        p = strdup(a);
                                אם (!p)
                                        החזר -ENOMEM;
                        }
                }

                اكسر;
        }

        حالة AF_UNIX:
                אם (salen <= offsetof(بنية sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(بنية sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                אחרת {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        حرف *path = (حرف*) sa->un_buffer + offsetof(بنية sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(بنية sockaddr_un, sun_path);

                        אם (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ حرف *e = عدم;

                                e = cescape_length(path + 1, path_len - 1);
                                אם (!e)
                                        החזר -ENOMEM;

                                p = strjoin("@", e);
                        } אחרת {
                                אם (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                אם (!p)
                        החזר -ENOMEM;

                اكسر;

        حالة AF_VSOCK:
                אם (include_port) {
                        אם (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        אחרת
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } אחרת
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                אם (r < 0)
                        החזר -ENOMEM;
                اكسر;

        افتراضي:
                החזר -EOPNOTSUPP;
        }

        *ret = p;
        החזר 0;
}

صحيح getpeername_pretty(صحيح fd, bool include_port, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        אם (getpeername(fd, &sa.sa, &salen) < 0)
                החזר -errno;

        אם (sa.sa.sa_family == AF_UNIX) {
                بنية ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                אם (r < 0)
                        החזר r;

                אם (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        החזר -ENOMEM;

                החזר 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        החזר sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

صحيح getsockname_pretty(صحيح fd, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);

        أكد(fd >= 0);
        أكد(ret);

        אם (getsockname(fd, &sa.sa, &salen) < 0)
                החזר -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        החזר sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

صحيح socknameinfo_pretty(ثابت بنية sockaddr *sa, socklen_t salen, حرف **ret) {
        حرف host[NI_MAXHOST];
        صحيح r;

        أكد(sa);
        أكد(salen >= حجم(sa_family_t));
        أكد(ret);

        r = getnameinfo(sa, salen, host, حجم(host), /* service= */ عدم, /* service_len= */ 0, IDN_FLAGS);
        אם (r != 0) {
                אם (r == EAI_MEMORY)
                        החזר log_oom_debug();
                אם (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                אחרת
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                החזר sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        החזר strdup_to(ret, host);
}

ساكن ثابت حرف* ثابت netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, صحيح, INT_MAX);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b) {
        أكد(a);
        أكد(b);

        אם (a->sa.sa_family != b->sa.sa_family)
                החזר false;

        אם (a->sa.sa_family == AF_INET)
                החזר a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        אם (a->sa.sa_family == AF_INET6)
                החזר memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, حجم(a->in6.sin6_addr)) == 0;

        אם (a->sa.sa_family == AF_VSOCK)
                החזר a->vm.svm_cid == b->vm.svm_cid;

        החזר false;
}

صحيح fd_set_sndbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, value;
        socklen_t l = حجم(value);

        אם (n > INT_MAX)
                החזר -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        אם (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                החזר 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        אם (r < 0)
                החזר r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        אם (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                החזר 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        אם (r < 0)
                החזר r;

        החזר 1;
}

صحيح fd_set_rcvbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, value;
        socklen_t l = حجم(value);

        אם (n > INT_MAX)
                החזר -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        אם (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                החזר 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        אם (r < 0)
                החזר r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        אם (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                החזר 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        אם (r < 0)
                החזר r;

        החזר 1;
}

ساكن ثابت حرف* ثابت ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, صحيح, 0xff);

bool ifname_valid_char(حرف a) {
        אם ((unsigned حرف) a >= 127U)
                החזר false;

        אם ((unsigned حرف) a <= 32U)
                החזר false;

        אם (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                החזר false;

        החזר true;
}

bool ifname_valid_full(ثابت حرف *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        أكد(!(flags & ~_IFNAME_VALID_ALL));

        אם (isempty(p))
                החזר false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        אם (parse_ifindex(p) >= 0)
                החזר flags & IFNAME_VALID_NUMERIC;

        אם (flags & IFNAME_VALID_ALTERNATIVE) {
                אם (strlen(p) >= ALTIFNAMSIZ)
                        החזר false;
        } אחרת {
                אם (strlen(p) >= IFNAMSIZ)
                        החזר false;
        }

        אם (dot_or_dot_dot(p))
                החזר false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        אם (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                החזר false;

        עבור (ثابت حرف *t = p; *t; t++) {
                אם (!ifname_valid_char(*t))
                        החזר false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        אם (numeric)
                החזר false;

        החזר true;
}

bool address_label_valid(ثابت حرف *p) {

        POINTER_MAY_BE_NULL(p);

        אם (isempty(p))
                החזר false;

        אם (strlen(p) >= IFNAMSIZ)
                החזר false;

        כלעוד (*p) {
                אם ((uint8_t) *p >= 127U)
                        החזר false;

                אם ((uint8_t) *p <= 31U)
                        החזר false;
                p++;
        }

        החזר true;
}

صحيح getpeercred(صحيح fd, بنية ucred *ucred) {
        socklen_t n = حجم(بنية ucred);
        بنية ucred u;

        أكد(fd >= 0);
        أكد(ucred);

        אם (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                החזר -errno;

        אם (n != حجم(بنية ucred))
                החזר -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        אם (!pid_is_valid(u.pid))
                החזר -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        החזר 0;
}

صحيح getpeersec(صحيح fd, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        socklen_t n = 64;

        أكد(fd >= 0);
        أكد(ret);

        עבור (;;) {
                s = new0(حرف, n+1);
                אם (!s)
                        החזר -ENOMEM;

                אם (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        اكسر;
                }

                אם (errno != ERANGE)
                        החזר -errno;

                s = mfree(s);
        }

        אם (isempty(s))
                החזר -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        החזר 0;
}

صحيح getpeergroups(صحيح fd, gid_t **ret) {
        socklen_t n = حجم(gid_t) * 64U;
        _cleanup_free_ gid_t *d = عدم;

        أكد(fd >= 0);
        أكد(ret);

        صحيح ngroups_max = sysconf_ngroups_max();
        אם (ngroups_max > 0)
                n = MAX(n, حجم(gid_t) * (socklen_t) ngroups_max);

        עבור (;;) {
                d = malloc(n);
                אם (!d)
                        החזר -ENOMEM;

                אם (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        اكسر;

                אם (errno != ERANGE)
                        החזר -errno;

                d = mfree(d);
        }

        assert_se(n % حجم(gid_t) == 0);
        n /= حجم(gid_t);

        אם (n > INT_MAX)
                החזר -E2BIG;

        *ret = TAKE_PTR(d);

        החזר (صحيح) n;
}

صحيح getpeerpidfd(صحيح fd) {
        socklen_t n = حجم(صحيح);
        صحيح pidfd = -EBADF;

        أكد(fd >= 0);

        אם (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                החזר -errno;

        אם (n != حجم(صحيح))
                החזר -EIO;

        החזר pidfd;
}

صحيح getpeerpidref(صحيح fd, PidRef *ret) {
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        صحيح pidfd = getpeerpidfd(fd);
        אם (pidfd < 0) {
                אם (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        החזר pidfd;

                بنية ucred ucred;
                r = getpeercred(fd, &ucred);
                אם (r < 0)
                        החזר r;

                החזר pidref_set_pid(ret, ucred.pid);
        }

        החזר pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية iovec *iov, size_t iovlen,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(حجم(صحيح))) control = {};
        بنية msghdr mh = {
                .msg_name = (بنية sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (بنية iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        أكد(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        אם (fd < 0 && !iov)
                החזר -EINVAL;

        אם (fd >= 0) {
                بنية cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = حجم(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(حجم(صحيح));
                memcpy(CMSG_DATA(cmsg), &fd, حجم(صحيح));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        אם (k < 0)
                החזר (ssize_t) -errno;

        החזר k;
}

صحيح send_one_fd_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags) {

        أكد(fd >= 0);

        החזר (صحيح) send_one_fd_iov_sa(transport_fd, fd, عدم, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                صحيح transport_fd,
                بنية iovec *iov, size_t iovlen,
                صحيح flags,
                صحيح *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(حجم(صحيح))) control;
        بنية msghdr mh = {
                .msg_control = &control,
                .msg_controllen = حجم(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        بنية cmsghdr *found;
        ssize_t k;

        أكد(transport_fd >= 0);
        أكد(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        אם (k < 0)
                החזר k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(حجم(صحيح)));
        אם (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                אם (k == 0)
                        החזר -EIO;
        }

        אם (found)
                *ret_fd = *CMSG_TYPED_DATA(found, صحيح);
        אחרת
                *ret_fd = -EBADF;

        החזר k;
}

صحيح receive_one_fd(صحيح transport_fd, صحيح flags) {
        صحيح fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, عدم, 0, flags, &fd);
        אם (k == 0)
                החזר fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        أكد(k < 0);
        החזר (صحيح) k;
}

ssize_t next_datagram_size_fd(صحيح fd) {
        ssize_t l;
        صحيح k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, عدم, 0, MSG_PEEK|MSG_TRUNC);
        אם (l < 0) {
                אם (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        اذهب fallback;

                החזר -errno;
        }
        אם (l == 0)
                اذهب fallback;

        החזר l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        אם (ioctl(fd, FIONREAD, &k) < 0)
                החזר -errno;

        החזר (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

صحيح flush_accept(صحيح fd) {

        صحيح r, b;
        socklen_t l = حجم(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        אם (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                החזר -errno;

        أكد(l == حجم(b));
        אם (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                החזר -ENOTTY;

        עבור (unsigned iteration = 0;; iteration++) {
                صحيح cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                אם (r == -EINTR)
                        استمر;
                אם (r <= 0)
                        החזר r;

                אם (iteration >= MAX_FLUSH_ITERATIONS)
                        החזר log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, عدم, عدم, SOCK_NONBLOCK|SOCK_CLOEXEC);
                אם (cfd < 0) {
                        אם (errno == EAGAIN)
                                החזר 0;

                        אם (ERRNO_IS_ACCEPT_AGAIN(errno))
                                استمر;

                        החזר -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(صحيح fd) {
        _cleanup_free_ حرف *buf = عدم;
        بنية mq_attr attr;
        ssize_t count = 0;
        صحيح r;

        أكد(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        עבור (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                אם (r == -EINTR)
                        استمر;
                אם (r < 0)
                        החזר r;
                אם (r == 0)
                        החזר count;

                אם (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        אם (mq_getattr(fd, &attr) < 0)
                                החזר -errno;

                        buf = malloc(attr.mq_msgsize);
                        אם (!buf)
                                החזר -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ عدم);
                אם (l < 0) {
                        אם (errno == EINTR)
                                استمر;

                        אם (errno == EAGAIN)
                                החזר count;

                        החזר -errno;
                }

                count += l;
        }
}

بنية cmsghdr* cmsg_find(بنية msghdr *mh, صحيح level, صحيح type, socklen_t length) {
        بنية cmsghdr *cmsg;

        أكد(mh);

        CMSG_FOREACH(cmsg, mh)
                אם (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        החזר cmsg;

        החזר عدم;
}

فراغ* cmsg_find_and_copy_data(بنية msghdr *mh, صحيح level, صحيح type, فراغ *buf, size_t buf_len) {
        بنية cmsghdr *cmsg;

        أكد(mh);
        أكد(buf);
        أكد(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        אם (!cmsg)
                החזר عدم;

        החזר memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ثابت بنية sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        أكد(sa->sll_family == AF_PACKET);

        size_t mac_len = حجم(sa->sll_addr);

        אם (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        אם (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        החזר offsetof(بنية sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ثابت بنية sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        أكد(sa->sun_family == AF_UNIX);

        החזר offsetof(بنية sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, حجم(sa->sun_path)-1) :
                        strnlen(sa->sun_path, حجم(sa->sun_path))+1);
}

size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa) {
        أكد(sa);

        بدل (sa->sa.sa_family) {
        حالة AF_INET:
                החזר حجم(بنية sockaddr_in);
        حالة AF_INET6:
                החזר حجم(بنية sockaddr_in6);
        حالة AF_UNIX:
                החזר sockaddr_un_len(&sa->un);
        حالة AF_PACKET:
                החזר sockaddr_ll_len(&sa->ll);
        حالة AF_NETLINK:
                החזר حجم(بنية sockaddr_nl);
        حالة AF_VSOCK:
                החזר حجم(بنية sockaddr_vm);
        افتراضي:
                assert_not_reached();
        }
}

صحيح socket_ioctl_fd(فراغ) {
        صحيح fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        אם (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        אם (fd < 0)
                החזר -errno;

        החזר fd;
}

صحيح sockaddr_un_unlink(ثابت بنية sockaddr_un *sa) {
        ثابت حرف *p, * nul;

        أكد(sa);

        אם (sa->sun_family != AF_UNIX)
                החזר -EPROTOTYPE;

        אם (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                החזר 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, حجم(sa->sun_path));
        אם (nul)
                p = sa->sun_path;
        אחרת
                p = memdupa_suffix0(sa->sun_path, حجم(sa->sun_path));

        אם (unlink(p) < 0)
                החזר -errno;

        החזר 1;
}

صحيح sockaddr_un_set_path(بنية sockaddr_un *ret, ثابت حرف *path) {
        size_t l;

        أكد(ret);
        أكد(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        אם (l < 2)
                החזר -EINVAL;
        אם (!IN_SET(path[0], '/', '@'))
                החזר -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        אם (l+1 > حجم(ret->sun_path))
                החזר path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (بنية sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        אם (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                החזר (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } אחרת {
                أكد(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                החזר (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

صحيح getsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح *ret) {
        صحيح v;
        socklen_t sl = حجم(v);

        أكد(fd >= 0);
        أكد(ret);

        אם (getsockopt(fd, level, optname, &v, &sl) < 0)
                החזר negative_errno();
        אם (sl != حجم(v))
                החזר -EIO;

        *ret = v;
        החזר 0;
}

صحيح socket_bind_to_ifname(صحيح fd, ثابت حرف *ifname) {
        أكد(fd >= 0);

        /* Call with NULL to drop binding */

        החזר RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

صحيح socket_bind_to_ifindex(صحيح fd, صحيح ifindex) {
        أكد(fd >= 0);

        אם (ifindex <= 0)
                /* Drop binding */
                החזר RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, عدم, 0));

        החזר setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

صحيح socket_autobind(صحيح fd, حرف **ret_name) {
        _cleanup_free_ حرف *name = عدم;
        uint64_t random;
        صحيح r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        أكد(fd >= 0);

        random = random_u64();

        אם (asprintf(&name, "@%" PRIu64, random) < 0)
                החזר -ENOMEM;

        اتحاد sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < حجم(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        אם (r < 0)
                החזר r;

        אם (bind(fd, &sa.sa, r) < 0)
                החזר -errno;

        אם (ret_name)
                *ret_name = TAKE_PTR(name);
        החזר 0;
}

ssize_t recvmsg_safe(صحيح sockfd, بنية msghdr *msg, صحيح flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        أكد(sockfd >= 0);
        أكد(msg);

        n = recvmsg(sockfd, msg, flags);
        אם (n < 0)
                החזר -errno;

        אם (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                החזר FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        החזר n;
}

صحيح socket_get_family(صحيح fd) {
        صحيح af;
        socklen_t sl = حجم(af);

        אם (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                החזר -errno;

        אם (sl != حجم(af))
                החזר -EINVAL;

        החזר af;
}

صحيح socket_set_recvpktinfo(صحيح fd, صحيح af, bool b) {

        אם (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                אם (af < 0)
                        החזר af;
        }

        بدل (af) {

        حالة AF_INET:
                החזר setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        حالة AF_INET6:
                החזר setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        حالة AF_NETLINK:
                החזר setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        حالة AF_PACKET:
                החזר setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        افتراضي:
                החזר -EAFNOSUPPORT;
        }
}

صحيح socket_set_unicast_if(صحيح fd, صحيح af, صحيح ifi) {
        be32_t ifindex_be = htobe32(ifi);

        אם (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                אם (af < 0)
                        החזר af;
        }

        بدل (af) {

        حالة AF_INET:
                החזר RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        حالة AF_INET6:
                החזר RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        افتراضي:
                החזר -EAFNOSUPPORT;
        }
}

صحيح socket_set_option(صحيح fd, صحيح af, صحيح opt_ipv4, صحيح opt_ipv6, صحيح val) {
        אם (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                אם (af < 0)
                        החזר af;
        }

        بدل (af) {

        حالة AF_INET:
                החזר setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        حالة AF_INET6:
                החזר setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        افتراضي:
                החזר -EAFNOSUPPORT;
        }
}

صحيح socket_get_mtu(صحيح fd, صحيح af, size_t *ret) {
        صحيح mtu, r;

        أكد(ret);

        אם (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                אם (af < 0)
                        החזר af;
        }

        بدل (af) {

        حالة AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                اكسر;

        حالة AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                اكسر;

        افتراضي:
                החזר -EAFNOSUPPORT;
        }

        אם (r < 0)
                החזר r;
        אם (mtu <= 0)
                החזר -EINVAL;

        *ret = (size_t) mtu;
        החזר 0;
}

ساكن صحيح connect_unix_path_simple(صحيح fd, ثابت حرف *path) {
        اتحاد sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        أكد(fd >= 0);
        أكد(path);

        l = strlen(path);
        أكد(l > 0);
        أكد(l < حجم(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        החזר RET_NERRNO(connect(fd, &sa.sa, offsetof(بنية sockaddr_un, sun_path) + l + 1));
}

ساكن صحيح connect_unix_inode(صحيح fd, صحيح inode_fd) {
        أكد(fd >= 0);
        أكد(inode_fd >= 0);

        החזר connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

صحيح connect_unix_path(صحيح fd, صحيح dir_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح inode_fd = -EBADF;

        أكد(fd >= 0);
        أكد(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        אם (!path) {
                אם (dir_fd < 0)
                        החזר -EISDIR;

                החזר connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        אם (isempty(path))
                החזר -EINVAL;

        /* Shortcut for the simple case */
        אם (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(بنية sockaddr_un, sun_path))
                החזר connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        אם (dir_fd == XAT_FDROOT) {
                _cleanup_free_ حرف *j = strjoin("/", path);
                אם (!j)
                        החזר -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } אחרת
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        אם (inode_fd < 0)
                החזר -errno;

        החזר connect_unix_inode(fd, inode_fd);
}

صحيح socket_address_parse_unix(SocketAddress *ret_address, ثابت حرف *s) {
        بنية sockaddr_un un;
        صحيح r;

        أكد(ret_address);
        أكد(s);

        אם (!IN_SET(*s, '/', '@'))
                החזר -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        אם (r < 0)
                החזר r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        החזר 0;
}

صحيح socket_address_equal_unix(ثابت حرف *a, ثابت حرف *b) {
        SocketAddress socket_a, socket_b;
        صحيح r;

        أكد(a);
        أكد(b);

        r = socket_address_parse_unix(&socket_a, a);
        אם (r < 0)
                החזר r;

        r = socket_address_parse_unix(&socket_b, b);
        אם (r < 0)
                החזר r;

        החזר sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

صحيح vsock_parse_port(ثابت حرف *s, unsigned *ret) {
        صحيح r;

        أكد(ret);

        אם (!s)
                החזר -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        אם (r < 0)
                החזר r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        אם (u == VMADDR_PORT_ANY)
                החזר -EINVAL;

        *ret = u;
        החזר 0;
}

صحيح vsock_parse_cid(ثابت حرف *s, unsigned *ret) {
        أكد(ret);

        אם (!s)
                החזר -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        אם (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        אחרת אם (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        אחרת אם (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        אחרת אם (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        אחרת
                החזר safe_atou(s, ret);

        החזר 0;
}

صحيح socket_address_parse_vsock(SocketAddress *ret_address, ثابت حرف *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ حرف *n = عدم;
        ثابت حرف *e, *cid_start;
        unsigned port, cid;
        صحيح type, r;

        أكد(ret_address);
        أكد(s);

        אם ((cid_start = startswith(s, "vsock:")))
                type = 0;
        אחרת אם ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        אחרת אם ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        אחרת אם ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        אחרת
                החזר -EPROTO;

        e = strchr(cid_start, ':');
        אם (!e)
                החזר -EINVAL;

        r = vsock_parse_port(e+1, &port);
        אם (r < 0)
                החזר r;

        n = strndup(cid_start, e - cid_start);
        אם (!n)
                החזר -ENOMEM;

        אם (isempty(n))
                cid = VMADDR_CID_ANY;
        אחרת {
                r = vsock_parse_cid(n, &cid);
                אם (r < 0)
                        החזר r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = حجم(بنية sockaddr_vm),
        };

        החזר 0;
}

صحيح netlink_socket_get_multicast_groups(صحيح fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = عدم;
        socklen_t len = 0, old_len;

        أكد(fd >= 0);

        אם (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, عدم, &len) < 0)
                החזר -errno;

        אם (len == 0)
                اذهب finalize;

        groups = new0(uint32_t, len);
        אם (!groups)
                החזר -ENOMEM;

        old_len = len;

        אם (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                החזר -errno;

        אם (old_len != len)
                החזר -EIO;

finalize:
        אם (ret_len)
                *ret_len = len;
        אם (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        החזר 0;
}

صحيح socket_get_cookie(صحيح fd, uint64_t *ret) {
        أكد(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = حجم(cookie);
        אם (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                החזר -errno;

        أكد(cookie_len == حجم(cookie));
        אם (ret)
                *ret = cookie;

        החזר 0;
}

فراغ cmsg_close_all(بنية msghdr *mh) {
        أكد(mh);

        بنية cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                אם (cmsg->cmsg_level != SOL_SOCKET)
                        استمر;

                אם (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, صحيح),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / حجم(صحيح));
                אחרת אם (cmsg->cmsg_type == SCM_PIDFD) {
                        أكد(cmsg->cmsg_len == CMSG_LEN(حجم(صحيح)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, صحيح));
                }
        }
}

صحيح tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        بدل (IPTOS_PREC(tos)) {
        حالة IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        حالة IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                החזר TC_PRIO_CONTROL;

        حالة IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        حالة IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                החזר TC_PRIO_INTERACTIVE;

        حالة IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        حالة IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                החזר TC_PRIO_INTERACTIVE_BULK;

        حالة IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                החזר TC_PRIO_BULK;

        حالة IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        افتراضي:
                החזר TC_PRIO_BESTEFFORT;
        }
}

صحيح socket_xattr_supported(فراغ) {
        صحيح r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ساكن صحيح cached = -1;
        אם (cached >= 0)
                החזר cached;

        _cleanup_close_ صحيح fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        אם (fd < 0)
                החזר -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ عدم, AT_EMPTY_PATH, "user.testxxx", "1");
        אם (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                החזר (cached = false);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to set test xattr on socket: %m");

        החזר (cached = true);
}
