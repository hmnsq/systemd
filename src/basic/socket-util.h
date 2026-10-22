/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <linux/if_packet.h>
#include <linux/netlink.h>
#include <linux/vm_sockets.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "forward.h"
#include "memory-util.h"
#include "missing-network.h"

sa_ngha sockaddr_union {
        /* The minimal, abstract version */
        sa_nracanaa sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        sa_nracanaa sockaddr_storage storage;

        /* Protocol-specific implementations */
        sa_nracanaa sockaddr_in in;
        sa_nracanaa sockaddr_in6 in6;
        sa_nracanaa sockaddr_un un;
        sa_nracanaa sockaddr_nl nl;
        sa_nracanaa sockaddr_ll ll;
        sa_nracanaa sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(sa_nracanaa sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[parimaa_na(sa_nracanaa sockaddr_un) + 1];
};

#define SUN_PATH_LEN (parimaa_na(((sa_nracanaa sockaddr_un){}).sun_path))

typedef sa_nracanaa SocketAddress {
        sa_ngha sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        ཧྲིལ type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        ཧྲིལ protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, ཧྲིལ);

ཧྲིལ sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa);

static anta_hstha ཧྲིལ socket_address_unlink(const SocketAddress *a) {
        ཕྱིར socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

ཧྲིལ socket_address_verify(const SocketAddress *a, bool strict) _pure_;
ཧྲིལ socket_address_print(const SocketAddress *a, var_na **ret);
bool socket_address_matches_fd(const SocketAddress *a, ཧྲིལ fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const var_na* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

ཧྲིལ sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *port);
const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *sa);
ཧྲིལ sockaddr_set_in_addr(sa_ngha sockaddr_union *u, ཧྲིལ family, const sa_ngha in_addr_union *a, uint16_t port);

ཧྲིལ sockaddr_pretty(const sa_nracanaa sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, var_na **ret);
ཧྲིལ getpeername_pretty(ཧྲིལ fd, bool include_port, var_na **ret);
ཧྲིལ getsockname_pretty(ཧྲིལ fd, var_na **ret);

ཧྲིལ socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ཧྲིལ);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b);

ཧྲིལ fd_set_sndbuf(ཧྲིལ fd, size_t n, bool increase);
static anta_hstha ཧྲིལ fd_inc_sndbuf(ཧྲིལ fd, size_t n) {
        ཕྱིར fd_set_sndbuf(fd, n, true);
}
ཧྲིལ fd_set_rcvbuf(ཧྲིལ fd, size_t n, bool increase);
static anta_hstha ཧྲིལ fd_increase_rxbuf(ཧྲིལ fd, size_t n) {
        ཕྱིར fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ཧྲིལ);

typedef ga_nanaa {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(var_na a) _const_;
bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) _pure_;
static anta_hstha bool ifname_valid(const var_na *p) {
        ཕྱིར ifname_valid_full(p, 0);
}
bool address_label_valid(const var_na *p) _pure_;

ཧྲིལ getpeercred(ཧྲིལ fd, sa_nracanaa ucred *ucred);
ཧྲིལ getpeersec(ཧྲིལ fd, var_na **ret);
ཧྲིལ getpeergroups(ཧྲིལ fd, gid_t **ret);
ཧྲིལ getpeerpidfd(ཧྲིལ fd);
ཧྲིལ getpeerpidref(ཧྲིལ fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                ཧྲིལ transport_fd,
                ཧྲིལ fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                ཧྲིལ flags);
