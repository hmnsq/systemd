/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <linux/if_packet.h>
#ضمن <linux/netlink.h>
#ضمن <linux/vm_sockets.h>
#ضمن <netinet/in.h>
#ضمن <sys/socket.h>
#ضمن <sys/un.h>

#ضمن "forward.h"
#ضمن "memory-util.h"
#ضمن "missing-network.h"

اتحاد sockaddr_union {
        /* The minimal, abstract version */
        بنية sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        بنية sockaddr_storage storage;

        /* Protocol-specific implementations */
        بنية sockaddr_in in;
        بنية sockaddr_in6 in6;
        بنية sockaddr_un un;
        بنية sockaddr_nl nl;
        بنية sockaddr_ll ll;
        بنية sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(بنية sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[حجم(بنية sockaddr_un) + 1];
};

#define SUN_PATH_LEN (حجم(((بنية sockaddr_un){}).sun_path))

تعريفنوع بنية SocketAddress {
        اتحاد sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        صحيح type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        صحيح protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, صحيح);

صحيح sockaddr_un_unlink(ثابت بنية sockaddr_un *sa);

ساكن مضمن صحيح socket_address_unlink(ثابت SocketAddress *a) {
        إرجاع socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ثابت SocketAddress *a) _pure_;

صحيح socket_address_verify(ثابت SocketAddress *a, bool strict) _pure_;
صحيح socket_address_print(ثابت SocketAddress *a, حرف **ret);
bool socket_address_matches_fd(ثابت SocketAddress *a, صحيح fd);

bool socket_address_equal(ثابت SocketAddress *a, ثابت SocketAddress *b) _pure_;

ثابت حرف* socket_address_get_path(ثابت SocketAddress *a);

bool socket_ipv6_is_supported(فراغ);
bool socket_ipv6_is_enabled(فراغ);

صحيح sockaddr_port(ثابت بنية sockaddr *_sa, unsigned *port);
ثابت اتحاد in_addr_union *sockaddr_in_addr(ثابت بنية sockaddr *sa);
صحيح sockaddr_set_in_addr(اتحاد sockaddr_union *u, صحيح family, ثابت اتحاد in_addr_union *a, uint16_t port);

صحيح sockaddr_pretty(ثابت بنية sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, حرف **ret);
صحيح getpeername_pretty(صحيح fd, bool include_port, حرف **ret);
صحيح getsockname_pretty(صحيح fd, حرف **ret);

صحيح socknameinfo_pretty(ثابت بنية sockaddr *sa, socklen_t salen, حرف **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, صحيح);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b);

صحيح fd_set_sndbuf(صحيح fd, size_t n, bool increase);
ساكن مضمن صحيح fd_inc_sndbuf(صحيح fd, size_t n) {
        إرجاع fd_set_sndbuf(fd, n, true);
}
صحيح fd_set_rcvbuf(صحيح fd, size_t n, bool increase);
ساكن مضمن صحيح fd_increase_rxbuf(صحيح fd, size_t n) {
        إرجاع fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, صحيح);

تعريفنوع تعداد {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(حرف a) _const_;
bool ifname_valid_full(ثابت حرف *p, IfnameValidFlags flags) _pure_;
ساكن مضمن bool ifname_valid(ثابت حرف *p) {
        إرجاع ifname_valid_full(p, 0);
}
bool address_label_valid(ثابت حرف *p) _pure_;

صحيح getpeercred(صحيح fd, بنية ucred *ucred);
صحيح getpeersec(صحيح fd, حرف **ret);
صحيح getpeergroups(صحيح fd, gid_t **ret);
صحيح getpeerpidfd(صحيح fd);
صحيح getpeerpidref(صحيح fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية iovec *iov, size_t iovlen,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags);
صحيح send_one_fd_sa(صحيح transport_fd,
                   صحيح fd,
                   ثابت بنية sockaddr *sa, socklen_t len,
                   صحيح flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, عدم, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, عدم, 0, عدم, 0, flags)
ssize_t receive_one_fd_iov(صحيح transport_fd, بنية iovec *iov, size_t iovlen, صحيح flags, صحيح *ret_fd);
صحيح receive_one_fd(صحيح transport_fd, صحيح flags);

ssize_t next_datagram_size_fd(صحيح fd);

صحيح flush_accept(صحيح fd);
ssize_t flush_mqueue(صحيح fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        لكل ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                بنية cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(بنية cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) عدم; \
        })

