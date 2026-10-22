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

#ܐܢ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ܐܚܪܢܐ
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
                ܐܢ (a->size != حجم(بنية sockaddr_in))
                        ܦܢܐ -EINVAL;

                ܐܢ (a->sockaddr.in.sin_port == 0)
                        ܦܢܐ -EINVAL;

                ܐܢ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ܦܢܐ -EINVAL;

                ܦܢܐ 0;

        حالة AF_INET6:
                ܐܢ (a->size != حجم(بنية sockaddr_in6))
                        ܦܢܐ -EINVAL;

                ܐܢ (a->sockaddr.in6.sin6_port == 0)
                        ܦܢܐ -EINVAL;

                ܐܢ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ܦܢܐ -EINVAL;

                ܦܢܐ 0;

        حالة AF_UNIX:
                ܐܢ (a->size < offsetof(بنية sockaddr_un, sun_path))
                        ܦܢܐ -EINVAL;
                ܐܢ (a->size > حجم(بنية sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ܦܢܐ -EINVAL;

                ܐܢ (a->size > offsetof(بنية sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ثابت حرف *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, حجم(a->sockaddr.un.sun_path));
                        ܐܢ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ܐܢ (a->size != offsetof(بنية sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ܦܢܐ -EINVAL;
                        } ܐܚܪܢܐ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ܐܢ (!IN_SET(a->size, حجم(a->sockaddr.un.sun_path), حجم(a->sockaddr.un.sun_path)+1))
                                        ܦܢܐ -EINVAL;
                        }
                }

                ܐܢ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ܦܢܐ -EINVAL;

                ܦܢܐ 0;

        حالة AF_NETLINK:

                ܐܢ (a->size != حجم(بنية sockaddr_nl))
                        ܦܢܐ -EINVAL;

                ܐܢ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ܦܢܐ -EINVAL;

                ܦܢܐ 0;

        حالة AF_VSOCK:
                ܐܢ (a->size != حجم(بنية sockaddr_vm))
                        ܦܢܐ -EINVAL;

                ܐܢ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ܦܢܐ -EINVAL;

                ܦܢܐ 0;

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
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
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ حرف *sfamily = عدم;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ܐܢ (r < 0)
                        ܦܢܐ -ENOMEM;

                ܦܢܐ 0;
        }

        ܦܢܐ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ثابت SocketAddress *a) {
        أكد(a);

        ܦܢܐ
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ثابت SocketAddress *a, ثابت SocketAddress *b) {
        أكد(a);
        أكد(b);

        /* Invalid addresses are unequal to all */
        ܐܢ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ܦܢܐ false;

        ܐܢ (a->type != b->type)
                ܦܢܐ false;

        ܐܢ (socket_address_family(a) != socket_address_family(b))
                ܦܢܐ false;

        بدل (socket_address_family(a)) {

        حالة AF_INET:
                ܐܢ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ܦܢܐ false;

                ܐܢ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ܦܢܐ false;

                اكسر;

        حالة AF_INET6:
                ܐܢ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, حجم(a->sockaddr.in6.sin6_addr)) != 0)
                        ܦܢܐ false;

                ܐܢ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ܦܢܐ false;

                اكسر;

        حالة AF_UNIX:
                ܐܢ (a->size <= offsetof(بنية sockaddr_un, sun_path) ||
                    b->size <= offsetof(بنية sockaddr_un, sun_path))
                        ܦܢܐ false;

                ܐܢ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ܦܢܐ false;

                ܐܢ (a->sockaddr.un.sun_path[0]) {
                        ܐܢ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ܦܢܐ false;
                } ܐܚܪܢܐ {
                        ܐܢ (a->size != b->size)
                                ܦܢܐ false;

                        ܐܢ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ܦܢܐ false;
                }

                اكسر;

        حالة AF_NETLINK:
                ܐܢ (a->protocol != b->protocol)
                        ܦܢܐ false;

                ܐܢ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ܦܢܐ false;

                اكسر;

        حالة AF_VSOCK:
                ܐܢ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ܦܢܐ false;

                ܐܢ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ܦܢܐ false;

                اكسر;

        افتراضي:
                /* Cannot compare, so we assume the addresses are different */
                ܦܢܐ false;
        }

        ܦܢܐ true;
}

