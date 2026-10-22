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
#وگرنه
#  define IDN_FLAGS 0
#endif

ایستا ثابت نویسه* ثابت socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, صحیح);

صحیح socket_address_verify(ثابت SocketAddress *a, bool strict) {
        أكد(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        انتخاب (socket_address_family(a)) {

        حالت AF_INET:
                اگر (a->size != اندازه(ساختار sockaddr_in))
                        بازگشت -EINVAL;

                اگر (a->sockaddr.in.sin_port == 0)
                        بازگشت -EINVAL;

                اگر (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        بازگشت -EINVAL;

                بازگشت 0;

        حالت AF_INET6:
                اگر (a->size != اندازه(ساختار sockaddr_in6))
                        بازگشت -EINVAL;

                اگر (a->sockaddr.in6.sin6_port == 0)
                        بازگشت -EINVAL;

                اگر (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        بازگشت -EINVAL;

                بازگشت 0;

        حالت AF_UNIX:
                اگر (a->size < offsetof(ساختار sockaddr_un, sun_path))
                        بازگشت -EINVAL;
                اگر (a->size > اندازه(ساختار sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        بازگشت -EINVAL;

                اگر (a->size > offsetof(ساختار sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ثابت نویسه *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, اندازه(a->sockaddr.un.sun_path));
                        اگر (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                اگر (a->size != offsetof(ساختار sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        بازگشت -EINVAL;
                        } وگرنه {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                اگر (!IN_SET(a->size, اندازه(a->sockaddr.un.sun_path), اندازه(a->sockaddr.un.sun_path)+1))
                                        بازگشت -EINVAL;
                        }
                }

                اگر (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        بازگشت -EINVAL;

                بازگشت 0;

        حالت AF_NETLINK:

                اگر (a->size != اندازه(ساختار sockaddr_nl))
                        بازگشت -EINVAL;

                اگر (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        بازگشت -EINVAL;

                بازگشت 0;

        حالت AF_VSOCK:
                اگر (a->size != اندازه(ساختار sockaddr_vm))
                        بازگشت -EINVAL;

                اگر (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        بازگشت -EINVAL;

                بازگشت 0;

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح socket_address_print(ثابت SocketAddress *a, نویسه **ret) {
        صحیح r;

        أكد(a);
        أكد(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        اگر (r < 0)
                بازگشت r;

        اگر (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ نویسه *sfamily = پوچ;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                اگر (r < 0)
                        بازگشت r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                اگر (r < 0)
                        بازگشت -ENOMEM;

                بازگشت 0;
        }

        بازگشت sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ثابت SocketAddress *a) {
        أكد(a);

        بازگشت
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ثابت SocketAddress *a, ثابت SocketAddress *b) {
        أكد(a);
        أكد(b);

        /* Invalid addresses are unequal to all */
        اگر (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                بازگشت false;

        اگر (a->type != b->type)
                بازگشت false;

        اگر (socket_address_family(a) != socket_address_family(b))
                بازگشت false;

        انتخاب (socket_address_family(a)) {

        حالت AF_INET:
                اگر (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        بازگشت false;

                اگر (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        بازگشت false;

                بشکن;

        حالت AF_INET6:
                اگر (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, اندازه(a->sockaddr.in6.sin6_addr)) != 0)
                        بازگشت false;

                اگر (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        بازگشت false;

                بشکن;

        حالت AF_UNIX:
                اگر (a->size <= offsetof(ساختار sockaddr_un, sun_path) ||
                    b->size <= offsetof(ساختار sockaddr_un, sun_path))
                        بازگشت false;

                اگر ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        بازگشت false;

                اگر (a->sockaddr.un.sun_path[0]) {
                        اگر (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                بازگشت false;
                } وگرنه {
                        اگر (a->size != b->size)
                                بازگشت false;

                        اگر (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                بازگشت false;
                }

                بشکن;

        حالت AF_NETLINK:
                اگر (a->protocol != b->protocol)
                        بازگشت false;

                اگر (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        بازگشت false;

                بشکن;

        حالت AF_VSOCK:
                اگر (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        بازگشت false;

                اگر (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        بازگشت false;

                بشکن;

        پیشفرض:
                /* Cannot compare, so we assume the addresses are different */
                بازگشت false;
        }

        بازگشت true;
}

ثابت نویسه* socket_address_get_path(ثابت SocketAddress *a) {
        أكد(a);

        اگر (socket_address_family(a) != AF_UNIX)
                بازگشت پوچ;

        اگر (a->sockaddr.un.sun_path[0] == 0)
                بازگشت پوچ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(اندازه(اجتماع sockaddr_union) >= اندازه(ساختار sockaddr_un)+1);
        بازگشت a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(تهی) {
        ایستا صحیح cached = -1;

        اگر (cached < 0) {

                اگر (access("/proc/net/if_inet6", F_OK) < 0) {

                        اگر (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                بازگشت false;
                        }

                        cached = false;
                } وگرنه
                        cached = true;
        }

        بازگشت cached;
}

bool socket_ipv6_is_enabled(تهی) {
        _cleanup_free_ نویسه *v = پوچ;
        صحیح r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        اگر (!socket_ipv6_is_supported())
                بازگشت false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        اگر (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                بازگشت true;
        }

        r = parse_boolean(v);
        اگر (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                بازگشت true;
        }

        بازگشت !r;
}

bool socket_address_matches_fd(ثابت SocketAddress *a, صحیح fd) {
        SocketAddress b;
        socklen_t solen;

        أكد(a);
        أكد(fd >= 0);

        b.size = اندازه(b.sockaddr);
        اگر (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                بازگشت false;

        اگر (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                بازگشت false;

        solen = اندازه(b.type);
        اگر (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                بازگشت false;

        اگر (b.type != a->type)
                بازگشت false;

        اگر (a->protocol != 0)  {
                solen = اندازه(b.protocol);
                اگر (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        بازگشت false;

                اگر (b.protocol != a->protocol)
                        بازگشت false;
        }

        بازگشت socket_address_equal(a, &b);
}

صحیح sockaddr_port(ثابت ساختار sockaddr *_sa, unsigned *ret_port) {
        ثابت اجتماع sockaddr_union *sa = (ثابت اجتماع sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        أكد(sa);
        أكد(ret_port);

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                بازگشت 0;

        حالت AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                بازگشت 0;

        حالت AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                بازگشت 0;

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

ثابت اجتماع in_addr_union *sockaddr_in_addr(ثابت ساختار sockaddr *_sa) {
        ثابت اجتماع sockaddr_union *sa = (ثابت اجتماع sockaddr_union*) _sa;

        اگر (!sa)
                بازگشت پوچ;

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET:
                بازگشت (ثابت اجتماع in_addr_union*) &sa->in.sin_addr;

        حالت AF_INET6:
                بازگشت (ثابت اجتماع in_addr_union*) &sa->in6.sin6_addr;

        پیشفرض:
                بازگشت پوچ;
        }
}

صحیح sockaddr_set_in_addr(
                اجتماع sockaddr_union *u,
                صحیح family,
                ثابت اجتماع in_addr_union *a,
                uint16_t port) {

        أكد(u);
        أكد(a);

        انتخاب (family) {

        حالت AF_INET:
                u->in = (ساختار sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                بازگشت 0;

        حالت AF_INET6:
                u->in6 = (ساختار sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                بازگشت 0;

        پیشفرض:
                بازگشت -EAFNOSUPPORT;

        }
}

صحیح sockaddr_pretty(
                ثابت ساختار sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                نویسه **ret) {

        اجتماع sockaddr_union *sa = (اجتماع sockaddr_union*) _sa;
        نویسه *p;
        صحیح r;

        أكد(sa);
        أكد(salen >= اندازه(sa->sa.sa_family));
        أكد(ret);

        انتخاب (sa->sa.sa_family) {

        حالت AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                اگر (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                وگرنه
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                اگر (r < 0)
                        بازگشت -ENOMEM;
                بشکن;
        }

        حالت AF_INET6: {
                ایستا ثابت unsigned نویسه ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                اگر (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, اندازه(ipv4_prefix)) == 0) {
                        ثابت uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        اگر (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        وگرنه
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        اگر (r < 0)
                                بازگشت -ENOMEM;
                } وگرنه {
                        ثابت نویسه *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        اگر (include_port) {
                                اگر (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        بازگشت -ENOMEM;
                        } وگرنه {
                                اگر (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                وگرنه
                                        p = strdup(a);
                                اگر (!p)
                                        بازگشت -ENOMEM;
                        }
                }

                بشکن;
        }

        حالت AF_UNIX:
                اگر (salen <= offsetof(ساختار sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ساختار sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                وگرنه {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        نویسه *path = (نویسه*) sa->un_buffer + offsetof(ساختار sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(ساختار sockaddr_un, sun_path);

                        اگر (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ نویسه *e = پوچ;

                                e = cescape_length(path + 1, path_len - 1);
                                اگر (!e)
                                        بازگشت -ENOMEM;

                                p = strjoin("@", e);
                        } وگرنه {
                                اگر (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                اگر (!p)
                        بازگشت -ENOMEM;

                بشکن;

        حالت AF_VSOCK:
                اگر (include_port) {
                        اگر (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        وگرنه
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } وگرنه
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                اگر (r < 0)
                        بازگشت -ENOMEM;
                بشکن;

        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }

        *ret = p;
        بازگشت 0;
}

صحیح getpeername_pretty(صحیح fd, bool include_port, نویسه **ret) {
        اجتماع sockaddr_union sa;
        socklen_t salen = اندازه(sa);
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        اگر (getpeername(fd, &sa.sa, &salen) < 0)
                بازگشت -errno;

        اگر (sa.sa.sa_family == AF_UNIX) {
                ساختار ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                اگر (r < 0)
                        بازگشت r;

                اگر (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        بازگشت -ENOMEM;

                بازگشت 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        بازگشت sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

صحیح getsockname_pretty(صحیح fd, نویسه **ret) {
        اجتماع sockaddr_union sa;
        socklen_t salen = اندازه(sa);

        أكد(fd >= 0);
        أكد(ret);

        اگر (getsockname(fd, &sa.sa, &salen) < 0)
                بازگشت -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        بازگشت sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

صحیح socknameinfo_pretty(ثابت ساختار sockaddr *sa, socklen_t salen, نویسه **ret) {
        نویسه host[NI_MAXHOST];
        صحیح r;

        أكد(sa);
        أكد(salen >= اندازه(sa_family_t));
        أكد(ret);

        r = getnameinfo(sa, salen, host, اندازه(host), /* service= */ پوچ, /* service_len= */ 0, IDN_FLAGS);
        اگر (r != 0) {
                اگر (r == EAI_MEMORY)
                        بازگشت log_oom_debug();
                اگر (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                وگرنه
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                بازگشت sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        بازگشت strdup_to(ret, host);
}

ایستا ثابت نویسه* ثابت netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, صحیح, INT_MAX);

bool sockaddr_equal(ثابت اجتماع sockaddr_union *a, ثابت اجتماع sockaddr_union *b) {
        أكد(a);
        أكد(b);

        اگر (a->sa.sa_family != b->sa.sa_family)
                بازگشت false;

        اگر (a->sa.sa_family == AF_INET)
                بازگشت a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        اگر (a->sa.sa_family == AF_INET6)
                بازگشت memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, اندازه(a->in6.sin6_addr)) == 0;

        اگر (a->sa.sa_family == AF_VSOCK)
                بازگشت a->vm.svm_cid == b->vm.svm_cid;

        بازگشت false;
}

صحیح fd_set_sndbuf(صحیح fd, size_t n, bool increase) {
        صحیح r, مقدار;
        socklen_t l = اندازه(مقدار);

        اگر (n > INT_MAX)
                بازگشت -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &مقدار, &l);
        اگر (r >= 0 && l == اندازه(مقدار) && increase ? (size_t) مقدار >= n*2 : (size_t) مقدار == n*2)
                بازگشت 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        اگر (r < 0)
                بازگشت r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = اندازه(مقدار);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &مقدار, &l);
        اگر (r >= 0 && l == اندازه(مقدار) && increase ? (size_t) مقدار >= n*2 : (size_t) مقدار == n*2)
                بازگشت 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

صحیح fd_set_rcvbuf(صحیح fd, size_t n, bool increase) {
        صحیح r, مقدار;
        socklen_t l = اندازه(مقدار);

        اگر (n > INT_MAX)
                بازگشت -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &مقدار, &l);
        اگر (r >= 0 && l == اندازه(مقدار) && increase ? (size_t) مقدار >= n*2 : (size_t) مقدار == n*2)
                بازگشت 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        اگر (r < 0)
                بازگشت r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = اندازه(مقدار);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &مقدار, &l);
        اگر (r >= 0 && l == اندازه(مقدار) && increase ? (size_t) مقدار >= n*2 : (size_t) مقدار == n*2)
                بازگشت 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

ایستا ثابت نویسه* ثابت ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, صحیح, 0xff);

bool ifname_valid_char(نویسه a) {
        اگر ((unsigned نویسه) a >= 127U)
                بازگشت false;

        اگر ((unsigned نویسه) a <= 32U)
                بازگشت false;

        اگر (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                بازگشت false;

        بازگشت true;
}

bool ifname_valid_full(ثابت نویسه *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        أكد(!(flags & ~_IFNAME_VALID_ALL));

        اگر (isempty(p))
                بازگشت false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        اگر (parse_ifindex(p) >= 0)
                بازگشت flags & IFNAME_VALID_NUMERIC;

        اگر (flags & IFNAME_VALID_ALTERNATIVE) {
                اگر (strlen(p) >= ALTIFNAMSIZ)
                        بازگشت false;
        } وگرنه {
                اگر (strlen(p) >= IFNAMSIZ)
                        بازگشت false;
        }

        اگر (dot_or_dot_dot(p))
                بازگشت false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        اگر (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                بازگشت false;

        برای (ثابت نویسه *t = p; *t; t++) {
                اگر (!ifname_valid_char(*t))
                        بازگشت false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        اگر (numeric)
                بازگشت false;

        بازگشت true;
}

bool address_label_valid(ثابت نویسه *p) {

        POINTER_MAY_BE_NULL(p);

        اگر (isempty(p))
                بازگشت false;

        اگر (strlen(p) >= IFNAMSIZ)
                بازگشت false;

        وقتی (*p) {
                اگر ((uint8_t) *p >= 127U)
                        بازگشت false;

                اگر ((uint8_t) *p <= 31U)
                        بازگشت false;
                p++;
        }

        بازگشت true;
}

صحیح getpeercred(صحیح fd, ساختار ucred *ucred) {
        socklen_t n = اندازه(ساختار ucred);
        ساختار ucred u;

        أكد(fd >= 0);
        أكد(ucred);

        اگر (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                بازگشت -errno;

        اگر (n != اندازه(ساختار ucred))
                بازگشت -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        اگر (!pid_is_valid(u.pid))
                بازگشت -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        بازگشت 0;
}

صحیح getpeersec(صحیح fd, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        socklen_t n = 64;

        أكد(fd >= 0);
        أكد(ret);

        برای (;;) {
                s = new0(نویسه, n+1);
                اگر (!s)
                        بازگشت -ENOMEM;

                اگر (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        بشکن;
                }

                اگر (errno != ERANGE)
                        بازگشت -errno;

                s = mfree(s);
        }

        اگر (isempty(s))
                بازگشت -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        بازگشت 0;
}

صحیح getpeergroups(صحیح fd, gid_t **ret) {
        socklen_t n = اندازه(gid_t) * 64U;
        _cleanup_free_ gid_t *d = پوچ;

        أكد(fd >= 0);
        أكد(ret);

        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max > 0)
                n = MAX(n, اندازه(gid_t) * (socklen_t) ngroups_max);

        برای (;;) {
                d = malloc(n);
                اگر (!d)
                        بازگشت -ENOMEM;

                اگر (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        بشکن;

                اگر (errno != ERANGE)
                        بازگشت -errno;

                d = mfree(d);
        }

        assert_se(n % اندازه(gid_t) == 0);
        n /= اندازه(gid_t);

        اگر (n > INT_MAX)
                بازگشت -E2BIG;

        *ret = TAKE_PTR(d);

        بازگشت (صحیح) n;
}

صحیح getpeerpidfd(صحیح fd) {
        socklen_t n = اندازه(صحیح);
        صحیح pidfd = -EBADF;

        أكد(fd >= 0);

        اگر (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                بازگشت -errno;

        اگر (n != اندازه(صحیح))
                بازگشت -EIO;

        بازگشت pidfd;
}

صحیح getpeerpidref(صحیح fd, PidRef *ret) {
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        صحیح pidfd = getpeerpidfd(fd);
        اگر (pidfd < 0) {
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        بازگشت pidfd;

                ساختار ucred ucred;
                r = getpeercred(fd, &ucred);
                اگر (r < 0)
                        بازگشت r;

                بازگشت pidref_set_pid(ret, ucred.pid);
        }

        بازگشت pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساختار iovec *iov, size_t iovlen,
                ثابت ساختار sockaddr *sa, socklen_t len,
                صحیح flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(اندازه(صحیح))) control = {};
        ساختار msghdr mh = {
                .msg_name = (ساختار sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ساختار iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        أكد(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        اگر (fd < 0 && !iov)
                بازگشت -EINVAL;

        اگر (fd >= 0) {
                ساختار cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = اندازه(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(اندازه(صحیح));
                memcpy(CMSG_DATA(cmsg), &fd, اندازه(صحیح));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        اگر (k < 0)
                بازگشت (ssize_t) -errno;

        بازگشت k;
}

صحیح send_one_fd_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساختار sockaddr *sa, socklen_t len,
                صحیح flags) {

        أكد(fd >= 0);

        بازگشت (صحیح) send_one_fd_iov_sa(transport_fd, fd, پوچ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                صحیح transport_fd,
                ساختار iovec *iov, size_t iovlen,
                صحیح flags,
                صحیح *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(اندازه(صحیح))) control;
        ساختار msghdr mh = {
                .msg_control = &control,
                .msg_controllen = اندازه(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ساختار cmsghdr *found;
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
        اگر (k < 0)
                بازگشت k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(اندازه(صحیح)));
        اگر (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                اگر (k == 0)
                        بازگشت -EIO;
        }

        اگر (found)
                *ret_fd = *CMSG_TYPED_DATA(found, صحیح);
        وگرنه
                *ret_fd = -EBADF;

        بازگشت k;
}

صحیح receive_one_fd(صحیح transport_fd, صحیح flags) {
        صحیح fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, پوچ, 0, flags, &fd);
        اگر (k == 0)
                بازگشت fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        أكد(k < 0);
        بازگشت (صحیح) k;
}

ssize_t next_datagram_size_fd(صحیح fd) {
        ssize_t l;
        صحیح k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, پوچ, 0, MSG_PEEK|MSG_TRUNC);
        اگر (l < 0) {
                اگر (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        برو fallback;

                بازگشت -errno;
        }
        اگر (l == 0)
                برو fallback;

        بازگشت l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        اگر (ioctl(fd, FIONREAD, &k) < 0)
                بازگشت -errno;

        بازگشت (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

صحیح flush_accept(صحیح fd) {

        صحیح r, b;
        socklen_t l = اندازه(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        اگر (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                بازگشت -errno;

        أكد(l == اندازه(b));
        اگر (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                بازگشت -ENOTTY;

        برای (unsigned iteration = 0;; iteration++) {
                صحیح cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                اگر (r == -EINTR)
                        ادامه;
                اگر (r <= 0)
                        بازگشت r;

                اگر (iteration >= MAX_FLUSH_ITERATIONS)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, پوچ, پوچ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                اگر (cfd < 0) {
                        اگر (errno == EAGAIN)
                                بازگشت 0;

                        اگر (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ادامه;

                        بازگشت -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(صحیح fd) {
        _cleanup_free_ نویسه *buf = پوچ;
        ساختار mq_attr attr;
        ssize_t شمار = 0;
        صحیح r;

        أكد(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        برای (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                اگر (r == -EINTR)
                        ادامه;
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت شمار;

                اگر (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        اگر (mq_getattr(fd, &attr) < 0)
                                بازگشت -errno;

                        buf = malloc(attr.mq_msgsize);
                        اگر (!buf)
                                بازگشت -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ پوچ);
                اگر (l < 0) {
                        اگر (errno == EINTR)
                                ادامه;

                        اگر (errno == EAGAIN)
                                بازگشت شمار;

                        بازگشت -errno;
                }

                شمار += l;
        }
}

ساختار cmsghdr* cmsg_find(ساختار msghdr *mh, صحیح level, صحیح type, socklen_t length) {
        ساختار cmsghdr *cmsg;

        أكد(mh);

        CMSG_FOREACH(cmsg, mh)
                اگر (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        بازگشت cmsg;

        بازگشت پوچ;
}

تهی* cmsg_find_and_copy_data(ساختار msghdr *mh, صحیح level, صحیح type, تهی *buf, size_t buf_len) {
        ساختار cmsghdr *cmsg;

        أكد(mh);
        أكد(buf);
        أكد(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        اگر (!cmsg)
                بازگشت پوچ;

        بازگشت memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ثابت ساختار sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        أكد(sa->sll_family == AF_PACKET);

        size_t mac_len = اندازه(sa->sll_addr);

        اگر (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        اگر (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        بازگشت offsetof(ساختار sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ثابت ساختار sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        أكد(sa->sun_family == AF_UNIX);

        بازگشت offsetof(ساختار sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, اندازه(sa->sun_path)-1) :
                        strnlen(sa->sun_path, اندازه(sa->sun_path))+1);
}

size_t sockaddr_len(ثابت اجتماع sockaddr_union *sa) {
        أكد(sa);

        انتخاب (sa->sa.sa_family) {
        حالت AF_INET:
                بازگشت اندازه(ساختار sockaddr_in);
        حالت AF_INET6:
                بازگشت اندازه(ساختار sockaddr_in6);
        حالت AF_UNIX:
                بازگشت sockaddr_un_len(&sa->un);
        حالت AF_PACKET:
                بازگشت sockaddr_ll_len(&sa->ll);
        حالت AF_NETLINK:
                بازگشت اندازه(ساختار sockaddr_nl);
        حالت AF_VSOCK:
                بازگشت اندازه(ساختار sockaddr_vm);
        پیشفرض:
                assert_not_reached();
        }
}

صحیح socket_ioctl_fd(تهی) {
        صحیح fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        اگر (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        اگر (fd < 0)
                بازگشت -errno;

        بازگشت fd;
}

صحیح sockaddr_un_unlink(ثابت ساختار sockaddr_un *sa) {
        ثابت نویسه *p, * nul;

        أكد(sa);

        اگر (sa->sun_family != AF_UNIX)
                بازگشت -EPROTOTYPE;

        اگر (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                بازگشت 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, اندازه(sa->sun_path));
        اگر (nul)
                p = sa->sun_path;
        وگرنه
                p = memdupa_suffix0(sa->sun_path, اندازه(sa->sun_path));

        اگر (unlink(p) < 0)
                بازگشت -errno;

        بازگشت 1;
}

صحیح sockaddr_un_set_path(ساختار sockaddr_un *ret, ثابت نویسه *path) {
        size_t l;

        أكد(ret);
        أكد(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        اگر (l < 2)
                بازگشت -EINVAL;
        اگر (!IN_SET(path[0], '/', '@'))
                بازگشت -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        اگر (l+1 > اندازه(ret->sun_path))
                بازگشت path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ساختار sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        اگر (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                بازگشت (صحیح) (offsetof(ساختار sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } وگرنه {
                أكد(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                بازگشت (صحیح) (offsetof(ساختار sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

صحیح getsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح *ret) {
        صحیح v;
        socklen_t sl = اندازه(v);

        أكد(fd >= 0);
        أكد(ret);

        اگر (getsockopt(fd, level, optname, &v, &sl) < 0)
                بازگشت negative_errno();
        اگر (sl != اندازه(v))
                بازگشت -EIO;

        *ret = v;
        بازگشت 0;
}

صحیح socket_bind_to_ifname(صحیح fd, ثابت نویسه *ifname) {
        أكد(fd >= 0);

        /* Call with NULL to drop binding */

        بازگشت RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

صحیح socket_bind_to_ifindex(صحیح fd, صحیح ifindex) {
        أكد(fd >= 0);

        اگر (ifindex <= 0)
                /* Drop binding */
                بازگشت RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, پوچ, 0));

        بازگشت setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

صحیح socket_autobind(صحیح fd, نویسه **ret_name) {
        _cleanup_free_ نویسه *name = پوچ;
        uint64_t random;
        صحیح r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        أكد(fd >= 0);

        random = random_u64();

        اگر (asprintf(&name, "@%" PRIu64, random) < 0)
                بازگشت -ENOMEM;

        اجتماع sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < اندازه(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        اگر (r < 0)
                بازگشت r;

        اگر (bind(fd, &sa.sa, r) < 0)
                بازگشت -errno;

        اگر (ret_name)
                *ret_name = TAKE_PTR(name);
        بازگشت 0;
}

ssize_t recvmsg_safe(صحیح sockfd, ساختار msghdr *msg, صحیح flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        أكد(sockfd >= 0);
        أكد(msg);

        n = recvmsg(sockfd, msg, flags);
        اگر (n < 0)
                بازگشت -errno;

        اگر (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                بازگشت FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        بازگشت n;
}

صحیح socket_get_family(صحیح fd) {
        صحیح af;
        socklen_t sl = اندازه(af);

        اگر (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                بازگشت -errno;

        اگر (sl != اندازه(af))
                بازگشت -EINVAL;

        بازگشت af;
}

صحیح socket_set_recvpktinfo(صحیح fd, صحیح af, bool b) {

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        بازگشت af;
        }

        انتخاب (af) {

        حالت AF_INET:
                بازگشت setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        حالت AF_INET6:
                بازگشت setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        حالت AF_NETLINK:
                بازگشت setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        حالت AF_PACKET:
                بازگشت setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح socket_set_unicast_if(صحیح fd, صحیح af, صحیح ifi) {
        be32_t ifindex_be = htobe32(ifi);

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        بازگشت af;
        }

        انتخاب (af) {

        حالت AF_INET:
                بازگشت RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, اندازه(ifindex_be)));

        حالت AF_INET6:
                بازگشت RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, اندازه(ifindex_be)));

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح socket_set_option(صحیح fd, صحیح af, صحیح opt_ipv4, صحیح opt_ipv6, صحیح val) {
        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        بازگشت af;
        }

        انتخاب (af) {

        حالت AF_INET:
                بازگشت setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        حالت AF_INET6:
                بازگشت setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }
}

صحیح socket_get_mtu(صحیح fd, صحیح af, size_t *ret) {
        صحیح mtu, r;

        أكد(ret);

        اگر (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                اگر (af < 0)
                        بازگشت af;
        }

        انتخاب (af) {

        حالت AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                بشکن;

        حالت AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                بشکن;

        پیشفرض:
                بازگشت -EAFNOSUPPORT;
        }

        اگر (r < 0)
                بازگشت r;
        اگر (mtu <= 0)
                بازگشت -EINVAL;

        *ret = (size_t) mtu;
        بازگشت 0;
}

ایستا صحیح connect_unix_path_simple(صحیح fd, ثابت نویسه *path) {
        اجتماع sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        أكد(fd >= 0);
        أكد(path);

        l = strlen(path);
        أكد(l > 0);
        أكد(l < اندازه(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        بازگشت RET_NERRNO(connect(fd, &sa.sa, offsetof(ساختار sockaddr_un, sun_path) + l + 1));
}

ایستا صحیح connect_unix_inode(صحیح fd, صحیح inode_fd) {
        أكد(fd >= 0);
        أكد(inode_fd >= 0);

        بازگشت connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

صحیح connect_unix_path(صحیح fd, صحیح dir_fd, ثابت نویسه *path) {
        _cleanup_close_ صحیح inode_fd = -EBADF;

        أكد(fd >= 0);
        أكد(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        اگر (!path) {
                اگر (dir_fd < 0)
                        بازگشت -EISDIR;

                بازگشت connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        اگر (isempty(path))
                بازگشت -EINVAL;

        /* Shortcut for the simple case */
        اگر (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(ساختار sockaddr_un, sun_path))
                بازگشت connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        اگر (dir_fd == XAT_FDROOT) {
                _cleanup_free_ نویسه *j = strjoin("/", path);
                اگر (!j)
                        بازگشت -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } وگرنه
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        اگر (inode_fd < 0)
                بازگشت -errno;

        بازگشت connect_unix_inode(fd, inode_fd);
}

صحیح socket_address_parse_unix(SocketAddress *ret_address, ثابت نویسه *s) {
        ساختار sockaddr_un un;
        صحیح r;

        أكد(ret_address);
        أكد(s);

        اگر (!IN_SET(*s, '/', '@'))
                بازگشت -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        اگر (r < 0)
                بازگشت r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        بازگشت 0;
}

صحیح socket_address_equal_unix(ثابت نویسه *a, ثابت نویسه *b) {
        SocketAddress socket_a, socket_b;
        صحیح r;

        أكد(a);
        أكد(b);

        r = socket_address_parse_unix(&socket_a, a);
        اگر (r < 0)
                بازگشت r;

        r = socket_address_parse_unix(&socket_b, b);
        اگر (r < 0)
                بازگشت r;

        بازگشت sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

صحیح vsock_parse_port(ثابت نویسه *s, unsigned *ret) {
        صحیح r;

        أكد(ret);

        اگر (!s)
                بازگشت -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        اگر (r < 0)
                بازگشت r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        اگر (u == VMADDR_PORT_ANY)
                بازگشت -EINVAL;

        *ret = u;
        بازگشت 0;
}

صحیح vsock_parse_cid(ثابت نویسه *s, unsigned *ret) {
        أكد(ret);

        اگر (!s)
                بازگشت -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        اگر (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        وگرنه اگر (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        وگرنه اگر (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        وگرنه اگر (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        وگرنه
                بازگشت safe_atou(s, ret);

        بازگشت 0;
}

صحیح socket_address_parse_vsock(SocketAddress *ret_address, ثابت نویسه *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ نویسه *n = پوچ;
        ثابت نویسه *e, *cid_start;
        unsigned port, cid;
        صحیح type, r;

        أكد(ret_address);
        أكد(s);

        اگر ((cid_start = startswith(s, "vsock:")))
                type = 0;
        وگرنه اگر ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        وگرنه اگر ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        وگرنه اگر ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        وگرنه
                بازگشت -EPROTO;

        e = strchr(cid_start, ':');
        اگر (!e)
                بازگشت -EINVAL;

        r = vsock_parse_port(e+1, &port);
        اگر (r < 0)
                بازگشت r;

        n = strndup(cid_start, e - cid_start);
        اگر (!n)
                بازگشت -ENOMEM;

        اگر (isempty(n))
                cid = VMADDR_CID_ANY;
        وگرنه {
                r = vsock_parse_cid(n, &cid);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = اندازه(ساختار sockaddr_vm),
        };

        بازگشت 0;
}

صحیح netlink_socket_get_multicast_groups(صحیح fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = پوچ;
        socklen_t len = 0, old_len;

        أكد(fd >= 0);

        اگر (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, پوچ, &len) < 0)
                بازگشت -errno;

        اگر (len == 0)
                برو finalize;

        groups = new0(uint32_t, len);
        اگر (!groups)
                بازگشت -ENOMEM;

        old_len = len;

        اگر (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                بازگشت -errno;

        اگر (old_len != len)
                بازگشت -EIO;

finalize:
        اگر (ret_len)
                *ret_len = len;
        اگر (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        بازگشت 0;
}

صحیح socket_get_cookie(صحیح fd, uint64_t *ret) {
        أكد(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = اندازه(cookie);
        اگر (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                بازگشت -errno;

        أكد(cookie_len == اندازه(cookie));
        اگر (ret)
                *ret = cookie;

        بازگشت 0;
}

تهی cmsg_close_all(ساختار msghdr *mh) {
        أكد(mh);

        ساختار cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                اگر (cmsg->cmsg_level != SOL_SOCKET)
                        ادامه;

                اگر (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, صحیح),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / اندازه(صحیح));
                وگرنه اگر (cmsg->cmsg_type == SCM_PIDFD) {
                        أكد(cmsg->cmsg_len == CMSG_LEN(اندازه(صحیح)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, صحیح));
                }
        }
}

صحیح tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        انتخاب (IPTOS_PREC(tos)) {
        حالت IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        حالت IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                بازگشت TC_PRIO_CONTROL;

        حالت IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        حالت IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                بازگشت TC_PRIO_INTERACTIVE;

        حالت IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        حالت IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                بازگشت TC_PRIO_INTERACTIVE_BULK;

        حالت IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                بازگشت TC_PRIO_BULK;

        حالت IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        پیشفرض:
                بازگشت TC_PRIO_BESTEFFORT;
        }
}

صحیح socket_xattr_supported(تهی) {
        صحیح r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ایستا صحیح cached = -1;
        اگر (cached >= 0)
                بازگشت cached;

        _cleanup_close_ صحیح fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        اگر (fd < 0)
                بازگشت -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ پوچ, AT_EMPTY_PATH, "user.testxxx", "1");
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                بازگشت (cached = false);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to set test xattr on socket: %m");

        بازگشت (cached = true);
}
