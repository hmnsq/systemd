/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <linux/if.h>
#समावेश <linux/if_arp.h>
#समावेश <linux/if_ether.h>
#समावेश <linux/if_infiniband.h>
#समावेश <linux/pkt_sched.h>
#समावेश <mqueue.h>
#समावेश <net/if.h>
#समावेश <netdb.h>
#समावेश <netinet/ip.h>
#समावेश <poll.h>
#समावेश <stdio.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "escape.h"
#समावेश "fd-util.h"
#समावेश "format-ifname.h"
#समावेश "format-util.h"
#समावेश "in-addr-util.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "random-util.h"
#समावेश "socket-util.h"
#समावेश "sparse-endian.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "sysctl-util.h"
#समावेश "user-util.h"
#समावेश "xattr-util.h"

#अगर ENABLE_IDN
#  define IDN_FLAGS NI_IDN
#अन्यथा
#  define IDN_FLAGS 0
#पूर्ण_अगर

अटल स्थिर अक्षर* स्थिर socket_address_type_table[] = {
        [SOCK_STREAM] =    "Stream",
        [SOCK_DGRAM] =     "Datagram",
        [SOCK_RAW] =       "Raw",
        [SOCK_RDM] =       "ReliableDatagram",
        [SOCK_SEQPACKET] = "SequentialPacket",
        [SOCK_DCCP] =      "DatagramCongestionControl",
};

DEFINE_STRING_TABLE_LOOKUP(socket_address_type, पूर्णांक);

पूर्णांक socket_address_verify(स्थिर SocketAddress *a, bool strict) {
        निश्चित(a);

        /* With 'strict' we enforce additional sanity constraints which are not set by the standard,
         * but should only apply to sockets we create ourselves. */

        चयन (socket_address_family(a)) {

        हाल AF_INET:
                अगर (a->आकार != माप(काष्ठा sockaddr_in))
                        वापस -EINVAL;

                अगर (a->sockaddr.in.sin_port == 0)
                        वापस -EINVAL;

                अगर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        वापस -EINVAL;

                वापस 0;

        हाल AF_INET6:
                अगर (a->आकार != माप(काष्ठा sockaddr_in6))
                        वापस -EINVAL;

                अगर (a->sockaddr.in6.sin6_port == 0)
                        वापस -EINVAL;

                अगर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        वापस -EINVAL;

                वापस 0;

        हाल AF_UNIX:
                अगर (a->आकार < दुरत्व(काष्ठा sockaddr_un, sun_path))
                        वापस -EINVAL;
                अगर (a->आकार > माप(काष्ठा sockaddr_un) + !strict)
                        /* If !strict, allow one extra byte, since getsockname() on Linux will append
                         * a NUL byte if we have path sockets that are above sun_path's full size. */
                        वापस -EINVAL;

                अगर (a->आकार > दुरत्व(काष्ठा sockaddr_un, sun_path) &&
                    a->sockaddr.un.sun_path[0] != 0 &&
                    strict) {
                        /* Only validate file system sockets here, and only in strict mode */
                        स्थिर अक्षर *e;

                        e = स_प्रथम(a->sockaddr.un.sun_path, 0, माप(a->sockaddr.un.sun_path));
                        अगर (e) {
                                /* If there's an embedded NUL byte, make sure the size of the socket address matches it */
                                अगर (a->आकार != दुरत्व(काष्ठा sockaddr_un, sun_path) + (e - a->sockaddr.un.sun_path) + 1)
                                        वापस -EINVAL;
                        } अन्यथा {
                                /* If there's no embedded NUL byte, then the size needs to match the whole
                                 * structure or the structure with one extra NUL byte suffixed. (Yeah, Linux is awful,
                                 * and considers both equivalent: getsockname() even extends sockaddr_un beyond its
                                 * size if the path is non NUL terminated.) */
                                अगर (!IN_SET(a->आकार, माप(a->sockaddr.un.sun_path), माप(a->sockaddr.un.sun_path)+1))
                                        वापस -EINVAL;
                        }
                }

                अगर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET))
                        वापस -EINVAL;

                वापस 0;

        हाल AF_NETLINK:

                अगर (a->आकार != माप(काष्ठा sockaddr_nl))
                        वापस -EINVAL;

                अगर (!IN_SET(a->type, 0, SOCK_RAW, SOCK_DGRAM))
                        वापस -EINVAL;

                वापस 0;

        हाल AF_VSOCK:
                अगर (a->आकार != माप(काष्ठा sockaddr_vm))
                        वापस -EINVAL;

                अगर (!IN_SET(a->type, 0, SOCK_STREAM, SOCK_DGRAM))
                        वापस -EINVAL;

                वापस 0;

        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक socket_address_print(स्थिर SocketAddress *a, अक्षर **ret) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(ret);

        r = socket_address_verify(a, false); /* We do non-strict validation, because we want to be
                                              * able to pretty-print any socket the kernel considers
                                              * valid. We still need to do validation to know if we
                                              * can meaningfully print the address. */
        अगर (r < 0)
                वापस r;

        अगर (socket_address_family(a) == AF_NETLINK) {
                _cleanup_free_ अक्षर *sfamily = शून्य;

                r = netlink_family_to_string_alloc(a->protocol, &sfamily);
                अगर (r < 0)
                        वापस r;

                r = asprintf(ret, "%s %u", sfamily, a->sockaddr.nl.nl_groups);
                अगर (r < 0)
                        वापस -ENOMEM;

                वापस 0;
        }

        वापस sockaddr_pretty(&a->sockaddr.sa, a->आकार, false, true, ret);
}

bool socket_address_can_accept(स्थिर SocketAddress *a) {
        निश्चित(a);

        वापस
                IN_SET(a->type, SOCK_STREAM, SOCK_SEQPACKET);
}

bool socket_address_equal(स्थिर SocketAddress *a, स्थिर SocketAddress *b) {
        निश्चित(a);
        निश्चित(b);

        /* Invalid addresses are unequal to all */
        अगर (socket_address_verify(a, false) < 0 ||
            socket_address_verify(b, false) < 0)
                वापस false;

        अगर (a->type != b->type)
                वापस false;

        अगर (socket_address_family(a) != socket_address_family(b))
                वापस false;

        चयन (socket_address_family(a)) {

        हाल AF_INET:
                अगर (a->sockaddr.in.sin_addr.s_addr != b->sockaddr.in.sin_addr.s_addr)
                        वापस false;

                अगर (a->sockaddr.in.sin_port != b->sockaddr.in.sin_port)
                        वापस false;

                अवरोध;

        हाल AF_INET6:
                अगर (स_भेद(&a->sockaddr.in6.sin6_addr, &b->sockaddr.in6.sin6_addr, माप(a->sockaddr.in6.sin6_addr)) != 0)
                        वापस false;

                अगर (a->sockaddr.in6.sin6_port != b->sockaddr.in6.sin6_port)
                        वापस false;

                अवरोध;

        हाल AF_UNIX:
                अगर (a->आकार <= दुरत्व(काष्ठा sockaddr_un, sun_path) ||
                    b->आकार <= दुरत्व(काष्ठा sockaddr_un, sun_path))
                        वापस false;

                अगर ((a->sockaddr.un.sun_path[0] == 0) != (b->sockaddr.un.sun_path[0] == 0))
                        वापस false;

                अगर (a->sockaddr.un.sun_path[0]) {
                        अगर (!path_equal_or_inode_same(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, 0))
                                वापस false;
                } अन्यथा {
                        अगर (a->आकार != b->आकार)
                                वापस false;

                        अगर (स_भेद(a->sockaddr.un.sun_path, b->sockaddr.un.sun_path, a->आकार) != 0)
                                वापस false;
                }

                अवरोध;

        हाल AF_NETLINK:
                अगर (a->protocol != b->protocol)
                        वापस false;

                अगर (a->sockaddr.nl.nl_groups != b->sockaddr.nl.nl_groups)
                        वापस false;

                अवरोध;

        हाल AF_VSOCK:
                अगर (a->sockaddr.vm.svm_cid != b->sockaddr.vm.svm_cid)
                        वापस false;

                अगर (a->sockaddr.vm.svm_port != b->sockaddr.vm.svm_port)
                        वापस false;

                अवरोध;

        शेष:
                /* Cannot compare, so we assume the addresses are different */
                वापस false;
        }

        वापस true;
}

