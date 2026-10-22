/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <linux/if_packet.h>
#અન્તર્ભાવ <linux/netlink.h>
#અન્તર્ભાવ <linux/vm_sockets.h>
#અન્તર્ભાવ <netinet/in.h>
#અન્તર્ભાવ <sys/socket.h>
#અન્તર્ભાવ <sys/un.h>

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "missing-network.h"

સંઘ sockaddr_union {
        /* The minimal, abstract version */
        સંરચના sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        સંરચના sockaddr_storage storage;

        /* Protocol-specific implementations */
        સંરચના sockaddr_in in;
        સંરચના sockaddr_in6 in6;
        સંરચના sockaddr_un un;
        સંરચના sockaddr_nl nl;
        સંરચના sockaddr_ll ll;
        સંરચના sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(સંરચના sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[કદ(સંરચના sockaddr_un) + 1];
};

#define SUN_PATH_LEN (કદ(((સંરચના sockaddr_un){}).sun_path))

પ્રકારવ્યાખ્યા સંરચના SocketAddress {
        સંઘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        પૂર્ણાંક type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        પૂર્ણાંક protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, પૂર્ણાંક);

પૂર્ણાંક sockaddr_un_unlink(અચલ સંરચના sockaddr_un *sa);

સ્થિર અંતઃરેખ પૂર્ણાંક socket_address_unlink(અચલ SocketAddress *a) {
        પાછા socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(અચલ SocketAddress *a) _pure_;

પૂર્ણાંક socket_address_verify(અચલ SocketAddress *a, bool strict) _pure_;
પૂર્ણાંક socket_address_print(અચલ SocketAddress *a, અક્ષર **ret);
bool socket_address_matches_fd(અચલ SocketAddress *a, પૂર્ણાંક fd);

bool socket_address_equal(અચલ SocketAddress *a, અચલ SocketAddress *b) _pure_;

અચલ અક્ષર* socket_address_get_path(અચલ SocketAddress *a);

bool socket_ipv6_is_supported(શૂન્ય);
bool socket_ipv6_is_enabled(શૂન્ય);

પૂર્ણાંક sockaddr_port(અચલ સંરચના sockaddr *_sa, unsigned *port);
અચલ સંઘ in_addr_union *sockaddr_in_addr(અચલ સંરચના sockaddr *sa);
પૂર્ણાંક sockaddr_set_in_addr(સંઘ sockaddr_union *u, પૂર્ણાંક family, અચલ સંઘ in_addr_union *a, uint16_t port);

પૂર્ણાંક sockaddr_pretty(અચલ સંરચના sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, અક્ષર **ret);
પૂર્ણાંક getpeername_pretty(પૂર્ણાંક fd, bool include_port, અક્ષર **ret);
પૂર્ણાંક getsockname_pretty(પૂર્ણાંક fd, અક્ષર **ret);

પૂર્ણાંક socknameinfo_pretty(અચલ સંરચના sockaddr *sa, socklen_t salen, અક્ષર **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, પૂર્ણાંક);

bool sockaddr_equal(અચલ સંઘ sockaddr_union *a, અચલ સંઘ sockaddr_union *b);

પૂર્ણાંક fd_set_sndbuf(પૂર્ણાંક fd, size_t n, bool increase);
સ્થિર અંતઃરેખ પૂર્ણાંક fd_inc_sndbuf(પૂર્ણાંક fd, size_t n) {
        પાછા fd_set_sndbuf(fd, n, true);
}
પૂર્ણાંક fd_set_rcvbuf(પૂર્ણાંક fd, size_t n, bool increase);
સ્થિર અંતઃરેખ પૂર્ણાંક fd_increase_rxbuf(પૂર્ણાંક fd, size_t n) {
        પાછા fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, પૂર્ણાંક);

પ્રકારવ્યાખ્યા ગણના {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(અક્ષર a) _const_;
bool ifname_valid_full(અચલ અક્ષર *p, IfnameValidFlags flags) _pure_;
સ્થિર અંતઃરેખ bool ifname_valid(અચલ અક્ષર *p) {
        પાછા ifname_valid_full(p, 0);
}
bool address_label_valid(અચલ અક્ષર *p) _pure_;

પૂર્ણાંક getpeercred(પૂર્ણાંક fd, સંરચના ucred *ucred);
પૂર્ણાંક getpeersec(પૂર્ણાંક fd, અક્ષર **ret);
પૂર્ણાંક getpeergroups(પૂર્ણાંક fd, gid_t **ret);
પૂર્ણાંક getpeerpidfd(પૂર્ણાંક fd);
પૂર્ણાંક getpeerpidref(પૂર્ણાંક fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                પૂર્ણાંક transport_fd,
                પૂર્ણાંક fd,
                અચલ સંરચના iovec *iov, size_t iovlen,
                અચલ સંરચના sockaddr *sa, socklen_t len,
                પૂર્ણાંક flags);
પૂર્ણાંક send_one_fd_sa(પૂર્ણાંક transport_fd,
                   પૂર્ણાંક fd,
                   અચલ સંરચના sockaddr *sa, socklen_t len,
                   પૂર્ણાંક flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ખાલી, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ખાલી, 0, ખાલી, 0, flags)
ssize_t receive_one_fd_iov(પૂર્ણાંક transport_fd, સંરચના iovec *iov, size_t iovlen, પૂર્ણાંક flags, પૂર્ણાંક *ret_fd);
પૂર્ણાંક receive_one_fd(પૂર્ણાંક transport_fd, પૂર્ણાંક flags);

ssize_t next_datagram_size_fd(પૂર્ણાંક fd);

પૂર્ણાંક flush_accept(પૂર્ણાંક fd);
ssize_t flush_mqueue(પૂર્ણાંક fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        માટે ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                સંરચના cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(સંરચના cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ખાલી; \
        })

સંરચના cmsghdr* cmsg_find(સંરચના msghdr *mh, પૂર્ણાંક level, પૂર્ણાંક type, socklen_t length);
શૂન્ય* cmsg_find_and_copy_data(સંરચના msghdr *mh, પૂર્ણાંક level, પૂર્ણાંક type, શૂન્ય *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(કદ(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, કદ(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        સંઘ {                                                         \
                સંરચના cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(અચલ સંરચના sockaddr_ll *sa);
size_t sockaddr_un_len(અચલ સંરચના sockaddr_un *sa);
size_t sockaddr_len(અચલ સંઘ sockaddr_union *sa);

પૂર્ણાંક socket_ioctl_fd(શૂન્ય);

પૂર્ણાંક sockaddr_un_set_path(સંરચના sockaddr_un *ret, અચલ અક્ષર *path);

સ્થિર અંતઃરેખ પૂર્ણાંક setsockopt_int(પૂર્ણાંક fd, પૂર્ણાંક level, પૂર્ણાંક optname, પૂર્ણાંક કિંમત) {
        જો (setsockopt(fd, level, optname, &કિંમત, કદ(કિંમત)) < 0)
                પાછા -errno;

        પાછા 0;
}

પૂર્ણાંક getsockopt_int(પૂર્ણાંક fd, પૂર્ણાંક level, પૂર્ણાંક optname, પૂર્ણાંક *ret);

પૂર્ણાંક socket_bind_to_ifname(પૂર્ણાંક fd, અચલ અક્ષર *ifname);
પૂર્ણાંક socket_bind_to_ifindex(પૂર્ણાંક fd, પૂર્ણાંક ifindex);

પૂર્ણાંક socket_autobind(પૂર્ણાંક fd, અક્ષર **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(કદ(સંરચના timeval)) + CMSG_SPACE(2 * કદ(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(કદ(સંરચના timespec)) + CMSG_SPACE(2 * કદ(uint64_t)))

ssize_t recvmsg_safe(પૂર્ણાંક sockfd, સંરચના msghdr *msg, પૂર્ણાંક flags);

પૂર્ણાંક socket_get_family(પૂર્ણાંક fd);
પૂર્ણાંક socket_set_recvpktinfo(પૂર્ણાંક fd, પૂર્ણાંક af, bool b);
પૂર્ણાંક socket_set_unicast_if(પૂર્ણાંક fd, પૂર્ણાંક af, પૂર્ણાંક ifi);

પૂર્ણાંક socket_set_option(પૂર્ણાંક fd, પૂર્ણાંક af, પૂર્ણાંક opt_ipv4, પૂર્ણાંક opt_ipv6, પૂર્ણાંક val);
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_recverr(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {
        પાછા socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_recvttl(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {
        પાછા socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_ttl(પૂર્ણાંક fd, પૂર્ણાંક af, પૂર્ણાંક ttl) {
        પાછા socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_freebind(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {
        પાછા socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_transparent(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {
        પાછા socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
સ્થિર અંતઃરેખ પૂર્ણાંક socket_set_recvfragsize(પૂર્ણાંક fd, પૂર્ણાંક af, bool b) {
        પાછા socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

પૂર્ણાંક socket_get_mtu(પૂર્ણાંક fd, પૂર્ણાંક af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

પૂર્ણાંક connect_unix_path(પૂર્ણાંક fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path);

સ્થિર અંતઃરેખ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        પાછા !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

પૂર્ણાંક vsock_parse_port(અચલ અક્ષર *s, unsigned *ret);
પૂર્ણાંક vsock_parse_cid(અચલ અક્ષર *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
પૂર્ણાંક socket_address_parse_unix(SocketAddress *ret_address, અચલ અક્ષર *s);
પૂર્ણાંક socket_address_parse_vsock(SocketAddress *ret_address, અચલ અક્ષર *s);
પૂર્ણાંક socket_address_equal_unix(અચલ અક્ષર *a, અચલ અક્ષર *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

પૂર્ણાંક netlink_socket_get_multicast_groups(પૂર્ણાંક fd, size_t *ret_len, uint32_t **ret_groups);

પૂર્ણાંક socket_get_cookie(પૂર્ણાંક fd, uint64_t *ret);

શૂન્ય cmsg_close_all(સંરચના msghdr *mh);

પૂર્ણાંક tos_to_priority(uint8_t tos);

પૂર્ણાંક socket_xattr_supported(શૂન્ય);
