/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_arp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_ether.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_infiniband.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/pkt_sched.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mqueue.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/ip.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-ifname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "in-addr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sparse-endian.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sysctl-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xattr-util.h"

#ਜੇ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ਨਹੀਂਤਾਂ
#  define IDN_FLAGS 0
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ socket_ਪਤਾ_type_ਸਾਰਣੀ[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(socket_ਪਤਾ_type, ਪੂਰਨ);

ਪੂਰਨ socket_ਪਤਾ_verify(ਅਚਲ Socket_ਪਤਾ *a, bool strict) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        ਚੋਣ (socket_ਪਤਾ_family(a)) {

        ਹਾਲਤ AF_INET:
                ਜੇ (a->ਮਾਪ != ਆਕਾਰ(ਬਣਤਰ sockaddr_in))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (a->sockaddr.ਵਿੱਚ.sin_port == 0)
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;

        ਹਾਲਤ AF_INET6:
                ਜੇ (a->ਮਾਪ != ਆਕਾਰ(ਬਣਤਰ sockaddr_in6))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (a->sockaddr.in6.sin6_port == 0)
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;

        ਹਾਲਤ AF_UNIX:
                ਜੇ (a->ਮਾਪ < offsetof(ਬਣਤਰ sockaddr_un, sun_path))
                        ਵਾਪਸ -EINVAL;
                ਜੇ (a->ਮਾਪ > ਆਕਾਰ(ਬਣਤਰ sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ਵਾਪਸ -EINVAL;

                ਜੇ (a->ਮਾਪ > offsetof(ਬਣਤਰ sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ਅਚਲ ਅੱਖਰ *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, ਆਕਾਰ(a->sockaddr.un.sun_path));
                        ਜੇ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ਜੇ (a->ਮਾਪ != offsetof(ਬਣਤਰ sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ਵਾਪਸ -EINVAL;
                        } ਨਹੀਂਤਾਂ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->ਮਾਪ, ਆਕਾਰ(a->sockaddr.un.sun_path), ਆਕਾਰ(a->sockaddr.un.sun_path)+1))
                                        ਵਾਪਸ -EINVAL;
                        }
                }

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;

        ਹਾਲਤ AF_NETLINK:

                ਜੇ (a->ਮਾਪ != ਆਕਾਰ(ਬਣਤਰ sockaddr_nl))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;

        ਹਾਲਤ AF_VSOCK:
                ਜੇ (a->ਮਾਪ != ਆਕਾਰ(ਬਣਤਰ sockaddr_vm))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ਵਾਪਸ -EINVAL;

                ਵਾਪਸ 0;

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ socket_ਪਤਾ_print(ਅਚਲ Socket_ਪਤਾ *a, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = socket_ਪਤਾ_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (socket_ਪਤਾ_family(a) == AF_NETLINK) {
                _cleanup_free_ ਅੱਖਰ *sfamily = ਨਿਰਾ;

                r = netlink_family_ਮੰਜ਼ਿਲ_string_alloc(a->protocol, &sfamily);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOMEM;

                ਵਾਪਸ 0;
        }

        ਵਾਪਸ sockaddr_pretty(&a->sockaddr.sa, a->ਮਾਪ, false, true, ret);
}

bool socket_ਪਤਾ_can_accept(ਅਚਲ Socket_ਪਤਾ *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਵਾਪਸ
                ਵਿੱਚ_ਸੈੱਟ(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_ਪਤਾ_equal(ਅਚਲ Socket_ਪਤਾ *a, ਅਚਲ Socket_ਪਤਾ *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        /* Invalid addresses are unequal to all */
        ਜੇ (socket_ਪਤਾ_verify(a, false) < 0 ||
            socket_ਪਤਾ_verify(b, false) < 0)
                ਵਾਪਸ false;

        ਜੇ (a->type != b->type)
                ਵਾਪਸ false;

        ਜੇ (socket_ਪਤਾ_family(a) != socket_ਪਤਾ_family(b))
                ਵਾਪਸ false;

        ਚੋਣ (socket_ਪਤਾ_family(a)) {

        ਹਾਲਤ AF_INET:
                ਜੇ (a->sockaddr.ਵਿੱਚ.sin_addr.s_addr != b->sockaddr.ਵਿੱਚ.sin_addr.s_addr)
                        ਵਾਪਸ false;

                ਜੇ (a->sockaddr.ਵਿੱਚ.sin_port != b->sockaddr.ਵਿੱਚ.sin_port)
                        ਵਾਪਸ false;

                ਰੋਕੋ;

        ਹਾਲਤ AF_INET6:
                ਜੇ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, ਆਕਾਰ(a->sockaddr.in6.sin6_addr)) != 0)
                        ਵਾਪਸ false;

                ਜੇ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ਵਾਪਸ false;

                ਰੋਕੋ;

        ਹਾਲਤ AF_UNIX:
                ਜੇ (a->ਮਾਪ <= offsetof(ਬਣਤਰ sockaddr_un, sun_path) ||
                    b->ਮਾਪ <= offsetof(ਬਣਤਰ sockaddr_un, sun_path))
                        ਵਾਪਸ false;

                ਜੇ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ਵਾਪਸ false;

                ਜੇ (a->sockaddr.un.sun_path[0]) {
                        ਜੇ (!ਰਸਤਾ_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ਵਾਪਸ false;
                } ਨਹੀਂਤਾਂ {
                        ਜੇ (a->ਮਾਪ != b->ਮਾਪ)
                                ਵਾਪਸ false;

                        ਜੇ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->ਮਾਪ) != 0)
                                ਵਾਪਸ false;
                }

                ਰੋਕੋ;

        ਹਾਲਤ AF_NETLINK:
                ਜੇ (a->protocol != b->protocol)
                        ਵਾਪਸ false;

                ਜੇ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ਵਾਪਸ false;

                ਰੋਕੋ;

        ਹਾਲਤ AF_VSOCK:
                ਜੇ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ਵਾਪਸ false;

                ਜੇ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ਵਾਪਸ false;

                ਰੋਕੋ;

        ਮੂਲ:
                /* Cannot compare, so we assume the addresses are different */
                ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

ਅਚਲ ਅੱਖਰ* socket_ਪਤਾ_get_ਰਸਤਾ(ਅਚਲ Socket_ਪਤਾ *a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਜੇ (socket_ਪਤਾ_family(a) != AF_UNIX)
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (a->sockaddr.un.sun_path[0] == 0)
                ਵਾਪਸ ਨਿਰਾ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(ਆਕਾਰ(ਸੰਘ sockaddr_union) >= ਆਕਾਰ(ਬਣਤਰ sockaddr_un)+1);
        ਵਾਪਸ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cached = -1;

        ਜੇ (cached < 0) {

                ਜੇ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ਜੇ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ਵਾਪਸ false;
                        }

                        cached = false;
                } ਨਹੀਂਤਾਂ
                        cached = true;
        }

        ਵਾਪਸ cached;
}

bool socket_ipv6_is_enabled(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ਜੇ (!socket_ipv6_is_supported())
                ਵਾਪਸ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ਜੇ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ਵਾਪਸ true;
        }

        r = parse_boolean(v);
        ਜੇ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ਵਾਪਸ true;
        }

        ਵਾਪਸ !r;
}