ثابت حرف* socket_address_get_path(ثابت SocketAddress *a) {
        أكد(a);

        ܐܢ (socket_address_family(a) != AF_UNIX)
                ܦܢܐ عدم;

        ܐܢ (a->sockaddr.un.sun_path[0] == 0)
                ܦܢܐ عدم;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(حجم(اتحاد sockaddr_union) >= حجم(بنية sockaddr_un)+1);
        ܦܢܐ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(فراغ) {
        ساكن صحيح cached = -1;

        ܐܢ (cached < 0) {

                ܐܢ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ܐܢ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ܦܢܐ false;
                        }

                        cached = false;
                } ܐܚܪܢܐ
                        cached = true;
        }

        ܦܢܐ cached;
}

bool socket_ipv6_is_enabled(فراغ) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ܐܢ (!socket_ipv6_is_supported())
                ܦܢܐ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ܐܢ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ܦܢܐ true;
        }

        r = parse_boolean(v);
        ܐܢ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ܦܢܐ true;
        }

        ܦܢܐ !r;
}

bool socket_address_matches_fd(ثابت SocketAddress *a, صحيح fd) {
        SocketAddress b;
        socklen_t solen;

        أكد(a);
        أكد(fd >= 0);

        b.size = حجم(b.sockaddr);
        ܐܢ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ܦܢܐ false;

        ܐܢ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ܦܢܐ false;

        solen = حجم(b.type);
        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ܦܢܐ false;

        ܐܢ (b.type != a->type)
                ܦܢܐ false;

        ܐܢ (a->protocol != 0)  {
                solen = حجم(b.protocol);
                ܐܢ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ܦܢܐ false;

                ܐܢ (b.protocol != a->protocol)
                        ܦܢܐ false;
        }

        ܦܢܐ socket_address_equal(a, &b);
}

صحيح sockaddr_port(ثابت بنية sockaddr *_sa, unsigned *ret_port) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        أكد(sa);
        أكد(ret_port);

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ܦܢܐ 0;

        حالة AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ܦܢܐ 0;

        حالة AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ܦܢܐ 0;

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
        }
}

