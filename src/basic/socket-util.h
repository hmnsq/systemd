/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <linux/if_packet.h>
#अन्तर्भाव <linux/netlink.h>
#अन्तर्भाव <linux/vm_sockets.h>
#अन्तर्भाव <netinet/in.h>
#अन्तर्भाव <sys/socket.h>
#अन्तर्भाव <sys/un.h>

#अन्तर्भाव "forward.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "missing-network.h"

सङ्घ sockaddr_union {
        /* The minimal, abstract version */
        संरचना sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        संरचना sockaddr_storage storage;

        /* Protocol-specific implementations */
        संरचना sockaddr_in in;
        संरचना sockaddr_in6 in6;
        संरचना sockaddr_un un;
        संरचना sockaddr_nl nl;
        संरचना sockaddr_ll ll;
        संरचना sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[दुरत्व(संरचना sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[परिमाण(संरचना sockaddr_un) + 1];
};

#define SUN_PATH_LEN (परिमाण(((संरचना sockaddr_un){}).sun_path))

प्रकारनिर्देश संरचना SocketAddress {
        सङ्घ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        पूर्णाङ्क type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        पूर्णाङ्क protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, पूर्णाङ्क);

पूर्णाङ्क sockaddr_un_unlink(ध्रुव संरचना sockaddr_un *sa);

स्थिर अन्तःस्थ पूर्णाङ्क socket_address_unlink(ध्रुव SocketAddress *a) {
        प्रत्यागम socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ध्रुव SocketAddress *a) _pure_;

पूर्णाङ्क socket_address_verify(ध्रुव SocketAddress *a, bool strict) _pure_;
पूर्णाङ्क socket_address_print(ध्रुव SocketAddress *a, वर्ण **ret);
bool socket_address_matches_fd(ध्रुव SocketAddress *a, पूर्णाङ्क fd);

bool socket_address_equal(ध्रुव SocketAddress *a, ध्रुव SocketAddress *b) _pure_;

ध्रुव वर्ण* socket_address_get_path(ध्रुव SocketAddress *a);

bool socket_ipv6_is_supported(शून्य);
bool socket_ipv6_is_enabled(शून्य);

पूर्णाङ्क sockaddr_port(ध्रुव संरचना sockaddr *_sa, अचिन्हित *port);
ध्रुव सङ्घ in_addr_union *sockaddr_in_addr(ध्रुव संरचना sockaddr *sa);
पूर्णाङ्क sockaddr_set_in_addr(सङ्घ sockaddr_union *u, पूर्णाङ्क family, ध्रुव सङ्घ in_addr_union *a, uint16_t port);

पूर्णाङ्क sockaddr_pretty(ध्रुव संरचना sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, वर्ण **ret);
पूर्णाङ्क getpeername_pretty(पूर्णाङ्क fd, bool include_port, वर्ण **ret);
पूर्णाङ्क getsockname_pretty(पूर्णाङ्क fd, वर्ण **ret);

पूर्णाङ्क socknameinfo_pretty(ध्रुव संरचना sockaddr *sa, socklen_t salen, वर्ण **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, पूर्णाङ्क);

bool sockaddr_equal(ध्रुव सङ्घ sockaddr_union *a, ध्रुव सङ्घ sockaddr_union *b);

पूर्णाङ्क fd_set_sndbuf(पूर्णाङ्क fd, माप_प्रकार n, bool increase);
स्थिर अन्तःस्थ पूर्णाङ्क fd_inc_sndbuf(पूर्णाङ्क fd, माप_प्रकार n) {
        प्रत्यागम fd_set_sndbuf(fd, n, true);
}
पूर्णाङ्क fd_set_rcvbuf(पूर्णाङ्क fd, माप_प्रकार n, bool increase);
स्थिर अन्तःस्थ पूर्णाङ्क fd_increase_rxbuf(पूर्णाङ्क fd, माप_प्रकार n) {
        प्रत्यागम fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, पूर्णाङ्क);

प्रकारनिर्देश गणना {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(वर्ण a) _const_;
bool ifname_valid_full(ध्रुव वर्ण *p, IfnameValidFlags flags) _pure_;
स्थिर अन्तःस्थ bool ifname_valid(ध्रुव वर्ण *p) {
        प्रत्यागम ifname_valid_full(p, 0);
}
bool address_label_valid(ध्रुव वर्ण *p) _pure_;

पूर्णाङ्क getpeercred(पूर्णाङ्क fd, संरचना ucred *ucred);
पूर्णाङ्क getpeersec(पूर्णाङ्क fd, वर्ण **ret);
पूर्णाङ्क getpeergroups(पूर्णाङ्क fd, gid_t **ret);
पूर्णाङ्क getpeerpidfd(पूर्णाङ्क fd);
पूर्णाङ्क getpeerpidref(पूर्णाङ्क fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                पूर्णाङ्क transport_fd,
                पूर्णाङ्क fd,
                ध्रुव संरचना iovec *iov, माप_प्रकार iovlen,
                ध्रुव संरचना sockaddr *sa, socklen_t len,
                पूर्णाङ्क flags);
पूर्णाङ्क send_one_fd_sa(पूर्णाङ्क transport_fd,
                   पूर्णाङ्क fd,
                   ध्रुव संरचना sockaddr *sa, socklen_t len,
                   पूर्णाङ्क flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, रिक्त, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, रिक्त, 0, रिक्त, 0, flags)
ssize_t receive_one_fd_iov(पूर्णाङ्क transport_fd, संरचना iovec *iov, माप_प्रकार iovlen, पूर्णाङ्क flags, पूर्णाङ्क *ret_fd);
पूर्णाङ्क receive_one_fd(पूर्णाङ्क transport_fd, पूर्णाङ्क flags);

ssize_t next_datagram_size_fd(पूर्णाङ्क fd);

पूर्णाङ्क flush_accept(पूर्णाङ्क fd);
ssize_t flush_mqueue(पूर्णाङ्क fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        प्रति ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                संरचना cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(संरचना cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) रिक्त; \
        })

संरचना cmsghdr* cmsg_find(संरचना msghdr *mh, पूर्णाङ्क level, पूर्णाङ्क type, socklen_t length);
शून्य* cmsg_find_and_copy_data(संरचना msghdr *mh, पूर्णाङ्क level, पूर्णाङ्क type, शून्य *buf, माप_प्रकार buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(परिमाण(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, परिमाण(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        सङ्घ {                                                         \
                संरचना cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

माप_प्रकार sockaddr_ll_len(ध्रुव संरचना sockaddr_ll *sa);
माप_प्रकार sockaddr_un_len(ध्रुव संरचना sockaddr_un *sa);
माप_प्रकार sockaddr_len(ध्रुव सङ्घ sockaddr_union *sa);

पूर्णाङ्क socket_ioctl_fd(शून्य);

पूर्णाङ्क sockaddr_un_set_path(संरचना sockaddr_un *ret, ध्रुव वर्ण *path);

स्थिर अन्तःस्थ पूर्णाङ्क setsockopt_int(पूर्णाङ्क fd, पूर्णाङ्क level, पूर्णाङ्क optname, पूर्णाङ्क मूल्य) {
        यदि (setsockopt(fd, level, optname, &मूल्य, परिमाण(मूल्य)) < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रत्यागम 0;
}

पूर्णाङ्क getsockopt_int(पूर्णाङ्क fd, पूर्णाङ्क level, पूर्णाङ्क optname, पूर्णाङ्क *ret);

पूर्णाङ्क socket_bind_to_ifname(पूर्णाङ्क fd, ध्रुव वर्ण *ifname);
पूर्णाङ्क socket_bind_to_ifindex(पूर्णाङ्क fd, पूर्णाङ्क ifindex);

पूर्णाङ्क socket_autobind(पूर्णाङ्क fd, वर्ण **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(परिमाण(संरचना timeval)) + CMSG_SPACE(2 * परिमाण(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(परिमाण(संरचना timespec)) + CMSG_SPACE(2 * परिमाण(uint64_t)))

ssize_t recvmsg_safe(पूर्णाङ्क sockfd, संरचना msghdr *msg, पूर्णाङ्क flags);

पूर्णाङ्क socket_get_family(पूर्णाङ्क fd);
पूर्णाङ्क socket_set_recvpktinfo(पूर्णाङ्क fd, पूर्णाङ्क af, bool b);
पूर्णाङ्क socket_set_unicast_if(पूर्णाङ्क fd, पूर्णाङ्क af, पूर्णाङ्क ifi);

पूर्णाङ्क socket_set_option(पूर्णाङ्क fd, पूर्णाङ्क af, पूर्णाङ्क opt_ipv4, पूर्णाङ्क opt_ipv6, पूर्णाङ्क val);
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_recverr(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {
        प्रत्यागम socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_recvttl(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {
        प्रत्यागम socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_ttl(पूर्णाङ्क fd, पूर्णाङ्क af, पूर्णाङ्क ttl) {
        प्रत्यागम socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_freebind(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {
        प्रत्यागम socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_transparent(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {
        प्रत्यागम socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
स्थिर अन्तःस्थ पूर्णाङ्क socket_set_recvfragsize(पूर्णाङ्क fd, पूर्णाङ्क af, bool b) {
        प्रत्यागम socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

पूर्णाङ्क socket_get_mtu(पूर्णाङ्क fd, पूर्णाङ्क af, माप_प्रकार *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

पूर्णाङ्क connect_unix_path(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path);

स्थिर अन्तःस्थ bool VSOCK_CID_IS_REGULAR(अचिन्हित cid) {
        /* Refuse special CID values */
        प्रत्यागम !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

पूर्णाङ्क vsock_parse_port(ध्रुव वर्ण *s, अचिन्हित *ret);
पूर्णाङ्क vsock_parse_cid(ध्रुव वर्ण *s, अचिन्हित *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
पूर्णाङ्क socket_address_parse_unix(SocketAddress *ret_address, ध्रुव वर्ण *s);
पूर्णाङ्क socket_address_parse_vsock(SocketAddress *ret_address, ध्रुव वर्ण *s);
पूर्णाङ्क socket_address_equal_unix(ध्रुव वर्ण *a, ध्रुव वर्ण *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE पूर्णांक_उच्च

पूर्णाङ्क netlink_socket_get_multicast_groups(पूर्णाङ्क fd, माप_प्रकार *ret_len, uint32_t **ret_groups);

पूर्णाङ्क socket_get_cookie(पूर्णाङ्क fd, uint64_t *ret);

शून्य cmsg_close_all(संरचना msghdr *mh);

पूर्णाङ्क tos_to_priority(uint8_t tos);

पूर्णाङ्क socket_xattr_supported(शून्य);
