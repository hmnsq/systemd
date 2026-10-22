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

#जर ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#अन्यथा
#  define IDN_FLAGS 0
#endif

स्थिर स्थिरांक अक्षर* स्थिरांक socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, पूर्णांक);

पूर्णांक socket_address_verify(स्थिरांक SocketAddress *a, bool strict) {
        प्रतिज्ञा(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        निवड (socket_address_family(a)) {

        स्थिती AF_INET:
                जर (a->size != आकार(संरचना sockaddr_in))
                        परत -EINVAL;

                जर (a->sockaddr.in.sin_port == 0)
                        परत -EINVAL;

                जर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        परत -EINVAL;

                परत 0;

        स्थिती AF_INET6:
                जर (a->size != आकार(संरचना sockaddr_in6))
                        परत -EINVAL;

                जर (a->sockaddr.in6.sin6_port == 0)
                        परत -EINVAL;

                जर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        परत -EINVAL;

                परत 0;

        स्थिती AF_UNIX:
                जर (a->size < दुरत्व(संरचना sockaddr_un, sun_path))
                        परत -EINVAL;
                जर (a->size > आकार(संरचना sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        परत -EINVAL;

                जर (a->size > दुरत्व(संरचना sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        स्थिरांक अक्षर *e;

                        e = स_प्रथम(a->sockaddr.un.sun_path, 0, आकार(a->sockaddr.un.sun_path));
                        जर (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                जर (a->size != दुरत्व(संरचना sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        परत -EINVAL;
                        } अन्यथा {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                जर (!IN_SET(a->size, आकार(a->sockaddr.un.sun_path), आकार(a->sockaddr.un.sun_path)+1))
                                        परत -EINVAL;
                        }
                }

                जर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        परत -EINVAL;

                परत 0;

        स्थिती AF_NETLINK:

                जर (a->size != आकार(संरचना sockaddr_nl))
                        परत -EINVAL;

                जर (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        परत -EINVAL;

                परत 0;

        स्थिती AF_VSOCK:
                जर (a->size != आकार(संरचना sockaddr_vm))
                        परत -EINVAL;

                जर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        परत -EINVAL;

                परत 0;

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक socket_address_print(स्थिरांक SocketAddress *a, अक्षर **ret) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        जर (r < 0)
                परत r;

        जर (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ अक्षर *sfamily = रिक्त;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                जर (r < 0)
                        परत r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                जर (r < 0)
                        परत -ENOMEM;

                परत 0;
        }

        परत sockaddr_pretty(&a->sockaddr.sa, a->size, false, true, ret);
}

bool socket_address_can_accept(स्थिरांक SocketAddress *a) {
        प्रतिज्ञा(a);

        परत
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(स्थिरांक SocketAddress *a, स्थिरांक SocketAddress *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        /* Invalid addresses are unequal to all */
        जर (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                परत false;

        जर (a->type != b->type)
                परत false;

        जर (socket_address_family(a) != socket_address_family(b))
                परत false;

        निवड (socket_address_family(a)) {

        स्थिती AF_INET:
                जर (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        परत false;

                जर (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        परत false;

                थांबा;

        स्थिती AF_INET6:
                जर (स_भेद(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, आकार(a->sockaddr.in6.sin6_addr)) != 0)
                        परत false;

                जर (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        परत false;

                थांबा;

        स्थिती AF_UNIX:
                जर (a->size <= दुरत्व(संरचना sockaddr_un, sun_path) ||
                    b->size <= दुरत्व(संरचना sockaddr_un, sun_path))
                        परत false;

                जर ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        परत false;

                जर (a->sockaddr.un.sun_path[0]) {
                        जर (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                परत false;
                } अन्यथा {
                        जर (a->size != b->size)
                                परत false;

                        जर (स_भेद(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->size) != 0)
                                परत false;
                }

                थांबा;

        स्थिती AF_NETLINK:
                जर (a->protocol != b->protocol)
                        परत false;

                जर (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        परत false;

                थांबा;

        स्थिती AF_VSOCK:
                जर (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        परत false;

                जर (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        परत false;

                थांबा;

        पूर्वनिर्धारित:
                /* Cannot compare, so we assume the addresses are different */
                परत false;
        }

        परत true;
}

स्थिरांक अक्षर* socket_address_get_path(स्थिरांक SocketAddress *a) {
        प्रतिज्ञा(a);

        जर (socket_address_family(a) != AF_UNIX)
                परत रिक्त;

        जर (a->sockaddr.un.sun_path[0] == 0)
                परत रिक्त;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(आकार(संघ sockaddr_union) >= आकार(संरचना sockaddr_un)+1);
        परत a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(शून्य) {
        स्थिर पूर्णांक cached = -1;

        जर (cached < 0) {

                जर (access("/proc/net/if_inet6", F_OK) < 0) {

                        जर (त्रुटि_सं != ENOENT) {
                                log_debug_errno(त्रुटि_सं, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                परत false;
                        }

                        cached = false;
                } अन्यथा
                        cached = true;
        }

        परत cached;
}

bool socket_ipv6_is_enabled(शून्य) {
        _cleanup_free_ अक्षर *v = रिक्त;
        पूर्णांक r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        जर (!socket_ipv6_is_supported())
                परत false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        जर (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                परत true;
        }

        r = parse_boolean(v);
        जर (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                परत true;
        }

        परत !r;
}

bool socket_address_matches_fd(स्थिरांक SocketAddress *a, पूर्णांक fd) {
        SocketAddress b;
        socklen_t solen;

        प्रतिज्ञा(a);
        प्रतिज्ञा(fd >= 0);

        b.size = आकार(b.sockaddr);
        जर (getsockname(fd, &b.sockaddr.sa, &b.size) < 0)
                परत false;

        जर (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                परत false;

        solen = आकार(b.type);
        जर (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                परत false;

        जर (b.type != a->type)
                परत false;

        जर (a->protocol != 0)  {
                solen = आकार(b.protocol);
                जर (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        परत false;

                जर (b.protocol != a->protocol)
                        परत false;
        }

        परत socket_address_equal(a, &b);
}

पूर्णांक sockaddr_port(स्थिरांक संरचना sockaddr *_sa, अचिन्हित *ret_port) {
        स्थिरांक संघ sockaddr_union *sa = (स्थिरांक संघ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        प्रतिज्ञा(sa);
        प्रतिज्ञा(ret_port);

        निवड (sa->sa.sa_family) {

        स्थिती AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                परत 0;

        स्थिती AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                परत 0;

        स्थिती AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                परत 0;

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

स्थिरांक संघ in_addr_union *sockaddr_in_addr(स्थिरांक संरचना sockaddr *_sa) {
        स्थिरांक संघ sockaddr_union *sa = (स्थिरांक संघ sockaddr_union*) _sa;

        जर (!sa)
                परत रिक्त;

        निवड (sa->sa.sa_family) {

        स्थिती AF_INET:
                परत (स्थिरांक संघ in_addr_union*) &sa->in.sin_addr;

        स्थिती AF_INET6:
                परत (स्थिरांक संघ in_addr_union*) &sa->in6.sin6_addr;

        पूर्वनिर्धारित:
                परत रिक्त;
        }
}

पूर्णांक sockaddr_set_in_addr(
                संघ sockaddr_union *u,
                पूर्णांक family,
                स्थिरांक संघ in_addr_union *a,
                uint16_t port) {

        प्रतिज्ञा(u);
        प्रतिज्ञा(a);

        निवड (family) {

        स्थिती AF_INET:
                u->in = (संरचना sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                परत 0;

        स्थिती AF_INET6:
                u->in6 = (संरचना sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                परत 0;

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;

        }
}

पूर्णांक sockaddr_pretty(
                स्थिरांक संरचना sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                अक्षर **ret) {

        संघ sockaddr_union *sa = (संघ sockaddr_union*) _sa;
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(sa);
        प्रतिज्ञा(salen >= आकार(sa->sa.sa_family));
        प्रतिज्ञा(ret);

        निवड (sa->sa.sa_family) {

        स्थिती AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                जर (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                अन्यथा
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                जर (r < 0)
                        परत -ENOMEM;
                थांबा;
        }

        स्थिती AF_INET6: {
                स्थिर स्थिरांक अचिन्हित अक्षर ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                जर (translate_ipv6 &&
                    स_भेद(&sa->in6.sin6_addr, ipv4_prefix, आकार(ipv4_prefix)) == 0) {
                        स्थिरांक uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        जर (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        अन्यथा
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        जर (r < 0)
                                परत -ENOMEM;
                } अन्यथा {
                        स्थिरांक अक्षर *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        जर (include_port) {
                                जर (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        परत -ENOMEM;
                        } अन्यथा {
                                जर (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                अन्यथा
                                        p = strdup(a);
                                जर (!p)
                                        परत -ENOMEM;
                        }
                }

                थांबा;
        }

        स्थिती AF_UNIX:
                जर (salen <= दुरत्व(संरचना sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == दुरत्व(संरचना sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                अन्यथा {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        अक्षर *path = (अक्षर*) sa->un_buffer + दुरत्व(संरचना sockaddr_un, sun_path);
                        माप_प्रकार path_len = salen - दुरत्व(संरचना sockaddr_un, sun_path);

                        जर (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ अक्षर *e = रिक्त;

                                e = cescape_length(path + 1, path_len - 1);
                                जर (!e)
                                        परत -ENOMEM;

                                p = strjoin("@", e);
                        } अन्यथा {
                                जर (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_length(path, path_len);
                        }
                }
                जर (!p)
                        परत -ENOMEM;

                थांबा;

        स्थिती AF_VSOCK:
                जर (include_port) {
                        जर (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        अन्यथा
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } अन्यथा
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                जर (r < 0)
                        परत -ENOMEM;
                थांबा;

        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }

        *ret = p;
        परत 0;
}

पूर्णांक getpeername_pretty(पूर्णांक fd, bool include_port, अक्षर **ret) {
        संघ sockaddr_union sa;
        socklen_t salen = आकार(sa);
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जर (getpeername(fd, &sa.sa, &salen) < 0)
                परत -त्रुटि_सं;

        जर (sa.sa.sa_family == AF_UNIX) {
                संरचना ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                जर (r < 0)
                        परत r;

                जर (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        परत -ENOMEM;

                परत 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        परत sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

पूर्णांक getsockname_pretty(पूर्णांक fd, अक्षर **ret) {
        संघ sockaddr_union sa;
        socklen_t salen = आकार(sa);

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जर (getsockname(fd, &sa.sa, &salen) < 0)
                परत -त्रुटि_सं;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        परत sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

पूर्णांक socknameinfo_pretty(स्थिरांक संरचना sockaddr *sa, socklen_t salen, अक्षर **ret) {
        अक्षर host[NI_MAXHOST];
        पूर्णांक r;

        प्रतिज्ञा(sa);
        प्रतिज्ञा(salen >= आकार(sa_family_t));
        प्रतिज्ञा(ret);

        r = getnameinfo(sa, salen, host, आकार(host), /* service= */ रिक्त, /* service_len= */ 0, IDN_FLAGS);
        जर (r != 0) {
                जर (r == EAI_MEMORY)
                        परत log_oom_debug();
                जर (r == EAI_SYSTEM)
                        log_debug_errno(त्रुटि_सं, "getnameinfo() failed, ignoring: %m");
                अन्यथा
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                परत sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        परत strdup_to(ret, host);
}

स्थिर स्थिरांक अक्षर* स्थिरांक netlink_family_table[] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, पूर्णांक, पूर्णांक_उच्च);

bool sockaddr_equal(स्थिरांक संघ sockaddr_union *a, स्थिरांक संघ sockaddr_union *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        जर (a->sa.sa_family != b->sa.sa_family)
                परत false;

        जर (a->sa.sa_family == AF_INET)
                परत a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        जर (a->sa.sa_family == AF_INET6)
                परत स_भेद(&a->in6.sin6_addr, &b->in6.sin6_addr, आकार(a->in6.sin6_addr)) == 0;

        जर (a->sa.sa_family == AF_VSOCK)
                परत a->vm.svm_cid == b->vm.svm_cid;

        परत false;
}

पूर्णांक fd_set_sndbuf(पूर्णांक fd, माप_प्रकार n, bool increase) {
        पूर्णांक r, मूल्य;
        socklen_t l = आकार(मूल्य);

        जर (n > पूर्णांक_उच्च)
                परत -दुस्फल;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मूल्य, &l);
        जर (r >= 0 && l == आकार(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                परत 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        जर (r < 0)
                परत r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = आकार(मूल्य);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मूल्य, &l);
        जर (r >= 0 && l == आकार(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                परत 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        जर (r < 0)
                परत r;

        परत 1;
}

पूर्णांक fd_set_rcvbuf(पूर्णांक fd, माप_प्रकार n, bool increase) {
        पूर्णांक r, मूल्य;
        socklen_t l = आकार(मूल्य);

        जर (n > पूर्णांक_उच्च)
                परत -दुस्फल;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मूल्य, &l);
        जर (r >= 0 && l == आकार(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                परत 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        जर (r < 0)
                परत r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = आकार(मूल्य);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मूल्य, &l);
        जर (r >= 0 && l == आकार(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                परत 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        जर (r < 0)
                परत r;

        परत 1;
}

स्थिर स्थिरांक अक्षर* स्थिरांक ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, पूर्णांक, 0xff);

bool ifname_valid_char(अक्षर a) {
        जर ((अचिन्हित अक्षर) a >= 127U)
                परत false;

        जर ((अचिन्हित अक्षर) a <= 32U)
                परत false;

        जर (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                परत false;

        परत true;
}

bool ifname_valid_full(स्थिरांक अक्षर *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        प्रतिज्ञा(!(flags & ~_IFNAME_VALID_ALL));

        जर (isempty(p))
                परत false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        जर (parse_ifindex(p) >= 0)
                परत flags & IFNAME_VALID_NUMERIC;

        जर (flags & IFNAME_VALID_ALTERNATIVE) {
                जर (म_माप(p) >= ALTIFNAMSIZ)
                        परत false;
        } अन्यथा {
                जर (म_माप(p) >= IFNAMSIZ)
                        परत false;
        }

        जर (dot_or_dot_dot(p))
                परत false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        जर (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                परत false;

        साठी (स्थिरांक अक्षर *t = p; *t; t++) {
                जर (!ifname_valid_char(*t))
                        परत false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        जर (numeric)
                परत false;

        परत true;
}

bool address_label_valid(स्थिरांक अक्षर *p) {

        POINTER_MAY_BE_NULL(p);

        जर (isempty(p))
                परत false;

        जर (म_माप(p) >= IFNAMSIZ)
                परत false;

        पर्यंत (*p) {
                जर ((uint8_t) *p >= 127U)
                        परत false;

                जर ((uint8_t) *p <= 31U)
                        परत false;
                p++;
        }

        परत true;
}

पूर्णांक getpeercred(पूर्णांक fd, संरचना ucred *ucred) {
        socklen_t n = आकार(संरचना ucred);
        संरचना ucred u;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ucred);

        जर (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                परत -त्रुटि_सं;

        जर (n != आकार(संरचना ucred))
                परत -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        जर (!pid_is_valid(u.pid))
                परत -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        परत 0;
}

पूर्णांक getpeersec(पूर्णांक fd, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        socklen_t n = 64;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        साठी (;;) {
                s = new0(अक्षर, n+1);
                जर (!s)
                        परत -ENOMEM;

                जर (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        थांबा;
                }

                जर (त्रुटि_सं != दुस्फल)
                        परत -त्रुटि_सं;

                s = mfree(s);
        }

        जर (isempty(s))
                परत -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        परत 0;
}

पूर्णांक getpeergroups(पूर्णांक fd, gid_t **ret) {
        socklen_t n = आकार(gid_t) * 64U;
        _cleanup_free_ gid_t *d = रिक्त;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        पूर्णांक ngroups_max = sysconf_ngroups_max();
        जर (ngroups_max > 0)
                n = MAX(n, आकार(gid_t) * (socklen_t) ngroups_max);

        साठी (;;) {
                d = दो_स्मृति(n);
                जर (!d)
                        परत -ENOMEM;

                जर (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        थांबा;

                जर (त्रुटि_सं != दुस्फल)
                        परत -त्रुटि_सं;

                d = mfree(d);
        }

        assert_se(n % आकार(gid_t) == 0);
        n /= आकार(gid_t);

        जर (n > पूर्णांक_उच्च)
                परत -E2BIG;

        *ret = TAKE_PTR(d);

        परत (पूर्णांक) n;
}

पूर्णांक getpeerpidfd(पूर्णांक fd) {
        socklen_t n = आकार(पूर्णांक);
        पूर्णांक pidfd = -EBADF;

        प्रतिज्ञा(fd >= 0);

        जर (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                परत -त्रुटि_सं;

        जर (n != आकार(पूर्णांक))
                परत -EIO;

        परत pidfd;
}

पूर्णांक getpeerpidref(पूर्णांक fd, PidRef *ret) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        पूर्णांक pidfd = getpeerpidfd(fd);
        जर (pidfd < 0) {
                जर (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        परत pidfd;

                संरचना ucred ucred;
                r = getpeercred(fd, &ucred);
                जर (r < 0)
                        परत r;

                परत pidref_set_pid(ret, ucred.pid);
        }

        परत pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                पूर्णांक transport_fd,
                पूर्णांक fd,
                स्थिरांक संरचना iovec *iov, माप_प्रकार iovlen,
                स्थिरांक संरचना sockaddr *sa, socklen_t len,
                पूर्णांक flags) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(आकार(पूर्णांक))) control = {};
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
        जर (fd < 0 && !iov)
                परत -EINVAL;

        जर (fd >= 0) {
                संरचना cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = आकार(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(आकार(पूर्णांक));
                स_नकल(CMSG_DATA(cmsg), &fd, आकार(पूर्णांक));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        जर (k < 0)
                परत (ssize_t) -त्रुटि_सं;

        परत k;
}

पूर्णांक send_one_fd_sa(
                पूर्णांक transport_fd,
                पूर्णांक fd,
                स्थिरांक संरचना sockaddr *sa, socklen_t len,
                पूर्णांक flags) {

        प्रतिज्ञा(fd >= 0);

        परत (पूर्णांक) send_one_fd_iov_sa(transport_fd, fd, रिक्त, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                पूर्णांक transport_fd,
                संरचना iovec *iov, माप_प्रकार iovlen,
                पूर्णांक flags,
                पूर्णांक *ret_fd) {

        CMSG_BUFFER_TYPE(CMSG_SPACE(आकार(पूर्णांक))) control;
        संरचना msghdr mh = {
                .msg_control = &control,
                .msg_controllen = आकार(control),
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
        जर (k < 0)
                परत k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(आकार(पूर्णांक)));
        जर (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                जर (k == 0)
                        परत -EIO;
        }

        जर (found)
                *ret_fd = *CMSG_TYPED_DATA(found, पूर्णांक);
        अन्यथा
                *ret_fd = -EBADF;

        परत k;
}

पूर्णांक receive_one_fd(पूर्णांक transport_fd, पूर्णांक flags) {
        पूर्णांक fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, रिक्त, 0, flags, &fd);
        जर (k == 0)
                परत fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        प्रतिज्ञा(k < 0);
        परत (पूर्णांक) k;
}

ssize_t next_datagram_size_fd(पूर्णांक fd) {
        ssize_t l;
        पूर्णांक k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, रिक्त, 0, MSG_PEEK|MSG_TRUNC);
        जर (l < 0) {
                जर (IN_SET(त्रुटि_सं, EOPNOTSUPP, EFAULT))
                        जा fallback;

                परत -त्रुटि_सं;
        }
        जर (l == 0)
                जा fallback;

        परत l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        जर (ioctl(fd, FIONREAD, &k) < 0)
                परत -त्रुटि_सं;

        परत (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#define MAX_FLUSH_ITERATIONS 1024

पूर्णांक flush_accept(पूर्णांक fd) {

        पूर्णांक r, b;
        socklen_t l = आकार(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        जर (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                परत -त्रुटि_सं;

        प्रतिज्ञा(l == आकार(b));
        जर (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                परत -ENOTTY;

        साठी (अचिन्हित iteration = 0;; iteration++) {
                पूर्णांक cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                जर (r == -EINTR)
                        सुरूठेवा;
                जर (r <= 0)
                        परत r;

                जर (iteration >= MAX_FLUSH_ITERATIONS)
                        परत log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, रिक्त, रिक्त, SOCK_NONBLOCK|SOCK_CLOEXEC);
                जर (cfd < 0) {
                        जर (त्रुटि_सं == EAGAIN)
                                परत 0;

                        जर (ERRNO_IS_ACCEPT_AGAIN(त्रुटि_सं))
                                सुरूठेवा;

                        परत -त्रुटि_सं;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(पूर्णांक fd) {
        _cleanup_free_ अक्षर *buf = रिक्त;
        संरचना mq_attr attr;
        ssize_t मोजणी = 0;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        साठी (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                जर (r == -EINTR)
                        सुरूठेवा;
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        परत मोजणी;

                जर (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        जर (mq_getattr(fd, &attr) < 0)
                                परत -त्रुटि_सं;

                        buf = दो_स्मृति(attr.mq_msgsize);
                        जर (!buf)
                                परत -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ रिक्त);
                जर (l < 0) {
                        जर (त्रुटि_सं == EINTR)
                                सुरूठेवा;

                        जर (त्रुटि_सं == EAGAIN)
                                परत मोजणी;

                        परत -त्रुटि_सं;
                }

                मोजणी += l;
        }
}

संरचना cmsghdr* cmsg_find(संरचना msghdr *mh, पूर्णांक level, पूर्णांक type, socklen_t length) {
        संरचना cmsghdr *cmsg;

        प्रतिज्ञा(mh);

        CMSG_FOREACH(cmsg, mh)
                जर (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        परत cmsg;

        परत रिक्त;
}

शून्य* cmsg_find_and_copy_data(संरचना msghdr *mh, पूर्णांक level, पूर्णांक type, शून्य *buf, माप_प्रकार buf_len) {
        संरचना cmsghdr *cmsg;

        प्रतिज्ञा(mh);
        प्रतिज्ञा(buf);
        प्रतिज्ञा(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        जर (!cmsg)
                परत रिक्त;

        परत memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

माप_प्रकार sockaddr_ll_len(स्थिरांक संरचना sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        प्रतिज्ञा(sa->sll_family == AF_PACKET);

        माप_प्रकार mac_len = आकार(sa->sll_addr);

        जर (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (माप_प्रकार) ETH_ALEN);
        जर (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (माप_प्रकार) INFINIBAND_ALEN);

        परत दुरत्व(संरचना sockaddr_ll, sll_addr) + mac_len;
}

माप_प्रकार sockaddr_un_len(स्थिरांक संरचना sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        प्रतिज्ञा(sa->sun_family == AF_UNIX);

        परत दुरत्व(संरचना sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, आकार(sa->sun_path)-1) :
                        strnlen(sa->sun_path, आकार(sa->sun_path))+1);
}

माप_प्रकार sockaddr_len(स्थिरांक संघ sockaddr_union *sa) {
        प्रतिज्ञा(sa);

        निवड (sa->sa.sa_family) {
        स्थिती AF_INET:
                परत आकार(संरचना sockaddr_in);
        स्थिती AF_INET6:
                परत आकार(संरचना sockaddr_in6);
        स्थिती AF_UNIX:
                परत sockaddr_un_len(&sa->un);
        स्थिती AF_PACKET:
                परत sockaddr_ll_len(&sa->ll);
        स्थिती AF_NETLINK:
                परत आकार(संरचना sockaddr_nl);
        स्थिती AF_VSOCK:
                परत आकार(संरचना sockaddr_vm);
        पूर्वनिर्धारित:
                assert_not_reached();
        }
}

पूर्णांक socket_ioctl_fd(शून्य) {
        पूर्णांक fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        जर (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        जर (fd < 0)
                परत -त्रुटि_सं;

        परत fd;
}

पूर्णांक sockaddr_un_unlink(स्थिरांक संरचना sockaddr_un *sa) {
        स्थिरांक अक्षर *p, * nul;

        प्रतिज्ञा(sa);

        जर (sa->sun_family != AF_UNIX)
                परत -EPROTOTYPE;

        जर (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                परत 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = स_प्रथम(sa->sun_path, 0, आकार(sa->sun_path));
        जर (nul)
                p = sa->sun_path;
        अन्यथा
                p = memdupa_suffix0(sa->sun_path, आकार(sa->sun_path));

        जर (unlink(p) < 0)
                परत -त्रुटि_सं;

        परत 1;
}

पूर्णांक sockaddr_un_set_path(संरचना sockaddr_un *ret, स्थिरांक अक्षर *path) {
        माप_प्रकार l;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = म_माप(path);
        जर (l < 2)
                परत -EINVAL;
        जर (!IN_SET(path[0], '/', '@'))
                परत -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        जर (l+1 > आकार(ret->sun_path))
                परत path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (संरचना sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        जर (path[0] == '@') {
                /* Abstract namespace socket */
                स_नकल(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                परत (पूर्णांक) (दुरत्व(संरचना sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } अन्यथा {
                प्रतिज्ञा(path[0] == '/');

                /* File system socket */
                स_नकल(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                परत (पूर्णांक) (दुरत्व(संरचना sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

पूर्णांक getsockopt_int(पूर्णांक fd, पूर्णांक level, पूर्णांक optname, पूर्णांक *ret) {
        पूर्णांक v;
        socklen_t sl = आकार(v);

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जर (getsockopt(fd, level, optname, &v, &sl) < 0)
                परत negative_errno();
        जर (sl != आकार(v))
                परत -EIO;

        *ret = v;
        परत 0;
}

पूर्णांक socket_bind_to_ifname(पूर्णांक fd, स्थिरांक अक्षर *ifname) {
        प्रतिज्ञा(fd >= 0);

        /* Call with NULL to drop binding */

        परत RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

पूर्णांक socket_bind_to_ifindex(पूर्णांक fd, पूर्णांक ifindex) {
        प्रतिज्ञा(fd >= 0);

        जर (ifindex <= 0)
                /* Drop binding */
                परत RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, रिक्त, 0));

        परत setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

पूर्णांक socket_autobind(पूर्णांक fd, अक्षर **ret_name) {
        _cleanup_free_ अक्षर *name = रिक्त;
        uint64_t random;
        पूर्णांक r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        प्रतिज्ञा(fd >= 0);

        random = random_u64();

        जर (asprintf(&name, "@%" PRIu64, random) < 0)
                परत -ENOMEM;

        संघ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < आकार(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        जर (r < 0)
                परत r;

        जर (bind(fd, &sa.sa, r) < 0)
                परत -त्रुटि_सं;

        जर (ret_name)
                *ret_name = TAKE_PTR(name);
        परत 0;
}

ssize_t recvmsg_safe(पूर्णांक sockfd, संरचना msghdr *msg, पूर्णांक flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        प्रतिज्ञा(sockfd >= 0);
        प्रतिज्ञा(msg);

        n = recvmsg(sockfd, msg, flags);
        जर (n < 0)
                परत -त्रुटि_सं;

        जर (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                परत FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        परत n;
}

पूर्णांक socket_get_family(पूर्णांक fd) {
        पूर्णांक af;
        socklen_t sl = आकार(af);

        जर (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                परत -त्रुटि_सं;

        जर (sl != आकार(af))
                परत -EINVAL;

        परत af;
}

पूर्णांक socket_set_recvpktinfo(पूर्णांक fd, पूर्णांक af, bool b) {

        जर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जर (af < 0)
                        परत af;
        }

        निवड (af) {

        स्थिती AF_INET:
                परत setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        स्थिती AF_INET6:
                परत setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        स्थिती AF_NETLINK:
                परत setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        स्थिती AF_PACKET:
                परत setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक socket_set_unicast_if(पूर्णांक fd, पूर्णांक af, पूर्णांक ifi) {
        be32_t ifindex_be = htobe32(ifi);

        जर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जर (af < 0)
                        परत af;
        }

        निवड (af) {

        स्थिती AF_INET:
                परत RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, आकार(ifindex_be)));

        स्थिती AF_INET6:
                परत RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, आकार(ifindex_be)));

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक socket_set_option(पूर्णांक fd, पूर्णांक af, पूर्णांक opt_ipv4, पूर्णांक opt_ipv6, पूर्णांक val) {
        जर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जर (af < 0)
                        परत af;
        }

        निवड (af) {

        स्थिती AF_INET:
                परत setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        स्थिती AF_INET6:
                परत setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }
}

पूर्णांक socket_get_mtu(पूर्णांक fd, पूर्णांक af, माप_प्रकार *ret) {
        पूर्णांक mtu, r;

        प्रतिज्ञा(ret);

        जर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                जर (af < 0)
                        परत af;
        }

        निवड (af) {

        स्थिती AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                थांबा;

        स्थिती AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                थांबा;

        पूर्वनिर्धारित:
                परत -EAFNOSUPPORT;
        }

        जर (r < 0)
                परत r;
        जर (mtu <= 0)
                परत -EINVAL;

        *ret = (माप_प्रकार) mtu;
        परत 0;
}

स्थिर पूर्णांक connect_unix_path_simple(पूर्णांक fd, स्थिरांक अक्षर *path) {
        संघ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        माप_प्रकार l;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(path);

        l = म_माप(path);
        प्रतिज्ञा(l > 0);
        प्रतिज्ञा(l < आकार(sa.un.sun_path));

        स_नकल(sa.un.sun_path, path, l + 1);
        परत RET_NERRNO(connect(fd, &sa.sa, दुरत्व(संरचना sockaddr_un, sun_path) + l + 1));
}

स्थिर पूर्णांक connect_unix_inode(पूर्णांक fd, पूर्णांक inode_fd) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(inode_fd >= 0);

        परत connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

पूर्णांक connect_unix_path(पूर्णांक fd, पूर्णांक dir_fd, स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक inode_fd = -EBADF;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        जर (!path) {
                जर (dir_fd < 0)
                        परत -EISDIR;

                परत connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        जर (isempty(path))
                परत -EINVAL;

        /* Shortcut for the simple case */
        जर (dir_fd == AT_FDCWD && म_माप(path) < sizeof_field(संरचना sockaddr_un, sun_path))
                परत connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        जर (dir_fd == XAT_FDROOT) {
                _cleanup_free_ अक्षर *j = strjoin("/", path);
                जर (!j)
                        परत -ENOMEM;

                inode_fd = खोलो(j, O_PATH|O_CLOEXEC);
        } अन्यथा
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        जर (inode_fd < 0)
                परत -त्रुटि_सं;

        परत connect_unix_inode(fd, inode_fd);
}

पूर्णांक socket_address_parse_unix(SocketAddress *ret_address, स्थिरांक अक्षर *s) {
        संरचना sockaddr_un un;
        पूर्णांक r;

        प्रतिज्ञा(ret_address);
        प्रतिज्ञा(s);

        जर (!IN_SET(*s, '/', '@'))
                परत -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        जर (r < 0)
                परत r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .size = r,
        };

        परत 0;
}

पूर्णांक socket_address_equal_unix(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        SocketAddress socket_a, socket_b;
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = socket_address_parse_unix(&socket_a, a);
        जर (r < 0)
                परत r;

        r = socket_address_parse_unix(&socket_b, b);
        जर (r < 0)
                परत r;

        परत sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

पूर्णांक vsock_parse_port(स्थिरांक अक्षर *s, अचिन्हित *ret) {
        पूर्णांक r;

        प्रतिज्ञा(ret);

        जर (!s)
                परत -EINVAL;

        अचिन्हित u;
        r = safe_atou(s, &u);
        जर (r < 0)
                परत r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        जर (u == VMADDR_PORT_ANY)
                परत -EINVAL;

        *ret = u;
        परत 0;
}

पूर्णांक vsock_parse_cid(स्थिरांक अक्षर *s, अचिन्हित *ret) {
        प्रतिज्ञा(ret);

        जर (!s)
                परत -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        जर (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        अन्यथा जर (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        अन्यथा जर (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        अन्यथा जर (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        अन्यथा
                परत safe_atou(s, ret);

        परत 0;
}

पूर्णांक socket_address_parse_vsock(SocketAddress *ret_address, स्थिरांक अक्षर *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ अक्षर *n = रिक्त;
        स्थिरांक अक्षर *e, *cid_start;
        अचिन्हित port, cid;
        पूर्णांक type, r;

        प्रतिज्ञा(ret_address);
        प्रतिज्ञा(s);

        जर ((cid_start = startswith(s, "vsock:")))
                type = 0;
        अन्यथा जर ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        अन्यथा जर ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        अन्यथा जर ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        अन्यथा
                परत -EPROTO;

        e = म_अक्षर(cid_start, ':');
        जर (!e)
                परत -EINVAL;

        r = vsock_parse_port(e+1, &port);
        जर (r < 0)
                परत r;

        n = strndup(cid_start, e - cid_start);
        जर (!n)
                परत -ENOMEM;

        जर (isempty(n))
                cid = VMADDR_CID_ANY;
        अन्यथा {
                r = vsock_parse_cid(n, &cid);
                जर (r < 0)
                        परत r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .size = आकार(संरचना sockaddr_vm),
        };

        परत 0;
}

पूर्णांक netlink_socket_get_multicast_groups(पूर्णांक fd, माप_प्रकार *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = रिक्त;
        socklen_t len = 0, old_len;

        प्रतिज्ञा(fd >= 0);

        जर (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, रिक्त, &len) < 0)
                परत -त्रुटि_सं;

        जर (len == 0)
                जा finalize;

        groups = new0(uint32_t, len);
        जर (!groups)
                परत -ENOMEM;

        old_len = len;

        जर (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                परत -त्रुटि_सं;

        जर (old_len != len)
                परत -EIO;

finalize:
        जर (ret_len)
                *ret_len = len;
        जर (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        परत 0;
}

पूर्णांक socket_get_cookie(पूर्णांक fd, uint64_t *ret) {
        प्रतिज्ञा(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = आकार(cookie);
        जर (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                परत -त्रुटि_सं;

        प्रतिज्ञा(cookie_len == आकार(cookie));
        जर (ret)
                *ret = cookie;

        परत 0;
}

शून्य cmsg_close_all(संरचना msghdr *mh) {
        प्रतिज्ञा(mh);

        संरचना cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                जर (cmsg->cmsg_level != SOL_SOCKET)
                        सुरूठेवा;

                जर (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_DATA(cmsg, पूर्णांक),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / आकार(पूर्णांक));
                अन्यथा जर (cmsg->cmsg_type == SCM_PIDFD) {
                        प्रतिज्ञा(cmsg->cmsg_len == CMSG_LEN(आकार(पूर्णांक)));
                        safe_close(*CMSG_TYPED_DATA(cmsg, पूर्णांक));
                }
        }
}

पूर्णांक tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        निवड (IPTOS_PREC(tos)) {
        स्थिती IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        स्थिती IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                परत TC_PRIO_CONTROL;

        स्थिती IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        स्थिती IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                परत TC_PRIO_INTERACTIVE;

        स्थिती IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        स्थिती IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                परत TC_PRIO_INTERACTIVE_BULK;

        स्थिती IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                परत TC_PRIO_BULK;

        स्थिती IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        पूर्वनिर्धारित:
                परत TC_PRIO_BESTEFFORT;
        }
}

पूर्णांक socket_xattr_supported(शून्य) {
        पूर्णांक r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        स्थिर पूर्णांक cached = -1;
        जर (cached >= 0)
                परत cached;

        _cleanup_close_ पूर्णांक fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        जर (fd < 0)
                परत -त्रुटि_सं;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ रिक्त, AT_EMPTY_PATH, "user.testxxx", "1");
        जर (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                परत (cached = false);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to set test xattr on socket: %m");

        परत (cached = true);
}
