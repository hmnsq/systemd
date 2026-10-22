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

সঙ্ঘ sockaddr_union {
        /* The minimal, abstract version */
        সংরচনা sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        সংরচনা sockaddr_storage storage;

        /* Protocol-specific implementations */
        সংরচনা sockaddr_in in;
        সংরচনা sockaddr_in6 in6;
        সংরচনা sockaddr_un un;
        সংরচনা sockaddr_nl nl;
        সংরচনা sockaddr_ll ll;
        সংরচনা sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(সংরচনা sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[পরিমাণ(সংরচনা sockaddr_un) + 1];
};

#define SUN_PATH_LEN (পরিমাণ(((সংরচনা sockaddr_un){}).sun_path))

প্রকারনির্দেশ সংরচনা SocketAddress {
        সঙ্ঘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        পূর্ণাঙ্ক type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        পূর্ণাঙ্ক protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, পূর্ণাঙ্ক);

পূর্ণাঙ্ক sockaddr_un_unlink(ধ্রুব সংরচনা sockaddr_un *sa);

স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_address_unlink(ধ্রুব SocketAddress *a) {
        প্রত্যাগম socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ধ্রুব SocketAddress *a) _pure_;

পূর্ণাঙ্ক socket_address_verify(ধ্রুব SocketAddress *a, bool strict) _pure_;
পূর্ণাঙ্ক socket_address_print(ধ্রুব SocketAddress *a, বর্ণ **ret);
bool socket_address_matches_fd(ধ্রুব SocketAddress *a, পূর্ণাঙ্ক fd);

bool socket_address_equal(ধ্রুব SocketAddress *a, ধ্রুব SocketAddress *b) _pure_;

ধ্রুব বর্ণ* socket_address_get_path(ধ্রুব SocketAddress *a);

bool socket_ipv6_is_supported(শূন্য);
bool socket_ipv6_is_enabled(শূন্য);

পূর্ণাঙ্ক sockaddr_port(ধ্রুব সংরচনা sockaddr *_sa, unsigned *port);
ধ্রুব সঙ্ঘ in_addr_union *sockaddr_in_addr(ধ্রুব সংরচনা sockaddr *sa);
পূর্ণাঙ্ক sockaddr_set_in_addr(সঙ্ঘ sockaddr_union *u, পূর্ণাঙ্ক family, ধ্রুব সঙ্ঘ in_addr_union *a, uint16_t port);

পূর্ণাঙ্ক sockaddr_pretty(ধ্রুব সংরচনা sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, বর্ণ **ret);
পূর্ণাঙ্ক getpeername_pretty(পূর্ণাঙ্ক fd, bool include_port, বর্ণ **ret);
পূর্ণাঙ্ক getsockname_pretty(পূর্ণাঙ্ক fd, বর্ণ **ret);

পূর্ণাঙ্ক socknameinfo_pretty(ধ্রুব সংরচনা sockaddr *sa, socklen_t salen, বর্ণ **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, পূর্ণাঙ্ক);

bool sockaddr_equal(ধ্রুব সঙ্ঘ sockaddr_union *a, ধ্রুব সঙ্ঘ sockaddr_union *b);

পূর্ণাঙ্ক fd_set_sndbuf(পূর্ণাঙ্ক fd, size_t n, bool increase);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_inc_sndbuf(পূর্ণাঙ্ক fd, size_t n) {
        প্রত্যাগম fd_set_sndbuf(fd, n, true);
}
পূর্ণাঙ্ক fd_set_rcvbuf(পূর্ণাঙ্ক fd, size_t n, bool increase);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_increase_rxbuf(পূর্ণাঙ্ক fd, size_t n) {
        প্রত্যাগম fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, পূর্ণাঙ্ক);

প্রকারনির্দেশ গণনা {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(বর্ণ a) _const_;
bool ifname_valid_full(ধ্রুব বর্ণ *p, IfnameValidFlags flags) _pure_;
স্থির অন্তঃস্থ bool ifname_valid(ধ্রুব বর্ণ *p) {
        প্রত্যাগম ifname_valid_full(p, 0);
}
bool address_label_valid(ধ্রুব বর্ণ *p) _pure_;

পূর্ণাঙ্ক getpeercred(পূর্ণাঙ্ক fd, সংরচনা ucred *ucred);
পূর্ণাঙ্ক getpeersec(পূর্ণাঙ্ক fd, বর্ণ **ret);
পূর্ণাঙ্ক getpeergroups(পূর্ণাঙ্ক fd, gid_t **ret);
পূর্ণাঙ্ক getpeerpidfd(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক getpeerpidref(পূর্ণাঙ্ক fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                পূর্ণাঙ্ক transport_fd,
                পূর্ণাঙ্ক fd,
                ধ্রুব সংরচনা iovec *iov, size_t iovlen,
                ধ্রুব সংরচনা sockaddr *sa, socklen_t len,
                পূর্ণাঙ্ক flags);
পূর্ণাঙ্ক send_one_fd_sa(পূর্ণাঙ্ক transport_fd,
                   পূর্ণাঙ্ক fd,
                   ধ্রুব সংরচনা sockaddr *sa, socklen_t len,
                   পূর্ণাঙ্ক flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, রিক্ত, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, রিক্ত, 0, রিক্ত, 0, flags)
ssize_t receive_one_fd_iov(পূর্ণাঙ্ক transport_fd, সংরচনা iovec *iov, size_t iovlen, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক *ret_fd);
পূর্ণাঙ্ক receive_one_fd(পূর্ণাঙ্ক transport_fd, পূর্ণাঙ্ক flags);

ssize_t next_datagram_size_fd(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক flush_accept(পূর্ণাঙ্ক fd);
ssize_t flush_mqueue(পূর্ণাঙ্ক fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        প্রতি ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                সংরচনা cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(সংরচনা cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) রিক্ত; \
        })

সংরচনা cmsghdr* cmsg_find(সংরচনা msghdr *mh, পূর্ণাঙ্ক level, পূর্ণাঙ্ক type, socklen_t length);
শূন্য* cmsg_find_and_copy_data(সংরচনা msghdr *mh, পূর্ণাঙ্ক level, পূর্ণাঙ্ক type, শূন্য *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(পরিমাণ(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, পরিমাণ(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        সঙ্ঘ {                                                         \
                সংরচনা cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ধ্রুব সংরচনা sockaddr_ll *sa);
size_t sockaddr_un_len(ধ্রুব সংরচনা sockaddr_un *sa);
size_t sockaddr_len(ধ্রুব সঙ্ঘ sockaddr_union *sa);

পূর্ণাঙ্ক socket_ioctl_fd(শূন্য);

পূর্ণাঙ্ক sockaddr_un_set_path(সংরচনা sockaddr_un *ret, ধ্রুব বর্ণ *path);

স্থির অন্তঃস্থ পূর্ণাঙ্ক setsockopt_int(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক level, পূর্ণাঙ্ক optname, পূর্ণাঙ্ক মান) {
        যদি (setsockopt(fd, level, optname, &মান, পরিমাণ(মান)) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক getsockopt_int(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক level, পূর্ণাঙ্ক optname, পূর্ণাঙ্ক *ret);

পূর্ণাঙ্ক socket_bind_to_ifname(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *ifname);
পূর্ণাঙ্ক socket_bind_to_ifindex(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক ifindex);

পূর্ণাঙ্ক socket_autobind(পূর্ণাঙ্ক fd, বর্ণ **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(পরিমাণ(সংরচনা timeval)) + CMSG_SPACE(2 * পরিমাণ(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(পরিমাণ(সংরচনা timespec)) + CMSG_SPACE(2 * পরিমাণ(uint64_t)))

ssize_t recvmsg_safe(পূর্ণাঙ্ক sockfd, সংরচনা msghdr *msg, পূর্ণাঙ্ক flags);

পূর্ণাঙ্ক socket_get_family(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক socket_set_recvpktinfo(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b);
পূর্ণাঙ্ক socket_set_unicast_if(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, পূর্ণাঙ্ক ifi);

পূর্ণাঙ্ক socket_set_option(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, পূর্ণাঙ্ক opt_ipv4, পূর্ণাঙ্ক opt_ipv6, পূর্ণাঙ্ক val);
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_recverr(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b) {
        প্রত্যাগম socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_recvttl(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b) {
        প্রত্যাগম socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_ttl(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, পূর্ণাঙ্ক ttl) {
        প্রত্যাগম socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_freebind(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b) {
        প্রত্যাগম socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_transparent(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b) {
        প্রত্যাগম socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক socket_set_recvfragsize(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, bool b) {
        প্রত্যাগম socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

পূর্ণাঙ্ক socket_get_mtu(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

পূর্ণাঙ্ক connect_unix_path(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path);

স্থির অন্তঃস্থ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        প্রত্যাগম !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

পূর্ণাঙ্ক vsock_parse_port(ধ্রুব বর্ণ *s, unsigned *ret);
পূর্ণাঙ্ক vsock_parse_cid(ধ্রুব বর্ণ *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
পূর্ণাঙ্ক socket_address_parse_unix(SocketAddress *ret_address, ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক socket_address_parse_vsock(SocketAddress *ret_address, ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক socket_address_equal_unix(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

পূর্ণাঙ্ক netlink_socket_get_multicast_groups(পূর্ণাঙ্ক fd, size_t *ret_len, uint32_t **ret_groups);

পূর্ণাঙ্ক socket_get_cookie(পূর্ণাঙ্ক fd, uint64_t *ret);

শূন্য cmsg_close_all(সংরচনা msghdr *mh);

পূর্ণাঙ্ক tos_to_priority(uint8_t tos);

পূর্ণাঙ্ক socket_xattr_supported(শূন্য);
