/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/if.h>
#അന്തര്ഭാവ <linux/if_arp.h>
#അന്തര്ഭാവ <linux/if_ether.h>
#അന്തര്ഭാവ <linux/if_infiniband.h>
#അന്തര്ഭാവ <linux/pkt_sched.h>
#അന്തര്ഭാവ <mqueue.h>
#അന്തര്ഭാവ <net/if.h>
#അന്തര്ഭാവ <netdb.h>
#അന്തര്ഭാവ <netinet/ip.h>
#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "format-ifname.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "in-addr-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "sparse-endian.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "sysctl-util.h"
#അന്തര്ഭാവ "user-util.h"
#അന്തര്ഭാവ "xattr-util.h"

#എങ്കിൽ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#അല്ലെങ്കിൽ
#  define IDN_FLAGS 0
#endif

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, പൂർണ്ണസംഖ്യ);

പൂർണ്ണസംഖ്യ socket_address_verify(സ്ഥിരം SocketAddress *a, bool strict) {
        പ്രതിജ്ഞാ(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        തിരഞ്ഞെടുപ്പ് (socket_address_family(a)) {

        അവസ്ഥ AF_INET:
                എങ്കിൽ (a->size != വലുപ്പം(ഘടന sockaddr_in))
                        തിരികെ -EINVAL;

                എങ്കിൽ (a->sockaddr.in.sin_port == 0)
                        തിരികെ -EINVAL;

                എങ്കിൽ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        തിരികെ -EINVAL;

                തിരികെ 0;

        അവസ്ഥ AF_INET6:
                എങ്കിൽ (a->size != വലുപ്പം(ഘടന sockaddr_in6))
                        തിരികെ -EINVAL;

                എങ്കിൽ (a->sockaddr.in6.sin6_port == 0)
                        തിരികെ -EINVAL;

                എങ്കിൽ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        തിരികെ -EINVAL;

                തിരികെ 0;

        അവസ്ഥ AF_UNIX:
                എങ്കിൽ (a->size < offsetof(ഘടന sockaddr_un, sun_path))
                        തിരികെ -EINVAL;
                എങ്കിൽ (a->size > വലുപ്പം(ഘടന sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        തിരികെ -EINVAL;

                എങ്കിൽ (a->size > offsetof(ഘടന sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        സ്ഥിരം അക്ഷരം *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, വലുപ്പം(a->sockaddr.un.sun_path));
                        എങ്കിൽ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                എങ്കിൽ (a->size != offsetof(ഘടന sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        തിരികെ -EINVAL;
                        } അല്ലെങ്കിൽ {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                എങ്കിൽ (!IN_SET(a->size, വലുപ്പം(a->sockaddr.un.sun_path), വലുപ്പം(a->sockaddr.un.sun_path)+1))
                                        തിരികെ -EINVAL;
                        }
                }

                എങ്കിൽ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        തിരികെ -EINVAL;

                തിരികെ 0;

        അവസ്ഥ AF_NETLINK:

                എങ്കിൽ (a->size != വലുപ്പം(ഘടന sockaddr_nl))
                        തിരികെ -EINVAL;

                എങ്കിൽ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        തിരികെ -EINVAL;

                തിരികെ 0;

        അവസ്ഥ AF_VSOCK:
                എങ്കിൽ (a->size != വലുപ്പം(ഘടന sockaddr_vm))
                        തിരികെ -EINVAL;

                എങ്കിൽ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        തിരികെ -EINVAL;

                തിരികെ 0;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ socket_address_print(സ്ഥിരം SocketAddress *a, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ അക്ഷരം *sfamily = ശൂന്യ;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOMEM;

                തിരികെ 0;
        }

        തിരികെ sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(സ്ഥിരം SocketAddress *a) {
        പ്രതിജ്ഞാ(a);

        തിരികെ
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(സ്ഥിരം SocketAddress *a, സ്ഥിരം SocketAddress *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        /* Invalid addresses are unequal to all */
        എങ്കിൽ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                തിരികെ false;

        എങ്കിൽ (a->type != b->type)
                തിരികെ false;

        എങ്കിൽ (socket_address_family(a) != socket_address_family(b))
                തിരികെ false;

        തിരഞ്ഞെടുപ്പ് (socket_address_family(a)) {

        അവസ്ഥ AF_INET:
                എങ്കിൽ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        തിരികെ false;

                എങ്കിൽ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        തിരികെ false;

                നിർത്തുക;

        അവസ്ഥ AF_INET6:
                എങ്കിൽ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, വലുപ്പം(a->sockaddr.in6.sin6_addr)) != 0)
                        തിരികെ false;

                എങ്കിൽ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        തിരികെ false;

                നിർത്തുക;

        അവസ്ഥ AF_UNIX:
                എങ്കിൽ (a->size <= offsetof(ഘടന sockaddr_un, sun_path) ||
                    b->size <= offsetof(ഘടന sockaddr_un, sun_path))
                        തിരികെ false;

                എങ്കിൽ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        തിരികെ false;

                എങ്കിൽ (a->sockaddr.un.sun_path[0]) {
                        എങ്കിൽ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                തിരികെ false;
                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (a->size != b->size)
                                തിരികെ false;

                        എങ്കിൽ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                തിരികെ false;
                }

                നിർത്തുക;

        അവസ്ഥ AF_NETLINK:
                എങ്കിൽ (a->protocol != b->protocol)
                        തിരികെ false;

                എങ്കിൽ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        തിരികെ false;

                നിർത്തുക;

        അവസ്ഥ AF_VSOCK:
                എങ്കിൽ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        തിരികെ false;

                എങ്കിൽ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        തിരികെ false;

                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                /* Cannot compare, so we assume the addresses are different */
                തിരികെ false;
        }

        തിരികെ true;
}

സ്ഥിരം അക്ഷരം* socket_address_get_path(സ്ഥിരം SocketAddress *a) {
        പ്രതിജ്ഞാ(a);

        എങ്കിൽ (socket_address_family(a) != AF_UNIX)
                തിരികെ ശൂന്യ;

        എങ്കിൽ (a->sockaddr.un.sun_path[0] == 0)
                തിരികെ ശൂന്യ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(വലുപ്പം(സംഘം sockaddr_union) >= വലുപ്പം(ഘടന sockaddr_un)+1);
        തിരികെ a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(ശൂന്യം) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ cached = -1;

        എങ്കിൽ (cached < 0) {

                എങ്കിൽ (access("/proc/net/if_inet6", F_OK) < 0) {

                        എങ്കിൽ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                തിരികെ false;
                        }

                        cached = false;
                } അല്ലെങ്കിൽ
                        cached = true;
        }

        തിരികെ cached;
}

bool socket_ipv6_is_enabled(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        എങ്കിൽ (!socket_ipv6_is_supported())
                തിരികെ false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                തിരികെ true;
        }

        r = parse_boolean(v);
        എങ്കിൽ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                തിരികെ true;
        }

        തിരികെ !r;
}

bool socket_address_matches_fd(സ്ഥിരം SocketAddress *a, പൂർണ്ണസംഖ്യ fd) {
        SocketAddress b;
        socklen_t solen;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(fd >= 0);

        b.size = വലുപ്പം(b.sockaddr);
        എങ്കിൽ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                തിരികെ false;

        എങ്കിൽ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                തിരികെ false;

        solen = വലുപ്പം(b.type);
        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                തിരികെ false;

        എങ്കിൽ (b.type != a->type)
                തിരികെ false;

        എങ്കിൽ (a->protocol != 0)  {
                solen = വലുപ്പം(b.protocol);
                എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        തിരികെ false;

                എങ്കിൽ (b.protocol != a->protocol)
                        തിരികെ false;
        }

        തിരികെ socket_address_equal(a, &b);
}

പൂർണ്ണസംഖ്യ sockaddr_port(സ്ഥിരം ഘടന sockaddr *_sa, unsigned *ret_port) {
        സ്ഥിരം സംഘം sockaddr_union *sa = (സ്ഥിരം സംഘം sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        പ്രതിജ്ഞാ(sa);
        പ്രതിജ്ഞാ(ret_port);

        തിരഞ്ഞെടുപ്പ് (sa->sa.sa_family) {

        അവസ്ഥ AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                തിരികെ 0;

        അവസ്ഥ AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                തിരികെ 0;

        അവസ്ഥ AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                തിരികെ 0;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

സ്ഥിരം സംഘം in_addr_union *sockaddr_in_addr(സ്ഥിരം ഘടന sockaddr *_sa) {
        സ്ഥിരം സംഘം sockaddr_union *sa = (സ്ഥിരം സംഘം sockaddr_union*) _sa;

        എങ്കിൽ (!sa)
                തിരികെ ശൂന്യ;

        തിരഞ്ഞെടുപ്പ് (sa->sa.sa_family) {

        അവസ്ഥ AF_INET:
                തിരികെ (സ്ഥിരം സംഘം in_addr_union*) &sa->in.sin_addr;

        അവസ്ഥ AF_INET6:
                തിരികെ (സ്ഥിരം സംഘം in_addr_union*) &sa->in6.sin6_addr;

        സ്ഥിരസ്ഥിതി:
                തിരികെ ശൂന്യ;
        }
}

പൂർണ്ണസംഖ്യ sockaddr_set_in_addr(
                സംഘം sockaddr_union *u,
                പൂർണ്ണസംഖ്യ family,
                സ്ഥിരം സംഘം in_addr_union *a,
                uint16_t port) {

        പ്രതിജ്ഞാ(u);
        പ്രതിജ്ഞാ(a);

        തിരഞ്ഞെടുപ്പ് (family) {

        അവസ്ഥ AF_INET:
                u->in = (ഘടന sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                തിരികെ 0;

        അവസ്ഥ AF_INET6:
                u->in6 = (ഘടന sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                തിരികെ 0;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;

        }
}

പൂർണ്ണസംഖ്യ sockaddr_pretty(
                സ്ഥിരം ഘടന sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                അക്ഷരം **ret) {

        സംഘം sockaddr_union *sa = (സംഘം sockaddr_union*) _sa;
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(sa);
        പ്രതിജ്ഞാ(salen >= വലുപ്പം(sa->sa.sa_family));
        പ്രതിജ്ഞാ(ret);

        തിരഞ്ഞെടുപ്പ് (sa->sa.sa_family) {

        അവസ്ഥ AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                എങ്കിൽ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                അല്ലെങ്കിൽ
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOMEM;
                നിർത്തുക;
        }

        അവസ്ഥ AF_INET6: {
                സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                എങ്കിൽ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, വലുപ്പം(ipv4_prefix)) == 0) {
                        സ്ഥിരം uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        എങ്കിൽ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        അല്ലെങ്കിൽ
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        എങ്കിൽ (r < 0)
                                തിരികെ -ENOMEM;
                } അല്ലെങ്കിൽ {
                        സ്ഥിരം അക്ഷരം *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        എങ്കിൽ (include_port) {
                                എങ്കിൽ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        തിരികെ -ENOMEM;
                        } അല്ലെങ്കിൽ {
                                എങ്കിൽ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                അല്ലെങ്കിൽ
                                        p = strdup(a);
                                എങ്കിൽ (!p)
                                        തിരികെ -ENOMEM;
                        }
                }

                നിർത്തുക;
        }

        അവസ്ഥ AF_UNIX:
                എങ്കിൽ (salen <= offsetof(ഘടന sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(ഘടന sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                അല്ലെങ്കിൽ {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        അക്ഷരം *path = (അക്ഷരം*) sa->un_buffer + offsetof(ഘടന sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(ഘടന sockaddr_un, sun_path);

                        എങ്കിൽ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ അക്ഷരം *e = ശൂന്യ;

                                e = cescape_length(path + 1, path_len - 1);
                                എങ്കിൽ (!e)
                                        തിരികെ -ENOMEM;

                                p = strjoin("@", e);
                        } അല്ലെങ്കിൽ {
                                എങ്കിൽ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                നിർത്തുക;

        അവസ്ഥ AF_VSOCK:
                എങ്കിൽ (include_port) {
                        എങ്കിൽ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        അല്ലെങ്കിൽ
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } അല്ലെങ്കിൽ
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOMEM;
                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }

        *ret = p;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getpeername_pretty(പൂർണ്ണസംഖ്യ fd, bool include_port, അക്ഷരം **ret) {
        സംഘം sockaddr_union sa;
        socklen_t salen = വലുപ്പം(sa);
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (getpeername(fd, &sa.sa, &salen) < 0)
                തിരികെ -errno;

        എങ്കിൽ (sa.sa.sa_family == AF_UNIX) {
                ഘടന ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        തിരികെ -ENOMEM;

                തിരികെ 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        തിരികെ sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

പൂർണ്ണസംഖ്യ getsockname_pretty(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        സംഘം sockaddr_union sa;
        socklen_t salen = വലുപ്പം(sa);

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (getsockname(fd, &sa.sa, &salen) < 0)
                തിരികെ -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        തിരികെ sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

പൂർണ്ണസംഖ്യ socknameinfo_pretty(സ്ഥിരം ഘടന sockaddr *sa, socklen_t salen, അക്ഷരം **ret) {
        അക്ഷരം host[NI_MAXHOST];
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(sa);
        പ്രതിജ്ഞാ(salen >= വലുപ്പം(sa_family_t));
        പ്രതിജ്ഞാ(ret);

        r = getnameinfo(sa, salen, host, വലുപ്പം(host), /* service= */ ശൂന്യ, /* service_len= */ 0, IDN_FLAGS);
        എങ്കിൽ (r != 0) {
                എങ്കിൽ (r == EAI_MEMORY)
                        തിരികെ log_oom_debug();
                എങ്കിൽ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                അല്ലെങ്കിൽ
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                തിരികെ sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        തിരികെ strdup_to(ret, host);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, പൂർണ്ണസംഖ്യ, INT_MAX);

bool sockaddr_equal(സ്ഥിരം സംഘം sockaddr_union *a, സ്ഥിരം സംഘം sockaddr_union *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        എങ്കിൽ (a->sa.sa_family != b->sa.sa_family)
                തിരികെ false;

        എങ്കിൽ (a->sa.sa_family == AF_INET)
                തിരികെ a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        എങ്കിൽ (a->sa.sa_family == AF_INET6)
                തിരികെ memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, വലുപ്പം(a->in6.sin6_addr)) == 0;

        എങ്കിൽ (a->sa.sa_family == AF_VSOCK)
                തിരികെ a->vm.svm_cid == b->vm.svm_cid;

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ fd_set_sndbuf(പൂർണ്ണസംഖ്യ fd, size_t n, bool increase) {
        പൂർണ്ണസംഖ്യ r, മൂല്യം;
        socklen_t l = വലുപ്പം(മൂല്യം);

        എങ്കിൽ (n > INT_MAX)
                തിരികെ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &മൂല്യം, &l);
        എങ്കിൽ (r >= 0 && l == വലുപ്പം(മൂല്യം) && increase ? (size_t) മൂല്യം >= n*2 : (size_t) മൂല്യം == n*2)
                തിരികെ 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = വലുപ്പം(മൂല്യം);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &മൂല്യം, &l);
        എങ്കിൽ (r >= 0 && l == വലുപ്പം(മൂല്യം) && increase ? (size_t) മൂല്യം >= n*2 : (size_t) മൂല്യം == n*2)
                തിരികെ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ fd_set_rcvbuf(പൂർണ്ണസംഖ്യ fd, size_t n, bool increase) {
        പൂർണ്ണസംഖ്യ r, മൂല്യം;
        socklen_t l = വലുപ്പം(മൂല്യം);

        എങ്കിൽ (n > INT_MAX)
                തിരികെ -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &മൂല്യം, &l);
        എങ്കിൽ (r >= 0 && l == വലുപ്പം(മൂല്യം) && increase ? (size_t) മൂല്യം >= n*2 : (size_t) മൂല്യം == n*2)
                തിരികെ 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = വലുപ്പം(മൂല്യം);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &മൂല്യം, &l);
        എങ്കിൽ (r >= 0 && l == വലുപ്പം(മൂല്യം) && increase ? (size_t) മൂല്യം >= n*2 : (size_t) മൂല്യം == n*2)
                തിരികെ 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, പൂർണ്ണസംഖ്യ, 0xff);

bool ifname_valid_char(അക്ഷരം a) {
        എങ്കിൽ ((unsigned അക്ഷരം) a >= 127U)
                തിരികെ false;

        എങ്കിൽ ((unsigned അക്ഷരം) a <= 32U)
                തിരികെ false;

        എങ്കിൽ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                തിരികെ false;

        തിരികെ true;
}

bool ifname_valid_full(സ്ഥിരം അക്ഷരം *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        പ്രതിജ്ഞാ(!(flags & ~_IFNAME_VALID_ALL));

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        എങ്കിൽ (parse_ifindex(p) >= 0)
                തിരികെ flags & IFNAME_VALID_NUMERIC;

        എങ്കിൽ (flags & IFNAME_VALID_ALTERNATIVE) {
                എങ്കിൽ (strlen(p) >= ALTIFNAMSIZ)
                        തിരികെ false;
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (strlen(p) >= IFNAMSIZ)
                        തിരികെ false;
        }

        എങ്കിൽ (dot_or_dot_dot(p))
                തിരികെ false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        എങ്കിൽ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                തിരികെ false;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *t = p; *t; t++) {
                എങ്കിൽ (!ifname_valid_char(*t))
                        തിരികെ false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        എങ്കിൽ (numeric)
                തിരികെ false;

        തിരികെ true;
}

bool address_label_valid(സ്ഥിരം അക്ഷരം *p) {

        POINTER_MAY_BE_NULL(p);

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        എങ്കിൽ (strlen(p) >= IFNAMSIZ)
                തിരികെ false;

        ആകുമ്പോൾ (*p) {
                എങ്കിൽ ((uint8_t) *p >= 127U)
                        തിരികെ false;

                എങ്കിൽ ((uint8_t) *p <= 31U)
                        തിരികെ false;
                p++;
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ getpeercred(പൂർണ്ണസംഖ്യ fd, ഘടന ucred *ucred) {
        socklen_t n = വലുപ്പം(ഘടന ucred);
        ഘടന ucred u;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ucred);

        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                തിരികെ -errno;

        എങ്കിൽ (n != വലുപ്പം(ഘടന ucred))
                തിരികെ -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        എങ്കിൽ (!pid_is_valid(u.pid))
                തിരികെ -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getpeersec(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        socklen_t n = 64;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (;;) {
                s = new0(അക്ഷരം, n+1);
                എങ്കിൽ (!s)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        നിർത്തുക;
                }

                എങ്കിൽ (errno != ERANGE)
                        തിരികെ -errno;

                s = mfree(s);
        }

        എങ്കിൽ (isempty(s))
                തിരികെ -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getpeergroups(പൂർണ്ണസംഖ്യ fd, gid_t **ret) {
        socklen_t n = വലുപ്പം(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ശൂന്യ;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        പൂർണ്ണസംഖ്യ ngroups_max = sysconf_ngroups_max();
        എങ്കിൽ (ngroups_max > 0)
                n = MAX(n, വലുപ്പം(gid_t) * (socklen_t) ngroups_max);

        വേണ്ടി (;;) {
                d = malloc(n);
                എങ്കിൽ (!d)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        നിർത്തുക;

                എങ്കിൽ (errno != ERANGE)
                        തിരികെ -errno;

                d = mfree(d);
        }

        assert_se(n % വലുപ്പം(gid_t) == 0);
        n /= വലുപ്പം(gid_t);

        എങ്കിൽ (n > INT_MAX)
                തിരികെ -E2BIG;

        *ret = TAKE_PTR(d);

        തിരികെ (പൂർണ്ണസംഖ്യ) n;
}

പൂർണ്ണസംഖ്യ getpeerpidfd(പൂർണ്ണസംഖ്യ fd) {
        socklen_t n = വലുപ്പം(പൂർണ്ണസംഖ്യ);
        പൂർണ്ണസംഖ്യ pidfd = -EBADF;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                തിരികെ -errno;

        എങ്കിൽ (n != വലുപ്പം(പൂർണ്ണസംഖ്യ))
                തിരികെ -EIO;

        തിരികെ pidfd;
}

പൂർണ്ണസംഖ്യ getpeerpidref(പൂർണ്ണസംഖ്യ fd, PidRef *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        പൂർണ്ണസംഖ്യ pidfd = getpeerpidfd(fd);
        എങ്കിൽ (pidfd < 0) {
                എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        തിരികെ pidfd;

                ഘടന ucred ucred;
                r = getpeercred(fd, &ucred);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ pidref_set_pid(ret, ucred.pid);
        }

        തിരികെ pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                പൂർണ്ണസംഖ്യ transport_fd,
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം ഘടന iovec *iov, size_t iovlen,
                സ്ഥിരം ഘടന sockaddr *sa, socklen_t len,
                പൂർണ്ണസംഖ്യ flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ))) control = {};
        ഘടന msghdr mh = {
                .msg_name = (ഘടന sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (ഘടന iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        പ്രതിജ്ഞാ(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        എങ്കിൽ (fd < 0 && !iov)
                തിരികെ -EINVAL;

        എങ്കിൽ (fd >= 0) {
                ഘടന cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = വലുപ്പം(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(വലുപ്പം(പൂർണ്ണസംഖ്യ));
                memcpy(CMSG_DATA(cmsg), &fd, വലുപ്പം(പൂർണ്ണസംഖ്യ));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        എങ്കിൽ (k < 0)
                തിരികെ (ssize_t) -errno;

        തിരികെ k;
}

പൂർണ്ണസംഖ്യ send_one_fd_sa(
                പൂർണ്ണസംഖ്യ transport_fd,
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം ഘടന sockaddr *sa, socklen_t len,
                പൂർണ്ണസംഖ്യ flags) {

        പ്രതിജ്ഞാ(fd >= 0);

        തിരികെ (പൂർണ്ണസംഖ്യ) send_one_fd_iov_sa(transport_fd, fd, ശൂന്യ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                പൂർണ്ണസംഖ്യ transport_fd,
                ഘടന iovec *iov, size_t iovlen,
                പൂർണ്ണസംഖ്യ flags,
                പൂർണ്ണസംഖ്യ *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(വലുപ്പം(പൂർണ്ണസംഖ്യ))) control;
        ഘടന msghdr mh = {
                .msg_control = &control,
                .msg_controllen = വലുപ്പം(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        ഘടന cmsghdr *found;
        ssize_t k;

        പ്രതിജ്ഞാ(transport_fd >= 0);
        പ്രതിജ്ഞാ(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        എങ്കിൽ (k < 0)
                തിരികെ k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(വലുപ്പം(പൂർണ്ണസംഖ്യ)));
        എങ്കിൽ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                എങ്കിൽ (k == 0)
                        തിരികെ -EIO;
        }

        എങ്കിൽ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, പൂർണ്ണസംഖ്യ);
        അല്ലെങ്കിൽ
                *ret_fd = -EBADF;

        തിരികെ k;
}

പൂർണ്ണസംഖ്യ receive_one_fd(പൂർണ്ണസംഖ്യ transport_fd, പൂർണ്ണസംഖ്യ flags) {
        പൂർണ്ണസംഖ്യ fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ശൂന്യ, 0, flags, &fd);
        എങ്കിൽ (k == 0)
                തിരികെ fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        പ്രതിജ്ഞാ(k < 0);
        തിരികെ (പൂർണ്ണസംഖ്യ) k;
}

ssize_t next_datagram_size_fd(പൂർണ്ണസംഖ്യ fd) {
        ssize_t l;
        പൂർണ്ണസംഖ്യ k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ശൂന്യ, 0, MSG_PEEK|MSG_TRUNC);
        എങ്കിൽ (l < 0) {
                എങ്കിൽ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        പോകുക fallback;

                തിരികെ -errno;
        }
        എങ്കിൽ (l == 0)
                പോകുക fallback;

        തിരികെ l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        എങ്കിൽ (ioctl(fd, FIONREAD, &k) < 0)
                തിരികെ -errno;

        തിരികെ (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

പൂർണ്ണസംഖ്യ flush_accept(പൂർണ്ണസംഖ്യ fd) {

        പൂർണ്ണസംഖ്യ r, b;
        socklen_t l = വലുപ്പം(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                തിരികെ -errno;

        പ്രതിജ്ഞാ(l == വലുപ്പം(b));
        എങ്കിൽ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                തിരികെ -ENOTTY;

        വേണ്ടി (unsigned iteration = 0;; iteration++) {
                പൂർണ്ണസംഖ്യ cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                എങ്കിൽ (r == -EINTR)
                        തുടരുക;
                എങ്കിൽ (r <= 0)
                        തിരികെ r;

                എങ്കിൽ (iteration >= MAX_FLUSH_ITERATIONS)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ശൂന്യ, ശൂന്യ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                എങ്കിൽ (cfd < 0) {
                        എങ്കിൽ (errno == EAGAIN)
                                തിരികെ 0;

                        എങ്കിൽ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                തുടരുക;

                        തിരികെ -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(പൂർണ്ണസംഖ്യ fd) {
        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        ഘടന mq_attr attr;
        ssize_t എണ്ണം = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        വേണ്ടി (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                എങ്കിൽ (r == -EINTR)
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ എണ്ണം;

                എങ്കിൽ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        എങ്കിൽ (mq_getattr(fd, &attr) < 0)
                                തിരികെ -errno;

                        buf = malloc(attr.mq_msgsize);
                        എങ്കിൽ (!buf)
                                തിരികെ -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ശൂന്യ);
                എങ്കിൽ (l < 0) {
                        എങ്കിൽ (errno == EINTR)
                                തുടരുക;

                        എങ്കിൽ (errno == EAGAIN)
                                തിരികെ എണ്ണം;

                        തിരികെ -errno;
                }

                എണ്ണം += l;
        }
}

ഘടന cmsghdr* cmsg_find(ഘടന msghdr *mh, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ type, socklen_t length) {
        ഘടന cmsghdr *cmsg;

        പ്രതിജ്ഞാ(mh);

        CMSG_FOREACH(cmsg, mh)
                എങ്കിൽ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        തിരികെ cmsg;

        തിരികെ ശൂന്യ;
}

ശൂന്യം* cmsg_find_and_copy_data(ഘടന msghdr *mh, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ type, ശൂന്യം *buf, size_t buf_len) {
        ഘടന cmsghdr *cmsg;

        പ്രതിജ്ഞാ(mh);
        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        എങ്കിൽ (!cmsg)
                തിരികെ ശൂന്യ;

        തിരികെ memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(സ്ഥിരം ഘടന sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        പ്രതിജ്ഞാ(sa->sll_family == AF_PACKET);

        size_t mac_len = വലുപ്പം(sa->sll_addr);

        എങ്കിൽ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        എങ്കിൽ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        തിരികെ offsetof(ഘടന sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(സ്ഥിരം ഘടന sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        പ്രതിജ്ഞാ(sa->sun_family == AF_UNIX);

        തിരികെ offsetof(ഘടന sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, വലുപ്പം(sa->sun_path)-1) :
                        strnlen(sa->sun_path, വലുപ്പം(sa->sun_path))+1);
}

size_t sockaddr_len(സ്ഥിരം സംഘം sockaddr_union *sa) {
        പ്രതിജ്ഞാ(sa);

        തിരഞ്ഞെടുപ്പ് (sa->sa.sa_family) {
        അവസ്ഥ AF_INET:
                തിരികെ വലുപ്പം(ഘടന sockaddr_in);
        അവസ്ഥ AF_INET6:
                തിരികെ വലുപ്പം(ഘടന sockaddr_in6);
        അവസ്ഥ AF_UNIX:
                തിരികെ sockaddr_un_len(&sa->un);
        അവസ്ഥ AF_PACKET:
                തിരികെ sockaddr_ll_len(&sa->ll);
        അവസ്ഥ AF_NETLINK:
                തിരികെ വലുപ്പം(ഘടന sockaddr_nl);
        അവസ്ഥ AF_VSOCK:
                തിരികെ വലുപ്പം(ഘടന sockaddr_vm);
        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}

പൂർണ്ണസംഖ്യ socket_ioctl_fd(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        എങ്കിൽ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        തിരികെ fd;
}

പൂർണ്ണസംഖ്യ sockaddr_un_unlink(സ്ഥിരം ഘടന sockaddr_un *sa) {
        സ്ഥിരം അക്ഷരം *p, * nul;

        പ്രതിജ്ഞാ(sa);

        എങ്കിൽ (sa->sun_family != AF_UNIX)
                തിരികെ -EPROTOTYPE;

        എങ്കിൽ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                തിരികെ 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, വലുപ്പം(sa->sun_path));
        എങ്കിൽ (nul)
                p = sa->sun_path;
        അല്ലെങ്കിൽ
                p = memdupa_suffix0(sa->sun_path, വലുപ്പം(sa->sun_path));

        എങ്കിൽ (unlink(p) < 0)
                തിരികെ -errno;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ sockaddr_un_set_path(ഘടന sockaddr_un *ret, സ്ഥിരം അക്ഷരം *path) {
        size_t l;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        എങ്കിൽ (l < 2)
                തിരികെ -EINVAL;
        എങ്കിൽ (!IN_SET(path[0], '/', '@'))
                തിരികെ -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        എങ്കിൽ (l+1 > വലുപ്പം(ret->sun_path))
                തിരികെ path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (ഘടന sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        എങ്കിൽ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                തിരികെ (പൂർണ്ണസംഖ്യ) (offsetof(ഘടന sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } അല്ലെങ്കിൽ {
                പ്രതിജ്ഞാ(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                തിരികെ (പൂർണ്ണസംഖ്യ) (offsetof(ഘടന sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

പൂർണ്ണസംഖ്യ getsockopt_int(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ optname, പൂർണ്ണസംഖ്യ *ret) {
        പൂർണ്ണസംഖ്യ v;
        socklen_t sl = വലുപ്പം(v);

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (getsockopt(fd, level, optname, &v, &sl) < 0)
                തിരികെ negative_errno();
        എങ്കിൽ (sl != വലുപ്പം(v))
                തിരികെ -EIO;

        *ret = v;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ socket_bind_to_ifname(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *ifname) {
        പ്രതിജ്ഞാ(fd >= 0);

        /* Call with NULL to drop binding */

        തിരികെ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

പൂർണ്ണസംഖ്യ socket_bind_to_ifindex(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ ifindex) {
        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (ifindex <= 0)
                /* Drop binding */
                തിരികെ RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ശൂന്യ, 0));

        തിരികെ setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

പൂർണ്ണസംഖ്യ socket_autobind(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret_name) {
        _cleanup_free_ അക്ഷരം *name = ശൂന്യ;
        uint64_t random;
        പൂർണ്ണസംഖ്യ r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        പ്രതിജ്ഞാ(fd >= 0);

        random = random_u64();

        എങ്കിൽ (asprintf(&name, "@%" PRIu64, random) < 0)
                തിരികെ -ENOMEM;

        സംഘം sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < വലുപ്പം(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (bind(fd, &sa.sa, r) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_name)
                *ret_name = TAKE_PTR(name);
        തിരികെ 0;
}

ssize_t recvmsg_safe(പൂർണ്ണസംഖ്യ sockfd, ഘടന msghdr *msg, പൂർണ്ണസംഖ്യ flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        പ്രതിജ്ഞാ(sockfd >= 0);
        പ്രതിജ്ഞാ(msg);

        n = recvmsg(sockfd, msg, flags);
        എങ്കിൽ (n < 0)
                തിരികെ -errno;

        എങ്കിൽ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                തിരികെ FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        തിരികെ n;
}

പൂർണ്ണസംഖ്യ socket_get_family(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ af;
        socklen_t sl = വലുപ്പം(af);

        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                തിരികെ -errno;

        എങ്കിൽ (sl != വലുപ്പം(af))
                തിരികെ -EINVAL;

        തിരികെ af;
}

പൂർണ്ണസംഖ്യ socket_set_recvpktinfo(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {

        എങ്കിൽ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                എങ്കിൽ (af < 0)
                        തിരികെ af;
        }

        തിരഞ്ഞെടുപ്പ് (af) {

        അവസ്ഥ AF_INET:
                തിരികെ setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        അവസ്ഥ AF_INET6:
                തിരികെ setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        അവസ്ഥ AF_NETLINK:
                തിരികെ setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        അവസ്ഥ AF_PACKET:
                തിരികെ setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ socket_set_unicast_if(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, പൂർണ്ണസംഖ്യ ifi) {
        be32_t ifindex_be = htobe32(ifi);

        എങ്കിൽ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                എങ്കിൽ (af < 0)
                        തിരികെ af;
        }

        തിരഞ്ഞെടുപ്പ് (af) {

        അവസ്ഥ AF_INET:
                തിരികെ RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, വലുപ്പം(ifindex_be)));

        അവസ്ഥ AF_INET6:
                തിരികെ RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, വലുപ്പം(ifindex_be)));

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ socket_set_option(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, പൂർണ്ണസംഖ്യ opt_ipv4, പൂർണ്ണസംഖ്യ opt_ipv6, പൂർണ്ണസംഖ്യ val) {
        എങ്കിൽ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                എങ്കിൽ (af < 0)
                        തിരികെ af;
        }

        തിരഞ്ഞെടുപ്പ് (af) {

        അവസ്ഥ AF_INET:
                തിരികെ setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        അവസ്ഥ AF_INET6:
                തിരികെ setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ socket_get_mtu(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, size_t *ret) {
        പൂർണ്ണസംഖ്യ mtu, r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                എങ്കിൽ (af < 0)
                        തിരികെ af;
        }

        തിരഞ്ഞെടുപ്പ് (af) {

        അവസ്ഥ AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                നിർത്തുക;

        അവസ്ഥ AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EAFNOSUPPORT;
        }

        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (mtu <= 0)
                തിരികെ -EINVAL;

        *ret = (size_t) mtu;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ connect_unix_path_simple(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path) {
        സംഘം sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(path);

        l = strlen(path);
        പ്രതിജ്ഞാ(l > 0);
        പ്രതിജ്ഞാ(l < വലുപ്പം(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        തിരികെ RET_NERRNO(connect(fd, &sa.sa, offsetof(ഘടന sockaddr_un, sun_path) + l + 1));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ connect_unix_inode(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ inode_fd) {
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(inode_fd >= 0);

        തിരികെ connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

പൂർണ്ണസംഖ്യ connect_unix_path(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ inode_fd = -EBADF;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        എങ്കിൽ (!path) {
                എങ്കിൽ (dir_fd < 0)
                        തിരികെ -EISDIR;

                തിരികെ connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        എങ്കിൽ (isempty(path))
                തിരികെ -EINVAL;

        /* Shortcut for the simple case */
        എങ്കിൽ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(ഘടന sockaddr_un, sun_path))
                തിരികെ connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        എങ്കിൽ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ അക്ഷരം *j = strjoin("/", path);
                എങ്കിൽ (!j)
                        തിരികെ -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } അല്ലെങ്കിൽ
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        എങ്കിൽ (inode_fd < 0)
                തിരികെ -errno;

        തിരികെ connect_unix_inode(fd, inode_fd);
}

പൂർണ്ണസംഖ്യ socket_address_parse_unix(SocketAddress *ret_address, സ്ഥിരം അക്ഷരം *s) {
        ഘടന sockaddr_un un;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_address);
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (!IN_SET(*s, '/', '@'))
                തിരികെ -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ socket_address_equal_unix(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        SocketAddress socket_a, socket_b;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = socket_address_parse_unix(&socket_a, a);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = socket_address_parse_unix(&socket_b, b);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

പൂർണ്ണസംഖ്യ vsock_parse_port(സ്ഥിരം അക്ഷരം *s, unsigned *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!s)
                തിരികെ -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        എങ്കിൽ (u == VMADDR_PORT_ANY)
                തിരികെ -EINVAL;

        *ret = u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ vsock_parse_cid(സ്ഥിരം അക്ഷരം *s, unsigned *ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!s)
                തിരികെ -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        എങ്കിൽ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        അല്ലെങ്കിൽ എങ്കിൽ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        അല്ലെങ്കിൽ എങ്കിൽ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        അല്ലെങ്കിൽ എങ്കിൽ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        അല്ലെങ്കിൽ
                തിരികെ safe_atou(s, ret);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ socket_address_parse_vsock(SocketAddress *ret_address, സ്ഥിരം അക്ഷരം *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ അക്ഷരം *n = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e, *cid_start;
        unsigned port, cid;
        പൂർണ്ണസംഖ്യ type, r;

        പ്രതിജ്ഞാ(ret_address);
        പ്രതിജ്ഞാ(s);

        എങ്കിൽ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        അല്ലെങ്കിൽ എങ്കിൽ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        അല്ലെങ്കിൽ എങ്കിൽ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        അല്ലെങ്കിൽ എങ്കിൽ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        അല്ലെങ്കിൽ
                തിരികെ -EPROTO;

        e = strchr(cid_start, ':');
        എങ്കിൽ (!e)
                തിരികെ -EINVAL;

        r = vsock_parse_port(e+1, &port);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        n = strndup(cid_start, e - cid_start);
        എങ്കിൽ (!n)
                തിരികെ -ENOMEM;

        എങ്കിൽ (isempty(n))
                cid = VMADDR_CID_ANY;
        അല്ലെങ്കിൽ {
                r = vsock_parse_cid(n, &cid);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = വലുപ്പം(ഘടന sockaddr_vm),
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ netlink_socket_get_multicast_groups(പൂർണ്ണസംഖ്യ fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ശൂന്യ;
        socklen_t len = 0, old_len;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ശൂന്യ, &len) < 0)
                തിരികെ -errno;

        എങ്കിൽ (len == 0)
                പോകുക finalize;

        groups = new0(uint32_t, len);
        എങ്കിൽ (!groups)
                തിരികെ -ENOMEM;

        old_len = len;

        എങ്കിൽ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                തിരികെ -errno;

        എങ്കിൽ (old_len != len)
                തിരികെ -EIO;

finalize:
        എങ്കിൽ (ret_len)
                *ret_len = len;
        എങ്കിൽ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ socket_get_cookie(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        പ്രതിജ്ഞാ(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = വലുപ്പം(cookie);
        എങ്കിൽ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                തിരികെ -errno;

        പ്രതിജ്ഞാ(cookie_len == വലുപ്പം(cookie));
        എങ്കിൽ (ret)
                *ret = cookie;

        തിരികെ 0;
}

ശൂന്യം cmsg_close_all(ഘടന msghdr *mh) {
        പ്രതിജ്ഞാ(mh);

        ഘടന cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                എങ്കിൽ (cmsg->cmsg_level != SOL_SOCKET)
                        തുടരുക;

                എങ്കിൽ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, പൂർണ്ണസംഖ്യ),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / വലുപ്പം(പൂർണ്ണസംഖ്യ));
                അല്ലെങ്കിൽ എങ്കിൽ (cmsg->cmsg_type == SCM_PIDFD) {
                        പ്രതിജ്ഞാ(cmsg->cmsg_len == CMSG_LEN(വലുപ്പം(പൂർണ്ണസംഖ്യ)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, പൂർണ്ണസംഖ്യ));
                }
        }
}

പൂർണ്ണസംഖ്യ tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        തിരഞ്ഞെടുപ്പ് (IPTOS_PREC(tos)) {
        അവസ്ഥ IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        അവസ്ഥ IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                തിരികെ TC_PRIO_CONTROL;

        അവസ്ഥ IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        അവസ്ഥ IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                തിരികെ TC_PRIO_INTERACTIVE;

        അവസ്ഥ IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        അവസ്ഥ IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                തിരികെ TC_PRIO_INTERACTIVE_BULK;

        അവസ്ഥ IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                തിരികെ TC_PRIO_BULK;

        അവസ്ഥ IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        സ്ഥിരസ്ഥിതി:
                തിരികെ TC_PRIO_BESTEFFORT;
        }
}

പൂർണ്ണസംഖ്യ socket_xattr_supported(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        സ്ഥിതം പൂർണ്ണസംഖ്യ cached = -1;
        എങ്കിൽ (cached >= 0)
                തിരികെ cached;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ശൂന്യ, AT_EMPTY_PATH, "user.testxxx", "1");
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                തിരികെ (cached = false);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to set test xattr on socket: %m");

        തിരികെ (cached = true);
}