स्थिर अक्षर* socket_address_get_path(स्थिर SocketAddress *a) {
        निश्चित(a);

        अगर (socket_address_family(a) != AF_UNIX)
                वापस शून्य;

        अगर (a->sockaddr.un.sun_path[0] == 0)
                वापस शून्य;

        /* Note that this is only safe because we know that there's an extra NUL byte after the sockaddr_un
         * structure. On Linux AF_UNIX file system socket addresses don't have to be NUL terminated if they take up the
         * full sun_path space. */
        assert_cc(माप(जोड़ sockaddr_union) >= माप(काष्ठा sockaddr_un)+1);
        वापस a->sockaddr.un.sun_path;
}

bool socket_ipv6_is_supported(व्योम) {
        अटल पूर्णांक cached = -1;

        अगर (cached < 0) {

                अगर (access("/proc/net/if_inet6", F_OK) < 0) {

                        अगर (त्रुटि_सं != ENOENT) {
                                log_debug_errno(त्रुटि_सं, "Unexpected error when checking whether /proc/net/if_inet6 exists: %m");
                                वापस false;
                        }

                        cached = false;
                } अन्यथा
                        cached = true;
        }

        वापस cached;
}

bool socket_ipv6_is_enabled(व्योम) {
        _cleanup_free_ अक्षर *v = शून्य;
        पूर्णांक r;

        /* Much like socket_ipv6_is_supported(), but also checks that the sysctl that disables IPv6 on all
         * interfaces isn't turned on */

        अगर (!socket_ipv6_is_supported())
                वापस false;

        r = sysctl_read_ip_property(AF_INET6, "all", "disable_ipv6", &v);
        अगर (r < 0) {
                log_debug_errno(r, "Unexpected error reading 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                वापस true;
        }

        r = parse_boolean(v);
        अगर (r < 0) {
                log_debug_errno(r, "Failed to pare 'net.ipv6.conf.all.disable_ipv6' sysctl: %m");
                वापस true;
        }

        वापस !r;
}

bool socket_address_matches_fd(स्थिर SocketAddress *a, पूर्णांक fd) {
        SocketAddress b;
        socklen_t solen;

        निश्चित(a);
        निश्चित(fd >= 0);

        b.आकार = माप(b.sockaddr);
        अगर (getsockname(fd, &b.sockaddr.sa, &b.आकार) < 0)
                वापस false;

        अगर (b.sockaddr.sa.sa_family != a->sockaddr.sa.sa_family)
                वापस false;

        solen = माप(b.type);
        अगर (getsockopt(fd, SOL_SOCKET, SO_TYPE, &b.type, &solen) < 0)
                वापस false;

        अगर (b.type != a->type)
                वापस false;

        अगर (a->protocol != 0)  {
                solen = माप(b.protocol);
                अगर (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, &b.protocol, &solen) < 0)
                        वापस false;

                अगर (b.protocol != a->protocol)
                        वापस false;
        }

        वापस socket_address_equal(a, &b);
}

पूर्णांक sockaddr_port(स्थिर काष्ठा sockaddr *_sa, अचिन्हित *ret_port) {
        स्थिर जोड़ sockaddr_union *sa = (स्थिर जोड़ sockaddr_union*) _sa;

        /* Note, this returns the port as 'unsigned' rather than 'uint16_t', as AF_VSOCK knows larger ports */

        निश्चित(sa);
        निश्चित(ret_port);

        चयन (sa->sa.sa_family) {

        हाल AF_INET:
                *ret_port = be16toh(sa->in.sin_port);
                वापस 0;

        हाल AF_INET6:
                *ret_port = be16toh(sa->in6.sin6_port);
                वापस 0;

        हाल AF_VSOCK:
                *ret_port = sa->vm.svm_port;
                वापस 0;

        शेष:
                वापस -EAFNOSUPPORT;
        }
}

स्थिर जोड़ in_addr_union *sockaddr_in_addr(स्थिर काष्ठा sockaddr *_sa) {
        स्थिर जोड़ sockaddr_union *sa = (स्थिर जोड़ sockaddr_union*) _sa;

        अगर (!sa)
                वापस शून्य;

        चयन (sa->sa.sa_family) {

        हाल AF_INET:
                वापस (स्थिर जोड़ in_addr_union*) &sa->in.sin_addr;

        हाल AF_INET6:
                वापस (स्थिर जोड़ in_addr_union*) &sa->in6.sin6_addr;

        शेष:
                वापस शून्य;
        }
}

पूर्णांक sockaddr_set_in_addr(
                जोड़ sockaddr_union *u,
                पूर्णांक family,
                स्थिर जोड़ in_addr_union *a,
                uint16_t port) {

        निश्चित(u);
        निश्चित(a);

        चयन (family) {

        हाल AF_INET:
                u->in = (काष्ठा sockaddr_in) {
                        .sin_family = AF_INET,
                        .sin_addr = a->in,
                        .sin_port = htobe16(port),
                };

                वापस 0;

        हाल AF_INET6:
                u->in6 = (काष्ठा sockaddr_in6) {
                        .sin6_family = AF_INET6,
                        .sin6_addr = a->in6,
                        .sin6_port = htobe16(port),
                };

                वापस 0;

        शेष:
                वापस -EAFNOSUPPORT;

        }
}