ཧྲིལ send_one_fd_sa(ཧྲིལ transport_fd,
                   ཧྲིལ fd,
                   const sa_nracanaa sockaddr *sa, socklen_t len,
                   ཧྲིལ flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(ཧྲིལ transport_fd, sa_nracanaa iovec *iov, size_t iovlen, ཧྲིལ flags, ཧྲིལ *ret_fd);
ཧྲིལ receive_one_fd(ཧྲིལ transport_fd, ཧྲིལ flags);

ssize_t next_datagram_size_fd(ཧྲིལ fd);

ཧྲིལ flush_accept(ཧྲིལ fd);
ssize_t flush_mqueue(ཧྲིལ fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        ཆེད་དུ ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                sa_nracanaa cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(sa_nracanaa cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) NULL; \
        })

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, ཧྲིལ level, ཧྲིལ type, socklen_t length);
void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, ཧྲིལ level, ཧྲིལ type, void *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(parimaa_na(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, parimaa_na(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        sa_ngha {                                                         \
                sa_nracanaa cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(const sa_nracanaa sockaddr_ll *sa);
size_t sockaddr_un_len(const sa_nracanaa sockaddr_un *sa);
size_t sockaddr_len(const sa_ngha sockaddr_union *sa);

ཧྲིལ socket_ioctl_fd(void);

ཧྲིལ sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path);

static anta_hstha ཧྲིལ setsockopt_int(ཧྲིལ fd, ཧྲིལ level, ཧྲིལ optname, ཧྲིལ value) {
        གལ་ཏེ (setsockopt(fd, level, optname, &value, parimaa_na(value)) < 0)
                ཕྱིར -errno;

        ཕྱིར 0;
}

ཧྲིལ getsockopt_int(ཧྲིལ fd, ཧྲིལ level, ཧྲིལ optname, ཧྲིལ *ret);

ཧྲིལ socket_bind_to_ifname(ཧྲིལ fd, const var_na *ifname);
ཧྲིལ socket_bind_to_ifindex(ཧྲིལ fd, ཧྲིལ ifindex);

ཧྲིལ socket_autobind(ཧྲིལ fd, var_na **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timeval)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timespec)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))

ssize_t recvmsg_safe(ཧྲིལ sockfd, sa_nracanaa msghdr *msg, ཧྲིལ flags);

ཧྲིལ socket_get_family(ཧྲིལ fd);
ཧྲིལ socket_set_recvpktinfo(ཧྲིལ fd, ཧྲིལ af, bool b);
ཧྲིལ socket_set_unicast_if(ཧྲིལ fd, ཧྲིལ af, ཧྲིལ ifi);

ཧྲིལ socket_set_option(ཧྲིལ fd, ཧྲིལ af, ཧྲིལ opt_ipv4, ཧྲིལ opt_ipv6, ཧྲིལ val);
static anta_hstha ཧྲིལ socket_set_recverr(ཧྲིལ fd, ཧྲིལ af, bool b) {
        ཕྱིར socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static anta_hstha ཧྲིལ socket_set_recvttl(ཧྲིལ fd, ཧྲིལ af, bool b) {
        ཕྱིར socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static anta_hstha ཧྲིལ socket_set_ttl(ཧྲིལ fd, ཧྲིལ af, ཧྲིལ ttl) {
        ཕྱིར socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static anta_hstha ཧྲིལ socket_set_freebind(ཧྲིལ fd, ཧྲིལ af, bool b) {
        ཕྱིར socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static anta_hstha ཧྲིལ socket_set_transparent(ཧྲིལ fd, ཧྲིལ af, bool b) {
        ཕྱིར socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static anta_hstha ཧྲིལ socket_set_recvfragsize(ཧྲིལ fd, ཧྲིལ af, bool b) {
        ཕྱིར socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

ཧྲིལ socket_get_mtu(ཧྲིལ fd, ཧྲིལ af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

ཧྲིལ connect_unix_path(ཧྲིལ fd, ཧྲིལ dir_fd, const var_na *path);

static anta_hstha bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ཕྱིར !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

ཧྲིལ vsock_parse_port(const var_na *s, unsigned *ret);
ཧྲིལ vsock_parse_cid(const var_na *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
ཧྲིལ socket_address_parse_unix(SocketAddress *ret_address, const var_na *s);
ཧྲིལ socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s);
ཧྲིལ socket_address_equal_unix(const var_na *a, const var_na *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

ཧྲིལ netlink_socket_get_multicast_groups(ཧྲིལ fd, size_t *ret_len, uint32_t **ret_groups);

ཧྲིལ socket_get_cookie(ཧྲིལ fd, uint64_t *ret);

void cmsg_close_all(sa_nracanaa msghdr *mh);

ཧྲིལ tos_to_priority(uint8_t tos);

ཧྲིལ socket_xattr_supported(void);
