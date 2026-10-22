/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <linux/if.h>
#شامل <linux/if_arp.h>
#شامل <linux/if_ether.h>
#شامل <linux/if_infiniband.h>
#شامل <linux/pkt_sched.h>
#شامل <mqueue.h>
#شامل <net/if.h>
#شامل <netdb.h>
#شامل <netinet/ip.h>
#شامل <poll.h>
#شامل <stdio.h>
#شامل <sys/ioctl.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "escape.h"
#شامل "fd-util.h"
#شامل "format-ifname.h"
#شامل "format-util.h"
#شامل "in-addr-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "random-util.h"
#شامل "socket-util.h"
#شامل "sparse-endian.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "sysctl-util.h"
#شامل "user-util.h"
#شامل "xattr-util.h"

#اگر ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ورنہ
#  define IDN_FLAGS 0
#اگر_ختم

ساکن ثابت حرف* ثابت socket_پتہ_type_جدول[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_جدول_LOOKUP(socket_پتہ_type, صحیح);

صحیح socket_پتہ_verify(ثابت Socket_پتہ *a, bool strict) {
        أكد(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        انتخاب (socket_پتہ_family(a)) {

        حالت AF_INET:
                اگر (a->سائز != حجم(ساخت sockaddr_in))
                        واپس -EINVAL;

                اگر (a->sockaddr.اندر.sin_port == 0)
                        واپس -EINVAL;

                اگر (!اندر_سیٹ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        واپس -EINVAL;

                واپس 0;

        حالت AF_INET6:
                اگر (a->سائز != حجم(ساخت sockaddr_in6))
                        واپس -EINVAL;

                اگر (a->sockaddr.in6.sin6_port == 0)
                        واپس -EINVAL;

                اگر (!اندر_سیٹ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        واپس -EINVAL;

                واپس 0;

        حالت AF_UNIX:
                اگر (a->سائز < offsetof(ساخت sockaddr_un, sun_path))
                        واپس -EINVAL;
                اگر (a->سائز > حجم(ساخت sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        واپس -EINVAL;

                اگر (a->سائز > offsetof(ساخت sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ثابت حرف *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, حجم(a->sockaddr.un.sun_path));
                        اگر (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                اگر (a->سائز != offsetof(ساخت sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        واپس -EINVAL;
                        } ورنہ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                اگر (!اندر_سیٹ(a->سائز, حجم(a->sockaddr.un.sun_path), حجم(a->sockaddr.un.sun_path)+1))
                                        واپس -EINVAL;
                        }
                }

                اگر (!اندر_سیٹ(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        واپس -EINVAL;

                واپس 0;

        حالت AF_NETLINK:

                اگر (a->سائز != حجم(ساخت sockaddr_nl))
                        واپس -EINVAL;

                اگر (!اندر_سیٹ(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        واپس -EINVAL;

                واپس 0;

        حالت AF_VSOCK:
                اگر (a->سائز != حجم(ساخت sockaddr_vm))
                        واپس -EINVAL;

                اگر (!اندر_سیٹ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        واپس -EINVAL;

                واپس 0;

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح socket_پتہ_print(ثابت Socket_پتہ *a, حرف **ret) {
        صحیح r;

        أكد(a);
        أكد(ret);

        r = socket_پتہ_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        اگر (r < 0)
                واپس r;

        اگر (socket_پتہ_family(a) == AF_NETLINK) {
                _cleanup_free_ حرف *sfamily = تہی;

                r = netlink_family_مقصد_string_alloc(a->protocol, &sfamily);
                اگر (r < 0)
                        واپس r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                اگر (r < 0)
                        واپس -ENOMEM;

                واپس 0;
        }

        واپس sockaddr_pretty(&a->sockaddr.sa, a->سائز, false, true, ret);
}

bool socket_پتہ_can_accept(ثابت Socket_پتہ *a) {
        أكد(a);

        واپس
                اندر_سیٹ(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_پتہ_equal(ثابت Socket_پتہ *a, ثابت Socket_پتہ *b) {
        أكد(a);
        أكد(b);

        /* Invalid addresses are unequal to all */
        اگر (socket_پتہ_verify(a, false) < 0 ||
            socket_پتہ_verify(b, false) < 0)
                واپس false;

        اگر (a->type != b->type)
                واپس false;

        اگر (socket_پتہ_family(a) != socket_پتہ_family(b))
                واپس false;

        انتخاب (socket_پتہ_family(a)) {

        حالت AF_INET:
                اگر (a->sockaddr.اندر.sin_addr.s_addr != b->sockaddr.اندر.sin_addr.s_addr)
                        واپس false;

                اگر (a->sockaddr.اندر.sin_port != b->sockaddr.اندر.sin_port)
                        واپس false;

                توڑو;

        حالت AF_INET6:
                اگر (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, حجم(a->sockaddr.in6.sin6_addr)) != 0)
                        واپس false;

                اگر (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        واپس false;

                توڑو;

        حالت AF_UNIX:
                اگر (a->سائز <= offsetof(ساخت sockaddr_un, sun_path) ||
                    b->سائز <= offsetof(ساخت sockaddr_un, sun_path))
                        واپس false;

                اگر ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        واپس false;

                اگر (a->sockaddr.un.sun_path[0]) {
                        اگر (!راستہ_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                واپس false;
                } ورنہ {
                        اگر (a->سائز != b->سائز)
                                واپس false;

                        اگر (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->سائز) != 0)
                                واپس false;
                }

                توڑو;

        حالت AF_NETLINK:
                اگر (a->protocol != b->protocol)
                        واپس false;

                اگر (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        واپس false;

                توڑو;

        حالت AF_VSOCK:
                اگر (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        واپس false;

                اگر (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        واپس false;

                توڑو;

        طےشدہ:
                /* Cannot compare, so we assume the addresses are different */
                واپس false;
        }

        واپس true;
}

ثابت حرف* socket_پتہ_get_راستہ(ثابت Socket_پتہ *a) {
        أكد(a);

        اگر (socket_پتہ_family(a) != AF_UNIX)
                واپس تہی;

        اگر (a->sockaddr.un.sun_path[0] == 0)
                واپس تہی;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(حجم(اتحاد sockaddr_union) >= حجم(ساخت sockaddr_un)+1);
        واپس a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(خالی) {
        ساکن صحیح cached = -1;

        اگر (cached < 0) {

                اگر (access("/proc/net/if_inet6", F_OK) < 0) {

                        اگر (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                واپس false;
                        }

                        cached = false;
                } ورنہ
                        cached = true;
        }

        واپس cached;
}

bool socket_ipv6_is_enabled(خالی) {
        _cleanup_free_ حرف *v = تہی;
        صحیح r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        اگر (!socket_ipv6_is_supported())
                واپس false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        اگر (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                واپس true;
        }

        r = parse_boolean(v);
        اگر (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                واپس true;
        }

        واپس !r;
}

bool socket_پتہ_matches_fd(ثابت Socket_پتہ *a, صحیح fd) {
        Socket_پتہ b;
        socklen_t solen;

        أكد(a);
        أكد(fd >= 0);

        b.سائز = حجم(b.sockaddr);
        اگر (getsockname(fd, &b.sockaddr.sa, &b.سائز) < 0)
                واپس false;

        اگر (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                واپس false;

        solen = حجم(b.type);
        اگر (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                واپس false;

        اگر (b.type != a->type)
                واپس false;

        اگر (a->protocol != 0)  {
                solen = حجم(b.protocol);
                اگر (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        واپس false;

                اگر (b.protocol != a->protocol)
                        واپس false;
        }

        واپس socket_پتہ_equal(a, &b);
}

صحیح sockaddr_پورٹ(ثابت ساخت sockaddr *_sa, غیرعلامتی *ret_پورٹ) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        أكد(sa);
        أكد(ret_پورٹ);

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET:
                *ret_پورٹ = be16toh(sa->اندر.sin_port);
                واپس 0;

        حالت AF_INET6:
                *ret_پورٹ = be16toh(sa->in6.sin6_port);
                واپس 0;

        حالت AF_VSOCK:
                *ret_پورٹ = sa->vm.svm_port;
                واپس 0;

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

ثابت اتحاد اندر_addr_union *sockaddr_اندر_addr(ثابت ساخت sockaddr *_sa) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        اگر (!sa)
                واپس تہی;

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET:
                واپس (ثابت اتحاد اندر_addr_union*) &sa->اندر.sin_addr;

        حالت AF_INET6:
                واپس (ثابت اتحاد اندر_addr_union*) &sa->in6.sin6_addr;

        طےشدہ:
                واپس تہی;
        }
}

صحیح sockaddr_سیٹ_اندر_addr(
                اتحاد sockaddr_union *u,
                صحیح family,
                ثابت اتحاد اندر_addr_union *a,
                uint16_t port) {

        أكد(u);
        أكد(a);

        انتخاب (family) {

        حالت AF_INET:
                u->اندر = (ساخت sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->اندر,
                        .sin_port = htobe16(port),
                };

                واپس 0;

        حالت AF_INET6:
                u->in6 = (ساخت sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                واپس 0;

        طےشدہ:
                واپس -EAFNOSUPPORT;

        }
}

صحیح sockaddr_pretty(
                ثابت ساخت sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_پورٹ,
                حرف **ret) {

        اتحاد sockaddr_union *sa = (اتحاد sockaddr_union*) _sa;
        حرف *p;
        صحیح r;

        أكد(sa);
        أكد(salen >= حجم(sa->sa.sa_family));
        أكد(ret);

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET: {
                uint32_t a;

                a = be32toh(sa->اندر.sin_addr.s_addr);

                اگر (include_پورٹ)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->اندر.sin_port));
                ورنہ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                اگر (r < 0)
                        واپس -ENOMEM;
                توڑو;
        }

        حالت AF_INET6: {
                ساکن ثابت غیرعلامتی حرف ipv4_سابقہ[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                اگر (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_سابقہ, حجم(ipv4_سابقہ)) == 0) {
                        ثابت uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        اگر (include_پورٹ)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ورنہ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        اگر (r < 0)
                                واپس -ENOMEM;
                } ورنہ {
                        ثابت حرف *a = IN6_ADDR_مقصد_STRING(&sa->in6.sin6_addr);

                        اگر (include_پورٹ) {
                                اگر (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_بھرا(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        واپس -ENOMEM;
                        } ورنہ {
                                اگر (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_بھرا(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ورنہ
                                        p = strdup(a);
                                اگر (!p)
                                        واپس -ENOMEM;
                        }
                }

                توڑو;
        }

        حالت AF_UNIX:
                اگر (salen <= offsetof(ساخت sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ساخت sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ورنہ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        حرف *راستہ = (حرف*) sa->un_بفر + offsetof(ساخت sockaddr_un, sun_path);
                        size_t راستہ_len = salen - offsetof(ساخت sockaddr_un, sun_path);

                        اگر (راستہ[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ حرف *e = تہی;

                                e = cescape_لمبائی(راستہ + 1, راستہ_len - 1);
                                اگر (!e)
                                        واپس -ENOMEM;

                                p = strjoin("@", e);
                        } ورنہ {
                                اگر (راستہ[راستہ_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        راستہ_len--;

                                p = cescape_لمبائی(راستہ, راستہ_len);
                        }
                }
                اگر (!p)
                        واپس -ENOMEM;

                توڑو;

        حالت AF_VSOCK:
                اگر (include_پورٹ) {
                        اگر (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ورنہ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ورنہ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                اگر (r < 0)
                        واپس -ENOMEM;
                توڑو;

        طےشدہ:
                واپس -EOPNOTSUPP;
        }

        *ret = p;
        واپس 0;
}

صحیح getpeername_pretty(صحیح fd, bool include_پورٹ, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        اگر (getpeername(fd, &sa.sa, &salen) < 0)
                واپس -errno;

        اگر (sa.sa.sa_family == AF_UNIX) {
                ساخت ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                اگر (r < 0)
                        واپس r;

                اگر (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        واپس -ENOMEM;

                واپس 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        واپس sockaddr_pretty(&sa.sa, salen, true, include_پورٹ, ret);
}

صحیح getsockname_pretty(صحیح fd, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);

        أكد(fd >= 0);
        أكد(ret);

        اگر (getsockname(fd, &sa.sa, &salen) < 0)
                واپس -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        واپس sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

صحیح socknameinfo_pretty(ثابت ساخت sockaddr *sa, socklen_t salen, حرف **ret) {
        حرف host[NI_MAXHOST];
        صحیح r;

        أكد(sa);
        أكد(salen >= حجم(sa_family_t));
        أكد(ret);

        r = getnameinfo(sa, salen, host, حجم(host), /* service= */ تہی, /* service_len= */ 0, IDN_FLAGS);
        اگر (r != 0) {
                اگر (r == EAI_MEMORY)
                        واپس log_oom_debug();
                اگر (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ورنہ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                واپس sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        واپس strdup_مقصد(ret, host);
}

ساکن ثابت حرف* ثابت netlink_family_جدول[] = {
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

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(netlink_family, صحیح, زیادہ_سے_زیادہ_عدد);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b) {
        أكد(a);
        أكد(b);

        اگر (a->sa.sa_family != b->sa.sa_family)
                واپس false;

        اگر (a->sa.sa_family == AF_INET)
                واپس a->اندر.sin_addr.s_addr == b->اندر.sin_addr.s_addr;

        اگر (a->sa.sa_family == AF_INET6)
                واپس memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, حجم(a->in6.sin6_addr)) == 0;

        اگر (a->sa.sa_family == AF_VSOCK)
                واپس a->vm.svm_cid == b->vm.svm_cid;

        واپس false;
}

صحیح fd_سیٹ_sndbuf(صحیح fd, size_t n, bool increase) {
        صحیح r, قدر;
        socklen_t l = حجم(قدر);

        اگر (n > زیادہ_سے_زیادہ_عدد)
                واپس -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &قدر, &l);
        اگر (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                واپس 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        اگر (r < 0)
                واپس r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(قدر);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &قدر, &l);
        اگر (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                واپس 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        اگر (r < 0)
                واپس r;

        واپس 1;
}

صحیح fd_سیٹ_rcvbuf(صحیح fd, size_t n, bool increase) {
        صحیح r, قدر;
        socklen_t l = حجم(قدر);

        اگر (n > زیادہ_سے_زیادہ_عدد)
                واپس -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &قدر, &l);
        اگر (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                واپس 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        اگر (r < 0)
                واپس r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(قدر);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &قدر, &l);
        اگر (r >= 0 && l == حجم(قدر) && increase ? (size_t) قدر >= n*2 : (size_t) قدر == n*2)
                واپس 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        اگر (r < 0)
                واپس r;

        واپس 1;
}

ساکن ثابت حرف* ثابت ip_tos_جدول[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(ip_tos, صحیح, 0xff);

bool ifname_valid_char(حرف a) {
        اگر ((غیرعلامتی حرف) a >= 127U)
                واپس false;

        اگر ((غیرعلامتی حرف) a <= 32U)
                واپس false;

        اگر (اندر_سیٹ(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                واپس false;

        واپس true;
}

bool ifname_valid_بھرا(ثابت حرف *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        أكد(!(flags & ~_IFNAME_VALID_ALL));

        اگر (isempty(p))
                واپس false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        اگر (parse_ifindex(p) >= 0)
                واپس flags & IFNAME_VALID_NUMERIC;

        اگر (flags & IFNAME_VALID_ALTERNATIVE) {
                اگر (لڑی_لمبائی(p) >= ALTIFNAMSIZ)
                        واپس false;
        } ورنہ {
                اگر (لڑی_لمبائی(p) >= IFNAMSIZ)
                        واپس false;
        }

        اگر (dot_or_dot_dot(p))
                واپس false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        اگر (!FLAGS_سیٹ(flags, IFNAME_VALID_SPECIAL) && STR_اندر_سیٹ(p, "all", "default"))
                واپس false;

        برائے (ثابت حرف *t = p; *t; t++) {
                اگر (!ifname_valid_char(*t))
                        واپس false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        اگر (numeric)
                واپس false;

        واپس true;
}

bool پتہ_label_valid(ثابت حرف *p) {

        اشارہ_گر_MAY_BE_NULL(p);

        اگر (isempty(p))
                واپس false;

        اگر (لڑی_لمبائی(p) >= IFNAMSIZ)
                واپس false;

        جب (*p) {
                اگر ((uint8_t) *p >= 127U)
                        واپس false;

                اگر ((uint8_t) *p <= 31U)
                        واپس false;
                p++;
        }

        واپس true;
}

صحیح getpeercred(صحیح fd, ساخت ucred *ucred) {
        socklen_t n = حجم(ساخت ucred);
        ساخت ucred u;

        أكد(fd >= 0);
        أكد(ucred);

        اگر (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                واپس -errno;

        اگر (n != حجم(ساخت ucred))
                واپس -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        اگر (!pid_is_valid(u.pid))
                واپس -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        واپس 0;
}

صحیح getpeersec(صحیح fd, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        socklen_t n = 64;

        أكد(fd >= 0);
        أكد(ret);

        برائے (;;) {
                s = new0(حرف, n+1);
                اگر (!s)
                        واپس -ENOMEM;

                اگر (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        توڑو;
                }

                اگر (errno != ERANGE)
                        واپس -errno;

                s = mfree(s);
        }

        اگر (isempty(s))
                واپس -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        واپس 0;
}

صحیح getpeergroups(صحیح fd, gid_t **ret) {
        socklen_t n = حجم(gid_t) * 64U;
        _cleanup_free_ gid_t *d = تہی;

        أكد(fd >= 0);
        أكد(ret);

        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max > 0)
                n = MAX(n, حجم(gid_t) * (socklen_t) ngroups_max);

        برائے (;;) {
                d = حافظہ_دو(n);
                اگر (!d)
                        واپس -ENOMEM;

                اگر (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        توڑو;

                اگر (errno != ERANGE)
                        واپس -errno;

                d = mfree(d);
        }

        assert_se(n % حجم(gid_t) == 0);
        n /= حجم(gid_t);

        اگر (n > زیادہ_سے_زیادہ_عدد)
                واپس -E2BIG;

        *ret = TAKE_PTR(d);

        واپس (صحیح) n;
}

صحیح getpeerpidfd(صحیح fd) {
        socklen_t n = حجم(صحیح);
        صحیح pidfd = -EBADF;

        أكد(fd >= 0);

        اگر (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                واپس -errno;

        اگر (n != حجم(صحیح))
                واپس -EIO;

        واپس pidfd;
}

صحیح getpeerpidref(صحیح fd, PidRef *ret) {
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        صحیح pidfd = getpeerpidfd(fd);
        اگر (pidfd < 0) {
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        واپس pidfd;

                ساخت ucred ucred;
                r = getpeercred(fd, &ucred);
                اگر (r < 0)
                        واپس r;

                واپس pidref_سیٹ_pid(ret, ucred.pid);
        }

        واپس pidref_سیٹ_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساخت iovec *iov, size_t iovlen,
                ثابت ساخت sockaddr *sa, socklen_t len,
                صحیح flags) {

        CMSG_بفر_TYPE(CMSG_SPACE(حجم(صحیح))) control = {};
        ساخت msghdr mh = {
                .msg_name = (ساخت sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ساخت iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        أكد(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        اگر (fd < 0 && !iov)
                واپس -EINVAL;

        اگر (fd >= 0) {
                ساخت cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = حجم(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(حجم(صحیح));
                حافظہ_نقل(CMSG_DATA(cmsg), &fd, حجم(صحیح));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        اگر (k < 0)
                واپس (ssize_t) -errno;

        واپس k;
}

صحیح send_one_fd_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساخت sockaddr *sa, socklen_t len,
                صحیح flags) {

        أكد(fd >= 0);

        واپس (صحیح) send_one_fd_iov_sa(transport_fd, fd, تہی, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                صحیح transport_fd,
                ساخت iovec *iov, size_t iovlen,
                صحیح flags,
                صحیح *ret_fd) {

        CMSG_بفر_TYPE(CMSG_SPACE(حجم(صحیح))) control;
        ساخت msghdr mh = {
                .msg_control = &control,
                .msg_controllen = حجم(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ساخت cmsghdr *ملا;
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
        اگر (k < 0)
                واپس k;

        ملا = cmsg_ڈھونڈو(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(حجم(صحیح)));
        اگر (!ملا) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                اگر (k == 0)
                        واپس -EIO;
        }

        اگر (ملا)
                *ret_fd = *CMSG_TYPED_ڈیٹا(ملا, صحیح);
        ورنہ
                *ret_fd = -EBADF;

        واپس k;
}

صحیح receive_one_fd(صحیح transport_fd, صحیح flags) {
        صحیح fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, تہی, 0, flags, &fd);
        اگر (k == 0)
                واپس fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        أكد(k < 0);
        واپس (صحیح) k;
}

ssize_t اگلا_datagram_سائز_fd(صحیح fd) {
        ssize_t l;
        صحیح k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, تہی, 0, MSG_PEEK|MSG_TRUNC);
        اگر (l < 0) {
                اگر (اندر_سیٹ(errno, EOPNOTSUPP, EFAULT))
                        جاؤ fallback;

                واپس -errno;
        }
        اگر (l == 0)
                جاؤ fallback;

        واپس l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        اگر (ioctl(fd, FIONREAD, &k) < 0)
                واپس -errno;

        واپس (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#تعریف_کرو MAX_FLUSH_ITERATIONS 1024

صحیح flush_accept(صحیح fd) {

        صحیح r, b;
        socklen_t l = حجم(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        اگر (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                واپس -errno;

        أكد(l == حجم(b));
        اگر (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                واپس -ENOTTY;

        برائے (غیرعلامتی iteration = 0;; iteration++) {
                صحیح cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                اگر (r == -EINTR)
                        جاری;
                اگر (r <= 0)
                        واپس r;

                اگر (iteration >= MAX_FLUSH_ITERATIONS)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, تہی, تہی, SOCK_NONBLOCK|SOCK_CLOEXEC);
                اگر (cfd < 0) {
                        اگر (errno == EAGAIN)
                                واپس 0;

                        اگر (ERRNO_IS_ACCEPT_AGAIN(errno))
                                جاری;

                        واپس -errno;
                }

                محفوظ_close(cfd);
        }
}

ssize_t flush_mqueue(صحیح fd) {
        _cleanup_free_ حرف *buf = تہی;
        ساخت mq_attr attr;
        ssize_t گنتی = 0;
        صحیح r;

        أكد(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        برائے (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                اگر (r == -EINTR)
                        جاری;
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس گنتی;

                اگر (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        اگر (mq_getattr(fd, &attr) < 0)
                                واپس -errno;

                        buf = حافظہ_دو(attr.mq_msgsize);
                        اگر (!buf)
                                واپس -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ تہی);
                اگر (l < 0) {
                        اگر (errno == EINTR)
                                جاری;

                        اگر (errno == EAGAIN)
                                واپس گنتی;

                        واپس -errno;
                }

                گنتی += l;
        }
}

ساخت cmsghdr* cmsg_ڈھونڈو(ساخت msghdr *mh, صحیح level, صحیح type, socklen_t length) {
        ساخت cmsghdr *cmsg;

        أكد(mh);

        CMSG_FOREACH(cmsg, mh)
                اگر (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        واپس cmsg;

        واپس تہی;
}

خالی* cmsg_ڈھونڈو_and_copy_ڈیٹا(ساخت msghdr *mh, صحیح level, صحیح type, خالی *buf, size_t buf_len) {
        ساخت cmsghdr *cmsg;

        أكد(mh);
        أكد(buf);
        أكد(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_ڈھونڈو(mh, level, type, CMSG_LEN(buf_len));
        اگر (!cmsg)
                واپس تہی;

        واپس memcpy_محفوظ(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ثابت ساخت sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        أكد(sa->sll_family == AF_PACKET);

        size_t mac_len = حجم(sa->sll_addr);

        اگر (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        اگر (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        واپس offsetof(ساخت sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ثابت ساخت sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        أكد(sa->sun_family == AF_UNIX);

        واپس offsetof(ساخت sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, حجم(sa->sun_path)-1) :
                        strnlen(sa->sun_path, حجم(sa->sun_path))+1);
}

size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa) {
        أكد(sa);

        انتخاب (sa->sa.sa_family) {
        حالت AF_INET:
                واپس حجم(ساخت sockaddr_in);
        حالت AF_INET6:
                واپس حجم(ساخت sockaddr_in6);
        حالت AF_UNIX:
                واپس sockaddr_un_len(&sa->un);
        حالت AF_PACKET:
                واپس sockaddr_ll_len(&sa->ll);
        حالت AF_NETLINK:
                واپس حجم(ساخت sockaddr_nl);
        حالت AF_VSOCK:
                واپس حجم(ساخت sockaddr_vm);
        طےشدہ:
                assert_not_reached();
        }
}

صحیح socket_ioctl_fd(خالی) {
        صحیح fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        اگر (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        اگر (fd < 0)
                واپس -errno;

        واپس fd;
}

صحیح sockaddr_un_unlink(ثابت ساخت sockaddr_un *sa) {
        ثابت حرف *p, * nul;

        أكد(sa);

        اگر (sa->sun_family != AF_UNIX)
                واپس -EPROTOTYPE;

        اگر (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                واپس 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, حجم(sa->sun_path));
        اگر (nul)
                p = sa->sun_path;
        ورنہ
                p = memdupa_suffix0(sa->sun_path, حجم(sa->sun_path));

        اگر (unlink(p) < 0)
                واپس -errno;

        واپس 1;
}

صحیح sockaddr_un_سیٹ_راستہ(ساخت sockaddr_un *ret, ثابت حرف *راستہ) {
        size_t l;

        أكد(ret);
        أكد(راستہ);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = لڑی_لمبائی(راستہ);
        اگر (l < 2)
                واپس -EINVAL;
        اگر (!اندر_سیٹ(راستہ[0], '/', '@'))
                واپس -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        اگر (l+1 > حجم(ret->sun_path))
                واپس راستہ[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ساخت sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        اگر (راستہ[0] == '@') {
                /* Abstract namespace socket */
                حافظہ_نقل(ret->sun_path + 1, راستہ + 1, l); /* copy *with* trailing NUL byte */
                واپس (صحیح) (offsetof(ساخت sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ورنہ {
                أكد(راستہ[0] == '/');

                /* File system socket */
                حافظہ_نقل(ret->sun_path, راستہ, l + 1); /* copy *with* trailing NUL byte */
                واپس (صحیح) (offsetof(ساخت sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

صحیح getsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح *ret) {
        صحیح v;
        socklen_t sl = حجم(v);

        أكد(fd >= 0);
        أكد(ret);

        اگر (getsockopt(fd, level, optname, &v, &sl) < 0)
                واپس negative_errno();
        اگر (sl != حجم(v))
                واپس -EIO;

        *ret = v;
        واپس 0;
}

صحیح socket_bind_مقصد_ifname(صحیح fd, ثابت حرف *ifname) {
        أكد(fd >= 0);

        /* Call with NULL to drop binding */

        واپس RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

صحیح socket_bind_مقصد_ifindex(صحیح fd, صحیح ifindex) {
        أكد(fd >= 0);

        اگر (ifindex <= 0)
                /* Drop binding */
                واپس RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, تہی, 0));

        واپس setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

صحیح socket_autobind(صحیح fd, حرف **ret_name) {
        _cleanup_free_ حرف *name = تہی;
        uint64_t random;
        صحیح r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        أكد(fd >= 0);

        random = random_u64();

        اگر (asprintf(&name, "@%" PRIu64, random) < 0)
                واپس -ENOMEM;

        اتحاد sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < حجم(sa.un.sun_path));

        r = sockaddr_un_سیٹ_راستہ(&sa.un, name);
        اگر (r < 0)
                واپس r;

        اگر (bind(fd, &sa.sa, r) < 0)
                واپس -errno;

        اگر (ret_name)
                *ret_name = TAKE_PTR(name);
        واپس 0;
}

ssize_t recvmsg_محفوظ(صحیح sockfd, ساخت msghdr *msg, صحیح flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        أكد(sockfd >= 0);
        أكد(msg);

        n = recvmsg(sockfd, msg, flags);
        اگر (n < 0)
                واپس -errno;

        اگر (FLAGS_سیٹ(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_سیٹ(flags, MSG_PEEK) && FLAGS_سیٹ(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                واپس FLAGS_سیٹ(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        واپس n;
}

صحیح socket_get_family(صحیح fd) {
        صحیح af;
        socklen_t sl = حجم(af);

        اگر (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                واپس -errno;

        اگر (sl != حجم(af))
                واپس -EINVAL;

        واپس af;
}

صحیح socket_سیٹ_recvpktinfo(صحیح fd, صحیح af, bool b) {

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        واپس af;
        }

        انتخاب (af) {

        حالت AF_INET:
                واپس setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        حالت AF_INET6:
                واپس setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        حالت AF_NETLINK:
                واپس setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        حالت AF_PACKET:
                واپس setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح socket_سیٹ_unicast_if(صحیح fd, صحیح af, صحیح ifi) {
        be32_t ifindex_be = htobe32(ifi);

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        واپس af;
        }

        انتخاب (af) {

        حالت AF_INET:
                واپس RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        حالت AF_INET6:
                واپس RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح socket_سیٹ_option(صحیح fd, صحیح af, صحیح opt_ipv4, صحیح opt_ipv6, صحیح val) {
        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        واپس af;
        }

        انتخاب (af) {

        حالت AF_INET:
                واپس setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        حالت AF_INET6:
                واپس setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }
}

صحیح socket_get_mtu(صحیح fd, صحیح af, size_t *ret) {
        صحیح mtu, r;

        أكد(ret);

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        واپس af;
        }

        انتخاب (af) {

        حالت AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                توڑو;

        حالت AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                توڑو;

        طےشدہ:
                واپس -EAFNOSUPPORT;
        }

        اگر (r < 0)
                واپس r;
        اگر (mtu <= 0)
                واپس -EINVAL;

        *ret = (size_t) mtu;
        واپس 0;
}

ساکن صحیح connect_unix_راستہ_simple(صحیح fd, ثابت حرف *راستہ) {
        اتحاد sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        أكد(fd >= 0);
        أكد(راستہ);

        l = لڑی_لمبائی(راستہ);
        أكد(l > 0);
        أكد(l < حجم(sa.un.sun_path));

        حافظہ_نقل(sa.un.sun_path, راستہ, l + 1);
        واپس RET_NERRNO(connect(fd, &sa.sa, offsetof(ساخت sockaddr_un, sun_path) + l + 1));
}

ساکن صحیح connect_unix_inode(صحیح fd, صحیح inode_fd) {
        أكد(fd >= 0);
        أكد(inode_fd >= 0);

        واپس connect_unix_راستہ_simple(fd, FORMAT_PROC_FD_راستہ(inode_fd));
}

صحیح connect_unix_راستہ(صحیح fd, صحیح dir_fd, ثابت حرف *راستہ) {
        _cleanup_close_ صحیح inode_fd = -EBADF;

        أكد(fd >= 0);
        أكد(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        اگر (!راستہ) {
                اگر (dir_fd < 0)
                        واپس -EISDIR;

                واپس connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        اگر (isempty(راستہ))
                واپس -EINVAL;

        /* Shortcut for the simple case */
        اگر (dir_fd == AT_FDCWD && لڑی_لمبائی(راستہ) < sizeof_میدان(ساخت sockaddr_un, sun_path))
                واپس connect_unix_راستہ_simple(fd, راستہ);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        اگر (dir_fd == XAT_FDROOT) {
                _cleanup_free_ حرف *j = strjoin("/", راستہ);
                اگر (!j)
                        واپس -ENOMEM;

                inode_fd = open(j, O_راستہ|O_CLOEXEC);
        } ورنہ
                inode_fd = openat(dir_fd, راستہ, O_راستہ|O_CLOEXEC);
        اگر (inode_fd < 0)
                واپس -errno;

        واپس connect_unix_inode(fd, inode_fd);
}

صحیح socket_پتہ_parse_unix(Socket_پتہ *ret_پتہ, ثابت حرف *s) {
        ساخت sockaddr_un un;
        صحیح r;

        أكد(ret_پتہ);
        أكد(s);

        اگر (!اندر_سیٹ(*s, '/', '@'))
                واپس -EPROTO;

        r = sockaddr_un_سیٹ_راستہ(&un, s);
        اگر (r < 0)
                واپس r;

        *ret_پتہ = (Socket_پتہ) {
                .sockaddr.un = un,
                .سائز = r,
        };

        واپس 0;
}

صحیح socket_پتہ_equal_unix(ثابت حرف *a, ثابت حرف *b) {
        Socket_پتہ socket_a, socket_b;
        صحیح r;

        أكد(a);
        أكد(b);

        r = socket_پتہ_parse_unix(&socket_a, a);
        اگر (r < 0)
                واپس r;

        r = socket_پتہ_parse_unix(&socket_b, b);
        اگر (r < 0)
                واپس r;

        واپس sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

صحیح vsock_parse_پورٹ(ثابت حرف *s, غیرعلامتی *ret) {
        صحیح r;

        أكد(ret);

        اگر (!s)
                واپس -EINVAL;

        غیرعلامتی u;
        r = محفوظ_atou(s, &u);
        اگر (r < 0)
                واپس r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        اگر (u == VMADDR_PORT_ANY)
                واپس -EINVAL;

        *ret = u;
        واپس 0;
}

صحیح vsock_parse_cid(ثابت حرف *s, غیرعلامتی *ret) {
        أكد(ret);

        اگر (!s)
                واپس -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        اگر (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ورنہ اگر (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ورنہ اگر (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ورنہ اگر (STR_اندر_سیٹ(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ورنہ
                واپس محفوظ_atou(s, ret);

        واپس 0;
}

صحیح socket_پتہ_parse_vsock(Socket_پتہ *ret_پتہ, ثابت حرف *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ حرف *n = تہی;
        ثابت حرف *e, *cid_آغاز;
        غیرعلامتی port, cid;
        صحیح type, r;

        أكد(ret_پتہ);
        أكد(s);

        اگر ((cid_آغاز = startswith(s, "vsock:")))
                type = 0;
        ورنہ اگر ((cid_آغاز = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ورنہ اگر ((cid_آغاز = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ورنہ اگر ((cid_آغاز = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ورنہ
                واپس -EPROTO;

        e = strchr(cid_آغاز, ':');
        اگر (!e)
                واپس -EINVAL;

        r = vsock_parse_پورٹ(e+1, &port);
        اگر (r < 0)
                واپس r;

        n = strndup(cid_آغاز, e - cid_آغاز);
        اگر (!n)
                واپس -ENOMEM;

        اگر (isempty(n))
                cid = VMADDR_CID_ANY;
        ورنہ {
                r = vsock_parse_cid(n, &cid);
                اگر (r < 0)
                        واپس r;
        }

        *ret_پتہ = (Socket_پتہ) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .سائز = حجم(ساخت sockaddr_vm),
        };

        واپس 0;
}

صحیح netlink_socket_get_multicast_groups(صحیح fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = تہی;
        socklen_t len = 0, old_len;

        أكد(fd >= 0);

        اگر (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, تہی, &len) < 0)
                واپس -errno;

        اگر (len == 0)
                جاؤ finalize;

        groups = new0(uint32_t, len);
        اگر (!groups)
                واپس -ENOMEM;

        old_len = len;

        اگر (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                واپس -errno;

        اگر (old_len != len)
                واپس -EIO;

finalize:
        اگر (ret_len)
                *ret_len = len;
        اگر (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        واپس 0;
}

صحیح socket_get_cookie(صحیح fd, uint64_t *ret) {
        أكد(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = حجم(cookie);
        اگر (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                واپس -errno;

        أكد(cookie_len == حجم(cookie));
        اگر (ret)
                *ret = cookie;

        واپس 0;
}

خالی cmsg_close_all(ساخت msghdr *mh) {
        أكد(mh);

        ساخت cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                اگر (cmsg->cmsg_level != SOL_SOCKET)
                        جاری;

                اگر (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_ڈیٹا(cmsg, صحیح),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / حجم(صحیح));
                ورنہ اگر (cmsg->cmsg_type == SCM_PIDFD) {
                        أكد(cmsg->cmsg_len == CMSG_LEN(حجم(صحیح)));
                        محفوظ_close(*CMSG_TYPED_ڈیٹا(cmsg, صحیح));
                }
        }
}

صحیح tos_مقصد_ترجیح(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        انتخاب (IPTOS_PREC(tos)) {
        حالت IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        حالت IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                واپس TC_PRIO_CONTROL;

        حالت IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        حالت IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                واپس TC_PRIO_INTERACTIVE;

        حالت IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        حالت IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                واپس TC_PRIO_INTERACTIVE_BULK;

        حالت IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                واپس TC_PRIO_BULK;

        حالت IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        طےشدہ:
                واپس TC_PRIO_BESTEFFORT;
        }
}

صحیح socket_xattr_supported(خالی) {
        صحیح r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ساکن صحیح cached = -1;
        اگر (cached >= 0)
                واپس cached;

        _cleanup_close_ صحیح fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        اگر (fd < 0)
                واپس -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ تہی, AT_خالی_خانے_راستہ, "user.testxxx", "1");
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                واپس (cached = false);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to set test xattr on socket: %m");

        واپس (cached = true);
}
