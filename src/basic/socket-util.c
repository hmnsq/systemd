/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/if.h>
#ଅନ୍ତର୍ଭାଵ <linux/if_arp.h>
#ଅନ୍ତର୍ଭାଵ <linux/if_ether.h>
#ଅନ୍ତର୍ଭାଵ <linux/if_infiniband.h>
#ଅନ୍ତର୍ଭାଵ <linux/pkt_sched.h>
#ଅନ୍ତର୍ଭାଵ <mqueue.h>
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#ଅନ୍ତର୍ଭାଵ <netinet/ip.h>
#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "format-ifname.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "in-addr-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "sparse-endian.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "sysctl-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"
#ଅନ୍ତର୍ଭାଵ "xattr-util.h"

#ଯଦି ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#ଅନ୍ୟଥା
#  define IDN_FLAGS 0
#endif

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_verify(ଅଚଳ SocketAddress *a, bool strict) {
        ପ୍ରତିଜ୍ଞା(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        ବାଛ (socket_address_family(a)) {

        ସ୍ଥିତି AF_INET:
                ଯଦି (a->size != ଆକାର(ସଂରଚନା sockaddr_in))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (a->sockaddr.in.sin_port == 0)
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_INET6:
                ଯଦି (a->size != ଆକାର(ସଂରଚନା sockaddr_in6))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (a->sockaddr.in6.sin6_port == 0)
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_UNIX:
                ଯଦି (a->size < offsetof(ସଂରଚନା sockaddr_un, sun_path))
                        ଫେରନ୍ତୁ -EINVAL;
                ଯଦି (a->size > ଆକାର(ସଂରଚନା sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (a->size > offsetof(ସଂରଚନା sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ଅଚଳ ଅକ୍ଷର *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, ଆକାର(a->sockaddr.un.sun_path));
                        ଯଦି (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ଯଦି (a->size != offsetof(ସଂରଚନା sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ଫେରନ୍ତୁ -EINVAL;
                        } ଅନ୍ୟଥା {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ଯଦି (!IN_SET(a->size, ଆକାର(a->sockaddr.un.sun_path), ଆକାର(a->sockaddr.un.sun_path)+1))
                                        ଫେରନ୍ତୁ -EINVAL;
                        }
                }

                ଯଦି (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_NETLINK:

                ଯଦି (a->size != ଆକାର(ସଂରଚନା sockaddr_nl))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_VSOCK:
                ଯଦି (a->size != ଆକାର(ସଂରଚନା sockaddr_vm))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ 0;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_print(ଅଚଳ SocketAddress *a, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ ଅକ୍ଷର *sfamily = ଖାଲି;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ଅଚଳ SocketAddress *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଫେରନ୍ତୁ
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ଅଚଳ SocketAddress *a, ଅଚଳ SocketAddress *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        /* Invalid addresses are unequal to all */
        ଯଦି (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (a->type != b->type)
                ଫେରନ୍ତୁ false;

        ଯଦି (socket_address_family(a) != socket_address_family(b))
                ଫେରନ୍ତୁ false;

        ବାଛ (socket_address_family(a)) {

        ସ୍ଥିତି AF_INET:
                ଯଦି (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ଫେରନ୍ତୁ false;

                ଯଦି (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ଫେରନ୍ତୁ false;

                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_INET6:
                ଯଦି (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, ଆକାର(a->sockaddr.in6.sin6_addr)) != 0)
                        ଫେରନ୍ତୁ false;

                ଯଦି (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ଫେରନ୍ତୁ false;

                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_UNIX:
                ଯଦି (a->size <= offsetof(ସଂରଚନା sockaddr_un, sun_path) ||
                    b->size <= offsetof(ସଂରଚନା sockaddr_un, sun_path))
                        ଫେରନ୍ତୁ false;

                ଯଦି ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ଫେରନ୍ତୁ false;

                ଯଦି (a->sockaddr.un.sun_path[0]) {
                        ଯଦି (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ଫେରନ୍ତୁ false;
                } ଅନ୍ୟଥା {
                        ଯଦି (a->size != b->size)
                                ଫେରନ୍ତୁ false;

                        ଯଦି (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                ଫେରନ୍ତୁ false;
                }

                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_NETLINK:
                ଯଦି (a->protocol != b->protocol)
                        ଫେରନ୍ତୁ false;

                ଯଦି (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ଫେରନ୍ତୁ false;

                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_VSOCK:
                ଯଦି (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ଫେରନ୍ତୁ false;

                ଯଦି (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ଫେରନ୍ତୁ false;

                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                /* Cannot compare, so we assume the addresses are different */
                ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

ଅଚଳ ଅକ୍ଷର* socket_address_get_path(ଅଚଳ SocketAddress *a) {
        ପ୍ରତିଜ୍ଞା(a);

        ଯଦି (socket_address_family(a) != AF_UNIX)
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (a->sockaddr.un.sun_path[0] == 0)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(ଆକାର(ସଂଘ sockaddr_union) >= ଆକାର(ସଂରଚନା sockaddr_un)+1);
        ଫେରନ୍ତୁ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(ଶୂନ୍ୟ) {
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = -1;

        ଯଦି (cached < 0) {

                ଯଦି (access("/proc/net/if_inet6", F_OK) < 0) {

                        ଯଦି (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ଫେରନ୍ତୁ false;
                        }

                        cached = false;
                } ଅନ୍ୟଥା
                        cached = true;
        }

        ଫେରନ୍ତୁ cached;
}

bool socket_ipv6_is_enabled(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ଯଦି (!socket_ipv6_is_supported())
                ଫେରନ୍ତୁ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ଯଦି (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ଫେରନ୍ତୁ true;
        }

        r = parse_boolean(v);
        ଯଦି (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ !r;
}

bool socket_address_matches_fd(ଅଚଳ SocketAddress *a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        SocketAddress b;
        socklen_t solen;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        b.size = ଆକାର(b.sockaddr);
        ଯଦି (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ଫେରନ୍ତୁ false;

        solen = ଆକାର(b.type);
        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (b.type != a->type)
                ଫେରନ୍ତୁ false;

        ଯଦି (a->protocol != 0)  {
                solen = ଆକାର(b.protocol);
                ଯଦି (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ଫେରନ୍ତୁ false;

                ଯଦି (b.protocol != a->protocol)
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ socket_address_equal(a, &b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_port(ଅଚଳ ସଂରଚନା sockaddr *_sa, unsigned *ret_port) {
        ଅଚଳ ସଂଘ sockaddr_union *sa = (ଅଚଳ ସଂଘ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ପ୍ରତିଜ୍ଞା(sa);
        ପ୍ରତିଜ୍ଞା(ret_port);

        ବାଛ (sa->sa.sa_family) {

        ସ୍ଥିତି AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ଫେରନ୍ତୁ 0;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ଅଚଳ ସଂଘ in_addr_union *sockaddr_in_addr(ଅଚଳ ସଂରଚନା sockaddr *_sa) {
        ଅଚଳ ସଂଘ sockaddr_union *sa = (ଅଚଳ ସଂଘ sockaddr_union*) _sa;

        ଯଦି (!sa)
                ଫେରନ୍ତୁ ଖାଲି;

        ବାଛ (sa->sa.sa_family) {

        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ (ଅଚଳ ସଂଘ in_addr_union*) &sa->in.sin_addr;

        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ (ଅଚଳ ସଂଘ in_addr_union*) &sa->in6.sin6_addr;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ ଖାଲି;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_set_in_addr(
                ସଂଘ sockaddr_union *u,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା family,
                ଅଚଳ ସଂଘ in_addr_union *a,
                uint16_t port) {

        ପ୍ରତିଜ୍ଞା(u);
        ପ୍ରତିଜ୍ଞା(a);

        ବାଛ (family) {

        ସ୍ଥିତି AF_INET:
                u->in = (ସଂରଚନା sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                ଫେରନ୍ତୁ 0;

        ସ୍ଥିତି AF_INET6:
                u->in6 = (ସଂରଚନା sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ଫେରନ୍ତୁ 0;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_pretty(
                ଅଚଳ ସଂରଚନା sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                ଅକ୍ଷର **ret) {

        ସଂଘ sockaddr_union *sa = (ସଂଘ sockaddr_union*) _sa;
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(sa);
        ପ୍ରତିଜ୍ଞା(salen >= ଆକାର(sa->sa.sa_family));
        ପ୍ରତିଜ୍ଞା(ret);

        ବାଛ (sa->sa.sa_family) {

        ସ୍ଥିତି AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                ଯଦି (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                ଅନ୍ୟଥା
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOMEM;
                ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି AF_INET6: {
                ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ଯଦି (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, ଆକାର(ipv4_prefix)) == 0) {
                        ଅଚଳ uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ଯଦି (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        ଅନ୍ୟଥା
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ -ENOMEM;
                } ଅନ୍ୟଥା {
                        ଅଚଳ ଅକ୍ଷର *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        ଯଦି (include_port) {
                                ଯଦି (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ଫେରନ୍ତୁ -ENOMEM;
                        } ଅନ୍ୟଥା {
                                ଯଦି (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                ଅନ୍ୟଥା
                                        p = strdup(a);
                                ଯଦି (!p)
                                        ଫେରନ୍ତୁ -ENOMEM;
                        }
                }

                ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି AF_UNIX:
                ଯଦି (salen <= offsetof(ସଂରଚନା sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ସଂରଚନା sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                ଅନ୍ୟଥା {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        ଅକ୍ଷର *path = (ଅକ୍ଷର*) sa->un_buffer + offsetof(ସଂରଚନା sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(ସଂରଚନା sockaddr_un, sun_path);

                        ଯଦି (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ ଅକ୍ଷର *e = ଖାଲି;

                                e = cescape_length(path + 1, path_len - 1);
                                ଯଦି (!e)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                p = strjoin("@", e);
                        } ଅନ୍ୟଥା {
                                ଯଦି (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_VSOCK:
                ଯଦି (include_port) {
                        ଯଦି (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        ଅନ୍ୟଥା
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } ଅନ୍ୟଥା
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOMEM;
                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }

        *ret = p;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeername_pretty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool include_port, ଅକ୍ଷର **ret) {
        ସଂଘ sockaddr_union sa;
        socklen_t salen = ଆକାର(sa);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (getpeername(fd, &sa.sa, &salen) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (sa.sa.sa_family == AF_UNIX) {
                ସଂରଚନା ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଫେରନ୍ତୁ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ଫେରନ୍ତୁ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getsockname_pretty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        ସଂଘ sockaddr_union sa;
        socklen_t salen = ଆକାର(sa);

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (getsockname(fd, &sa.sa, &salen) < 0)
                ଫେରନ୍ତୁ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ଫେରନ୍ତୁ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socknameinfo_pretty(ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t salen, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର host[NI_MAXHOST];
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(sa);
        ପ୍ରତିଜ୍ଞା(salen >= ଆକାର(sa_family_t));
        ପ୍ରତିଜ୍ଞା(ret);

        r = getnameinfo(sa, salen, host, ଆକାର(host), /* service= */ ଖାଲି, /* service_len= */ 0, IDN_FLAGS);
        ଯଦି (r != 0) {
                ଯଦି (r == EAI_MEMORY)
                        ଫେରନ୍ତୁ log_oom_debug();
                ଯଦି (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                ଅନ୍ୟଥା
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ଫେରନ୍ତୁ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ଫେରନ୍ତୁ strdup_to(ret, host);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, INT_MAX);

bool sockaddr_equal(ଅଚଳ ସଂଘ sockaddr_union *a, ଅଚଳ ସଂଘ sockaddr_union *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଯଦି (a->sa.sa_family != b->sa.sa_family)
                ଫେରନ୍ତୁ false;

        ଯଦି (a->sa.sa_family == AF_INET)
                ଫେରନ୍ତୁ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        ଯଦି (a->sa.sa_family == AF_INET6)
                ଫେରନ୍ତୁ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, ଆକାର(a->in6.sin6_addr)) == 0;

        ଯଦି (a->sa.sa_family == AF_VSOCK)
                ଫେରନ୍ତୁ a->vm.svm_cid == b->vm.svm_cid;

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_set_sndbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n, bool increase) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ମୂଲ୍ୟ;
        socklen_t l = ଆକାର(ମୂଲ୍ୟ);

        ଯଦି (n > INT_MAX)
                ଫେରନ୍ତୁ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ମୂଲ୍ୟ, &l);
        ଯଦି (r >= 0 && l == ଆକାର(ମୂଲ୍ୟ) && increase ? (size_t) ମୂଲ୍ୟ >= n*2 : (size_t) ମୂଲ୍ୟ == n*2)
                ଫେରନ୍ତୁ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ଆକାର(ମୂଲ୍ୟ);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &ମୂଲ୍ୟ, &l);
        ଯଦି (r >= 0 && l == ଆକାର(ମୂଲ୍ୟ) && increase ? (size_t) ମୂଲ୍ୟ >= n*2 : (size_t) ମୂଲ୍ୟ == n*2)
                ଫେରନ୍ତୁ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_set_rcvbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n, bool increase) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ମୂଲ୍ୟ;
        socklen_t l = ଆକାର(ମୂଲ୍ୟ);

        ଯଦି (n > INT_MAX)
                ଫେରନ୍ତୁ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ମୂଲ୍ୟ, &l);
        ଯଦି (r >= 0 && l == ଆକାର(ମୂଲ୍ୟ) && increase ? (size_t) ମୂଲ୍ୟ >= n*2 : (size_t) ମୂଲ୍ୟ == n*2)
                ଫେରନ୍ତୁ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = ଆକାର(ମୂଲ୍ୟ);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &ମୂଲ୍ୟ, &l);
        ଯଦି (r >= 0 && l == ଆକାର(ମୂଲ୍ୟ) && increase ? (size_t) ମୂଲ୍ୟ >= n*2 : (size_t) ମୂଲ୍ୟ == n*2)
                ଫେରନ୍ତୁ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, 0xff);

bool ifname_valid_char(ଅକ୍ଷର a) {
        ଯଦି ((unsigned ଅକ୍ଷର) a >= 127U)
                ଫେରନ୍ତୁ false;

        ଯଦି ((unsigned ଅକ୍ଷର) a <= 32U)
                ଫେରନ୍ତୁ false;

        ଯଦି (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool ifname_valid_full(ଅଚଳ ଅକ୍ଷର *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ପ୍ରତିଜ୍ଞା(!(flags & ~_IFNAME_VALID_ALL));

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ଯଦି (parse_ifindex(p) >= 0)
                ଫେରନ୍ତୁ flags & IFNAME_VALID_NUMERIC;

        ଯଦି (flags & IFNAME_VALID_ALTERNATIVE) {
                ଯଦି (strlen(p) >= ALTIFNAMSIZ)
                        ଫେରନ୍ତୁ false;
        } ଅନ୍ୟଥା {
                ଯଦି (strlen(p) >= IFNAMSIZ)
                        ଫେରନ୍ତୁ false;
        }

        ଯଦି (dot_or_dot_dot(p))
                ଫେରନ୍ତୁ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ଯଦି (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ଫେରନ୍ତୁ false;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *t = p; *t; t++) {
                ଯଦି (!ifname_valid_char(*t))
                        ଫେରନ୍ତୁ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ଯଦି (numeric)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool address_label_valid(ଅଚଳ ଅକ୍ଷର *p) {

        POINTER_MAY_BE_NULL(p);

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (strlen(p) >= IFNAMSIZ)
                ଫେରନ୍ତୁ false;

        ଯେତେବେଳେ (*p) {
                ଯଦି ((uint8_t) *p >= 127U)
                        ଫେରନ୍ତୁ false;

                ଯଦି ((uint8_t) *p <= 31U)
                        ଫେରନ୍ତୁ false;
                p++;
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeercred(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା ucred *ucred) {
        socklen_t n = ଆକାର(ସଂରଚନା ucred);
        ସଂରଚନା ucred u;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ucred);

        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (n != ଆକାର(ସଂରଚନା ucred))
                ଫେରନ୍ତୁ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ଯଦି (!pid_is_valid(u.pid))
                ଫେରନ୍ତୁ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeersec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        socklen_t n = 64;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (;;) {
                s = new0(ଅକ୍ଷର, n+1);
                ଯଦି (!s)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        ଭାଙ୍ଗ;
                }

                ଯଦି (errno != ERANGE)
                        ଫେରନ୍ତୁ -errno;

                s = mfree(s);
        }

        ଯଦି (isempty(s))
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeergroups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, gid_t **ret) {
        socklen_t n = ଆକାର(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngroups_max = sysconf_ngroups_max();
        ଯଦି (ngroups_max > 0)
                n = MAX(n, ଆକାର(gid_t) * (socklen_t) ngroups_max);

        ପାଇଁ (;;) {
                d = malloc(n);
                ଯଦି (!d)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        ଭାଙ୍ଗ;

                ଯଦି (errno != ERANGE)
                        ଫେରନ୍ତୁ -errno;

                d = mfree(d);
        }

        assert_se(n % ଆକାର(gid_t) == 0);
        n /= ଆକାର(gid_t);

        ଯଦି (n > INT_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        *ret = TAKE_PTR(d);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeerpidfd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        socklen_t n = ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd = -EBADF;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (n != ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
                ଫେରନ୍ତୁ -EIO;

        ଫେରନ୍ତୁ pidfd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeerpidref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, PidRef *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd = getpeerpidfd(fd);
        ଯଦି (pidfd < 0) {
                ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ଫେରନ୍ତୁ pidfd;

                ସଂରଚନା ucred ucred;
                r = getpeercred(fd, &ucred);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ pidref_set_pid(ret, ucred.pid);
        }

        ଫେରନ୍ତୁ pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ସଂରଚନା iovec *iov, size_t iovlen,
                ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t len,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))) control = {};
        ସଂରଚନା msghdr mh = {
                .msg_name = (ସଂରଚନା sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ସଂରଚନା iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ପ୍ରତିଜ୍ଞା(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ଯଦି (fd < 0 && !iov)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (fd >= 0) {
                ସଂରଚନା cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = ଆକାର(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
                memcpy(CMSG_DATA(cmsg), &fd, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ଯଦି (k < 0)
                ଫେରନ୍ତୁ (ssize_t) -errno;

        ଫେରନ୍ତୁ k;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା send_one_fd_sa(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t len,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) send_one_fd_iov_sa(transport_fd, fd, ଖାଲି, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd,
                ସଂରଚନା iovec *iov, size_t iovlen,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))) control;
        ସଂରଚନା msghdr mh = {
                .msg_control = &control,
                .msg_controllen = ଆକାର(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ସଂରଚନା cmsghdr *found;
        ssize_t k;

        ପ୍ରତିଜ୍ଞା(transport_fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ଯଦି (k < 0)
                ଫେରନ୍ତୁ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)));
        ଯଦି (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ଯଦି (k == 0)
                        ଫେରନ୍ତୁ -EIO;
        }

        ଯଦି (found)
                *ret_fd = *CMSG_TYPED_DATA(found, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
        ଅନ୍ୟଥା
                *ret_fd = -EBADF;

        ଫେରନ୍ତୁ k;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା receive_one_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ଖାଲି, 0, flags, &fd);
        ଯଦି (k == 0)
                ଫେରନ୍ତୁ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ପ୍ରତିଜ୍ଞା(k < 0);
        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) k;
}

ssize_t next_datagram_size_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ssize_t l;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ଖାଲି, 0, MSG_PEEK|MSG_TRUNC);
        ଯଦି (l < 0) {
                ଯଦି (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        ଯାଅ fallback;

                ଫେରନ୍ତୁ -errno;
        }
        ଯଦି (l == 0)
                ଯାଅ fallback;

        ଫେରନ୍ତୁ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ଯଦି (ioctl(fd, FIONREAD, &k) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flush_accept(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, b;
        socklen_t l = ଆକାର(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ଫେରନ୍ତୁ -errno;

        ପ୍ରତିଜ୍ଞା(l == ଆକାର(b));
        ଯଦି (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ଫେରନ୍ତୁ -ENOTTY;

        ପାଇଁ (unsigned iteration = 0;; iteration++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ଯଦି (r == -EINTR)
                        ଜାରି;
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (iteration >= MAX_FLUSH_ITERATIONS)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ଖାଲି, ଖାଲି, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ଯଦି (cfd < 0) {
                        ଯଦି (errno == EAGAIN)
                                ଫେରନ୍ତୁ 0;

                        ଯଦି (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ଜାରି;

                        ଫେରନ୍ତୁ -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        ସଂରଚନା mq_attr attr;
        ssize_t ଗଣନା_2 = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        ପାଇଁ (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ଯଦି (r == -EINTR)
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ ଗଣନା_2;

                ଯଦି (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ଯଦି (mq_getattr(fd, &attr) < 0)
                                ଫେରନ୍ତୁ -errno;

                        buf = malloc(attr.mq_msgsize);
                        ଯଦି (!buf)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ଖାଲି);
                ଯଦି (l < 0) {
                        ଯଦି (errno == EINTR)
                                ଜାରି;

                        ଯଦି (errno == EAGAIN)
                                ଫେରନ୍ତୁ ଗଣନା_2;

                        ଫେରନ୍ତୁ -errno;
                }

                ଗଣନା_2 += l;
        }
}

ସଂରଚନା cmsghdr* cmsg_find(ସଂରଚନା msghdr *mh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, socklen_t length) {
        ସଂରଚନା cmsghdr *cmsg;

        ପ୍ରତିଜ୍ଞା(mh);

        CMSG_FOREACH(cmsg, mh)
                ଯଦି (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ଫେରନ୍ତୁ cmsg;

        ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ* cmsg_find_and_copy_data(ସଂରଚନା msghdr *mh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *buf, size_t buf_len) {
        ସଂରଚନା cmsghdr *cmsg;

        ପ୍ରତିଜ୍ଞା(mh);
        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        ଯଦି (!cmsg)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ଅଚଳ ସଂରଚନା sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ପ୍ରତିଜ୍ଞା(sa->sll_family == AF_PACKET);

        size_t mac_len = ଆକାର(sa->sll_addr);

        ଯଦି (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ଯଦି (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ଫେରନ୍ତୁ offsetof(ସଂରଚନା sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ଅଚଳ ସଂରଚନା sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ପ୍ରତିଜ୍ଞା(sa->sun_family == AF_UNIX);

        ଫେରନ୍ତୁ offsetof(ସଂରଚନା sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, ଆକାର(sa->sun_path)-1) :
                        strnlen(sa->sun_path, ଆକାର(sa->sun_path))+1);
}

size_t sockaddr_len(ଅଚଳ ସଂଘ sockaddr_union *sa) {
        ପ୍ରତିଜ୍ଞା(sa);

        ବାଛ (sa->sa.sa_family) {
        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା sockaddr_in);
        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା sockaddr_in6);
        ସ୍ଥିତି AF_UNIX:
                ଫେରନ୍ତୁ sockaddr_un_len(&sa->un);
        ସ୍ଥିତି AF_PACKET:
                ଫେରନ୍ତୁ sockaddr_ll_len(&sa->ll);
        ସ୍ଥିତି AF_NETLINK:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା sockaddr_nl);
        ସ୍ଥିତି AF_VSOCK:
                ଫେରନ୍ତୁ ଆକାର(ସଂରଚନା sockaddr_vm);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_ioctl_fd(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ଯଦି (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_un_unlink(ଅଚଳ ସଂରଚନା sockaddr_un *sa) {
        ଅଚଳ ଅକ୍ଷର *p, * nul;

        ପ୍ରତିଜ୍ଞା(sa);

        ଯଦି (sa->sun_family != AF_UNIX)
                ଫେରନ୍ତୁ -EPROTOTYPE;

        ଯଦି (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ଫେରନ୍ତୁ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, ଆକାର(sa->sun_path));
        ଯଦି (nul)
                p = sa->sun_path;
        ଅନ୍ୟଥା
                p = memdupa_suffix0(sa->sun_path, ଆକାର(sa->sun_path));

        ଯଦି (unlink(p) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_un_set_path(ସଂରଚନା sockaddr_un *ret, ଅଚଳ ଅକ୍ଷର *path) {
        size_t l;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        ଯଦି (l < 2)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!IN_SET(path[0], '/', '@'))
                ଫେରନ୍ତୁ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ଯଦି (l+1 > ଆକାର(ret->sun_path))
                ଫେରନ୍ତୁ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ସଂରଚନା sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ଯଦି (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (offsetof(ସଂରଚନା sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } ଅନ୍ୟଥା {
                ପ୍ରତିଜ୍ଞା(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (offsetof(ସଂରଚନା sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getsockopt_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା optname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;
        socklen_t sl = ଆକାର(v);

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (getsockopt(fd, level, optname, &v, &sl) < 0)
                ଫେରନ୍ତୁ negative_errno();
        ଯଦି (sl != ଆକାର(v))
                ଫେରନ୍ତୁ -EIO;

        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_bind_to_ifname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *ifname) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Call with NULL to drop binding */

        ଫେରନ୍ତୁ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_bind_to_ifindex(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (ifindex <= 0)
                /* Drop binding */
                ଫେରନ୍ତୁ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ଖାଲି, 0));

        ଫେରନ୍ତୁ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_autobind(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret_name) {
        _cleanup_free_ ଅକ୍ଷର *name = ଖାଲି;
        uint64_t random;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        random = random_u64();

        ଯଦି (asprintf(&name, "@%" PRIu64, random) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ସଂଘ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < ଆକାର(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (bind(fd, &sa.sa, r) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_name)
                *ret_name = TAKE_PTR(name);
        ଫେରନ୍ତୁ 0;
}

ssize_t recvmsg_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockfd, ସଂରଚନା msghdr *msg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ପ୍ରତିଜ୍ଞା(sockfd >= 0);
        ପ୍ରତିଜ୍ଞା(msg);

        n = recvmsg(sockfd, msg, flags);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ଫେରନ୍ତୁ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_family(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା af;
        socklen_t sl = ଆକାର(af);

        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (sl != ଆକାର(af))
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ af;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_recvpktinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {

        ଯଦି (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ଯଦି (af < 0)
                        ଫେରନ୍ତୁ af;
        }

        ବାଛ (af) {

        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        ସ୍ଥିତି AF_NETLINK:
                ଫେରନ୍ତୁ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        ସ୍ଥିତି AF_PACKET:
                ଫେରନ୍ତୁ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_unicast_if(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ଯଦି (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ଯଦି (af < 0)
                        ଫେରନ୍ତୁ af;
        }

        ବାଛ (af) {

        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, ଆକାର(ifindex_be)));

        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, ଆକାର(ifindex_be)));

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_ipv4, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_ipv6, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val) {
        ଯଦି (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ଯଦି (af < 0)
                        ଫେରନ୍ତୁ af;
        }

        ବାଛ (af) {

        ସ୍ଥିତି AF_INET:
                ଫେରନ୍ତୁ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        ସ୍ଥିତି AF_INET6:
                ଫେରନ୍ତୁ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_mtu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, size_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mtu, r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ଯଦି (af < 0)
                        ଫେରନ୍ତୁ af;
        }

        ବାଛ (af) {

        ସ୍ଥିତି AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                ଭାଙ୍ଗ;

        ସ୍ଥିତି AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EAFNOSUPPORT;
        }

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (mtu <= 0)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = (size_t) mtu;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect_unix_path_simple(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path) {
        ସଂଘ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(path);

        l = strlen(path);
        ପ୍ରତିଜ୍ଞା(l > 0);
        ପ୍ରତିଜ୍ଞା(l < ଆକାର(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        ଫେରନ୍ତୁ RET_NERRNO(connect(fd, &sa.sa, offsetof(ସଂରଚନା sockaddr_un, sun_path) + l + 1));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect_unix_inode(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_fd) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(inode_fd >= 0);

        ଫେରନ୍ତୁ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect_unix_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ଯଦି (!path) {
                ଯଦି (dir_fd < 0)
                        ଫେରନ୍ତୁ -EISDIR;

                ଫେରନ୍ତୁ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ -EINVAL;

        /* Shortcut for the simple case */
        ଯଦି (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(ସଂରଚନା sockaddr_un, sun_path))
                ଫେରନ୍ତୁ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ଯଦି (dir_fd == XAT_FDROOT) {
                _cleanup_free_ ଅକ୍ଷର *j = strjoin("/", path);
                ଯଦି (!j)
                        ଫେରନ୍ତୁ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } ଅନ୍ୟଥା
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        ଯଦି (inode_fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ connect_unix_inode(fd, inode_fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_parse_unix(SocketAddress *ret_address, ଅଚଳ ଅକ୍ଷର *s) {
        ସଂରଚନା sockaddr_un un;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_address);
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (!IN_SET(*s, '/', '@'))
                ଫେରନ୍ତୁ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_equal_unix(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        SocketAddress socket_a, socket_b;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = socket_address_parse_unix(&socket_a, a);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = socket_address_parse_unix(&socket_b, b);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vsock_parse_port(ଅଚଳ ଅକ୍ଷର *s, unsigned *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!s)
                ଫେରନ୍ତୁ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ଯଦି (u == VMADDR_PORT_ANY)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vsock_parse_cid(ଅଚଳ ଅକ୍ଷର *s, unsigned *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!s)
                ଫେରନ୍ତୁ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ଯଦି (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        ଅନ୍ୟଥା ଯଦି (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        ଅନ୍ୟଥା ଯଦି (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        ଅନ୍ୟଥା ଯଦି (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ safe_atou(s, ret);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_parse_vsock(SocketAddress *ret_address, ଅଚଳ ଅକ୍ଷର *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e, *cid_start;
        unsigned port, cid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, r;

        ପ୍ରତିଜ୍ଞା(ret_address);
        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି ((cid_start = startswith(s, "vsock:")))
                type = 0;
        ଅନ୍ୟଥା ଯଦି ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        ଅନ୍ୟଥା ଯଦି ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        ଅନ୍ୟଥା ଯଦି ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -EPROTO;

        e = strchr(cid_start, ':');
        ଯଦି (!e)
                ଫେରନ୍ତୁ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        n = strndup(cid_start, e - cid_start);
        ଯଦି (!n)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (isempty(n))
                cid = VMADDR_CID_ANY;
        ଅନ୍ୟଥା {
                r = vsock_parse_cid(n, &cid);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = ଆକାର(ସଂରଚନା sockaddr_vm),
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା netlink_socket_get_multicast_groups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ଖାଲି;
        socklen_t len = 0, old_len;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ଖାଲି, &len) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (len == 0)
                ଯାଅ finalize;

        groups = new0(uint32_t, len);
        ଯଦି (!groups)
                ଫେରନ୍ତୁ -ENOMEM;

        old_len = len;

        ଯଦି (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (old_len != len)
                ଫେରନ୍ତୁ -EIO;

finalize:
        ଯଦି (ret_len)
                *ret_len = len;
        ଯଦି (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_cookie(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = ଆକାର(cookie);
        ଯଦି (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ଫେରନ୍ତୁ -errno;

        ପ୍ରତିଜ୍ଞା(cookie_len == ଆକାର(cookie));
        ଯଦି (ret)
                *ret = cookie;

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ cmsg_close_all(ସଂରଚନା msghdr *mh) {
        ପ୍ରତିଜ୍ଞା(mh);

        ସଂରଚନା cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ଯଦି (cmsg->cmsg_level != SOL_SOCKET)
                        ଜାରି;

                ଯଦି (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
                ଅନ୍ୟଥା ଯଦି (cmsg->cmsg_type == SCM_PIDFD) {
                        ପ୍ରତିଜ୍ଞା(cmsg->cmsg_len == CMSG_LEN(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
                }
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        ବାଛ (IPTOS_PREC(tos)) {
        ସ୍ଥିତି IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        ସ୍ଥିତି IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ଫେରନ୍ତୁ TC_PRIO_CONTROL;

        ସ୍ଥିତି IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        ସ୍ଥିତି IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ଫେରନ୍ତୁ TC_PRIO_INTERACTIVE;

        ସ୍ଥିତି IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        ସ୍ଥିତି IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ଫେରନ୍ତୁ TC_PRIO_INTERACTIVE_BULK;

        ସ୍ଥିତି IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ଫେରନ୍ତୁ TC_PRIO_BULK;

        ସ୍ଥିତି IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ TC_PRIO_BESTEFFORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_xattr_supported(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = -1;
        ଯଦି (cached >= 0)
                ଫେରନ୍ତୁ cached;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ଖାଲି, AT_EMPTY_PATH, "user.testxxx", "1");
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ଫେରନ୍ତୁ (cached = false);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ଫେରନ୍ତୁ (cached = true);
}
