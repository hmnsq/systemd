/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/if.h>
#అన్తర్భావ <linux/if_arp.h>
#అన్తర్భావ <linux/if_ether.h>
#అన్తర్భావ <linux/if_infiniband.h>
#అన్తర్భావ <linux/pkt_sched.h>
#అన్తర్భావ <mqueue.h>
#అన్తర్భావ <net/if.h>
#అన్తర్భావ <netdb.h>
#అన్తర్భావ <netinet/ip.h>
#అన్తర్భావ <poll.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "format-ifname.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "in-addr-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "sparse-endian.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "sysctl-util.h"
#అన్తర్భావ "user-util.h"
#అన్తర్భావ "xattr-util.h"

#ఒకవేళ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#లేకపోతే
#  define IDN_FLAGS 0
#అయితే_ముగింపు

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి socket_చిరునామా_type_పట్టిక[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_పట్టిక_LOOKUP(socket_చిరునామా_type, పూర్ణాంకం);

పూర్ణాంకం socket_చిరునామా_verify(స్థిరరాశి Socket_చిరునామా *a, bool strict) {
        ప్రతిజ్ఞా(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        ఎంపిక (socket_చిరునామా_family(a)) {

        స్థితి AF_INET:
                ఒకవేళ (a->కొలత != పరిమాణం(నిర్మాణం sockaddr_in))
                        తిరిగి -EINVAL;

                ఒకవేళ (a->sockaddr.లోపల.sin_port == 0)
                        తిరిగి -EINVAL;

                ఒకవేళ (!లోపల_సమితి(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        తిరిగి -EINVAL;

                తిరిగి 0;

        స్థితి AF_INET6:
                ఒకవేళ (a->కొలత != పరిమాణం(నిర్మాణం sockaddr_in6))
                        తిరిగి -EINVAL;

                ఒకవేళ (a->sockaddr.in6.sin6_port == 0)
                        తిరిగి -EINVAL;

                ఒకవేళ (!లోపల_సమితి(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        తిరిగి -EINVAL;

                తిరిగి 0;

        స్థితి AF_UNIX:
                ఒకవేళ (a->కొలత < offsetof(నిర్మాణం sockaddr_un, sun_path))
                        తిరిగి -EINVAL;
                ఒకవేళ (a->కొలత > పరిమాణం(నిర్మాణం sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        తిరిగి -EINVAL;

                ఒకవేళ (a->కొలత > offsetof(నిర్మాణం sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        స్థిరరాశి అక్షరం *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, పరిమాణం(a->sockaddr.un.sun_path));
                        ఒకవేళ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                ఒకవేళ (a->కొలత != offsetof(నిర్మాణం sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        తిరిగి -EINVAL;
                        } లేకపోతే {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                ఒకవేళ (!లోపల_సమితి(a->కొలత, పరిమాణం(a->sockaddr.un.sun_path), పరిమాణం(a->sockaddr.un.sun_path)+1))
                                        తిరిగి -EINVAL;
                        }
                }

                ఒకవేళ (!లోపల_సమితి(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        తిరిగి -EINVAL;

                తిరిగి 0;

        స్థితి AF_NETLINK:

                ఒకవేళ (a->కొలత != పరిమాణం(నిర్మాణం sockaddr_nl))
                        తిరిగి -EINVAL;

                ఒకవేళ (!లోపల_సమితి(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        తిరిగి -EINVAL;

                తిరిగి 0;

        స్థితి AF_VSOCK:
                ఒకవేళ (a->కొలత != పరిమాణం(నిర్మాణం sockaddr_vm))
                        తిరిగి -EINVAL;

                ఒకవేళ (!లోపల_సమితి(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        తిరిగి -EINVAL;

                తిరిగి 0;

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం socket_చిరునామా_print(స్థిరరాశి Socket_చిరునామా *a, అక్షరం **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(ret);

        r = socket_చిరునామా_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (socket_చిరునామా_family(a) == AF_NETLINK) {
                _cleanup_free_ అక్షరం *sfamily = ఖాళీ;

                r = netlink_family_గమ్యం_string_alloc(a->protocol, &sfamily);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                ఒకవేళ (r < 0)
                        తిరిగి -ENOMEM;

                తిరిగి 0;
        }

        తిరిగి sockaddr_pretty(&a->sockaddr.sa, a->కొలత, false, true, ret);
}

bool socket_చిరునామా_can_accept(స్థిరరాశి Socket_చిరునామా *a) {
        ప్రతిజ్ఞా(a);

        తిరిగి
                లోపల_సమితి(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_చిరునామా_equal(స్థిరరాశి Socket_చిరునామా *a, స్థిరరాశి Socket_చిరునామా *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        /* Invalid addresses are unequal to all */
        ఒకవేళ (socket_చిరునామా_verify(a, false) < 0 ||
            socket_చిరునామా_verify(b, false) < 0)
                తిరిగి false;

        ఒకవేళ (a->type != b->type)
                తిరిగి false;

        ఒకవేళ (socket_చిరునామా_family(a) != socket_చిరునామా_family(b))
                తిరిగి false;

        ఎంపిక (socket_చిరునామా_family(a)) {

        స్థితి AF_INET:
                ఒకవేళ (a->sockaddr.లోపల.sin_addr.s_addr != b->sockaddr.లోపల.sin_addr.s_addr)
                        తిరిగి false;

                ఒకవేళ (a->sockaddr.లోపల.sin_port != b->sockaddr.లోపల.sin_port)
                        తిరిగి false;

                ఆపు;

        స్థితి AF_INET6:
                ఒకవేళ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, పరిమాణం(a->sockaddr.in6.sin6_addr)) != 0)
                        తిరిగి false;

                ఒకవేళ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        తిరిగి false;

                ఆపు;

        స్థితి AF_UNIX:
                ఒకవేళ (a->కొలత <= offsetof(నిర్మాణం sockaddr_un, sun_path) ||
                    b->కొలత <= offsetof(నిర్మాణం sockaddr_un, sun_path))
                        తిరిగి false;

                ఒకవేళ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        తిరిగి false;

                ఒకవేళ (a->sockaddr.un.sun_path[0]) {
                        ఒకవేళ (!మార్గం_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                తిరిగి false;
                } లేకపోతే {
                        ఒకవేళ (a->కొలత != b->కొలత)
                                తిరిగి false;

                        ఒకవేళ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->కొలత) != 0)
                                తిరిగి false;
                }

                ఆపు;

        స్థితి AF_NETLINK:
                ఒకవేళ (a->protocol != b->protocol)
                        తిరిగి false;

                ఒకవేళ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        తిరిగి false;

                ఆపు;

        స్థితి AF_VSOCK:
                ఒకవేళ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        తిరిగి false;

                ఒకవేళ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        తిరిగి false;

                ఆపు;

        అప్రమేయ:
                /* Cannot compare, so we assume the addresses are different */
                తిరిగి false;
        }

        తిరిగి true;
}

స్థిరరాశి అక్షరం* socket_చిరునామా_get_మార్గం(స్థిరరాశి Socket_చిరునామా *a) {
        ప్రతిజ్ఞా(a);

        ఒకవేళ (socket_చిరునామా_family(a) != AF_UNIX)
                తిరిగి ఖాళీ;

        ఒకవేళ (a->sockaddr.un.sun_path[0] == 0)
                తిరిగి ఖాళీ;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(పరిమాణం(సంఘం sockaddr_union) >= పరిమాణం(నిర్మాణం sockaddr_un)+1);
        తిరిగి a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(శూన్య) {
        స్థిర పూర్ణాంకం cached = -1;

        ఒకవేళ (cached < 0) {

                ఒకవేళ (access("/proc/net/if_inet6", F_OK) < 0) {

                        ఒకవేళ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                తిరిగి false;
                        }

                        cached = false;
                } లేకపోతే
                        cached = true;
        }

        తిరిగి cached;
}

bool socket_ipv6_is_enabled(శూన్య) {
        _cleanup_free_ అక్షరం *v = ఖాళీ;
        పూర్ణాంకం r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        ఒకవేళ (!socket_ipv6_is_supported())
                తిరిగి false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        ఒకవేళ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                తిరిగి true;
        }

        r = parse_boolean(v);
        ఒకవేళ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                తిరిగి true;
        }

        తిరిగి !r;
}

bool socket_చిరునామా_matches_fd(స్థిరరాశి Socket_చిరునామా *a, పూర్ణాంకం fd) {
        Socket_చిరునామా b;
        socklen_t solen;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(fd >= 0);

        b.కొలత = పరిమాణం(b.sockaddr);
        ఒకవేళ (getsockname(fd, &b.sockaddr.sa, &b.కొలత) < 0)
                తిరిగి false;

        ఒకవేళ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                తిరిగి false;

        solen = పరిమాణం(b.type);
        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                తిరిగి false;

        ఒకవేళ (b.type != a->type)
                తిరిగి false;

        ఒకవేళ (a->protocol != 0)  {
                solen = పరిమాణం(b.protocol);
                ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        తిరిగి false;

                ఒకవేళ (b.protocol != a->protocol)
                        తిరిగి false;
        }

        తిరిగి socket_చిరునామా_equal(a, &b);
}

పూర్ణాంకం sockaddr_పోర్టు(స్థిరరాశి నిర్మాణం sockaddr *_sa, గుర్తులేని *ret_పోర్టు) {
        స్థిరరాశి సంఘం sockaddr_union *sa = (స్థిరరాశి సంఘం sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ప్రతిజ్ఞా(sa);
        ప్రతిజ్ఞా(ret_పోర్టు);

        ఎంపిక (sa->sa.sa_family) {

        స్థితి AF_INET:
                *ret_పోర్టు = be16toh(sa->లోపల.sin_port);
                తిరిగి 0;

        స్థితి AF_INET6:
                *ret_పోర్టు = be16toh(sa->in6.sin6_port);
                తిరిగి 0;

        స్థితి AF_VSOCK:
                *ret_పోర్టు = sa->vm.svm_port;
                తిరిగి 0;

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

స్థిరరాశి సంఘం లోపల_addr_union *sockaddr_లోపల_addr(స్థిరరాశి నిర్మాణం sockaddr *_sa) {
        స్థిరరాశి సంఘం sockaddr_union *sa = (స్థిరరాశి సంఘం sockaddr_union*) _sa;

        ఒకవేళ (!sa)
                తిరిగి ఖాళీ;

        ఎంపిక (sa->sa.sa_family) {

        స్థితి AF_INET:
                తిరిగి (స్థిరరాశి సంఘం లోపల_addr_union*) &sa->లోపల.sin_addr;

        స్థితి AF_INET6:
                తిరిగి (స్థిరరాశి సంఘం లోపల_addr_union*) &sa->in6.sin6_addr;

        అప్రమేయ:
                తిరిగి ఖాళీ;
        }
}

పూర్ణాంకం sockaddr_సమితి_లోపల_addr(
                సంఘం sockaddr_union *u,
                పూర్ణాంకం family,
                స్థిరరాశి సంఘం లోపల_addr_union *a,
                uint16_t port) {

        ప్రతిజ్ఞా(u);
        ప్రతిజ్ఞా(a);

        ఎంపిక (family) {

        స్థితి AF_INET:
                u->లోపల = (నిర్మాణం sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->లోపల,
                        .sin_port = htobe16(port),
                };

                తిరిగి 0;

        స్థితి AF_INET6:
                u->in6 = (నిర్మాణం sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                తిరిగి 0;

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;

        }
}

పూర్ణాంకం sockaddr_pretty(
                స్థిరరాశి నిర్మాణం sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_పోర్టు,
                అక్షరం **ret) {

        సంఘం sockaddr_union *sa = (సంఘం sockaddr_union*) _sa;
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(sa);
        ప్రతిజ్ఞా(salen >= పరిమాణం(sa->sa.sa_family));
        ప్రతిజ్ఞా(ret);

        ఎంపిక (sa->sa.sa_family) {

        స్థితి AF_INET: {
                uint32_t a;

                a = be32toh(sa->లోపల.sin_addr.s_addr);

                ఒకవేళ (include_పోర్టు)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->లోపల.sin_port));
                లేకపోతే
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                ఒకవేళ (r < 0)
                        తిరిగి -ENOMEM;
                ఆపు;
        }

        స్థితి AF_INET6: {
                స్థిర స్థిరరాశి గుర్తులేని అక్షరం ipv4_ఉపసర్గ[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                ఒకవేళ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_ఉపసర్గ, పరిమాణం(ipv4_ఉపసర్గ)) == 0) {
                        స్థిరరాశి uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        ఒకవేళ (include_పోర్టు)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        లేకపోతే
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        ఒకవేళ (r < 0)
                                తిరిగి -ENOMEM;
                } లేకపోతే {
                        స్థిరరాశి అక్షరం *a = IN6_ADDR_గమ్యం_STRING(&sa->in6.sin6_addr);

                        ఒకవేళ (include_పోర్టు) {
                                ఒకవేళ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_నిండిన(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        తిరిగి -ENOMEM;
                        } లేకపోతే {
                                ఒకవేళ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_నిండిన(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                లేకపోతే
                                        p = strdup(a);
                                ఒకవేళ (!p)
                                        తిరిగి -ENOMEM;
                        }
                }

                ఆపు;
        }

        స్థితి AF_UNIX:
                ఒకవేళ (salen <= offsetof(నిర్మాణం sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(నిర్మాణం sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                లేకపోతే {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        అక్షరం *మార్గం = (అక్షరం*) sa->un_బఫరు + offsetof(నిర్మాణం sockaddr_un, sun_path);
                        size_t మార్గం_len = salen - offsetof(నిర్మాణం sockaddr_un, sun_path);

                        ఒకవేళ (మార్గం[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ అక్షరం *e = ఖాళీ;

                                e = cescape_నిడివి(మార్గం + 1, మార్గం_len - 1);
                                ఒకవేళ (!e)
                                        తిరిగి -ENOMEM;

                                p = strjoin("@", e);
                        } లేకపోతే {
                                ఒకవేళ (మార్గం[మార్గం_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        మార్గం_len--;

                                p = cescape_నిడివి(మార్గం, మార్గం_len);
                        }
                }
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                ఆపు;

        స్థితి AF_VSOCK:
                ఒకవేళ (include_పోర్టు) {
                        ఒకవేళ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        లేకపోతే
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } లేకపోతే
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                ఒకవేళ (r < 0)
                        తిరిగి -ENOMEM;
                ఆపు;

        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }

        *ret = p;
        తిరిగి 0;
}

పూర్ణాంకం getpeername_pretty(పూర్ణాంకం fd, bool include_పోర్టు, అక్షరం **ret) {
        సంఘం sockaddr_union sa;
        socklen_t salen = పరిమాణం(sa);
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (getpeername(fd, &sa.sa, &salen) < 0)
                తిరిగి -errno;

        ఒకవేళ (sa.sa.sa_family == AF_UNIX) {
                నిర్మాణం ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        తిరిగి -ENOMEM;

                తిరిగి 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        తిరిగి sockaddr_pretty(&sa.sa, salen, true, include_పోర్టు, ret);
}

పూర్ణాంకం getsockname_pretty(పూర్ణాంకం fd, అక్షరం **ret) {
        సంఘం sockaddr_union sa;
        socklen_t salen = పరిమాణం(sa);

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (getsockname(fd, &sa.sa, &salen) < 0)
                తిరిగి -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        తిరిగి sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

పూర్ణాంకం socknameinfo_pretty(స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t salen, అక్షరం **ret) {
        అక్షరం host[NI_MAXHOST];
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(sa);
        ప్రతిజ్ఞా(salen >= పరిమాణం(sa_family_t));
        ప్రతిజ్ఞా(ret);

        r = getnameinfo(sa, salen, host, పరిమాణం(host), /* service= */ ఖాళీ, /* service_len= */ 0, IDN_FLAGS);
        ఒకవేళ (r != 0) {
                ఒకవేళ (r == EAI_MEMORY)
                        తిరిగి log_oom_debug();
                ఒకవేళ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                లేకపోతే
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                తిరిగి sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        తిరిగి strdup_గమ్యం(ret, host);
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి netlink_family_పట్టిక[] = {
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

DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(netlink_family, పూర్ణాంకం, గరిష్ఠ_పూర్ణాంకం);

bool sockaddr_equal(స్థిరరాశి సంఘం sockaddr_union *a, స్థిరరాశి సంఘం sockaddr_union *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        ఒకవేళ (a->sa.sa_family != b->sa.sa_family)
                తిరిగి false;

        ఒకవేళ (a->sa.sa_family == AF_INET)
                తిరిగి a->లోపల.sin_addr.s_addr == b->లోపల.sin_addr.s_addr;

        ఒకవేళ (a->sa.sa_family == AF_INET6)
                తిరిగి memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, పరిమాణం(a->in6.sin6_addr)) == 0;

        ఒకవేళ (a->sa.sa_family == AF_VSOCK)
                తిరిగి a->vm.svm_cid == b->vm.svm_cid;

        తిరిగి false;
}

పూర్ణాంకం fd_సమితి_sndbuf(పూర్ణాంకం fd, size_t n, bool increase) {
        పూర్ణాంకం r, విలువ;
        socklen_t l = పరిమాణం(విలువ);

        ఒకవేళ (n > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &విలువ, &l);
        ఒకవేళ (r >= 0 && l == పరిమాణం(విలువ) && increase ? (size_t) విలువ >= n*2 : (size_t) విలువ == n*2)
                తిరిగి 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = పరిమాణం(విలువ);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &విలువ, &l);
        ఒకవేళ (r >= 0 && l == పరిమాణం(విలువ) && increase ? (size_t) విలువ >= n*2 : (size_t) విలువ == n*2)
                తిరిగి 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

పూర్ణాంకం fd_సమితి_rcvbuf(పూర్ణాంకం fd, size_t n, bool increase) {
        పూర్ణాంకం r, విలువ;
        socklen_t l = పరిమాణం(విలువ);

        ఒకవేళ (n > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &విలువ, &l);
        ఒకవేళ (r >= 0 && l == పరిమాణం(విలువ) && increase ? (size_t) విలువ >= n*2 : (size_t) విలువ == n*2)
                తిరిగి 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = పరిమాణం(విలువ);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &విలువ, &l);
        ఒకవేళ (r >= 0 && l == పరిమాణం(విలువ) && increase ? (size_t) విలువ >= n*2 : (size_t) విలువ == n*2)
                తిరిగి 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి ip_tos_పట్టిక[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(ip_tos, పూర్ణాంకం, 0xff);

bool ifname_valid_char(అక్షరం a) {
        ఒకవేళ ((గుర్తులేని అక్షరం) a >= 127U)
                తిరిగి false;

        ఒకవేళ ((గుర్తులేని అక్షరం) a <= 32U)
                తిరిగి false;

        ఒకవేళ (లోపల_సమితి(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                తిరిగి false;

        తిరిగి true;
}

bool ifname_valid_నిండిన(స్థిరరాశి అక్షరం *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ప్రతిజ్ఞా(!(flags & ~_IFNAME_VALID_ALL));

        ఒకవేళ (isempty(p))
                తిరిగి false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        ఒకవేళ (parse_ifindex(p) >= 0)
                తిరిగి flags & IFNAME_VALID_NUMERIC;

        ఒకవేళ (flags & IFNAME_VALID_ALTERNATIVE) {
                ఒకవేళ (అక్షరశ్రేణి_పొడవు(p) >= ALTIFNAMSIZ)
                        తిరిగి false;
        } లేకపోతే {
                ఒకవేళ (అక్షరశ్రేణి_పొడవు(p) >= IFNAMSIZ)
                        తిరిగి false;
        }

        ఒకవేళ (dot_or_dot_dot(p))
                తిరిగి false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        ఒకవేళ (!FLAGS_సమితి(flags, IFNAME_VALID_SPECIAL) && STR_లోపల_సమితి(p, "all", "default"))
                తిరిగి false;

        కొరకు (స్థిరరాశి అక్షరం *t = p; *t; t++) {
                ఒకవేళ (!ifname_valid_char(*t))
                        తిరిగి false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        ఒకవేళ (numeric)
                తిరిగి false;

        తిరిగి true;
}

bool చిరునామా_label_valid(స్థిరరాశి అక్షరం *p) {

        సూచిక_MAY_BE_NULL(p);

        ఒకవేళ (isempty(p))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(p) >= IFNAMSIZ)
                తిరిగి false;

        అయితే (*p) {
                ఒకవేళ ((uint8_t) *p >= 127U)
                        తిరిగి false;

                ఒకవేళ ((uint8_t) *p <= 31U)
                        తిరిగి false;
                p++;
        }

        తిరిగి true;
}

పూర్ణాంకం getpeercred(పూర్ణాంకం fd, నిర్మాణం ucred *ucred) {
        socklen_t n = పరిమాణం(నిర్మాణం ucred);
        నిర్మాణం ucred u;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ucred);

        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                తిరిగి -errno;

        ఒకవేళ (n != పరిమాణం(నిర్మాణం ucred))
                తిరిగి -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        ఒకవేళ (!pid_is_valid(u.pid))
                తిరిగి -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        తిరిగి 0;
}

పూర్ణాంకం getpeersec(పూర్ణాంకం fd, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        socklen_t n = 64;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        కొరకు (;;) {
                s = new0(అక్షరం, n+1);
                ఒకవేళ (!s)
                        తిరిగి -ENOMEM;

                ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        ఆపు;
                }

                ఒకవేళ (errno != ERANGE)
                        తిరిగి -errno;

                s = mfree(s);
        }

        ఒకవేళ (isempty(s))
                తిరిగి -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        తిరిగి 0;
}

పూర్ణాంకం getpeergroups(పూర్ణాంకం fd, gid_t **ret) {
        socklen_t n = పరిమాణం(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ఖాళీ;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        పూర్ణాంకం ngroups_max = sysconf_ngroups_max();
        ఒకవేళ (ngroups_max > 0)
                n = MAX(n, పరిమాణం(gid_t) * (socklen_t) ngroups_max);

        కొరకు (;;) {
                d = జ్ఞాపకం_ఇవ్వు(n);
                ఒకవేళ (!d)
                        తిరిగి -ENOMEM;

                ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        ఆపు;

                ఒకవేళ (errno != ERANGE)
                        తిరిగి -errno;

                d = mfree(d);
        }

        assert_se(n % పరిమాణం(gid_t) == 0);
        n /= పరిమాణం(gid_t);

        ఒకవేళ (n > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -E2BIG;

        *ret = TAKE_PTR(d);

        తిరిగి (పూర్ణాంకం) n;
}

పూర్ణాంకం getpeerpidfd(పూర్ణాంకం fd) {
        socklen_t n = పరిమాణం(పూర్ణాంకం);
        పూర్ణాంకం pidfd = -EBADF;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                తిరిగి -errno;

        ఒకవేళ (n != పరిమాణం(పూర్ణాంకం))
                తిరిగి -EIO;

        తిరిగి pidfd;
}

పూర్ణాంకం getpeerpidref(పూర్ణాంకం fd, PidRef *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        పూర్ణాంకం pidfd = getpeerpidfd(fd);
        ఒకవేళ (pidfd < 0) {
                ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        తిరిగి pidfd;

                నిర్మాణం ucred ucred;
                r = getpeercred(fd, &ucred);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి pidref_సమితి_pid(ret, ucred.pid);
        }

        తిరిగి pidref_సమితి_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                పూర్ణాంకం transport_fd,
                పూర్ణాంకం fd,
                స్థిరరాశి నిర్మాణం iovec *iov, size_t iovlen,
                స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t len,
                పూర్ణాంకం flags) {

        CMSG_బఫరు_TYPE(CMSG_SPACE(పరిమాణం(పూర్ణాంకం))) control = {};
        నిర్మాణం msghdr mh = {
                .msg_name = (నిర్మాణం sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (నిర్మాణం iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ప్రతిజ్ఞా(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        ఒకవేళ (fd < 0 && !iov)
                తిరిగి -EINVAL;

        ఒకవేళ (fd >= 0) {
                నిర్మాణం cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = పరిమాణం(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(పరిమాణం(పూర్ణాంకం));
                జ్ఞాపకం_నకలు(CMSG_DATA(cmsg), &fd, పరిమాణం(పూర్ణాంకం));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        ఒకవేళ (k < 0)
                తిరిగి (ssize_t) -errno;

        తిరిగి k;
}

పూర్ణాంకం send_one_fd_sa(
                పూర్ణాంకం transport_fd,
                పూర్ణాంకం fd,
                స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t len,
                పూర్ణాంకం flags) {

        ప్రతిజ్ఞా(fd >= 0);

        తిరిగి (పూర్ణాంకం) send_one_fd_iov_sa(transport_fd, fd, ఖాళీ, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                పూర్ణాంకం transport_fd,
                నిర్మాణం iovec *iov, size_t iovlen,
                పూర్ణాంకం flags,
                పూర్ణాంకం *ret_fd) {

        CMSG_బఫరు_TYPE(CMSG_SPACE(పరిమాణం(పూర్ణాంకం))) control;
        నిర్మాణం msghdr mh = {
                .msg_control = &control,
                .msg_controllen = పరిమాణం(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        నిర్మాణం cmsghdr *దొరికింది;
        ssize_t k;

        ప్రతిజ్ఞా(transport_fd >= 0);
        ప్రతిజ్ఞా(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_సురక్షిత(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        ఒకవేళ (k < 0)
                తిరిగి k;

        దొరికింది = cmsg_కనుగొను(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(పరిమాణం(పూర్ణాంకం)));
        ఒకవేళ (!దొరికింది) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                ఒకవేళ (k == 0)
                        తిరిగి -EIO;
        }

        ఒకవేళ (దొరికింది)
                *ret_fd = *CMSG_TYPED_దత్తాంశం(దొరికింది, పూర్ణాంకం);
        లేకపోతే
                *ret_fd = -EBADF;

        తిరిగి k;
}

పూర్ణాంకం receive_one_fd(పూర్ణాంకం transport_fd, పూర్ణాంకం flags) {
        పూర్ణాంకం fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ఖాళీ, 0, flags, &fd);
        ఒకవేళ (k == 0)
                తిరిగి fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ప్రతిజ్ఞా(k < 0);
        తిరిగి (పూర్ణాంకం) k;
}

ssize_t తదుపరి_datagram_కొలత_fd(పూర్ణాంకం fd) {
        ssize_t l;
        పూర్ణాంకం k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ఖాళీ, 0, MSG_PEEK|MSG_TRUNC);
        ఒకవేళ (l < 0) {
                ఒకవేళ (లోపల_సమితి(errno, EOPNOTSUPP, EFAULT))
                        వెళ్ళు fallback;

                తిరిగి -errno;
        }
        ఒకవేళ (l == 0)
                వెళ్ళు fallback;

        తిరిగి l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        ఒకవేళ (ioctl(fd, FIONREAD, &k) < 0)
                తిరిగి -errno;

        తిరిగి (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#నిర్వచించు MAX_FLUSH_ITERATIONS 1024

పూర్ణాంకం flush_accept(పూర్ణాంకం fd) {

        పూర్ణాంకం r, b;
        socklen_t l = పరిమాణం(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                తిరిగి -errno;

        ప్రతిజ్ఞా(l == పరిమాణం(b));
        ఒకవేళ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                తిరిగి -ENOTTY;

        కొరకు (గుర్తులేని iteration = 0;; iteration++) {
                పూర్ణాంకం cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ఒకవేళ (r == -EINTR)
                        కొనసాగు;
                ఒకవేళ (r <= 0)
                        తిరిగి r;

                ఒకవేళ (iteration >= MAX_FLUSH_ITERATIONS)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ఖాళీ, ఖాళీ, SOCK_NONBLOCK|SOCK_CLOEXEC);
                ఒకవేళ (cfd < 0) {
                        ఒకవేళ (errno == EAGAIN)
                                తిరిగి 0;

                        ఒకవేళ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                కొనసాగు;

                        తిరిగి -errno;
                }

                సురక్షిత_close(cfd);
        }
}

ssize_t flush_mqueue(పూర్ణాంకం fd) {
        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        నిర్మాణం mq_attr attr;
        ssize_t లెక్క = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        కొరకు (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                ఒకవేళ (r == -EINTR)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి లెక్క;

                ఒకవేళ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        ఒకవేళ (mq_getattr(fd, &attr) < 0)
                                తిరిగి -errno;

                        buf = జ్ఞాపకం_ఇవ్వు(attr.mq_msgsize);
                        ఒకవేళ (!buf)
                                తిరిగి -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ఖాళీ);
                ఒకవేళ (l < 0) {
                        ఒకవేళ (errno == EINTR)
                                కొనసాగు;

                        ఒకవేళ (errno == EAGAIN)
                                తిరిగి లెక్క;

                        తిరిగి -errno;
                }

                లెక్క += l;
        }
}

నిర్మాణం cmsghdr* cmsg_కనుగొను(నిర్మాణం msghdr *mh, పూర్ణాంకం level, పూర్ణాంకం type, socklen_t length) {
        నిర్మాణం cmsghdr *cmsg;

        ప్రతిజ్ఞా(mh);

        CMSG_FOREACH(cmsg, mh)
                ఒకవేళ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        తిరిగి cmsg;

        తిరిగి ఖాళీ;
}

శూన్య* cmsg_కనుగొను_and_copy_దత్తాంశం(నిర్మాణం msghdr *mh, పూర్ణాంకం level, పూర్ణాంకం type, శూన్య *buf, size_t buf_len) {
        నిర్మాణం cmsghdr *cmsg;

        ప్రతిజ్ఞా(mh);
        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_కనుగొను(mh, level, type, CMSG_LEN(buf_len));
        ఒకవేళ (!cmsg)
                తిరిగి ఖాళీ;

        తిరిగి memcpy_సురక్షిత(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(స్థిరరాశి నిర్మాణం sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ప్రతిజ్ఞా(sa->sll_family == AF_PACKET);

        size_t mac_len = పరిమాణం(sa->sll_addr);

        ఒకవేళ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        ఒకవేళ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        తిరిగి offsetof(నిర్మాణం sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(స్థిరరాశి నిర్మాణం sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ప్రతిజ్ఞా(sa->sun_family == AF_UNIX);

        తిరిగి offsetof(నిర్మాణం sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, పరిమాణం(sa->sun_path)-1) :
                        strnlen(sa->sun_path, పరిమాణం(sa->sun_path))+1);
}

size_t sockaddr_len(స్థిరరాశి సంఘం sockaddr_union *sa) {
        ప్రతిజ్ఞా(sa);

        ఎంపిక (sa->sa.sa_family) {
        స్థితి AF_INET:
                తిరిగి పరిమాణం(నిర్మాణం sockaddr_in);
        స్థితి AF_INET6:
                తిరిగి పరిమాణం(నిర్మాణం sockaddr_in6);
        స్థితి AF_UNIX:
                తిరిగి sockaddr_un_len(&sa->un);
        స్థితి AF_PACKET:
                తిరిగి sockaddr_ll_len(&sa->ll);
        స్థితి AF_NETLINK:
                తిరిగి పరిమాణం(నిర్మాణం sockaddr_nl);
        స్థితి AF_VSOCK:
                తిరిగి పరిమాణం(నిర్మాణం sockaddr_vm);
        అప్రమేయ:
                assert_not_reached();
        }
}

పూర్ణాంకం socket_ioctl_fd(శూన్య) {
        పూర్ణాంకం fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        ఒకవేళ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        తిరిగి fd;
}

పూర్ణాంకం sockaddr_un_unlink(స్థిరరాశి నిర్మాణం sockaddr_un *sa) {
        స్థిరరాశి అక్షరం *p, * nul;

        ప్రతిజ్ఞా(sa);

        ఒకవేళ (sa->sun_family != AF_UNIX)
                తిరిగి -EPROTOTYPE;

        ఒకవేళ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                తిరిగి 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, పరిమాణం(sa->sun_path));
        ఒకవేళ (nul)
                p = sa->sun_path;
        లేకపోతే
                p = memdupa_suffix0(sa->sun_path, పరిమాణం(sa->sun_path));

        ఒకవేళ (unlink(p) < 0)
                తిరిగి -errno;

        తిరిగి 1;
}

పూర్ణాంకం sockaddr_un_సమితి_మార్గం(నిర్మాణం sockaddr_un *ret, స్థిరరాశి అక్షరం *మార్గం) {
        size_t l;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(మార్గం);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = అక్షరశ్రేణి_పొడవు(మార్గం);
        ఒకవేళ (l < 2)
                తిరిగి -EINVAL;
        ఒకవేళ (!లోపల_సమితి(మార్గం[0], '/', '@'))
                తిరిగి -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        ఒకవేళ (l+1 > పరిమాణం(ret->sun_path))
                తిరిగి మార్గం[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (నిర్మాణం sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        ఒకవేళ (మార్గం[0] == '@') {
                /* Abstract namespace socket */
                జ్ఞాపకం_నకలు(ret->sun_path + 1, మార్గం + 1, l); /* copy *with* trailing NUL byte */
                తిరిగి (పూర్ణాంకం) (offsetof(నిర్మాణం sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } లేకపోతే {
                ప్రతిజ్ఞా(మార్గం[0] == '/');

                /* File system socket */
                జ్ఞాపకం_నకలు(ret->sun_path, మార్గం, l + 1); /* copy *with* trailing NUL byte */
                తిరిగి (పూర్ణాంకం) (offsetof(నిర్మాణం sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

పూర్ణాంకం getsockopt_int(పూర్ణాంకం fd, పూర్ణాంకం level, పూర్ణాంకం optname, పూర్ణాంకం *ret) {
        పూర్ణాంకం v;
        socklen_t sl = పరిమాణం(v);

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (getsockopt(fd, level, optname, &v, &sl) < 0)
                తిరిగి negative_errno();
        ఒకవేళ (sl != పరిమాణం(v))
                తిరిగి -EIO;

        *ret = v;
        తిరిగి 0;
}

పూర్ణాంకం socket_bind_గమ్యం_ifname(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *ifname) {
        ప్రతిజ్ఞా(fd >= 0);

        /* Call with NULL to drop binding */

        తిరిగి RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

పూర్ణాంకం socket_bind_గమ్యం_ifindex(పూర్ణాంకం fd, పూర్ణాంకం ifindex) {
        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (ifindex <= 0)
                /* Drop binding */
                తిరిగి RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ఖాళీ, 0));

        తిరిగి setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

పూర్ణాంకం socket_autobind(పూర్ణాంకం fd, అక్షరం **ret_name) {
        _cleanup_free_ అక్షరం *name = ఖాళీ;
        uint64_t random;
        పూర్ణాంకం r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ప్రతిజ్ఞా(fd >= 0);

        random = random_u64();

        ఒకవేళ (asprintf(&name, "@%" PRIu64, random) < 0)
                తిరిగి -ENOMEM;

        సంఘం sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < పరిమాణం(sa.un.sun_path));

        r = sockaddr_un_సమితి_మార్గం(&sa.un, name);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (bind(fd, &sa.sa, r) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_name)
                *ret_name = TAKE_PTR(name);
        తిరిగి 0;
}

ssize_t recvmsg_సురక్షిత(పూర్ణాంకం sockfd, నిర్మాణం msghdr *msg, పూర్ణాంకం flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ప్రతిజ్ఞా(sockfd >= 0);
        ప్రతిజ్ఞా(msg);

        n = recvmsg(sockfd, msg, flags);
        ఒకవేళ (n < 0)
                తిరిగి -errno;

        ఒకవేళ (FLAGS_సమితి(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_సమితి(flags, MSG_PEEK) && FLAGS_సమితి(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                తిరిగి FLAGS_సమితి(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        తిరిగి n;
}

పూర్ణాంకం socket_get_family(పూర్ణాంకం fd) {
        పూర్ణాంకం af;
        socklen_t sl = పరిమాణం(af);

        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                తిరిగి -errno;

        ఒకవేళ (sl != పరిమాణం(af))
                తిరిగి -EINVAL;

        తిరిగి af;
}

పూర్ణాంకం socket_సమితి_recvpktinfo(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {

        ఒకవేళ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ఒకవేళ (af < 0)
                        తిరిగి af;
        }

        ఎంపిక (af) {

        స్థితి AF_INET:
                తిరిగి setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        స్థితి AF_INET6:
                తిరిగి setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        స్థితి AF_NETLINK:
                తిరిగి setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        స్థితి AF_PACKET:
                తిరిగి setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం socket_సమితి_unicast_if(పూర్ణాంకం fd, పూర్ణాంకం af, పూర్ణాంకం ifi) {
        be32_t ifindex_be = htobe32(ifi);

        ఒకవేళ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ఒకవేళ (af < 0)
                        తిరిగి af;
        }

        ఎంపిక (af) {

        స్థితి AF_INET:
                తిరిగి RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, పరిమాణం(ifindex_be)));

        స్థితి AF_INET6:
                తిరిగి RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, పరిమాణం(ifindex_be)));

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం socket_సమితి_option(పూర్ణాంకం fd, పూర్ణాంకం af, పూర్ణాంకం opt_ipv4, పూర్ణాంకం opt_ipv6, పూర్ణాంకం val) {
        ఒకవేళ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ఒకవేళ (af < 0)
                        తిరిగి af;
        }

        ఎంపిక (af) {

        స్థితి AF_INET:
                తిరిగి setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        స్థితి AF_INET6:
                తిరిగి setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }
}

పూర్ణాంకం socket_get_mtu(పూర్ణాంకం fd, పూర్ణాంకం af, size_t *ret) {
        పూర్ణాంకం mtu, r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                ఒకవేళ (af < 0)
                        తిరిగి af;
        }

        ఎంపిక (af) {

        స్థితి AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                ఆపు;

        స్థితి AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                ఆపు;

        అప్రమేయ:
                తిరిగి -EAFNOSUPPORT;
        }

        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (mtu <= 0)
                తిరిగి -EINVAL;

        *ret = (size_t) mtu;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం connect_unix_మార్గం_simple(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం) {
        సంఘం sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(మార్గం);

        l = అక్షరశ్రేణి_పొడవు(మార్గం);
        ప్రతిజ్ఞా(l > 0);
        ప్రతిజ్ఞా(l < పరిమాణం(sa.un.sun_path));

        జ్ఞాపకం_నకలు(sa.un.sun_path, మార్గం, l + 1);
        తిరిగి RET_NERRNO(connect(fd, &sa.sa, offsetof(నిర్మాణం sockaddr_un, sun_path) + l + 1));
}

స్థిర పూర్ణాంకం connect_unix_inode(పూర్ణాంకం fd, పూర్ణాంకం inode_fd) {
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(inode_fd >= 0);

        తిరిగి connect_unix_మార్గం_simple(fd, FORMAT_PROC_FD_మార్గం(inode_fd));
}

పూర్ణాంకం connect_unix_మార్గం(పూర్ణాంకం fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం inode_fd = -EBADF;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        ఒకవేళ (!మార్గం) {
                ఒకవేళ (dir_fd < 0)
                        తిరిగి -EISDIR;

                తిరిగి connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        ఒకవేళ (isempty(మార్గం))
                తిరిగి -EINVAL;

        /* Shortcut for the simple case */
        ఒకవేళ (dir_fd == AT_FDCWD && అక్షరశ్రేణి_పొడవు(మార్గం) < sizeof_క్షేత్రం(నిర్మాణం sockaddr_un, sun_path))
                తిరిగి connect_unix_మార్గం_simple(fd, మార్గం);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        ఒకవేళ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ అక్షరం *j = strjoin("/", మార్గం);
                ఒకవేళ (!j)
                        తిరిగి -ENOMEM;

                inode_fd = open(j, O_మార్గం|O_CLOEXEC);
        } లేకపోతే
                inode_fd = openat(dir_fd, మార్గం, O_మార్గం|O_CLOEXEC);
        ఒకవేళ (inode_fd < 0)
                తిరిగి -errno;

        తిరిగి connect_unix_inode(fd, inode_fd);
}

పూర్ణాంకం socket_చిరునామా_parse_unix(Socket_చిరునామా *ret_చిరునామా, స్థిరరాశి అక్షరం *s) {
        నిర్మాణం sockaddr_un un;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_చిరునామా);
        ప్రతిజ్ఞా(s);

        ఒకవేళ (!లోపల_సమితి(*s, '/', '@'))
                తిరిగి -EPROTO;

        r = sockaddr_un_సమితి_మార్గం(&un, s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret_చిరునామా = (Socket_చిరునామా) {
                .sockaddr.un = un,
                .కొలత = r,
        };

        తిరిగి 0;
}

పూర్ణాంకం socket_చిరునామా_equal_unix(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        Socket_చిరునామా socket_a, socket_b;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = socket_చిరునామా_parse_unix(&socket_a, a);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = socket_చిరునామా_parse_unix(&socket_b, b);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

పూర్ణాంకం vsock_parse_పోర్టు(స్థిరరాశి అక్షరం *s, గుర్తులేని *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!s)
                తిరిగి -EINVAL;

        గుర్తులేని u;
        r = సురక్షిత_atou(s, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        ఒకవేళ (u == VMADDR_PORT_ANY)
                తిరిగి -EINVAL;

        *ret = u;
        తిరిగి 0;
}

పూర్ణాంకం vsock_parse_cid(స్థిరరాశి అక్షరం *s, గుర్తులేని *ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!s)
                తిరిగి -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        ఒకవేళ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        లేకపోతే ఒకవేళ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        లేకపోతే ఒకవేళ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        లేకపోతే ఒకవేళ (STR_లోపల_సమితి(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        లేకపోతే
                తిరిగి సురక్షిత_atou(s, ret);

        తిరిగి 0;
}

పూర్ణాంకం socket_చిరునామా_parse_vsock(Socket_చిరునామా *ret_చిరునామా, స్థిరరాశి అక్షరం *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ అక్షరం *n = ఖాళీ;
        స్థిరరాశి అక్షరం *e, *cid_ప్రారంభం;
        గుర్తులేని port, cid;
        పూర్ణాంకం type, r;

        ప్రతిజ్ఞా(ret_చిరునామా);
        ప్రతిజ్ఞా(s);

        ఒకవేళ ((cid_ప్రారంభం = startswith(s, "vsock:")))
                type = 0;
        లేకపోతే ఒకవేళ ((cid_ప్రారంభం = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        లేకపోతే ఒకవేళ ((cid_ప్రారంభం = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        లేకపోతే ఒకవేళ ((cid_ప్రారంభం = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        లేకపోతే
                తిరిగి -EPROTO;

        e = strchr(cid_ప్రారంభం, ':');
        ఒకవేళ (!e)
                తిరిగి -EINVAL;

        r = vsock_parse_పోర్టు(e+1, &port);
        ఒకవేళ (r < 0)
                తిరిగి r;

        n = strndup(cid_ప్రారంభం, e - cid_ప్రారంభం);
        ఒకవేళ (!n)
                తిరిగి -ENOMEM;

        ఒకవేళ (isempty(n))
                cid = VMADDR_CID_ANY;
        లేకపోతే {
                r = vsock_parse_cid(n, &cid);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret_చిరునామా = (Socket_చిరునామా) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .కొలత = పరిమాణం(నిర్మాణం sockaddr_vm),
        };

        తిరిగి 0;
}

పూర్ణాంకం netlink_socket_get_multicast_groups(పూర్ణాంకం fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ఖాళీ;
        socklen_t len = 0, old_len;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ఖాళీ, &len) < 0)
                తిరిగి -errno;

        ఒకవేళ (len == 0)
                వెళ్ళు finalize;

        groups = new0(uint32_t, len);
        ఒకవేళ (!groups)
                తిరిగి -ENOMEM;

        old_len = len;

        ఒకవేళ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                తిరిగి -errno;

        ఒకవేళ (old_len != len)
                తిరిగి -EIO;

finalize:
        ఒకవేళ (ret_len)
                *ret_len = len;
        ఒకవేళ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        తిరిగి 0;
}

పూర్ణాంకం socket_get_cookie(పూర్ణాంకం fd, uint64_t *ret) {
        ప్రతిజ్ఞా(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = పరిమాణం(cookie);
        ఒకవేళ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                తిరిగి -errno;

        ప్రతిజ్ఞా(cookie_len == పరిమాణం(cookie));
        ఒకవేళ (ret)
                *ret = cookie;

        తిరిగి 0;
}

శూన్య cmsg_close_all(నిర్మాణం msghdr *mh) {
        ప్రతిజ్ఞా(mh);

        నిర్మాణం cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                ఒకవేళ (cmsg->cmsg_level != SOL_SOCKET)
                        కొనసాగు;

                ఒకవేళ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_దత్తాంశం(cmsg, పూర్ణాంకం),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / పరిమాణం(పూర్ణాంకం));
                లేకపోతే ఒకవేళ (cmsg->cmsg_type == SCM_PIDFD) {
                        ప్రతిజ్ఞా(cmsg->cmsg_len == CMSG_LEN(పరిమాణం(పూర్ణాంకం)));
                        సురక్షిత_close(*CMSG_TYPED_దత్తాంశం(cmsg, పూర్ణాంకం));
                }
        }
}

పూర్ణాంకం tos_గమ్యం_ప్రాధాన్యత(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        ఎంపిక (IPTOS_PREC(tos)) {
        స్థితి IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        స్థితి IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                తిరిగి TC_PRIO_CONTROL;

        స్థితి IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        స్థితి IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                తిరిగి TC_PRIO_INTERACTIVE;

        స్థితి IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        స్థితి IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                తిరిగి TC_PRIO_INTERACTIVE_BULK;

        స్థితి IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                తిరిగి TC_PRIO_BULK;

        స్థితి IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        అప్రమేయ:
                తిరిగి TC_PRIO_BESTEFFORT;
        }
}

పూర్ణాంకం socket_xattr_supported(శూన్య) {
        పూర్ణాంకం r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        స్థిర పూర్ణాంకం cached = -1;
        ఒకవేళ (cached >= 0)
                తిరిగి cached;

        _cleanup_close_ పూర్ణాంకం fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ఖాళీ, AT_ఖాళీలు_మార్గం, "user.testxxx", "1");
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                తిరిగి (cached = false);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to set test xattr on socket: %m");

        తిరిగి (cached = true);
}
