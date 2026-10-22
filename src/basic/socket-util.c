/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/if.h>
#அந்தர்பாவ <linux/if_arp.h>
#அந்தர்பாவ <linux/if_ether.h>
#அந்தர்பாவ <linux/if_infiniband.h>
#அந்தர்பாவ <linux/pkt_sched.h>
#அந்தர்பாவ <mqueue.h>
#அந்தர்பாவ <net/if.h>
#அந்தர்பாவ <netdb.h>
#அந்தர்பாவ <netinet/ip.h>
#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "format-ifname.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "in-addr-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "random-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "sparse-endian.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "sysctl-util.h"
#அந்தர்பாவ "user-util.h"
#அந்தர்பாவ "xattr-util.h"

#எனில் ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#இல்லையெனில்
#  define IDN_FLAGS 0
#endif

நிலையான மாறிலி எழுத்து* மாறிலி socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, முழு);

முழு socket_address_verify(மாறிலி SocketAddress *a, bool strict) {
        ப்ரதிஜ்ஞா(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        தேர்வு (socket_address_family(a)) {

        நிலை AF_INET:
                எனில் (a->size != அளவு(கட்டமைப்பு sockaddr_in))
                        திரும்பு -EINVAL;

                எனில் (a->sockaddr.in.sin_port == 0)
                        திரும்பு -EINVAL;

                எனில் (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        திரும்பு -EINVAL;

                திரும்பு 0;

        நிலை AF_INET6:
                எனில் (a->size != அளவு(கட்டமைப்பு sockaddr_in6))
                        திரும்பு -EINVAL;

                எனில் (a->sockaddr.in6.sin6_port == 0)
                        திரும்பு -EINVAL;

                எனில் (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        திரும்பு -EINVAL;

                திரும்பு 0;

        நிலை AF_UNIX:
                எனில் (a->size < offsetof(கட்டமைப்பு sockaddr_un, sun_path))
                        திரும்பு -EINVAL;
                எனில் (a->size > அளவு(கட்டமைப்பு sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        திரும்பு -EINVAL;

                எனில் (a->size > offsetof(கட்டமைப்பு sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        மாறிலி எழுத்து *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, அளவு(a->sockaddr.un.sun_path));
                        எனில் (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                எனில் (a->size != offsetof(கட்டமைப்பு sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        திரும்பு -EINVAL;
                        } இல்லையெனில் {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                எனில் (!IN_SET(a->size, அளவு(a->sockaddr.un.sun_path), அளவு(a->sockaddr.un.sun_path)+1))
                                        திரும்பு -EINVAL;
                        }
                }

                எனில் (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        திரும்பு -EINVAL;

                திரும்பு 0;

        நிலை AF_NETLINK:

                எனில் (a->size != அளவு(கட்டமைப்பு sockaddr_nl))
                        திரும்பு -EINVAL;

                எனில் (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        திரும்பு -EINVAL;

                திரும்பு 0;

        நிலை AF_VSOCK:
                எனில் (a->size != அளவு(கட்டமைப்பு sockaddr_vm))
                        திரும்பு -EINVAL;

                எனில் (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        திரும்பு -EINVAL;

                திரும்பு 0;

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு socket_address_print(மாறிலி SocketAddress *a, எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        எனில் (r < 0)
                திரும்பு r;

        எனில் (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ எழுத்து *sfamily = வெற்று;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                எனில் (r < 0)
                        திரும்பு r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                எனில் (r < 0)
                        திரும்பு -ENOMEM;

                திரும்பு 0;
        }

        திரும்பு sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(மாறிலி SocketAddress *a) {
        ப்ரதிஜ்ஞா(a);

        திரும்பு
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(மாறிலி SocketAddress *a, மாறிலி SocketAddress *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        /* Invalid addresses are unequal to all */
        எனில் (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                திரும்பு false;

        எனில் (a->type != b->type)
                திரும்பு false;

        எனில் (socket_address_family(a) != socket_address_family(b))
                திரும்பு false;

        தேர்வு (socket_address_family(a)) {

        நிலை AF_INET:
                எனில் (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        திரும்பு false;

                எனில் (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        திரும்பு false;

                நிறுத்து;

        நிலை AF_INET6:
                எனில் (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, அளவு(a->sockaddr.in6.sin6_addr)) != 0)
                        திரும்பு false;

                எனில் (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        திரும்பு false;

                நிறுத்து;

        நிலை AF_UNIX:
                எனில் (a->size <= offsetof(கட்டமைப்பு sockaddr_un, sun_path) ||
                    b->size <= offsetof(கட்டமைப்பு sockaddr_un, sun_path))
                        திரும்பு false;

                எனில் ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        திரும்பு false;

                எனில் (a->sockaddr.un.sun_path[0]) {
                        எனில் (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                திரும்பு false;
                } இல்லையெனில் {
                        எனில் (a->size != b->size)
                                திரும்பு false;

                        எனில் (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                திரும்பு false;
                }

                நிறுத்து;

        நிலை AF_NETLINK:
                எனில் (a->protocol != b->protocol)
                        திரும்பு false;

                எனில் (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        திரும்பு false;

                நிறுத்து;

        நிலை AF_VSOCK:
                எனில் (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        திரும்பு false;

                எனில் (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        திரும்பு false;

                நிறுத்து;

        இயல்பு:
                /* Cannot compare, so we assume the addresses are different */
                திரும்பு false;
        }

        திரும்பு true;
}

மாறிலி எழுத்து* socket_address_get_path(மாறிலி SocketAddress *a) {
        ப்ரதிஜ்ஞா(a);

        எனில் (socket_address_family(a) != AF_UNIX)
                திரும்பு வெற்று;

        எனில் (a->sockaddr.un.sun_path[0] == 0)
                திரும்பு வெற்று;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(அளவு(ஒன்றிப்பு sockaddr_union) >= அளவு(கட்டமைப்பு sockaddr_un)+1);
        திரும்பு a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(வெறுமை) {
        நிலையான முழு cached = -1;

        எனில் (cached < 0) {

                எனில் (access("/proc/net/if_inet6", F_OK) < 0) {

                        எனில் (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                திரும்பு false;
                        }

                        cached = false;
                } இல்லையெனில்
                        cached = true;
        }

        திரும்பு cached;
}

bool socket_ipv6_is_enabled(வெறுமை) {
        _cleanup_free_ எழுத்து *v = வெற்று;
        முழு r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        எனில் (!socket_ipv6_is_supported())
                திரும்பு false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        எனில் (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                திரும்பு true;
        }

        r = parse_boolean(v);
        எனில் (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                திரும்பு true;
        }

        திரும்பு !r;
}

bool socket_address_matches_fd(மாறிலி SocketAddress *a, முழு fd) {
        SocketAddress b;
        socklen_t solen;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(fd >= 0);

        b.size = அளவு(b.sockaddr);
        எனில் (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                திரும்பு false;

        எனில் (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                திரும்பு false;

        solen = அளவு(b.type);
        எனில் (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                திரும்பு false;

        எனில் (b.type != a->type)
                திரும்பு false;

        எனில் (a->protocol != 0)  {
                solen = அளவு(b.protocol);
                எனில் (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        திரும்பு false;

                எனில் (b.protocol != a->protocol)
                        திரும்பு false;
        }

        திரும்பு socket_address_equal(a, &b);
}

முழு sockaddr_port(மாறிலி கட்டமைப்பு sockaddr *_sa, unsigned *ret_port) {
        மாறிலி ஒன்றிப்பு sockaddr_union *sa = (மாறிலி ஒன்றிப்பு sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        ப்ரதிஜ்ஞா(sa);
        ப்ரதிஜ்ஞா(ret_port);

        தேர்வு (sa->sa.sa_family) {

        நிலை AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                திரும்பு 0;

        நிலை AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                திரும்பு 0;

        நிலை AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                திரும்பு 0;

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

மாறிலி ஒன்றிப்பு in_addr_union *sockaddr_in_addr(மாறிலி கட்டமைப்பு sockaddr *_sa) {
        மாறிலி ஒன்றிப்பு sockaddr_union *sa = (மாறிலி ஒன்றிப்பு sockaddr_union*) _sa;

        எனில் (!sa)
                திரும்பு வெற்று;

        தேர்வு (sa->sa.sa_family) {

        நிலை AF_INET:
                திரும்பு (மாறிலி ஒன்றிப்பு in_addr_union*) &sa->in.sin_addr;

        நிலை AF_INET6:
                திரும்பு (மாறிலி ஒன்றிப்பு in_addr_union*) &sa->in6.sin6_addr;

        இயல்பு:
                திரும்பு வெற்று;
        }
}

முழு sockaddr_set_in_addr(
                ஒன்றிப்பு sockaddr_union *u,
                முழு family,
                மாறிலி ஒன்றிப்பு in_addr_union *a,
                uint16_t port) {

        ப்ரதிஜ்ஞா(u);
        ப்ரதிஜ்ஞா(a);

        தேர்வு (family) {

        நிலை AF_INET:
                u->in = (கட்டமைப்பு sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                திரும்பு 0;

        நிலை AF_INET6:
                u->in6 = (கட்டமைப்பு sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                திரும்பு 0;

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;

        }
}

முழு sockaddr_pretty(
                மாறிலி கட்டமைப்பு sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                எழுத்து **ret) {

        ஒன்றிப்பு sockaddr_union *sa = (ஒன்றிப்பு sockaddr_union*) _sa;
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(sa);
        ப்ரதிஜ்ஞா(salen >= அளவு(sa->sa.sa_family));
        ப்ரதிஜ்ஞா(ret);

        தேர்வு (sa->sa.sa_family) {

        நிலை AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                எனில் (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                இல்லையெனில்
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                எனில் (r < 0)
                        திரும்பு -ENOMEM;
                நிறுத்து;
        }

        நிலை AF_INET6: {
                நிலையான மாறிலி unsigned எழுத்து ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                எனில் (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, அளவு(ipv4_prefix)) == 0) {
                        மாறிலி uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        எனில் (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        இல்லையெனில்
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        எனில் (r < 0)
                                திரும்பு -ENOMEM;
                } இல்லையெனில் {
                        மாறிலி எழுத்து *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        எனில் (include_port) {
                                எனில் (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        திரும்பு -ENOMEM;
                        } இல்லையெனில் {
                                எனில் (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                இல்லையெனில்
                                        p = strdup(a);
                                எனில் (!p)
                                        திரும்பு -ENOMEM;
                        }
                }

                நிறுத்து;
        }

        நிலை AF_UNIX:
                எனில் (salen <= offsetof(கட்டமைப்பு sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(கட்டமைப்பு sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                இல்லையெனில் {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        எழுத்து *path = (எழுத்து*) sa->un_buffer + offsetof(கட்டமைப்பு sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(கட்டமைப்பு sockaddr_un, sun_path);

                        எனில் (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ எழுத்து *e = வெற்று;

                                e = cescape_length(path + 1, path_len - 1);
                                எனில் (!e)
                                        திரும்பு -ENOMEM;

                                p = strjoin("@", e);
                        } இல்லையெனில் {
                                எனில் (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                எனில் (!p)
                        திரும்பு -ENOMEM;

                நிறுத்து;

        நிலை AF_VSOCK:
                எனில் (include_port) {
                        எனில் (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        இல்லையெனில்
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } இல்லையெனில்
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                எனில் (r < 0)
                        திரும்பு -ENOMEM;
                நிறுத்து;

        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }

        *ret = p;
        திரும்பு 0;
}

முழு getpeername_pretty(முழு fd, bool include_port, எழுத்து **ret) {
        ஒன்றிப்பு sockaddr_union sa;
        socklen_t salen = அளவு(sa);
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (getpeername(fd, &sa.sa, &salen) < 0)
                திரும்பு -errno;

        எனில் (sa.sa.sa_family == AF_UNIX) {
                கட்டமைப்பு ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        திரும்பு -ENOMEM;

                திரும்பு 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        திரும்பு sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

முழு getsockname_pretty(முழு fd, எழுத்து **ret) {
        ஒன்றிப்பு sockaddr_union sa;
        socklen_t salen = அளவு(sa);

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (getsockname(fd, &sa.sa, &salen) < 0)
                திரும்பு -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        திரும்பு sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

முழு socknameinfo_pretty(மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t salen, எழுத்து **ret) {
        எழுத்து host[NI_MAXHOST];
        முழு r;

        ப்ரதிஜ்ஞா(sa);
        ப்ரதிஜ்ஞா(salen >= அளவு(sa_family_t));
        ப்ரதிஜ்ஞா(ret);

        r = getnameinfo(sa, salen, host, அளவு(host), /* service= */ வெற்று, /* service_len= */ 0, IDN_FLAGS);
        எனில் (r != 0) {
                எனில் (r == EAI_MEMORY)
                        திரும்பு log_oom_debug();
                எனில் (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                இல்லையெனில்
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                திரும்பு sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        திரும்பு strdup_to(ret, host);
}

நிலையான மாறிலி எழுத்து* மாறிலி netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, முழு, INT_MAX);

bool sockaddr_equal(மாறிலி ஒன்றிப்பு sockaddr_union *a, மாறிலி ஒன்றிப்பு sockaddr_union *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        எனில் (a->sa.sa_family != b->sa.sa_family)
                திரும்பு false;

        எனில் (a->sa.sa_family == AF_INET)
                திரும்பு a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        எனில் (a->sa.sa_family == AF_INET6)
                திரும்பு memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, அளவு(a->in6.sin6_addr)) == 0;

        எனில் (a->sa.sa_family == AF_VSOCK)
                திரும்பு a->vm.svm_cid == b->vm.svm_cid;

        திரும்பு false;
}

முழு fd_set_sndbuf(முழு fd, size_t n, bool increase) {
        முழு r, மதிப்பு;
        socklen_t l = அளவு(மதிப்பு);

        எனில் (n > INT_MAX)
                திரும்பு -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &மதிப்பு, &l);
        எனில் (r >= 0 && l == அளவு(மதிப்பு) && increase ? (size_t) மதிப்பு >= n*2 : (size_t) மதிப்பு == n*2)
                திரும்பு 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        எனில் (r < 0)
                திரும்பு r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = அளவு(மதிப்பு);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &மதிப்பு, &l);
        எனில் (r >= 0 && l == அளவு(மதிப்பு) && increase ? (size_t) மதிப்பு >= n*2 : (size_t) மதிப்பு == n*2)
                திரும்பு 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

முழு fd_set_rcvbuf(முழு fd, size_t n, bool increase) {
        முழு r, மதிப்பு;
        socklen_t l = அளவு(மதிப்பு);

        எனில் (n > INT_MAX)
                திரும்பு -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &மதிப்பு, &l);
        எனில் (r >= 0 && l == அளவு(மதிப்பு) && increase ? (size_t) மதிப்பு >= n*2 : (size_t) மதிப்பு == n*2)
                திரும்பு 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        எனில் (r < 0)
                திரும்பு r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = அளவு(மதிப்பு);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &மதிப்பு, &l);
        எனில் (r >= 0 && l == அளவு(மதிப்பு) && increase ? (size_t) மதிப்பு >= n*2 : (size_t) மதிப்பு == n*2)
                திரும்பு 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

நிலையான மாறிலி எழுத்து* மாறிலி ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, முழு, 0xff);

bool ifname_valid_char(எழுத்து a) {
        எனில் ((unsigned எழுத்து) a >= 127U)
                திரும்பு false;

        எனில் ((unsigned எழுத்து) a <= 32U)
                திரும்பு false;

        எனில் (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                திரும்பு false;

        திரும்பு true;
}

bool ifname_valid_full(மாறிலி எழுத்து *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        ப்ரதிஜ்ஞா(!(flags & ~_IFNAME_VALID_ALL));

        எனில் (isempty(p))
                திரும்பு false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        எனில் (parse_ifindex(p) >= 0)
                திரும்பு flags & IFNAME_VALID_NUMERIC;

        எனில் (flags & IFNAME_VALID_ALTERNATIVE) {
                எனில் (strlen(p) >= ALTIFNAMSIZ)
                        திரும்பு false;
        } இல்லையெனில் {
                எனில் (strlen(p) >= IFNAMSIZ)
                        திரும்பு false;
        }

        எனில் (dot_or_dot_dot(p))
                திரும்பு false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        எனில் (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                திரும்பு false;

        க்கு (மாறிலி எழுத்து *t = p; *t; t++) {
                எனில் (!ifname_valid_char(*t))
                        திரும்பு false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        எனில் (numeric)
                திரும்பு false;

        திரும்பு true;
}

bool address_label_valid(மாறிலி எழுத்து *p) {

        POINTER_MAY_BE_NULL(p);

        எனில் (isempty(p))
                திரும்பு false;

        எனில் (strlen(p) >= IFNAMSIZ)
                திரும்பு false;

        போது (*p) {
                எனில் ((uint8_t) *p >= 127U)
                        திரும்பு false;

                எனில் ((uint8_t) *p <= 31U)
                        திரும்பு false;
                p++;
        }

        திரும்பு true;
}

முழு getpeercred(முழு fd, கட்டமைப்பு ucred *ucred) {
        socklen_t n = அளவு(கட்டமைப்பு ucred);
        கட்டமைப்பு ucred u;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ucred);

        எனில் (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                திரும்பு -errno;

        எனில் (n != அளவு(கட்டமைப்பு ucred))
                திரும்பு -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        எனில் (!pid_is_valid(u.pid))
                திரும்பு -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        திரும்பு 0;
}

முழு getpeersec(முழு fd, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        socklen_t n = 64;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        க்கு (;;) {
                s = new0(எழுத்து, n+1);
                எனில் (!s)
                        திரும்பு -ENOMEM;

                எனில் (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        நிறுத்து;
                }

                எனில் (errno != ERANGE)
                        திரும்பு -errno;

                s = mfree(s);
        }

        எனில் (isempty(s))
                திரும்பு -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        திரும்பு 0;
}

முழு getpeergroups(முழு fd, gid_t **ret) {
        socklen_t n = அளவு(gid_t) * 64U;
        _cleanup_free_ gid_t *d = வெற்று;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        முழு ngroups_max = sysconf_ngroups_max();
        எனில் (ngroups_max > 0)
                n = MAX(n, அளவு(gid_t) * (socklen_t) ngroups_max);

        க்கு (;;) {
                d = malloc(n);
                எனில் (!d)
                        திரும்பு -ENOMEM;

                எனில் (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        நிறுத்து;

                எனில் (errno != ERANGE)
                        திரும்பு -errno;

                d = mfree(d);
        }

        assert_se(n % அளவு(gid_t) == 0);
        n /= அளவு(gid_t);

        எனில் (n > INT_MAX)
                திரும்பு -E2BIG;

        *ret = TAKE_PTR(d);

        திரும்பு (முழு) n;
}

முழு getpeerpidfd(முழு fd) {
        socklen_t n = அளவு(முழு);
        முழு pidfd = -EBADF;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                திரும்பு -errno;

        எனில் (n != அளவு(முழு))
                திரும்பு -EIO;

        திரும்பு pidfd;
}

முழு getpeerpidref(முழு fd, PidRef *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        முழு pidfd = getpeerpidfd(fd);
        எனில் (pidfd < 0) {
                எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        திரும்பு pidfd;

                கட்டமைப்பு ucred ucred;
                r = getpeercred(fd, &ucred);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு pidref_set_pid(ret, ucred.pid);
        }

        திரும்பு pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                முழு transport_fd,
                முழு fd,
                மாறிலி கட்டமைப்பு iovec *iov, size_t iovlen,
                மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t len,
                முழு flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(அளவு(முழு))) control = {};
        கட்டமைப்பு msghdr mh = {
                .msg_name = (கட்டமைப்பு sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (கட்டமைப்பு iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        ப்ரதிஜ்ஞா(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        எனில் (fd < 0 && !iov)
                திரும்பு -EINVAL;

        எனில் (fd >= 0) {
                கட்டமைப்பு cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = அளவு(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(அளவு(முழு));
                memcpy(CMSG_DATA(cmsg), &fd, அளவு(முழு));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        எனில் (k < 0)
                திரும்பு (ssize_t) -errno;

        திரும்பு k;
}

முழு send_one_fd_sa(
                முழு transport_fd,
                முழு fd,
                மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t len,
                முழு flags) {

        ப்ரதிஜ்ஞா(fd >= 0);

        திரும்பு (முழு) send_one_fd_iov_sa(transport_fd, fd, வெற்று, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                முழு transport_fd,
                கட்டமைப்பு iovec *iov, size_t iovlen,
                முழு flags,
                முழு *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(அளவு(முழு))) control;
        கட்டமைப்பு msghdr mh = {
                .msg_control = &control,
                .msg_controllen = அளவு(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        கட்டமைப்பு cmsghdr *found;
        ssize_t k;

        ப்ரதிஜ்ஞா(transport_fd >= 0);
        ப்ரதிஜ்ஞா(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        எனில் (k < 0)
                திரும்பு k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(அளவு(முழு)));
        எனில் (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                எனில் (k == 0)
                        திரும்பு -EIO;
        }

        எனில் (found)
                *ret_fd = *CMSG_TYPED_DATA(found, முழு);
        இல்லையெனில்
                *ret_fd = -EBADF;

        திரும்பு k;
}

முழு receive_one_fd(முழு transport_fd, முழு flags) {
        முழு fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, வெற்று, 0, flags, &fd);
        எனில் (k == 0)
                திரும்பு fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        ப்ரதிஜ்ஞா(k < 0);
        திரும்பு (முழு) k;
}

ssize_t next_datagram_size_fd(முழு fd) {
        ssize_t l;
        முழு k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, வெற்று, 0, MSG_PEEK|MSG_TRUNC);
        எனில் (l < 0) {
                எனில் (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        செல் fallback;

                திரும்பு -errno;
        }
        எனில் (l == 0)
                செல் fallback;

        திரும்பு l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        எனில் (ioctl(fd, FIONREAD, &k) < 0)
                திரும்பு -errno;

        திரும்பு (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

முழு flush_accept(முழு fd) {

        முழு r, b;
        socklen_t l = அளவு(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        எனில் (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                திரும்பு -errno;

        ப்ரதிஜ்ஞா(l == அளவு(b));
        எனில் (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                திரும்பு -ENOTTY;

        க்கு (unsigned iteration = 0;; iteration++) {
                முழு cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                எனில் (r == -EINTR)
                        தொடர்;
                எனில் (r <= 0)
                        திரும்பு r;

                எனில் (iteration >= MAX_FLUSH_ITERATIONS)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, வெற்று, வெற்று, SOCK_NONBLOCK|SOCK_CLOEXEC);
                எனில் (cfd < 0) {
                        எனில் (errno == EAGAIN)
                                திரும்பு 0;

                        எனில் (ERRNO_IS_ACCEPT_AGAIN(errno))
                                தொடர்;

                        திரும்பு -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(முழு fd) {
        _cleanup_free_ எழுத்து *buf = வெற்று;
        கட்டமைப்பு mq_attr attr;
        ssize_t மொத்தம் = 0;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        க்கு (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                எனில் (r == -EINTR)
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு மொத்தம்;

                எனில் (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        எனில் (mq_getattr(fd, &attr) < 0)
                                திரும்பு -errno;

                        buf = malloc(attr.mq_msgsize);
                        எனில் (!buf)
                                திரும்பு -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ வெற்று);
                எனில் (l < 0) {
                        எனில் (errno == EINTR)
                                தொடர்;

                        எனில் (errno == EAGAIN)
                                திரும்பு மொத்தம்;

                        திரும்பு -errno;
                }

                மொத்தம் += l;
        }
}

கட்டமைப்பு cmsghdr* cmsg_find(கட்டமைப்பு msghdr *mh, முழு level, முழு type, socklen_t length) {
        கட்டமைப்பு cmsghdr *cmsg;

        ப்ரதிஜ்ஞா(mh);

        CMSG_FOREACH(cmsg, mh)
                எனில் (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        திரும்பு cmsg;

        திரும்பு வெற்று;
}

வெறுமை* cmsg_find_and_copy_data(கட்டமைப்பு msghdr *mh, முழு level, முழு type, வெறுமை *buf, size_t buf_len) {
        கட்டமைப்பு cmsghdr *cmsg;

        ப்ரதிஜ்ஞா(mh);
        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        எனில் (!cmsg)
                திரும்பு வெற்று;

        திரும்பு memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(மாறிலி கட்டமைப்பு sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        ப்ரதிஜ்ஞா(sa->sll_family == AF_PACKET);

        size_t mac_len = அளவு(sa->sll_addr);

        எனில் (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        எனில் (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        திரும்பு offsetof(கட்டமைப்பு sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(மாறிலி கட்டமைப்பு sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        ப்ரதிஜ்ஞா(sa->sun_family == AF_UNIX);

        திரும்பு offsetof(கட்டமைப்பு sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, அளவு(sa->sun_path)-1) :
                        strnlen(sa->sun_path, அளவு(sa->sun_path))+1);
}

size_t sockaddr_len(மாறிலி ஒன்றிப்பு sockaddr_union *sa) {
        ப்ரதிஜ்ஞா(sa);

        தேர்வு (sa->sa.sa_family) {
        நிலை AF_INET:
                திரும்பு அளவு(கட்டமைப்பு sockaddr_in);
        நிலை AF_INET6:
                திரும்பு அளவு(கட்டமைப்பு sockaddr_in6);
        நிலை AF_UNIX:
                திரும்பு sockaddr_un_len(&sa->un);
        நிலை AF_PACKET:
                திரும்பு sockaddr_ll_len(&sa->ll);
        நிலை AF_NETLINK:
                திரும்பு அளவு(கட்டமைப்பு sockaddr_nl);
        நிலை AF_VSOCK:
                திரும்பு அளவு(கட்டமைப்பு sockaddr_vm);
        இயல்பு:
                assert_not_reached();
        }
}

முழு socket_ioctl_fd(வெறுமை) {
        முழு fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        எனில் (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        எனில் (fd < 0)
                திரும்பு -errno;

        திரும்பு fd;
}

முழு sockaddr_un_unlink(மாறிலி கட்டமைப்பு sockaddr_un *sa) {
        மாறிலி எழுத்து *p, * nul;

        ப்ரதிஜ்ஞா(sa);

        எனில் (sa->sun_family != AF_UNIX)
                திரும்பு -EPROTOTYPE;

        எனில் (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                திரும்பு 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, அளவு(sa->sun_path));
        எனில் (nul)
                p = sa->sun_path;
        இல்லையெனில்
                p = memdupa_suffix0(sa->sun_path, அளவு(sa->sun_path));

        எனில் (unlink(p) < 0)
                திரும்பு -errno;

        திரும்பு 1;
}

முழு sockaddr_un_set_path(கட்டமைப்பு sockaddr_un *ret, மாறிலி எழுத்து *path) {
        size_t l;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        எனில் (l < 2)
                திரும்பு -EINVAL;
        எனில் (!IN_SET(path[0], '/', '@'))
                திரும்பு -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        எனில் (l+1 > அளவு(ret->sun_path))
                திரும்பு path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (கட்டமைப்பு sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        எனில் (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                திரும்பு (முழு) (offsetof(கட்டமைப்பு sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } இல்லையெனில் {
                ப்ரதிஜ்ஞா(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                திரும்பு (முழு) (offsetof(கட்டமைப்பு sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

முழு getsockopt_int(முழு fd, முழு level, முழு optname, முழு *ret) {
        முழு v;
        socklen_t sl = அளவு(v);

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (getsockopt(fd, level, optname, &v, &sl) < 0)
                திரும்பு negative_errno();
        எனில் (sl != அளவு(v))
                திரும்பு -EIO;

        *ret = v;
        திரும்பு 0;
}

முழு socket_bind_to_ifname(முழு fd, மாறிலி எழுத்து *ifname) {
        ப்ரதிஜ்ஞா(fd >= 0);

        /* Call with NULL to drop binding */

        திரும்பு RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

முழு socket_bind_to_ifindex(முழு fd, முழு ifindex) {
        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (ifindex <= 0)
                /* Drop binding */
                திரும்பு RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, வெற்று, 0));

        திரும்பு setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

முழு socket_autobind(முழு fd, எழுத்து **ret_name) {
        _cleanup_free_ எழுத்து *name = வெற்று;
        uint64_t random;
        முழு r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        ப்ரதிஜ்ஞா(fd >= 0);

        random = random_u64();

        எனில் (asprintf(&name, "@%" PRIu64, random) < 0)
                திரும்பு -ENOMEM;

        ஒன்றிப்பு sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < அளவு(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (bind(fd, &sa.sa, r) < 0)
                திரும்பு -errno;

        எனில் (ret_name)
                *ret_name = TAKE_PTR(name);
        திரும்பு 0;
}

ssize_t recvmsg_safe(முழு sockfd, கட்டமைப்பு msghdr *msg, முழு flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        ப்ரதிஜ்ஞா(sockfd >= 0);
        ப்ரதிஜ்ஞா(msg);

        n = recvmsg(sockfd, msg, flags);
        எனில் (n < 0)
                திரும்பு -errno;

        எனில் (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                திரும்பு FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        திரும்பு n;
}

முழு socket_get_family(முழு fd) {
        முழு af;
        socklen_t sl = அளவு(af);

        எனில் (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                திரும்பு -errno;

        எனில் (sl != அளவு(af))
                திரும்பு -EINVAL;

        திரும்பு af;
}

முழு socket_set_recvpktinfo(முழு fd, முழு af, bool b) {

        எனில் (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                எனில் (af < 0)
                        திரும்பு af;
        }

        தேர்வு (af) {

        நிலை AF_INET:
                திரும்பு setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        நிலை AF_INET6:
                திரும்பு setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        நிலை AF_NETLINK:
                திரும்பு setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        நிலை AF_PACKET:
                திரும்பு setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு socket_set_unicast_if(முழு fd, முழு af, முழு ifi) {
        be32_t ifindex_be = htobe32(ifi);

        எனில் (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                எனில் (af < 0)
                        திரும்பு af;
        }

        தேர்வு (af) {

        நிலை AF_INET:
                திரும்பு RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, அளவு(ifindex_be)));

        நிலை AF_INET6:
                திரும்பு RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, அளவு(ifindex_be)));

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு socket_set_option(முழு fd, முழு af, முழு opt_ipv4, முழு opt_ipv6, முழு val) {
        எனில் (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                எனில் (af < 0)
                        திரும்பு af;
        }

        தேர்வு (af) {

        நிலை AF_INET:
                திரும்பு setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        நிலை AF_INET6:
                திரும்பு setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }
}

முழு socket_get_mtu(முழு fd, முழு af, size_t *ret) {
        முழு mtu, r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                எனில் (af < 0)
                        திரும்பு af;
        }

        தேர்வு (af) {

        நிலை AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                நிறுத்து;

        நிலை AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                நிறுத்து;

        இயல்பு:
                திரும்பு -EAFNOSUPPORT;
        }

        எனில் (r < 0)
                திரும்பு r;
        எனில் (mtu <= 0)
                திரும்பு -EINVAL;

        *ret = (size_t) mtu;
        திரும்பு 0;
}

நிலையான முழு connect_unix_path_simple(முழு fd, மாறிலி எழுத்து *path) {
        ஒன்றிப்பு sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(path);

        l = strlen(path);
        ப்ரதிஜ்ஞா(l > 0);
        ப்ரதிஜ்ஞா(l < அளவு(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        திரும்பு RET_NERRNO(connect(fd, &sa.sa, offsetof(கட்டமைப்பு sockaddr_un, sun_path) + l + 1));
}

நிலையான முழு connect_unix_inode(முழு fd, முழு inode_fd) {
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(inode_fd >= 0);

        திரும்பு connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

முழு connect_unix_path(முழு fd, முழு dir_fd, மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு inode_fd = -EBADF;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        எனில் (!path) {
                எனில் (dir_fd < 0)
                        திரும்பு -EISDIR;

                திரும்பு connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        எனில் (isempty(path))
                திரும்பு -EINVAL;

        /* Shortcut for the simple case */
        எனில் (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(கட்டமைப்பு sockaddr_un, sun_path))
                திரும்பு connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        எனில் (dir_fd == XAT_FDROOT) {
                _cleanup_free_ எழுத்து *j = strjoin("/", path);
                எனில் (!j)
                        திரும்பு -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } இல்லையெனில்
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        எனில் (inode_fd < 0)
                திரும்பு -errno;

        திரும்பு connect_unix_inode(fd, inode_fd);
}

முழு socket_address_parse_unix(SocketAddress *ret_address, மாறிலி எழுத்து *s) {
        கட்டமைப்பு sockaddr_un un;
        முழு r;

        ப்ரதிஜ்ஞா(ret_address);
        ப்ரதிஜ்ஞா(s);

        எனில் (!IN_SET(*s, '/', '@'))
                திரும்பு -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        எனில் (r < 0)
                திரும்பு r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        திரும்பு 0;
}

முழு socket_address_equal_unix(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        SocketAddress socket_a, socket_b;
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = socket_address_parse_unix(&socket_a, a);
        எனில் (r < 0)
                திரும்பு r;

        r = socket_address_parse_unix(&socket_b, b);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

முழு vsock_parse_port(மாறிலி எழுத்து *s, unsigned *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (!s)
                திரும்பு -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        எனில் (r < 0)
                திரும்பு r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        எனில் (u == VMADDR_PORT_ANY)
                திரும்பு -EINVAL;

        *ret = u;
        திரும்பு 0;
}

முழு vsock_parse_cid(மாறிலி எழுத்து *s, unsigned *ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (!s)
                திரும்பு -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        எனில் (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        இல்லையெனில் எனில் (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        இல்லையெனில் எனில் (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        இல்லையெனில் எனில் (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        இல்லையெனில்
                திரும்பு safe_atou(s, ret);

        திரும்பு 0;
}

முழு socket_address_parse_vsock(SocketAddress *ret_address, மாறிலி எழுத்து *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ எழுத்து *n = வெற்று;
        மாறிலி எழுத்து *e, *cid_start;
        unsigned port, cid;
        முழு type, r;

        ப்ரதிஜ்ஞா(ret_address);
        ப்ரதிஜ்ஞா(s);

        எனில் ((cid_start = startswith(s, "vsock:")))
                type = 0;
        இல்லையெனில் எனில் ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        இல்லையெனில் எனில் ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        இல்லையெனில் எனில் ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        இல்லையெனில்
                திரும்பு -EPROTO;

        e = strchr(cid_start, ':');
        எனில் (!e)
                திரும்பு -EINVAL;

        r = vsock_parse_port(e+1, &port);
        எனில் (r < 0)
                திரும்பு r;

        n = strndup(cid_start, e - cid_start);
        எனில் (!n)
                திரும்பு -ENOMEM;

        எனில் (isempty(n))
                cid = VMADDR_CID_ANY;
        இல்லையெனில் {
                r = vsock_parse_cid(n, &cid);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = அளவு(கட்டமைப்பு sockaddr_vm),
        };

        திரும்பு 0;
}

முழு netlink_socket_get_multicast_groups(முழு fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = வெற்று;
        socklen_t len = 0, old_len;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, வெற்று, &len) < 0)
                திரும்பு -errno;

        எனில் (len == 0)
                செல் finalize;

        groups = new0(uint32_t, len);
        எனில் (!groups)
                திரும்பு -ENOMEM;

        old_len = len;

        எனில் (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                திரும்பு -errno;

        எனில் (old_len != len)
                திரும்பு -EIO;

finalize:
        எனில் (ret_len)
                *ret_len = len;
        எனில் (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        திரும்பு 0;
}

முழு socket_get_cookie(முழு fd, uint64_t *ret) {
        ப்ரதிஜ்ஞா(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = அளவு(cookie);
        எனில் (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                திரும்பு -errno;

        ப்ரதிஜ்ஞா(cookie_len == அளவு(cookie));
        எனில் (ret)
                *ret = cookie;

        திரும்பு 0;
}

வெறுமை cmsg_close_all(கட்டமைப்பு msghdr *mh) {
        ப்ரதிஜ்ஞா(mh);

        கட்டமைப்பு cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                எனில் (cmsg->cmsg_level != SOL_SOCKET)
                        தொடர்;

                எனில் (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, முழு),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / அளவு(முழு));
                இல்லையெனில் எனில் (cmsg->cmsg_type == SCM_PIDFD) {
                        ப்ரதிஜ்ஞா(cmsg->cmsg_len == CMSG_LEN(அளவு(முழு)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, முழு));
                }
        }
}

முழு tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        தேர்வு (IPTOS_PREC(tos)) {
        நிலை IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        நிலை IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                திரும்பு TC_PRIO_CONTROL;

        நிலை IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        நிலை IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                திரும்பு TC_PRIO_INTERACTIVE;

        நிலை IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        நிலை IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                திரும்பு TC_PRIO_INTERACTIVE_BULK;

        நிலை IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                திரும்பு TC_PRIO_BULK;

        நிலை IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        இயல்பு:
                திரும்பு TC_PRIO_BESTEFFORT;
        }
}

முழு socket_xattr_supported(வெறுமை) {
        முழு r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        நிலையான முழு cached = -1;
        எனில் (cached >= 0)
                திரும்பு cached;

        _cleanup_close_ முழு fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        எனில் (fd < 0)
                திரும்பு -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ வெற்று, AT_EMPTY_PATH, "user.testxxx", "1");
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                திரும்பு (cached = false);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to set test xattr on socket: %m");

        திரும்பு (cached = true);
}