पूर्णांक sockaddr_pretty(
                स्थिर काष्ठा sockaddr *_sa,
                socklen_t salen,
                bool translate_ipv6,
                bool include_port,
                अक्षर **ret) {

        जोड़ sockaddr_union *sa = (जोड़ sockaddr_union*) _sa;
        अक्षर *p;
        पूर्णांक r;

        निश्चित(sa);
        निश्चित(salen >= माप(sa->sa.sa_family));
        निश्चित(ret);

        चयन (sa->sa.sa_family) {

        हाल AF_INET: {
                uint32_t a;

                a = be32toh(sa->in.sin_addr.s_addr);

                अगर (include_port)
                        r = asprintf(&p,
                                     "%u.%u.%u.%u:%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF,
                                     be16toh(sa->in.sin_port));
                अन्यथा
                        r = asprintf(&p,
                                     "%u.%u.%u.%u",
                                     a >> 24, (a >> 16) & 0xFF, (a >> 8) & 0xFF, a & 0xFF);
                अगर (r < 0)
                        वापस -ENOMEM;
                अवरोध;
        }

        हाल AF_INET6: {
                अटल स्थिर अचिन्हित अक्षर ipv4_prefix[] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF
                };

                अगर (translate_ipv6 &&
                    स_भेद(&sa->in6.sin6_addr, ipv4_prefix, माप(ipv4_prefix)) == 0) {
                        स्थिर uint8_t *a = sa->in6.sin6_addr.s6_addr+12;
                        अगर (include_port)
                                r = asprintf(&p,
                                             "%u.%u.%u.%u:%u",
                                             a[0], a[1], a[2], a[3],
                                             be16toh(sa->in6.sin6_port));
                        अन्यथा
                                r = asprintf(&p,
                                             "%u.%u.%u.%u",
                                             a[0], a[1], a[2], a[3]);
                        अगर (r < 0)
                                वापस -ENOMEM;
                } अन्यथा {
                        स्थिर अक्षर *a = IN6_ADDR_TO_STRING(&sa->in6.sin6_addr);

                        अगर (include_port) {
                                अगर (asprintf(&p,
                                             "[%s]:%u%s%s",
                                             a,
                                             be16toh(sa->in6.sin6_port),
                                             sa->in6.sin6_scope_id != 0 ? "%" : "",
                                             FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX)) < 0)
                                        वापस -ENOMEM;
                        } अन्यथा {
                                अगर (sa->in6.sin6_scope_id != 0)
                                        p = strjoin(a, "%", FORMAT_IFNAME_FULL(sa->in6.sin6_scope_id, FORMAT_IFNAME_IFINDEX));
                                अन्यथा
                                        p = strdup(a);
                                अगर (!p)
                                        वापस -ENOMEM;
                        }
                }

                अवरोध;
        }

        हाल AF_UNIX:
                अगर (salen <= दुरत्व(काष्ठा sockaddr_un, sun_path) ||
                    (sa->un.sun_path[0] == 0 && salen == दुरत्व(काष्ठा sockaddr_un, sun_path) + 1))
                        /* The name must have at least one character (and the leading NUL does not count) */
                        p = strdup("<unnamed>");
                अन्यथा {
                        /* Note that we calculate the path pointer here through the .un_buffer[] field, in order to
                         * outtrick bounds checking tools such as ubsan, which are too smart for their own good: on
                         * Linux the kernel may return sun_path[] data one byte longer than the declared size of the
                         * field. */
                        अक्षर *path = (अक्षर*) sa->un_बफर + दुरत्व(काष्ठा sockaddr_un, sun_path);
                        माप_प्रकार path_len = salen - दुरत्व(काष्ठा sockaddr_un, sun_path);

                        अगर (path[0] == 0) {
                                /* Abstract socket. When parsing address information from, we
                                 * explicitly reject overly long paths and paths with embedded NULs.
                                 * But we might get such a socket from the outside. Let's return
                                 * something meaningful and printable in this case. */

                                _cleanup_free_ अक्षर *e = शून्य;

                                e = cescape_लंबाई(path + 1, path_len - 1);
                                अगर (!e)
                                        वापस -ENOMEM;

                                p = strjoin("@", e);
                        } अन्यथा {
                                अगर (path[path_len - 1] == '\0')
                                        /* We expect a terminating NUL and don't print it */
                                        path_len--;

                                p = cescape_लंबाई(path, path_len);
                        }
                }
                अगर (!p)
                        वापस -ENOMEM;

                अवरोध;

        हाल AF_VSOCK:
                अगर (include_port) {
                        अगर (sa->vm.svm_cid == VMADDR_CID_ANY)
                                r = asprintf(&p, "vsock::%u", sa->vm.svm_port);
                        अन्यथा
                                r = asprintf(&p, "vsock:%u:%u", sa->vm.svm_cid, sa->vm.svm_port);
                } अन्यथा
                        r = asprintf(&p, "vsock:%u", sa->vm.svm_cid);
                अगर (r < 0)
                        वापस -ENOMEM;
                अवरोध;

        शेष:
                वापस -EOPNOTSUPP;
        }

        *ret = p;
        वापस 0;
}

पूर्णांक getpeername_pretty(पूर्णांक fd, bool include_port, अक्षर **ret) {
        जोड़ sockaddr_union sa;
        socklen_t salen = माप(sa);
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (getpeername(fd, &sa.sa, &salen) < 0)
                वापस -त्रुटि_सं;

        अगर (sa.sa.sa_family == AF_UNIX) {
                काष्ठा ucred ucred = UCRED_INVALID;

                /* UNIX connection sockets are anonymous, so let's use
                 * PID/UID as pretty credentials instead */

                r = getpeercred(fd, &ucred);
                अगर (r < 0)
                        वापस r;

                अगर (asprintf(ret, "PID "PID_FMT"/UID "UID_FMT, ucred.pid, ucred.uid) < 0)
                        वापस -ENOMEM;

                वापस 0;
        }

        /* For remote sockets we translate IPv6 addresses back to IPv4
         * if applicable, since that's nicer. */

        वापस sockaddr_pretty(&sa.sa, salen, true, include_port, ret);
}

पूर्णांक getsockname_pretty(पूर्णांक fd, अक्षर **ret) {
        जोड़ sockaddr_union sa;
        socklen_t salen = माप(sa);

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (getsockname(fd, &sa.sa, &salen) < 0)
                वापस -त्रुटि_सं;

        /* For local sockets we do not translate IPv6 addresses back
         * to IPv6 if applicable, since this is usually used for
         * listening sockets where the difference between IPv4 and
         * IPv6 matters. */

        वापस sockaddr_pretty(&sa.sa, salen, false, true, ret);
}

पूर्णांक socknameinfo_pretty(स्थिर काष्ठा sockaddr *sa, socklen_t salen, अक्षर **ret) {
        अक्षर host[NI_MAXHOST];
        पूर्णांक r;

        निश्चित(sa);
        निश्चित(salen >= माप(sa_family_t));
        निश्चित(ret);

        r = getnameinfo(sa, salen, host, माप(host), /* service= */ शून्य, /* service_len= */ 0, IDN_FLAGS);
        अगर (r != 0) {
                अगर (r == EAI_MEMORY)
                        वापस log_oom_debug();
                अगर (r == EAI_SYSTEM)
                        log_debug_errno(त्रुटि_सं, "getnameinfo() failed, ignoring: %m");
                अन्यथा
                        log_debug("getnameinfo() failed, ignoring: %s", gai_strerror(r));

                वापस sockaddr_pretty(sa, salen, /* translate_ipv6= */ true, /* include_port= */ true, ret);
        }

        वापस strdup_to(ret, host);
}