ثابت اتحاد in_addr_union *sockaddr_in_addr(ثابت بنية sockaddr *_sa) {
        ثابت اتحاد sockaddr_union *sa = (ثابت اتحاد sockaddr_union*) _sa;

        ܐܢ (!sa)
                ܦܢܐ عدم;

        بدل (sa->sa.sa_family) {

        حالة AF_INET:
                ܦܢܐ (ثابت اتحاد in_addr_union*) &sa->in.sin_addr;

        حالة AF_INET6:
                ܦܢܐ (ثابت اتحاد in_addr_union*) &sa->in6.sin6_addr;

        افتراضي:
                ܦܢܐ عدم;
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

                ܦܢܐ 0;

        حالة AF_INET6:
                u->in6 = (بنية sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ܦܢܐ 0;

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;

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

                ܐܢ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                ܐܚܪܢܐ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ܐܢ (r < 0)
                        ܦܢܐ -ENOMEM;
                اكسر;
        }

        حالة AF_INET6: {
                ساكن ثابت unsigned حرف ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ܐܢ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, حجم(ipv4_prefix)) == 0) {
                        ثابت uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ܐܢ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ܐܚܪܢܐ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ܐܢ (r < 0)
                                ܦܢܐ -ENOMEM;
                } ܐܚܪܢܐ {
                        ثابت حرف *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ܐܢ (include_port) {
                                ܐܢ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ܦܢܐ -ENOMEM;
                        } ܐܚܪܢܐ {
                                ܐܢ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ܐܚܪܢܐ
                                        p = strdup(a);
                                ܐܢ (!p)
                                        ܦܢܐ -ENOMEM;
                        }
                }

                اكسر;
        }

        حالة AF_UNIX:
                ܐܢ (salen <= offsetof(بنية sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(بنية sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ܐܚܪܢܐ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        حرف *path = (حرف*) sa->un_buffer + offsetof(بنية sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(بنية sockaddr_un, sun_path);

                        ܐܢ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ حرف *e = عدم;

                                e = cescape_length(path + 1, path_len - 1);
                                ܐܢ (!e)
                                        ܦܢܐ -ENOMEM;

                                p = strjoin("@", e);
                        } ܐܚܪܢܐ {
                                ܐܢ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ܐܢ (!p)
                        ܦܢܐ -ENOMEM;

                اكسر;

        حالة AF_VSOCK:
                ܐܢ (include_port) {
                        ܐܢ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ܐܚܪܢܐ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ܐܚܪܢܐ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ܐܢ (r < 0)
                        ܦܢܐ -ENOMEM;
                اكسر;

        افتراضي:
                ܦܢܐ -EOPNOTSUPP;
        }

        *ret = p;
        ܦܢܐ 0;
}

صحيح getpeername_pretty(صحيح fd, bool include_port, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        ܐܢ (getpeername(fd, &sa.sa, &salen) < 0)
                ܦܢܐ -errno;

        ܐܢ (sa.sa.sa_family == AF_UNIX) {
                بنية ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ܦܢܐ -ENOMEM;

                ܦܢܐ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ܦܢܐ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

صحيح getsockname_pretty(صحيح fd, حرف **ret) {
        اتحاد sockaddr_union sa;
        socklen_t salen = حجم(sa);

        أكد(fd >= 0);
        أكد(ret);

        ܐܢ (getsockname(fd, &sa.sa, &salen) < 0)
                ܦܢܐ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ܦܢܐ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

صحيح socknameinfo_pretty(ثابت بنية sockaddr *sa, socklen_t salen, حرف **ret) {
        حرف host[NI_MAXHOST];
        صحيح r;

        أكد(sa);
        أكد(salen >= حجم(sa_family_t));
        أكد(ret);

        r = getnameinfo(sa, salen, host, حجم(host), /* service= */ عدم, /* service_len= */ 0, IDN_FLAGS);
        ܐܢ (r != 0) {
                ܐܢ (r == EAI_MEMORY)
                        ܦܢܐ log_oom_debug();
                ܐܢ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ܐܚܪܢܐ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ܦܢܐ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ܦܢܐ strdup_to(ret, host);
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

        ܐܢ (a->sa.sa_family != b->sa.sa_family)
                ܦܢܐ false;

        ܐܢ (a->sa.sa_family == AF_INET)
                ܦܢܐ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ܐܢ (a->sa.sa_family == AF_INET6)
                ܦܢܐ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, حجم(a->in6.sin6_addr)) == 0;

        ܐܢ (a->sa.sa_family == AF_VSOCK)
                ܦܢܐ a->vm.svm_cid == b->vm.svm_cid;

        ܦܢܐ false;
}

صحيح fd_set_sndbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, value;
        socklen_t l = حجم(value);

        ܐܢ (n > INT_MAX)
                ܦܢܐ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ܐܢ (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ܦܢܐ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(value);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &value, &l);
        ܐܢ (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ܦܢܐ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ 1;
}

صحيح fd_set_rcvbuf(صحيح fd, size_t n, bool increase) {
        صحيح r, value;
        socklen_t l = حجم(value);

        ܐܢ (n > INT_MAX)
                ܦܢܐ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ܐܢ (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ܦܢܐ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = حجم(value);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &l);
        ܐܢ (r >= 0 && l == حجم(value) && increase ? (size_t) value >= n*2 : (size_t) value == n*2)
                ܦܢܐ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ 1;
}

ساكن ثابت حرف* ثابت ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, صحيح, 0xff);

bool ifname_valid_char(حرف a) {
        ܐܢ ((unsigned حرف) a >= 127U)
                ܦܢܐ false;

        ܐܢ ((unsigned حرف) a <= 32U)
                ܦܢܐ false;

        ܐܢ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ܦܢܐ false;

        ܦܢܐ true;
}

bool ifname_valid_full(ثابت حرف *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        أكد(!(flags & ~_IFNAME_VALID_ALL));

        ܐܢ (isempty(p))
                ܦܢܐ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ܐܢ (parse_ifindex(p) >= 0)
                ܦܢܐ flags & IFNAME_VALID_NUMERIC;

        ܐܢ (flags & IFNAME_VALID_ALTERNATIVE) {
                ܐܢ (strlen(p) >= ALTIFNAMSIZ)
                        ܦܢܐ false;
        } ܐܚܪܢܐ {
                ܐܢ (strlen(p) >= IFNAMSIZ)
                        ܦܢܐ false;
        }

        ܐܢ (dot_or_dot_dot(p))
                ܦܢܐ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ܐܢ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ܦܢܐ false;

        لكل (ثابت حرف *t = p; *t; t++) {
                ܐܢ (!ifname_valid_char(*t))
                        ܦܢܐ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ܐܢ (numeric)
                ܦܢܐ false;

        ܦܢܐ true;
}

bool address_label_valid(ثابت حرف *p) {

        POINTER_MAY_BE_NULL(p);

        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܐܢ (strlen(p) >= IFNAMSIZ)
                ܦܢܐ false;

        ܟܡܐ (*p) {
                ܐܢ ((uint8_t) *p >= 127U)
                        ܦܢܐ false;

                ܐܢ ((uint8_t) *p <= 31U)
                        ܦܢܐ false;
                p++;
        }

        ܦܢܐ true;
}

صحيح getpeercred(صحيح fd, بنية ucred *ucred) {
        socklen_t n = حجم(بنية ucred);
        بنية ucred u;

        أكد(fd >= 0);
        أكد(ucred);

        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ܦܢܐ -errno;

        ܐܢ (n != حجم(بنية ucred))
                ܦܢܐ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ܐܢ (!pid_is_valid(u.pid))
                ܦܢܐ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ܦܢܐ 0;
}

صحيح getpeersec(صحيح fd, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        socklen_t n = 64;

        أكد(fd >= 0);
        أكد(ret);

        لكل (;;) {
                s = new0(حرف, n+1);
                ܐܢ (!s)
                        ܦܢܐ -ENOMEM;

                ܐܢ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        اكسر;
                }

                ܐܢ (errno != ERANGE)
                        ܦܢܐ -errno;

                s = mfree(s);
        }

        ܐܢ (isempty(s))
                ܦܢܐ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ܦܢܐ 0;
}

صحيح getpeergroups(صحيح fd, gid_t **ret) {
        socklen_t n = حجم(gid_t) * 64U;
        _cleanup_free_ gid_t *d = عدم;

        أكد(fd >= 0);
        أكد(ret);

        صحيح ngroups_max = sysconf_ngroups_max();
        ܐܢ (ngroups_max > 0)
                n = MAX(n, حجم(gid_t) * (socklen_t) ngroups_max);

        لكل (;;) {
                d = malloc(n);
                ܐܢ (!d)
                        ܦܢܐ -ENOMEM;

                ܐܢ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        اكسر;

                ܐܢ (errno != ERANGE)
                        ܦܢܐ -errno;

                d = mfree(d);
        }

        assert_se(n % حجم(gid_t) == 0);
        n /= حجم(gid_t);

        ܐܢ (n > INT_MAX)
                ܦܢܐ -E2BIG;

        *ret = TAKE_PTR(d);

        ܦܢܐ (صحيح) n;
}

صحيح getpeerpidfd(صحيح fd) {
        socklen_t n = حجم(صحيح);
        صحيح pidfd = -EBADF;

        أكد(fd >= 0);

        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ܦܢܐ -errno;

        ܐܢ (n != حجم(صحيح))
                ܦܢܐ -EIO;

        ܦܢܐ pidfd;
}

صحيح getpeerpidref(صحيح fd, PidRef *ret) {
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        صحيح pidfd = getpeerpidfd(fd);
        ܐܢ (pidfd < 0) {
                ܐܢ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ܦܢܐ pidfd;

                بنية ucred ucred;
                r = getpeercred(fd, &ucred);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܦܢܐ pidref_set_pid(ret, ucred.pid);
        }

        ܦܢܐ pidref_set_pidfd_consume(ret, pidfd);
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
        ܐܢ (fd < 0 && !iov)
                ܦܢܐ -EINVAL;

        ܐܢ (fd >= 0) {
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
        ܐܢ (k < 0)
                ܦܢܐ (ssize_t) -errno;

        ܦܢܐ k;
}

صحيح send_one_fd_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags) {

        أكد(fd >= 0);

        ܦܢܐ (صحيح) send_one_fd_iov_sa(transport_fd, fd, عدم, 0, sa, len, flags);
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
        ܐܢ (k < 0)
                ܦܢܐ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(حجم(صحيح)));
        ܐܢ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ܐܢ (k == 0)
                        ܦܢܐ -EIO;
        }

        ܐܢ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, صحيح);
        ܐܚܪܢܐ
                *ret_fd = -EBADF;

        ܦܢܐ k;
}

صحيح receive_one_fd(صحيح transport_fd, صحيح flags) {
        صحيح fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, عدم, 0, flags, &fd);
        ܐܢ (k == 0)
                ܦܢܐ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        أكد(k < 0);
        ܦܢܐ (صحيح) k;
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
        ܐܢ (l < 0) {
                ܐܢ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        اذهب fallback;

                ܦܢܐ -errno;
        }
        ܐܢ (l == 0)
                اذهب fallback;

        ܦܢܐ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ܐܢ (ioctl(fd, FIONREAD, &k) < 0)
                ܦܢܐ -errno;

        ܦܢܐ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

صحيح flush_accept(صحيح fd) {

        صحيح r, b;
        socklen_t l = حجم(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ܦܢܐ -errno;

        أكد(l == حجم(b));
        ܐܢ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ܦܢܐ -ENOTTY;

        لكل (unsigned iteration = 0;; iteration++) {
                صحيح cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ܐܢ (r == -EINTR)
                        استمر;
                ܐܢ (r <= 0)
                        ܦܢܐ r;

                ܐܢ (iteration >= MAX_FLUSH_ITERATIONS)
                        ܦܢܐ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, عدم, عدم, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ܐܢ (cfd < 0) {
                        ܐܢ (errno == EAGAIN)
                                ܦܢܐ 0;

                        ܐܢ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                استمر;

                        ܦܢܐ -errno;
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

        لكل (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ܐܢ (r == -EINTR)
                        استمر;
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        ܦܢܐ count;

                ܐܢ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ܐܢ (mq_getattr(fd, &attr) < 0)
                                ܦܢܐ -errno;

                        buf = malloc(attr.mq_msgsize);
                        ܐܢ (!buf)
                                ܦܢܐ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ عدم);
                ܐܢ (l < 0) {
                        ܐܢ (errno == EINTR)
                                استمر;

                        ܐܢ (errno == EAGAIN)
                                ܦܢܐ count;

                        ܦܢܐ -errno;
                }

                count += l;
        }
}

بنية cmsghdr* cmsg_find(بنية msghdr *mh, صحيح level, صحيح type, socklen_t length) {
        بنية cmsghdr *cmsg;

        أكد(mh);

        CMSG_FOREACH(cmsg, mh)
                ܐܢ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ܦܢܐ cmsg;

        ܦܢܐ عدم;
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
        ܐܢ (!cmsg)
                ܦܢܐ عدم;

        ܦܢܐ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ثابت بنية sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        أكد(sa->sll_family == AF_PACKET);

        size_t mac_len = حجم(sa->sll_addr);

        ܐܢ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ܐܢ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ܦܢܐ offsetof(بنية sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ثابت بنية sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        أكد(sa->sun_family == AF_UNIX);

        ܦܢܐ offsetof(بنية sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, حجم(sa->sun_path)-1) :
                        strnlen(sa->sun_path, حجم(sa->sun_path))+1);
}

size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa) {
        أكد(sa);

        بدل (sa->sa.sa_family) {
        حالة AF_INET:
                ܦܢܐ حجم(بنية sockaddr_in);
        حالة AF_INET6:
                ܦܢܐ حجم(بنية sockaddr_in6);
        حالة AF_UNIX:
                ܦܢܐ sockaddr_un_len(&sa->un);
        حالة AF_PACKET:
                ܦܢܐ sockaddr_ll_len(&sa->ll);
        حالة AF_NETLINK:
                ܦܢܐ حجم(بنية sockaddr_nl);
        حالة AF_VSOCK:
                ܦܢܐ حجم(بنية sockaddr_vm);
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
        ܐܢ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ܐܢ (fd < 0)
                ܦܢܐ -errno;

        ܦܢܐ fd;
}

صحيح sockaddr_un_unlink(ثابت بنية sockaddr_un *sa) {
        ثابت حرف *p, * nul;

        أكد(sa);

        ܐܢ (sa->sun_family != AF_UNIX)
                ܦܢܐ -EPROTOTYPE;

        ܐܢ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ܦܢܐ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, حجم(sa->sun_path));
        ܐܢ (nul)
                p = sa->sun_path;
        ܐܚܪܢܐ
                p = memdupa_suffix0(sa->sun_path, حجم(sa->sun_path));

        ܐܢ (unlink(p) < 0)
                ܦܢܐ -errno;

        ܦܢܐ 1;
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
        ܐܢ (l < 2)
                ܦܢܐ -EINVAL;
        ܐܢ (!IN_SET(path[0], '/', '@'))
                ܦܢܐ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ܐܢ (l+1 > حجم(ret->sun_path))
                ܦܢܐ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (بنية sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ܐܢ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ܦܢܐ (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ܐܚܪܢܐ {
                أكد(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ܦܢܐ (صحيح) (offsetof(بنية sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

صحيح getsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح *ret) {
        صحيح v;
        socklen_t sl = حجم(v);

        أكد(fd >= 0);
        أكد(ret);

        ܐܢ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ܦܢܐ negative_errno();
        ܐܢ (sl != حجم(v))
                ܦܢܐ -EIO;

        *ret = v;
        ܦܢܐ 0;
}

صحيح socket_bind_to_ifname(صحيح fd, ثابت حرف *ifname) {
        أكد(fd >= 0);

        /* Call with NULL to drop binding */

        ܦܢܐ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

صحيح socket_bind_to_ifindex(صحيح fd, صحيح ifindex) {
        أكد(fd >= 0);

        ܐܢ (ifindex <= 0)
                /* Drop binding */
                ܦܢܐ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, عدم, 0));

        ܦܢܐ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

صحيح socket_autobind(صحيح fd, حرف **ret_name) {
        _cleanup_free_ حرف *name = عدم;
        uint64_t random;
        صحيح r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        أكد(fd >= 0);

        random = random_u64();

        ܐܢ (asprintf(&name, "@%" PRIu64, random) < 0)
                ܦܢܐ -ENOMEM;

        اتحاد sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < حجم(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (bind(fd, &sa.sa, r) < 0)
                ܦܢܐ -errno;

        ܐܢ (ret_name)
                *ret_name = TAKE_PTR(name);
        ܦܢܐ 0;
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
        ܐܢ (n < 0)
                ܦܢܐ -errno;

        ܐܢ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ܦܢܐ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ܦܢܐ n;
}

صحيح socket_get_family(صحيح fd) {
        صحيح af;
        socklen_t sl = حجم(af);

        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ܦܢܐ -errno;

        ܐܢ (sl != حجم(af))
                ܦܢܐ -EINVAL;

        ܦܢܐ af;
}

صحيح socket_set_recvpktinfo(صحيح fd, صحيح af, bool b) {

        ܐܢ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ܐܢ (af < 0)
                        ܦܢܐ af;
        }

        بدل (af) {

        حالة AF_INET:
                ܦܢܐ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        حالة AF_INET6:
                ܦܢܐ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        حالة AF_NETLINK:
                ܦܢܐ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        حالة AF_PACKET:
                ܦܢܐ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
        }
}

صحيح socket_set_unicast_if(صحيح fd, صحيح af, صحيح ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ܐܢ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ܐܢ (af < 0)
                        ܦܢܐ af;
        }

        بدل (af) {

        حالة AF_INET:
                ܦܢܐ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        حالة AF_INET6:
                ܦܢܐ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, حجم(ifindex_be)));

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
        }
}

صحيح socket_set_option(صحيح fd, صحيح af, صحيح opt_ipv4, صحيح opt_ipv6, صحيح val) {
        ܐܢ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ܐܢ (af < 0)
                        ܦܢܐ af;
        }

        بدل (af) {

        حالة AF_INET:
                ܦܢܐ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        حالة AF_INET6:
                ܦܢܐ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
        }
}

صحيح socket_get_mtu(صحيح fd, صحيح af, size_t *ret) {
        صحيح mtu, r;

        أكد(ret);

        ܐܢ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ܐܢ (af < 0)
                        ܦܢܐ af;
        }

        بدل (af) {

        حالة AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                اكسر;

        حالة AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                اكسر;

        افتراضي:
                ܦܢܐ -EAFNOSUPPORT;
        }

        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (mtu <= 0)
                ܦܢܐ -EINVAL;

        *ret = (size_t) mtu;
        ܦܢܐ 0;
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
        ܦܢܐ RET_NERRNO(connect(fd, &sa.sa, offsetof(بنية sockaddr_un, sun_path) + l + 1));
}

ساكن صحيح connect_unix_inode(صحيح fd, صحيح inode_fd) {
        أكد(fd >= 0);
        أكد(inode_fd >= 0);

        ܦܢܐ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

صحيح connect_unix_path(صحيح fd, صحيح dir_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح inode_fd = -EBADF;

        أكد(fd >= 0);
        أكد(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ܐܢ (!path) {
                ܐܢ (dir_fd < 0)
                        ܦܢܐ -EISDIR;

                ܦܢܐ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ܐܢ (isempty(path))
                ܦܢܐ -EINVAL;

        /* Shortcut for the simple case */
        ܐܢ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(بنية sockaddr_un, sun_path))
                ܦܢܐ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ܐܢ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ حرف *j = strjoin("/", path);
                ܐܢ (!j)
                        ܦܢܐ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } ܐܚܪܢܐ
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ܐܢ (inode_fd < 0)
                ܦܢܐ -errno;

        ܦܢܐ connect_unix_inode(fd, inode_fd);
}

صحيح socket_address_parse_unix(SocketAddress *ret_address, ثابت حرف *s) {
        بنية sockaddr_un un;
        صحيح r;

        أكد(ret_address);
        أكد(s);

        ܐܢ (!IN_SET(*s, '/', '@'))
                ܦܢܐ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ܦܢܐ 0;
}

صحيح socket_address_equal_unix(ثابت حرف *a, ثابت حرف *b) {
        SocketAddress socket_a, socket_b;
        صحيح r;

        أكد(a);
        أكد(b);

        r = socket_address_parse_unix(&socket_a, a);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = socket_address_parse_unix(&socket_b, b);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

صحيح vsock_parse_port(ثابت حرف *s, unsigned *ret) {
        صحيح r;

        أكد(ret);

        ܐܢ (!s)
                ܦܢܐ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ܐܢ (u == VMADDR_PORT_ANY)
                ܦܢܐ -EINVAL;

        *ret = u;
        ܦܢܐ 0;
}

صحيح vsock_parse_cid(ثابت حرف *s, unsigned *ret) {
        أكد(ret);

        ܐܢ (!s)
                ܦܢܐ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ܐܢ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ܐܚܪܢܐ ܐܢ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ܐܚܪܢܐ ܐܢ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ܐܚܪܢܐ ܐܢ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ܐܚܪܢܐ
                ܦܢܐ safe_atou(s, ret);

        ܦܢܐ 0;
}

صحيح socket_address_parse_vsock(SocketAddress *ret_address, ثابت حرف *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ حرف *n = عدم;
        ثابت حرف *e, *cid_start;
        unsigned port, cid;
        صحيح type, r;

        أكد(ret_address);
        أكد(s);

        ܐܢ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        ܐܚܪܢܐ ܐܢ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ܐܚܪܢܐ ܐܢ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ܐܚܪܢܐ ܐܢ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ܐܚܪܢܐ
                ܦܢܐ -EPROTO;

        e = strchr(cid_start, ':');
        ܐܢ (!e)
                ܦܢܐ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ܐܢ (r < 0)
                ܦܢܐ r;

        n = strndup(cid_start, e - cid_start);
        ܐܢ (!n)
                ܦܢܐ -ENOMEM;

        ܐܢ (isempty(n))
                cid = VMADDR_CID_ANY;
        ܐܚܪܢܐ {
                r = vsock_parse_cid(n, &cid);
                ܐܢ (r < 0)
                        ܦܢܐ r;
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

        ܦܢܐ 0;
}

صحيح netlink_socket_get_multicast_groups(صحيح fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = عدم;
        socklen_t len = 0, old_len;

        أكد(fd >= 0);

        ܐܢ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, عدم, &len) < 0)
                ܦܢܐ -errno;

        ܐܢ (len == 0)
                اذهب finalize;

        groups = new0(uint32_t, len);
        ܐܢ (!groups)
                ܦܢܐ -ENOMEM;

        old_len = len;

        ܐܢ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ܦܢܐ -errno;

        ܐܢ (old_len != len)
                ܦܢܐ -EIO;

finalize:
        ܐܢ (ret_len)
                *ret_len = len;
        ܐܢ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ܦܢܐ 0;
}

صحيح socket_get_cookie(صحيح fd, uint64_t *ret) {
        أكد(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = حجم(cookie);
        ܐܢ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ܦܢܐ -errno;

        أكد(cookie_len == حجم(cookie));
        ܐܢ (ret)
                *ret = cookie;

        ܦܢܐ 0;
}

فراغ cmsg_close_all(بنية msghdr *mh) {
        أكد(mh);

        بنية cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ܐܢ (cmsg->cmsg_level != SOL_SOCKET)
                        استمر;

                ܐܢ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, صحيح),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / حجم(صحيح));
                ܐܚܪܢܐ ܐܢ (cmsg->cmsg_type == SCM_PIDFD) {
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
                ܦܢܐ TC_PRIO_CONTROL;

        حالة IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        حالة IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ܦܢܐ TC_PRIO_INTERACTIVE;

        حالة IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        حالة IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ܦܢܐ TC_PRIO_INTERACTIVE_BULK;

        حالة IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ܦܢܐ TC_PRIO_BULK;

        حالة IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        افتراضي:
                ܦܢܐ TC_PRIO_BESTEFFORT;
        }
}

صحيح socket_xattr_supported(فراغ) {
        صحيح r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ساكن صحيح cached = -1;
        ܐܢ (cached >= 0)
                ܦܢܐ cached;

        _cleanup_close_ صحيح fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ܐܢ (fd < 0)
                ܦܢܐ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ عدم, AT_EMPTY_PATH, "user.testxxx", "1");
        ܐܢ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ܦܢܐ (cached = false);
        ܐܢ (r < 0)
                ܦܢܐ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ܦܢܐ (cached = true);
}
