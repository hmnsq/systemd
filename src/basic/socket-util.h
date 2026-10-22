/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <linux/if_packet.h>
#অন্তর্ভাব <linux/netlink.h>
#অন্তর্ভাব <linux/vm_sockets.h>
#অন্তর্ভাব <netinet/in.h>
#অন্তর্ভাব <sys/socket.h>
#অন্তর্ভাব <sys/un.h>

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "missing-network.h"

সংঘ sockaddr_union {
        /* The minimal, abstract version */
        কাঠামো sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        কাঠামো sockaddr_storage storage;

        /* Protocol-specific implementations */
        কাঠামো sockaddr_in in;
        কাঠামো sockaddr_in6 in6;
        কাঠামো sockaddr_un un;
        কাঠামো sockaddr_nl nl;
        কাঠামো sockaddr_ll ll;
        কাঠামো sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_বাফার[offsetof(কাঠামো sockaddr_ll, sll_addr) + HW_ADDR_MAX_আকার];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_বাফার[আকার(কাঠামো sockaddr_un) + 1];
};

#define SUN_PATH_LEN (আকার(((কাঠামো sockaddr_un){}).sun_path))

প্রকারলেখ কাঠামো SocketAddress {
        সংঘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t আকার_2;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        পূর্ণসংখ্যা type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        পূর্ণসংখ্যা protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, পূর্ণসংখ্যা);

পূর্ণসংখ্যা sockaddr_un_unlink(ধ্রুবক কাঠামো sockaddr_un *sa);

স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_address_unlink(ধ্রুবক SocketAddress *a) {
        ফেরত socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ধ্রুবক SocketAddress *a) _pure_;

পূর্ণসংখ্যা socket_address_verify(ধ্রুবক SocketAddress *a, bool strict) _pure_;
পূর্ণসংখ্যা socket_address_print(ধ্রুবক SocketAddress *a, অক্ষর **ret);
bool socket_address_matches_fd(ধ্রুবক SocketAddress *a, পূর্ণসংখ্যা fd);

bool socket_address_equal(ধ্রুবক SocketAddress *a, ধ্রুবক SocketAddress *b) _pure_;

ধ্রুবক অক্ষর* socket_address_get_path(ধ্রুবক SocketAddress *a);

bool socket_ipv6_is_supported(শূন্য);
bool socket_ipv6_is_enabled(শূন্য);

পূর্ণসংখ্যা sockaddr_port(ধ্রুবক কাঠামো sockaddr *_sa, unsigned *port);
ধ্রুবক সংঘ in_addr_union *sockaddr_in_addr(ধ্রুবক কাঠামো sockaddr *sa);
পূর্ণসংখ্যা sockaddr_set_in_addr(সংঘ sockaddr_union *u, পূর্ণসংখ্যা family, ধ্রুবক সংঘ in_addr_union *a, uint16_t port);

পূর্ণসংখ্যা sockaddr_pretty(ধ্রুবক কাঠামো sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, অক্ষর **ret);
পূর্ণসংখ্যা getpeername_pretty(পূর্ণসংখ্যা fd, bool include_port, অক্ষর **ret);
পূর্ণসংখ্যা getsockname_pretty(পূর্ণসংখ্যা fd, অক্ষর **ret);

পূর্ণসংখ্যা socknameinfo_pretty(ধ্রুবক কাঠামো sockaddr *sa, socklen_t salen, অক্ষর **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, পূর্ণসংখ্যা);

bool sockaddr_equal(ধ্রুবক সংঘ sockaddr_union *a, ধ্রুবক সংঘ sockaddr_union *b);

পূর্ণসংখ্যা fd_set_sndbuf(পূর্ণসংখ্যা fd, size_t n, bool increase);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_inc_sndbuf(পূর্ণসংখ্যা fd, size_t n) {
        ফেরত fd_set_sndbuf(fd, n, true);
}
পূর্ণসংখ্যা fd_set_rcvbuf(পূর্ণসংখ্যা fd, size_t n, bool increase);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_increase_rxbuf(পূর্ণসংখ্যা fd, size_t n) {
        ফেরত fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, পূর্ণসংখ্যা);