अटल स्थिर अक्षर* स्थिर netlink_family_table[] = {
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

bool sockaddr_equal(स्थिर जोड़ sockaddr_union *a, स्थिर जोड़ sockaddr_union *b) {
        निश्चित(a);
        निश्चित(b);

        अगर (a->sa.sa_family != b->sa.sa_family)
                वापस false;

        अगर (a->sa.sa_family == AF_INET)
                वापस a->in.sin_addr.s_addr == b->in.sin_addr.s_addr;

        अगर (a->sa.sa_family == AF_INET6)
                वापस स_भेद(&a->in6.sin6_addr, &b->in6.sin6_addr, माप(a->in6.sin6_addr)) == 0;

        अगर (a->sa.sa_family == AF_VSOCK)
                वापस a->vm.svm_cid == b->vm.svm_cid;

        वापस false;
}

पूर्णांक fd_set_sndbuf(पूर्णांक fd, माप_प्रकार n, bool increase) {
        पूर्णांक r, मूल्य;
        socklen_t l = माप(मूल्य);

        अगर (n > पूर्णांक_उच्च)
                वापस -दुस्फल;

        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मूल्य, &l);
        अगर (r >= 0 && l == माप(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                वापस 0;

        /* First, try to set the buffer size with SO_SNDBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUF, n);
        अगर (r < 0)
                वापस r;

        /* SO_SNDBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = माप(मूल्य);
        r = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &मूल्य, &l);
        अगर (r >= 0 && l == माप(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                वापस 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_SNDBUFFORCE, n);
        अगर (r < 0)
                वापस r;

        वापस 1;
}

पूर्णांक fd_set_rcvbuf(पूर्णांक fd, माप_प्रकार n, bool increase) {
        पूर्णांक r, मूल्य;
        socklen_t l = माप(मूल्य);

        अगर (n > पूर्णांक_उच्च)
                वापस -दुस्फल;

        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मूल्य, &l);
        अगर (r >= 0 && l == माप(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                वापस 0;

        /* First, try to set the buffer size with SO_RCVBUF. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUF, n);
        अगर (r < 0)
                वापस r;

        /* SO_RCVBUF above may set to the kernel limit, instead of the requested size.
         * So, we need to check the actual buffer size here. */
        l = माप(मूल्य);
        r = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &मूल्य, &l);
        अगर (r >= 0 && l == माप(मूल्य) && increase ? (माप_प्रकार) मूल्य >= n*2 : (माप_प्रकार) मूल्य == n*2)
                वापस 1;

        /* If we have the privileges we will ignore the kernel limit. */
        r = setsockopt_int(fd, SOL_SOCKET, SO_RCVBUFFORCE, n);
        अगर (r < 0)
                वापस r;

        वापस 1;
}

अटल स्थिर अक्षर* स्थिर ip_tos_table[] = {
        [IPTOS_LOWDELAY]    = "low-delay",
        [IPTOS_THROUGHPUT]  = "throughput",
        [IPTOS_RELIABILITY] = "reliability",
        [IPTOS_LOWCOST]     = "low-cost",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, पूर्णांक, 0xff);

bool ifname_valid_char(अक्षर a) {
        अगर ((अचिन्हित अक्षर) a >= 127U)
                वापस false;

        अगर ((अचिन्हित अक्षर) a <= 32U)
                वापस false;

        अगर (IN_SET(a,
                   ':',  /* colons are used by the legacy "alias" interface logic */
                   '/',  /* slashes cannot work, since we need to use network interfaces in sysfs paths, and in paths slashes are separators */
                   '%')) /* %d is used in the kernel's weird foo%d format string naming feature which we really really don't want to ever run into by accident */
                वापस false;

        वापस true;
}

bool ifname_valid_full(स्थिर अक्षर *p, IfnameValidFlags flags) {
        bool numeric = true;

        /* Checks whether a network interface name is valid. This is inspired by dev_valid_name() in the kernel sources
         * but slightly stricter, as we only allow non-control, non-space ASCII characters in the interface name. We
         * also don't permit names that only container numbers, to avoid confusion with numeric interface indexes. */

        निश्चित(!(flags & ~_IFNAME_VALID_ALL));

        अगर (isempty(p))
                वापस false;

        /* A valid ifindex? If so, it's valid iff IFNAME_VALID_NUMERIC is set */
        अगर (parse_ifindex(p) >= 0)
                वापस flags & IFNAME_VALID_NUMERIC;

        अगर (flags & IFNAME_VALID_ALTERNATIVE) {
                अगर (म_माप(p) >= ALTIFNAMSIZ)
                        वापस false;
        } अन्यथा {
                अगर (म_माप(p) >= IFNAMSIZ)
                        वापस false;
        }

        अगर (dot_or_dot_dot(p))
                वापस false;

        /* Let's refuse "all" and "default" as interface name, to avoid collisions with the special sysctl
         * directories /proc/sys/net/{ipv4,ipv6}/conf/{all,default} */
        अगर (!FLAGS_SET(flags, IFNAME_VALID_SPECIAL) && STR_IN_SET(p, "all", "default"))
                वापस false;

        क्रम (स्थिर अक्षर *t = p; *t; t++) {
                अगर (!ifname_valid_char(*t))
                        वापस false;

                numeric = numeric && ascii_isdigit(*t);
        }

        /* It's fully numeric but didn't parse as valid ifindex above? if so, it must be too large or zero or
         * so, let's refuse that. */
        अगर (numeric)
                वापस false;

        वापस true;
}

bool address_label_valid(स्थिर अक्षर *p) {

        POINTER_MAY_BE_NULL(p);

        अगर (isempty(p))
                वापस false;

        अगर (म_माप(p) >= IFNAMSIZ)
                वापस false;

        जबतक (*p) {
                अगर ((uint8_t) *p >= 127U)
                        वापस false;

                अगर ((uint8_t) *p <= 31U)
                        वापस false;
                p++;
        }

        वापस true;
}

पूर्णांक getpeercred(पूर्णांक fd, काष्ठा ucred *ucred) {
        socklen_t n = माप(काष्ठा ucred);
        काष्ठा ucred u;

        निश्चित(fd >= 0);
        निश्चित(ucred);

        अगर (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &u, &n) < 0)
                वापस -त्रुटि_सं;

        अगर (n != माप(काष्ठा ucred))
                वापस -EIO;

        /* Check if the data is actually useful and not suppressed due to namespacing issues */
        अगर (!pid_is_valid(u.pid))
                वापस -ENODATA;

        /* Note that we don't check UID/GID here, as namespace translation works differently there: instead of
         * receiving in "invalid" user/group we get the overflow UID/GID. */

        *ucred = u;
        वापस 0;
}

पूर्णांक getpeersec(पूर्णांक fd, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        socklen_t n = 64;

        निश्चित(fd >= 0);
        निश्चित(ret);

        क्रम (;;) {
                s = new0(अक्षर, n+1);
                अगर (!s)
                        वापस -ENOMEM;

                अगर (getsockopt(fd, SOL_SOCKET, SO_PEERSEC, s, &n) >= 0) {
                        s[n] = 0;
                        अवरोध;
                }

                अगर (त्रुटि_सं != दुस्फल)
                        वापस -त्रुटि_सं;

                s = mfree(s);
        }

        अगर (isempty(s))
                वापस -EOPNOTSUPP;

        *ret = TAKE_PTR(s);

        वापस 0;
}

पूर्णांक getpeergroups(पूर्णांक fd, gid_t **ret) {
        socklen_t n = माप(gid_t) * 64U;
        _cleanup_free_ gid_t *d = शून्य;

        निश्चित(fd >= 0);
        निश्चित(ret);

        पूर्णांक ngroups_max = sysconf_ngroups_max();
        अगर (ngroups_max > 0)
                n = MAX(n, माप(gid_t) * (socklen_t) ngroups_max);

        क्रम (;;) {
                d = दो_स्मृति(n);
                अगर (!d)
                        वापस -ENOMEM;

                अगर (getsockopt(fd, SOL_SOCKET, SO_PEERGROUPS, d, &n) >= 0)
                        अवरोध;

                अगर (त्रुटि_सं != दुस्फल)
                        वापस -त्रुटि_सं;

                d = mfree(d);
        }

        assert_se(n % माप(gid_t) == 0);
        n /= माप(gid_t);

        अगर (n > पूर्णांक_उच्च)
                वापस -E2BIG;

        *ret = TAKE_PTR(d);

        वापस (पूर्णांक) n;
}

पूर्णांक getpeerpidfd(पूर्णांक fd) {
        socklen_t n = माप(पूर्णांक);
        पूर्णांक pidfd = -EBADF;

        निश्चित(fd >= 0);

        अगर (getsockopt(fd, SOL_SOCKET, SO_PEERPIDFD, &pidfd, &n) < 0)
                वापस -त्रुटि_सं;

        अगर (n != माप(पूर्णांक))
                वापस -EIO;

        वापस pidfd;
}

पूर्णांक getpeerpidref(पूर्णांक fd, PidRef *ret) {
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(ret);

        पूर्णांक pidfd = getpeerpidfd(fd);
        अगर (pidfd < 0) {
                अगर (!ERRNO_IS_NEG_NOT_SUPPORTED(pidfd))
                        वापस pidfd;

                काष्ठा ucred ucred;
                r = getpeercred(fd, &ucred);
                अगर (r < 0)
                        वापस r;

                वापस pidref_set_pid(ret, ucred.pid);
        }

        वापस pidref_set_pidfd_consume(ret, pidfd);
}

ssize_t send_one_fd_iov_sa(
                पूर्णांक transport_fd,
                पूर्णांक fd,
                स्थिर काष्ठा iovec *iov, माप_प्रकार iovlen,
                स्थिर काष्ठा sockaddr *sa, socklen_t len,
                पूर्णांक flags) {

        CMSG_बफर_TYPE(CMSG_SPACE(माप(पूर्णांक))) control = {};
        काष्ठा msghdr mh = {
                .msg_name = (काष्ठा sockaddr*) sa,
                .msg_namelen = len,
                .msg_iov = (काष्ठा iovec *)iov,
                .msg_iovlen = iovlen,
        };
        ssize_t k;

        निश्चित(transport_fd >= 0);

        /*
         * We need either an FD or data to send.
         * If there's nothing, return an error.
         */
        अगर (fd < 0 && !iov)
                वापस -EINVAL;

        अगर (fd >= 0) {
                काष्ठा cmsghdr *cmsg;

                mh.msg_control = &control;
                mh.msg_controllen = माप(control);

                cmsg = CMSG_FIRSTHDR(&mh);
                cmsg->cmsg_level = SOL_SOCKET;
                cmsg->cmsg_type = SCM_RIGHTS;
                cmsg->cmsg_len = CMSG_LEN(माप(पूर्णांक));
                स_नकल(CMSG_DATA(cmsg), &fd, माप(पूर्णांक));
        }
        k = sendmsg(transport_fd, &mh, MSG_NOSIGNAL | flags);
        अगर (k < 0)
                वापस (ssize_t) -त्रुटि_सं;

        वापस k;
}

