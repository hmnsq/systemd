/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/if.h>
#અન્તર્ભાવ <linux/if_arp.h>
#અન્તર્ભાવ <linux/if_ether.h>
#અન્તર્ભાવ <linux/if_infiniband.h>
#અન્તર્ભાવ <linux/pkt_sched.h>
#અન્તર્ભાવ <mqueue.h>
#અન્તર્ભાવ <net/if.h>
#અન્તર્ભાવ <netdb.h>
#અન્તર્ભાવ <netinet/ip.h>
#અન્તર્ભાવ <poll.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "format-ifname.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "in-addr-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "random-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "sparse-endian.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "sysctl-util.h"
#અન્તર્ભાવ "user-util.h"
#અન્તર્ભાવ "xattr-util.h"

#જો ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#નહીંતર
#  define IDN_FLAGS 0
#endif

સ્થિર અચલ અક્ષર* અચલ socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, પૂર્ણાંક);

પૂર્ણાંક socket_address_verify(અચલ SocketAddress *a, bool strict) {
        પ્રતિજ્ઞા(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        પસંદગી (socket_address_family(a)) {

        સ્થિતિ AF_INET:
                જો (a->size != કદ(સંરચના sockaddr_in))
                        પાછા -EINVAL;

                જો (a->sockaddr.in.sin_port == 0)
                        પાછા -EINVAL;

                જો (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        પાછા -EINVAL;

                પાછા 0;

        સ્થિતિ AF_INET6:
                જો (a->size != કદ(સંરચના sockaddr_in6))
                        પાછા -EINVAL;

                જો (a->sockaddr.in6.sin6_port == 0)
                        પાછા -EINVAL;

                જો (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        પાછા -EINVAL;

                પાછા 0;

        સ્થિતિ AF_UNIX:
                જો (a->size < offsetof(સંરચના sockaddr_un, sun_path))
                        પાછા -EINVAL;
                જો (a->size > કદ(સંરચના sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        પાછા -EINVAL;

                જો (a->size > offsetof(સંરચના sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        અચલ અક્ષર *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, કદ(a->sockaddr.un.sun_path));
                        જો (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                જો (a->size != offsetof(સંરચના sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        પાછા -EINVAL;
                        } નહીંતર {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                જો (!IN_SET(a->size, કદ(a->sockaddr.un.sun_path), કદ(a->sockaddr.un.sun_path)+1))
                                        પાછા -EINVAL;
                        }
                }

                જો (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        પાછા -EINVAL;

                પાછા 0;

        સ્થિતિ AF_NETLINK:

                જો (a->size != કદ(સંરચના sockaddr_nl))
                        પાછા -EINVAL;

                જો (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        પાછા -EINVAL;

                પાછા 0;

        સ્થિતિ AF_VSOCK:
                જો (a->size != કદ(સંરચના sockaddr_vm))
                        પાછા -EINVAL;

                જો (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        પાછા -EINVAL;

                પાછા 0;

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક socket_address_print(અચલ SocketAddress *a, અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        જો (r < 0)
                પાછા r;

        જો (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ અક્ષર *sfamily = ખાલી;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                જો (r < 0)
                        પાછા r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                જો (r < 0)
                        પાછા -ENOMEM;

                પાછા 0;
        }

        પાછા sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(અચલ SocketAddress *a) {
        પ્રતિજ્ઞા(a);

        પાછા
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(અચલ SocketAddress *a, અચલ SocketAddress *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        /* Invalid addresses are unequal to all */
        જો (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                પાછા false;

        જો (a->type != b->type)
                પાછા false;

        જો (socket_address_family(a) != socket_address_family(b))
                પાછા false;

        પસંદગી (socket_address_family(a)) {

        સ્થિતિ AF_INET:
                જો (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        પાછા false;

                જો (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        પાછા false;

                તોડો;

        સ્થિતિ AF_INET6:
                જો (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, કદ(a->sockaddr.in6.sin6_addr)) != 0)
                        પાછા false;

                જો (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        પાછા false;

                તોડો;

        સ્થિતિ AF_UNIX:
                જો (a->size <= offsetof(સંરચના sockaddr_un, sun_path) ||
                    b->size <= offsetof(સંરચના sockaddr_un, sun_path))
                        પાછા false;

                જો ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        પાછા false;

                જો (a->sockaddr.un.sun_path[0]) {
                        જો (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                પાછા false;
                } નહીંતર {
                        જો (a->size != b->size)
                                પાછા false;

                        જો (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                પાછા false;
                }

                તોડો;

        સ્થિતિ AF_NETLINK:
                જો (a->protocol != b->protocol)
                        પાછા false;

                જો (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        પાછા false;

                તોડો;

        સ્થિતિ AF_VSOCK:
                જો (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        પાછા false;

                જો (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        પાછા false;

                તોડો;

        મૂળભૂત:
                /* Cannot compare, so we assume the addresses are different */
                પાછા false;
        }

        પાછા true;
}

અચલ અક્ષર* socket_address_get_path(અચલ SocketAddress *a) {
        પ્રતિજ્ઞા(a);

        જો (socket_address_family(a) != AF_UNIX)
                પાછા ખાલી;

        જો (a->sockaddr.un.sun_path[0] == 0)
                પાછા ખાલી;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(કદ(સંઘ sockaddr_union) >= કદ(સંરચના sockaddr_un)+1);
        પાછા a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(શૂન્ય) {
        સ્થિર પૂર્ણાંક cached = -1;

        જો (cached < 0) {

                જો (access("/proc/net/if_inet6", F_OK) < 0) {

                        જો (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                પાછા false;
                        }

                        cached = false;
                } નહીંતર
                        cached = true;
        }

        પાછા cached;
}

bool socket_ipv6_is_enabled(શૂન્ય) {
        _cleanup_free_ અક્ષર *v = ખાલી;
        પૂર્ણાંક r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        જો (!socket_ipv6_is_supported())
                પાછા false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        જો (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                પાછા true;
        }

        r = parse_boolean(v);
        જો (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                પાછા true;
        }

        પાછા !r;
}

bool socket_address_matches_fd(અચલ SocketAddress *a, પૂર્ણાંક fd) {
        SocketAddress b;
        socklen_t solen;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(fd >= 0);

        b.size = કદ(b.sockaddr);
        જો (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                પાછા false;

        જો (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                પાછા false;

        solen = કદ(b.type);
        જો (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                પાછા false;

        જો (b.type != a->type)
                પાછા false;

        જો (a->protocol != 0)  {
                solen = કદ(b.protocol);
                જો (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        પાછા false;

                જો (b.protocol != a->protocol)
                        પાછા false;
        }

        પાછા socket_address_equal(a, &b);
}

પૂર્ણાંક sockaddr_port(અચલ સંરચના sockaddr *_sa, unsigned *ret_port) {
        અચલ સંઘ sockaddr_union *sa = (અચલ સંઘ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        પ્રતિજ્ઞા(sa);
        પ્રતિજ્ઞા(ret_port);

        પસંદગી (sa->sa.sa_family) {

        સ્થિતિ AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                પાછા 0;

        સ્થિતિ AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                પાછા 0;

        સ્થિતિ AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                પાછા 0;

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

અચલ સંઘ in_addr_union *sockaddr_in_addr(અચલ સંરચના sockaddr *_sa) {
        અચલ સંઘ sockaddr_union *sa = (અચલ સંઘ sockaddr_union*) _sa;

        જો (!sa)
                પાછા ખાલી;

        પસંદગી (sa->sa.sa_family) {

        સ્થિતિ AF_INET:
                પાછા (અચલ સંઘ in_addr_union*) &sa->in.sin_addr;

        સ્થિતિ AF_INET6:
                પાછા (અચલ સંઘ in_addr_union*) &sa->in6.sin6_addr;

        મૂળભૂત:
                પાછા ખાલી;
        }
}

પૂર્ણાંક sockaddr_set_in_addr(
                સંઘ sockaddr_union *u,
                પૂર્ણાંક family,
                અચલ સંઘ in_addr_union *a,
                uint16_t port) {

        પ્રતિજ્ઞા(u);
        પ્રતિજ્ઞા(a);

        પસંદગી (family) {

        સ્થિતિ AF_INET:
                u->in = (સંરચના sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                પાછા 0;

        સ્થિતિ AF_INET6:
                u->in6 = (સંરચના sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                પાછા 0;

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;

        }
}

પૂર્ણાંક sockaddr_pretty(
                અચલ સંરચના sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                અક્ષર **ret) {

        સંઘ sockaddr_union *sa = (સંઘ sockaddr_union*) _sa;
        અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(sa);
        પ્રતિજ્ઞા(salen >= કદ(sa->sa.sa_family));
        પ્રતિજ્ઞા(ret);

        પસંદગી (sa->sa.sa_family) {

        સ્થિતિ AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                જો (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                નહીંતર
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                જો (r < 0)
                        પાછા -ENOMEM;
                તોડો;
        }

        સ્થિતિ AF_INET6: {
                સ્થિર અચલ unsigned અક્ષર ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                જો (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, કદ(ipv4_prefix)) == 0) {
                        અચલ uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        જો (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        નહીંતર
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        જો (r < 0)
                                પાછા -ENOMEM;
                } નહીંતર {
                        અચલ અક્ષર *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        જો (include_port) {
                                જો (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        પાછા -ENOMEM;
                        } નહીંતર {
                                જો (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                નહીંતર
                                        p = strdup(a);
                                જો (!p)
                                        પાછા -ENOMEM;
                        }
                }

                તોડો;
        }

        સ્થિતિ AF_UNIX:
                જો (salen <= offsetof(સંરચના sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(સંરચના sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                નહીંતર {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        અક્ષર *path = (અક્ષર*) sa->un_buffer + offsetof(સંરચના sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(સંરચના sockaddr_un, sun_path);

                        જો (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ અક્ષર *e = ખાલી;

                                e = cescape_length(path + 1, path_len - 1);
                                જો (!e)
                                        પાછા -ENOMEM;

                                p = strjoin("@", e);
                        } નહીંતર {
                                જો (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                જો (!p)
                        પાછા -ENOMEM;

                તોડો;

        સ્થિતિ AF_VSOCK:
                જો (include_port) {
                        જો (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        નહીંતર
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } નહીંતર
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                જો (r < 0)
                        પાછા -ENOMEM;
                તોડો;

        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }

        *ret = p;
        પાછા 0;
}

પૂર્ણાંક getpeername_pretty(પૂર્ણાંક fd, bool include_port, અક્ષર **ret) {
        સંઘ sockaddr_union sa;
        socklen_t salen = કદ(sa);
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (getpeername(fd, &sa.sa, &salen) < 0)
                પાછા -errno;

        જો (sa.sa.sa_family == AF_UNIX) {
                સંરચના ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                જો (r < 0)
                        પાછા r;

                જો (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        પાછા -ENOMEM;

                પાછા 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        પાછા sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

પૂર્ણાંક getsockname_pretty(પૂર્ણાંક fd, અક્ષર **ret) {
        સંઘ sockaddr_union sa;
        socklen_t salen = કદ(sa);

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (getsockname(fd, &sa.sa, &salen) < 0)
                પાછા -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        પાછા sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

પૂર્ણાંક socknameinfo_pretty(અચલ સંરચના sockaddr *sa, socklen_t salen, અક્ષર **ret) {
        અક્ષર host[NI_MAXHOST];
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(sa);
        પ્રતિજ્ઞા(salen >= કદ(sa_family_t));
        પ્રતિજ્ઞા(ret);

        r = getnameinfo(sa, salen, host, કદ(host), /* service= */ ખાલી, /* service_len= */ 0, IDN_FLAGS);
        જો (r != 0) {
                જો (r == EAI_MEMORY)
                        પાછા log_oom_debug();
                જો (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                નહીંતર
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                પાછા sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        પાછા strdup_to(ret, host);
}

સ્થિર અચલ અક્ષર* અચલ netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, પૂર્ણાંક, INT_MAX);

bool sockaddr_equal(અચલ સંઘ sockaddr_union *a, અચલ સંઘ sockaddr_union *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        જો (a->sa.sa_family != b->sa.sa_family)
                પાછા false;

        જો (a->sa.sa_family == AF_INET)
                પાછા a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        જો (a->sa.sa_family == AF_INET6)
                પાછા memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, કદ(a->in6.sin6_addr)) == 0;

        જો (a->sa.sa_family == AF_VSOCK)
                પાછા a->vm.svm_cid == b->vm.svm_cid;

        પાછા false;
}

પૂર્ણાંક fd_set_sndbuf(પૂર્ણાંક fd, size_t n, bool increase) {
        પૂર્ણાંક r, કિંમત;
        socklen_t l = કદ(કિંમત);

        જો (n > INT_MAX)
                પાછા -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &કિંમત, &l);
        જો (r >= 0 && l == કદ(કિંમત) && increase ? (size_t) કિંમત >= n*2 : (size_t) કિંમત == n*2)
                પાછા 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        જો (r < 0)
                પાછા r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = કદ(કિંમત);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &કિંમત, &l);
        જો (r >= 0 && l == કદ(કિંમત) && increase ? (size_t) કિંમત >= n*2 : (size_t) કિંમત == n*2)
                પાછા 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

પૂર્ણાંક fd_set_rcvbuf(પૂર્ણાંક fd, size_t n, bool increase) {
        પૂર્ણાંક r, કિંમત;
        socklen_t l = કદ(કિંમત);

        જો (n > INT_MAX)
                પાછા -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &કિંમત, &l);
        જો (r >= 0 && l == કદ(કિંમત) && increase ? (size_t) કિંમત >= n*2 : (size_t) કિંમત == n*2)
                પાછા 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        જો (r < 0)
                પાછા r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = કદ(કિંમત);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &કિંમત, &l);
        જો (r >= 0 && l == કદ(કિંમત) && increase ? (size_t) કિંમત >= n*2 : (size_t) કિંમત == n*2)
                પાછા 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

સ્થિર અચલ અક્ષર* અચલ ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, પૂર્ણાંક, 0xff);

bool ifname_valid_char(અક્ષર a) {
        જો ((unsigned અક્ષર) a >= 127U)
                પાછા false;

        જો ((unsigned અક્ષર) a <= 32U)
                પાછા false;

        જો (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                પાછા false;

        પાછા true;
}

bool ifname_valid_full(અચલ અક્ષર *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        પ્રતિજ્ઞા(!(flags & ~_IFNAME_VALID_ALL));

        જો (isempty(p))
                પાછા false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        જો (parse_ifindex(p) >= 0)
                પાછા flags & IFNAME_VALID_NUMERIC;

        જો (flags & IFNAME_VALID_ALTERNATIVE) {
                જો (strlen(p) >= ALTIFNAMSIZ)
                        પાછા false;
        } નહીંતર {
                જો (strlen(p) >= IFNAMSIZ)
                        પાછા false;
        }

        જો (dot_or_dot_dot(p))
                પાછા false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        જો (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                પાછા false;

        માટે (અચલ અક્ષર *t = p; *t; t++) {
                જો (!ifname_valid_char(*t))
                        પાછા false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        જો (numeric)
                પાછા false;

        પાછા true;
}

bool address_label_valid(અચલ અક્ષર *p) {

        POINTER_MAY_BE_NULL(p);

        જો (isempty(p))
                પાછા false;

        જો (strlen(p) >= IFNAMSIZ)
                પાછા false;

        જ્યારે (*p) {
                જો ((uint8_t) *p >= 127U)
                        પાછા false;

                જો ((uint8_t) *p <= 31U)
                        પાછા false;
                p++;
        }

        પાછા true;
}

પૂર્ણાંક getpeercred(પૂર્ણાંક fd, સંરચના ucred *ucred) {
        socklen_t n = કદ(સંરચના ucred);
        સંરચના ucred u;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ucred);

        જો (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                પાછા -errno;

        જો (n != કદ(સંરચના ucred))
                પાછા -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        જો (!pid_is_valid(u.pid))
                પાછા -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        પાછા 0;
}

પૂર્ણાંક getpeersec(પૂર્ણાંક fd, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        socklen_t n = 64;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        માટે (;;) {
                s = new0(અક્ષર, n+1);
                જો (!s)
                        પાછા -ENOMEM;

                જો (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        તોડો;
                }

                જો (errno != ERANGE)
                        પાછા -errno;

                s = mfree(s);
        }

        જો (isempty(s))
                પાછા -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        પાછા 0;
}

પૂર્ણાંક getpeergroups(પૂર્ણાંક fd, gid_t **ret) {
        socklen_t n = કદ(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ખાલી;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        પૂર્ણાંક ngroups_max = sysconf_ngroups_max();
        જો (ngroups_max > 0)
                n = MAX(n, કદ(gid_t) * (socklen_t) ngroups_max);

        માટે (;;) {
                d = malloc(n);
                જો (!d)
                        પાછા -ENOMEM;

                જો (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        તોડો;

                જો (errno != ERANGE)
                        પાછા -errno;

                d = mfree(d);
        }

        assert_se(n % કદ(gid_t) == 0);
        n /= કદ(gid_t);

        જો (n > INT_MAX)
                પાછા -E2BIG;

        *ret = TAKE_PTR(d);

        પાછા (પૂર્ણાંક) n;
}

પૂર્ણાંક getpeerpidfd(પૂર્ણાંક fd) {
        socklen_t n = કદ(પૂર્ણાંક);
        પૂર્ણાંક pidfd = -EBADF;

        પ્રતિજ્ઞા(fd >= 0);

        જો (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                પાછા -errno;

        જો (n != કદ(પૂર્ણાંક))
                પાછા -EIO;

        પાછા pidfd;
}

પૂર્ણાંક getpeerpidref(પૂર્ણાંક fd, PidRef *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        પૂર્ણાંક pidfd = getpeerpidfd(fd);
        જો (pidfd < 0) {
                જો (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        પાછા pidfd;

                સંરચના ucred ucred;
                r = getpeercred(fd, &ucred);
                જો (r < 0)
                        પાછા r;

                પાછા pidref_set_pid(ret, ucred.pid);
        }

        પાછા pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                પૂર્ણાંક transport_fd,
                પૂર્ણાંક fd,
                અચલ સંરચના iovec *iov, size_t iovlen,
                અચલ સંરચના sockaddr *sa, socklen_t len,
                પૂર્ણાંક flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(કદ(પૂર્ણાંક))) control = {};
        સંરચના msghdr mh = {
                .msg_name = (સંરચના sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (સંરચના iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        પ્રતિજ્ઞા(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        જો (fd < 0 && !iov)
                પાછા -EINVAL;

        જો (fd >= 0) {
                સંરચના cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = કદ(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(કદ(પૂર્ણાંક));
                memcpy(CMSG_DATA(cmsg), &fd, કદ(પૂર્ણાંક));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        જો (k < 0)
                પાછા (ssize_t) -errno;

        પાછા k;
}

પૂર્ણાંક send_one_fd_sa(
                પૂર્ણાંક transport_fd,
                પૂર્ણાંક fd,
                અચલ સંરચના sockaddr *sa, socklen_t len,
                પૂર્ણાંક flags) {

        પ્રતિજ્ઞા(fd >= 0);

        પાછા (પૂર્ણાંક) send_one_fd_iov_sa(transport_fd, fd, ખાલી, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                પૂર્ણાંક transport_fd,
                સંરચના iovec *iov, size_t iovlen,
                પૂર્ણાંક flags,
                પૂર્ણાંક *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(કદ(પૂર્ણાંક))) control;
        સંરચના msghdr mh = {
                .msg_control = &control,
                .msg_controllen = કદ(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        સંરચના cmsghdr *found;
        ssize_t k;

        પ્રતિજ્ઞા(transport_fd >= 0);
        પ્રતિજ્ઞા(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        જો (k < 0)
                પાછા k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(કદ(પૂર્ણાંક)));
        જો (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                જો (k == 0)
                        પાછા -EIO;
        }

        જો (found)
                *ret_fd = *CMSG_TYPED_DATA(found, પૂર્ણાંક);
        નહીંતર
                *ret_fd = -EBADF;

        પાછા k;
}

પૂર્ણાંક receive_one_fd(પૂર્ણાંક transport_fd, પૂર્ણાંક flags) {
        પૂર્ણાંક fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ખાલી, 0, flags, &fd);
        જો (k == 0)
                પાછા fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        પ્રતિજ્ઞા(k < 0);
        પાછા (પૂર્ણાંક) k;
}

ssize_t next_datagram_size_fd(પૂર્ણાંક fd) {
        ssize_t l;
        પૂર્ણાંક k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ખાલી, 0, MSG_PEEK|MSG_TRUNC);
        જો (l < 0) {
                જો (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        જાઓ fallback;

                પાછા -errno;
        }
        જો (l == 0)
                જાઓ fallback;

        પાછા l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        જો (ioctl(fd, FIONREAD, &k) < 0)
                પાછા -errno;

        પાછા (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

પૂર્ણાંક flush_accept(પૂર્ણાંક fd) {

        પૂર્ણાંક r, b;
        socklen_t l = કદ(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        જો (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                પાછા -errno;

        પ્રતિજ્ઞા(l == કદ(b));
        જો (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                પાછા -ENOTTY;

        માટે (unsigned iteration = 0;; iteration++) {
                પૂર્ણાંક cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                જો (r == -EINTR)
                        ચાલુ;
                જો (r <= 0)
                        પાછા r;

                જો (iteration >= MAX_FLUSH_ITERATIONS)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ખાલી, ખાલી, SOCK_NONBLOCK|SOCK_CLOEXEC);
                જો (cfd < 0) {
                        જો (errno == EAGAIN)
                                પાછા 0;

                        જો (ERRNO_IS_ACCEPT_AGAIN(errno))
                                ચાલુ;

                        પાછા -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(પૂર્ણાંક fd) {
        _cleanup_free_ અક્ષર *buf = ખાલી;
        સંરચના mq_attr attr;
        ssize_t ગણતરી = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        માટે (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                જો (r == -EINTR)
                        ચાલુ;
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા ગણતરી;

                જો (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        જો (mq_getattr(fd, &attr) < 0)
                                પાછા -errno;

                        buf = malloc(attr.mq_msgsize);
                        જો (!buf)
                                પાછા -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ખાલી);
                જો (l < 0) {
                        જો (errno == EINTR)
                                ચાલુ;

                        જો (errno == EAGAIN)
                                પાછા ગણતરી;

                        પાછા -errno;
                }

                ગણતરી += l;
        }
}

સંરચના cmsghdr* cmsg_find(સંરચના msghdr *mh, પૂર્ણાંક level, પૂર્ણાંક type, socklen_t length) {
        સંરચના cmsghdr *cmsg;

        પ્રતિજ્ઞા(mh);

        CMSG_FOREACH(cmsg, mh)
                જો (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        પાછા cmsg;

        પાછા ખાલી;
}

શૂન્ય* cmsg_find_and_copy_data(સંરચના msghdr *mh, પૂર્ણાંક level, પૂર્ણાંક type, શૂન્ય *buf, size_t buf_len) {
        સંરચના cmsghdr *cmsg;

        પ્રતિજ્ઞા(mh);
        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        જો (!cmsg)
                પાછા ખાલી;

        પાછા memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(અચલ સંરચના sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        પ્રતિજ્ઞા(sa->sll_family == AF_PACKET);

        size_t mac_len = કદ(sa->sll_addr);

        જો (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        જો (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        પાછા offsetof(સંરચના sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(અચલ સંરચના sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        પ્રતિજ્ઞા(sa->sun_family == AF_UNIX);

        પાછા offsetof(સંરચના sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, કદ(sa->sun_path)-1) :
                        strnlen(sa->sun_path, કદ(sa->sun_path))+1);
}

size_t sockaddr_len(અચલ સંઘ sockaddr_union *sa) {
        પ્રતિજ્ઞા(sa);

        પસંદગી (sa->sa.sa_family) {
        સ્થિતિ AF_INET:
                પાછા કદ(સંરચના sockaddr_in);
        સ્થિતિ AF_INET6:
                પાછા કદ(સંરચના sockaddr_in6);
        સ્થિતિ AF_UNIX:
                પાછા sockaddr_un_len(&sa->un);
        સ્થિતિ AF_PACKET:
                પાછા sockaddr_ll_len(&sa->ll);
        સ્થિતિ AF_NETLINK:
                પાછા કદ(સંરચના sockaddr_nl);
        સ્થિતિ AF_VSOCK:
                પાછા કદ(સંરચના sockaddr_vm);
        મૂળભૂત:
                assert_not_reached();
        }
}

પૂર્ણાંક socket_ioctl_fd(શૂન્ય) {
        પૂર્ણાંક fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        જો (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        જો (fd < 0)
                પાછા -errno;

        પાછા fd;
}

પૂર્ણાંક sockaddr_un_unlink(અચલ સંરચના sockaddr_un *sa) {
        અચલ અક્ષર *p, * nul;

        પ્રતિજ્ઞા(sa);

        જો (sa->sun_family != AF_UNIX)
                પાછા -EPROTOTYPE;

        જો (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                પાછા 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, કદ(sa->sun_path));
        જો (nul)
                p = sa->sun_path;
        નહીંતર
                p = memdupa_suffix0(sa->sun_path, કદ(sa->sun_path));

        જો (unlink(p) < 0)
                પાછા -errno;

        પાછા 1;
}

પૂર્ણાંક sockaddr_un_set_path(સંરચના sockaddr_un *ret, અચલ અક્ષર *path) {
        size_t l;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        જો (l < 2)
                પાછા -EINVAL;
        જો (!IN_SET(path[0], '/', '@'))
                પાછા -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        જો (l+1 > કદ(ret->sun_path))
                પાછા path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (સંરચના sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        જો (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                પાછા (પૂર્ણાંક) (offsetof(સંરચના sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } નહીંતર {
                પ્રતિજ્ઞા(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                પાછા (પૂર્ણાંક) (offsetof(સંરચના sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

પૂર્ણાંક getsockopt_int(પૂર્ણાંક fd, પૂર્ણાંક level, પૂર્ણાંક optname, પૂર્ણાંક *ret) {
        પૂર્ણાંક v;
        socklen_t sl = કદ(v);

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (getsockopt(fd, level, optname, &v, &sl) < 0)
                પાછા negative_errno();
        જો (sl != કદ(v))
                પાછા -EIO;

        *ret = v;
        પાછા 0;
}

પૂર્ણાંક socket_bind_to_ifname(પૂર્ણાંક fd, અચલ અક્ષર *ifname) {
        પ્રતિજ્ઞા(fd >= 0);

        /* Call with NULL to drop binding */

        પાછા RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

પૂર્ણાંક socket_bind_to_ifindex(પૂર્ણાંક fd, પૂર્ણાંક ifindex) {
        પ્રતિજ્ઞા(fd >= 0);

        જો (ifindex <= 0)
                /* Drop binding */
                પાછા RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ખાલી, 0));

        પાછા setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

પૂર્ણાંક socket_autobind(પૂર્ણાંક fd, અક્ષર **ret_name) {
        _cleanup_free_ અક્ષર *name = ખાલી;
        uint64_t random;
        પૂર્ણાંક r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        પ્રતિજ્ઞા(fd >= 0);

        random = random_u64();

        જો (asprintf(&name, "@%" PRIu64, random) < 0)
                પાછા -ENOMEM;

        સંઘ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < કદ(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        જો (r < 0)
                પાછા r;

        જો (bind(fd, &sa.sa, r) < 0)
                પાછા -errno;

        જો (ret_name)
                *ret_name = TAKE_PTR(name);
        પાછા 0;
}

ssize_t recvmsg_safe(પૂર્ણાંક sockfd, સંરચના msghdr *msg, પૂર્ણાંક flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        પ્રતિજ્ઞા(sockfd >= 0);
        પ્રતિજ્ઞા(msg);

        n = recvmsg(sockfd, msg, flags);
        જો (n < 0)
                પાછા -errno;

        જો (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                પાછા FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        પાછા n;
}

પૂર્ણાંક socket_get_family(પૂર્ણાંક fd) {
        પૂર્ણાંક af;
        socklen_t sl = કદ(af);

        જો (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                પાછા -errno;

        જો (sl != કદ(af))
                પાછા -EINVAL;

        પાછા af;
}

પૂર્ણાંક socket_set_recvpktinfo(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {

        જો (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                જો (af < 0)
                        પાછા af;
        }

        પસંદગી (af) {

        સ્થિતિ AF_INET:
                પાછા setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        સ્થિતિ AF_INET6:
                પાછા setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        સ્થિતિ AF_NETLINK:
                પાછા setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        સ્થિતિ AF_PACKET:
                પાછા setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક socket_set_unicast_if(પૂર્ણાંક fd, પૂર્ણાંક af, પૂર્ણાંક ifi) {
        be32_t ifindex_be = htobe32(ifi);

        જો (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                જો (af < 0)
                        પાછા af;
        }

        પસંદગી (af) {

        સ્થિતિ AF_INET:
                પાછા RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, કદ(ifindex_be)));

        સ્થિતિ AF_INET6:
                પાછા RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, કદ(ifindex_be)));

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક socket_set_option(પૂર્ણાંક fd, પૂર્ણાંક af, પૂર્ણાંક opt_ipv4, પૂર્ણાંક opt_ipv6, પૂર્ણાંક val) {
        જો (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                જો (af < 0)
                        પાછા af;
        }

        પસંદગી (af) {

        સ્થિતિ AF_INET:
                પાછા setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        સ્થિતિ AF_INET6:
                પાછા setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }
}

પૂર્ણાંક socket_get_mtu(પૂર્ણાંક fd, પૂર્ણાંક af, size_t *ret) {
        પૂર્ણાંક mtu, r;

        પ્રતિજ્ઞા(ret);

        જો (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                જો (af < 0)
                        પાછા af;
        }

        પસંદગી (af) {

        સ્થિતિ AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                તોડો;

        સ્થિતિ AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                તોડો;

        મૂળભૂત:
                પાછા -EAFNOSUPPORT;
        }

        જો (r < 0)
                પાછા r;
        જો (mtu <= 0)
                પાછા -EINVAL;

        *ret = (size_t) mtu;
        પાછા 0;
}

સ્થિર પૂર્ણાંક connect_unix_path_simple(પૂર્ણાંક fd, અચલ અક્ષર *path) {
        સંઘ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(path);

        l = strlen(path);
        પ્રતિજ્ઞા(l > 0);
        પ્રતિજ્ઞા(l < કદ(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        પાછા RET_NERRNO(connect(fd, &sa.sa, offsetof(સંરચના sockaddr_un, sun_path) + l + 1));
}

સ્થિર પૂર્ણાંક connect_unix_inode(પૂર્ણાંક fd, પૂર્ણાંક inode_fd) {
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(inode_fd >= 0);

        પાછા connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

પૂર્ણાંક connect_unix_path(પૂર્ણાંક fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક inode_fd = -EBADF;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        જો (!path) {
                જો (dir_fd < 0)
                        પાછા -EISDIR;

                પાછા connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        જો (isempty(path))
                પાછા -EINVAL;

        /* Shortcut for the simple case */
        જો (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(સંરચના sockaddr_un, sun_path))
                પાછા connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        જો (dir_fd == XAT_FDROOT) {
                _cleanup_free_ અક્ષર *j = strjoin("/", path);
                જો (!j)
                        પાછા -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } નહીંતર
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        જો (inode_fd < 0)
                પાછા -errno;

        પાછા connect_unix_inode(fd, inode_fd);
}

પૂર્ણાંક socket_address_parse_unix(SocketAddress *ret_address, અચલ અક્ષર *s) {
        સંરચના sockaddr_un un;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_address);
        પ્રતિજ્ઞા(s);

        જો (!IN_SET(*s, '/', '@'))
                પાછા -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        જો (r < 0)
                પાછા r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        પાછા 0;
}

પૂર્ણાંક socket_address_equal_unix(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        SocketAddress socket_a, socket_b;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = socket_address_parse_unix(&socket_a, a);
        જો (r < 0)
                પાછા r;

        r = socket_address_parse_unix(&socket_b, b);
        જો (r < 0)
                પાછા r;

        પાછા sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

પૂર્ણાંક vsock_parse_port(અચલ અક્ષર *s, unsigned *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        જો (!s)
                પાછા -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        જો (r < 0)
                પાછા r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        જો (u == VMADDR_PORT_ANY)
                પાછા -EINVAL;

        *ret = u;
        પાછા 0;
}

પૂર્ણાંક vsock_parse_cid(અચલ અક્ષર *s, unsigned *ret) {
        પ્રતિજ્ઞા(ret);

        જો (!s)
                પાછા -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        જો (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        નહીંતર જો (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        નહીંતર જો (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        નહીંતર જો (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        નહીંતર
                પાછા safe_atou(s, ret);

        પાછા 0;
}

પૂર્ણાંક socket_address_parse_vsock(SocketAddress *ret_address, અચલ અક્ષર *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ અક્ષર *n = ખાલી;
        અચલ અક્ષર *e, *cid_start;
        unsigned port, cid;
        પૂર્ણાંક type, r;

        પ્રતિજ્ઞા(ret_address);
        પ્રતિજ્ઞા(s);

        જો ((cid_start = startswith(s, "vsock:")))
                type = 0;
        નહીંતર જો ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        નહીંતર જો ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        નહીંતર જો ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        નહીંતર
                પાછા -EPROTO;

        e = strchr(cid_start, ':');
        જો (!e)
                પાછા -EINVAL;

        r = vsock_parse_port(e+1, &port);
        જો (r < 0)
                પાછા r;

        n = strndup(cid_start, e - cid_start);
        જો (!n)
                પાછા -ENOMEM;

        જો (isempty(n))
                cid = VMADDR_CID_ANY;
        નહીંતર {
                r = vsock_parse_cid(n, &cid);
                જો (r < 0)
                        પાછા r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = કદ(સંરચના sockaddr_vm),
        };

        પાછા 0;
}

પૂર્ણાંક netlink_socket_get_multicast_groups(પૂર્ણાંક fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ખાલી;
        socklen_t len = 0, old_len;

        પ્રતિજ્ઞા(fd >= 0);

        જો (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ખાલી, &len) < 0)
                પાછા -errno;

        જો (len == 0)
                જાઓ finalize;

        groups = new0(uint32_t, len);
        જો (!groups)
                પાછા -ENOMEM;

        old_len = len;

        જો (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                પાછા -errno;

        જો (old_len != len)
                પાછા -EIO;

finalize:
        જો (ret_len)
                *ret_len = len;
        જો (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        પાછા 0;
}

પૂર્ણાંક socket_get_cookie(પૂર્ણાંક fd, uint64_t *ret) {
        પ્રતિજ્ઞા(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = કદ(cookie);
        જો (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                પાછા -errno;

        પ્રતિજ્ઞા(cookie_len == કદ(cookie));
        જો (ret)
                *ret = cookie;

        પાછા 0;
}

શૂન્ય cmsg_close_all(સંરચના msghdr *mh) {
        પ્રતિજ્ઞા(mh);

        સંરચના cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                જો (cmsg->cmsg_level != SOL_SOCKET)
                        ચાલુ;

                જો (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, પૂર્ણાંક),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / કદ(પૂર્ણાંક));
                નહીંતર જો (cmsg->cmsg_type == SCM_PIDFD) {
                        પ્રતિજ્ઞા(cmsg->cmsg_len == CMSG_LEN(કદ(પૂર્ણાંક)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, પૂર્ણાંક));
                }
        }
}

પૂર્ણાંક tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        પસંદગી (IPTOS_PREC(tos)) {
        સ્થિતિ IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        સ્થિતિ IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                પાછા TC_PRIO_CONTROL;

        સ્થિતિ IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        સ્થિતિ IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                પાછા TC_PRIO_INTERACTIVE;

        સ્થિતિ IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        સ્થિતિ IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                પાછા TC_PRIO_INTERACTIVE_BULK;

        સ્થિતિ IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                પાછા TC_PRIO_BULK;

        સ્થિતિ IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        મૂળભૂત:
                પાછા TC_PRIO_BESTEFFORT;
        }
}

પૂર્ણાંક socket_xattr_supported(શૂન્ય) {
        પૂર્ણાંક r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        સ્થિર પૂર્ણાંક cached = -1;
        જો (cached >= 0)
                પાછા cached;

        _cleanup_close_ પૂર્ણાંક fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        જો (fd < 0)
                પાછા -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ખાલી, AT_EMPTY_PATH, "user.testxxx", "1");
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                પાછા (cached = false);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to set test xattr on socket: %m");

        પાછા (cached = true);
}
