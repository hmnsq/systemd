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

#إذا ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#وإلا
#  define IDN_FLAGS 0
#جيڪڏهن_پورو

ساكن ثابت حرف* ثابت socket_پتو_type_جدول[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_جدول_LOOKUP(socket_پتو_type, صحيح);

صحيح socket_پتو_verify(ثابت Socket_پتو *a, bool strict) {
        أكد(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        بدل (socket_پتو_family(a)) {

        حالة AF_INET:
                إذا (a->سائيز != حجم(بنية sockaddr_in))
                        أرجع -EINVAL;

                إذا (a->sockaddr.اندريون.sin_port == 0)
                        أرجع -EINVAL;

                إذا (!اندريون_سيٽ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        أرجع -EINVAL;

                أرجع 0;

        حالة AF_INET6:
                إذا (a->سائيز != حجم(بنية sockaddr_in6))
                        أرجع -EINVAL;

                إذا (a->sockaddr.in6.sin6_port == 0)
                        أرجع -EINVAL;

                إذا (!اندريون_سيٽ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        أرجع -EINVAL;

                أرجع 0;

        حالة AF_UNIX:
                إذا (a->سائيز < offsetof(بنية sockaddr_un, sun_path))
                        أرجع -EINVAL;
                إذا (a->سائيز > حجم(بنية sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        أرجع -EINVAL;

                إذا (a->سائيز > offsetof(بنية sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ثابت حرف *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, حجم(a->sockaddr.un.sun_path));
                        إذا (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                إذا (a->سائيز != offsetof(بنية sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        أرجع -EINVAL;
                        } وإلا {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                إذا (!اندريون_سيٽ(a->سائيز, حجم(a->sockaddr.un.sun_path), حجم(a->sockaddr.un.sun_path)+1))
                                        أرجع -EINVAL;
                        }
                }

                إذا (!اندريون_سيٽ(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        أرجع -EINVAL;

                أرجع 0;

        حالة AF_NETLINK:

                إذا (a->سائيز != حجم(بنية sockaddr_nl))
                        أرجع -EINVAL;

                إذا (!اندريون_سيٽ(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        أرجع -EINVAL;

                أرجع 0;

        حالة AF_VSOCK:
                إذا (a->سائيز != حجم(بنية sockaddr_vm))
                        أرجع -EINVAL;

                إذا (!اندريون_سيٽ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        أرجع -EINVAL;

                أرجع 0;

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح socket_پتو_print(ثابت Socket_پتو *a, حرف **ret) {
        صحيح r;

        أكد(a);
        أكد(ret);

        r = socket_پتو_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        إذا (r < 0)
                أرجع r;

        إذا (socket_پتو_family(a) == AF_NETLINK) {
                _cleanup_free_ حرف *sfamily = عدم;

                r = netlink_family_منزل_string_alloc(a->protocol, &sfamily);
                إذا (r < 0)
                        أرجع r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                إذا (r < 0)
                        أرجع -ENOMEM;

                أرجع 0;
        }

        أرجع sockaddr_pretty(&a->sockaddr.sa, a->سائيز, false, true, ret);
}

bool socket_پتو_can_accept(ثابت Socket_پتو *a) {
        أكد(a);

        أرجع
                اندريون_سيٽ(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_پتو_equal(ثابت Socket_پتو *a, ثابت Socket_پتو *b) {
        أكد(a);
        أكد(b);

        /* Invalid addresses are unequal to all */
        إذا (socket_پتو_verify(a, false) < 0 ||
            socket_پتو_verify(b, false) < 0)
                أرجع false;

        إذا (a->type != b->type)
                أرجع false;

        إذا (socket_پتو_family(a) != socket_پتو_family(b))
                أرجع false;

        بدل (socket_پتو_family(a)) {

        حالة AF_INET:
                إذا (a->sockaddr.اندريون.sin_addr.s_addr != b->sockaddr.اندريون.sin_addr.s_addr)
                        أرجع false;

                إذا (a->sockaddr.اندريون.sin_port != b->sockaddr.اندريون.sin_port)
                        أرجع false;

                اكسر;

        حالة AF_INET6:
                إذا (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, حجم(a->sockaddr.in6.sin6_addr)) != 0)
                        أرجع false;

                إذا (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        أرجع false;

                اكسر;

        حالة AF_UNIX:
                إذا (a->سائيز <= offsetof(بنية sockaddr_un, sun_path) ||
                    b->سائيز <= offsetof(بنية sockaddr_un, sun_path))
                        أرجع false;

                إذا ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        أرجع false;

                إذا (a->sockaddr.un.sun_path[0]) {
                        إذا (!رستو_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                أرجع false;
                } وإلا {
                        إذا (a->سائيز != b->سائيز)
                                أرجع false;

                        إذا (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->سائيز) != 0)
                                أرجع false;
                }

                اكسر;

        حالة AF_NETLINK:
                إذا (a->protocol != b->protocol)
                        أرجع false;

                إذا (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        أرجع false;

                اكسر;

        حالة AF_VSOCK:
                إذا (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        أرجع false;

                إذا (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        أرجع false;

                اكسر;

        افتراضي:
                /* Cannot compare, so we assume the addresses are different */
                أرجع false;
        }

        أرجع true;
}

ثابت حرف* socket_پتو_get_رستو(ثابت Socket_پتو *a) {
        أكد(a);

        إذا (socket_پتو_family(a) != AF_UNIX)
                أرجع عدم;

        إذا (a->sockaddr.un.sun_path[0] == 0)
                أرجع عدم;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(حجم(اتحاد sockaddr_union) >= حجم(بنية sockaddr_un)+1);
        أرجع a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(فراغ) {
        ساكن صحيح cached = -1;

        إذا (cached < 0) {

                إذا (access("/proc/net/if_inet6", F_OK) < 0) {

                        إذا (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                أرجع false;
                        }

                        cached = false;
                } وإلا
                        cached = true;
        }

        أرجع cached;
}

bool socket_ipv6_is_enabled(فراغ) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        إذا (!socket_ipv6_is_supported())
                أرجع false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        إذا (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                أرجع true;
        }

        r = parse_boolean(v);
        إذا (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                أرجع true;
        }

        أرجع !r;
}

bool socket_پتو_matches_fd(ثابت Socket_پتو *a, صحيح fd) {
        Socket_پتو b;
        socklen_t solen;

        أكد(a);
        أكد(fd >= 0);

        b.سائيز = حجم(b.sockaddr);
        إذا (getsockname(fd, &b.sockaddr.sa, &b.سائيز) < 0)
                أرجع false;

        إذا (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                أرجع false;

        solen = حجم(b.type);
        إذا (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                أرجع false;

        إذا (b.type != a->type)
                أرجع false;

        إذا (a->protocol != 0)  {
                solen = حجم(b.protocol);
                إذا (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        أرجع false;

                إذا (b.protocol != a->protocol)
                        أرجع false;
        }

        أرجع socket_پتو_equal(a, &b);
}

صحيح sockaddr_پورٽ(ثابت بنية sockaddr *_sa, بنانشان *ret_پورٽ) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        أكد(sa);
        أكد(ret_پورٽ);

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                *ret_پورٽ = be16toh(sa->اندريون.sin_port);
                أرجع 0;

        حالة AF_INET6:
                *ret_پورٽ = be16toh(sa->in6.sin6_port);
                أرجع 0;

        حالة AF_VSOCK:
                *ret_پورٽ = sa->vm.svm_port;
                أرجع 0;

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

ثابت اتحاد اندريون_addr_union *sockaddr_اندريون_addr(ثابت بنية sockaddr *_sa) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        إذا (!sa)
                أرجع عدم;

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                أرجع (ثابت اتحاد اندريون_addr_union*) &sa->اندريون.sin_addr;

        حالة AF_INET6:
                أرجع (ثابت اتحاد اندريون_addr_union*) &sa->in6.sin6_addr;

        افتراضي:
                أرجع عدم;
        }
}

صحيح sockaddr_سيٽ_اندريون_addr(
                اتحاد sockaddr_union *u,
                صحيح family,
                ثابت اتحاد اندريون_addr_union *a,
                uint16_t port) {

        أكد(u);
        أكد(a);

        بدل (family) {

        حالة AF_INET:
                u->اندريون = (بنية sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->اندريون,
                        .sin_port = htobe16(port),
                };

                أرجع 0;

        حالة AF_INET6:
                u->in6 = (بنية sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                أرجع 0;

        افتراضي:
                أرجع -EAFNOSUPPORT;

        }
}

صحيح sockaddr_pretty(
                ثابت بنية sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_پورٽ,
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

                a = be32toh(sa->اندريون.sin_addr.s_addr);

                إذا (include_پورٽ)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->اندريون.sin_port));
                وإلا
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                إذا (r < 0)
                        أرجع -ENOMEM;
                اكسر;
        }

        حالة AF_INET6: {
                ساكن ثابت بنانشان حرف ipv4_اڳياڙي[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                إذا (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_اڳياڙي, حجم(ipv4_اڳياڙي)) == 0) {
                        ثابت uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        إذا (include_پورٽ)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        وإلا
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        إذا (r < 0)
                                أرجع -ENOMEM;
                } وإلا {
                        ثابت حرف *a = IN6_ADDR_منزل_STRING(&sa->in6.sin6_addr);

                        إذا (include_پورٽ) {
                                إذا (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_ڀريل(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        أرجع -ENOMEM;
                        } وإلا {
                                إذا (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_ڀريل(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                وإلا
                                        p = strdup(a);
                                إذا (!p)
                                        أرجع -ENOMEM;
                        }
                }

                اكسر;
        }

        حالة AF_UNIX:
                إذا (salen <= offsetof(بنية sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(بنية sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                وإلا {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        حرف *رستو = (حرف*) sa->un_بفر + offsetof(بنية sockaddr_un, sun_path);
                        size_t رستو_len = salen - offsetof(بنية sockaddr_un, sun_path);

                        إذا (رستو[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ حرف *e = عدم;

                                e = cescape_ڊيگهائي(رستو + 1, رستو_len - 1);
                                إذا (!e)
                                        أرجع -ENOMEM;

                                p = strjoin("@", e);
                        } وإلا {
                                إذا (رستو[رستو_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        رستو_len--;

                                p = cescape_ڊيگهائي(رستو, رستو_len);
                        }
                }
                إذا (!p)
                        أرجع -ENOMEM;

                اكسر;

        حالة AF_VSOCK:
                إذا (include_پورٽ) {
                        إذا (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        وإلا
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } وإلا
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                إذا (r < 0)
                        أرجع -ENOMEM;
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        *ret = p;
        أرجع 0;
}

صحيح getpeername_pretty(صحيح fd, bool include_پورٽ, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        إذا (getpeername(fd, &sa.sa, &salen) < 0)
                أرجع -errno;

        إذا (sa.sa.sa_family == AF_UNIX) {
                بنية ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                إذا (r < 0)
                        أرجع r;

                إذا (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        أرجع -ENOMEM;

                أرجع 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        أرجع sockaddr_pretty(&sa.sa, salen, true, include_پورٽ, ret);
}

صحيح getsockname_pretty(صحيح fd, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);

        أكد(fd >= 0);
        أكد(ret);

        إذا (getsockname(fd, &sa.sa, &salen) < 0)
                أرجع -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        أرجع sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

صحيح socknameinfo_pretty(ثابت بنية sockaddr *sa, socklen_t salen, حرف **ret) {
        حرف host[NI_MAXHOST];
        صحيح r;

        أكد(sa);
        أكد(salen >= حجم(sa_family_t));
        أكد(ret);

        r = getnameinfo(sa, salen, host, حجم(host), /* service= */ عدم, /* service_len= */ 0, IDN_FLAGS);
        إذا (r != 0) {
                إذا (r == EAI_MEMORY)
                        أرجع log_oom_debug();
                إذا (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                وإلا
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                أرجع sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        أرجع strdup_منزل(ret, host);
}

ساكن ثابت حرف* ثابت netlink_family_جدول[] = {
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

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(netlink_family, صحيح, وڌ_کان_وڌ_عدد);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b) {
        أكد(a);
        أكد(b);

        إذا (a->sa.sa_family != b->sa.sa_family)
                أرجع false;

        إذا (a->sa.sa_family == AF_INET)
                أرجع a->اندريون.sin_addr.s_addr == b->اندريون.sin_addr.s_addr;

        إذا (a->sa.sa_family == AF_INET6)
                أرجع memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, حجم(a->in6.sin6_addr)) == 0;

        إذا (a->sa.sa_family == AF_VSOCK)
                أرجع a->vm.svm_cid == b->vm.svm_cid;

        أرجع false;
}

صحيح fd_سيٽ_sndbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, قدر;
        socklen_t l = حجم(قدر);

        إذا (n > وڌ_کان_وڌ_عدد)
                أرجع -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &قدر, &l);
        إذا (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                أرجع 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        إذا (r < 0)
                أرجع r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(قدر);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &قدر, &l);
        إذا (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                أرجع 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

صحيح fd_سيٽ_rcvbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, قدر;
        socklen_t l = حجم(قدر);

        إذا (n > وڌ_کان_وڌ_عدد)
                أرجع -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &قدر, &l);
        إذا (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                أرجع 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        إذا (r < 0)
                أرجع r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(قدر);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &قدر, &l);
        إذا (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                أرجع 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

ساكن ثابت حرف* ثابت ip_tos_جدول[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(ip_tos, صحيح, 0xff);

bool ifname_valid_char(حرف a) {
        إذا ((بنانشان حرف) a >= 127U)
                أرجع false;

        إذا ((بنانشان حرف) a <= 32U)
                أرجع false;

        إذا (اندريون_سيٽ(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                أرجع false;

        أرجع true;
}

bool ifname_valid_ڀريل(ثابت حرف *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        أكد(!(flags & ~_IFNAME_VALID_ALL));

        إذا (isempty(p))
                أرجع false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        إذا (parse_ifindex(p) >= 0)
                أرجع flags & IFNAME_VALID_NUMERIC;

        إذا (flags & IFNAME_VALID_ALTERNATIVE) {
                إذا (لڙي_ڊيگهه(p) >= ALTIFNAMSIZ)
                        أرجع false;
        } وإلا {
                إذا (لڙي_ڊيگهه(p) >= IFNAMSIZ)
                        أرجع false;
        }

        إذا (dot_or_dot_dot(p))
                أرجع false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        إذا (!FLAGS_سيٽ(flags, IFNAME_VALID_SPECIAL) && STR_اندريون_سيٽ(p, "all", "default"))
                أرجع false;

        لكل (ثابت حرف *t = p; *t; t++) {
                إذا (!ifname_valid_char(*t))
                        أرجع false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        إذا (numeric)
                أرجع false;

        أرجع true;
}

bool پتو_label_valid(ثابت حرف *p) {

        اشارو_MAY_BE_NULL(p);

        إذا (isempty(p))
                أرجع false;

        إذا (لڙي_ڊيگهه(p) >= IFNAMSIZ)
                أرجع false;

        طالما (*p) {
                إذا ((uint8_t) *p >= 127U)
                        أرجع false;

                إذا ((uint8_t) *p <= 31U)
                        أرجع false;
                p++;
        }

        أرجع true;
}

صحيح getpeercred(صحيح fd, بنية ucred *ucred) {
        socklen_t n = حجم(بنية ucred);
        بنية ucred u;

        أكد(fd >= 0);
        أكد(ucred);

        إذا (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                أرجع -errno;

        إذا (n != حجم(بنية ucred))
                أرجع -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        إذا (!pid_is_valid(u.pid))
                أرجع -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        أرجع 0;
}

صحيح getpeersec(صحيح fd, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        socklen_t n = 64;

        أكد(fd >= 0);
        أكد(ret);

        لكل (;;) {
                s = new0(حرف, n+1);
                إذا (!s)
                        أرجع -ENOMEM;

                إذا (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        اكسر;
                }

                إذا (errno != ERANGE)
                        أرجع -errno;

                s = mfree(s);
        }

        إذا (isempty(s))
                أرجع -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        أرجع 0;
}

صحيح getpeergroups(صحيح fd, gid_t **ret) {
        socklen_t n = حجم(gid_t) * 64U;
        _cleanup_free_ gid_t *d = عدم;

        أكد(fd >= 0);
        أكد(ret);

        صحيح ngroups_max = sysconf_ngroups_max();
        إذا (ngroups_max > 0)
                n = MAX(n, حجم(gid_t) * (socklen_t) ngroups_max);

        لكل (;;) {
                d = يادگيري_ڏيو(n);
                إذا (!d)
                        أرجع -ENOMEM;

                إذا (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        اكسر;

                إذا (errno != ERANGE)
                        أرجع -errno;

                d = mfree(d);
        }

        assert_se(n % حجم(gid_t) == 0);
        n /= حجم(gid_t);

        إذا (n > وڌ_کان_وڌ_عدد)
                أرجع -E2BIG;

        *ret = TAKE_PTR(d);

        أرجع (صحيح) n;
}

صحيح getpeerpidfd(صحيح fd) {
        socklen_t n = حجم(صحيح);
        صحيح pidfd = -EBADF;

        أكد(fd >= 0);

        إذا (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                أرجع -errno;

        إذا (n != حجم(صحيح))
                أرجع -EIO;

        أرجع pidfd;
}

صحيح getpeerpidref(صحيح fd, PidRef *ret) {
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        صحيح pidfd = getpeerpidfd(fd);
        إذا (pidfd < 0) {
                إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        أرجع pidfd;

                بنية ucred ucred;
                r = getpeercred(fd, &ucred);
                إذا (r < 0)
                        أرجع r;

                أرجع pidref_سيٽ_pid(ret, ucred.pid);
        }

        أرجع pidref_سيٽ_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية iovec *iov, size_t iovlen,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags) {

        CMSG_بفر_TYPE(CMSG_SPACE(حجم(صحيح))) control = {};
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
        إذا (fd < 0 && !iov)
                أرجع -EINVAL;

        إذا (fd >= 0) {
                بنية cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = حجم(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(حجم(صحيح));
                يادگيري_نقل(CMSG_DATA(cmsg), &fd, حجم(صحيح));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        إذا (k < 0)
                أرجع (ssize_t) -errno;

        أرجع k;
}

صحيح send_one_fd_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags) {

        أكد(fd >= 0);

        أرجع (صحيح) send_one_fd_iov_sa(transport_fd, fd, عدم, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                صحيح transport_fd,
                بنية iovec *iov, size_t iovlen,
                صحيح flags,
                صحيح *ret_fd) {

        CMSG_بفر_TYPE(CMSG_SPACE(حجم(صحيح))) control;
        بنية msghdr mh = {
                .msg_control = &control,
                .msg_controllen = حجم(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        بنية cmsghdr *مليو;
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

        k = recvmsg_محفوظ(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        إذا (k < 0)
                أرجع k;

        مليو = cmsg_ڳوليو(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(حجم(صحيح)));
        إذا (!مليو) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                إذا (k == 0)
                        أرجع -EIO;
        }

        إذا (مليو)
                *ret_fd = *CMSG_TYPED_معلومات(مليو, صحيح);
        وإلا
                *ret_fd = -EBADF;

        أرجع k;
}

صحيح receive_one_fd(صحيح transport_fd, صحيح flags) {
        صحيح fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, عدم, 0, flags, &fd);
        إذا (k == 0)
                أرجع fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        أكد(k < 0);
        أرجع (صحيح) k;
}

ssize_t ايندڙ_datagram_سائيز_fd(صحيح fd) {
        ssize_t l;
        صحيح k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, عدم, 0, MSG_PEEK|MSG_TRUNC);
        إذا (l < 0) {
                إذا (اندريون_سيٽ(errno, EOPNOTSUPP, EFAULT))
                        اذهب fallback;

                أرجع -errno;
        }
        إذا (l == 0)
                اذهب fallback;

        أرجع l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        إذا (ioctl(fd, FIONREAD, &k) < 0)
                أرجع -errno;

        أرجع (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#وصف_ڪريو MAX_FLUSH_ITERATIONS 1024

صحيح flush_accept(صحيح fd) {

        صحيح r, b;
        socklen_t l = حجم(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        إذا (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                أرجع -errno;

        أكد(l == حجم(b));
        إذا (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                أرجع -ENOTTY;

        لكل (بنانشان iteration = 0;; iteration++) {
                صحيح cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                إذا (r == -EINTR)
                        استمر;
                إذا (r <= 0)
                        أرجع r;

                إذا (iteration >= MAX_FLUSH_ITERATIONS)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, عدم, عدم, SOCK_NONBLOCK|SOCK_CLOEXEC);
                إذا (cfd < 0) {
                        إذا (errno == EAGAIN)
                                أرجع 0;

                        إذا (ERRNO_IS_ACCEPT_AGAIN(errno))
                                استمر;

                        أرجع -errno;
                }

                محفوظ_close(cfd);
        }
}

ssize_t flush_mqueue(صحيح fd) {
        _cleanup_free_ حرف *buf = عدم;
        بنية mq_attr attr;
        ssize_t ڳڻپ = 0;
        صحيح r;

        أكد(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        لكل (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                إذا (r == -EINTR)
                        استمر;
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع ڳڻپ;

                إذا (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        إذا (mq_getattr(fd, &attr) < 0)
                                أرجع -errno;

                        buf = يادگيري_ڏيو(attr.mq_msgsize);
                        إذا (!buf)
                                أرجع -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ عدم);
                إذا (l < 0) {
                        إذا (errno == EINTR)
                                استمر;

                        إذا (errno == EAGAIN)
                                أرجع ڳڻپ;

                        أرجع -errno;
                }

                ڳڻپ += l;
        }
}

بنية cmsghdr* cmsg_ڳوليو(بنية msghdr *mh, صحيح level, صحيح type, socklen_t length) {
        بنية cmsghdr *cmsg;

        أكد(mh);

        CMSG_FOREACH(cmsg, mh)
                إذا (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        أرجع cmsg;

        أرجع عدم;
}

فراغ* cmsg_ڳوليو_and_copy_معلومات(بنية msghdr *mh, صحيح level, صحيح type, فراغ *buf, size_t buf_len) {
        بنية cmsghdr *cmsg;

        أكد(mh);
        أكد(buf);
        أكد(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_ڳوليو(mh, level, type, CMSG_LEN(buf_len));
        إذا (!cmsg)
                أرجع عدم;

        أرجع memcpy_محفوظ(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ثابت بنية sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        أكد(sa->sll_family == AF_PACKET);

        size_t mac_len = حجم(sa->sll_addr);

        إذا (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        إذا (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        أرجع offsetof(بنية sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ثابت بنية sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        أكد(sa->sun_family == AF_UNIX);

        أرجع offsetof(بنية sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, حجم(sa->sun_path)-1) :
                        strnlen(sa->sun_path, حجم(sa->sun_path))+1);
}

size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa) {
        أكد(sa);

        بدل (sa->sa.sa_family) {
        حالة AF_INET:
                أرجع حجم(بنية sockaddr_in);
        حالة AF_INET6:
                أرجع حجم(بنية sockaddr_in6);
        حالة AF_UNIX:
                أرجع sockaddr_un_len(&sa->un);
        حالة AF_PACKET:
                أرجع sockaddr_ll_len(&sa->ll);
        حالة AF_NETLINK:
                أرجع حجم(بنية sockaddr_nl);
        حالة AF_VSOCK:
                أرجع حجم(بنية sockaddr_vm);
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
        إذا (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        إذا (fd < 0)
                أرجع -errno;

        أرجع fd;
}

صحيح sockaddr_un_unlink(ثابت بنية sockaddr_un *sa) {
        ثابت حرف *p, * nul;

        أكد(sa);

        إذا (sa->sun_family != AF_UNIX)
                أرجع -EPROTOTYPE;

        إذا (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                أرجع 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, حجم(sa->sun_path));
        إذا (nul)
                p = sa->sun_path;
        وإلا
                p = memdupa_suffix0(sa->sun_path, حجم(sa->sun_path));

        إذا (unlink(p) < 0)
                أرجع -errno;

        أرجع 1;
}

صحيح sockaddr_un_سيٽ_رستو(بنية sockaddr_un *ret, ثابت حرف *رستو) {
        size_t l;

        أكد(ret);
        أكد(رستو);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = لڙي_ڊيگهه(رستو);
        إذا (l < 2)
                أرجع -EINVAL;
        إذا (!اندريون_سيٽ(رستو[0], '/', '@'))
                أرجع -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        إذا (l+1 > حجم(ret->sun_path))
                أرجع رستو[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (بنية sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        إذا (رستو[0] == '@') {
                /* Abstract namespace socket */
                يادگيري_نقل(ret->sun_path + 1, رستو + 1, l); /* copy *with* trailing NUL byte */
                أرجع (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } وإلا {
                أكد(رستو[0] == '/');

                /* File system socket */
                يادگيري_نقل(ret->sun_path, رستو, l + 1); /* copy *with* trailing NUL byte */
                أرجع (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

صحيح getsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح *ret) {
        صحيح v;
        socklen_t sl = حجم(v);

        أكد(fd >= 0);
        أكد(ret);

        إذا (getsockopt(fd, level, optname, &v, &sl) < 0)
                أرجع negative_errno();
        إذا (sl != حجم(v))
                أرجع -EIO;

        *ret = v;
        أرجع 0;
}

صحيح socket_bind_منزل_ifname(صحيح fd, ثابت حرف *ifname) {
        أكد(fd >= 0);

        /* Call with NULL to drop binding */

        أرجع RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

صحيح socket_bind_منزل_ifindex(صحيح fd, صحيح ifindex) {
        أكد(fd >= 0);

        إذا (ifindex <= 0)
                /* Drop binding */
                أرجع RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, عدم, 0));

        أرجع setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

صحيح socket_autobind(صحيح fd, حرف **ret_name) {
        _cleanup_free_ حرف *name = عدم;
        uint64_t random;
        صحيح r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        أكد(fd >= 0);

        random = random_u64();

        إذا (asprintf(&name, "@%" PRIu64, random) < 0)
                أرجع -ENOMEM;

        اتحاد sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < حجم(sa.un.sun_path));

        r = sockaddr_un_سيٽ_رستو(&sa.un, name);
        إذا (r < 0)
                أرجع r;

        إذا (bind(fd, &sa.sa, r) < 0)
                أرجع -errno;

        إذا (ret_name)
                *ret_name = TAKE_PTR(name);
        أرجع 0;
}

ssize_t recvmsg_محفوظ(صحيح sockfd, بنية msghdr *msg, صحيح flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        أكد(sockfd >= 0);
        أكد(msg);

        n = recvmsg(sockfd, msg, flags);
        إذا (n < 0)
                أرجع -errno;

        إذا (FLAGS_سيٽ(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_سيٽ(flags, MSG_PEEK) && FLAGS_سيٽ(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                أرجع FLAGS_سيٽ(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        أرجع n;
}

صحيح socket_get_family(صحيح fd) {
        صحيح af;
        socklen_t sl = حجم(af);

        إذا (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                أرجع -errno;

        إذا (sl != حجم(af))
                أرجع -EINVAL;

        أرجع af;
}

صحيح socket_سيٽ_recvpktinfo(صحيح fd, صحيح af, bool b) {

        إذا (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                إذا (af < 0)
                        أرجع af;
        }

        بدل (af) {

        حالة AF_INET:
                أرجع setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        حالة AF_INET6:
                أرجع setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        حالة AF_NETLINK:
                أرجع setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        حالة AF_PACKET:
                أرجع setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح socket_سيٽ_unicast_if(صحيح fd, صحيح af, صحيح ifi) {
        be32_t ifindex_be = htobe32(ifi);

        إذا (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                إذا (af < 0)
                        أرجع af;
        }

        بدل (af) {

        حالة AF_INET:
                أرجع RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        حالة AF_INET6:
                أرجع RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح socket_سيٽ_option(صحيح fd, صحيح af, صحيح opt_ipv4, صحيح opt_ipv6, صحيح val) {
        إذا (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                إذا (af < 0)
                        أرجع af;
        }

        بدل (af) {

        حالة AF_INET:
                أرجع setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        حالة AF_INET6:
                أرجع setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }
}

صحيح socket_get_mtu(صحيح fd, صحيح af, size_t *ret) {
        صحيح mtu, r;

        أكد(ret);

        إذا (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                إذا (af < 0)
                        أرجع af;
        }

        بدل (af) {

        حالة AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                اكسر;

        حالة AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                اكسر;

        افتراضي:
                أرجع -EAFNOSUPPORT;
        }

        إذا (r < 0)
                أرجع r;
        إذا (mtu <= 0)
                أرجع -EINVAL;

        *ret = (size_t) mtu;
        أرجع 0;
}

ساكن صحيح connect_unix_رستو_simple(صحيح fd, ثابت حرف *رستو) {
        اتحاد sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        أكد(fd >= 0);
        أكد(رستو);

        l = لڙي_ڊيگهه(رستو);
        أكد(l > 0);
        أكد(l < حجم(sa.un.sun_path));

        يادگيري_نقل(sa.un.sun_path, رستو, l + 1);
        أرجع RET_NERRNO(connect(fd, &sa.sa, offsetof(بنية sockaddr_un, sun_path) + l + 1));
}

ساكن صحيح connect_unix_inode(صحيح fd, صحيح inode_fd) {
        أكد(fd >= 0);
        أكد(inode_fd >= 0);

        أرجع connect_unix_رستو_simple(fd, FORMAT_PROC_FD_رستو(inode_fd));
}

صحيح connect_unix_رستو(صحيح fd, صحيح dir_fd, ثابت حرف *رستو) {
        _cleanup_close_ صحيح inode_fd = -EBADF;

        أكد(fd >= 0);
        أكد(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        إذا (!رستو) {
                إذا (dir_fd < 0)
                        أرجع -EISDIR;

                أرجع connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        إذا (isempty(رستو))
                أرجع -EINVAL;

        /* Shortcut for the simple case */
        إذا (dir_fd == AT_FDCWD && لڙي_ڊيگهه(رستو) < sizeof_ميدان(بنية sockaddr_un, sun_path))
                أرجع connect_unix_رستو_simple(fd, رستو);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        إذا (dir_fd == XAT_FDROOT) {
                _cleanup_free_ حرف *j = strjoin("/", رستو);
                إذا (!j)
                        أرجع -ENOMEM;

                inode_fd = open(j, O_رستو|O_CLOEXEC);
        } وإلا
                inode_fd = openat(dir_fd, رستو, O_رستو|O_CLOEXEC);
        إذا (inode_fd < 0)
                أرجع -errno;

        أرجع connect_unix_inode(fd, inode_fd);
}

صحيح socket_پتو_parse_unix(Socket_پتو *ret_پتو, ثابت حرف *s) {
        بنية sockaddr_un un;
        صحيح r;

        أكد(ret_پتو);
        أكد(s);

        إذا (!اندريون_سيٽ(*s, '/', '@'))
                أرجع -EPROTO;

        r = sockaddr_un_سيٽ_رستو(&un, s);
        إذا (r < 0)
                أرجع r;

        *ret_پتو = (Socket_پتو) {
                .sockaddr.un = un,
                .سائيز = r,
        };

        أرجع 0;
}

صحيح socket_پتو_equal_unix(ثابت حرف *a, ثابت حرف *b) {
        Socket_پتو socket_a, socket_b;
        صحيح r;

        أكد(a);
        أكد(b);

        r = socket_پتو_parse_unix(&socket_a, a);
        إذا (r < 0)
                أرجع r;

        r = socket_پتو_parse_unix(&socket_b, b);
        إذا (r < 0)
                أرجع r;

        أرجع sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

صحيح vsock_parse_پورٽ(ثابت حرف *s, بنانشان *ret) {
        صحيح r;

        أكد(ret);

        إذا (!s)
                أرجع -EINVAL;

        بنانشان u;
        r = محفوظ_atou(s, &u);
        إذا (r < 0)
                أرجع r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        إذا (u == VMADDR_PORT_ANY)
                أرجع -EINVAL;

        *ret = u;
        أرجع 0;
}

صحيح vsock_parse_cid(ثابت حرف *s, بنانشان *ret) {
        أكد(ret);

        إذا (!s)
                أرجع -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        إذا (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        وإلا إذا (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        وإلا إذا (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        وإلا إذا (STR_اندريون_سيٽ(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        وإلا
                أرجع محفوظ_atou(s, ret);

        أرجع 0;
}

صحيح socket_پتو_parse_vsock(Socket_پتو *ret_پتو, ثابت حرف *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ حرف *n = عدم;
        ثابت حرف *e, *cid_شروعات;
        بنانشان port, cid;
        صحيح type, r;

        أكد(ret_پتو);
        أكد(s);

        إذا ((cid_شروعات = startswith(s, "vsock:")))
                type = 0;
        وإلا إذا ((cid_شروعات = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        وإلا إذا ((cid_شروعات = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        وإلا إذا ((cid_شروعات = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        وإلا
                أرجع -EPROTO;

        e = strchr(cid_شروعات, ':');
        إذا (!e)
                أرجع -EINVAL;

        r = vsock_parse_پورٽ(e+1, &port);
        إذا (r < 0)
                أرجع r;

        n = strndup(cid_شروعات, e - cid_شروعات);
        إذا (!n)
                أرجع -ENOMEM;

        إذا (isempty(n))
                cid = VMADDR_CID_ANY;
        وإلا {
                r = vsock_parse_cid(n, &cid);
                إذا (r < 0)
                        أرجع r;
        }

        *ret_پتو = (Socket_پتو) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .سائيز = حجم(بنية sockaddr_vm),
        };

        أرجع 0;
}

صحيح netlink_socket_get_multicast_groups(صحيح fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = عدم;
        socklen_t len = 0, old_len;

        أكد(fd >= 0);

        إذا (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, عدم, &len) < 0)
                أرجع -errno;

        إذا (len == 0)
                اذهب finalize;

        groups = new0(uint32_t, len);
        إذا (!groups)
                أرجع -ENOMEM;

        old_len = len;

        إذا (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                أرجع -errno;

        إذا (old_len != len)
                أرجع -EIO;

finalize:
        إذا (ret_len)
                *ret_len = len;
        إذا (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        أرجع 0;
}

صحيح socket_get_cookie(صحيح fd, uint64_t *ret) {
        أكد(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = حجم(cookie);
        إذا (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                أرجع -errno;

        أكد(cookie_len == حجم(cookie));
        إذا (ret)
                *ret = cookie;

        أرجع 0;
}

فراغ cmsg_close_all(بنية msghdr *mh) {
        أكد(mh);

        بنية cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                إذا (cmsg->cmsg_level != SOL_SOCKET)
                        استمر;

                إذا (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_معلومات(cmsg, صحيح),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / حجم(صحيح));
                وإلا إذا (cmsg->cmsg_type == SCM_PIDFD) {
                        أكد(cmsg->cmsg_len == CMSG_LEN(حجم(صحيح)));
                        محفوظ_close(*CMSG_TYPED_معلومات(cmsg, صحيح));
                }
        }
}

صحيح tos_منزل_ترجيح(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        بدل (IPTOS_PREC(tos)) {
        حالة IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        حالة IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                أرجع TC_PRIO_CONTROL;

        حالة IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        حالة IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                أرجع TC_PRIO_INTERACTIVE;

        حالة IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        حالة IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                أرجع TC_PRIO_INTERACTIVE_BULK;

        حالة IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                أرجع TC_PRIO_BULK;

        حالة IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        افتراضي:
                أرجع TC_PRIO_BESTEFFORT;
        }
}

صحيح socket_xattr_supported(فراغ) {
        صحيح r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ساكن صحيح cached = -1;
        إذا (cached >= 0)
                أرجع cached;

        _cleanup_close_ صحيح fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        إذا (fd < 0)
                أرجع -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ عدم, AT_خالي_جايون_رستو, "user.testxxx", "1");
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                أرجع (cached = false);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to set test xattr on socket: %m");

        أرجع (cached = true);
}