पूर्णांक send_one_fd_sa(
                पूर्णांक transport_fd,
                पूर्णांक fd,
                स्थिर काष्ठा sockaddr *sa, socklen_t len,
                पूर्णांक flags) {

        निश्चित(fd >= 0);

        वापस (पूर्णांक) send_one_fd_iov_sa(transport_fd, fd, शून्य, 0, sa, len, flags);
}

ssize_t receive_one_fd_iov(
                पूर्णांक transport_fd,
                काष्ठा iovec *iov, माप_प्रकार iovlen,
                पूर्णांक flags,
                पूर्णांक *ret_fd) {

        CMSG_बफर_TYPE(CMSG_SPACE(माप(पूर्णांक))) control;
        काष्ठा msghdr mh = {
                .msg_control = &control,
                .msg_controllen = माप(control),
                .msg_iov = iov,
                .msg_iovlen = iovlen,
        };
        काष्ठा cmsghdr *found;
        ssize_t k;

        निश्चित(transport_fd >= 0);
        निश्चित(ret_fd);

        /*
         * Receive a single FD via @transport_fd. We don't care for
         * the transport-type. We retrieve a single FD at most, so for
         * packet-based transports, the caller must ensure to send
         * only a single FD per packet.  This is best used in
         * combination with send_one_fd().
         */

        k = recvmsg_safe(transport_fd, &mh, MSG_CMSG_CLOEXEC | flags);
        अगर (k < 0)
                वापस k;

        found = cmsg_find(&mh, SOL_SOCKET, SCM_RIGHTS, CMSG_LEN(माप(पूर्णांक)));
        अगर (!found) {
                cmsg_close_all(&mh);

                /* If didn't receive an FD or any data, return an error. */
                अगर (k == 0)
                        वापस -EIO;
        }

        अगर (found)
                *ret_fd = *CMSG_TYPED_तथ्य(found, पूर्णांक);
        अन्यथा
                *ret_fd = -EBADF;

        वापस k;
}

पूर्णांक receive_one_fd(पूर्णांक transport_fd, पूर्णांक flags) {
        पूर्णांक fd;
        ssize_t k;

        k = receive_one_fd_iov(transport_fd, शून्य, 0, flags, &fd);
        अगर (k == 0)
                वापस fd;

        /* k must be negative, since receive_one_fd_iov() only returns
         * a positive value if data was received through the iov. */
        निश्चित(k < 0);
        वापस (पूर्णांक) k;
}

ssize_t अग्र_datagram_आकार_fd(पूर्णांक fd) {
        ssize_t l;
        पूर्णांक k;

        /* This is a bit like FIONREAD/SIOCINQ, however a bit more powerful. The difference being: recv(MSG_PEEK) will
         * actually cause the next datagram in the queue to be validated regarding checksums, which FIONREAD doesn't
         * do. This difference is actually of major importance as we need to be sure that the size returned here
         * actually matches what we will read with recvmsg() next, as otherwise we might end up allocating a buffer of
         * the wrong size. */

        l = recv(fd, शून्य, 0, MSG_PEEK|MSG_TRUNC);
        अगर (l < 0) {
                अगर (IN_SET(त्रुटि_सं, EOPNOTSUPP, EFAULT))
                        जाओ fallback;

                वापस -त्रुटि_सं;
        }
        अगर (l == 0)
                जाओ fallback;

        वापस l;

fallback:
        k = 0;

        /* Some sockets (AF_PACKET) do not support null-sized recv() with MSG_TRUNC set, let's fall back to FIONREAD
         * for them. Checksums don't matter for raw sockets anyway, hence this should be fine. */

        अगर (ioctl(fd, FIONREAD, &k) < 0)
                वापस -त्रुटि_सं;

        वापस (ssize_t) k;
}

/* Put a limit on how many times will attempt to call accept4(). We loop
 * only on "transient" errors, but let's make sure we don't loop forever. */
#घोषणा MAX_FLUSH_ITERATIONS 1024

पूर्णांक flush_accept(पूर्णांक fd) {

        पूर्णांक r, b;
        socklen_t l = माप(b);

        /* Similar to flush_fd() but flushes all incoming connections by accepting and immediately closing
         * them. */

        अगर (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &b, &l) < 0)
                वापस -त्रुटि_सं;

        निश्चित(l == माप(b));
        अगर (!b) /* Let's check if this socket accepts connections before calling accept(). accept4() can
                 * return EOPNOTSUPP if the fd is not a listening socket, which we should treat as a fatal
                 * error, or in case the incoming TCP connection triggered a network issue, which we want to
                 * treat as a transient error. Thus, let's rule out the first reason for EOPNOTSUPP early, so
                 * we can loop safely on transient errors below. */
                वापस -ENOTTY;

        क्रम (अचिन्हित iteration = 0;; iteration++) {
                पूर्णांक cfd;

                r = fd_wait_for_event(fd, POLLIN, 0);
                अगर (r == -EINTR)
                        जारी;
                अगर (r <= 0)
                        वापस r;

                अगर (iteration >= MAX_FLUSH_ITERATIONS)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EBUSY),
                                               "Failed to flush connections within " STRINGIFY(MAX_FLUSH_ITERATIONS) " iterations.");

                cfd = accept4(fd, शून्य, शून्य, SOCK_NONBLOCK|SOCK_CLOEXEC);
                अगर (cfd < 0) {
                        अगर (त्रुटि_सं == EAGAIN)
                                वापस 0;

                        अगर (ERRNO_IS_ACCEPT_AGAIN(त्रुटि_सं))
                                जारी;

                        वापस -त्रुटि_सं;
                }

                safe_close(cfd);
        }
}

ssize_t flush_mqueue(पूर्णांक fd) {
        _cleanup_free_ अक्षर *buf = शून्य;
        काष्ठा mq_attr attr;
        ssize_t गिनती = 0;
        पूर्णांक r;

        निश्चित(fd >= 0);

        /* Similar to flush_fd() but flushes all messages from a POSIX message queue. */

        क्रम (;;) {
                ssize_t l;

                r = fd_wait_for_event(fd, POLLIN, /* timeout= */ 0);
                अगर (r == -EINTR)
                        जारी;
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस गिनती;

                अगर (!buf) {
                        /* Buffer must be at least as large as mq_msgsize. */
                        अगर (mq_getattr(fd, &attr) < 0)
                                वापस -त्रुटि_सं;

                        buf = दो_स्मृति(attr.mq_msgsize);
                        अगर (!buf)
                                वापस -ENOMEM;
                }

                l = mq_receive(fd, buf, attr.mq_msgsize, /* msg_prio= */ शून्य);
                अगर (l < 0) {
                        अगर (त्रुटि_सं == EINTR)
                                जारी;

                        अगर (त्रुटि_सं == EAGAIN)
                                वापस गिनती;

                        वापस -त्रुटि_सं;
                }

                गिनती += l;
        }
}

काष्ठा cmsghdr* cmsg_find(काष्ठा msghdr *mh, पूर्णांक level, पूर्णांक type, socklen_t length) {
        काष्ठा cmsghdr *cmsg;

        निश्चित(mh);

        CMSG_FOREACH(cmsg, mh)
                अगर (cmsg->cmsg_level == level &&
                    cmsg->cmsg_type == type &&
                    (length == (socklen_t) -1 || length == cmsg->cmsg_len))
                        वापस cmsg;

        वापस शून्य;
}

