/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <linux/if_packet.h>
#അന്തര്ഭാവ <linux/netlink.h>
#അന്തര്ഭാവ <linux/vm_sockets.h>
#അന്തര്ഭാവ <netinet/in.h>
#അന്തര്ഭാവ <sys/socket.h>
#അന്തര്ഭാവ <sys/un.h>

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "missing-network.h"

സംഘം sockaddr_union {
        /* The minimal, abstract version */
        ഘടന sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ഘടന sockaddr_storage storage;

        /* Protocol-specific implementations */
        ഘടന sockaddr_in in;
        ഘടന sockaddr_in6 in6;
        ഘടന sockaddr_un un;
        ഘടന sockaddr_nl nl;
        ഘടന sockaddr_ll ll;
        ഘടന sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(ഘടന sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[വലുപ്പം(ഘടന sockaddr_un) + 1];
};

#define SUN_PATH_LEN (വലുപ്പം(((ഘടന sockaddr_un){}).sun_path))

തരനിർവചനം ഘടന SocketAddress {
        സംഘം sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        പൂർണ്ണസംഖ്യ type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        പൂർണ്ണസംഖ്യ protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, പൂർണ്ണസംഖ്യ);

പൂർണ്ണസംഖ്യ sockaddr_un_unlink(സ്ഥിരം ഘടന sockaddr_un *sa);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_address_unlink(സ്ഥിരം SocketAddress *a) {
        തിരികെ socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(സ്ഥിരം SocketAddress *a) _pure_;

പൂർണ്ണസംഖ്യ socket_address_verify(സ്ഥിരം SocketAddress *a, bool strict) _pure_;
പൂർണ്ണസംഖ്യ socket_address_print(സ്ഥിരം SocketAddress *a, അക്ഷരം **ret);
bool socket_address_matches_fd(സ്ഥിരം SocketAddress *a, പൂർണ്ണസംഖ്യ fd);

bool socket_address_equal(സ്ഥിരം SocketAddress *a, സ്ഥിരം SocketAddress *b) _pure_;

സ്ഥിരം അക്ഷരം* socket_address_get_path(സ്ഥിരം SocketAddress *a);

bool socket_ipv6_is_supported(ശൂന്യം);
bool socket_ipv6_is_enabled(ശൂന്യം);

പൂർണ്ണസംഖ്യ sockaddr_port(സ്ഥിരം ഘടന sockaddr *_sa, unsigned *port);
സ്ഥിരം സംഘം in_addr_union *sockaddr_in_addr(സ്ഥിരം ഘടന sockaddr *sa);
പൂർണ്ണസംഖ്യ sockaddr_set_in_addr(സംഘം sockaddr_union *u, പൂർണ്ണസംഖ്യ family, സ്ഥിരം സംഘം in_addr_union *a, uint16_t port);

പൂർണ്ണസംഖ്യ sockaddr_pretty(സ്ഥിരം ഘടന sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ getpeername_pretty(പൂർണ്ണസംഖ്യ fd, bool include_port, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ getsockname_pretty(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ socknameinfo_pretty(സ്ഥിരം ഘടന sockaddr *sa, socklen_t salen, അക്ഷരം **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, പൂർണ്ണസംഖ്യ);

bool sockaddr_equal(സ്ഥിരം സംഘം sockaddr_union *a, സ്ഥിരം സംഘം sockaddr_union *b);

പൂർണ്ണസംഖ്യ fd_set_sndbuf(പൂർണ്ണസംഖ്യ fd, size_t n, bool increase);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_inc_sndbuf(പൂർണ്ണസംഖ്യ fd, size_t n) {
        തിരികെ fd_set_sndbuf(fd, n, true);
}
പൂർണ്ണസംഖ്യ fd_set_rcvbuf(പൂർണ്ണസംഖ്യ fd, size_t n, bool increase);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_increase_rxbuf(പൂർണ്ണസംഖ്യ fd, size_t n) {
        തിരികെ fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, പൂർണ്ണസംഖ്യ);

തരനിർവചനം എണ്ണൽ {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(അക്ഷരം a) _const_;
bool ifname_valid_full(സ്ഥിരം അക്ഷരം *p, IfnameValidFlags flags) _pure_;
സ്ഥിതം വരിയിൽ bool ifname_valid(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ ifname_valid_full(p, 0);
}
bool address_label_valid(സ്ഥിരം അക്ഷരം *p) _pure_;

പൂർണ്ണസംഖ്യ getpeercred(പൂർണ്ണസംഖ്യ fd, ഘടന ucred *ucred);
പൂർണ്ണസംഖ്യ getpeersec(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ getpeergroups(പൂർണ്ണസംഖ്യ fd, gid_t **ret);
പൂർണ്ണസംഖ്യ getpeerpidfd(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ getpeerpidref(പൂർണ്ണസംഖ്യ fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                പൂർണ്ണസംഖ്യ transport_fd,
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം ഘടന iovec *iov, size_t iovlen,
                സ്ഥിരം ഘടന sockaddr *sa, socklen_t len,
                പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ send_one_fd_sa(പൂർണ്ണസംഖ്യ transport_fd,
                   പൂർണ്ണസംഖ്യ fd,
                   സ്ഥിരം ഘടന sockaddr *sa, socklen_t len,
                   പൂർണ്ണസംഖ്യ flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ശൂന്യ, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ശൂന്യ, 0, ശൂന്യ, 0, flags)
ssize_t receive_one_fd_iov(പൂർണ്ണസംഖ്യ transport_fd, ഘടന iovec *iov, size_t iovlen, പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ *ret_fd);
പൂർണ്ണസംഖ്യ receive_one_fd(പൂർണ്ണസംഖ്യ transport_fd, പൂർണ്ണസംഖ്യ flags);

ssize_t next_datagram_size_fd(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ flush_accept(പൂർണ്ണസംഖ്യ fd);
ssize_t flush_mqueue(പൂർണ്ണസംഖ്യ fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        വേണ്ടി ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                ഘടന cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ഘടന cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ശൂന്യ; \
        })

ഘടന cmsghdr* cmsg_find(ഘടന msghdr *mh, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ type, socklen_t length);
ശൂന്യം* cmsg_find_and_copy_data(ഘടന msghdr *mh, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ type, ശൂന്യം *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(വലുപ്പം(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, വലുപ്പം(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        സംഘം {                                                         \
                ഘടന cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(സ്ഥിരം ഘടന sockaddr_ll *sa);
size_t sockaddr_un_len(സ്ഥിരം ഘടന sockaddr_un *sa);
size_t sockaddr_len(സ്ഥിരം സംഘം sockaddr_union *sa);

പൂർണ്ണസംഖ്യ socket_ioctl_fd(ശൂന്യം);

പൂർണ്ണസംഖ്യ sockaddr_un_set_path(ഘടന sockaddr_un *ret, സ്ഥിരം അക്ഷരം *path);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ setsockopt_int(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ optname, പൂർണ്ണസംഖ്യ മൂല്യം) {
        എങ്കിൽ (setsockopt(fd, level, optname, &മൂല്യം, വലുപ്പം(മൂല്യം)) < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getsockopt_int(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ level, പൂർണ്ണസംഖ്യ optname, പൂർണ്ണസംഖ്യ *ret);

പൂർണ്ണസംഖ്യ socket_bind_to_ifname(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *ifname);
പൂർണ്ണസംഖ്യ socket_bind_to_ifindex(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ ifindex);

പൂർണ്ണസംഖ്യ socket_autobind(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(വലുപ്പം(ഘടന timeval)) + CMSG_SPACE(2 * വലുപ്പം(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(വലുപ്പം(ഘടന timespec)) + CMSG_SPACE(2 * വലുപ്പം(uint64_t)))

ssize_t recvmsg_safe(പൂർണ്ണസംഖ്യ sockfd, ഘടന msghdr *msg, പൂർണ്ണസംഖ്യ flags);

പൂർണ്ണസംഖ്യ socket_get_family(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ socket_set_recvpktinfo(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b);
പൂർണ്ണസംഖ്യ socket_set_unicast_if(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, പൂർണ്ണസംഖ്യ ifi);

പൂർണ്ണസംഖ്യ socket_set_option(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, പൂർണ്ണസംഖ്യ opt_ipv4, പൂർണ്ണസംഖ്യ opt_ipv6, പൂർണ്ണസംഖ്യ val);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_recverr(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {
        തിരികെ socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_recvttl(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {
        തിരികെ socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_ttl(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, പൂർണ്ണസംഖ്യ ttl) {
        തിരികെ socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_freebind(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {
        തിരികെ socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_transparent(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {
        തിരികെ socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ socket_set_recvfragsize(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, bool b) {
        തിരികെ socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

പൂർണ്ണസംഖ്യ socket_get_mtu(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

പൂർണ്ണസംഖ്യ connect_unix_path(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path);

സ്ഥിതം വരിയിൽ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        തിരികെ !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

പൂർണ്ണസംഖ്യ vsock_parse_port(സ്ഥിരം അക്ഷരം *s, unsigned *ret);
പൂർണ്ണസംഖ്യ vsock_parse_cid(സ്ഥിരം അക്ഷരം *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
പൂർണ്ണസംഖ്യ socket_address_parse_unix(SocketAddress *ret_address, സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ socket_address_parse_vsock(SocketAddress *ret_address, സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ socket_address_equal_unix(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

പൂർണ്ണസംഖ്യ netlink_socket_get_multicast_groups(പൂർണ്ണസംഖ്യ fd, size_t *ret_len, uint32_t **ret_groups);

പൂർണ്ണസംഖ്യ socket_get_cookie(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);

ശൂന്യം cmsg_close_all(ഘടന msghdr *mh);

പൂർണ്ണസംഖ്യ tos_to_priority(uint8_t tos);

പൂർണ്ണസംഖ്യ socket_xattr_supported(ശൂന്യം);