بنية cmsghdr* cmsg_find(بنية msghdr *mh, صحيح level, صحيح type, socklen_t length);
فراغ* cmsg_find_and_copy_data(بنية msghdr *mh, صحيح level, صحيح type, فراغ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(حجم(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, حجم(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        اتحاد {                                                         \
                بنية cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ثابت بنية sockaddr_ll *sa);
size_t sockaddr_un_len(ثابت بنية sockaddr_un *sa);
size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa);

صحيح socket_ioctl_fd(فراغ);

صحيح sockaddr_un_set_path(بنية sockaddr_un *ret, ثابت حرف *path);

ساكن مضمن صحيح setsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح قيمة) {
        إذا (setsockopt(fd, level, optname, &قيمة, حجم(قيمة)) < 0)
                إرجاع -errno;

        إرجاع 0;
}

صحيح getsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح *ret);

صحيح socket_bind_to_ifname(صحيح fd, ثابت حرف *ifname);
صحيح socket_bind_to_ifindex(صحيح fd, صحيح ifindex);

صحيح socket_autobind(صحيح fd, حرف **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(حجم(بنية timeval)) + CMSG_SPACE(2 * حجم(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(حجم(بنية timespec)) + CMSG_SPACE(2 * حجم(uint64_t)))

ssize_t recvmsg_safe(صحيح sockfd, بنية msghdr *msg, صحيح flags);

صحيح socket_get_family(صحيح fd);
صحيح socket_set_recvpktinfo(صحيح fd, صحيح af, bool b);
صحيح socket_set_unicast_if(صحيح fd, صحيح af, صحيح ifi);

صحيح socket_set_option(صحيح fd, صحيح af, صحيح opt_ipv4, صحيح opt_ipv6, صحيح val);
ساكن مضمن صحيح socket_set_recverr(صحيح fd, صحيح af, bool b) {
        إرجاع socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ساكن مضمن صحيح socket_set_recvttl(صحيح fd, صحيح af, bool b) {
        إرجاع socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ساكن مضمن صحيح socket_set_ttl(صحيح fd, صحيح af, صحيح ttl) {
        إرجاع socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ساكن مضمن صحيح socket_set_freebind(صحيح fd, صحيح af, bool b) {
        إرجاع socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ساكن مضمن صحيح socket_set_transparent(صحيح fd, صحيح af, bool b) {
        إرجاع socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ساكن مضمن صحيح socket_set_recvfragsize(صحيح fd, صحيح af, bool b) {
        إرجاع socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

صحيح socket_get_mtu(صحيح fd, صحيح af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

صحيح connect_unix_path(صحيح fd, صحيح dir_fd, ثابت حرف *path);

ساكن مضمن bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        إرجاع !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

صحيح vsock_parse_port(ثابت حرف *s, unsigned *ret);
صحيح vsock_parse_cid(ثابت حرف *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
صحيح socket_address_parse_unix(SocketAddress *ret_address, ثابت حرف *s);
صحيح socket_address_parse_vsock(SocketAddress *ret_address, ثابت حرف *s);
صحيح socket_address_equal_unix(ثابت حرف *a, ثابت حرف *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

صحيح netlink_socket_get_multicast_groups(صحيح fd, size_t *ret_len, uint32_t **ret_groups);

صحيح socket_get_cookie(صحيح fd, uint64_t *ret);

فراغ cmsg_close_all(بنية msghdr *mh);

صحيح tos_to_priority(uint8_t tos);

صحيح socket_xattr_supported(فراغ);
