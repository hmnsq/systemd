/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <linux/if_packet.h>
#ಅನ್ತರ್ಭಾವ <linux/netlink.h>
#ಅನ್ತರ್ಭಾವ <linux/vm_sockets.h>
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>
#ಅನ್ತರ್ಭಾವ <sys/un.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "missing-network.h"

ಒಕ್ಕೂಟ sockaddr_union {
        /* The minimal, abstract version */
        ರಚನೆ sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ರಚನೆ sockaddr_storage storage;

        /* Protocol-specific implementations */
        ರಚನೆ sockaddr_in ಒಳ;
        ರಚನೆ sockaddr_in6 in6;
        ರಚನೆ sockaddr_un un;
        ರಚನೆ sockaddr_nl nl;
        ರಚನೆ sockaddr_ll ll;
        ರಚನೆ sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_ಬಫರ್[offsetof(ರಚನೆ sockaddr_ll, sll_addr) + HW_ADDR_MAX_ಗಾತ್ರ];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_ಬಫರ್[ಗಾತ್ರ(ರಚನೆ sockaddr_un) + 1];
};

#ವ್ಯಾಖ್ಯಾನಿಸು SUN_ಮಾರ್ಗ_LEN (ಗಾತ್ರ(((ರಚನೆ sockaddr_un){}).sun_path))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Socket_ವಿಳಾಸ {
        ಒಕ್ಕೂಟ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t ಗಾತ್ರ_2;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        ಪೂರ್ಣಾಂಕ type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        ಪೂರ್ಣಾಂಕ protocol;
} Socket_ವಿಳಾಸ;

#ವ್ಯಾಖ್ಯಾನಿಸು socket_ವಿಳಾಸ_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(socket_ವಿಳಾಸ_type, ಪೂರ್ಣಾಂಕ);

ಪೂರ್ಣಾಂಕ sockaddr_un_unlink(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_un *sa);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_unlink(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a) {
        ಹಿಂತಿರುಗಿ socket_ವಿಳಾಸ_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_ವಿಳಾಸ_can_accept(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_verify(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, bool strict) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_print(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಅಕ್ಷರ **ret);
bool socket_ವಿಳಾಸ_matches_fd(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಪೂರ್ಣಾಂಕ fd);

bool socket_ವಿಳಾಸ_equal(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a, ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *b) ಶುದ್ಧ;

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* socket_ವಿಳಾಸ_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ Socket_ವಿಳಾಸ *a);

bool socket_ipv6_is_supported(ಶೂನ್ಯ);
bool socket_ipv6_is_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ sockaddr_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *_sa, ಚಿಹ್ನೆರಹಿತ *port);
ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *sockaddr_ಒಳ_addr(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa);
ಪೂರ್ಣಾಂಕ sockaddr_ಗಣ_ಒಳ_addr(ಒಕ್ಕೂಟ sockaddr_union *u, ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ ಒಳ_addr_union *a, uint16_t port);

