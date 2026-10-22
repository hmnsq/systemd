/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <linux/if_packet.h>
#شامل <linux/netlink.h>
#شامل <linux/vm_sockets.h>
#شامل <netinet/in.h>
#شامل <sys/socket.h>
#شامل <sys/un.h>

#شامل "forward.h"
#شامل "memory-util.h"
#شامل "missing-network.h"

اجتماع sockaddr_union {
        /* The minimal, abstract version */
        ساختار sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ساختار sockaddr_storage storage;

        /* Protocol-specific implementations */
        ساختار sockaddr_in in;
        ساختار sockaddr_in6 in6;
        ساختار sockaddr_un un;
        ساختار sockaddr_nl nl;
        ساختار sockaddr_ll ll;
        ساختار sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(ساختار sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[اندازه(ساختار sockaddr_un) + 1];
};

#define SUN_PATH_LEN (اندازه(((ساختار sockaddr_un){}).sun_path))

تعریفنوع ساختار SocketAddress {
        اجتماع sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        صحیح type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        صحیح protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, صحیح);

صحیح sockaddr_un_unlink(ثابت ساختار sockaddr_un *sa);

ایستا درونخطی صحیح socket_address_unlink(ثابت SocketAddress *a) {
        بازگشت socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ثابت SocketAddress *a) _pure_;

صحیح socket_address_verify(ثابت SocketAddress *a, bool strict) _pure_;
صحیح socket_address_print(ثابت SocketAddress *a, نویسه **ret);
bool socket_address_matches_fd(ثابت SocketAddress *a, صحیح fd);

bool socket_address_equal(ثابت SocketAddress *a, ثابت SocketAddress *b) _pure_;

ثابت نویسه* socket_address_get_path(ثابت SocketAddress *a);

bool socket_ipv6_is_supported(تهی);
bool socket_ipv6_is_enabled(تهی);

صحیح sockaddr_port(ثابت ساختار sockaddr *_sa, unsigned *port);
ثابت اجتماع in_addr_union *sockaddr_in_addr(ثابت ساختار sockaddr *sa);
صحیح sockaddr_set_in_addr(اجتماع sockaddr_union *u, صحیح family, ثابت اجتماع in_addr_union *a, uint16_t port);

صحیح sockaddr_pretty(ثابت ساختار sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, نویسه **ret);
صحیح getpeername_pretty(صحیح fd, bool include_port, نویسه **ret);
صحیح getsockname_pretty(صحیح fd, نویسه **ret);

صحیح socknameinfo_pretty(ثابت ساختار sockaddr *sa, socklen_t salen, نویسه **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, صحیح);

bool sockaddr_equal(ثابت اجتماع sockaddr_union *a, ثابت اجتماع sockaddr_union *b);

صحیح fd_set_sndbuf(صحیح fd, size_t n, bool increase);
ایستا درونخطی صحیح fd_inc_sndbuf(صحیح fd, size_t n) {
        بازگشت fd_set_sndbuf(fd, n, true);
}
صحیح fd_set_rcvbuf(صحیح fd, size_t n, bool increase);
ایستا درونخطی صحیح fd_increase_rxbuf(صحیح fd, size_t n) {
        بازگشت fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, صحیح);

تعریفنوع شمارش {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(نویسه a) _const_;
bool ifname_valid_full(ثابت نویسه *p, IfnameValidFlags flags) _pure_;
ایستا درونخطی bool ifname_valid(ثابت نویسه *p) {
        بازگشت ifname_valid_full(p, 0);
}
bool address_label_valid(ثابت نویسه *p) _pure_;

صحیح getpeercred(صحیح fd, ساختار ucred *ucred);
صحیح getpeersec(صحیح fd, نویسه **ret);
صحیح getpeergroups(صحیح fd, gid_t **ret);
صحیح getpeerpidfd(صحیح fd);
صحیح getpeerpidref(صحیح fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساختار iovec *iov, size_t iovlen,
                ثابت ساختار sockaddr *sa, socklen_t len,
                صحیح flags);
صحیح send_one_fd_sa(صحیح transport_fd,
                   صحیح fd,
                   ثابت ساختار sockaddr *sa, socklen_t len,
                   صحیح flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, پوچ, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, پوچ, 0, پوچ, 0, flags)
ssize_t receive_one_fd_iov(صحیح transport_fd, ساختار iovec *iov, size_t iovlen, صحیح flags, صحیح *ret_fd);
صحیح receive_one_fd(صحیح transport_fd, صحیح flags);

ssize_t next_datagram_size_fd(صحیح fd);

صحیح flush_accept(صحیح fd);
ssize_t flush_mqueue(صحیح fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        برای ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                ساختار cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ساختار cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) پوچ; \
        })

