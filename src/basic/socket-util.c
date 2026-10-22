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

#ಆದರೆ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ಇಲ್ಲದಿದ್ದರೆ
#  define IDN_FLAGS 0
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ socket_ವಿಳಾಸ_type_ಕೋಷ್ಟಕ[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(socket_ವಿಳಾಸ_type, ಪೂರ್ಣಾಂಕ);

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_verify(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, bool strict) {
        ಪ್ರತಿಜ್ಞಾ(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        ಆಯ್ಕೆ (socket_ವಿಳಾಸ_family(a)) {

        ಸ್ಥಿತಿ AF_INET:
                ಆದರೆ (a->ಗಾತ್ರ_2 != ಗಾತ್ರ(ರಚನೆ sockaddr_in))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (a->sockaddr.ಒಳ.sin_port == 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ಒಳ_ಗಣ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_INET6:
                ಆದರೆ (a->ಗಾತ್ರ_2 != ಗಾತ್ರ(ರಚನೆ sockaddr_in6))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (a->sockaddr.in6.sin6_port == 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ಒಳ_ಗಣ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_UNIX:
                ಆದರೆ (a->ಗಾತ್ರ_2 < offsetof(ರಚನೆ sockaddr_un, sun_path))
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಆದರೆ (a->ಗಾತ್ರ_2 > ಗಾತ್ರ(ರಚನೆ sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (a->ಗಾತ್ರ_2 > offsetof(ರಚನೆ sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, ಗಾತ್ರ(a->sockaddr.un.sun_path));
                        ಆದರೆ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ಆದರೆ (a->ಗಾತ್ರ_2 != offsetof(ರಚನೆ sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ಹಿಂತಿರುಗಿ -EINVAL;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ಆದರೆ (!ಒಳ_ಗಣ(a->ಗಾತ್ರ_2, ಗಾತ್ರ(a->sockaddr.un.sun_path), ಗಾತ್ರ(a->sockaddr.un.sun_path)+1))
                                        ಹಿಂತಿರುಗಿ -EINVAL;
                        }
                }

                ಆದರೆ (!ಒಳ_ಗಣ(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_NETLINK:

                ಆದರೆ (a->ಗಾತ್ರ_2 != ಗಾತ್ರ(ರಚನೆ sockaddr_nl))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ಒಳ_ಗಣ(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_VSOCK:
                ಆದರೆ (a->ಗಾತ್ರ_2 != ಗಾತ್ರ(ರಚನೆ sockaddr_vm))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (!ಒಳ_ಗಣ(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ 0;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_print(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = socket_ವಿಳಾಸ_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (socket_ವಿಳಾಸ_family(a) == AF_NETLINK) {
                _cleanup_free_ ಅಕ್ಷರ *sfamily = ಖಾಲಿ;

                r = netlink_family_ಗಮ್ಯ_string_alloc(a->protocol, &sfamily);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ sockaddr_pretty(&a->sockaddr.sa, a->ಗಾತ್ರ_2, false, true, ret);
}

bool socket_ವಿಳಾಸ_can_accept(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಹಿಂತಿರುಗಿ
                ಒಳ_ಗಣ(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_ವಿಳಾಸ_equal(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        /* Invalid addresses are unequal to all */
        ಆದರೆ (socket_ವಿಳಾಸ_verify(a, false) < 0 ||
            socket_ವಿಳಾಸ_verify(b, false) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (a->type != b->type)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (socket_ವಿಳಾಸ_family(a) != socket_ವಿಳಾಸ_family(b))
                ಹಿಂತಿರುಗಿ false;

        ಆಯ್ಕೆ (socket_ವಿಳಾಸ_family(a)) {

        ಸ್ಥಿತಿ AF_INET:
                ಆದರೆ (a->sockaddr.ಒಳ.sin_addr.s_addr != b->sockaddr.ಒಳ.sin_addr.s_addr)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (a->sockaddr.ಒಳ.sin_port != b->sockaddr.ಒಳ.sin_port)
                        ಹಿಂತಿರುಗಿ false;

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_INET6:
                ಆದರೆ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, ಗಾತ್ರ(a->sockaddr.in6.sin6_addr)) != 0)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ಹಿಂತಿರುಗಿ false;

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_UNIX:
                ಆದರೆ (a->ಗಾತ್ರ_2 <= offsetof(ರಚನೆ sockaddr_un, sun_path) ||
                    b->ಗಾತ್ರ_2 <= offsetof(ರಚನೆ sockaddr_un, sun_path))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (a->sockaddr.un.sun_path[0]) {
                        ಆದರೆ (!ಮಾರ್ಗ_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ಹಿಂತಿರುಗಿ false;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (a->ಗಾತ್ರ_2 != b->ಗಾತ್ರ_2)
                                ಹಿಂತಿರುಗಿ false;

                        ಆದರೆ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->ಗಾತ್ರ_2) != 0)
                                ಹಿಂತಿರುಗಿ false;
                }

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_NETLINK:
                ಆದರೆ (a->protocol != b->protocol)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ಹಿಂತಿರುಗಿ false;

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_VSOCK:
                ಆದರೆ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ಹಿಂತಿರುಗಿ false;

                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                /* Cannot compare, so we assume the addresses are different */
                ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* socket_ವಿಳಾಸ_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a) {
        ಪ್ರತಿಜ್ಞಾ(a);

        ಆದರೆ (socket_ವಿಳಾಸ_family(a) != AF_UNIX)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (a->sockaddr.un.sun_path[0] == 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(ಗಾತ್ರ(ಒಕ್ಕೂಟ sockaddr_union) >= ಗಾತ್ರ(ರಚನೆ sockaddr_un)+1);
        ಹಿಂತಿರುಗಿ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached = -1;

        ಆದರೆ (cached < 0) {

                ಆದರೆ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ಆದರೆ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ಹಿಂತಿರುಗಿ false;
                        }

                        cached = false;
                } ಇಲ್ಲದಿದ್ದರೆ
                        cached = true;
        }

        ಹಿಂತಿರುಗಿ cached;
}

bool socket_ipv6_is_enabled(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ಆದರೆ (!socket_ipv6_is_supported())
                ಹಿಂತಿರುಗಿ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ಆದರೆ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ಹಿಂತಿರುಗಿ true;
        }

        r = parse_boolean(v);
        ಆದರೆ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ !r;
}

bool socket_ವಿಳಾಸ_matches_fd(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಪೂರ್ಣಾಂಕ fd) {
        Socket_ವಿಳಾಸ b;
        socklen_t solen;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        b.ಗಾತ್ರ_2 = ಗಾತ್ರ(b.sockaddr);
        ಆದರೆ (getsockname(fd, &b.sockaddr.sa, &b.ಗಾತ್ರ_2) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ಹಿಂತಿರುಗಿ false;

        solen = ಗಾತ್ರ(b.type);
        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (b.type != a->type)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (a->protocol != 0)  {
                solen = ಗಾತ್ರ(b.protocol);
                ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (b.protocol != a->protocol)
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ socket_ವಿಳಾಸ_equal(a, &b);
}

ಪೂರ್ಣಾಂಕ sockaddr_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *_sa, ಚಿಹ್ನೆರಹಿತ *ret_ಪೋರ್ಟ್) {
        ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *sa = (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(ret_ಪೋರ್ಟ್);

        ಆಯ್ಕೆ (sa->sa.sa_family) {

        ಸ್ಥಿತಿ AF_INET:
                *ret_ಪೋರ್ಟ್ = be16toh(sa->ಒಳ.sin_port);
                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_INET6:
                *ret_ಪೋರ್ಟ್ = be16toh(sa->in6.sin6_port);
                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_VSOCK:
                *ret_ಪೋರ್ಟ್ = sa->vm.svm_port;
                ಹಿಂತಿರುಗಿ 0;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *sockaddr_ಒಳ_addr(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *_sa) {
        ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *sa = (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union*) _sa;

        ಆದರೆ (!sa)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆಯ್ಕೆ (sa->sa.sa_family) {

        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union*) &sa->ಒಳ.sin_addr;

        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union*) &sa->in6.sin6_addr;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
}

ಪೂರ್ಣಾಂಕ sockaddr_ಗಣ_ಒಳ_addr(
                ಒಕ್ಕೂಟ sockaddr_union *u,
                ಪೂರ್ಣಾಂಕ family,
                ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a,
                uint16_t port) {

        ಪ್ರತಿಜ್ಞಾ(u);
        ಪ್ರತಿಜ್ಞಾ(a);

        ಆಯ್ಕೆ (family) {

        ಸ್ಥಿತಿ AF_INET:
                u->ಒಳ = (ರಚನೆ sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->ಒಳ,
                        .sin_port = htobe16(port),
                };

                ಹಿಂತಿರುಗಿ 0;

        ಸ್ಥಿತಿ AF_INET6:
                u->in6 = (ರಚನೆ sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ಹಿಂತಿರುಗಿ 0;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        }
}

ಪೂರ್ಣಾಂಕ sockaddr_pretty(
                ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_ಪೋರ್ಟ್,
                ಅಕ್ಷರ **ret) {

        ಒಕ್ಕೂಟ sockaddr_union *sa = (ಒಕ್ಕೂಟ sockaddr_union*) _sa;
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(salen >= ಗಾತ್ರ(sa->sa.sa_family));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆಯ್ಕೆ (sa->sa.sa_family) {

        ಸ್ಥಿತಿ AF_INET: {
                uint32_t a;

                a = be32toh(sa->ಒಳ.sin_addr.s_addr);

                ಆದರೆ (include_ಪೋರ್ಟ್)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->ಒಳ.sin_port));
                ಇಲ್ಲದಿದ್ದರೆ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ AF_INET6: {
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ipv4_ಪೂರ್ವಪ್ರತ್ಯಯ[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ಆದರೆ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_ಪೂರ್ವಪ್ರತ್ಯಯ, ಗಾತ್ರ(ipv4_ಪೂರ್ವಪ್ರತ್ಯಯ)) == 0) {
                        ಸ್ಥಿರಾಂಕ uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ಆದರೆ (include_ಪೋರ್ಟ್)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ಇಲ್ಲದಿದ್ದರೆ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a = IN6_ADDR_ಗಮ್ಯ_STRING(&sa->in6.sin6_addr);

                        ಆದರೆ (include_ಪೋರ್ಟ್) {
                                ಆದರೆ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_ತುಂಬಿದ(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_ತುಂಬಿದ(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ಇಲ್ಲದಿದ್ದರೆ
                                        p = strdup(a);
                                ಆದರೆ (!p)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                        }
                }

                ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ AF_UNIX:
                ಆದರೆ (salen <= offsetof(ರಚನೆ sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ರಚನೆ sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ಇಲ್ಲದಿದ್ದರೆ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        ಅಕ್ಷರ *ಮಾರ್ಗ = (ಅಕ್ಷರ*) sa->un_ಬಫರ್ + offsetof(ರಚನೆ sockaddr_un, sun_path);
                        size_t ಮಾರ್ಗ_len = salen - offsetof(ರಚನೆ sockaddr_un, sun_path);

                        ಆದರೆ (ಮಾರ್ಗ[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ ಅಕ್ಷರ *e = ಖಾಲಿ;

                                e = cescape_ಉದ್ದಳತೆ(ಮಾರ್ಗ + 1, ಮಾರ್ಗ_len - 1);
                                ಆದರೆ (!e)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                p = strjoin("@", e);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (ಮಾರ್ಗ[ಮಾರ್ಗ_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        ಮಾರ್ಗ_len--;

                                p = cescape_ಉದ್ದಳತೆ(ಮಾರ್ಗ, ಮಾರ್ಗ_len);
                        }
                }
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_VSOCK:
                ಆದರೆ (include_ಪೋರ್ಟ್) {
                        ಆದರೆ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ಇಲ್ಲದಿದ್ದರೆ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ಇಲ್ಲದಿದ್ದರೆ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }

        *ret = p;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getpeername_pretty(ಪೂರ್ಣಾಂಕ fd, bool include_ಪೋರ್ಟ್, ಅಕ್ಷರ **ret) {
        ಒಕ್ಕೂಟ sockaddr_union sa;
        socklen_t salen = ಗಾತ್ರ(sa);
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (getpeername(fd, &sa.sa, &salen) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (sa.sa.sa_family == AF_UNIX) {
                ರಚನೆ ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಹಿಂತಿರುಗಿ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ಹಿಂತಿರುಗಿ sockaddr_pretty(&sa.sa, salen, true, include_ಪೋರ್ಟ್, ret);
}

ಪೂರ್ಣಾಂಕ getsockname_pretty(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        ಒಕ್ಕೂಟ sockaddr_union sa;
        socklen_t salen = ಗಾತ್ರ(sa);

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (getsockname(fd, &sa.sa, &salen) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ಹಿಂತಿರುಗಿ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ಪೂರ್ಣಾಂಕ socknameinfo_pretty(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t salen, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ host[NI_MAXHOST];
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(sa);
        ಪ್ರತಿಜ್ಞಾ(salen >= ಗಾತ್ರ(sa_family_t));
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getnameinfo(sa, salen, host, ಗಾತ್ರ(host), /* service= */ ಖಾಲಿ, /* service_len= */ 0, IDN_FLAGS);
        ಆದರೆ (r != 0) {
                ಆದರೆ (r == EAI_MEMORY)
                        ಹಿಂತಿರುಗಿ log_oom_debug();
                ಆದರೆ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ಇಲ್ಲದಿದ್ದರೆ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ಹಿಂತಿರುಗಿ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, host);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ netlink_family_ಕೋಷ್ಟಕ[] = {
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(netlink_family, ಪೂರ್ಣಾಂಕ, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);

bool sockaddr_equal(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *a, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಆದರೆ (a->sa.sa_family != b->sa.sa_family)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (a->sa.sa_family == AF_INET)
                ಹಿಂತಿರುಗಿ a->ಒಳ.sin_addr.s_addr == b->ಒಳ.sin_addr.s_addr;

        ಆದರೆ (a->sa.sa_family == AF_INET6)
                ಹಿಂತಿರುಗಿ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, ಗಾತ್ರ(a->in6.sin6_addr)) == 0;

        ಆದರೆ (a->sa.sa_family == AF_VSOCK)
                ಹಿಂತಿರುಗಿ a->vm.svm_cid == b->vm.svm_cid;

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ fd_ಗಣ_sndbuf(ಪೂರ್ಣಾಂಕ fd, size_t n, bool increase) {
        ಪೂರ್ಣಾಂಕ r, ಮೌಲ್ಯ;
        socklen_t l = ಗಾತ್ರ(ಮೌಲ್ಯ);

        ಆದರೆ (n > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ಮೌಲ್ಯ, &l);
        ಆದರೆ (r >= 0 && l == ಗಾತ್ರ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಹಿಂತಿರುಗಿ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ಗಾತ್ರ(ಮೌಲ್ಯ);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ಮೌಲ್ಯ, &l);
        ಆದರೆ (r >= 0 && l == ಗಾತ್ರ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಹಿಂತಿರುಗಿ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ fd_ಗಣ_rcvbuf(ಪೂರ್ಣಾಂಕ fd, size_t n, bool increase) {
        ಪೂರ್ಣಾಂಕ r, ಮೌಲ್ಯ;
        socklen_t l = ಗಾತ್ರ(ಮೌಲ್ಯ);

        ಆದರೆ (n > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ಮೌಲ್ಯ, &l);
        ಆದರೆ (r >= 0 && l == ಗಾತ್ರ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಹಿಂತಿರುಗಿ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ಗಾತ್ರ(ಮೌಲ್ಯ);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ಮೌಲ್ಯ, &l);
        ಆದರೆ (r >= 0 && l == ಗಾತ್ರ(ಮೌಲ್ಯ) && increase ? (size_t) ಮೌಲ್ಯ >= n*2 : (size_t) ಮೌಲ್ಯ == n*2)
                ಹಿಂತಿರುಗಿ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ip_tos_ಕೋಷ್ಟಕ[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(ip_tos, ಪೂರ್ಣಾಂಕ, 0xff);

bool ifname_valid_char(ಅಕ್ಷರ a) {
        ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) a >= 127U)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) a <= 32U)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಒಳ_ಗಣ(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool ifname_valid_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ಪ್ರತಿಜ್ಞಾ(!(flags & ~_IFNAME_VALID_ALL));

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ಆದರೆ (parse_ifindex(p) >= 0)
                ಹಿಂತಿರುಗಿ flags & IFNAME_VALID_NUMERIC;

        ಆದರೆ (flags & IFNAME_VALID_ALTERNATIVE) {
                ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) >= ALTIFNAMSIZ)
                        ಹಿಂತಿರುಗಿ false;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) >= IFNAMSIZ)
                        ಹಿಂತಿರುಗಿ false;
        }

        ಆದರೆ (dot_or_dot_dot(p))
                ಹಿಂತಿರುಗಿ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ಆದರೆ (!FLAGS_ಗಣ(flags, IFNAME_VALID_SPECIAL) && STR_ಒಳ_ಗಣ(p, "all", "default"))
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t = p; *t; t++) {
                ಆದರೆ (!ifname_valid_char(*t))
                        ಹಿಂತಿರುಗಿ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ಆದರೆ (numeric)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool ವಿಳಾಸ_label_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {

        ಸೂಚಕ_MAY_BE_NULL(p);

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p) >= IFNAMSIZ)
                ಹಿಂತಿರುಗಿ false;

        ಆಗ (*p) {
                ಆದರೆ ((uint8_t) *p >= 127U)
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ ((uint8_t) *p <= 31U)
                        ಹಿಂತಿರುಗಿ false;
                p++;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ getpeercred(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ ucred *ucred) {
        socklen_t n = ಗಾತ್ರ(ರಚನೆ ucred);
        ರಚನೆ ucred u;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ucred);

        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (n != ಗಾತ್ರ(ರಚನೆ ucred))
                ಹಿಂತಿರುಗಿ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ಆದರೆ (!pid_is_valid(u.pid))
                ಹಿಂತಿರುಗಿ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getpeersec(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        socklen_t n = 64;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (;;) {
                s = new0(ಅಕ್ಷರ, n+1);
                ಆದರೆ (!s)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (errno != ERANGE)
                        ಹಿಂತಿರುಗಿ -errno;

                s = mfree(s);
        }

        ಆದರೆ (isempty(s))
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getpeergroups(ಪೂರ್ಣಾಂಕ fd, gid_t **ret) {
        socklen_t n = ಗಾತ್ರ(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಂಕ ngroups_max = sysconf_ngroups_max();
        ಆದರೆ (ngroups_max > 0)
                n = MAX(n, ಗಾತ್ರ(gid_t) * (socklen_t) ngroups_max);

        ಗಾಗಿ (;;) {
                d = ಸ್ಮರಣೆ_ಕೊಡು(n);
                ಆದರೆ (!d)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (errno != ERANGE)
                        ಹಿಂತಿರುಗಿ -errno;

                d = mfree(d);
        }

        assert_se(n % ಗಾತ್ರ(gid_t) == 0);
        n /= ಗಾತ್ರ(gid_t);

        ಆದರೆ (n > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -E2BIG;

        *ret = TAKE_PTR(d);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;
}

ಪೂರ್ಣಾಂಕ getpeerpidfd(ಪೂರ್ಣಾಂಕ fd) {
        socklen_t n = ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ);
        ಪೂರ್ಣಾಂಕ pidfd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (n != ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))
                ಹಿಂತಿರುಗಿ -EIO;

        ಹಿಂತಿರುಗಿ pidfd;
}

ಪೂರ್ಣಾಂಕ getpeerpidref(ಪೂರ್ಣಾಂಕ fd, PidRef *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಂಕ pidfd = getpeerpidfd(fd);
        ಆದರೆ (pidfd < 0) {
                ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ಹಿಂತಿರುಗಿ pidfd;

                ರಚನೆ ucred ucred;
                r = getpeercred(fd, &ucred);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ pidref_ಗಣ_pid(ret, ucred.pid);
        }

        ಹಿಂತಿರುಗಿ pidref_ಗಣ_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ಪೂರ್ಣಾಂಕ transport_fd,
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, size_t iovlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಂಕ flags) {

        CMSG_ಬಫರ್_TYPE(CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))) control = {};
        ರಚನೆ msghdr mh = {
                .msg_name = (ರಚನೆ sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ರಚನೆ iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ಪ್ರತಿಜ್ಞಾ(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ಆದರೆ (fd < 0 && !iov)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (fd >= 0) {
                ರಚನೆ cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = ಗಾತ್ರ(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
                ಸ್ಮರಣೆ_ನಕಲು(CMSG_DATA(cmsg), &fd, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ಆದರೆ (k < 0)
                ಹಿಂತಿರುಗಿ (ssize_t) -errno;

        ಹಿಂತಿರುಗಿ k;
}

ಪೂರ್ಣಾಂಕ send_one_fd_sa(
                ಪೂರ್ಣಾಂಕ transport_fd,
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಂಕ flags) {

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) send_one_fd_iov_sa(transport_fd, fd, ಖಾಲಿ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ಪೂರ್ಣಾಂಕ transport_fd,
                ರಚನೆ iovec *iov, size_t iovlen,
                ಪೂರ್ಣಾಂಕ flags,
                ಪೂರ್ಣಾಂಕ *ret_fd) {

        CMSG_ಬಫರ್_TYPE(CMSG_SPACE(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))) control;
        ರಚನೆ msghdr mh = {
                .msg_control = &control,
                .msg_controllen = ಗಾತ್ರ(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ರಚನೆ cmsghdr *ಸಿಕ್ಕಿತು;
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

        k = recvmsg_ಸುರಕ್ಷಿತ(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ಆದರೆ (k < 0)
                ಹಿಂತಿರುಗಿ k;

        ಸಿಕ್ಕಿತು = cmsg_ಕಂಡುಹಿಡಿ(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)));
        ಆದರೆ (!ಸಿಕ್ಕಿತು) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ಆದರೆ (k == 0)
                        ಹಿಂತಿರುಗಿ -EIO;
        }

        ಆದರೆ (ಸಿಕ್ಕಿತು)
                *ret_fd = *CMSG_TYPED_ದತ್ತಾಂಶ(ಸಿಕ್ಕಿತು, ಪೂರ್ಣಾಂಕ);
        ಇಲ್ಲದಿದ್ದರೆ
                *ret_fd = -EBADF;

        ಹಿಂತಿರುಗಿ k;
}

ಪೂರ್ಣಾಂಕ receive_one_fd(ಪೂರ್ಣಾಂಕ transport_fd, ಪೂರ್ಣಾಂಕ flags) {
        ಪೂರ್ಣಾಂಕ fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ಖಾಲಿ, 0, flags, &fd);
        ಆದರೆ (k == 0)
                ಹಿಂತಿರುಗಿ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ಪ್ರತಿಜ್ಞಾ(k < 0);
        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) k;
}

ssize_t ಮುಂದಿನ_datagram_ಗಾತ್ರ_fd(ಪೂರ್ಣಾಂಕ fd) {
        ssize_t l;
        ಪೂರ್ಣಾಂಕ k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ಖಾಲಿ, 0, MSG_PEEK|MSG_TRUNC);
        ಆದರೆ (l < 0) {
                ಆದರೆ (ಒಳ_ಗಣ(errno, EOPNOTSUPP, EFAULT))
                        ಹೋಗು fallback;

                ಹಿಂತಿರುಗಿ -errno;
        }
        ಆದರೆ (l == 0)
                ಹೋಗು fallback;

        ಹಿಂತಿರುಗಿ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ಆದರೆ (ioctl(fd, FIONREAD, &k) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FLUSH_ITERATIONS 1024

ಪೂರ್ಣಾಂಕ flush_accept(ಪೂರ್ಣಾಂಕ fd) {

        ಪೂರ್ಣಾಂಕ r, b;
        socklen_t l = ಗಾತ್ರ(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪ್ರತಿಜ್ಞಾ(l == ಗಾತ್ರ(b));
        ಆದರೆ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ iteration = 0;; iteration++) {
                ಪೂರ್ಣಾಂಕ cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ಆದರೆ (r == -EINTR)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (iteration >= MAX_FLUSH_ITERATIONS)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ಖಾಲಿ, ಖಾಲಿ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ಆದರೆ (cfd < 0) {
                        ಆದರೆ (errno == EAGAIN)
                                ಹಿಂತಿರುಗಿ 0;

                        ಆದರೆ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ಮುಂದುವರಿ;

                        ಹಿಂತಿರುಗಿ -errno;
                }

                ಸುರಕ್ಷಿತ_close(cfd);
        }
}

ssize_t flush_mqueue(ಪೂರ್ಣಾಂಕ fd) {
        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        ರಚನೆ mq_attr attr;
        ssize_t ಎಣಿಕೆ_2 = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        ಗಾಗಿ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ಆದರೆ (r == -EINTR)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;

                ಆದರೆ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ಆದರೆ (mq_getattr(fd, &attr) < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        buf = ಸ್ಮರಣೆ_ಕೊಡು(attr.mq_msgsize);
                        ಆದರೆ (!buf)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ಖಾಲಿ);
                ಆದರೆ (l < 0) {
                        ಆದರೆ (errno == EINTR)
                                ಮುಂದುವರಿ;

                        ಆದರೆ (errno == EAGAIN)
                                ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;

                        ಹಿಂತಿರುಗಿ -errno;
                }

                ಎಣಿಕೆ_2 += l;
        }
}

ರಚನೆ cmsghdr* cmsg_ಕಂಡುಹಿಡಿ(ರಚನೆ msghdr *mh, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ type, socklen_t length) {
        ರಚನೆ cmsghdr *cmsg;

        ಪ್ರತಿಜ್ಞಾ(mh);

        CMSG_FOREACH(cmsg, mh)
                ಆದರೆ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ಹಿಂತಿರುಗಿ cmsg;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ* cmsg_ಕಂಡುಹಿಡಿ_and_copy_ದತ್ತಾಂಶ(ರಚನೆ msghdr *mh, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *buf, size_t buf_len) {
        ರಚನೆ cmsghdr *cmsg;

        ಪ್ರತಿಜ್ಞಾ(mh);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_ಕಂಡುಹಿಡಿ(mh, level, type, CMSG_LEN(buf_len));
        ಆದರೆ (!cmsg)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ memcpy_ಸುರಕ್ಷಿತ(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ಪ್ರತಿಜ್ಞಾ(sa->sll_family == AF_PACKET);

        size_t mac_len = ಗಾತ್ರ(sa->sll_addr);

        ಆದರೆ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ಆದರೆ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ಹಿಂತಿರುಗಿ offsetof(ರಚನೆ sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ಪ್ರತಿಜ್ಞಾ(sa->sun_family == AF_UNIX);

        ಹಿಂತಿರುಗಿ offsetof(ರಚನೆ sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, ಗಾತ್ರ(sa->sun_path)-1) :
                        strnlen(sa->sun_path, ಗಾತ್ರ(sa->sun_path))+1);
}

size_t sockaddr_len(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *sa) {
        ಪ್ರತಿಜ್ಞಾ(sa);

        ಆಯ್ಕೆ (sa->sa.sa_family) {
        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ sockaddr_in);
        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ sockaddr_in6);
        ಸ್ಥಿತಿ AF_UNIX:
                ಹಿಂತಿರುಗಿ sockaddr_un_len(&sa->un);
        ಸ್ಥಿತಿ AF_PACKET:
                ಹಿಂತಿರುಗಿ sockaddr_ll_len(&sa->ll);
        ಸ್ಥಿತಿ AF_NETLINK:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ sockaddr_nl);
        ಸ್ಥಿತಿ AF_VSOCK:
                ಹಿಂತಿರುಗಿ ಗಾತ್ರ(ರಚನೆ sockaddr_vm);
        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಂಕ socket_ioctl_fd(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ಆದರೆ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ fd;
}

ಪೂರ್ಣಾಂಕ sockaddr_un_unlink(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_un *sa) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, * nul;

        ಪ್ರತಿಜ್ಞಾ(sa);

        ಆದರೆ (sa->sun_family != AF_UNIX)
                ಹಿಂತಿರುಗಿ -EPROTOTYPE;

        ಆದರೆ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ಹಿಂತಿರುಗಿ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, ಗಾತ್ರ(sa->sun_path));
        ಆದರೆ (nul)
                p = sa->sun_path;
        ಇಲ್ಲದಿದ್ದರೆ
                p = memdupa_suffix0(sa->sun_path, ಗಾತ್ರ(sa->sun_path));

        ಆದರೆ (unlink(p) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ sockaddr_un_ಗಣ_ಮಾರ್ಗ(ರಚನೆ sockaddr_un *ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
        ಆದರೆ (l < 2)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!ಒಳ_ಗಣ(ಮಾರ್ಗ[0], '/', '@'))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ಆದರೆ (l+1 > ಗಾತ್ರ(ret->sun_path))
                ಹಿಂತಿರುಗಿ ಮಾರ್ಗ[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ರಚನೆ sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ಆದರೆ (ಮಾರ್ಗ[0] == '@') {
                /* Abstract namespace socket */
                ಸ್ಮರಣೆ_ನಕಲು(ret->sun_path + 1, ಮಾರ್ಗ + 1, l); /* copy *with* trailing NUL byte */
                ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) (offsetof(ರಚನೆ sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ಇಲ್ಲದಿದ್ದರೆ {
                ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ[0] == '/');

                /* File system socket */
                ಸ್ಮರಣೆ_ನಕಲು(ret->sun_path, ಮಾರ್ಗ, l + 1); /* copy *with* trailing NUL byte */
                ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) (offsetof(ರಚನೆ sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ಪೂರ್ಣಾಂಕ getsockopt_int(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ optname, ಪೂರ್ಣಾಂಕ *ret) {
        ಪೂರ್ಣಾಂಕ v;
        socklen_t sl = ಗಾತ್ರ(v);

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (getsockopt(fd, level, optname, &v, &sl) < 0)
                ಹಿಂತಿರುಗಿ negative_errno();
        ಆದರೆ (sl != ಗಾತ್ರ(v))
                ಹಿಂತಿರುಗಿ -EIO;

        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ socket_bind_ಗಮ್ಯ_ifname(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Call with NULL to drop binding */

        ಹಿಂತಿರುಗಿ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ಪೂರ್ಣಾಂಕ socket_bind_ಗಮ್ಯ_ifindex(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ifindex) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (ifindex <= 0)
                /* Drop binding */
                ಹಿಂತಿರುಗಿ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ಖಾಲಿ, 0));

        ಹಿಂತಿರುಗಿ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ಪೂರ್ಣಾಂಕ socket_autobind(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret_name) {
        _cleanup_free_ ಅಕ್ಷರ *name = ಖಾಲಿ;
        uint64_t random;
        ಪೂರ್ಣಾಂಕ r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        random = random_u64();

        ಆದರೆ (asprintf(&name, "@%" PRIu64, random) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಒಕ್ಕೂಟ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < ಗಾತ್ರ(sa.un.sun_path));

        r = sockaddr_un_ಗಣ_ಮಾರ್ಗ(&sa.un, name);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (bind(fd, &sa.sa, r) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_name)
                *ret_name = TAKE_PTR(name);
        ಹಿಂತಿರುಗಿ 0;
}

ssize_t recvmsg_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ sockfd, ರಚನೆ msghdr *msg, ಪೂರ್ಣಾಂಕ flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ಪ್ರತಿಜ್ಞಾ(sockfd >= 0);
        ಪ್ರತಿಜ್ಞಾ(msg);

        n = recvmsg(sockfd, msg, flags);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (FLAGS_ಗಣ(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_ಗಣ(flags, MSG_PEEK) && FLAGS_ಗಣ(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ socket_get_family(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ af;
        socklen_t sl = ಗಾತ್ರ(af);

        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (sl != ಗಾತ್ರ(af))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ af;
}

ಪೂರ್ಣಾಂಕ socket_ಗಣ_recvpktinfo(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {

        ಆದರೆ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಆದರೆ (af < 0)
                        ಹಿಂತಿರುಗಿ af;
        }

        ಆಯ್ಕೆ (af) {

        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        ಸ್ಥಿತಿ AF_NETLINK:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        ಸ್ಥಿತಿ AF_PACKET:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ socket_ಗಣ_unicast_if(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, ಪೂರ್ಣಾಂಕ ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ಆದರೆ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಆದರೆ (af < 0)
                        ಹಿಂತಿರುಗಿ af;
        }

        ಆಯ್ಕೆ (af) {

        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, ಗಾತ್ರ(ifindex_be)));

        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, ಗಾತ್ರ(ifindex_be)));

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ socket_ಗಣ_option(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, ಪೂರ್ಣಾಂಕ opt_ipv4, ಪೂರ್ಣಾಂಕ opt_ipv6, ಪೂರ್ಣಾಂಕ val) {
        ಆದರೆ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಆದರೆ (af < 0)
                        ಹಿಂತಿರುಗಿ af;
        }

        ಆಯ್ಕೆ (af) {

        ಸ್ಥಿತಿ AF_INET:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        ಸ್ಥಿತಿ AF_INET6:
                ಹಿಂತಿರುಗಿ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ socket_get_mtu(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, size_t *ret) {
        ಪೂರ್ಣಾಂಕ mtu, r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ಆದರೆ (af < 0)
                        ಹಿಂತಿರುಗಿ af;
        }

        ಆಯ್ಕೆ (af) {

        ಸ್ಥಿತಿ AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;
        }

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (mtu <= 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = (size_t) mtu;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ connect_unix_ಮಾರ್ಗ_simple(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಒಕ್ಕೂಟ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        l = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(l > 0);
        ಪ್ರತಿಜ್ಞಾ(l < ಗಾತ್ರ(sa.un.sun_path));

        ಸ್ಮರಣೆ_ನಕಲು(sa.un.sun_path, ಮಾರ್ಗ, l + 1);
        ಹಿಂತಿರುಗಿ RET_NERRNO(connect(fd, &sa.sa, offsetof(ರಚನೆ sockaddr_un, sun_path) + l + 1));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ connect_unix_inode(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ inode_fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(inode_fd >= 0);

        ಹಿಂತಿರುಗಿ connect_unix_ಮಾರ್ಗ_simple(fd, FORMAT_PROC_FD_ಮಾರ್ಗ(inode_fd));
}

ಪೂರ್ಣಾಂಕ connect_unix_ಮಾರ್ಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ inode_fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ಆದರೆ (!ಮಾರ್ಗ) {
                ಆದರೆ (dir_fd < 0)
                        ಹಿಂತಿರುಗಿ -EISDIR;

                ಹಿಂತಿರುಗಿ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Shortcut for the simple case */
        ಆದರೆ (dir_fd == AT_FDCWD && ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ) < sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ sockaddr_un, sun_path))
                ಹಿಂತಿರುಗಿ connect_unix_ಮಾರ್ಗ_simple(fd, ಮಾರ್ಗ);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ಆದರೆ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ ಅಕ್ಷರ *j = strjoin("/", ಮಾರ್ಗ);
                ಆದರೆ (!j)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                inode_fd = open(j, O_ಮಾರ್ಗ|O_CLOEXEC);
        } ಇಲ್ಲದಿದ್ದರೆ
                inode_fd = openat(dir_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC);
        ಆದರೆ (inode_fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ connect_unix_inode(fd, inode_fd);
}

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_parse_unix(Socket_ವಿಳಾಸ *ret_ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ರಚನೆ sockaddr_un un;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_ವಿಳಾಸ);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (!ಒಳ_ಗಣ(*s, '/', '@'))
                ಹಿಂತಿರುಗಿ -EPROTO;

        r = sockaddr_un_ಗಣ_ಮಾರ್ಗ(&un, s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret_ವಿಳಾಸ = (Socket_ವಿಳಾಸ) {
                .sockaddr.un = un,
                .ಗಾತ್ರ_2 = r,
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_equal_unix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        Socket_ವಿಳಾಸ socket_a, socket_b;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = socket_ವಿಳಾಸ_parse_unix(&socket_a, a);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = socket_ವಿಳಾಸ_parse_unix(&socket_b, b);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ಪೂರ್ಣಾಂಕ vsock_parse_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಚಿಹ್ನೆರಹಿತ u;
        r = ಸುರಕ್ಷಿತ_atou(s, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ಆದರೆ (u == VMADDR_PORT_ANY)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ vsock_parse_cid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ಆದರೆ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STR_ಒಳ_ಗಣ(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou(s, ret);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_parse_vsock(Socket_ವಿಳಾಸ *ret_ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *cid_ಆರಂಭ;
        ಚಿಹ್ನೆರಹಿತ port, cid;
        ಪೂರ್ಣಾಂಕ type, r;

        ಪ್ರತಿಜ್ಞಾ(ret_ವಿಳಾಸ);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ ((cid_ಆರಂಭ = startswith(s, "vsock:")))
                type = 0;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cid_ಆರಂಭ = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cid_ಆರಂಭ = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cid_ಆರಂಭ = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -EPROTO;

        e = strchr(cid_ಆರಂಭ, ':');
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = vsock_parse_ಪೋರ್ಟ್(e+1, &port);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        n = strndup(cid_ಆರಂಭ, e - cid_ಆರಂಭ);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (isempty(n))
                cid = VMADDR_CID_ANY;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = vsock_parse_cid(n, &cid);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret_ವಿಳಾಸ = (Socket_ವಿಳಾಸ) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ sockaddr_vm),
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ netlink_socket_get_multicast_groups(ಪೂರ್ಣಾಂಕ fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ಖಾಲಿ;
        socklen_t len = 0, old_len;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ಖಾಲಿ, &len) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (len == 0)
                ಹೋಗು finalize;

        groups = new0(uint32_t, len);
        ಆದರೆ (!groups)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        old_len = len;

        ಆದರೆ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (old_len != len)
                ಹಿಂತಿರುಗಿ -EIO;

finalize:
        ಆದರೆ (ret_len)
                *ret_len = len;
        ಆದರೆ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ socket_get_cookie(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = ಗಾತ್ರ(cookie);
        ಆದರೆ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪ್ರತಿಜ್ಞಾ(cookie_len == ಗಾತ್ರ(cookie));
        ಆದರೆ (ret)
                *ret = cookie;

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ cmsg_close_all(ರಚನೆ msghdr *mh) {
        ಪ್ರತಿಜ್ಞಾ(mh);

        ರಚನೆ cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ಆದರೆ (cmsg->cmsg_level != SOL_SOCKET)
                        ಮುಂದುವರಿ;

                ಆದರೆ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_ದತ್ತಾಂಶ(cmsg, ಪೂರ್ಣಾಂಕ),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cmsg->cmsg_type == SCM_PIDFD) {
                        ಪ್ರತಿಜ್ಞಾ(cmsg->cmsg_len == CMSG_LEN(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)));
                        ಸುರಕ್ಷಿತ_close(*CMSG_TYPED_ದತ್ತಾಂಶ(cmsg, ಪೂರ್ಣಾಂಕ));
                }
        }
}

ಪೂರ್ಣಾಂಕ tos_ಗಮ್ಯ_ಆದ್ಯತೆ(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        ಆಯ್ಕೆ (IPTOS_PREC(tos)) {
        ಸ್ಥಿತಿ IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        ಸ್ಥಿತಿ IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ಹಿಂತಿರುಗಿ TC_PRIO_CONTROL;

        ಸ್ಥಿತಿ IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        ಸ್ಥಿತಿ IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ಹಿಂತಿರುಗಿ TC_PRIO_INTERACTIVE;

        ಸ್ಥಿತಿ IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        ಸ್ಥಿತಿ IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ಹಿಂತಿರುಗಿ TC_PRIO_INTERACTIVE_BULK;

        ಸ್ಥಿತಿ IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ಹಿಂತಿರುಗಿ TC_PRIO_BULK;

        ಸ್ಥಿತಿ IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ TC_PRIO_BESTEFFORT;
        }
}

ಪೂರ್ಣಾಂಕ socket_xattr_supported(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached = -1;
        ಆದರೆ (cached >= 0)
                ಹಿಂತಿರುಗಿ cached;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, "user.testxxx", "1");
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ಹಿಂತಿರುಗಿ (cached = false);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ಹಿಂತಿರುಗಿ (cached = true);
}