ಪೂರ್ಣಾಂಕ sockaddr_pretty(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_ಪೋರ್ಟ್, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ getpeername_pretty(ಪೂರ್ಣಾಂಕ fd, bool include_ಪೋರ್ಟ್, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ getsockname_pretty(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ socknameinfo_pretty(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t salen, ಅಕ್ಷರ **_ret);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(netlink_family, ಪೂರ್ಣಾಂಕ);

bool sockaddr_equal(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *a, ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *b);

ಪೂರ್ಣಾಂಕ fd_ಗಣ_sndbuf(ಪೂರ್ಣಾಂಕ fd, size_t n, bool increase);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_inc_sndbuf(ಪೂರ್ಣಾಂಕ fd, size_t n) {
        ಹಿಂತಿರುಗಿ fd_ಗಣ_sndbuf(fd, n, true);
}
ಪೂರ್ಣಾಂಕ fd_ಗಣ_rcvbuf(ಪೂರ್ಣಾಂಕ fd, size_t n, bool increase);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_increase_rxbuf(ಪೂರ್ಣಾಂಕ fd, size_t n) {
        ಹಿಂತಿರುಗಿ fd_ಗಣ_rcvbuf(fd, n, true);
}

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(ip_tos, ಪೂರ್ಣಾಂಕ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(ಅಕ್ಷರ a) _const_;
bool ifname_valid_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, IfnameValidFlags flags) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ifname_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ ifname_valid_ತುಂಬಿದ(p, 0);
}
bool ವಿಳಾಸ_label_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ getpeercred(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ ucred *ucred);
ಪೂರ್ಣಾಂಕ getpeersec(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ getpeergroups(ಪೂರ್ಣಾಂಕ fd, gid_t **ret);
ಪೂರ್ಣಾಂಕ getpeerpidfd(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ getpeerpidref(ಪೂರ್ಣಾಂಕ fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                ಪೂರ್ಣಾಂಕ transport_fd,
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, size_t iovlen,
                ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t len,
                ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ send_one_fd_sa(ಪೂರ್ಣಾಂಕ transport_fd,
                   ಪೂರ್ಣಾಂಕ fd,
                   ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *sa, socklen_t len,
                   ಪೂರ್ಣಾಂಕ flags);
#ವ್ಯಾಖ್ಯಾನಿಸು send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ಖಾಲಿ, 0, flags)
#ವ್ಯಾಖ್ಯಾನಿಸು send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ಖಾಲಿ, 0, ಖಾಲಿ, 0, flags)
ssize_t receive_one_fd_iov(ಪೂರ್ಣಾಂಕ transport_fd, ರಚನೆ iovec *iov, size_t iovlen, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ *ret_fd);
ಪೂರ್ಣಾಂಕ receive_one_fd(ಪೂರ್ಣಾಂಕ transport_fd, ಪೂರ್ಣಾಂಕ flags);

ssize_t ಮುಂದಿನ_datagram_ಗಾತ್ರ_fd(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ flush_accept(ಪೂರ್ಣಾಂಕ fd);
ssize_t flush_mqueue(ಪೂರ್ಣಾಂಕ fd);

#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_FOREACH(cmsg, mh)                                          \
        ಗಾಗಿ ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_TYPED_ದತ್ತಾಂಶ(cmsg, type)                                     \
        ({                                                              \
                ರಚನೆ cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ರಚನೆ cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ಖಾಲಿ; \
        })

ರಚನೆ cmsghdr* cmsg_ಕಂಡುಹಿಡಿ(ರಚನೆ msghdr *mh, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ type, socklen_t length);
ಶೂನ್ಯ* cmsg_ಕಂಡುಹಿಡಿ_and_copy_ದತ್ತಾಂಶ(ರಚನೆ msghdr *mh, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ type, ಶೂನ್ಯ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಕಂಡುಹಿಡಿ_ದತ್ತಾಂಶ(mh, level, type, ctype)                          \
        CMSG_TYPED_ದತ್ತಾಂಶ(cmsg_ಕಂಡುಹಿಡಿ(mh, level, type, CMSG_LEN(ಗಾತ್ರ(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಕಂಡುಹಿಡಿ_AND_COPY_ದತ್ತಾಂಶ(mh, level, type, ctype)             \
        (ctype*) cmsg_ಕಂಡುಹಿಡಿ_and_copy_ದತ್ತಾಂಶ(mh, level, type, &(ctype){}, ಗಾತ್ರ(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_ಬಫರ್_TYPE(ಗಾತ್ರ_2)                                          \
        ಒಕ್ಕೂಟ {                                                         \
                ರಚನೆ cmsghdr cmsghdr;                                 \
                uint8_t buf[ಗಾತ್ರ_2];                                      \
                uint8_t align_check[(ಗಾತ್ರ_2) >= CMSG_SPACE(0) &&          \
                                    (ಗಾತ್ರ_2) == CMSG_ALIGN(ಗಾತ್ರ_2) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_ll *sa);
size_t sockaddr_un_len(ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr_un *sa);
size_t sockaddr_len(ಸ್ಥಿರಾಂಕ ಒಕ್ಕೂಟ sockaddr_union *sa);

ಪೂರ್ಣಾಂಕ socket_ioctl_fd(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ sockaddr_un_ಗಣ_ಮಾರ್ಗ(ರಚನೆ sockaddr_un *ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ setsockopt_int(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ optname, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) {
        ಆದರೆ (setsockopt(fd, level, optname, &ಮೌಲ್ಯ, ಗಾತ್ರ(ಮೌಲ್ಯ)) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getsockopt_int(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ level, ಪೂರ್ಣಾಂಕ optname, ಪೂರ್ಣಾಂಕ *ret);

ಪೂರ್ಣಾಂಕ socket_bind_ಗಮ್ಯ_ifname(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname);
ಪೂರ್ಣಾಂಕ socket_bind_ಗಮ್ಯ_ifindex(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ifindex);

ಪೂರ್ಣಾಂಕ socket_autobind(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(ಗಾತ್ರ(ರಚನೆ timeval)) + CMSG_SPACE(2 * ಗಾತ್ರ(uint64_t)))
#ವ್ಯಾಖ್ಯಾನಿಸು CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(ಗಾತ್ರ(ರಚನೆ timespec)) + CMSG_SPACE(2 * ಗಾತ್ರ(uint64_t)))

ssize_t recvmsg_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ sockfd, ರಚನೆ msghdr *msg, ಪೂರ್ಣಾಂಕ flags);

ಪೂರ್ಣಾಂಕ socket_get_family(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ socket_ಗಣ_recvpktinfo(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b);
ಪೂರ್ಣಾಂಕ socket_ಗಣ_unicast_if(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, ಪೂರ್ಣಾಂಕ ifi);

ಪೂರ್ಣಾಂಕ socket_ಗಣ_option(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, ಪೂರ್ಣಾಂಕ opt_ipv4, ಪೂರ್ಣಾಂಕ opt_ipv6, ಪೂರ್ಣಾಂಕ val);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_recverr(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_recvttl(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_ttl(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, ಪೂರ್ಣಾಂಕ ttl) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_freebind(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_transparent(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ socket_ಗಣ_recvfragsize(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, bool b) {
        ಹಿಂತಿರುಗಿ socket_ಗಣ_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

ಪೂರ್ಣಾಂಕ socket_get_mtu(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#ವ್ಯಾಖ್ಯಾನಿಸು UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

ಪೂರ್ಣಾಂಕ connect_unix_ಮಾರ್ಗ(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool VSOCK_CID_IS_REGULAR(ಚಿಹ್ನೆರಹಿತ cid) {
        /* Refuse special CID values */
        ಹಿಂತಿರುಗಿ !ಒಳ_ಗಣ(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

ಪೂರ್ಣಾಂಕ vsock_parse_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ *ret);
ಪೂರ್ಣಾಂಕ vsock_parse_cid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_parse_unix(Socket_ವಿಳಾಸ *ret_ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_parse_vsock(Socket_ವಿಳಾಸ *ret_ವಿಳಾಸ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ socket_ವಿಳಾಸ_equal_unix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#ವ್ಯಾಖ್ಯಾನಿಸು SOMAXCONN_DELUXE ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ

ಪೂರ್ಣಾಂಕ netlink_socket_get_multicast_groups(ಪೂರ್ಣಾಂಕ fd, size_t *ret_len, uint32_t **ret_groups);

ಪೂರ್ಣಾಂಕ socket_get_cookie(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);

ಶೂನ್ಯ cmsg_close_all(ರಚನೆ msghdr *mh);

ಪೂರ್ಣಾಂಕ tos_ಗಮ್ಯ_ಆದ್ಯತೆ(uint8_t tos);

ಪೂರ್ಣಾಂಕ socket_xattr_supported(ಶೂನ್ಯ);