ساختار cmsghdr* cmsg_find(ساختار msghdr *mh, صحیح level, صحیح type, socklen_t length);
تهی* cmsg_find_and_copy_data(ساختار msghdr *mh, صحیح level, صحیح type, تهی *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(اندازه(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, اندازه(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        اجتماع {                                                         \
                ساختار cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ثابت ساختار sockaddr_ll *sa);
size_t sockaddr_un_len(ثابت ساختار sockaddr_un *sa);
size_t sockaddr_len(ثابت اجتماع sockaddr_union *sa);

صحیح socket_ioctl_fd(تهی);

صحیح sockaddr_un_set_path(ساختار sockaddr_un *ret, ثابت نویسه *path);

ایستا درونخطی صحیح setsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح مقدار) {
        اگر (setsockopt(fd, level, optname, &مقدار, اندازه(مقدار)) < 0)
                بازگشت -errno;

        بازگشت 0;
}

صحیح getsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح *ret);

صحیح socket_bind_to_ifname(صحیح fd, ثابت نویسه *ifname);
صحیح socket_bind_to_ifindex(صحیح fd, صحیح ifindex);

صحیح socket_autobind(صحیح fd, نویسه **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(اندازه(ساختار timeval)) + CMSG_SPACE(2 * اندازه(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(اندازه(ساختار timespec)) + CMSG_SPACE(2 * اندازه(uint64_t)))

ssize_t recvmsg_safe(صحیح sockfd, ساختار msghdr *msg, صحیح flags);

صحیح socket_get_family(صحیح fd);
صحیح socket_set_recvpktinfo(صحیح fd, صحیح af, bool b);
صحیح socket_set_unicast_if(صحیح fd, صحیح af, صحیح ifi);

صحیح socket_set_option(صحیح fd, صحیح af, صحیح opt_ipv4, صحیح opt_ipv6, صحیح val);
ایستا درونخطی صحیح socket_set_recverr(صحیح fd, صحیح af, bool b) {
        بازگشت socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ایستا درونخطی صحیح socket_set_recvttl(صحیح fd, صحیح af, bool b) {
        بازگشت socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ایستا درونخطی صحیح socket_set_ttl(صحیح fd, صحیح af, صحیح ttl) {
        بازگشت socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ایستا درونخطی صحیح socket_set_freebind(صحیح fd, صحیح af, bool b) {
        بازگشت socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ایستا درونخطی صحیح socket_set_transparent(صحیح fd, صحیح af, bool b) {
        بازگشت socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ایستا درونخطی صحیح socket_set_recvfragsize(صحیح fd, صحیح af, bool b) {
        بازگشت socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

صحیح socket_get_mtu(صحیح fd, صحیح af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

صحیح connect_unix_path(صحیح fd, صحیح dir_fd, ثابت نویسه *path);

ایستا درونخطی bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        بازگشت !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

صحیح vsock_parse_port(ثابت نویسه *s, unsigned *ret);
صحیح vsock_parse_cid(ثابت نویسه *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
صحیح socket_address_parse_unix(SocketAddress *ret_address, ثابت نویسه *s);
صحیح socket_address_parse_vsock(SocketAddress *ret_address, ثابت نویسه *s);
صحیح socket_address_equal_unix(ثابت نویسه *a, ثابت نویسه *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

صحیح netlink_socket_get_multicast_groups(صحیح fd, size_t *ret_len, uint32_t **ret_groups);

صحیح socket_get_cookie(صحیح fd, uint64_t *ret);

تهی cmsg_close_all(ساختار msghdr *mh);

صحیح tos_to_priority(uint8_t tos);

صحیح socket_xattr_supported(تهی);
