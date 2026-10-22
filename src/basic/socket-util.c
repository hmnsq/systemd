/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/if.h>
#অন্তর্ভাব <linux/if_arp.h>
#অন্তর্ভাব <linux/if_ether.h>
#অন্তর্ভাব <linux/if_infiniband.h>
#অন্তর্ভাব <linux/pkt_sched.h>
#অন্তর্ভাব <mqueue.h>
#অন্তর্ভাব <net/if.h>
#অন্তর্ভাব <netdb.h>
#অন্তর্ভাব <netinet/ip.h>
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "format-ifname.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "in-addr-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "random-util.h"
#অন্তর্ভাব "socket-util.h"
#অন্তর্ভাব "sparse-endian.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "sysctl-util.h"
#অন্তর্ভাব "user-util.h"
#অন্তর্ভাব "xattr-util.h"

#যদি ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#অন্যথা
#  define IDN_FLAGS 0
#endif

স্থির ধ্রুবক অক্ষর* ধ্রুবক socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, পূর্ণসংখ্যা);

পূর্ণসংখ্যা socket_address_verify(ধ্রুবক SocketAddress *a, bool strict) {
        প্রতিজ্ঞা(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        নির্বাচন (socket_address_family(a)) {

        ক্ষেত্র AF_INET:
                যদি (a->আকার_2 != আকার(কাঠামো sockaddr_in))
                        ফেরত -EINVAL;

                যদি (a->sockaddr.in.sin_port == 0)
                        ফেরত -EINVAL;

                যদি (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ফেরত -EINVAL;

                ফেরত 0;

        ক্ষেত্র AF_INET6:
                যদি (a->আকার_2 != আকার(কাঠামো sockaddr_in6))
                        ফেরত -EINVAL;

                যদি (a->sockaddr.in6.sin6_port == 0)
                        ফেরত -EINVAL;

                যদি (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ফেরত -EINVAL;

                ফেরত 0;

        ক্ষেত্র AF_UNIX:
                যদি (a->আকার_2 < offsetof(কাঠামো sockaddr_un, sun_path))
                        ফেরত -EINVAL;
                যদি (a->আকার_2 > আকার(কাঠামো sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        ফেরত -EINVAL;

                যদি (a->আকার_2 > offsetof(কাঠামো sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ধ্রুবক অক্ষর *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, আকার(a->sockaddr.un.sun_path));
                        যদি (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                যদি (a->আকার_2 != offsetof(কাঠামো sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        ফেরত -EINVAL;
                        } অন্যথা {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                যদি (!IN_SET(a->আকার_2, আকার(a->sockaddr.un.sun_path), আকার(a->sockaddr.un.sun_path)+1))
                                        ফেরত -EINVAL;
                        }
                }

                যদি (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        ফেরত -EINVAL;

                ফেরত 0;

        ক্ষেত্র AF_NETLINK:

                যদি (a->আকার_2 != আকার(কাঠামো sockaddr_nl))
                        ফেরত -EINVAL;

                যদি (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        ফেরত -EINVAL;

                ফেরত 0;

        ক্ষেত্র AF_VSOCK:
                যদি (a->আকার_2 != আকার(কাঠামো sockaddr_vm))
                        ফেরত -EINVAL;

                যদি (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        ফেরত -EINVAL;

                ফেরত 0;

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা socket_address_print(ধ্রুবক SocketAddress *a, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        যদি (r < 0)
                ফেরত r;

        যদি (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ অক্ষর *sfamily = ফাঁকা;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                যদি (r < 0)
                        ফেরত r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                যদি (r < 0)
                        ফেরত -ENOMEM;

                ফেরত 0;
        }

        ফেরত sockaddr_pretty(&a->sockaddr.sa, a->আকার_2, false, true, ret);
}

bool socket_address_can_accept(ধ্রুবক SocketAddress *a) {
        প্রতিজ্ঞা(a);

        ফেরত
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ধ্রুবক SocketAddress *a, ধ্রুবক SocketAddress *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        /* Invalid addresses are unequal to all */
        যদি (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                ফেরত false;

        যদি (a->type != b->type)
                ফেরত false;

        যদি (socket_address_family(a) != socket_address_family(b))
                ফেরত false;

        নির্বাচন (socket_address_family(a)) {

        ক্ষেত্র AF_INET:
                যদি (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        ফেরত false;

                যদি (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        ফেরত false;

                বিরতি;

        ক্ষেত্র AF_INET6:
                যদি (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, আকার(a->sockaddr.in6.sin6_addr)) != 0)
                        ফেরত false;

                যদি (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        ফেরত false;

                বিরতি;

        ক্ষেত্র AF_UNIX:
                যদি (a->আকার_2 <= offsetof(কাঠামো sockaddr_un, sun_path) ||
                    b->আকার_2 <= offsetof(কাঠামো sockaddr_un, sun_path))
                        ফেরত false;

                যদি ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        ফেরত false;

                যদি (a->sockaddr.un.sun_path[0]) {
                        যদি (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                ফেরত false;
                } অন্যথা {
                        যদি (a->আকার_2 != b->আকার_2)
                                ফেরত false;

                        যদি (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->আকার_2) != 0)
                                ফেরত false;
                }

                বিরতি;

        ক্ষেত্র AF_NETLINK:
                যদি (a->protocol != b->protocol)
                        ফেরত false;

                যদি (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        ফেরত false;

                বিরতি;

        ক্ষেত্র AF_VSOCK:
                যদি (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        ফেরত false;

                যদি (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        ফেরত false;

                বিরতি;

        পূর্বনির্ধারিত:
                /* Cannot compare, so we assume the addresses are different */
                ফেরত false;
        }

        ফেরত true;
}

ধ্রুবক অক্ষর* socket_address_get_path(ধ্রুবক SocketAddress *a) {
        প্রতিজ্ঞা(a);

        যদি (socket_address_family(a) != AF_UNIX)
                ফেরত ফাঁকা;

        যদি (a->sockaddr.un.sun_path[0] == 0)
                ফেরত ফাঁকা;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(আকার(সংঘ sockaddr_union) >= আকার(কাঠামো sockaddr_un)+1);
        ফেরত a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(শূন্য) {
        স্থির পূর্ণসংখ্যা cached = -1;

        যদি (cached < 0) {

                যদি (access("/proc/net/if_inet6", F_OK) < 0) {

                        যদি (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                ফেরত false;
                        }

                        cached = false;
                } অন্যথা
                        cached = true;
        }

        ফেরত cached;
}

bool socket_ipv6_is_enabled(শূন্য) {
        _cleanup_free_ অক্ষর *v = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        যদি (!socket_ipv6_is_supported())
                ফেরত false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        যদি (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ফেরত true;
        }

        r = parse_boolean(v);
        যদি (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                ফেরত true;
        }

        ফেরত !r;
}

bool socket_address_matches_fd(ধ্রুবক SocketAddress *a, পূর্ণসংখ্যা fd) {
        SocketAddress b;
        socklen_t solen;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(fd >= 0);

        b.আকার_2 = আকার(b.sockaddr);
        যদি (getsockname(fd, &b.sockaddr.sa, &b.আকার_2) < 0)
                ফেরত false;

        যদি (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                ফেরত false;

        solen = আকার(b.type);
        যদি (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                ফেরত false;

        যদি (b.type != a->type)
                ফেরত false;

        যদি (a->protocol != 0)  {
                solen = আকার(b.protocol);
                যদি (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        ফেরত false;

                যদি (b.protocol != a->protocol)
                        ফেরত false;
        }

        ফেরত socket_address_equal(a, &b);
}

পূর্ণসংখ্যা sockaddr_port(ধ্রুবক কাঠামো sockaddr *_sa, unsigned *ret_port) {
        ধ্রুবক সংঘ sockaddr_union *sa = (ধ্রুবক সংঘ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        প্রতিজ্ঞা(sa);
        প্রতিজ্ঞা(ret_port);

        নির্বাচন (sa->sa.sa_family) {

        ক্ষেত্র AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                ফেরত 0;

        ক্ষেত্র AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                ফেরত 0;

        ক্ষেত্র AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                ফেরত 0;

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

ধ্রুবক সংঘ in_addr_union *sockaddr_in_addr(ধ্রুবক কাঠামো sockaddr *_sa) {
        ধ্রুবক সংঘ sockaddr_union *sa = (ধ্রুবক সংঘ sockaddr_union*) _sa;

        যদি (!sa)
                ফেরত ফাঁকা;

        নির্বাচন (sa->sa.sa_family) {

        ক্ষেত্র AF_INET:
                ফেরত (ধ্রুবক সংঘ in_addr_union*) &sa->in.sin_addr;

        ক্ষেত্র AF_INET6:
                ফেরত (ধ্রুবক সংঘ in_addr_union*) &sa->in6.sin6_addr;

        পূর্বনির্ধারিত:
                ফেরত ফাঁকা;
        }
}

পূর্ণসংখ্যা sockaddr_set_in_addr(
                সংঘ sockaddr_union *u,
                পূর্ণসংখ্যা family,
                ধ্রুবক সংঘ in_addr_union *a,
                uint16_t port) {

        প্রতিজ্ঞা(u);
        প্রতিজ্ঞা(a);

        নির্বাচন (family) {

        ক্ষেত্র AF_INET:
                u->in = (কাঠামো sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                ফেরত 0;

        ক্ষেত্র AF_INET6:
                u->in6 = (কাঠামো sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                ফেরত 0;

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;

        }
}

পূর্ণসংখ্যা sockaddr_pretty(
                ধ্রুবক কাঠামো sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                অক্ষর **ret) {

        সংঘ sockaddr_union *sa = (সংঘ sockaddr_union*) _sa;
        অক্ষর *p;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(sa);
        প্রতিজ্ঞা(salen >= আকার(sa->sa.sa_family));
        প্রতিজ্ঞা(ret);

        নির্বাচন (sa->sa.sa_family) {

        ক্ষেত্র AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                যদি (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                অন্যথা
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                যদি (r < 0)
                        ফেরত -ENOMEM;
                বিরতি;
        }

        ক্ষেত্র AF_INET6: {
                স্থির ধ্রুবক unsigned অক্ষর ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                যদি (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, আকার(ipv4_prefix)) == 0) {
                        ধ্রুবক uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        যদি (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        অন্যথা
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        যদি (r < 0)
                                ফেরত -ENOMEM;
                } অন্যথা {
                        ধ্রুবক অক্ষর *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        যদি (include_port) {
                                যদি (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        ফেরত -ENOMEM;
                        } অন্যথা {
                                যদি (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                অন্যথা
                                        p = strdup(a);
                                যদি (!p)
                                        ফেরত -ENOMEM;
                        }
                }

                বিরতি;
        }

        ক্ষেত্র AF_UNIX:
                যদি (salen <= offsetof(কাঠামো sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(কাঠামো sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                অন্যথা {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        অক্ষর *path = (অক্ষর*) sa->un_বাফার + offsetof(কাঠামো sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(কাঠামো sockaddr_un, sun_path);

                        যদি (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ অক্ষর *e = ফাঁকা;

                                e = cescape_দৈর্ঘ্য(path + 1, path_len - 1);
                                যদি (!e)
                                        ফেরত -ENOMEM;

                                p = strjoin("@", e);
                        } অন্যথা {
                                যদি (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_দৈর্ঘ্য(path, path_len);
                        }
                }
                যদি (!p)
                        ফেরত -ENOMEM;

                বিরতি;

        ক্ষেত্র AF_VSOCK:
                যদি (include_port) {
                        যদি (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        অন্যথা
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } অন্যথা
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                যদি (r < 0)
                        ফেরত -ENOMEM;
                বিরতি;

        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }

        *ret = p;
        ফেরত 0;
}

পূর্ণসংখ্যা getpeername_pretty(পূর্ণসংখ্যা fd, bool include_port, অক্ষর **ret) {
        সংঘ sockaddr_union sa;
        socklen_t salen = আকার(sa);
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (getpeername(fd, &sa.sa, &salen) < 0)
                ফেরত -errno;

        যদি (sa.sa.sa_family == AF_UNIX) {
                কাঠামো ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                যদি (r < 0)
                        ফেরত r;

                যদি (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        ফেরত -ENOMEM;

                ফেরত 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        ফেরত sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

পূর্ণসংখ্যা getsockname_pretty(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        সংঘ sockaddr_union sa;
        socklen_t salen = আকার(sa);

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (getsockname(fd, &sa.sa, &salen) < 0)
                ফেরত -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        ফেরত sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

পূর্ণসংখ্যা socknameinfo_pretty(ধ্রুবক কাঠামো sockaddr *sa, socklen_t salen, অক্ষর **ret) {
        অক্ষর host[NI_MAXHOST];
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(sa);
        প্রতিজ্ঞা(salen >= আকার(sa_family_t));
        প্রতিজ্ঞা(ret);

        r = getnameinfo(sa, salen, host, আকার(host), /* service= */ ফাঁকা, /* service_len= */ 0, IDN_FLAGS);
        যদি (r != 0) {
                যদি (r == EAI_MEMORY)
                        ফেরত log_oom_debug();
                যদি (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                অন্যথা
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                ফেরত sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        ফেরত strdup_to(ret, host);
}

স্থির ধ্রুবক অক্ষর* ধ্রুবক netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, পূর্ণসংখ্যা, INT_MAX);

bool sockaddr_equal(ধ্রুবক সংঘ sockaddr_union *a, ধ্রুবক সংঘ sockaddr_union *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (a->sa.sa_family != b->sa.sa_family)
                ফেরত false;

        যদি (a->sa.sa_family == AF_INET)
                ফেরত a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        যদি (a->sa.sa_family == AF_INET6)
                ফেরত memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, আকার(a->in6.sin6_addr)) == 0;

        যদি (a->sa.sa_family == AF_VSOCK)
                ফেরত a->vm.svm_cid == b->vm.svm_cid;

        ফেরত false;
}

পূর্ণসংখ্যা fd_set_sndbuf(পূর্ণসংখ্যা fd, size_t n, bool increase) {
        পূর্ণসংখ্যা r, মান;
        socklen_t l = আকার(মান);

        যদি (n > INT_MAX)
                ফেরত -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &মান, &l);
        যদি (r >= 0 && l == আকার(মান) && increase ? (size_t) মান >= n*2 : (size_t) মান == n*2)
                ফেরত 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        যদি (r < 0)
                ফেরত r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = আকার(মান);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &মান, &l);
        যদি (r >= 0 && l == আকার(মান) && increase ? (size_t) মান >= n*2 : (size_t) মান == n*2)
                ফেরত 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

পূর্ণসংখ্যা fd_set_rcvbuf(পূর্ণসংখ্যা fd, size_t n, bool increase) {
        পূর্ণসংখ্যা r, মান;
        socklen_t l = আকার(মান);

        যদি (n > INT_MAX)
                ফেরত -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &মান, &l);
        যদি (r >= 0 && l == আকার(মান) && increase ? (size_t) মান >= n*2 : (size_t) মান == n*2)
                ফেরত 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        যদি (r < 0)
                ফেরত r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = আকার(মান);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &মান, &l);
        যদি (r >= 0 && l == আকার(মান) && increase ? (size_t) মান >= n*2 : (size_t) মান == n*2)
                ফেরত 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

স্থির ধ্রুবক অক্ষর* ধ্রুবক ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, পূর্ণসংখ্যা, 0xff);

bool ifname_valid_char(অক্ষর a) {
        যদি ((unsigned অক্ষর) a >= 127U)
                ফেরত false;

        যদি ((unsigned অক্ষর) a <= 32U)
                ফেরত false;

        যদি (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                ফেরত false;

        ফেরত true;
}

bool ifname_valid_full(ধ্রুবক অক্ষর *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        প্রতিজ্ঞা(!(flags & ~_IFNAME_VALID_ALL));

        যদি (isempty(p))
                ফেরত false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        যদি (parse_ifindex(p) >= 0)
                ফেরত flags & IFNAME_VALID_NUMERIC;

        যদি (flags & IFNAME_VALID_ALTERNATIVE) {
                যদি (strlen(p) >= ALTIFNAMSIZ)
                        ফেরত false;
        } অন্যথা {
                যদি (strlen(p) >= IFNAMSIZ)
                        ফেরত false;
        }

        যদি (dot_or_dot_dot(p))
                ফেরত false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        যদি (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                ফেরত false;

        জন্য (ধ্রুবক অক্ষর *t = p; *t; t++) {
                যদি (!ifname_valid_char(*t))
                        ফেরত false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        যদি (numeric)
                ফেরত false;

        ফেরত true;
}

bool address_label_valid(ধ্রুবক অক্ষর *p) {

        POINTER_MAY_BE_NULL(p);

        যদি (isempty(p))
                ফেরত false;

        যদি (strlen(p) >= IFNAMSIZ)
                ফেরত false;

        যখন (*p) {
                যদি ((uint8_t) *p >= 127U)
                        ফেরত false;

                যদি ((uint8_t) *p <= 31U)
                        ফেরত false;
                p++;
        }

        ফেরত true;
}

পূর্ণসংখ্যা getpeercred(পূর্ণসংখ্যা fd, কাঠামো ucred *ucred) {
        socklen_t n = আকার(কাঠামো ucred);
        কাঠামো ucred u;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ucred);

        যদি (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                ফেরত -errno;

        যদি (n != আকার(কাঠামো ucred))
                ফেরত -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        যদি (!pid_is_valid(u.pid))
                ফেরত -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        ফেরত 0;
}

পূর্ণসংখ্যা getpeersec(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        socklen_t n = 64;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        জন্য (;;) {
                s = new0(অক্ষর, n+1);
                যদি (!s)
                        ফেরত -ENOMEM;

                যদি (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        বিরতি;
                }

                যদি (errno != ERANGE)
                        ফেরত -errno;

                s = mfree(s);
        }

        যদি (isempty(s))
                ফেরত -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        ফেরত 0;
}

পূর্ণসংখ্যা getpeergroups(পূর্ণসংখ্যা fd, gid_t **ret) {
        socklen_t n = আকার(gid_t) * 64U;
        _cleanup_free_ gid_t *d = ফাঁকা;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        পূর্ণসংখ্যা ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max > 0)
                n = MAX(n, আকার(gid_t) * (socklen_t) ngroups_max);

        জন্য (;;) {
                d = malloc(n);
                যদি (!d)
                        ফেরত -ENOMEM;

                যদি (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        বিরতি;

                যদি (errno != ERANGE)
                        ফেরত -errno;

                d = mfree(d);
        }

        assert_se(n % আকার(gid_t) == 0);
        n /= আকার(gid_t);

        যদি (n > INT_MAX)
                ফেরত -E2BIG;

        *ret = TAKE_PTR(d);

        ফেরত (পূর্ণসংখ্যা) n;
}

পূর্ণসংখ্যা getpeerpidfd(পূর্ণসংখ্যা fd) {
        socklen_t n = আকার(পূর্ণসংখ্যা);
        পূর্ণসংখ্যা pidfd = -EBADF;

        প্রতিজ্ঞা(fd >= 0);

        যদি (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                ফেরত -errno;

        যদি (n != আকার(পূর্ণসংখ্যা))
                ফেরত -EIO;

        ফেরত pidfd;
}

পূর্ণসংখ্যা getpeerpidref(পূর্ণসংখ্যা fd, PidRef *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        পূর্ণসংখ্যা pidfd = getpeerpidfd(fd);
        যদি (pidfd < 0) {
                যদি (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        ফেরত pidfd;

                কাঠামো ucred ucred;
                r = getpeercred(fd, &ucred);
                যদি (r < 0)
                        ফেরত r;

                ফেরত pidref_set_pid(ret, ucred.pid);
        }

        ফেরত pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                পূর্ণসংখ্যা transport_fd,
                পূর্ণসংখ্যা fd,
                ধ্রুবক কাঠামো iovec *iov, size_t iovlen,
                ধ্রুবক কাঠামো sockaddr *sa, socklen_t len,
                পূর্ণসংখ্যা flags) {

        CMSG_বাফার_TYPE(CMSG_SPACE(আকার(পূর্ণসংখ্যা))) control = {};
        কাঠামো msghdr mh = {
                .msg_name = (কাঠামো sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (কাঠামো iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        প্রতিজ্ঞা(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        যদি (fd < 0 && !iov)
                ফেরত -EINVAL;

        যদি (fd >= 0) {
                কাঠামো cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = আকার(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(আকার(পূর্ণসংখ্যা));
                memcpy(CMSG_DATA(cmsg), &fd, আকার(পূর্ণসংখ্যা));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        যদি (k < 0)
                ফেরত (ssize_t) -errno;

        ফেরত k;
}

পূর্ণসংখ্যা send_one_fd_sa(
                পূর্ণসংখ্যা transport_fd,
                পূর্ণসংখ্যা fd,
                ধ্রুবক কাঠামো sockaddr *sa, socklen_t len,
                পূর্ণসংখ্যা flags) {

        প্রতিজ্ঞা(fd >= 0);

        ফেরত (পূর্ণসংখ্যা) send_one_fd_iov_sa(transport_fd, fd, ফাঁকা, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                পূর্ণসংখ্যা transport_fd,
                কাঠামো iovec *iov, size_t iovlen,
                পূর্ণসংখ্যা flags,
                পূর্ণসংখ্যা *ret_fd) {

        CMSG_বাফার_TYPE(CMSG_SPACE(আকার(পূর্ণসংখ্যা))) control;
        কাঠামো msghdr mh = {
                .msg_control = &control,
                .msg_controllen = আকার(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        কাঠামো cmsghdr *found;
        ssize_t k;

        প্রতিজ্ঞা(transport_fd >= 0);
        প্রতিজ্ঞা(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        যদি (k < 0)
                ফেরত k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(আকার(পূর্ণসংখ্যা)));
        যদি (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                যদি (k == 0)
                        ফেরত -EIO;
        }

        যদি (found)
                *ret_fd = *CMSG_TYPED_তথ্য(found, পূর্ণসংখ্যা);
        অন্যথা
                *ret_fd = -EBADF;

        ফেরত k;
}

পূর্ণসংখ্যা receive_one_fd(পূর্ণসংখ্যা transport_fd, পূর্ণসংখ্যা flags) {
        পূর্ণসংখ্যা fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, ফাঁকা, 0, flags, &fd);
        যদি (k == 0)
                ফেরত fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        প্রতিজ্ঞা(k < 0);
        ফেরত (পূর্ণসংখ্যা) k;
}

ssize_t পরবর্তী_datagram_আকার_fd(পূর্ণসংখ্যা fd) {
        ssize_t l;
        পূর্ণসংখ্যা k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, ফাঁকা, 0, MSG_PEEK|MSG_TRUNC);
        যদি (l < 0) {
                যদি (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        যাও fallback;

                ফেরত -errno;
        }
        যদি (l == 0)
                যাও fallback;

        ফেরত l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        যদি (ioctl(fd, FIONREAD, &k) < 0)
                ফেরত -errno;

        ফেরত (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

পূর্ণসংখ্যা flush_accept(পূর্ণসংখ্যা fd) {

        পূর্ণসংখ্যা r, b;
        socklen_t l = আকার(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        যদি (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                ফেরত -errno;

        প্রতিজ্ঞা(l == আকার(b));
        যদি (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                ফেরত -ENOTTY;

        জন্য (unsigned iteration = 0;; iteration++) {
                পূর্ণসংখ্যা cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                যদি (r == -EINTR)
                        চালু;
                যদি (r <= 0)
                        ফেরত r;

                যদি (iteration >= MAX_FLUSH_ITERATIONS)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, ফাঁকা, ফাঁকা, SOCK_NONBLOCK|SOCK_CLOEXEC);
                যদি (cfd < 0) {
                        যদি (errno == EAGAIN)
                                ফেরত 0;

                        যদি (ERRNO_IS_ACCEPT_AGAIN(errno))
                                চালু;

                        ফেরত -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(পূর্ণসংখ্যা fd) {
        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        কাঠামো mq_attr attr;
        ssize_t গুনতি = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        জন্য (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                যদি (r == -EINTR)
                        চালু;
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত গুনতি;

                যদি (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        যদি (mq_getattr(fd, &attr) < 0)
                                ফেরত -errno;

                        buf = malloc(attr.mq_msgsize);
                        যদি (!buf)
                                ফেরত -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ ফাঁকা);
                যদি (l < 0) {
                        যদি (errno == EINTR)
                                চালু;

                        যদি (errno == EAGAIN)
                                ফেরত গুনতি;

                        ফেরত -errno;
                }

                গুনতি += l;
        }
}

কাঠামো cmsghdr* cmsg_find(কাঠামো msghdr *mh, পূর্ণসংখ্যা level, পূর্ণসংখ্যা type, socklen_t length) {
        কাঠামো cmsghdr *cmsg;

        প্রতিজ্ঞা(mh);

        CMSG_FOREACH(cmsg, mh)
                যদি (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        ফেরত cmsg;

        ফেরত ফাঁকা;
}

শূন্য* cmsg_find_and_copy_তথ্য(কাঠামো msghdr *mh, পূর্ণসংখ্যা level, পূর্ণসংখ্যা type, শূন্য *buf, size_t buf_len) {
        কাঠামো cmsghdr *cmsg;

        প্রতিজ্ঞা(mh);
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        যদি (!cmsg)
                ফেরত ফাঁকা;

        ফেরত memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ধ্রুবক কাঠামো sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        প্রতিজ্ঞা(sa->sll_family == AF_PACKET);

        size_t mac_len = আকার(sa->sll_addr);

        যদি (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        যদি (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        ফেরত offsetof(কাঠামো sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ধ্রুবক কাঠামো sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        প্রতিজ্ঞা(sa->sun_family == AF_UNIX);

        ফেরত offsetof(কাঠামো sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, আকার(sa->sun_path)-1) :
                        strnlen(sa->sun_path, আকার(sa->sun_path))+1);
}

size_t sockaddr_len(ধ্রুবক সংঘ sockaddr_union *sa) {
        প্রতিজ্ঞা(sa);

        নির্বাচন (sa->sa.sa_family) {
        ক্ষেত্র AF_INET:
                ফেরত আকার(কাঠামো sockaddr_in);
        ক্ষেত্র AF_INET6:
                ফেরত আকার(কাঠামো sockaddr_in6);
        ক্ষেত্র AF_UNIX:
                ফেরত sockaddr_un_len(&sa->un);
        ক্ষেত্র AF_PACKET:
                ফেরত sockaddr_ll_len(&sa->ll);
        ক্ষেত্র AF_NETLINK:
                ফেরত আকার(কাঠামো sockaddr_nl);
        ক্ষেত্র AF_VSOCK:
                ফেরত আকার(কাঠামো sockaddr_vm);
        পূর্বনির্ধারিত:
                assert_not_reached();
        }
}

পূর্ণসংখ্যা socket_ioctl_fd(শূন্য) {
        পূর্ণসংখ্যা fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        যদি (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        যদি (fd < 0)
                ফেরত -errno;

        ফেরত fd;
}

পূর্ণসংখ্যা sockaddr_un_unlink(ধ্রুবক কাঠামো sockaddr_un *sa) {
        ধ্রুবক অক্ষর *p, * nul;

        প্রতিজ্ঞা(sa);

        যদি (sa->sun_family != AF_UNIX)
                ফেরত -EPROTOTYPE;

        যদি (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                ফেরত 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, আকার(sa->sun_path));
        যদি (nul)
                p = sa->sun_path;
        অন্যথা
                p = memdupa_suffix0(sa->sun_path, আকার(sa->sun_path));

        যদি (unlink(p) < 0)
                ফেরত -errno;

        ফেরত 1;
}

পূর্ণসংখ্যা sockaddr_un_set_path(কাঠামো sockaddr_un *ret, ধ্রুবক অক্ষর *path) {
        size_t l;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        যদি (l < 2)
                ফেরত -EINVAL;
        যদি (!IN_SET(path[0], '/', '@'))
                ফেরত -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        যদি (l+1 > আকার(ret->sun_path))
                ফেরত path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (কাঠামো sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        যদি (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                ফেরত (পূর্ণসংখ্যা) (offsetof(কাঠামো sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } অন্যথা {
                প্রতিজ্ঞা(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                ফেরত (পূর্ণসংখ্যা) (offsetof(কাঠামো sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

পূর্ণসংখ্যা getsockopt_int(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা level, পূর্ণসংখ্যা optname, পূর্ণসংখ্যা *ret) {
        পূর্ণসংখ্যা v;
        socklen_t sl = আকার(v);

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (getsockopt(fd, level, optname, &v, &sl) < 0)
                ফেরত negative_errno();
        যদি (sl != আকার(v))
                ফেরত -EIO;

        *ret = v;
        ফেরত 0;
}

পূর্ণসংখ্যা socket_bind_to_ifname(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *ifname) {
        প্রতিজ্ঞা(fd >= 0);

        /* Call with NULL to drop binding */

        ফেরত RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

পূর্ণসংখ্যা socket_bind_to_ifindex(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা ifindex) {
        প্রতিজ্ঞা(fd >= 0);

        যদি (ifindex <= 0)
                /* Drop binding */
                ফেরত RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ফাঁকা, 0));

        ফেরত setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

পূর্ণসংখ্যা socket_autobind(পূর্ণসংখ্যা fd, অক্ষর **ret_name) {
        _cleanup_free_ অক্ষর *name = ফাঁকা;
        uint64_t random;
        পূর্ণসংখ্যা r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        প্রতিজ্ঞা(fd >= 0);

        random = random_u64();

        যদি (asprintf(&name, "@%" PRIu64, random) < 0)
                ফেরত -ENOMEM;

        সংঘ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < আকার(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        যদি (r < 0)
                ফেরত r;

        যদি (bind(fd, &sa.sa, r) < 0)
                ফেরত -errno;

        যদি (ret_name)
                *ret_name = TAKE_PTR(name);
        ফেরত 0;
}

ssize_t recvmsg_safe(পূর্ণসংখ্যা sockfd, কাঠামো msghdr *msg, পূর্ণসংখ্যা flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        প্রতিজ্ঞা(sockfd >= 0);
        প্রতিজ্ঞা(msg);

        n = recvmsg(sockfd, msg, flags);
        যদি (n < 0)
                ফেরত -errno;

        যদি (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                ফেরত FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        ফেরত n;
}

পূর্ণসংখ্যা socket_get_family(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা af;
        socklen_t sl = আকার(af);

        যদি (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                ফেরত -errno;

        যদি (sl != আকার(af))
                ফেরত -EINVAL;

        ফেরত af;
}

পূর্ণসংখ্যা socket_set_recvpktinfo(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {

        যদি (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                যদি (af < 0)
                        ফেরত af;
        }

        নির্বাচন (af) {

        ক্ষেত্র AF_INET:
                ফেরত setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        ক্ষেত্র AF_INET6:
                ফেরত setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        ক্ষেত্র AF_NETLINK:
                ফেরত setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        ক্ষেত্র AF_PACKET:
                ফেরত setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা socket_set_unicast_if(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, পূর্ণসংখ্যা ifi) {
        be32_t ifindex_be = htobe32(ifi);

        যদি (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                যদি (af < 0)
                        ফেরত af;
        }

        নির্বাচন (af) {

        ক্ষেত্র AF_INET:
                ফেরত RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, আকার(ifindex_be)));

        ক্ষেত্র AF_INET6:
                ফেরত RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, আকার(ifindex_be)));

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা socket_set_option(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, পূর্ণসংখ্যা opt_ipv4, পূর্ণসংখ্যা opt_ipv6, পূর্ণসংখ্যা val) {
        যদি (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                যদি (af < 0)
                        ফেরত af;
        }

        নির্বাচন (af) {

        ক্ষেত্র AF_INET:
                ফেরত setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        ক্ষেত্র AF_INET6:
                ফেরত setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }
}

পূর্ণসংখ্যা socket_get_mtu(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, size_t *ret) {
        পূর্ণসংখ্যা mtu, r;

        প্রতিজ্ঞা(ret);

        যদি (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                যদি (af < 0)
                        ফেরত af;
        }

        নির্বাচন (af) {

        ক্ষেত্র AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                বিরতি;

        ক্ষেত্র AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                বিরতি;

        পূর্বনির্ধারিত:
                ফেরত -EAFNOSUPPORT;
        }

        যদি (r < 0)
                ফেরত r;
        যদি (mtu <= 0)
                ফেরত -EINVAL;

        *ret = (size_t) mtu;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা connect_unix_path_simple(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path) {
        সংঘ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(path);

        l = strlen(path);
        প্রতিজ্ঞা(l > 0);
        প্রতিজ্ঞা(l < আকার(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        ফেরত RET_NERRNO(connect(fd, &sa.sa, offsetof(কাঠামো sockaddr_un, sun_path) + l + 1));
}

স্থির পূর্ণসংখ্যা connect_unix_inode(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা inode_fd) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(inode_fd >= 0);

        ফেরত connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

পূর্ণসংখ্যা connect_unix_path(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা inode_fd = -EBADF;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        যদি (!path) {
                যদি (dir_fd < 0)
                        ফেরত -EISDIR;

                ফেরত connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        যদি (isempty(path))
                ফেরত -EINVAL;

        /* Shortcut for the simple case */
        যদি (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(কাঠামো sockaddr_un, sun_path))
                ফেরত connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        যদি (dir_fd == XAT_FDROOT) {
                _cleanup_free_ অক্ষর *j = strjoin("/", path);
                যদি (!j)
                        ফেরত -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } অন্যথা
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        যদি (inode_fd < 0)
                ফেরত -errno;

        ফেরত connect_unix_inode(fd, inode_fd);
}

পূর্ণসংখ্যা socket_address_parse_unix(SocketAddress *ret_address, ধ্রুবক অক্ষর *s) {
        কাঠামো sockaddr_un un;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_address);
        প্রতিজ্ঞা(s);

        যদি (!IN_SET(*s, '/', '@'))
                ফেরত -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        যদি (r < 0)
                ফেরত r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .আকার_2 = r,
        };

        ফেরত 0;
}

পূর্ণসংখ্যা socket_address_equal_unix(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        SocketAddress socket_a, socket_b;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = socket_address_parse_unix(&socket_a, a);
        যদি (r < 0)
                ফেরত r;

        r = socket_address_parse_unix(&socket_b, b);
        যদি (r < 0)
                ফেরত r;

        ফেরত sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

পূর্ণসংখ্যা vsock_parse_port(ধ্রুবক অক্ষর *s, unsigned *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        যদি (!s)
                ফেরত -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        যদি (r < 0)
                ফেরত r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        যদি (u == VMADDR_PORT_ANY)
                ফেরত -EINVAL;

        *ret = u;
        ফেরত 0;
}

পূর্ণসংখ্যা vsock_parse_cid(ধ্রুবক অক্ষর *s, unsigned *ret) {
        প্রতিজ্ঞা(ret);

        যদি (!s)
                ফেরত -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        যদি (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        অন্যথা যদি (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        অন্যথা যদি (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        অন্যথা যদি (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        অন্যথা
                ফেরত safe_atou(s, ret);

        ফেরত 0;
}

পূর্ণসংখ্যা socket_address_parse_vsock(SocketAddress *ret_address, ধ্রুবক অক্ষর *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ অক্ষর *n = ফাঁকা;
        ধ্রুবক অক্ষর *e, *cid_start;
        unsigned port, cid;
        পূর্ণসংখ্যা type, r;

        প্রতিজ্ঞা(ret_address);
        প্রতিজ্ঞা(s);

        যদি ((cid_start = startswith(s, "vsock:")))
                type = 0;
        অন্যথা যদি ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        অন্যথা যদি ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        অন্যথা যদি ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        অন্যথা
                ফেরত -EPROTO;

        e = strchr(cid_start, ':');
        যদি (!e)
                ফেরত -EINVAL;

        r = vsock_parse_port(e+1, &port);
        যদি (r < 0)
                ফেরত r;

        n = strndup(cid_start, e - cid_start);
        যদি (!n)
                ফেরত -ENOMEM;

        যদি (isempty(n))
                cid = VMADDR_CID_ANY;
        অন্যথা {
                r = vsock_parse_cid(n, &cid);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .আকার_2 = আকার(কাঠামো sockaddr_vm),
        };

        ফেরত 0;
}

পূর্ণসংখ্যা netlink_socket_get_multicast_groups(পূর্ণসংখ্যা fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = ফাঁকা;
        socklen_t len = 0, old_len;

        প্রতিজ্ঞা(fd >= 0);

        যদি (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, ফাঁকা, &len) < 0)
                ফেরত -errno;

        যদি (len == 0)
                যাও finalize;

        groups = new0(uint32_t, len);
        যদি (!groups)
                ফেরত -ENOMEM;

        old_len = len;

        যদি (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                ফেরত -errno;

        যদি (old_len != len)
                ফেরত -EIO;

finalize:
        যদি (ret_len)
                *ret_len = len;
        যদি (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        ফেরত 0;
}

পূর্ণসংখ্যা socket_get_cookie(পূর্ণসংখ্যা fd, uint64_t *ret) {
        প্রতিজ্ঞা(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = আকার(cookie);
        যদি (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                ফেরত -errno;

        প্রতিজ্ঞা(cookie_len == আকার(cookie));
        যদি (ret)
                *ret = cookie;

        ফেরত 0;
}

শূন্য cmsg_close_all(কাঠামো msghdr *mh) {
        প্রতিজ্ঞা(mh);

        কাঠামো cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                যদি (cmsg->cmsg_level != SOL_SOCKET)
                        চালু;

                যদি (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_তথ্য(cmsg, পূর্ণসংখ্যা),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / আকার(পূর্ণসংখ্যা));
                অন্যথা যদি (cmsg->cmsg_type == SCM_PIDFD) {
                        প্রতিজ্ঞা(cmsg->cmsg_len == CMSG_LEN(আকার(পূর্ণসংখ্যা)));
                        safe_close(*CMSG_TYPED_তথ্য(cmsg, পূর্ণসংখ্যা));
                }
        }
}

পূর্ণসংখ্যা tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        নির্বাচন (IPTOS_PREC(tos)) {
        ক্ষেত্র IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        ক্ষেত্র IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                ফেরত TC_PRIO_CONTROL;

        ক্ষেত্র IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        ক্ষেত্র IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                ফেরত TC_PRIO_INTERACTIVE;

        ক্ষেত্র IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        ক্ষেত্র IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                ফেরত TC_PRIO_INTERACTIVE_BULK;

        ক্ষেত্র IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                ফেরত TC_PRIO_BULK;

        ক্ষেত্র IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        পূর্বনির্ধারিত:
                ফেরত TC_PRIO_BESTEFFORT;
        }
}

পূর্ণসংখ্যা socket_xattr_supported(শূন্য) {
        পূর্ণসংখ্যা r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        স্থির পূর্ণসংখ্যা cached = -1;
        যদি (cached >= 0)
                ফেরত cached;

        _cleanup_close_ পূর্ণসংখ্যা fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        যদি (fd < 0)
                ফেরত -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ ফাঁকা, AT_EMPTY_PATH, "user.testxxx", "1");
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                ফেরত (cached = false);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to set test xattr on socket: %m");

        ফেরত (cached = true);
}