व्योम* cmsg_find_and_copy_तथ्य(काष्ठा msghdr *mh, पूर्णांक level, पूर्णांक type, व्योम *buf, माप_प्रकार buf_len) {
        काष्ठा cmsghdr *cmsg;

        निश्चित(mh);
        निश्चित(buf);
        निश्चित(buf_len > 0);

        /* This is similar to cmsg_find_data(), but copy the found data to buf. This should be typically used
         * when reading possibly unaligned data such as timestamp, as time_t is 64-bit and size_t is 32-bit on
         * RISCV32. See issue #27241. */

        cmsg = cmsg_find(mh, level, type, CMSG_LEN(buf_len));
        अगर (!cmsg)
                वापस शून्य;

        वापस memcpy_safe(buf, CMSG_DATA(cmsg), buf_len);
}

माप_प्रकार sockaddr_ll_len(स्थिर काष्ठा sockaddr_ll *sa) {
        /* Certain hardware address types (e.g Infiniband) do not fit into sll_addr
         * (8 bytes) and run over the structure. This function returns the correct size that
         * must be passed to kernel. */

        निश्चित(sa->sll_family == AF_PACKET);

        माप_प्रकार mac_len = माप(sa->sll_addr);

        अगर (be16toh(sa->sll_hatype) == ARPHRD_ETHER)
                mac_len = MAX(mac_len, (माप_प्रकार) ETH_ALEN);
        अगर (be16toh(sa->sll_hatype) == ARPHRD_INFINIBAND)
                mac_len = MAX(mac_len, (माप_प्रकार) INFINIBAND_ALEN);

        वापस दुरत्व(काष्ठा sockaddr_ll, sll_addr) + mac_len;
}

माप_प्रकार sockaddr_un_len(स्थिर काष्ठा sockaddr_un *sa) {
        /* Covers only file system and abstract AF_UNIX socket addresses, but not unnamed socket addresses. */

        निश्चित(sa->sun_family == AF_UNIX);

        वापस दुरत्व(काष्ठा sockaddr_un, sun_path) +
                (sa->sun_path[0] == 0 ?
                        1 + strnlen(sa->sun_path+1, माप(sa->sun_path)-1) :
                        strnlen(sa->sun_path, माप(sa->sun_path))+1);
}

माप_प्रकार sockaddr_len(स्थिर जोड़ sockaddr_union *sa) {
        निश्चित(sa);

        चयन (sa->sa.sa_family) {
        हाल AF_INET:
                वापस माप(काष्ठा sockaddr_in);
        हाल AF_INET6:
                वापस माप(काष्ठा sockaddr_in6);
        हाल AF_UNIX:
                वापस sockaddr_un_len(&sa->un);
        हाल AF_PACKET:
                वापस sockaddr_ll_len(&sa->ll);
        हाल AF_NETLINK:
                वापस माप(काष्ठा sockaddr_nl);
        हाल AF_VSOCK:
                वापस माप(काष्ठा sockaddr_vm);
        शेष:
                assert_not_reached();
        }
}

पूर्णांक socket_ioctl_fd(व्योम) {
        पूर्णांक fd;

        /* Create a socket to invoke the various network interface ioctl()s on. Traditionally only AF_INET was good for
         * that. Since kernel 4.6 AF_NETLINK works for this too. We first try to use AF_INET hence, but if that's not
         * available (for example, because it is made unavailable via SECCOMP or such), we'll fall back to the more
         * generic AF_NETLINK. */

        fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
        अगर (fd < 0)
                fd = socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, NETLINK_GENERIC);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        वापस fd;
}

पूर्णांक sockaddr_un_unlink(स्थिर काष्ठा sockaddr_un *sa) {
        स्थिर अक्षर *p, * nul;

        निश्चित(sa);

        अगर (sa->sun_family != AF_UNIX)
                वापस -EPROTOTYPE;

        अगर (sa->sun_path[0] == 0) /* Nothing to do for abstract sockets */
                वापस 0;

        /* The path in .sun_path is not necessarily NUL terminated. Let's fix that. */
        nul = स_प्रथम(sa->sun_path, 0, माप(sa->sun_path));
        अगर (nul)
                p = sa->sun_path;
        अन्यथा
                p = memdupa_suffix0(sa->sun_path, माप(sa->sun_path));

        अगर (unlink(p) < 0)
                वापस -त्रुटि_सं;

        वापस 1;
}

पूर्णांक sockaddr_un_set_path(काष्ठा sockaddr_un *ret, स्थिर अक्षर *path) {
        माप_प्रकार l;

        निश्चित(ret);
        निश्चित(path);

        /* Initialize ret->sun_path from the specified argument. This will interpret paths starting with '@' as
         * abstract namespace sockets, and those starting with '/' as regular filesystem sockets. It won't accept
         * anything else (i.e. no relative paths), to avoid ambiguities. Note that this function cannot be used to
         * reference paths in the abstract namespace that include NUL bytes in the name. */

        l = म_माप(path);
        अगर (l < 2)
                वापस -EINVAL;
        अगर (!IN_SET(path[0], '/', '@'))
                वापस -EINVAL;

        /* Don't allow paths larger than the space in sockaddr_un. Note that we are a tiny bit more restrictive than
         * the kernel is: we insist on NUL termination (both for abstract namespace and regular file system socket
         * addresses!), which the kernel doesn't. We do this to reduce chance of incompatibility with other apps that
         * do not expect non-NUL terminated file system path. */
        अगर (l+1 > माप(ret->sun_path))
                वापस path[0] == '@' ? -EINVAL : -ENAMETOOLONG; /* return a recognizable error if this is
                                                                  * too long to fit into a sockaddr_un, but
                                                                  * is a file system path, and thus might be
                                                                  * connectible via O_PATH indirection. */

        *ret = (काष्ठा sockaddr_un) {
                .sun_family = AF_UNIX,
        };

        अगर (path[0] == '@') {
                /* Abstract namespace socket */
                स_नकल(ret->sun_path + 1, path + 1, l); /* copy *with* trailing NUL byte */
                वापस (पूर्णांक) (दुरत्व(काष्ठा sockaddr_un, sun_path) + l); /* 🔥 *don't* 🔥 include trailing NUL in size */

        } अन्यथा {
                निश्चित(path[0] == '/');

                /* File system socket */
                स_नकल(ret->sun_path, path, l + 1); /* copy *with* trailing NUL byte */
                वापस (पूर्णांक) (दुरत्व(काष्ठा sockaddr_un, sun_path) + l + 1); /* include trailing NUL in size */
        }
}

पूर्णांक getsockopt_int(पूर्णांक fd, पूर्णांक level, पूर्णांक optname, पूर्णांक *ret) {
        पूर्णांक v;
        socklen_t sl = माप(v);

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (getsockopt(fd, level, optname, &v, &sl) < 0)
                वापस negative_errno();
        अगर (sl != माप(v))
                वापस -EIO;

        *ret = v;
        वापस 0;
}

पूर्णांक socket_bind_to_ifname(पूर्णांक fd, स्थिर अक्षर *ifname) {
        निश्चित(fd >= 0);

        /* Call with NULL to drop binding */

        वापस RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen_ptr(ifname)));
}

पूर्णांक socket_bind_to_ifindex(पूर्णांक fd, पूर्णांक ifindex) {
        निश्चित(fd >= 0);

        अगर (ifindex <= 0)
                /* Drop binding */
                वापस RET_NERRNO(setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, शून्य, 0));

        वापस setsockopt_int(fd, SOL_SOCKET, SO_BINDTOIFINDEX, ifindex);
}

पूर्णांक socket_autobind(पूर्णांक fd, अक्षर **ret_name) {
        _cleanup_free_ अक्षर *name = शून्य;
        uint64_t random;
        पूर्णांक r;

        /* Generate a random abstract socket name and bind fd to it. This is modeled after the kernel
         * "autobind" feature, but uses 64-bit random number internally. */

        निश्चित(fd >= 0);

        random = random_u64();

        अगर (asprintf(&name, "@%" PRIu64, random) < 0)
                वापस -ENOMEM;

        जोड़ sockaddr_union sa;
        assert_cc(DECIMAL_STR_MAX(uint64_t) < माप(sa.un.sun_path));

        r = sockaddr_un_set_path(&sa.un, name);
        अगर (r < 0)
                वापस r;

        अगर (bind(fd, &sa.sa, r) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_name)
                *ret_name = TAKE_PTR(name);
        वापस 0;
}