bool socket_ਪਤਾ_matches_fd(ਅਚਲ Socket_ਪਤਾ *a, ਪੂਰਨ fd) {
        Socket_ਪਤਾ b;
        socklen_t solen;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        b.ਮਾਪ = ਆਕਾਰ(b.sockaddr);
        ਜੇ (getsockname(fd, &b.sockaddr.sa, &b.ਮਾਪ) < 0)
                ਵਾਪਸ false;

        ਜੇ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ਵਾਪਸ false;

        solen = ਆਕਾਰ(b.type);
        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ਵਾਪਸ false;

        ਜੇ (b.type != a->type)
                ਵਾਪਸ false;

        ਜੇ (a->protocol != 0)  {
                solen = ਆਕਾਰ(b.protocol);
                ਜੇ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ਵਾਪਸ false;

                ਜੇ (b.protocol != a->protocol)
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ socket_ਪਤਾ_equal(a, &b);
}

ਪੂਰਨ sockaddr_ਪੋਰਟ(ਅਚਲ ਬਣਤਰ sockaddr *_sa, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੋਰਟ) {
        ਅਚਲ ਸੰਘ sockaddr_union *sa = (ਅਚਲ ਸੰਘ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ਪ੍ਰਤਿਜ੍ਞਾ(sa);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਪੋਰਟ);

        ਚੋਣ (sa->sa.sa_family) {

        ਹਾਲਤ AF_INET:
                *ret_ਪੋਰਟ = be16toh(sa->ਵਿੱਚ.sin_port);
                ਵਾਪਸ 0;

        ਹਾਲਤ AF_INET6:
                *ret_ਪੋਰਟ = be16toh(sa->in6.sin6_port);
                ਵਾਪਸ 0;

        ਹਾਲਤ AF_VSOCK:
                *ret_ਪੋਰਟ = sa->vm.svm_port;
                ਵਾਪਸ 0;

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *sockaddr_ਵਿੱਚ_addr(ਅਚਲ ਬਣਤਰ sockaddr *_sa) {
        ਅਚਲ ਸੰਘ sockaddr_union *sa = (ਅਚਲ ਸੰਘ sockaddr_union*) _sa;

        ਜੇ (!sa)
                ਵਾਪਸ ਨਿਰਾ;

        ਚੋਣ (sa->sa.sa_family) {

        ਹਾਲਤ AF_INET:
                ਵਾਪਸ (ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union*) &sa->ਵਿੱਚ.sin_addr;

        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ (ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union*) &sa->in6.sin6_addr;

        ਮੂਲ:
                ਵਾਪਸ ਨਿਰਾ;
        }
}

ਪੂਰਨ sockaddr_ਸੈੱਟ_ਵਿੱਚ_addr(
                ਸੰਘ sockaddr_union *u,
                ਪੂਰਨ family,
                ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a,
                uint16_t port) {

        ਪ੍ਰਤਿਜ੍ਞਾ(u);
        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        ਚੋਣ (family) {

        ਹਾਲਤ AF_INET:
                u->ਵਿੱਚ = (ਬਣਤਰ sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->ਵਿੱਚ,
                        .sin_port = htobe16(port),
                };

                ਵਾਪਸ 0;

        ਹਾਲਤ AF_INET6:
                u->in6 = (ਬਣਤਰ sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ਵਾਪਸ 0;

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;

        }
}

ਪੂਰਨ sockaddr_pretty(
                ਅਚਲ ਬਣਤਰ sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_ਪੋਰਟ,
                ਅੱਖਰ **ret) {

        ਸੰਘ sockaddr_union *sa = (ਸੰਘ sockaddr_union*) _sa;
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(sa);
        ਪ੍ਰਤਿਜ੍ਞਾ(salen >= ਆਕਾਰ(sa->sa.sa_family));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਚੋਣ (sa->sa.sa_family) {

        ਹਾਲਤ AF_INET: {
                uint32_t a;

                a = be32toh(sa->ਵਿੱਚ.sin_addr.s_addr);

                ਜੇ (include_ਪੋਰਟ)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->ਵਿੱਚ.sin_port));
                ਨਹੀਂਤਾਂ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOMEM;
                ਰੋਕੋ;
        }

        ਹਾਲਤ AF_INET6: {
                ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ipv4_ਅਗੇਤਰ[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ਜੇ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_ਅਗੇਤਰ, ਆਕਾਰ(ipv4_ਅਗੇਤਰ)) == 0) {
                        ਅਚਲ uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ਜੇ (include_ਪੋਰਟ)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ਨਹੀਂਤਾਂ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ਜੇ (r < 0)
                                ਵਾਪਸ -ENOMEM;
                } ਨਹੀਂਤਾਂ {
                        ਅਚਲ ਅੱਖਰ *a = IN6_ADDR_ਮੰਜ਼ਿਲ_STRING(&sa->in6.sin6_addr);

                        ਜੇ (include_ਪੋਰਟ) {
                                ਜੇ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_ਭਰਿਆ(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ਵਾਪਸ -ENOMEM;
                        } ਨਹੀਂਤਾਂ {
                                ਜੇ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_ਭਰਿਆ(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ਨਹੀਂਤਾਂ
                                        p = strdup(a);
                                ਜੇ (!p)
                                        ਵਾਪਸ -ENOMEM;
                        }
                }

                ਰੋਕੋ;
        }

        ਹਾਲਤ AF_UNIX:
                ਜੇ (salen <= offsetof(ਬਣਤਰ sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ਬਣਤਰ sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ਨਹੀਂਤਾਂ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        ਅੱਖਰ *ਰਸਤਾ = (ਅੱਖਰ*) sa->un_ਬਫਰ + offsetof(ਬਣਤਰ sockaddr_un, sun_path);
                        size_t ਰਸਤਾ_len = salen - offsetof(ਬਣਤਰ sockaddr_un, sun_path);

                        ਜੇ (ਰਸਤਾ[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ ਅੱਖਰ *e = ਨਿਰਾ;

                                e = cescape_ਲੰਮਾਈ(ਰਸਤਾ + 1, ਰਸਤਾ_len - 1);
                                ਜੇ (!e)
                                        ਵਾਪਸ -ENOMEM;

                                p = strjoin("@", e);
                        } ਨਹੀਂਤਾਂ {
                                ਜੇ (ਰਸਤਾ[ਰਸਤਾ_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        ਰਸਤਾ_len--;

                                p = cescape_ਲੰਮਾਈ(ਰਸਤਾ, ਰਸਤਾ_len);
                        }
                }
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                ਰੋਕੋ;

        ਹਾਲਤ AF_VSOCK:
                ਜੇ (include_ਪੋਰਟ) {
                        ਜੇ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ਨਹੀਂਤਾਂ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ਨਹੀਂਤਾਂ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOMEM;
                ਰੋਕੋ;

        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }

        *ret = p;
        ਵਾਪਸ 0;
}

ਪੂਰਨ getpeername_pretty(ਪੂਰਨ fd, bool include_ਪੋਰਟ, ਅੱਖਰ **ret) {
        ਸੰਘ sockaddr_union sa;
        socklen_t salen = ਆਕਾਰ(sa);
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (getpeername(fd, &sa.sa, &salen) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (sa.sa.sa_family == AF_UNIX) {
                ਬਣਤਰ ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ਵਾਪਸ -ENOMEM;

                ਵਾਪਸ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ਵਾਪਸ sockaddr_pretty(&sa.sa, salen, true, include_ਪੋਰਟ, ret);
}

ਪੂਰਨ getsockname_pretty(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        ਸੰਘ sockaddr_union sa;
        socklen_t salen = ਆਕਾਰ(sa);

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (getsockname(fd, &sa.sa, &salen) < 0)
                ਵਾਪਸ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ਵਾਪਸ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ਪੂਰਨ socknameinfo_pretty(ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t salen, ਅੱਖਰ **ret) {
        ਅੱਖਰ host[NI_MAXHOST];
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(sa);
        ਪ੍ਰਤਿਜ੍ਞਾ(salen >= ਆਕਾਰ(sa_family_t));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = getnameinfo(sa, salen, host, ਆਕਾਰ(host), /* service= */ ਨਿਰਾ, /* service_len= */ 0, IDN_FLAGS);
        ਜੇ (r != 0) {
                ਜੇ (r == EAI_MEMORY)
                        ਵਾਪਸ log_oom_debug();
                ਜੇ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ਨਹੀਂਤਾਂ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ਵਾਪਸ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, host);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ netlink_family_ਸਾਰਣੀ[] = {
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(netlink_family, ਪੂਰਨ, ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);

bool sockaddr_equal(ਅਚਲ ਸੰਘ sockaddr_union *a, ਅਚਲ ਸੰਘ sockaddr_union *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਜੇ (a->sa.sa_family != b->sa.sa_family)
                ਵਾਪਸ false;

        ਜੇ (a->sa.sa_family == AF_INET)
                ਵਾਪਸ a->ਵਿੱਚ.sin_addr.s_addr == b->ਵਿੱਚ.sin_addr.s_addr;

        ਜੇ (a->sa.sa_family == AF_INET6)
                ਵਾਪਸ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, ਆਕਾਰ(a->in6.sin6_addr)) == 0;

        ਜੇ (a->sa.sa_family == AF_VSOCK)
                ਵਾਪਸ a->vm.svm_cid == b->vm.svm_cid;

        ਵਾਪਸ false;
}

ਪੂਰਨ fd_ਸੈੱਟ_sndbuf(ਪੂਰਨ fd, size_t n, bool increase) {
        ਪੂਰਨ r, ਮੁੱਲ;
        socklen_t l = ਆਕਾਰ(ਮੁੱਲ);

        ਜੇ (n > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ਮੁੱਲ, &l);
        ਜੇ (r >= 0 && l == ਆਕਾਰ(ਮੁੱਲ) && increase ? (size_t) ਮੁੱਲ >= n*2 : (size_t) ਮੁੱਲ == n*2)
                ਵਾਪਸ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ਆਕਾਰ(ਮੁੱਲ);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ਮੁੱਲ, &l);
        ਜੇ (r >= 0 && l == ਆਕਾਰ(ਮੁੱਲ) && increase ? (size_t) ਮੁੱਲ >= n*2 : (size_t) ਮੁੱਲ == n*2)
                ਵਾਪਸ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਪੂਰਨ fd_ਸੈੱਟ_rcvbuf(ਪੂਰਨ fd, size_t n, bool increase) {
        ਪੂਰਨ r, ਮੁੱਲ;
        socklen_t l = ਆਕਾਰ(ਮੁੱਲ);

        ਜੇ (n > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ਮੁੱਲ, &l);
        ਜੇ (r >= 0 && l == ਆਕਾਰ(ਮੁੱਲ) && increase ? (size_t) ਮੁੱਲ >= n*2 : (size_t) ਮੁੱਲ == n*2)
                ਵਾਪਸ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ਆਕਾਰ(ਮੁੱਲ);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ਮੁੱਲ, &l);
        ਜੇ (r >= 0 && l == ਆਕਾਰ(ਮੁੱਲ) && increase ? (size_t) ਮੁੱਲ >= n*2 : (size_t) ਮੁੱਲ == n*2)
                ਵਾਪਸ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ip_tos_ਸਾਰਣੀ[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(ip_tos, ਪੂਰਨ, 0xff);

bool ifname_valid_char(ਅੱਖਰ a) {
        ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) a >= 127U)
                ਵਾਪਸ false;

        ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) a <= 32U)
                ਵਾਪਸ false;

        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool ifname_valid_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & ~_IFNAME_VALID_ALL));

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ਜੇ (parse_ifindex(p) >= 0)
                ਵਾਪਸ flags & IFNAME_VALID_NUMERIC;

        ਜੇ (flags & IFNAME_VALID_ALTERNATIVE) {
                ਜੇ (ਲੜੀ_ਲੰਬਾਈ(p) >= ALTIFNAMSIZ)
                        ਵਾਪਸ false;
        } ਨਹੀਂਤਾਂ {
                ਜੇ (ਲੜੀ_ਲੰਬਾਈ(p) >= IFNAMSIZ)
                        ਵਾਪਸ false;
        }

        ਜੇ (dot_or_dot_dot(p))
                ਵਾਪਸ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ਜੇ (!FLAGS_ਸੈੱਟ(flags, IFNAME_VALID_SPECIAL) && STR_ਵਿੱਚ_ਸੈੱਟ(p, "all", "default"))
                ਵਾਪਸ false;

        ਲਈ (ਅਚਲ ਅੱਖਰ *t = p; *t; t++) {
                ਜੇ (!ifname_valid_char(*t))
                        ਵਾਪਸ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ਜੇ (numeric)
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool ਪਤਾ_label_valid(ਅਚਲ ਅੱਖਰ *p) {

        ਸੂਚਕ_MAY_BE_NULL(p);

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(p) >= IFNAMSIZ)
                ਵਾਪਸ false;

        ਜਦੋਂ (*p) {
                ਜੇ ((uint8_t) *p >= 127U)
                        ਵਾਪਸ false;

                ਜੇ ((uint8_t) *p <= 31U)
                        ਵਾਪਸ false;
                p++;
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ getpeercred(ਪੂਰਨ fd, ਬਣਤਰ ucred *ucred) {
        socklen_t n = ਆਕਾਰ(ਬਣਤਰ ucred);
        ਬਣਤਰ ucred u;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ucred);

        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (n != ਆਕਾਰ(ਬਣਤਰ ucred))
                ਵਾਪਸ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ਜੇ (!pid_is_valid(u.pid))
                ਵਾਪਸ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ getpeersec(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        socklen_t n = 64;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (;;) {
                s = new0(ਅੱਖਰ, n+1);
                ਜੇ (!s)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        ਰੋਕੋ;
                }

                ਜੇ (errno != ERANGE)
                        ਵਾਪਸ -errno;

                s = mfree(s);
        }

        ਜੇ (isempty(s))
                ਵਾਪਸ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ਵਾਪਸ 0;
}

ਪੂਰਨ getpeergroups(ਪੂਰਨ fd, gid_t **ret) {
        socklen_t n = ਆਕਾਰ(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਪੂਰਨ ngroups_max = sysconf_ngroups_max();
        ਜੇ (ngroups_max > 0)
                n = MAX(n, ਆਕਾਰ(gid_t) * (socklen_t) ngroups_max);

        ਲਈ (;;) {
                d = ਸਮ੍ਰਿਤੀ_ਦਿਓ(n);
                ਜੇ (!d)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        ਰੋਕੋ;

                ਜੇ (errno != ERANGE)
                        ਵਾਪਸ -errno;

                d = mfree(d);
        }

        assert_se(n % ਆਕਾਰ(gid_t) == 0);
        n /= ਆਕਾਰ(gid_t);

        ਜੇ (n > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -E2BIG;

        *ret = TAKE_PTR(d);

        ਵਾਪਸ (ਪੂਰਨ) n;
}

ਪੂਰਨ getpeerpidfd(ਪੂਰਨ fd) {
        socklen_t n = ਆਕਾਰ(ਪੂਰਨ);
        ਪੂਰਨ pidfd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (n != ਆਕਾਰ(ਪੂਰਨ))
                ਵਾਪਸ -EIO;

        ਵਾਪਸ pidfd;
}

ਪੂਰਨ getpeerpidref(ਪੂਰਨ fd, PidRef *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਪੂਰਨ pidfd = getpeerpidfd(fd);
        ਜੇ (pidfd < 0) {
                ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ਵਾਪਸ pidfd;

                ਬਣਤਰ ucred ucred;
                r = getpeercred(fd, &ucred);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ pidref_ਸੈੱਟ_pid(ret, ucred.pid);
        }

        ਵਾਪਸ pidref_ਸੈੱਟ_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ਪੂਰਨ transport_fd,
                ਪੂਰਨ fd,
                ਅਚਲ ਬਣਤਰ iovec *iov, size_t iovlen,
                ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t len,
                ਪੂਰਨ flags) {

        CMSG_ਬਫਰ_TYPE(CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ))) control = {};
        ਬਣਤਰ msghdr mh = {
                .msg_name = (ਬਣਤਰ sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ਬਣਤਰ iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ਪ੍ਰਤਿਜ੍ਞਾ(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ਜੇ (fd < 0 && !iov)
                ਵਾਪਸ -EINVAL;

        ਜੇ (fd >= 0) {
                ਬਣਤਰ cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = ਆਕਾਰ(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(ਆਕਾਰ(ਪੂਰਨ));
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(CMSG_DATA(cmsg), &fd, ਆਕਾਰ(ਪੂਰਨ));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ਜੇ (k < 0)
                ਵਾਪਸ (ssize_t) -errno;

        ਵਾਪਸ k;
}

ਪੂਰਨ send_one_fd_sa(
                ਪੂਰਨ transport_fd,
                ਪੂਰਨ fd,
                ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t len,
                ਪੂਰਨ flags) {

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਵਾਪਸ (ਪੂਰਨ) send_one_fd_iov_sa(transport_fd, fd, ਨਿਰਾ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ਪੂਰਨ transport_fd,
                ਬਣਤਰ iovec *iov, size_t iovlen,
                ਪੂਰਨ flags,
                ਪੂਰਨ *ret_fd) {

        CMSG_ਬਫਰ_TYPE(CMSG_SPACE(ਆਕਾਰ(ਪੂਰਨ))) control;
        ਬਣਤਰ msghdr mh = {
                .msg_control = &control,
                .msg_controllen = ਆਕਾਰ(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ਬਣਤਰ cmsghdr *ਮਿਲਿਆ;
        ssize_t k;

        ਪ੍ਰਤਿਜ੍ਞਾ(transport_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_ਸੁਰੱਖਿਅਤ(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ਜੇ (k < 0)
                ਵਾਪਸ k;

        ਮਿਲਿਆ = cmsg_ਲੱਭੋ(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(ਆਕਾਰ(ਪੂਰਨ)));
        ਜੇ (!ਮਿਲਿਆ) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ਜੇ (k == 0)
                        ਵਾਪਸ -EIO;
        }

        ਜੇ (ਮਿਲਿਆ)
                *ret_fd = *CMSG_TYPED_ਅੰਕੜੇ(ਮਿਲਿਆ, ਪੂਰਨ);
        ਨਹੀਂਤਾਂ
                *ret_fd = -EBADF;

        ਵਾਪਸ k;
}

ਪੂਰਨ receive_one_fd(ਪੂਰਨ transport_fd, ਪੂਰਨ flags) {
        ਪੂਰਨ fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ਨਿਰਾ, 0, flags, &fd);
        ਜੇ (k == 0)
                ਵਾਪਸ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ਪ੍ਰਤਿਜ੍ਞਾ(k < 0);
        ਵਾਪਸ (ਪੂਰਨ) k;
}

ssize_t ਅਗਲਾ_datagram_ਮਾਪ_fd(ਪੂਰਨ fd) {
        ssize_t l;
        ਪੂਰਨ k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ਨਿਰਾ, 0, MSG_PEEK|MSG_TRUNC);
        ਜੇ (l < 0) {
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(errno, EOPNOTSUPP, EFAULT))
                        ਜਾਓ fallback;

                ਵਾਪਸ -errno;
        }
        ਜੇ (l == 0)
                ਜਾਓ fallback;

        ਵਾਪਸ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ਜੇ (ioctl(fd, FIONREAD, &k) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FLUSH_ITERATIONS 1024

ਪੂਰਨ flush_accept(ਪੂਰਨ fd) {

        ਪੂਰਨ r, b;
        socklen_t l = ਆਕਾਰ(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ਵਾਪਸ -errno;

        ਪ੍ਰਤਿਜ੍ਞਾ(l == ਆਕਾਰ(b));
        ਜੇ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ਵਾਪਸ -ENOTTY;

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ iteration = 0;; iteration++) {
                ਪੂਰਨ cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ਜੇ (r == -EINTR)
                        ਜਾਰੀ;
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;

                ਜੇ (iteration >= MAX_FLUSH_ITERATIONS)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ਨਿਰਾ, ਨਿਰਾ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ਜੇ (cfd < 0) {
                        ਜੇ (errno == EAGAIN)
                                ਵਾਪਸ 0;

                        ਜੇ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ਜਾਰੀ;

                        ਵਾਪਸ -errno;
                }

                ਸੁਰੱਖਿਅਤ_close(cfd);
        }
}

ssize_t flush_mqueue(ਪੂਰਨ fd) {
        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        ਬਣਤਰ mq_attr attr;
        ssize_t ਗਿਣਤੀ_2 = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        ਲਈ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ਜੇ (r == -EINTR)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ ਗਿਣਤੀ_2;

                ਜੇ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ਜੇ (mq_getattr(fd, &attr) < 0)
                                ਵਾਪਸ -errno;

                        buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(attr.mq_msgsize);
                        ਜੇ (!buf)
                                ਵਾਪਸ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ਨਿਰਾ);
                ਜੇ (l < 0) {
                        ਜੇ (errno == EINTR)
                                ਜਾਰੀ;

                        ਜੇ (errno == EAGAIN)
                                ਵਾਪਸ ਗਿਣਤੀ_2;

                        ਵਾਪਸ -errno;
                }

                ਗਿਣਤੀ_2 += l;
        }
}

ਬਣਤਰ cmsghdr* cmsg_ਲੱਭੋ(ਬਣਤਰ msghdr *mh, ਪੂਰਨ level, ਪੂਰਨ type, socklen_t length) {
        ਬਣਤਰ cmsghdr *cmsg;

        ਪ੍ਰਤਿਜ੍ਞਾ(mh);

        CMSG_FOREACH(cmsg, mh)
                ਜੇ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ਵਾਪਸ cmsg;

        ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ* cmsg_ਲੱਭੋ_and_copy_ਅੰਕੜੇ(ਬਣਤਰ msghdr *mh, ਪੂਰਨ level, ਪੂਰਨ type, ਖਾਲੀ *buf, size_t buf_len) {
        ਬਣਤਰ cmsghdr *cmsg;

        ਪ੍ਰਤਿਜ੍ਞਾ(mh);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_ਲੱਭੋ(mh, level, type, CMSG_LEN(buf_len));
        ਜੇ (!cmsg)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ memcpy_ਸੁਰੱਖਿਅਤ(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ਅਚਲ ਬਣਤਰ sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ਪ੍ਰਤਿਜ੍ਞਾ(sa->sll_family == AF_PACKET);

        size_t mac_len = ਆਕਾਰ(sa->sll_addr);

        ਜੇ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ਜੇ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ਵਾਪਸ offsetof(ਬਣਤਰ sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ਅਚਲ ਬਣਤਰ sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ਪ੍ਰਤਿਜ੍ਞਾ(sa->sun_family == AF_UNIX);

        ਵਾਪਸ offsetof(ਬਣਤਰ sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, ਆਕਾਰ(sa->sun_path)-1) :
                        strnlen(sa->sun_path, ਆਕਾਰ(sa->sun_path))+1);
}

size_t sockaddr_len(ਅਚਲ ਸੰਘ sockaddr_union *sa) {
        ਪ੍ਰਤਿਜ੍ਞਾ(sa);

        ਚੋਣ (sa->sa.sa_family) {
        ਹਾਲਤ AF_INET:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ sockaddr_in);
        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ sockaddr_in6);
        ਹਾਲਤ AF_UNIX:
                ਵਾਪਸ sockaddr_un_len(&sa->un);
        ਹਾਲਤ AF_PACKET:
                ਵਾਪਸ sockaddr_ll_len(&sa->ll);
        ਹਾਲਤ AF_NETLINK:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ sockaddr_nl);
        ਹਾਲਤ AF_VSOCK:
                ਵਾਪਸ ਆਕਾਰ(ਬਣਤਰ sockaddr_vm);
        ਮੂਲ:
                assert_not_reached();
        }
}

ਪੂਰਨ socket_ioctl_fd(ਖਾਲੀ) {
        ਪੂਰਨ fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ਜੇ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ fd;
}

ਪੂਰਨ sockaddr_un_unlink(ਅਚਲ ਬਣਤਰ sockaddr_un *sa) {
        ਅਚਲ ਅੱਖਰ *p, * nul;

        ਪ੍ਰਤਿਜ੍ਞਾ(sa);

        ਜੇ (sa->sun_family != AF_UNIX)
                ਵਾਪਸ -EPROTOTYPE;

        ਜੇ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ਵਾਪਸ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, ਆਕਾਰ(sa->sun_path));
        ਜੇ (nul)
                p = sa->sun_path;
        ਨਹੀਂਤਾਂ
                p = memdupa_suffix0(sa->sun_path, ਆਕਾਰ(sa->sun_path));

        ਜੇ (unlink(p) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 1;
}

ਪੂਰਨ sockaddr_un_ਸੈੱਟ_ਰਸਤਾ(ਬਣਤਰ sockaddr_un *ret, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        size_t l;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
        ਜੇ (l < 2)
                ਵਾਪਸ -EINVAL;
        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(ਰਸਤਾ[0], '/', '@'))
                ਵਾਪਸ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ਜੇ (l+1 > ਆਕਾਰ(ret->sun_path))
                ਵਾਪਸ ਰਸਤਾ[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ਬਣਤਰ sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ਜੇ (ਰਸਤਾ[0] == '@') {
                /* Abstract namespace socket */
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret->sun_path + 1, ਰਸਤਾ + 1, l); /* copy *with* trailing NUL byte */
                ਵਾਪਸ (ਪੂਰਨ) (offsetof(ਬਣਤਰ sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ਨਹੀਂਤਾਂ {
                ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ[0] == '/');

                /* File system socket */
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret->sun_path, ਰਸਤਾ, l + 1); /* copy *with* trailing NUL byte */
                ਵਾਪਸ (ਪੂਰਨ) (offsetof(ਬਣਤਰ sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ਪੂਰਨ getsockopt_int(ਪੂਰਨ fd, ਪੂਰਨ level, ਪੂਰਨ optname, ਪੂਰਨ *ret) {
        ਪੂਰਨ v;
        socklen_t sl = ਆਕਾਰ(v);

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ਵਾਪਸ negative_errno();
        ਜੇ (sl != ਆਕਾਰ(v))
                ਵਾਪਸ -EIO;

        *ret = v;
        ਵਾਪਸ 0;
}

ਪੂਰਨ socket_bind_ਮੰਜ਼ਿਲ_ifname(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ifname) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Call with NULL to drop binding */

        ਵਾਪਸ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ਪੂਰਨ socket_bind_ਮੰਜ਼ਿਲ_ifindex(ਪੂਰਨ fd, ਪੂਰਨ ifindex) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (ifindex <= 0)
                /* Drop binding */
                ਵਾਪਸ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ਨਿਰਾ, 0));

        ਵਾਪਸ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ਪੂਰਨ socket_autobind(ਪੂਰਨ fd, ਅੱਖਰ **ret_name) {
        _cleanup_free_ ਅੱਖਰ *name = ਨਿਰਾ;
        uint64_t random;
        ਪੂਰਨ r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        random = random_u64();

        ਜੇ (asprintf(&name, "@%" PRIu64, random) < 0)
                ਵਾਪਸ -ENOMEM;

        ਸੰਘ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < ਆਕਾਰ(sa.un.sun_path));

        r = sockaddr_un_ਸੈੱਟ_ਰਸਤਾ(&sa.un, name);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (bind(fd, &sa.sa, r) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_name)
                *ret_name = TAKE_PTR(name);
        ਵਾਪਸ 0;
}

ssize_t recvmsg_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ sockfd, ਬਣਤਰ msghdr *msg, ਪੂਰਨ flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ਪ੍ਰਤਿਜ੍ਞਾ(sockfd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(msg);

        n = recvmsg(sockfd, msg, flags);
        ਜੇ (n < 0)
                ਵਾਪਸ -errno;

        ਜੇ (FLAGS_ਸੈੱਟ(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_ਸੈੱਟ(flags, MSG_PEEK) && FLAGS_ਸੈੱਟ(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ਵਾਪਸ FLAGS_ਸੈੱਟ(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ਵਾਪਸ n;
}

ਪੂਰਨ socket_get_family(ਪੂਰਨ fd) {
        ਪੂਰਨ af;
        socklen_t sl = ਆਕਾਰ(af);

        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (sl != ਆਕਾਰ(af))
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ af;
}

ਪੂਰਨ socket_ਸੈੱਟ_recvpktinfo(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {

        ਜੇ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ਜੇ (af < 0)
                        ਵਾਪਸ af;
        }

        ਚੋਣ (af) {

        ਹਾਲਤ AF_INET:
                ਵਾਪਸ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        ਹਾਲਤ AF_NETLINK:
                ਵਾਪਸ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        ਹਾਲਤ AF_PACKET:
                ਵਾਪਸ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ socket_ਸੈੱਟ_unicast_if(ਪੂਰਨ fd, ਪੂਰਨ af, ਪੂਰਨ ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ਜੇ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ਜੇ (af < 0)
                        ਵਾਪਸ af;
        }

        ਚੋਣ (af) {

        ਹਾਲਤ AF_INET:
                ਵਾਪਸ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, ਆਕਾਰ(ifindex_be)));

        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, ਆਕਾਰ(ifindex_be)));

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ socket_ਸੈੱਟ_option(ਪੂਰਨ fd, ਪੂਰਨ af, ਪੂਰਨ opt_ipv4, ਪੂਰਨ opt_ipv6, ਪੂਰਨ val) {
        ਜੇ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ਜੇ (af < 0)
                        ਵਾਪਸ af;
        }

        ਚੋਣ (af) {

        ਹਾਲਤ AF_INET:
                ਵਾਪਸ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        ਹਾਲਤ AF_INET6:
                ਵਾਪਸ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }
}

ਪੂਰਨ socket_get_mtu(ਪੂਰਨ fd, ਪੂਰਨ af, size_t *ret) {
        ਪੂਰਨ mtu, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ਜੇ (af < 0)
                        ਵਾਪਸ af;
        }

        ਚੋਣ (af) {

        ਹਾਲਤ AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                ਰੋਕੋ;

        ਹਾਲਤ AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                ਰੋਕੋ;

        ਮੂਲ:
                ਵਾਪਸ -EAFNOSUPPORT;
        }

        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (mtu <= 0)
                ਵਾਪਸ -EINVAL;

        *ret = (size_t) mtu;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ connect_unix_ਰਸਤਾ_simple(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਸੰਘ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        l = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(l > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(l < ਆਕਾਰ(sa.un.sun_path));

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(sa.un.sun_path, ਰਸਤਾ, l + 1);
        ਵਾਪਸ RET_NERRNO(connect(fd, &sa.sa, offsetof(ਬਣਤਰ sockaddr_un, sun_path) + l + 1));
}

ਸਥਿਰ ਪੂਰਨ connect_unix_inode(ਪੂਰਨ fd, ਪੂਰਨ inode_fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(inode_fd >= 0);

        ਵਾਪਸ connect_unix_ਰਸਤਾ_simple(fd, FORMAT_PROC_FD_ਰਸਤਾ(inode_fd));
}

ਪੂਰਨ connect_unix_ਰਸਤਾ(ਪੂਰਨ fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ inode_fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ਜੇ (!ਰਸਤਾ) {
                ਜੇ (dir_fd < 0)
                        ਵਾਪਸ -EISDIR;

                ਵਾਪਸ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        /* Shortcut for the simple case */
        ਜੇ (dir_fd == AT_FDCWD && ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) < sizeof_ਖੇਤਰ(ਬਣਤਰ sockaddr_un, sun_path))
                ਵਾਪਸ connect_unix_ਰਸਤਾ_simple(fd, ਰਸਤਾ);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ਜੇ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ ਅੱਖਰ *j = strjoin("/", ਰਸਤਾ);
                ਜੇ (!j)
                        ਵਾਪਸ -ENOMEM;

                inode_fd = open(j, O_ਰਸਤਾ|O_CLOEXEC);
        } ਨਹੀਂਤਾਂ
                inode_fd = openat(dir_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC);
        ਜੇ (inode_fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ connect_unix_inode(fd, inode_fd);
}

ਪੂਰਨ socket_ਪਤਾ_parse_unix(Socket_ਪਤਾ *ret_ਪਤਾ, ਅਚਲ ਅੱਖਰ *s) {
        ਬਣਤਰ sockaddr_un un;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਪਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(*s, '/', '@'))
                ਵਾਪਸ -EPROTO;

        r = sockaddr_un_ਸੈੱਟ_ਰਸਤਾ(&un, s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret_ਪਤਾ = (Socket_ਪਤਾ) {
                .sockaddr.un = un,
                .ਮਾਪ = r,
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ socket_ਪਤਾ_equal_unix(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        Socket_ਪਤਾ socket_a, socket_b;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = socket_ਪਤਾ_parse_unix(&socket_a, a);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = socket_ਪਤਾ_parse_unix(&socket_b, b);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ਪੂਰਨ vsock_parse_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!s)
                ਵਾਪਸ -EINVAL;

        ਬਿਨਾਨਿਸ਼ਾਨੀ u;
        r = ਸੁਰੱਖਿਅਤ_atou(s, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ਜੇ (u == VMADDR_PORT_ANY)
                ਵਾਪਸ -EINVAL;

        *ret = u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ vsock_parse_cid(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!s)
                ਵਾਪਸ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ਜੇ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ਨਹੀਂਤਾਂ ਜੇ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ਨਹੀਂਤਾਂ ਜੇ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ਨਹੀਂਤਾਂ ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou(s, ret);

        ਵਾਪਸ 0;
}

ਪੂਰਨ socket_ਪਤਾ_parse_vsock(Socket_ਪਤਾ *ret_ਪਤਾ, ਅਚਲ ਅੱਖਰ *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e, *cid_ਸ਼ੁਰੂ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ port, cid;
        ਪੂਰਨ type, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਪਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ ((cid_ਸ਼ੁਰੂ = startswith(s, "vsock:")))
                type = 0;
        ਨਹੀਂਤਾਂ ਜੇ ((cid_ਸ਼ੁਰੂ = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ਨਹੀਂਤਾਂ ਜੇ ((cid_ਸ਼ੁਰੂ = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ਨਹੀਂਤਾਂ ਜੇ ((cid_ਸ਼ੁਰੂ = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -EPROTO;

        e = strchr(cid_ਸ਼ੁਰੂ, ':');
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;

        r = vsock_parse_ਪੋਰਟ(e+1, &port);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        n = strndup(cid_ਸ਼ੁਰੂ, e - cid_ਸ਼ੁਰੂ);
        ਜੇ (!n)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (isempty(n))
                cid = VMADDR_CID_ANY;
        ਨਹੀਂਤਾਂ {
                r = vsock_parse_cid(n, &cid);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret_ਪਤਾ = (Socket_ਪਤਾ) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ sockaddr_vm),
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ netlink_socket_get_multicast_groups(ਪੂਰਨ fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ਨਿਰਾ;
        socklen_t len = 0, old_len;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ਨਿਰਾ, &len) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (len == 0)
                ਜਾਓ finalize;

        groups = new0(uint32_t, len);
        ਜੇ (!groups)
                ਵਾਪਸ -ENOMEM;

        old_len = len;

        ਜੇ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (old_len != len)
                ਵਾਪਸ -EIO;

finalize:
        ਜੇ (ret_len)
                *ret_len = len;
        ਜੇ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ਵਾਪਸ 0;
}

ਪੂਰਨ socket_get_cookie(ਪੂਰਨ fd, uint64_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = ਆਕਾਰ(cookie);
        ਜੇ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ਵਾਪਸ -errno;

        ਪ੍ਰਤਿਜ੍ਞਾ(cookie_len == ਆਕਾਰ(cookie));
        ਜੇ (ret)
                *ret = cookie;

        ਵਾਪਸ 0;
}

ਖਾਲੀ cmsg_close_all(ਬਣਤਰ msghdr *mh) {
        ਪ੍ਰਤਿਜ੍ਞਾ(mh);

        ਬਣਤਰ cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ਜੇ (cmsg->cmsg_level != SOL_SOCKET)
                        ਜਾਰੀ;

                ਜੇ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_ਅੰਕੜੇ(cmsg, ਪੂਰਨ),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / ਆਕਾਰ(ਪੂਰਨ));
                ਨਹੀਂਤਾਂ ਜੇ (cmsg->cmsg_type == SCM_PIDFD) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(cmsg->cmsg_len == CMSG_LEN(ਆਕਾਰ(ਪੂਰਨ)));
                        ਸੁਰੱਖਿਅਤ_close(*CMSG_TYPED_ਅੰਕੜੇ(cmsg, ਪੂਰਨ));
                }
        }
}

ਪੂਰਨ tos_ਮੰਜ਼ਿਲ_ਤਰਜੀਹ(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        ਚੋਣ (IPTOS_PREC(tos)) {
        ਹਾਲਤ IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        ਹਾਲਤ IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ਵਾਪਸ TC_PRIO_CONTROL;

        ਹਾਲਤ IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        ਹਾਲਤ IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ਵਾਪਸ TC_PRIO_INTERACTIVE;

        ਹਾਲਤ IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        ਹਾਲਤ IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ਵਾਪਸ TC_PRIO_INTERACTIVE_BULK;

        ਹਾਲਤ IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ਵਾਪਸ TC_PRIO_BULK;

        ਹਾਲਤ IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        ਮੂਲ:
                ਵਾਪਸ TC_PRIO_BESTEFFORT;
        }
}

ਪੂਰਨ socket_xattr_supported(ਖਾਲੀ) {
        ਪੂਰਨ r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ਸਥਿਰ ਪੂਰਨ cached = -1;
        ਜੇ (cached >= 0)
                ਵਾਪਸ cached;

        _cleanup_close_ ਪੂਰਨ fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, "user.testxxx", "1");
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ਵਾਪਸ (cached = false);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ਵਾਪਸ (cached = true);
}
