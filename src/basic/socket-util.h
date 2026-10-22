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

union sockaddr_union {
        /* The minimal, abstract version */
        struct sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        struct sockaddr_storage storage;

        /* Protocol-specific implementations */
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
        struct sockaddr_un un;
        struct sockaddr_nl nl;
        struct sockaddr_ll ll;
        struct sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(struct sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[sizeof(struct sockaddr_un) + 1];
};

#define SUN_PATH_LEN (sizeof(((struct sockaddr_un){}).sun_path))

typedef struct SocketAddress {
        union sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        정수 type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        정수 protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, 정수);

정수 sockaddr_un_unlink(const struct sockaddr_un *sa);

static inline 정수 socket_address_unlink(const SocketAddress *a) {
        반환 socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

정수 socket_address_verify(const SocketAddress *a, bool strict) _pure_;
정수 socket_address_print(const SocketAddress *a, char **ret);
bool socket_address_matches_fd(const SocketAddress *a, 정수 fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const char* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

정수 sockaddr_port(const struct sockaddr *_sa, unsigned *port);
const union in_addr_union *sockaddr_in_addr(const struct sockaddr *sa);
정수 sockaddr_set_in_addr(union sockaddr_union *u, 정수 family, const union in_addr_union *a, uint16_t port);

정수 sockaddr_pretty(const struct sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, char **ret);
정수 getpeername_pretty(정수 fd, bool include_port, char **ret);
정수 getsockname_pretty(정수 fd, char **ret);

정수 socknameinfo_pretty(const struct sockaddr *sa, socklen_t salen, char **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, 정수);

bool sockaddr_equal(const union sockaddr_union *a, const union sockaddr_union *b);

정수 fd_set_sndbuf(정수 fd, size_t n, bool increase);
static inline 정수 fd_inc_sndbuf(정수 fd, size_t n) {
        반환 fd_set_sndbuf(fd, n, true);
}
정수 fd_set_rcvbuf(정수 fd, size_t n, bool increase);
static inline 정수 fd_increase_rxbuf(정수 fd, size_t n) {
        반환 fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, 정수);

typedef enum {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(char a) _const_;
bool ifname_valid_full(const char *p, IfnameValidFlags flags) _pure_;
static inline bool ifname_valid(const char *p) {
        반환 ifname_valid_full(p, 0);
}
bool address_label_valid(const char *p) _pure_;

정수 getpeercred(정수 fd, struct ucred *ucred);
정수 getpeersec(정수 fd, char **ret);
정수 getpeergroups(정수 fd, gid_t **ret);
정수 getpeerpidfd(정수 fd);
정수 getpeerpidref(정수 fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                정수 transport_fd,
                정수 fd,
                const struct iovec *iov, size_t iovlen,
                const struct sockaddr *sa, socklen_t len,
                정수 flags);
정수 send_one_fd_sa(정수 transport_fd,
                   정수 fd,
                   const struct sockaddr *sa, socklen_t len,
                   정수 flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(정수 transport_fd, struct iovec *iov, size_t iovlen, 정수 flags, 정수 *ret_fd);
정수 receive_one_fd(정수 transport_fd, 정수 flags);

ssize_t next_datagram_size_fd(정수 fd);

정수 flush_accept(정수 fd);
ssize_t flush_mqueue(정수 fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        위해 ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                struct cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(struct cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) NULL; \
        })

struct cmsghdr* cmsg_find(struct msghdr *mh, 정수 level, 정수 type, socklen_t length);
void* cmsg_find_and_copy_data(struct msghdr *mh, 정수 level, 정수 type, void *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(sizeof(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, sizeof(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        union {                                                         \
                struct cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(const struct sockaddr_ll *sa);
size_t sockaddr_un_len(const struct sockaddr_un *sa);
size_t sockaddr_len(const union sockaddr_union *sa);

정수 socket_ioctl_fd(void);

정수 sockaddr_un_set_path(struct sockaddr_un *ret, const char *path);

static inline 정수 setsockopt_int(정수 fd, 정수 level, 정수 optname, 정수 value) {
        만약 (setsockopt(fd, level, optname, &value, sizeof(value)) < 0)
                반환 -errno;

        반환 0;
}

정수 getsockopt_int(정수 fd, 정수 level, 정수 optname, 정수 *ret);

정수 socket_bind_to_ifname(정수 fd, const char *ifname);
정수 socket_bind_to_ifindex(정수 fd, 정수 ifindex);

정수 socket_autobind(정수 fd, char **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(2 * sizeof(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(sizeof(struct timespec)) + CMSG_SPACE(2 * sizeof(uint64_t)))

ssize_t recvmsg_safe(정수 sockfd, struct msghdr *msg, 정수 flags);

정수 socket_get_family(정수 fd);
정수 socket_set_recvpktinfo(정수 fd, 정수 af, bool b);
정수 socket_set_unicast_if(정수 fd, 정수 af, 정수 ifi);

정수 socket_set_option(정수 fd, 정수 af, 정수 opt_ipv4, 정수 opt_ipv6, 정수 val);
static inline 정수 socket_set_recverr(정수 fd, 정수 af, bool b) {
        반환 socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static inline 정수 socket_set_recvttl(정수 fd, 정수 af, bool b) {
        반환 socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static inline 정수 socket_set_ttl(정수 fd, 정수 af, 정수 ttl) {
        반환 socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static inline 정수 socket_set_freebind(정수 fd, 정수 af, bool b) {
        반환 socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static inline 정수 socket_set_transparent(정수 fd, 정수 af, bool b) {
        반환 socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static inline 정수 socket_set_recvfragsize(정수 fd, 정수 af, bool b) {
        반환 socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

정수 socket_get_mtu(정수 fd, 정수 af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

정수 connect_unix_path(정수 fd, 정수 dir_fd, const char *path);

static inline bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        반환 !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

정수 vsock_parse_port(const char *s, unsigned *ret);
정수 vsock_parse_cid(const char *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
정수 socket_address_parse_unix(SocketAddress *ret_address, const char *s);
정수 socket_address_parse_vsock(SocketAddress *ret_address, const char *s);
정수 socket_address_equal_unix(const char *a, const char *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

정수 netlink_socket_get_multicast_groups(정수 fd, size_t *ret_len, uint32_t **ret_groups);

정수 socket_get_cookie(정수 fd, uint64_t *ret);

void cmsg_close_all(struct msghdr *mh);

정수 tos_to_priority(uint8_t tos);

정수 socket_xattr_supported(void);
