/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/if.h>
#ಅನ್ತರ್ಭಾವ <linux/if_arp.h>
#ಅನ್ತರ್ಭಾವ <linux/if_ether.h>
#ಅನ್ತರ್ಭಾವ <linux/if_infiniband.h>
#ಅನ್ತರ್ಭಾವ <linux/pkt_sched.h>
#ಅನ್ತರ್ಭಾವ <mqueue.h>
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಅನ್ತರ್ಭಾವ <netinet/ip.h>
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "format-ifname.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "in-addr-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "sparse-endian.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "sysctl-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"
#ಅನ್ತರ್ಭಾವ "xattr-util.h"

#ಯದಿ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ಅನ್ಯಥಾ
#  define IDN_FLAGS 0
#endif

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, ಪೂರ್ಣಾಙ್ಕ);

ಪೂರ್ಣಾಙ್ಕ socket_address_verify(ಧ್ರುವ SocketAddress *a, bool strict) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        ವಿಕಲ್ಪ (socket_address_family(a)) {

        ಪಕ್ಷ AF_INET:
                ಯದಿ (a->size != ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (a->sockaddr.in.sin_port == 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_INET6:
                ಯದಿ (a->size != ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (a->sockaddr.in6.sin6_port == 0)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_UNIX:
                ಯದಿ (a->size < offsetof(ಸಂರಚನಾ sockaddr_un, sun_path))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಯದಿ (a->size > ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (a->size > offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ಧ್ರುವ ವರ್ಣ *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, ಪರಿಮಾಣ(a->sockaddr.un.sun_path));
                        ಯದಿ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ಯದಿ (a->size != offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        } ಅನ್ಯಥಾ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ಯದಿ (!IN_SET(a->size, ಪರಿಮಾಣ(a->sockaddr.un.sun_path), ಪರಿಮಾಣ(a->sockaddr.un.sun_path)+1))
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        }
                }

                ಯದಿ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_NETLINK:

                ಯದಿ (a->size != ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_nl))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_VSOCK:
                ಯದಿ (a->size != ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_vm))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ 0;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_address_print(ಧ್ರುವ SocketAddress *a, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ ವರ್ಣ *sfamily = ರಿಕ್ತ;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ಧ್ರುವ SocketAddress *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಪ್ರತ್ಯಾಗಮ
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ಧ್ರುವ SocketAddress *a, ಧ್ರುವ SocketAddress *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Invalid addresses are unequal to all */
        ಯದಿ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (a->type != b->type)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (socket_address_family(a) != socket_address_family(b))
                ಪ್ರತ್ಯಾಗಮ false;

        ವಿಕಲ್ಪ (socket_address_family(a)) {

        ಪಕ್ಷ AF_INET:
                ಯದಿ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಭಙ್ಗ;

        ಪಕ್ಷ AF_INET6:
                ಯದಿ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, ಪರಿಮಾಣ(a->sockaddr.in6.sin6_addr)) != 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಭಙ್ಗ;

        ಪಕ್ಷ AF_UNIX:
                ಯದಿ (a->size <= offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) ||
                    b->size <= offsetof(ಸಂರಚನಾ sockaddr_un, sun_path))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (a->sockaddr.un.sun_path[0]) {
                        ಯದಿ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ಪ್ರತ್ಯಾಗಮ false;
                } ಅನ್ಯಥಾ {
                        ಯದಿ (a->size != b->size)
                                ಪ್ರತ್ಯಾಗಮ false;

                        ಯದಿ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ಪ್ರತ್ಯಾಗಮ false;
                }

                ಭಙ್ಗ;

        ಪಕ್ಷ AF_NETLINK:
                ಯದಿ (a->protocol != b->protocol)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಭಙ್ಗ;

        ಪಕ್ಷ AF_VSOCK:
                ಯದಿ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                /* Cannot compare, so we assume the addresses are different */
                ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಧ್ರುವ ವರ್ಣ* socket_address_get_path(ಧ್ರುವ SocketAddress *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಯದಿ (socket_address_family(a) != AF_UNIX)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (a->sockaddr.un.sun_path[0] == 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(ಪರಿಮಾಣ(ಸಙ್ಘ sockaddr_union) >= ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un)+1);
        ಪ್ರತ್ಯಾಗಮ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cached = -1;

        ಯದಿ (cached < 0) {

                ಯದಿ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ಯದಿ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ಪ್ರತ್ಯಾಗಮ false;
                        }

                        cached = false;
                } ಅನ್ಯಥಾ
                        cached = true;
        }

        ಪ್ರತ್ಯಾಗಮ cached;
}

