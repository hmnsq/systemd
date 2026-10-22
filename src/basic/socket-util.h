/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <linux/if_packet.h>
#அந்தர்பாவ <linux/netlink.h>
#அந்தர்பாவ <linux/vm_sockets.h>
#அந்தர்பாவ <netinet/in.h>
#அந்தர்பாவ <sys/socket.h>
#அந்தர்பாவ <sys/un.h>

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "missing-network.h"

ஒன்றிப்பு sockaddr_union {
        /* The minimal, abstract version */
        கட்டமைப்பு sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        கட்டமைப்பு sockaddr_storage storage;

        /* Protocol-specific implementations */
        கட்டமைப்பு sockaddr_in in;
        கட்டமைப்பு sockaddr_in6 in6;
        கட்டமைப்பு sockaddr_un un;
        கட்டமைப்பு sockaddr_nl nl;
        கட்டமைப்பு sockaddr_ll ll;
        கட்டமைப்பு sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(கட்டமைப்பு sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[அளவு(கட்டமைப்பு sockaddr_un) + 1];
};

#define SUN_PATH_LEN (அளவு(((கட்டமைப்பு sockaddr_un){}).sun_path))

வகைவரை கட்டமைப்பு SocketAddress {
        ஒன்றிப்பு sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        முழு type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        முழு protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, முழு);

முழு sockaddr_un_unlink(மாறிலி கட்டமைப்பு sockaddr_un *sa);

நிலையான உள்வரி முழு socket_address_unlink(மாறிலி SocketAddress *a) {
        திரும்பு socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(மாறிலி SocketAddress *a) _pure_;

முழு socket_address_verify(மாறிலி SocketAddress *a, bool strict) _pure_;
முழு socket_address_print(மாறிலி SocketAddress *a, எழுத்து **ret);
bool socket_address_matches_fd(மாறிலி SocketAddress *a, முழு fd);

bool socket_address_equal(மாறிலி SocketAddress *a, மாறிலி SocketAddress *b) _pure_;

மாறிலி எழுத்து* socket_address_get_path(மாறிலி SocketAddress *a);

bool socket_ipv6_is_supported(வெறுமை);
bool socket_ipv6_is_enabled(வெறுமை);

முழு sockaddr_port(மாறிலி கட்டமைப்பு sockaddr *_sa, unsigned *port);
மாறிலி ஒன்றிப்பு in_addr_union *sockaddr_in_addr(மாறிலி கட்டமைப்பு sockaddr *sa);
முழு sockaddr_set_in_addr(ஒன்றிப்பு sockaddr_union *u, முழு family, மாறிலி ஒன்றிப்பு in_addr_union *a, uint16_t port);

முழு sockaddr_pretty(மாறிலி கட்டமைப்பு sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, எழுத்து **ret);
முழு getpeername_pretty(முழு fd, bool include_port, எழுத்து **ret);
முழு getsockname_pretty(முழு fd, எழுத்து **ret);

முழு socknameinfo_pretty(மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t salen, எழுத்து **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, முழு);

bool sockaddr_equal(மாறிலி ஒன்றிப்பு sockaddr_union *a, மாறிலி ஒன்றிப்பு sockaddr_union *b);

முழு fd_set_sndbuf(முழு fd, size_t n, bool increase);
நிலையான உள்வரி முழு fd_inc_sndbuf(முழு fd, size_t n) {
        திரும்பு fd_set_sndbuf(fd, n, true);
}
முழு fd_set_rcvbuf(முழு fd, size_t n, bool increase);
நிலையான உள்வரி முழு fd_increase_rxbuf(முழு fd, size_t n) {
        திரும்பு fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, முழு);

வகைவரை எண்ணிக்கை {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(எழுத்து a) _const_;
bool ifname_valid_full(மாறிலி எழுத்து *p, IfnameValidFlags flags) _pure_;
நிலையான உள்வரி bool ifname_valid(மாறிலி எழுத்து *p) {
        திரும்பு ifname_valid_full(p, 0);
}
bool address_label_valid(மாறிலி எழுத்து *p) _pure_;

முழு getpeercred(முழு fd, கட்டமைப்பு ucred *ucred);
முழு getpeersec(முழு fd, எழுத்து **ret);
முழு getpeergroups(முழு fd, gid_t **ret);
முழு getpeerpidfd(முழு fd);
முழு getpeerpidref(முழு fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                முழு transport_fd,
                முழு fd,
                மாறிலி கட்டமைப்பு iovec *iov, size_t iovlen,
                மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t len,
                முழு flags);
முழு send_one_fd_sa(முழு transport_fd,
                   முழு fd,
                   மாறிலி கட்டமைப்பு sockaddr *sa, socklen_t len,
                   முழு flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, வெற்று, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, வெற்று, 0, வெற்று, 0, flags)
ssize_t receive_one_fd_iov(முழு transport_fd, கட்டமைப்பு iovec *iov, size_t iovlen, முழு flags, முழு *ret_fd);
முழு receive_one_fd(முழு transport_fd, முழு flags);

ssize_t next_datagram_size_fd(முழு fd);

முழு flush_accept(முழு fd);
ssize_t flush_mqueue(முழு fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        க்கு ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                கட்டமைப்பு cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(கட்டமைப்பு cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) வெற்று; \
        })

கட்டமைப்பு cmsghdr* cmsg_find(கட்டமைப்பு msghdr *mh, முழு level, முழு type, socklen_t length);
வெறுமை* cmsg_find_and_copy_data(கட்டமைப்பு msghdr *mh, முழு level, முழு type, வெறுமை *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(அளவு(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, அளவு(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        ஒன்றிப்பு {                                                         \
                கட்டமைப்பு cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(மாறிலி கட்டமைப்பு sockaddr_ll *sa);
size_t sockaddr_un_len(மாறிலி கட்டமைப்பு sockaddr_un *sa);
size_t sockaddr_len(மாறிலி ஒன்றிப்பு sockaddr_union *sa);

முழு socket_ioctl_fd(வெறுமை);

முழு sockaddr_un_set_path(கட்டமைப்பு sockaddr_un *ret, மாறிலி எழுத்து *path);

நிலையான உள்வரி முழு setsockopt_int(முழு fd, முழு level, முழு optname, முழு மதிப்பு) {
        எனில் (setsockopt(fd, level, optname, &மதிப்பு, அளவு(மதிப்பு)) < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

முழு getsockopt_int(முழு fd, முழு level, முழு optname, முழு *ret);

முழு socket_bind_to_ifname(முழு fd, மாறிலி எழுத்து *ifname);
முழு socket_bind_to_ifindex(முழு fd, முழு ifindex);

முழு socket_autobind(முழு fd, எழுத்து **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(அளவு(கட்டமைப்பு timeval)) + CMSG_SPACE(2 * அளவு(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(அளவு(கட்டமைப்பு timespec)) + CMSG_SPACE(2 * அளவு(uint64_t)))

ssize_t recvmsg_safe(முழு sockfd, கட்டமைப்பு msghdr *msg, முழு flags);

முழு socket_get_family(முழு fd);
முழு socket_set_recvpktinfo(முழு fd, முழு af, bool b);
முழு socket_set_unicast_if(முழு fd, முழு af, முழு ifi);

முழு socket_set_option(முழு fd, முழு af, முழு opt_ipv4, முழு opt_ipv6, முழு val);
நிலையான உள்வரி முழு socket_set_recverr(முழு fd, முழு af, bool b) {
        திரும்பு socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
நிலையான உள்வரி முழு socket_set_recvttl(முழு fd, முழு af, bool b) {
        திரும்பு socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
நிலையான உள்வரி முழு socket_set_ttl(முழு fd, முழு af, முழு ttl) {
        திரும்பு socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
நிலையான உள்வரி முழு socket_set_freebind(முழு fd, முழு af, bool b) {
        திரும்பு socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
நிலையான உள்வரி முழு socket_set_transparent(முழு fd, முழு af, bool b) {
        திரும்பு socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
நிலையான உள்வரி முழு socket_set_recvfragsize(முழு fd, முழு af, bool b) {
        திரும்பு socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

முழு socket_get_mtu(முழு fd, முழு af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

முழு connect_unix_path(முழு fd, முழு dir_fd, மாறிலி எழுத்து *path);

நிலையான உள்வரி bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        திரும்பு !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

முழு vsock_parse_port(மாறிலி எழுத்து *s, unsigned *ret);
முழு vsock_parse_cid(மாறிலி எழுத்து *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
முழு socket_address_parse_unix(SocketAddress *ret_address, மாறிலி எழுத்து *s);
முழு socket_address_parse_vsock(SocketAddress *ret_address, மாறிலி எழுத்து *s);
முழு socket_address_equal_unix(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

முழு netlink_socket_get_multicast_groups(முழு fd, size_t *ret_len, uint32_t **ret_groups);

முழு socket_get_cookie(முழு fd, uint64_t *ret);

வெறுமை cmsg_close_all(கட்டமைப்பு msghdr *mh);

முழு tos_to_priority(uint8_t tos);

முழு socket_xattr_supported(வெறுமை);
