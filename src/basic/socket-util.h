/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <linux/if_packet.h>
#ಅನ್ತರ್ಭಾವ <linux/netlink.h>
#ಅನ್ತರ್ಭಾವ <linux/vm_sockets.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಅನ್ತರ್ಭಾವ <sys/un.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "missing-network.h"

ಸಙ್ಘ sockaddr_union {
        /* The minimal, abstract version */
        ಸಂರಚನಾ sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ಸಂರಚನಾ sockaddr_storage storage;

        /* Protocol-specific implementations */
        ಸಂರಚನಾ sockaddr_in in;
        ಸಂರಚನಾ sockaddr_in6 in6;
        ಸಂರಚನಾ sockaddr_un un;
        ಸಂರಚನಾ sockaddr_nl nl;
        ಸಂರಚನಾ sockaddr_ll ll;
        ಸಂರಚನಾ sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(ಸಂರಚನಾ sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[ಪರಿಮಾಣ(ಸಂರಚನಾ sockaddr_un) + 1];
};

#define SUN_PATH_LEN (ಪರಿಮಾಣ(((ಸಂರಚನಾ sockaddr_un){}).sun_path))

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ SocketAddress {
        ಸಙ್ಘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        ಪೂರ್ಣಾಙ್ಕ type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        ಪೂರ್ಣಾಙ್ಕ protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, ಪೂರ್ಣಾಙ್ಕ);

ಪೂರ್ಣಾಙ್ಕ sockaddr_un_unlink(ಧ್ರುವ ಸಂರಚನಾ sockaddr_un *sa);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_address_unlink(ಧ್ರುವ SocketAddress *a) {
        ಪ್ರತ್ಯಾಗಮ socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ಧ್ರುವ SocketAddress *a) _pure_;

ಪೂರ್ಣಾಙ್ಕ socket_address_verify(ಧ್ರುವ SocketAddress *a, bool strict) _pure_;
ಪೂರ್ಣಾಙ್ಕ socket_address_print(ಧ್ರುವ SocketAddress *a, ವರ್ಣ **ret);
bool socket_address_matches_fd(ಧ್ರುವ SocketAddress *a, ಪೂರ್ಣಾಙ್ಕ fd);

bool socket_address_equal(ಧ್ರುವ SocketAddress *a, ಧ್ರುವ SocketAddress *b) _pure_;

ಧ್ರುವ ವರ್ಣ* socket_address_get_path(ಧ್ರುವ SocketAddress *a);

bool socket_ipv6_is_supported(ಶೂನ್ಯ);
bool socket_ipv6_is_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ sockaddr_port(ಧ್ರುವ ಸಂರಚನಾ sockaddr *_sa, unsigned *port);
ಧ್ರುವ ಸಙ್ಘ in_addr_union *sockaddr_in_addr(ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa);
ಪೂರ್ಣಾಙ್ಕ sockaddr_set_in_addr(ಸಙ್ಘ sockaddr_union *u, ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ಸಙ್ಘ in_addr_union *a, uint16_t port);

ಪೂರ್ಣಾಙ್ಕ sockaddr_pretty(ಧ್ರುವ ಸಂರಚನಾ sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ getpeername_pretty(ಪೂರ್ಣಾಙ್ಕ fd, bool include_port, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ getsockname_pretty(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ socknameinfo_pretty(ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t salen, ವರ್ಣ **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ಪೂರ್ಣಾಙ್ಕ);

bool sockaddr_equal(ಧ್ರುವ ಸಙ್ಘ sockaddr_union *a, ಧ್ರುವ ಸಙ್ಘ sockaddr_union *b);

ಪೂರ್ಣಾಙ್ಕ fd_set_sndbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n, bool increase);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_inc_sndbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n) {
        ಪ್ರತ್ಯಾಗಮ fd_set_sndbuf(fd, n, true);
}
ಪೂರ್ಣಾಙ್ಕ fd_set_rcvbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n, bool increase);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_increase_rxbuf(ಪೂರ್ಣಾಙ್ಕ fd, size_t n) {
        ಪ್ರತ್ಯಾಗಮ fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ಪೂರ್ಣಾಙ್ಕ);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(ವರ್ಣ a) _const_;
bool ifname_valid_full(ಧ್ರುವ ವರ್ಣ *p, IfnameValidFlags flags) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool ifname_valid(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ ifname_valid_full(p, 0);
}
bool address_label_valid(ಧ್ರುವ ವರ್ಣ *p) _pure_;

ಪೂರ್ಣಾಙ್ಕ getpeercred(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ ucred *ucred);
ಪೂರ್ಣಾಙ್ಕ getpeersec(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ getpeergroups(ಪೂರ್ಣಾಙ್ಕ fd, gid_t **ret);
ಪೂರ್ಣಾಙ್ಕ getpeerpidfd(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ getpeerpidref(ಪೂರ್ಣಾಙ್ಕ fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                ಪೂರ್ಣಾಙ್ಕ transport_fd,
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ಸಂರಚನಾ iovec *iov, size_t iovlen,
                ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಙ್ಕ flags);
ಪೂರ್ಣಾಙ್ಕ send_one_fd_sa(ಪೂರ್ಣಾಙ್ಕ transport_fd,
                   ಪೂರ್ಣಾಙ್ಕ fd,
                   ಧ್ರುವ ಸಂರಚನಾ sockaddr *sa, socklen_t len,
                   ಪೂರ್ಣಾಙ್ಕ flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ರಿಕ್ತ, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ರಿಕ್ತ, 0, ರಿಕ್ತ, 0, flags)
ssize_t receive_one_fd_iov(ಪೂರ್ಣಾಙ್ಕ transport_fd, ಸಂರಚನಾ iovec *iov, size_t iovlen, ಪೂರ್ಣಾಙ್ಕ flags, ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಪೂರ್ಣಾಙ್ಕ receive_one_fd(ಪೂರ್ಣಾಙ್ಕ transport_fd, ಪೂರ್ಣಾಙ್ಕ flags);

ssize_t next_datagram_size_fd(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ flush_accept(ಪೂರ್ಣಾಙ್ಕ fd);
ssize_t flush_mqueue(ಪೂರ್ಣಾಙ್ಕ fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        ಪ್ರತಿ ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                ಸಂರಚನಾ cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ಸಂರಚನಾ cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ರಿಕ್ತ; \
        })

ಸಂರಚನಾ cmsghdr* cmsg_find(ಸಂರಚನಾ msghdr *mh, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ type, socklen_t length);
ಶೂನ್ಯ* cmsg_find_and_copy_data(ಸಂರಚನಾ msghdr *mh, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ type, ಶೂನ್ಯ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(ಪರಿಮಾಣ(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, ಪರಿಮಾಣ(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        ಸಙ್ಘ {                                                         \
                ಸಂರಚನಾ cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ಧ್ರುವ ಸಂರಚನಾ sockaddr_ll *sa);
size_t sockaddr_un_len(ಧ್ರುವ ಸಂರಚನಾ sockaddr_un *sa);
size_t sockaddr_len(ಧ್ರುವ ಸಙ್ಘ sockaddr_union *sa);

ಪೂರ್ಣಾಙ್ಕ socket_ioctl_fd(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ sockaddr_un_set_path(ಸಂರಚನಾ sockaddr_un *ret, ಧ್ರುವ ವರ್ಣ *path);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ setsockopt_int(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ optname, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) {
        ಯದಿ (setsockopt(fd, level, optname, &ಮೌಲ್ಯ, ಪರಿಮಾಣ(ಮೌಲ್ಯ)) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getsockopt_int(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ level, ಪೂರ್ಣಾಙ್ಕ optname, ಪೂರ್ಣಾಙ್ಕ *ret);

ಪೂರ್ಣಾಙ್ಕ socket_bind_to_ifname(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *ifname);
ಪೂರ್ಣಾಙ್ಕ socket_bind_to_ifindex(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ ifindex);

ಪೂರ್ಣಾಙ್ಕ socket_autobind(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(ಪರಿಮಾಣ(ಸಂರಚನಾ timeval)) + CMSG_SPACE(2 * ಪರಿಮಾಣ(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(ಪರಿಮಾಣ(ಸಂರಚನಾ timespec)) + CMSG_SPACE(2 * ಪರಿಮಾಣ(uint64_t)))

ssize_t recvmsg_safe(ಪೂರ್ಣಾಙ್ಕ sockfd, ಸಂರಚನಾ msghdr *msg, ಪೂರ್ಣಾಙ್ಕ flags);

ಪೂರ್ಣಾಙ್ಕ socket_get_family(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ socket_set_recvpktinfo(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b);
ಪೂರ್ಣಾಙ್ಕ socket_set_unicast_if(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, ಪೂರ್ಣಾಙ್ಕ ifi);

ಪೂರ್ಣಾಙ್ಕ socket_set_option(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, ಪೂರ್ಣಾಙ್ಕ opt_ipv4, ಪೂರ್ಣಾಙ್ಕ opt_ipv6, ಪೂರ್ಣಾಙ್ಕ val);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_recverr(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_recvttl(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_ttl(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, ಪೂರ್ಣಾಙ್ಕ ttl) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_freebind(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_transparent(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ socket_set_recvfragsize(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, bool b) {
        ಪ್ರತ್ಯಾಗಮ socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

ಪೂರ್ಣಾಙ್ಕ socket_get_mtu(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

ಪೂರ್ಣಾಙ್ಕ connect_unix_path(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ಪ್ರತ್ಯಾಗಮ !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

ಪೂರ್ಣಾಙ್ಕ vsock_parse_port(ಧ್ರುವ ವರ್ಣ *s, unsigned *ret);
ಪೂರ್ಣಾಙ್ಕ vsock_parse_cid(ಧ್ರುವ ವರ್ಣ *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
ಪೂರ್ಣಾಙ್ಕ socket_address_parse_unix(SocketAddress *ret_address, ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ socket_address_parse_vsock(SocketAddress *ret_address, ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ socket_address_equal_unix(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

ಪೂರ್ಣಾಙ್ಕ netlink_socket_get_multicast_groups(ಪೂರ್ಣಾಙ್ಕ fd, size_t *ret_len, uint32_t **ret_groups);

ಪೂರ್ಣಾಙ್ಕ socket_get_cookie(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);

ಶೂನ್ಯ cmsg_close_all(ಸಂರಚನಾ msghdr *mh);

ಪೂರ್ಣಾಙ್ಕ tos_to_priority(uint8_t tos);

ಪೂರ್ಣಾಙ್ಕ socket_xattr_supported(ಶೂನ್ಯ);