bool socket_ipv6_is_enabled(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ಯದಿ (!socket_ipv6_is_supported())
                ಪ್ರತ್ಯಾಗಮ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ಯದಿ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ಪ್ರತ್ಯಾಗಮ true;
        }

        r = parse_boolean(v);
        ಯದಿ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ !r;
}

bool socket_address_matches_fd(ಧ್ರುವ SocketAddress *a, ಪೂರ್ಣಾಙ್ಕ fd) {
        SocketAddress b;
        socklen_t solen;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        b.size = ಪರಿಮಾಣ(b.sockaddr);
        ಯದಿ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ಪ್ರತ್ಯಾಗಮ false;

        solen = ಪರಿಮಾಣ(b.type);
        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (b.type != a->type)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (a->protocol != 0)  {
                solen = ಪರಿಮಾಣ(b.protocol);
                ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (b.protocol != a->protocol)
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ socket_address_equal(a, &b);
}

ಪೂರ್ಣಾಙ್ಕ sockaddr_port(ಧ್ರುವ ಸಂರಚನಾ sockaddr *_sa, unsigned *ret_port) {
        ಧ್ರುವ ಸಙ್ಘ sockaddr_union *sa = (ಧ್ರುವ ಸಙ್ಘ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(ret_port);

        ವಿಕಲ್ಪ (sa->sa.sa_family) {

        ಪಕ್ಷ AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ಪ್ರತ್ಯಾಗಮ 0;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಧ್ರುವ ಸಙ್ಘ in_addr_union *sockaddr_in_addr(ಧ್ರುವ ಸಂರಚನಾ sockaddr *_sa) {
        ಧ್ರುವ ಸಙ್ಘ sockaddr_union *sa = (ಧ್ರುವ ಸಙ್ಘ sockaddr_union*) _sa;

        ಯದಿ (!sa)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ವಿಕಲ್ಪ (sa->sa.sa_family) {

        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ಸಙ್ಘ in_addr_union*) &sa->in.sin_addr;

        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ಸಙ್ಘ in_addr_union*) &sa->in6.sin6_addr;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
}

ಪೂರ್ಣಾಙ್ಕ sockaddr_set_in_addr(
                ಸಙ್ಘ sockaddr_union *u,
                ಪೂರ್ಣಾಙ್ಕ family,
                ಧ್ರುವ ಸಙ್ಘ in_addr_union *a,
                uint16_t port) {

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(a);

        ವಿಕಲ್ಪ (family) {

        ಪಕ್ಷ AF_INET:
                u->in = (ಸಂರಚನಾ sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                ಪ್ರತ್ಯಾಗಮ 0;

        ಪಕ್ಷ AF_INET6:
                u->in6 = (ಸಂರಚನಾ sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ಪ್ರತ್ಯಾಗಮ 0;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        }
}

ಪೂರ್ಣಾಙ್ಕ sockaddr_pretty(
                ಧ್ರುವ ಸಂರಚನಾ sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                ವರ್ಣ **ret) {

        ಸಙ್ಘ sockaddr_union *sa = (ಸಙ್ಘ sockaddr_union*) _sa;
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(salen >= ಪರಿಮಾಣ(sa->sa.sa_family));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ವಿಕಲ್ಪ (sa->sa.sa_family) {

        ಪಕ್ಷ AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ಯದಿ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                ಅನ್ಯಥಾ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                ಭಙ್ಗ;
        }

        ಪಕ್ಷ AF_INET6: {
                ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ಯದಿ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, ಪರಿಮಾಣ(ipv4_prefix)) == 0) {
                        ಧ್ರುವ uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ಯದಿ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ಅನ್ಯಥಾ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                } ಅನ್ಯಥಾ {
                        ಧ್ರುವ ವರ್ಣ *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ಯದಿ (include_port) {
                                ಯದಿ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        } ಅನ್ಯಥಾ {
                                ಯದಿ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ಅನ್ಯಥಾ
                                        p = strdup(a);
                                ಯದಿ (!p)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        }
                }

                ಭಙ್ಗ;
        }

        ಪಕ್ಷ AF_UNIX:
                ಯದಿ (salen <= offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ಅನ್ಯಥಾ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        ವರ್ಣ *path = (ವರ್ಣ*) sa->un_buffer + offsetof(ಸಂರಚನಾ sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(ಸಂರಚನಾ sockaddr_un, sun_path);

                        ಯದಿ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ ವರ್ಣ *e = ರಿಕ್ತ;

                                e = cescape_length(path + 1, path_len - 1);
                                ಯದಿ (!e)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                p = strjoin("@", e);
                        } ಅನ್ಯಥಾ {
                                ಯದಿ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಭಙ್ಗ;

        ಪಕ್ಷ AF_VSOCK:
                ಯದಿ (include_port) {
                        ಯದಿ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ಅನ್ಯಥಾ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ಅನ್ಯಥಾ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }

        *ret = p;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getpeername_pretty(ಪೂರ್ಣಾಙ್ಕ fd, bool include_port, ವರ್ಣ **ret) {
        ಸಙ್ಘ sockaddr_union sa;
        socklen_t salen = ಪರಿಮಾಣ(sa);
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (getpeername(fd, &sa.sa, &salen) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (sa.sa.sa_family == AF_UNIX) {
                ಸಂರಚನಾ ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ಪ್ರತ್ಯಾಗಮ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

ಪೂರ್ಣಾಙ್ಕ getsockname_pretty(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        ಸಙ್ಘ sockaddr_union sa;
        socklen_t salen = ಪರಿಮಾಣ(sa);

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (getsockname(fd, &sa.sa, &salen) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ಪ್ರತ್ಯಾಗಮ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ಪೂರ್ಣಾಙ್ಕ socknameinfo_pretty(ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t salen, ವರ್ಣ **ret) {
        ವರ್ಣ host[NI_MAXHOST];
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(salen >= ಪರಿಮಾಣ(sa_family_t));
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getnameinfo(sa, salen, host, ಪರಿಮಾಣ(host), /* service= */ ರಿಕ್ತ, /* service_len= */ 0, IDN_FLAGS);
        ಯದಿ (r != 0) {
                ಯದಿ (r == EAI_MEMORY)
                        ಪ್ರತ್ಯಾಗಮ log_oom_debug();
                ಯದಿ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ಅನ್ಯಥಾ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ಪ್ರತ್ಯಾಗಮ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, host);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ಪೂರ್ಣಾಙ್ಕ, INT_MAX);

bool sockaddr_equal(ಧ್ರುವ ಸಙ್ಘ sockaddr_union *a, ಧ್ರುವ ಸಙ್ಘ sockaddr_union *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಯದಿ (a->sa.sa_family != b->sa.sa_family)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (a->sa.sa_family == AF_INET)
                ಪ್ರತ್ಯಾಗಮ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ಯದಿ (a->sa.sa_family == AF_INET6)
                ಪ್ರತ್ಯಾಗಮ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, ಪರಿಮಾಣ(a->in6.sin6_addr)) == 0;

        ಯದಿ (a->sa.sa_family == AF_VSOCK)
                ಪ್ರತ್ಯಾಗಮ a->vm.svm_cid == b->vm.svm_cid;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ fd_set_sndbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n, bool increase) {
        ಪೂರ್ಣಾಙ್ಕ r, ಮೌಲ್ಯ;
        socklen_t l = ಪರಿಮಾಣ(ಮೌಲ್ಯ);

        ಯದಿ (n > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ಮೌಲ್ಯ, &l);
        ಯದಿ (r >= 0 && l == ಪರಿಮಾಣ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ಪರಿಮಾಣ(ಮೌಲ್ಯ);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ಮೌಲ್ಯ, &l);
        ಯದಿ (r >= 0 && l == ಪರಿಮಾಣ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಪ್ರತ್ಯಾಗಮ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ fd_set_rcvbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n, bool increase) {
        ಪೂರ್ಣಾಙ್ಕ r, ಮೌಲ್ಯ;
        socklen_t l = ಪರಿಮಾಣ(ಮೌಲ್ಯ);

        ಯದಿ (n > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ಮೌಲ್ಯ, &l);
        ಯದಿ (r >= 0 && l == ಪರಿಮಾಣ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ಪರಿಮಾಣ(ಮೌಲ್ಯ);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ಮೌಲ್ಯ, &l);
        ಯದಿ (r >= 0 && l == ಪರಿಮಾಣ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಪ್ರತ್ಯಾಗಮ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ಪೂರ್ಣಾಙ್ಕ, 0xff);

bool ifname_valid_char(ವರ್ಣ a) {
        ಯದಿ ((unsigned ವರ್ಣ) a >= 127U)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ ((unsigned ವರ್ಣ) a <= 32U)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool ifname_valid_full(ಧ್ರುವ ವರ್ಣ *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ಪ್ರತಿಜ್ಞಾ(!(flags & ~_IFNAME_VALID_ALL));

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ಯದಿ (parse_ifindex(p) >= 0)
                ಪ್ರತ್ಯಾಗಮ flags & IFNAME_VALID_NUMERIC;

        ಯದಿ (flags & IFNAME_VALID_ALTERNATIVE) {
                ಯದಿ (strlen(p) >= ALTIFNAMSIZ)
                        ಪ್ರತ್ಯಾಗಮ false;
        } ಅನ್ಯಥಾ {
                ಯದಿ (strlen(p) >= IFNAMSIZ)
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಯದಿ (dot_or_dot_dot(p))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ಯದಿ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *t = p; *t; t++) {
                ಯದಿ (!ifname_valid_char(*t))
                        ಪ್ರತ್ಯಾಗಮ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ಯದಿ (numeric)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool address_label_valid(ಧ್ರುವ ವರ್ಣ *p) {

        POINTER_MAY_BE_NULL(p);

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strlen(p) >= IFNAMSIZ)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯಾವತ್ (*p) {
                ಯದಿ ((uint8_t) *p >= 127U)
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ ((uint8_t) *p <= 31U)
                        ಪ್ರತ್ಯಾಗಮ false;
                p++;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ getpeercred(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ ucred *ucred) {
        socklen_t n = ಪರಿಮಾಣ(ಸಂರಚನಾ ucred);
        ಸಂರಚನಾ ucred u;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ucred);

        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (n != ಪರಿಮಾಣ(ಸಂರಚನಾ ucred))
                ಪ್ರತ್ಯಾಗಮ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ಯದಿ (!pid_is_valid(u.pid))
                ಪ್ರತ್ಯಾಗಮ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getpeersec(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        socklen_t n = 64;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (;;) {
                s = new0(ವರ್ಣ, n+1);
                ಯದಿ (!s)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        ಭಙ್ಗ;
                }

                ಯದಿ (errno != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                s = mfree(s);
        }

        ಯದಿ (isempty(s))
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getpeergroups(ಪೂರ್ಣಾಙ್ಕ fd, gid_t **ret) {
        socklen_t n = ಪರಿಮಾಣ(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಙ್ಕ ngroups_max = sysconf_ngroups_max();
        ಯದಿ (ngroups_max > 0)
                n = MAX(n, ಪರಿಮಾಣ(gid_t) * (socklen_t) ngroups_max);

        ಪ್ರತಿ (;;) {
                d = malloc(n);
                ಯದಿ (!d)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        ಭಙ್ಗ;

                ಯದಿ (errno != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                d = mfree(d);
        }

        assert_se(n % ಪರಿಮಾಣ(gid_t) == 0);
        n /= ಪರಿಮಾಣ(gid_t);

        ಯದಿ (n > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        *ret = TAKE_PTR(d);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;
}

ಪೂರ್ಣಾಙ್ಕ getpeerpidfd(ಪೂರ್ಣಾಙ್ಕ fd) {
        socklen_t n = ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ);
        ಪೂರ್ಣಾಙ್ಕ pidfd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (n != ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಪ್ರತ್ಯಾಗಮ pidfd;
}

ಪೂರ್ಣಾಙ್ಕ getpeerpidref(ಪೂರ್ಣಾಙ್ಕ fd, PidRef *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಙ್ಕ pidfd = getpeerpidfd(fd);
        ಯದಿ (pidfd < 0) {
                ಯದಿ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ಪ್ರತ್ಯಾಗಮ pidfd;

                ಸಂರಚನಾ ucred ucred;
                r = getpeercred(fd, &ucred);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ pidref_set_pid(ret, ucred.pid);
        }

        ಪ್ರತ್ಯಾಗಮ pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ಪೂರ್ಣಾಙ್ಕ transport_fd,
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ಸಂರಚನಾ iovec *iov, size_t iovlen,
                ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಙ್ಕ flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))) control = {};
        ಸಂರಚನಾ msghdr mh = {
                .msg_name = (ಸಂರಚನಾ sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ಸಂರಚನಾ iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ಪ್ರತಿಜ್ಞಾ(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ಯದಿ (fd < 0 && !iov)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (fd >= 0) {
                ಸಂರಚನಾ cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = ಪರಿಮಾಣ(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
                memcpy(CMSG_DATA(cmsg), &fd, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ಯದಿ (k < 0)
                ಪ್ರತ್ಯಾಗಮ (ssize_t) -errno;

        ಪ್ರತ್ಯಾಗಮ k;
}

ಪೂರ್ಣಾಙ್ಕ send_one_fd_sa(
                ಪೂರ್ಣಾಙ್ಕ transport_fd,
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಙ್ಕ flags) {

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) send_one_fd_iov_sa(transport_fd, fd, ರಿಕ್ತ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ಪೂರ್ಣಾಙ್ಕ transport_fd,
                ಸಂರಚನಾ iovec *iov, size_t iovlen,
                ಪೂರ್ಣಾಙ್ಕ flags,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ))) control;
        ಸಂರಚನಾ msghdr mh = {
                .msg_control = &control,
                .msg_controllen = ಪರಿಮಾಣ(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ಸಂರಚನಾ cmsghdr *found;
        ssize_t k;

        ಪ್ರತಿಜ್ಞಾ(transport_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ಯದಿ (k < 0)
                ಪ್ರತ್ಯಾಗಮ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)));
        ಯದಿ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ಯದಿ (k == 0)
                        ಪ್ರತ್ಯಾಗಮ -EIO;
        }

        ಯದಿ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, ಪೂರ್ಣಾಙ್ಕ);
        ಅನ್ಯಥಾ
                *ret_fd = -EBADF;

        ಪ್ರತ್ಯಾಗಮ k;
}

ಪೂರ್ಣಾಙ್ಕ receive_one_fd(ಪೂರ್ಣಾಙ್ಕ transport_fd, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪೂರ್ಣಾಙ್ಕ fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ರಿಕ್ತ, 0, flags, &fd);
        ಯದಿ (k == 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ಪ್ರತಿಜ್ಞಾ(k < 0);
        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) k;
}

ssize_t next_datagram_size_fd(ಪೂರ್ಣಾಙ್ಕ fd) {
        ssize_t l;
        ಪೂರ್ಣಾಙ್ಕ k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ರಿಕ್ತ, 0, MSG_PEEK|MSG_TRUNC);
        ಯದಿ (l < 0) {
                ಯದಿ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        ಗಚ್ಛ fallback;

                ಪ್ರತ್ಯಾಗಮ -errno;
        }
        ಯದಿ (l == 0)
                ಗಚ್ಛ fallback;

        ಪ್ರತ್ಯಾಗಮ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ಯದಿ (ioctl(fd, FIONREAD, &k) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

ಪೂರ್ಣಾಙ್ಕ flush_accept(ಪೂರ್ಣಾಙ್ಕ fd) {

        ಪೂರ್ಣಾಙ್ಕ r, b;
        socklen_t l = ಪರಿಮಾಣ(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿಜ್ಞಾ(l == ಪರಿಮಾಣ(b));
        ಯದಿ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        ಪ್ರತಿ (unsigned iteration = 0;; iteration++) {
                ಪೂರ್ಣಾಙ್ಕ cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ಯದಿ (r == -EINTR)
                        ಅನುವರ್ತ;
                ಯದಿ (r <= 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (iteration >= MAX_FLUSH_ITERATIONS)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ರಿಕ್ತ, ರಿಕ್ತ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ಯದಿ (cfd < 0) {
                        ಯದಿ (errno == EAGAIN)
                                ಪ್ರತ್ಯಾಗಮ 0;

                        ಯದಿ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ಅನುವರ್ತ;

                        ಪ್ರತ್ಯಾಗಮ -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(ಪೂರ್ಣಾಙ್ಕ fd) {
        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        ಸಂರಚನಾ mq_attr attr;
        ssize_t ಎಣಿಕೆ = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        ಪ್ರತಿ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ಯದಿ (r == -EINTR)
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;

                ಯದಿ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ಯದಿ (mq_getattr(fd, &attr) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        buf = malloc(attr.mq_msgsize);
                        ಯದಿ (!buf)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ರಿಕ್ತ);
                ಯದಿ (l < 0) {
                        ಯದಿ (errno == EINTR)
                                ಅನುವರ್ತ;

                        ಯದಿ (errno == EAGAIN)
                                ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;

                        ಪ್ರತ್ಯಾಗಮ -errno;
                }

                ಎಣಿಕೆ += l;
        }
}

ಸಂರಚನಾ cmsghdr* cmsg_find(ಸಂರಚನಾ msghdr *mh, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ type, socklen_t length) {
        ಸಂರಚನಾ cmsghdr *cmsg;

        ಪ್ರತಿಜ್ಞಾ(mh);

        CMSG_FOREACH(cmsg, mh)
                ಯದಿ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ಪ್ರತ್ಯಾಗಮ cmsg;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ* cmsg_find_and_copy_data(ಸಂರಚನಾ msghdr *mh, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *buf, size_t buf_len) {
        ಸಂರಚನಾ cmsghdr *cmsg;

        ಪ್ರತಿಜ್ಞಾ(mh);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ಯದಿ (!cmsg)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ಧ್ರುವ ಸಂರಚನಾ sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ಪ್ರತಿಜ್ಞಾ(sa->sll_family == AF_PACKET);

        size_t mac_len = ಪರಿಮಾಣ(sa->sll_addr);

        ಯದಿ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ಯದಿ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ಪ್ರತ್ಯಾಗಮ offsetof(ಸಂರಚನಾ sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ಧ್ರುವ ಸಂರಚನಾ sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ಪ್ರತಿಜ್ಞಾ(sa->sun_family == AF_UNIX);

        ಪ್ರತ್ಯಾಗಮ offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, ಪರಿಮಾಣ(sa->sun_path)-1) :
                        strnlen(sa->sun_path, ಪರಿಮಾಣ(sa->sun_path))+1);
}

size_t sockaddr_len(ಧ್ರುವ ಸಙ್ಘ sockaddr_union *sa) {
        ಪ್ರತಿಜ್ಞಾ(sa);

        ವಿಕಲ್ಪ (sa->sa.sa_family) {
        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in);
        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_in6);
        ಪಕ್ಷ AF_UNIX:
                ಪ್ರತ್ಯಾಗಮ sockaddr_un_len(&sa->un);
        ಪಕ್ಷ AF_PACKET:
                ಪ್ರತ್ಯಾಗಮ sockaddr_ll_len(&sa->ll);
        ಪಕ್ಷ AF_NETLINK:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_nl);
        ಪಕ್ಷ AF_VSOCK:
                ಪ್ರತ್ಯಾಗಮ ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_vm);
        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_ioctl_fd(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ಯದಿ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಪೂರ್ಣಾಙ್ಕ sockaddr_un_unlink(ಧ್ರುವ ಸಂರಚನಾ sockaddr_un *sa) {
        ಧ್ರುವ ವರ್ಣ *p, * nul;

        ಪ್ರತಿಜ್ಞಾ(sa);

        ಯದಿ (sa->sun_family != AF_UNIX)
                ಪ್ರತ್ಯಾಗಮ -EPROTOTYPE;

        ಯದಿ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ಪ್ರತ್ಯಾಗಮ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, ಪರಿಮಾಣ(sa->sun_path));
        ಯದಿ (nul)
                p = sa->sun_path;
        ಅನ್ಯಥಾ
                p = memdupa_suffix0(sa->sun_path, ಪರಿಮಾಣ(sa->sun_path));

        ಯದಿ (unlink(p) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ sockaddr_un_set_path(ಸಂರಚನಾ sockaddr_un *ret, ಧ್ರುವ ವರ್ಣ *path) {
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ಯದಿ (l < 2)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (!IN_SET(path[0], '/', '@'))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ಯದಿ (l+1 > ಪರಿಮಾಣ(ret->sun_path))
                ಪ್ರತ್ಯಾಗಮ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ಸಂರಚನಾ sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ಯದಿ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) (offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ಅನ್ಯಥಾ {
                ಪ್ರತಿಜ್ಞಾ(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) (offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ಪೂರ್ಣಾಙ್ಕ getsockopt_int(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ optname, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪೂರ್ಣಾಙ್ಕ v;
        socklen_t sl = ಪರಿಮಾಣ(v);

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ಪ್ರತ್ಯಾಗಮ negative_errno();
        ಯದಿ (sl != ಪರಿಮಾಣ(v))
                ಪ್ರತ್ಯಾಗಮ -EIO;

        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ socket_bind_to_ifname(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *ifname) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Call with NULL to drop binding */

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ಪೂರ್ಣಾಙ್ಕ socket_bind_to_ifindex(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ ifindex) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (ifindex <= 0)
                /* Drop binding */
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ರಿಕ್ತ, 0));

        ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ಪೂರ್ಣಾಙ್ಕ socket_autobind(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret_name) {
        _cleanup_free_ ವರ್ಣ *name = ರಿಕ್ತ;
        uint64_t random;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        random = random_u64();

        ಯದಿ (asprintf(&name, "@%" PRIu64, random) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಸಙ್ಘ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < ಪರಿಮಾಣ(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (bind(fd, &sa.sa, r) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_name)
                *ret_name = TAKE_PTR(name);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ssize_t recvmsg_safe(ಪೂರ್ಣಾಙ್ಕ sockfd, ಸಂರಚನಾ msghdr *msg, ಪೂರ್ಣಾಙ್ಕ flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ಪ್ರತಿಜ್ಞಾ(sockfd >= 0);
        ಪ್ರತಿಜ್ಞಾ(msg);

        n = recvmsg(sockfd, msg, flags);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ಪ್ರತ್ಯಾಗಮ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ socket_get_family(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ af;
        socklen_t sl = ಪರಿಮಾಣ(af);

        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (sl != ಪರಿಮಾಣ(af))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ af;
}

ಪೂರ್ಣಾಙ್ಕ socket_set_recvpktinfo(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {

        ಯದಿ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಯದಿ (af < 0)
                        ಪ್ರತ್ಯಾಗಮ af;
        }

        ವಿಕಲ್ಪ (af) {

        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        ಪಕ್ಷ AF_NETLINK:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        ಪಕ್ಷ AF_PACKET:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_set_unicast_if(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, ಪೂರ್ಣಾಙ್ಕ ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ಯದಿ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಯದಿ (af < 0)
                        ಪ್ರತ್ಯಾಗಮ af;
        }

        ವಿಕಲ್ಪ (af) {

        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, ಪರಿಮಾಣ(ifindex_be)));

        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, ಪರಿಮಾಣ(ifindex_be)));

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_set_option(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, ಪೂರ್ಣಾಙ್ಕ opt_ipv4, ಪೂರ್ಣಾಙ್ಕ opt_ipv6, ಪೂರ್ಣಾಙ್ಕ val) {
        ಯದಿ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಯದಿ (af < 0)
                        ಪ್ರತ್ಯಾಗಮ af;
        }

        ವಿಕಲ್ಪ (af) {

        ಪಕ್ಷ AF_INET:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        ಪಕ್ಷ AF_INET6:
                ಪ್ರತ್ಯಾಗಮ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_get_mtu(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, size_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ mtu, r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಯದಿ (af < 0)
                        ಪ್ರತ್ಯಾಗಮ af;
        }

        ವಿಕಲ್ಪ (af) {

        ಪಕ್ಷ AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                ಭಙ್ಗ;

        ಪಕ್ಷ AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;
        }

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (mtu <= 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = (size_t) mtu;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ connect_unix_path_simple(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path) {
        ಸಙ್ಘ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(path);

        l = strlen(path);
        ಪ್ರತಿಜ್ಞಾ(l > 0);
        ಪ್ರತಿಜ್ಞಾ(l < ಪರಿಮಾಣ(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(connect(fd, &sa.sa, offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + l + 1));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ connect_unix_inode(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ inode_fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(inode_fd >= 0);

        ಪ್ರತ್ಯಾಗಮ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

ಪೂರ್ಣಾಙ್ಕ connect_unix_path(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ inode_fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ಯದಿ (!path) {
                ಯದಿ (dir_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -EISDIR;

                ಪ್ರತ್ಯಾಗಮ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Shortcut for the simple case */
        ಯದಿ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(ಸಂರಚನಾ sockaddr_un, sun_path))
                ಪ್ರತ್ಯಾಗಮ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ಯದಿ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ ವರ್ಣ *j = strjoin("/", path);
                ಯದಿ (!j)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } ಅನ್ಯಥಾ
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ಯದಿ (inode_fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ connect_unix_inode(fd, inode_fd);
}

ಪೂರ್ಣಾಙ್ಕ socket_address_parse_unix(SocketAddress *ret_address, ಧ್ರುವ ವರ್ಣ *s) {
        ಸಂರಚನಾ sockaddr_un un;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_address);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (!IN_SET(*s, '/', '@'))
                ಪ್ರತ್ಯಾಗಮ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ socket_address_equal_unix(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        SocketAddress socket_a, socket_b;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = socket_address_parse_unix(&socket_a, a);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = socket_address_parse_unix(&socket_b, b);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ಪೂರ್ಣಾಙ್ಕ vsock_parse_port(ಧ್ರುವ ವರ್ಣ *s, unsigned *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ಯದಿ (u == VMADDR_PORT_ANY)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ vsock_parse_cid(ಧ್ರುವ ವರ್ಣ *s, unsigned *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ಯದಿ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ಅನ್ಯಥಾ ಯದಿ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ಅನ್ಯಥಾ ಯದಿ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ಅನ್ಯಥಾ ಯದಿ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ safe_atou(s, ret);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ socket_address_parse_vsock(SocketAddress *ret_address, ಧ್ರುವ ವರ್ಣ *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e, *cid_start;
        unsigned port, cid;
        ಪೂರ್ಣಾಙ್ಕ type, r;

        ಪ್ರತಿಜ್ಞಾ(ret_address);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        ಅನ್ಯಥಾ ಯದಿ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ಅನ್ಯಥಾ ಯದಿ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ಅನ್ಯಥಾ ಯದಿ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -EPROTO;

        e = strchr(cid_start, ':');
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        n = strndup(cid_start, e - cid_start);
        ಯದಿ (!n)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (isempty(n))
                cid = VMADDR_CID_ANY;
        ಅನ್ಯಥಾ {
                r = vsock_parse_cid(n, &cid);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_vm),
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ netlink_socket_get_multicast_groups(ಪೂರ್ಣಾಙ್ಕ fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ರಿಕ್ತ;
        socklen_t len = 0, old_len;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ರಿಕ್ತ, &len) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (len == 0)
                ಗಚ್ಛ finalize;

        groups = new0(uint32_t, len);
        ಯದಿ (!groups)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        old_len = len;

        ಯದಿ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (old_len != len)
                ಪ್ರತ್ಯಾಗಮ -EIO;

finalize:
        ಯದಿ (ret_len)
                *ret_len = len;
        ಯದಿ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ socket_get_cookie(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = ಪರಿಮಾಣ(cookie);
        ಯದಿ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿಜ್ಞಾ(cookie_len == ಪರಿಮಾಣ(cookie));
        ಯದಿ (ret)
                *ret = cookie;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ cmsg_close_all(ಸಂರಚನಾ msghdr *mh) {
        ಪ್ರತಿಜ್ಞಾ(mh);

        ಸಂರಚನಾ cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ಯದಿ (cmsg->cmsg_level != SOL_SOCKET)
                        ಅನುವರ್ತ;

                ಯದಿ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, ಪೂರ್ಣಾಙ್ಕ),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
                ಅನ್ಯಥಾ ಯದಿ (cmsg->cmsg_type == SCM_PIDFD) {
                        ಪ್ರತಿಜ್ಞಾ(cmsg->cmsg_len == CMSG_LEN(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, ಪೂರ್ಣಾಙ್ಕ));
                }
        }
}

ಪೂರ್ಣಾಙ್ಕ tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        ವಿಕಲ್ಪ (IPTOS_PREC(tos)) {
        ಪಕ್ಷ IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        ಪಕ್ಷ IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ಪ್ರತ್ಯಾಗಮ TC_PRIO_CONTROL;

        ಪಕ್ಷ IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        ಪಕ್ಷ IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ಪ್ರತ್ಯಾಗಮ TC_PRIO_INTERACTIVE;

        ಪಕ್ಷ IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        ಪಕ್ಷ IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ಪ್ರತ್ಯಾಗಮ TC_PRIO_INTERACTIVE_BULK;

        ಪಕ್ಷ IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ಪ್ರತ್ಯಾಗಮ TC_PRIO_BULK;

        ಪಕ್ಷ IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ TC_PRIO_BESTEFFORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ socket_xattr_supported(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cached = -1;
        ಯದಿ (cached >= 0)
                ಪ್ರತ್ಯಾಗಮ cached;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ರಿಕ್ತ, AT_EMPTY_PATH, "user.testxxx", "1");
        ಯದಿ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ಪ್ರತ್ಯಾಗಮ (cached = false);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ಪ್ರತ್ಯಾಗಮ (cached = true);
}