ssize_t recvmsg_safe(पूर्णांक sockfd, काष्ठा msghdr *msg, पूर्णांक flags) {
        ssize_t n;

        /* A wrapper around recvmsg() that checks for MSG_CTRUNC and MSG_TRUNC, and turns them into an error,
         * in a reasonably safe way, closing any received fds in the error path.
         *
         * Note that unlike our usual coding style this might modify *msg on failure. */

        निश्चित(sockfd >= 0);
        निश्चित(msg);

        n = recvmsg(sockfd, msg, flags);
        अगर (n < 0)
                वापस -त्रुटि_सं;

        अगर (FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ||
            (!FLAGS_SET(flags, MSG_PEEK) && FLAGS_SET(msg->msg_flags, MSG_TRUNC))) {
                cmsg_close_all(msg);
                वापस FLAGS_SET(msg->msg_flags, MSG_CTRUNC) ? -ECHRNG : -EXFULL;
        }

        वापस n;
}

पूर्णांक socket_get_family(पूर्णांक fd) {
        पूर्णांक af;
        socklen_t sl = माप(af);

        अगर (getsockopt(fd, SOL_SOCKET, SO_DOMAIN, &af, &sl) < 0)
                वापस -त्रुटि_सं;

        अगर (sl != माप(af))
                वापस -EINVAL;

        वापस af;
}

पूर्णांक socket_set_recvpktinfo(पूर्णांक fd, पूर्णांक af, bool b) {

        अगर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                अगर (af < 0)
                        वापस af;
        }

        चयन (af) {

        हाल AF_INET:
                वापस setsockopt_int(fd, IPPROTO_IP, IP_PKTINFO, b);

        हाल AF_INET6:
                वापस setsockopt_int(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, b);

        हाल AF_NETLINK:
                वापस setsockopt_int(fd, SOL_NETLINK, NETLINK_PKTINFO, b);

        हाल AF_PACKET:
                वापस setsockopt_int(fd, SOL_PACKET, PACKET_AUXDATA, b);

        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक socket_set_unicast_if(पूर्णांक fd, पूर्णांक af, पूर्णांक ifi) {
        be32_t ifindex_be = htobe32(ifi);

        अगर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                अगर (af < 0)
                        वापस af;
        }

        चयन (af) {

        हाल AF_INET:
                वापस RET_NERRNO(setsockopt(fd, IPPROTO_IP, IP_UNICAST_IF, &ifindex_be, माप(ifindex_be)));

        हाल AF_INET6:
                वापस RET_NERRNO(setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_IF, &ifindex_be, माप(ifindex_be)));

        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक socket_set_option(पूर्णांक fd, पूर्णांक af, पूर्णांक opt_ipv4, पूर्णांक opt_ipv6, पूर्णांक val) {
        अगर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                अगर (af < 0)
                        वापस af;
        }

        चयन (af) {

        हाल AF_INET:
                वापस setsockopt_int(fd, IPPROTO_IP, opt_ipv4, val);

        हाल AF_INET6:
                वापस setsockopt_int(fd, IPPROTO_IPV6, opt_ipv6, val);

        शेष:
                वापस -EAFNOSUPPORT;
        }
}

पूर्णांक socket_get_mtu(पूर्णांक fd, पूर्णांक af, माप_प्रकार *ret) {
        पूर्णांक mtu, r;

        निश्चित(ret);

        अगर (af == AF_UNSPEC) {
                af = socket_get_family(fd);
                अगर (af < 0)
                        वापस af;
        }

        चयन (af) {

        हाल AF_INET:
                r = getsockopt_int(fd, IPPROTO_IP, IP_MTU, &mtu);
                अवरोध;

        हाल AF_INET6:
                r = getsockopt_int(fd, IPPROTO_IPV6, IPV6_MTU, &mtu);
                अवरोध;

        शेष:
                वापस -EAFNOSUPPORT;
        }

        अगर (r < 0)
                वापस r;
        अगर (mtu <= 0)
                वापस -EINVAL;

        *ret = (माप_प्रकार) mtu;
        वापस 0;
}

अटल पूर्णांक connect_unix_path_simple(पूर्णांक fd, स्थिर अक्षर *path) {
        जोड़ sockaddr_union sa = {
                .un.sun_family = AF_UNIX,
        };
        माप_प्रकार l;

        निश्चित(fd >= 0);
        निश्चित(path);

        l = म_माप(path);
        निश्चित(l > 0);
        निश्चित(l < माप(sa.un.sun_path));

        स_नकल(sa.un.sun_path, path, l + 1);
        वापस RET_NERRNO(connect(fd, &sa.sa, दुरत्व(काष्ठा sockaddr_un, sun_path) + l + 1));
}

अटल पूर्णांक connect_unix_inode(पूर्णांक fd, पूर्णांक inode_fd) {
        निश्चित(fd >= 0);
        निश्चित(inode_fd >= 0);

        वापस connect_unix_path_simple(fd, FORMAT_PROC_FD_PATH(inode_fd));
}

पूर्णांक connect_unix_path(पूर्णांक fd, पूर्णांक dir_fd, स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक inode_fd = -EBADF;

        निश्चित(fd >= 0);
        निश्चित(wildcard_fd_is_valid(dir_fd));

        /* Connects to the specified AF_UNIX socket in the file system. Works around the 108 byte size limit
         * in sockaddr_un, by going via O_PATH if needed. This hence works for any kind of path. */

        अगर (!path) {
                अगर (dir_fd < 0)
                        वापस -EISDIR;

                वापस connect_unix_inode(fd, dir_fd); /* If no path is specified, then dir_fd refers to the socket inode to connect to. */
        }

        /* Refuse zero length path early, to make sure AF_UNIX stack won't mistake this for an abstract
         * namespace path, since first char is NUL */
        अगर (isempty(path))
                वापस -EINVAL;

        /* Shortcut for the simple case */
        अगर (dir_fd == AT_FDCWD && म_माप(path) < sizeof_field(काष्ठा sockaddr_un, sun_path))
                वापस connect_unix_path_simple(fd, path);

        /* If dir_fd is specified, then we need to go the indirect O_PATH route, because connectat() does not
         * exist. If the path is too long, we also need to take the indirect route, since we can't fit this
         * into a sockaddr_un directly. */

        अगर (dir_fd == XAT_FDROOT) {
                _cleanup_free_ अक्षर *j = strjoin("/", path);
                अगर (!j)
                        वापस -ENOMEM;

                inode_fd = खोलो(j, O_PATH|O_CLOEXEC);
        } अन्यथा
                inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC);
        अगर (inode_fd < 0)
                वापस -त्रुटि_सं;

        वापस connect_unix_inode(fd, inode_fd);
}

पूर्णांक socket_address_parse_unix(SocketAddress *ret_address, स्थिर अक्षर *s) {
        काष्ठा sockaddr_un un;
        पूर्णांक r;

        निश्चित(ret_address);
        निश्चित(s);

        अगर (!IN_SET(*s, '/', '@'))
                वापस -EPROTO;

        r = sockaddr_un_set_path(&un, s);
        अगर (r < 0)
                वापस r;

        *ret_address = (SocketAddress) {
                .sockaddr.un = un,
                .आकार = r,
        };

        वापस 0;
}

पूर्णांक socket_address_equal_unix(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        SocketAddress socket_a, socket_b;
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = socket_address_parse_unix(&socket_a, a);
        अगर (r < 0)
                वापस r;

        r = socket_address_parse_unix(&socket_b, b);
        अगर (r < 0)
                वापस r;

        वापस sockaddr_equal(&socket_a.sockaddr, &socket_b.sockaddr);
}

