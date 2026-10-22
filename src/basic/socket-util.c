/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/if.h>
#अन्तर्भाव <linux/if_arp.h>
#अन्तर्भाव <linux/if_ether.h>
#अन्तर्भाव <linux/if_infiniband.h>
#अन्तर्भाव <linux/pkt_sched.h>
#अन्तर्भाव <mqueue.h>
#अन्तर्भाव <net/if.h>
#अन्तर्भाव <netdb.h>
#अन्तर्भाव <netinet/ip.h>
#अन्तर्भाव <poll.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "format-ifname.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "in-addr-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "sparse-endian.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "sysctl-util.h"
#अन्तर्भाव "user-util.h"
#अन्तर्भाव "xattr-util.h"

#जइ ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#अण्णहा
#  define IDN_FLAGS 0
#endif

स्थिर ध्रुव वर्ण* ध्रुव socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, पूर्णाङ्क);

पूर्णाङ्क socket_address_verify(ध्रुव SocketAddress *a, bool strict) {
        प्रतिज्ञा(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        विकल्प (socket_address_family(a)) {

        पक्ष AF_INET:
                जइ (a->size != परिमाण(संरचना sockaddr_in))
                        प्रत्यागम -EINVAL;

                जइ (a->sockaddr.in.sin_port == 0)
                        प्रत्यागम -EINVAL;

                जइ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        प्रत्यागम -EINVAL;

                प्रत्यागम 0;

        पक्ष AF_INET6:
                जइ (a->size != परिमाण(संरचना sockaddr_in6))
                        प्रत्यागम -EINVAL;

                जइ (a->sockaddr.in6.sin6_port == 0)
                        प्रत्यागम -EINVAL;

                जइ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        प्रत्यागम -EINVAL;

                प्रत्यागम 0;

        पक्ष AF_UNIX:
                जइ (a->size < offsetof(संरचना sockaddr_un, sun_path))
                        प्रत्यागम -EINVAL;
                जइ (a->size > परिमाण(संरचना sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        प्रत्यागम -EINVAL;

                जइ (a->size > offsetof(संरचना sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        ध्रुव वर्ण *e;

                        e = memchr(a->sockaddr.un.sun_path, 0, परिमाण(a->sockaddr.un.sun_path));
                        जइ (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                जइ (a->size != offsetof(संरचना sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        प्रत्यागम -EINVAL;
                        } अण्णहा {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                जइ (!IN_SET(a->size, परिमाण(a->sockaddr.un.sun_path), परिमाण(a->sockaddr.un.sun_path)+1))
                                        प्रत्यागम -EINVAL;
                        }
                }

                जइ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        प्रत्यागम -EINVAL;

                प्रत्यागम 0;

        पक्ष AF_NETLINK:

                जइ (a->size != परिमाण(संरचना sockaddr_nl))
                        प्रत्यागम -EINVAL;

                जइ (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        प्रत्यागम -EINVAL;

                प्रत्यागम 0;

        पक्ष AF_VSOCK:
                जइ (a->size != परिमाण(संरचना sockaddr_vm))
                        प्रत्यागम -EINVAL;

                जइ (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        प्रत्यागम -EINVAL;

                प्रत्यागम 0;

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }
}

पूर्णाङ्क socket_address_print(ध्रुव SocketAddress *a, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        जइ (r < 0)
                प्रत्यागम r;

        जइ (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ वर्ण *sfamily = रिक्त;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                जइ (r < 0)
                        प्रत्यागम r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                जइ (r < 0)
                        प्रत्यागम -ENOMEM;

                प्रत्यागम 0;
        }

        प्रत्यागम sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(ध्रुव SocketAddress *a) {
        प्रतिज्ञा(a);

        प्रत्यागम
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(ध्रुव SocketAddress *a, ध्रुव SocketAddress *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Invalid addresses are unequal to all */
        जइ (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                प्रत्यागम false;

        जइ (a->type != b->type)
                प्रत्यागम false;

        जइ (socket_address_family(a) != socket_address_family(b))
                प्रत्यागम false;

        विकल्प (socket_address_family(a)) {

        पक्ष AF_INET:
                जइ (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        प्रत्यागम false;

                जइ (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        प्रत्यागम false;

                भङ्ग;

        पक्ष AF_INET6:
                जइ (memcmp(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, परिमाण(a->sockaddr.in6.sin6_addr)) != 0)
                        प्रत्यागम false;

                जइ (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        प्रत्यागम false;

                भङ्ग;

        पक्ष AF_UNIX:
                जइ (a->size <= offsetof(संरचना sockaddr_un, sun_path) ||
                    b->size <= offsetof(संरचना sockaddr_un, sun_path))
                        प्रत्यागम false;

                जइ ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        प्रत्यागम false;

                जइ (a->sockaddr.un.sun_path[0]) {
                        जइ (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                प्रत्यागम false;
                } अण्णहा {
                        जइ (a->size != b->size)
                                प्रत्यागम false;

                        जइ (memcmp(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                प्रत्यागम false;
                }

                भङ्ग;

        पक्ष AF_NETLINK:
                जइ (a->protocol != b->protocol)
                        प्रत्यागम false;

                जइ (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        प्रत्यागम false;

                भङ्ग;

        पक्ष AF_VSOCK:
                जइ (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        प्रत्यागम false;

                जइ (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        प्रत्यागम false;

                भङ्ग;

        स्वभाव:
                /* Cannot compare, so we assume the addresses are different */
                प्रत्यागम false;
        }

        प्रत्यागम true;
}

ध्रुव वर्ण* socket_address_get_path(ध्रुव SocketAddress *a) {
        प्रतिज्ञा(a);

        जइ (socket_address_family(a) != AF_UNIX)
                प्रत्यागम रिक्त;

        जइ (a->sockaddr.un.sun_path[0] == 0)
                प्रत्यागम रिक्त;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(परिमाण(सङ्घ sockaddr_union) >= परिमाण(संरचना sockaddr_un)+1);
        प्रत्यागम a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(शून्य) {
        स्थिर पूर्णाङ्क cached = -1;

        जइ (cached < 0) {

                जइ (access("/proc/net/if_inet6", F_OK) < 0) {

                        जइ (errno != ENOENT) {
                                log_debug_errno(errno, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                प्रत्यागम false;
                        }

                        cached = false;
                } अण्णहा
                        cached = true;
        }

        प्रत्यागम cached;
}

bool socket_ipv6_is_enabled(शून्य) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        जइ (!socket_ipv6_is_supported())
                प्रत्यागम false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        जइ (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                प्रत्यागम true;
        }

        r = parse_boolean(v);
        जइ (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                प्रत्यागम true;
        }

        प्रत्यागम !r;
}

bool socket_address_matches_fd(ध्रुव SocketAddress *a, पूर्णाङ्क fd) {
        SocketAddress b;
        socklen_t solen;

        प्रतिज्ञा(a);
        प्रतिज्ञा(fd >= 0);

        b.size = परिमाण(b.sockaddr);
        जइ (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                प्रत्यागम false;

        जइ (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                प्रत्यागम false;

        solen = परिमाण(b.type);
        जइ (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                प्रत्यागम false;

        जइ (b.type != a->type)
                प्रत्यागम false;

        जइ (a->protocol != 0)  {
                solen = परिमाण(b.protocol);
                जइ (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        प्रत्यागम false;

                जइ (b.protocol != a->protocol)
                        प्रत्यागम false;
        }

        प्रत्यागम socket_address_equal(a, &b);
}

पूर्णाङ्क sockaddr_port(ध्रुव संरचना sockaddr *_sa, unsigned *ret_port) {
        ध्रुव सङ्घ sockaddr_union *sa = (ध्रुव सङ्घ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        प्रतिज्ञा(sa);
        प्रतिज्ञा(ret_port);

        विकल्प (sa->sa.sa_family) {

        पक्ष AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                प्रत्यागम 0;

        पक्ष AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                प्रत्यागम 0;

        पक्ष AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                प्रत्यागम 0;

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }
}

ध्रुव सङ्घ in_addr_union *sockaddr_in_addr(ध्रुव संरचना sockaddr *_sa) {
        ध्रुव सङ्घ sockaddr_union *sa = (ध्रुव सङ्घ sockaddr_union*) _sa;

        जइ (!sa)
                प्रत्यागम रिक्त;

        विकल्प (sa->sa.sa_family) {

        पक्ष AF_INET:
                प्रत्यागम (ध्रुव सङ्घ in_addr_union*) &sa->in.sin_addr;

        पक्ष AF_INET6:
                प्रत्यागम (ध्रुव सङ्घ in_addr_union*) &sa->in6.sin6_addr;

        स्वभाव:
                प्रत्यागम रिक्त;
        }
}

पूर्णाङ्क sockaddr_set_in_addr(
                सङ्घ sockaddr_union *u,
                पूर्णाङ्क family,
                ध्रुव सङ्घ in_addr_union *a,
                uint16_t port) {

        प्रतिज्ञा(u);
        प्रतिज्ञा(a);

        विकल्प (family) {

        पक्ष AF_INET:
                u->in = (संरचना sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                प्रत्यागम 0;

        पक्ष AF_INET6:
                u->in6 = (संरचना sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                प्रत्यागम 0;

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;

        }
}

पूर्णाङ्क sockaddr_pretty(
                ध्रुव संरचना sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                वर्ण **ret) {

        सङ्घ sockaddr_union *sa = (सङ्घ sockaddr_union*) _sa;
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(sa);
        प्रतिज्ञा(salen >= परिमाण(sa->sa.sa_family));
        प्रतिज्ञा(ret);

        विकल्प (sa->sa.sa_family) {

        पक्ष AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                जइ (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                अण्णहा
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                जइ (r < 0)
                        प्रत्यागम -ENOMEM;
                भङ्ग;
        }

        पक्ष AF_INET6: {
                स्थिर ध्रुव unsigned वर्ण ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                जइ (translate_ipv6 &&
                    memcmp(&sa->in6.sin6_addr, ipv4_prefix, परिमाण(ipv4_prefix)) == 0) {
                        ध्रुव uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        जइ (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        अण्णहा
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        जइ (r < 0)
                                प्रत्यागम -ENOMEM;
                } अण्णहा {
                        ध्रुव वर्ण *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        जइ (include_port) {
                                जइ (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        प्रत्यागम -ENOMEM;
                        } अण्णहा {
                                जइ (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                अण्णहा
                                        p = strdup(a);
                                जइ (!p)
                                        प्रत्यागम -ENOMEM;
                        }
                }

                भङ्ग;
        }

        पक्ष AF_UNIX:
                जइ (salen <= offsetof(संरचना sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == offsetof(संरचना sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                अण्णहा {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        वर्ण *path = (वर्ण*) sa->un_buffer + offsetof(संरचना sockaddr_un, sun_path);
                        size_t path_len = salen - offsetof(संरचना sockaddr_un, sun_path);

                        जइ (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ वर्ण *e = रिक्त;

                                e = cescape_length(path + 1, path_len - 1);
                                जइ (!e)
                                        प्रत्यागम -ENOMEM;

                                p = strjoin("@", e);
                        } अण्णहा {
                                जइ (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                जइ (!p)
                        प्रत्यागम -ENOMEM;

                भङ्ग;

        पक्ष AF_VSOCK:
                जइ (include_port) {
                        जइ (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        अण्णहा
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } अण्णहा
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                जइ (r < 0)
                        प्रत्यागम -ENOMEM;
                भङ्ग;

        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }

        *ret = p;
        प्रत्यागम 0;
}

पूर्णाङ्क getpeername_pretty(पूर्णाङ्क fd, bool include_port, वर्ण **ret) {
        सङ्घ sockaddr_union sa;
        socklen_t salen = परिमाण(sa);
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जइ (getpeername(fd, &sa.sa, &salen) < 0)
                प्रत्यागम -errno;

        जइ (sa.sa.sa_family == AF_UNIX) {
                संरचना ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                जइ (r < 0)
                        प्रत्यागम r;

                जइ (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        प्रत्यागम -ENOMEM;

                प्रत्यागम 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        प्रत्यागम sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

पूर्णाङ्क getsockname_pretty(पूर्णाङ्क fd, वर्ण **ret) {
        सङ्घ sockaddr_union sa;
        socklen_t salen = परिमाण(sa);

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जइ (getsockname(fd, &sa.sa, &salen) < 0)
                प्रत्यागम -errno;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        प्रत्यागम sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

पूर्णाङ्क socknameinfo_pretty(ध्रुव संरचना sockaddr *sa, socklen_t salen, वर्ण **ret) {
        वर्ण host[NI_MAXHOST];
        पूर्णाङ्क r;

        प्रतिज्ञा(sa);
        प्रतिज्ञा(salen >= परिमाण(sa_family_t));
        प्रतिज्ञा(ret);

        r = getnameinfo(sa, salen, host, परिमाण(host), /* service= */ रिक्त, /* service_len= */ 0, IDN_FLAGS);
        जइ (r != 0) {
                जइ (r == EAI_MEMORY)
                        प्रत्यागम log_oom_debug();
                जइ (r == EAI_SYSTEM)
                        log_debug_errno(errno, "getnameinfo() failed, ignoring: %m");
                अण्णहा
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                प्रत्यागम sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        प्रत्यागम strdup_to(ret, host);
}

स्थिर ध्रुव वर्ण* ध्रुव netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, पूर्णाङ्क, INT_MAX);

bool sockaddr_equal(ध्रुव सङ्घ sockaddr_union *a, ध्रुव सङ्घ sockaddr_union *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        जइ (a->sa.sa_family != b->sa.sa_family)
                प्रत्यागम false;

        जइ (a->sa.sa_family == AF_INET)
                प्रत्यागम a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        जइ (a->sa.sa_family == AF_INET6)
                प्रत्यागम memcmp(&a->in6.sin6_addr, &b->in6.sin6_addr, परिमाण(a->in6.sin6_addr)) == 0;

        जइ (a->sa.sa_family == AF_VSOCK)
                प्रत्यागम a->vm.svm_cid == b->vm.svm_cid;

        प्रत्यागम false;
}

पूर्णाङ्क fd_set_sndbuf(पूर्णाङ्क fd, size_t n, bool increase) {
        पूर्णाङ्क r, मानम्;
        socklen_t l = परिमाण(मानम्);

        जइ (n > INT_MAX)
                प्रत्यागम -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मानम्, &l);
        जइ (r >= 0 && l == परिमाण(मानम्) && increase ? (size_t) मानम् >= n*2 : (size_t) मानम् == n*2)
                प्रत्यागम 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        जइ (r < 0)
                प्रत्यागम r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = परिमाण(मानम्);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मानम्, &l);
        जइ (r >= 0 && l == परिमाण(मानम्) && increase ? (size_t) मानम् >= n*2 : (size_t) मानम् == n*2)
                प्रत्यागम 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

पूर्णाङ्क fd_set_rcvbuf(पूर्णाङ्क fd, size_t n, bool increase) {
        पूर्णाङ्क r, मानम्;
        socklen_t l = परिमाण(मानम्);

        जइ (n > INT_MAX)
                प्रत्यागम -ERANGE;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मानम्, &l);
        जइ (r >= 0 && l == परिमाण(मानम्) && increase ? (size_t) मानम् >= n*2 : (size_t) मानम् == n*2)
                प्रत्यागम 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        जइ (r < 0)
                प्रत्यागम r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = परिमाण(मानम्);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मानम्, &l);
        जइ (r >= 0 && l == परिमाण(मानम्) && increase ? (size_t) मानम् >= n*2 : (size_t) मानम् == n*2)
                प्रत्यागम 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

स्थिर ध्रुव वर्ण* ध्रुव ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, पूर्णाङ्क, 0xff);

bool ifname_valid_char(वर्ण a) {
        जइ ((unsigned वर्ण) a >= 127U)
                प्रत्यागम false;

        जइ ((unsigned वर्ण) a <= 32U)
                प्रत्यागम false;

        जइ (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                प्रत्यागम false;

        प्रत्यागम true;
}

bool ifname_valid_full(ध्रुव वर्ण *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        प्रतिज्ञा(!(flags & ~_IFNAME_VALID_ALL));

        जइ (isempty(p))
                प्रत्यागम false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        जइ (parse_ifindex(p) >= 0)
                प्रत्यागम flags & IFNAME_VALID_NUMERIC;

        जइ (flags & IFNAME_VALID_ALTERNATIVE) {
                जइ (strlen(p) >= ALTIFNAMSIZ)
                        प्रत्यागम false;
        } अण्णहा {
                जइ (strlen(p) >= IFNAMSIZ)
                        प्रत्यागम false;
        }

        जइ (dot_or_dot_dot(p))
                प्रत्यागम false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        जइ (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                प्रत्यागम false;

        प्रति (ध्रुव वर्ण *t = p; *t; t++) {
                जइ (!ifname_valid_char(*t))
                        प्रत्यागम false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        जइ (numeric)
                प्रत्यागम false;

        प्रत्यागम true;
}

bool address_label_valid(ध्रुव वर्ण *p) {

        POINTER_MAY_BE_NULL(p);

        जइ (isempty(p))
                प्रत्यागम false;

        जइ (strlen(p) >= IFNAMSIZ)
                प्रत्यागम false;

        जाव (*p) {
                जइ ((uint8_t) *p >= 127U)
                        प्रत्यागम false;

                जइ ((uint8_t) *p <= 31U)
                        प्रत्यागम false;
                p++;
        }

        प्रत्यागम true;
}

पूर्णाङ्क getpeercred(पूर्णाङ्क fd, संरचना ucred *ucred) {
        socklen_t n = परिमाण(संरचना ucred);
        संरचना ucred u;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ucred);

        जइ (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                प्रत्यागम -errno;

        जइ (n != परिमाण(संरचना ucred))
                प्रत्यागम -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        जइ (!pid_is_valid(u.pid))
                प्रत्यागम -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        प्रत्यागम 0;
}

पूर्णाङ्क getpeersec(पूर्णाङ्क fd, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        socklen_t n = 64;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        प्रति (;;) {
                s = new0(वर्ण, n+1);
                जइ (!s)
                        प्रत्यागम -ENOMEM;

                जइ (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        भङ्ग;
                }

                जइ (errno != ERANGE)
                        प्रत्यागम -errno;

                s = mfree(s);
        }

        जइ (isempty(s))
                प्रत्यागम -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        प्रत्यागम 0;
}

पूर्णाङ्क getpeergroups(पूर्णाङ्क fd, gid_t **ret) {
        socklen_t n = परिमाण(gid_t) * 64U;
        _cleanup_free_ gid_t *d = रिक्त;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        पूर्णाङ्क ngroups_max = sysconf_ngroups_max();
        जइ (ngroups_max > 0)
                n = MAX(n, परिमाण(gid_t) * (socklen_t) ngroups_max);

        प्रति (;;) {
                d = malloc(n);
                जइ (!d)
                        प्रत्यागम -ENOMEM;

                जइ (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        भङ्ग;

                जइ (errno != ERANGE)
                        प्रत्यागम -errno;

                d = mfree(d);
        }

        assert_se(n % परिमाण(gid_t) == 0);
        n /= परिमाण(gid_t);

        जइ (n > INT_MAX)
                प्रत्यागम -E2BIG;

        *ret = TAKE_PTR(d);

        प्रत्यागम (पूर्णाङ्क) n;
}

पूर्णाङ्क getpeerpidfd(पूर्णाङ्क fd) {
        socklen_t n = परिमाण(पूर्णाङ्क);
        पूर्णाङ्क pidfd = -EBADF;

        प्रतिज्ञा(fd >= 0);

        जइ (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                प्रत्यागम -errno;

        जइ (n != परिमाण(पूर्णाङ्क))
                प्रत्यागम -EIO;

        प्रत्यागम pidfd;
}

पूर्णाङ्क getpeerpidref(पूर्णाङ्क fd, PidRef *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        पूर्णाङ्क pidfd = getpeerpidfd(fd);
        जइ (pidfd < 0) {
                जइ (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        प्रत्यागम pidfd;

                संरचना ucred ucred;
                r = getpeercred(fd, &ucred);
                जइ (r < 0)
                        प्रत्यागम r;

                प्रत्यागम pidref_set_pid(ret, ucred.pid);
        }

        प्रत्यागम pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                पूर्णाङ्क transport_fd,
                पूर्णाङ्क fd,
                ध्रुव संरचना iovec *iov, size_t iovlen,
                ध्रुव संरचना sockaddr *sa, socklen_t len,
                पूर्णाङ्क flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(परिमाण(पूर्णाङ्क))) control = {};
        संरचना msghdr mh = {
                .msg_name = (संरचना sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (संरचना iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        प्रतिज्ञा(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        जइ (fd < 0 && !iov)
                प्रत्यागम -EINVAL;

        जइ (fd >= 0) {
                संरचना cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = परिमाण(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(परिमाण(पूर्णाङ्क));
                memcpy(CMSG_DATA(cmsg), &fd, परिमाण(पूर्णाङ्क));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        जइ (k < 0)
                प्रत्यागम (ssize_t) -errno;

        प्रत्यागम k;
}

पूर्णाङ्क send_one_fd_sa(
                पूर्णाङ्क transport_fd,
                पूर्णाङ्क fd,
                ध्रुव संरचना sockaddr *sa, socklen_t len,
                पूर्णाङ्क flags) {

        प्रतिज्ञा(fd >= 0);

        प्रत्यागम (पूर्णाङ्क) send_one_fd_iov_sa(transport_fd, fd, रिक्त, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                पूर्णाङ्क transport_fd,
                संरचना iovec *iov, size_t iovlen,
                पूर्णाङ्क flags,
                पूर्णाङ्क *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(परिमाण(पूर्णाङ्क))) control;
        संरचना msghdr mh = {
                .msg_control = &control,
                .msg_controllen = परिमाण(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        संरचना cmsghdr *found;
        ssize_t k;

        प्रतिज्ञा(transport_fd >= 0);
        प्रतिज्ञा(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        जइ (k < 0)
                प्रत्यागम k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(परिमाण(पूर्णाङ्क)));
        जइ (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                जइ (k == 0)
                        प्रत्यागम -EIO;
        }

        जइ (found)
                *ret_fd = *CMSG_TYPED_DATA(found, पूर्णाङ्क);
        अण्णहा
                *ret_fd = -EBADF;

        प्रत्यागम k;
}

पूर्णाङ्क receive_one_fd(पूर्णाङ्क transport_fd, पूर्णाङ्क flags) {
        पूर्णाङ्क fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, रिक्त, 0, flags, &fd);
        जइ (k == 0)
                प्रत्यागम fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        प्रतिज्ञा(k < 0);
        प्रत्यागम (पूर्णाङ्क) k;
}

ssize_t next_datagram_size_fd(पूर्णाङ्क fd) {
        ssize_t l;
        पूर्णाङ्क k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, रिक्त, 0, MSG_PEEK|MSG_TRUNC);
        जइ (l < 0) {
                जइ (IN_SET(errno, EOPNOTSUPP, EFAULT))
                        गच्छ fallback;

                प्रत्यागम -errno;
        }
        जइ (l == 0)
                गच्छ fallback;

        प्रत्यागम l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        जइ (ioctl(fd, FIONREAD, &k) < 0)
                प्रत्यागम -errno;

        प्रत्यागम (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

पूर्णाङ्क flush_accept(पूर्णाङ्क fd) {

        पूर्णाङ्क r, b;
        socklen_t l = परिमाण(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        जइ (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                प्रत्यागम -errno;

        प्रतिज्ञा(l == परिमाण(b));
        जइ (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                प्रत्यागम -ENOTTY;

        प्रति (unsigned iteration = 0;; iteration++) {
                पूर्णाङ्क cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                जइ (r == -EINTR)
                        अनुवर्त;
                जइ (r <= 0)
                        प्रत्यागम r;

                जइ (iteration >= MAX_FLUSH_ITERATIONS)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, रिक्त, रिक्त, SOCK_NONBLOCK|SOCK_CLOEXEC);
                जइ (cfd < 0) {
                        जइ (errno == EAGAIN)
                                प्रत्यागम 0;

                        जइ (ERRNO_IS_ACCEPT_AGAIN(errno))
                                अनुवर्त;

                        प्रत्यागम -errno;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(पूर्णाङ्क fd) {
        _cleanup_free_ वर्ण *buf = रिक्त;
        संरचना mq_attr attr;
        ssize_t गणनम् = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        प्रति (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                जइ (r == -EINTR)
                        अनुवर्त;
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        प्रत्यागम गणनम्;

                जइ (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        जइ (mq_getattr(fd, &attr) < 0)
                                प्रत्यागम -errno;

                        buf = malloc(attr.mq_msgsize);
                        जइ (!buf)
                                प्रत्यागम -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ रिक्त);
                जइ (l < 0) {
                        जइ (errno == EINTR)
                                अनुवर्त;

                        जइ (errno == EAGAIN)
                                प्रत्यागम गणनम्;

                        प्रत्यागम -errno;
                }

                गणनम् += l;
        }
}

संरचना cmsghdr* cmsg_find(संरचना msghdr *mh, पूर्णाङ्क level, पूर्णाङ्क type, socklen_t length) {
        संरचना cmsghdr *cmsg;

        प्रतिज्ञा(mh);

        CMSG_FOREACH(cmsg, mh)
                जइ (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        प्रत्यागम cmsg;

        प्रत्यागम रिक्त;
}

शून्य* cmsg_find_and_copy_data(संरचना msghdr *mh, पूर्णाङ्क level, पूर्णाङ्क type, शून्य *buf, size_t buf_len) {
        संरचना cmsghdr *cmsg;

        प्रतिज्ञा(mh);
        प्रतिज्ञा(buf);
        प्रतिज्ञा(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        जइ (!cmsg)
                प्रत्यागम रिक्त;

        प्रत्यागम memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

size_t sockaddr_ll_len(ध्रुव संरचना sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        प्रतिज्ञा(sa->sll_family == AF_PACKET);

        size_t mac_len = परिमाण(sa->sll_addr);

        जइ (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (size_t) ETH_ALEN);
        जइ (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (size_t) INFINIBAND_ALEN);

        प्रत्यागम offsetof(संरचना sockaddr_ll, sll_addr) + mac_len;
}

size_t sockaddr_un_len(ध्रुव संरचना sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        प्रतिज्ञा(sa->sun_family == AF_UNIX);

        प्रत्यागम offsetof(संरचना sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, परिमाण(sa->sun_path)-1) :
                        strnlen(sa->sun_path, परिमाण(sa->sun_path))+1);
}

size_t sockaddr_len(ध्रुव सङ्घ sockaddr_union *sa) {
        प्रतिज्ञा(sa);

        विकल्प (sa->sa.sa_family) {
        पक्ष AF_INET:
                प्रत्यागम परिमाण(संरचना sockaddr_in);
        पक्ष AF_INET6:
                प्रत्यागम परिमाण(संरचना sockaddr_in6);
        पक्ष AF_UNIX:
                प्रत्यागम sockaddr_un_len(&sa->un);
        पक्ष AF_PACKET:
                प्रत्यागम sockaddr_ll_len(&sa->ll);
        पक्ष AF_NETLINK:
                प्रत्यागम परिमाण(संरचना sockaddr_nl);
        पक्ष AF_VSOCK:
                प्रत्यागम परिमाण(संरचना sockaddr_vm);
        स्वभाव:
                assert_not_reached();
        }
}

पूर्णाङ्क socket_ioctl_fd(शून्य) {
        पूर्णाङ्क fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        जइ (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        जइ (fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fd;
}

पूर्णाङ्क sockaddr_un_unlink(ध्रुव संरचना sockaddr_un *sa) {
        ध्रुव वर्ण *p, * nul;

        प्रतिज्ञा(sa);

        जइ (sa->sun_family != AF_UNIX)
                प्रत्यागम -EPROTOTYPE;

        जइ (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                प्रत्यागम 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = memchr(sa->sun_path, 0, परिमाण(sa->sun_path));
        जइ (nul)
                p = sa->sun_path;
        अण्णहा
                p = memdupa_suffix0(sa->sun_path, परिमाण(sa->sun_path));

        जइ (unlink(p) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 1;
}

पूर्णाङ्क sockaddr_un_set_path(संरचना sockaddr_un *ret, ध्रुव वर्ण *path) {
        size_t l;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = strlen(path);
        जइ (l < 2)
                प्रत्यागम -EINVAL;
        जइ (!IN_SET(path[0], '/', '@'))
                प्रत्यागम -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        जइ (l+1 > परिमाण(ret->sun_path))
                प्रत्यागम path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (संरचना sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        जइ (path[0] == '@') {
                /* Abstract namespace socket */
                memcpy(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                प्रत्यागम (पूर्णाङ्क) (offsetof(संरचना sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } अण्णहा {
                प्रतिज्ञा(path[0] == '/');

                /* File system socket */
                memcpy(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                प्रत्यागम (पूर्णाङ्क) (offsetof(संरचना sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

पूर्णाङ्क getsockopt_int(पूर्णाङ्क fd, पूर्णाङ्क level, पूर्णाङ्क optname, पूर्णाङ्क *ret) {
        पूर्णाङ्क v;
        socklen_t sl = परिमाण(v);

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जइ (getsockopt(fd, level, optname, &v, &sl) < 0)
                प्रत्यागम negative_errno();
        जइ (sl != परिमाण(v))
                प्रत्यागम -EIO;

        *ret = v;
        प्रत्यागम 0;
}

पूर्णाङ्क socket_bind_to_ifname(पूर्णाङ्क fd, ध्रुव वर्ण *ifname) {
        प्रतिज्ञा(fd >= 0);

        /* Call with NULL to drop binding */

        प्रत्यागम RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

पूर्णाङ्क socket_bind_to_ifindex(पूर्णाङ्क fd, पूर्णाङ्क ifindex) {
        प्रतिज्ञा(fd >= 0);

        जइ (ifindex <= 0)
                /* Drop binding */
                प्रत्यागम RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, रिक्त, 0));

        प्रत्यागम setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

पूर्णाङ्क socket_autobind(पूर्णाङ्क fd, वर्ण **ret_name) {
        _cleanup_free_ वर्ण *name = रिक्त;
        uint64_t random;
        पूर्णाङ्क r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        प्रतिज्ञा(fd >= 0);

        random = random_u64();

        जइ (asprintf(&name, "@%" PRIu64, random) < 0)
                प्रत्यागम -ENOMEM;

        सङ्घ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < परिमाण(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        जइ (r < 0)
                प्रत्यागम r;

        जइ (bind(fd, &sa.sa, r) < 0)
                प्रत्यागम -errno;

        जइ (ret_name)
                *ret_name = TAKE_PTR(name);
        प्रत्यागम 0;
}

ssize_t recvmsg_safe(पूर्णाङ्क sockfd, संरचना msghdr *msg, पूर्णाङ्क flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        प्रतिज्ञा(sockfd >= 0);
        प्रतिज्ञा(msg);

        n = recvmsg(sockfd, msg, flags);
        जइ (n < 0)
                प्रत्यागम -errno;

        जइ (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                प्रत्यागम FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        प्रत्यागम n;
}

पूर्णाङ्क socket_get_family(पूर्णाङ्क fd) {
        पूर्णाङ्क af;
        socklen_t sl = परिमाण(af);

        जइ (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                प्रत्यागम -errno;

        जइ (sl != परिमाण(af))
                प्रत्यागम -EINVAL;

        प्रत्यागम af;
}

पूर्णाङ्क socket_set_recvpktinfo(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {

        जइ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जइ (af < 0)
                        प्रत्यागम af;
        }

        विकल्प (af) {

        पक्ष AF_INET:
                प्रत्यागम setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        पक्ष AF_INET6:
                प्रत्यागम setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        पक्ष AF_NETLINK:
                प्रत्यागम setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        पक्ष AF_PACKET:
                प्रत्यागम setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }
}

पूर्णाङ्क socket_set_unicast_if(पूर्णाङ्क fd, पूर्णाङ्क af, पूर्णाङ्क ifi) {
        be32_t ifindex_be = htobe32(ifi);

        जइ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जइ (af < 0)
                        प्रत्यागम af;
        }

        विकल्प (af) {

        पक्ष AF_INET:
                प्रत्यागम RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, परिमाण(ifindex_be)));

        पक्ष AF_INET6:
                प्रत्यागम RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, परिमाण(ifindex_be)));

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }
}

पूर्णाङ्क socket_set_option(पूर्णाङ्क fd, पूर्णाङ्क af, पूर्णाङ्क opt_ipv4, पूर्णाङ्क opt_ipv6, पूर्णाङ्क val) {
        जइ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जइ (af < 0)
                        प्रत्यागम af;
        }

        विकल्प (af) {

        पक्ष AF_INET:
                प्रत्यागम setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        पक्ष AF_INET6:
                प्रत्यागम setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }
}

पूर्णाङ्क socket_get_mtu(पूर्णाङ्क fd, पूर्णाङ्क af, size_t *ret) {
        पूर्णाङ्क mtu, r;

        प्रतिज्ञा(ret);

        जइ (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जइ (af < 0)
                        प्रत्यागम af;
        }

        विकल्प (af) {

        पक्ष AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                भङ्ग;

        पक्ष AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                भङ्ग;

        स्वभाव:
                प्रत्यागम -EAFNOSUPPORT;
        }

        जइ (r < 0)
                प्रत्यागम r;
        जइ (mtu <= 0)
                प्रत्यागम -EINVAL;

        *ret = (size_t) mtu;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क connect_unix_path_simple(पूर्णाङ्क fd, ध्रुव वर्ण *path) {
        सङ्घ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        size_t l;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(path);

        l = strlen(path);
        प्रतिज्ञा(l > 0);
        प्रतिज्ञा(l < परिमाण(sa.un.sun_path));

        memcpy(sa.un.sun_path, path, l + 1);
        प्रत्यागम RET_NERRNO(connect(fd, &sa.sa, offsetof(संरचना sockaddr_un, sun_path) + l + 1));
}

स्थिर पूर्णाङ्क connect_unix_inode(पूर्णाङ्क fd, पूर्णाङ्क inode_fd) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(inode_fd >= 0);

        प्रत्यागम connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

पूर्णाङ्क connect_unix_path(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क inode_fd = -EBADF;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        जइ (!path) {
                जइ (dir_fd < 0)
                        प्रत्यागम -EISDIR;

                प्रत्यागम connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        जइ (isempty(path))
                प्रत्यागम -EINVAL;

        /* Shortcut for the simple case */
        जइ (dir_fd == AT_FDCWD && strlen(path) < sizeof_field(संरचना sockaddr_un, sun_path))
                प्रत्यागम connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        जइ (dir_fd == XAT_FDROOT) {
                _cleanup_free_ वर्ण *j = strjoin("/", path);
                जइ (!j)
                        प्रत्यागम -ENOMEM;

                inode_fd = open(j, O_PATH|O_CLOEXEC);
        } अण्णहा
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        जइ (inode_fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम connect_unix_inode(fd, inode_fd);
}

पूर्णाङ्क socket_address_parse_unix(SocketAddress *ret_address, ध्रुव वर्ण *s) {
        संरचना sockaddr_un un;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_address);
        प्रतिज्ञा(s);

        जइ (!IN_SET(*s, '/', '@'))
                प्रत्यागम -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        जइ (r < 0)
                प्रत्यागम r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        प्रत्यागम 0;
}

पूर्णाङ्क socket_address_equal_unix(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        SocketAddress socket_a, socket_b;
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = socket_address_parse_unix(&socket_a, a);
        जइ (r < 0)
                प्रत्यागम r;

        r = socket_address_parse_unix(&socket_b, b);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

पूर्णाङ्क vsock_parse_port(ध्रुव वर्ण *s, unsigned *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        जइ (!s)
                प्रत्यागम -EINVAL;

        unsigned u;
        r = safe_atou(s, &u);
        जइ (r < 0)
                प्रत्यागम r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        जइ (u == VMADDR_PORT_ANY)
                प्रत्यागम -EINVAL;

        *ret = u;
        प्रत्यागम 0;
}

पूर्णाङ्क vsock_parse_cid(ध्रुव वर्ण *s, unsigned *ret) {
        प्रतिज्ञा(ret);

        जइ (!s)
                प्रत्यागम -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        जइ (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        अण्णहा जइ (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        अण्णहा जइ (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        अण्णहा जइ (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        अण्णहा
                प्रत्यागम safe_atou(s, ret);

        प्रत्यागम 0;
}

पूर्णाङ्क socket_address_parse_vsock(SocketAddress *ret_address, ध्रुव वर्ण *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ वर्ण *n = रिक्त;
        ध्रुव वर्ण *e, *cid_start;
        unsigned port, cid;
        पूर्णाङ्क type, r;

        प्रतिज्ञा(ret_address);
        प्रतिज्ञा(s);

        जइ ((cid_start = startswith(s, "vsock:")))
                type = 0;
        अण्णहा जइ ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        अण्णहा जइ ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        अण्णहा जइ ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        अण्णहा
                प्रत्यागम -EPROTO;

        e = strchr(cid_start, ':');
        जइ (!e)
                प्रत्यागम -EINVAL;

        r = vsock_parse_port(e+1, &port);
        जइ (r < 0)
                प्रत्यागम r;

        n = strndup(cid_start, e - cid_start);
        जइ (!n)
                प्रत्यागम -ENOMEM;

        जइ (isempty(n))
                cid = VMADDR_CID_ANY;
        अण्णहा {
                r = vsock_parse_cid(n, &cid);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = परिमाण(संरचना sockaddr_vm),
        };

        प्रत्यागम 0;
}

पूर्णाङ्क netlink_socket_get_multicast_groups(पूर्णाङ्क fd, size_t *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = रिक्त;
        socklen_t len = 0, old_len;

        प्रतिज्ञा(fd >= 0);

        जइ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, रिक्त, &len) < 0)
                प्रत्यागम -errno;

        जइ (len == 0)
                गच्छ finalize;

        groups = new0(uint32_t, len);
        जइ (!groups)
                प्रत्यागम -ENOMEM;

        old_len = len;

        जइ (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                प्रत्यागम -errno;

        जइ (old_len != len)
                प्रत्यागम -EIO;

finalize:
        जइ (ret_len)
                *ret_len = len;
        जइ (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        प्रत्यागम 0;
}

पूर्णाङ्क socket_get_cookie(पूर्णाङ्क fd, uint64_t *ret) {
        प्रतिज्ञा(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = परिमाण(cookie);
        जइ (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                प्रत्यागम -errno;

        प्रतिज्ञा(cookie_len == परिमाण(cookie));
        जइ (ret)
                *ret = cookie;

        प्रत्यागम 0;
}

शून्य cmsg_close_all(संरचना msghdr *mh) {
        प्रतिज्ञा(mh);

        संरचना cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                जइ (cmsg->cmsg_level != SOL_SOCKET)
                        अनुवर्त;

                जइ (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, पूर्णाङ्क),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / परिमाण(पूर्णाङ्क));
                अण्णहा जइ (cmsg->cmsg_type == SCM_PIDFD) {
                        प्रतिज्ञा(cmsg->cmsg_len == CMSG_LEN(परिमाण(पूर्णाङ्क)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, पूर्णाङ्क));
                }
        }
}

पूर्णाङ्क tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        विकल्प (IPTOS_PREC(tos)) {
        पक्ष IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        पक्ष IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                प्रत्यागम TC_PRIO_CONTROL;

        पक्ष IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        पक्ष IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                प्रत्यागम TC_PRIO_INTERACTIVE;

        पक्ष IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        पक्ष IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                प्रत्यागम TC_PRIO_INTERACTIVE_BULK;

        पक्ष IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                प्रत्यागम TC_PRIO_BULK;

        पक्ष IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        स्वभाव:
                प्रत्यागम TC_PRIO_BESTEFFORT;
        }
}

पूर्णाङ्क socket_xattr_supported(शून्य) {
        पूर्णाङ्क r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        स्थिर पूर्णाङ्क cached = -1;
        जइ (cached >= 0)
                प्रत्यागम cached;

        _cleanup_close_ पूर्णाङ्क fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        जइ (fd < 0)
                प्रत्यागम -errno;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ रिक्त, AT_EMPTY_PATH, "user.testxxx", "1");
        जइ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                प्रत्यागम (cached = false);
        जइ (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to set test xattr on socket: %m");

        प्रत्यागम (cached = true);
}