প্রকারলেখ গণনা {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(অক্ষর a) _const_;
bool ifname_valid_full(ধ্রুবক অক্ষর *p, IfnameValidFlags flags) _pure_;
স্থির অন্তঃরেখ bool ifname_valid(ধ্রুবক অক্ষর *p) {
        ফেরত ifname_valid_full(p, 0);
}
bool address_label_valid(ধ্রুবক অক্ষর *p) _pure_;

পূর্ণসংখ্যা getpeercred(পূর্ণসংখ্যা fd, কাঠামো ucred *ucred);
পূর্ণসংখ্যা getpeersec(পূর্ণসংখ্যা fd, অক্ষর **ret);
পূর্ণসংখ্যা getpeergroups(পূর্ণসংখ্যা fd, gid_t **ret);
পূর্ণসংখ্যা getpeerpidfd(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা getpeerpidref(পূর্ণসংখ্যা fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                পূর্ণসংখ্যা transport_fd,
                পূর্ণসংখ্যা fd,
                ধ্রুবক কাঠামো iovec *iov, size_t iovlen,
                ধ্রুবক কাঠামো sockaddr *sa, socklen_t len,
                পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা send_one_fd_sa(পূর্ণসংখ্যা transport_fd,
                   পূর্ণসংখ্যা fd,
                   ধ্রুবক কাঠামো sockaddr *sa, socklen_t len,
                   পূর্ণসংখ্যা flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ফাঁকা, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ফাঁকা, 0, ফাঁকা, 0, flags)
ssize_t receive_one_fd_iov(পূর্ণসংখ্যা transport_fd, কাঠামো iovec *iov, size_t iovlen, পূর্ণসংখ্যা flags, পূর্ণসংখ্যা *ret_fd);
পূর্ণসংখ্যা receive_one_fd(পূর্ণসংখ্যা transport_fd, পূর্ণসংখ্যা flags);

ssize_t পরবর্তী_datagram_আকার_fd(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা flush_accept(পূর্ণসংখ্যা fd);
ssize_t flush_mqueue(পূর্ণসংখ্যা fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        জন্য ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_তথ্য(cmsg, type)                                     \
        ({                                                              \
                কাঠামো cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(কাঠামো cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ফাঁকা; \
        })

কাঠামো cmsghdr* cmsg_find(কাঠামো msghdr *mh, পূর্ণসংখ্যা level, পূর্ণসংখ্যা type, socklen_t length);
শূন্য* cmsg_find_and_copy_তথ্য(কাঠামো msghdr *mh, পূর্ণসংখ্যা level, পূর্ণসংখ্যা type, শূন্য *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_তথ্য(mh, level, type, ctype)                          \
        CMSG_TYPED_তথ্য(cmsg_find(mh, level, type, CMSG_LEN(আকার(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_তথ্য(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_তথ্য(mh, level, type, &(ctype){}, আকার(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_বাফার_TYPE(আকার_2)                                          \
        সংঘ {                                                         \
                কাঠামো cmsghdr cmsghdr;                                 \
                uint8_t buf[আকার_2];                                      \
                uint8_t align_check[(আকার_2) >= CMSG_SPACE(0) &&          \
                                    (আকার_2) == CMSG_ALIGN(আকার_2) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ধ্রুবক কাঠামো sockaddr_ll *sa);
size_t sockaddr_un_len(ধ্রুবক কাঠামো sockaddr_un *sa);
size_t sockaddr_len(ধ্রুবক সংঘ sockaddr_union *sa);

পূর্ণসংখ্যা socket_ioctl_fd(শূন্য);

পূর্ণসংখ্যা sockaddr_un_set_path(কাঠামো sockaddr_un *ret, ধ্রুবক অক্ষর *path);

স্থির অন্তঃরেখ পূর্ণসংখ্যা setsockopt_int(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা level, পূর্ণসংখ্যা optname, পূর্ণসংখ্যা মান) {
        যদি (setsockopt(fd, level, optname, &মান, আকার(মান)) < 0)
                ফেরত -errno;

        ফেরত 0;
}

পূর্ণসংখ্যা getsockopt_int(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা level, পূর্ণসংখ্যা optname, পূর্ণসংখ্যা *ret);

পূর্ণসংখ্যা socket_bind_to_ifname(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *ifname);
পূর্ণসংখ্যা socket_bind_to_ifindex(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা ifindex);

পূর্ণসংখ্যা socket_autobind(পূর্ণসংখ্যা fd, অক্ষর **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(আকার(কাঠামো timeval)) + CMSG_SPACE(2 * আকার(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(আকার(কাঠামো timespec)) + CMSG_SPACE(2 * আকার(uint64_t)))

ssize_t recvmsg_safe(পূর্ণসংখ্যা sockfd, কাঠামো msghdr *msg, পূর্ণসংখ্যা flags);

পূর্ণসংখ্যা socket_get_family(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা socket_set_recvpktinfo(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b);
পূর্ণসংখ্যা socket_set_unicast_if(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, পূর্ণসংখ্যা ifi);

পূর্ণসংখ্যা socket_set_option(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, পূর্ণসংখ্যা opt_ipv4, পূর্ণসংখ্যা opt_ipv6, পূর্ণসংখ্যা val);
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_recverr(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {
        ফেরত socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_recvttl(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {
        ফেরত socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_ttl(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, পূর্ণসংখ্যা ttl) {
        ফেরত socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_freebind(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {
        ফেরত socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_transparent(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {
        ফেরত socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা socket_set_recvfragsize(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, bool b) {
        ফেরত socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

পূর্ণসংখ্যা socket_get_mtu(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

পূর্ণসংখ্যা connect_unix_path(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path);

স্থির অন্তঃরেখ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ফেরত !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

পূর্ণসংখ্যা vsock_parse_port(ধ্রুবক অক্ষর *s, unsigned *ret);
পূর্ণসংখ্যা vsock_parse_cid(ধ্রুবক অক্ষর *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
পূর্ণসংখ্যা socket_address_parse_unix(SocketAddress *ret_address, ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা socket_address_parse_vsock(SocketAddress *ret_address, ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা socket_address_equal_unix(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

পূর্ণসংখ্যা netlink_socket_get_multicast_groups(পূর্ণসংখ্যা fd, size_t *ret_len, uint32_t **ret_groups);

পূর্ণসংখ্যা socket_get_cookie(পূর্ণসংখ্যা fd, uint64_t *ret);

শূন্য cmsg_close_all(কাঠামো msghdr *mh);

পূর্ণসংখ্যা tos_to_priority(uint8_t tos);

পূর্ণসংখ্যা socket_xattr_supported(শূন্য);