पूर्णांक vsock_parse_port(स्थिर अक्षर *s, अचिन्हित *ret) {
        पूर्णांक r;

        निश्चित(ret);

        अगर (!s)
                वापस -EINVAL;

        अचिन्हित u;
        r = safe_atou(s, &u);
        अगर (r < 0)
                वापस r;

        /* Port 0 is apparently valid and not special in AF_VSOCK (unlike on IP). But VMADDR_PORT_ANY
         * (UINT32_MAX) is. Hence refuse that. */

        अगर (u == VMADDR_PORT_ANY)
                वापस -EINVAL;

        *ret = u;
        वापस 0;
}

पूर्णांक vsock_parse_cid(स्थिर अक्षर *s, अचिन्हित *ret) {
        निश्चित(ret);

        अगर (!s)
                वापस -EINVAL;

        /* Parsed an AF_VSOCK "CID". This is a 32bit entity, and the usual type is "unsigned". We recognize
         * the four special CIDs as strings, and otherwise parse the numeric CIDs. */

        अगर (streq(s, "hypervisor"))
                *ret = VMADDR_CID_HYPERVISOR;
        अन्यथा अगर (streq(s, "local"))
                *ret = VMADDR_CID_LOCAL;
        अन्यथा अगर (streq(s, "host"))
                *ret = VMADDR_CID_HOST;
        अन्यथा अगर (STR_IN_SET(s, "any", "-1"))
                *ret = VMADDR_CID_ANY;
        अन्यथा
                वापस safe_atou(s, ret);

        वापस 0;
}

पूर्णांक socket_address_parse_vsock(SocketAddress *ret_address, स्थिर अक्षर *s) {
        /* AF_VSOCK socket in vsock:cid:port notation */
        _cleanup_free_ अक्षर *n = शून्य;
        स्थिर अक्षर *e, *cid_start;
        अचिन्हित port, cid;
        पूर्णांक type, r;

        निश्चित(ret_address);
        निश्चित(s);

        अगर ((cid_start = startswith(s, "vsock:")))
                type = 0;
        अन्यथा अगर ((cid_start = startswith(s, "vsock-dgram:")))
                type = SOCK_DGRAM;
        अन्यथा अगर ((cid_start = startswith(s, "vsock-seqpacket:")))
                type = SOCK_SEQPACKET;
        अन्यथा अगर ((cid_start = startswith(s, "vsock-stream:")))
                type = SOCK_STREAM;
        अन्यथा
                वापस -EPROTO;

        e = म_अक्षर(cid_start, ':');
        अगर (!e)
                वापस -EINVAL;

        r = vsock_parse_port(e+1, &port);
        अगर (r < 0)
                वापस r;

        n = strndup(cid_start, e - cid_start);
        अगर (!n)
                वापस -ENOMEM;

        अगर (isempty(n))
                cid = VMADDR_CID_ANY;
        अन्यथा {
                r = vsock_parse_cid(n, &cid);
                अगर (r < 0)
                        वापस r;
        }

        *ret_address = (SocketAddress) {
                .sockaddr.vm = {
                        .svm_family = AF_VSOCK,
                        .svm_cid = cid,
                        .svm_port = port,
                },
                .type = type,
                .आकार = माप(काष्ठा sockaddr_vm),
        };

        वापस 0;
}

पूर्णांक netlink_socket_get_multicast_groups(पूर्णांक fd, माप_प्रकार *ret_len, uint32_t **ret_groups) {
        _cleanup_free_ uint32_t *groups = शून्य;
        socklen_t len = 0, old_len;

        निश्चित(fd >= 0);

        अगर (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, शून्य, &len) < 0)
                वापस -त्रुटि_सं;

        अगर (len == 0)
                जाओ finalize;

        groups = new0(uint32_t, len);
        अगर (!groups)
                वापस -ENOMEM;

        old_len = len;

        अगर (getsockopt(fd, SOL_NETLINK, NETLINK_LIST_MEMBERSHIPS, groups, &len) < 0)
                वापस -त्रुटि_सं;

        अगर (old_len != len)
                वापस -EIO;

finalize:
        अगर (ret_len)
                *ret_len = len;
        अगर (ret_groups)
                *ret_groups = TAKE_PTR(groups);

        वापस 0;
}

पूर्णांक socket_get_cookie(पूर्णांक fd, uint64_t *ret) {
        निश्चित(fd >= 0);

        uint64_t cookie = 0;
        socklen_t cookie_len = माप(cookie);
        अगर (getsockopt(fd, SOL_SOCKET, SO_COOKIE, &cookie, &cookie_len) < 0)
                वापस -त्रुटि_सं;

        निश्चित(cookie_len == माप(cookie));
        अगर (ret)
                *ret = cookie;

        वापस 0;
}

व्योम cmsg_close_all(काष्ठा msghdr *mh) {
        निश्चित(mh);

        काष्ठा cmsghdr *cmsg;
        CMSG_FOREACH(cmsg, mh) {
                अगर (cmsg->cmsg_level != SOL_SOCKET)
                        जारी;

                अगर (cmsg->cmsg_type == SCM_RIGHTS)
                        close_many(CMSG_TYPED_तथ्य(cmsg, पूर्णांक),
                                   (cmsg->cmsg_len - CMSG_LEN(0)) / माप(पूर्णांक));
                अन्यथा अगर (cmsg->cmsg_type == SCM_PIDFD) {
                        निश्चित(cmsg->cmsg_len == CMSG_LEN(माप(पूर्णांक)));
                        safe_close(*CMSG_TYPED_तथ्य(cmsg, पूर्णांक));
                }
        }
}

पूर्णांक tos_to_priority(uint8_t tos) {
        /* Map the IP Precedence (top 3 bits of the TOS field) to Linux internal packet priorities
         * (TC_PRIO_*). This exactly mirrors the standard Linux kernel IP precedence-to-priority mapping
         * (rt_tos2priority) to ensure consistent behavior when explicitly setting SO_PRIORITY. */
        चयन (IPTOS_PREC(tos)) {
        हाल IPTOS_PREC_NETCONTROL:      /* 0xc0 (CS7) - Network Control. Used for infrastructure control (e.g., STP, keepalives). */
        हाल IPTOS_PREC_INTERNETCONTROL: /* 0xe0 (CS6) - Internetwork Control. Used for routing protocols (e.g., OSPF, BGP) and DHCP. */
                वापस TC_PRIO_CONTROL;

        हाल IPTOS_PREC_CRITIC_ECP:      /* 0xa0 (CS5) - Critical. Used for delay-sensitive traffic like Voice over IP (VoIP). */
        हाल IPTOS_PREC_FLASHOVERRIDE:   /* 0x80 (CS4) - Flash Override. Used for interactive video and multimedia. */
                वापस TC_PRIO_INTERACTIVE;

        हाल IPTOS_PREC_FLASH:           /* 0x60 (CS3) - Flash. Used for broadcast video and call signaling (e.g., SIP). */
        हाल IPTOS_PREC_IMMEDIATE:       /* 0x40 (CS2) - Immediate. Used for OAM (Operations, Administration, and Management) and transactional data. */
                वापस TC_PRIO_INTERACTIVE_BULK;

        हाल IPTOS_PREC_PRIORITY:        /* 0x20 (CS1) - Priority. Used for background traffic and bulk data transfers. */
                वापस TC_PRIO_BULK;

        हाल IPTOS_PREC_ROUTINE:         /* 0x00 (CS0) - Routine. Best effort traffic. */
        शेष:
                वापस TC_PRIO_BESTEFFORT;
        }
}

पूर्णांक socket_xattr_supported(व्योम) {
        पूर्णांक r;

        // FIXME: Drop this check once Linux 7.1 becomes our baseline

        अटल पूर्णांक cached = -1;
        अगर (cached >= 0)
                वापस cached;

        _cleanup_close_ पूर्णांक fd = socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, /* protocol= */ 0);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        /* Old kernels return EPERM. But let's also check for more appropriate error codes, to be friendly to
         * seccomp policies */
        r = xsetxattr(fd, /* path= */ शून्य, AT_EMPTY_PATH, "user.testxxx", "1");
        अगर (ERRNO_IS_NEG_NOT_SUPPORTED(r) || r == -EPERM)
                वापस (cached = false);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to set test xattr on socket: %m");

        वापस (cached = true);
}
