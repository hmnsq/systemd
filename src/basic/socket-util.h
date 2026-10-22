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
        số_nguyên type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        số_nguyên protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, số_nguyên);

số_nguyên sockaddr_un_unlink(const struct sockaddr_un *sa);

static inline số_nguyên socket_address_unlink(const SocketAddress *a) {
        trả_về socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

số_nguyên socket_address_verify(const SocketAddress *a, bool strict) _pure_;
số_nguyên socket_address_print(const SocketAddress *a, char **ret);
bool socket_address_matches_fd(const SocketAddress *a, số_nguyên fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const char* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

số_nguyên sockaddr_port(const struct sockaddr *_sa, unsigned *port);
const union in_addr_union *sockaddr_in_addr(const struct sockaddr *sa);
số_nguyên sockaddr_set_in_addr(union sockaddr_union *u, số_nguyên family, const union in_addr_union *a, uint16_t port);

số_nguyên sockaddr_pretty(const struct sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, char **ret);
số_nguyên getpeername_pretty(số_nguyên fd, bool include_port, char **ret);
số_nguyên getsockname_pretty(số_nguyên fd, char **ret);

số_nguyên socknameinfo_pretty(const struct sockaddr *sa, socklen_t salen, char **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, số_nguyên);

bool sockaddr_equal(const union sockaddr_union *a, const union sockaddr_union *b);

số_nguyên fd_set_sndbuf(số_nguyên fd, size_t n, bool increase);
static inline số_nguyên fd_inc_sndbuf(số_nguyên fd, size_t n) {
        trả_về fd_set_sndbuf(fd, n, true);
}
số_nguyên fd_set_rcvbuf(số_nguyên fd, size_t n, bool increase);
static inline số_nguyên fd_increase_rxbuf(số_nguyên fd, size_t n) {
        trả_về fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, số_nguyên);

typedef enum {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(char a) _const_;
bool ifname_valid_full(const char *p, IfnameValidFlags flags) _pure_;
static inline bool ifname_valid(const char *p) {
        trả_về ifname_valid_full(p, 0);
}
bool address_label_valid(const char *p) _pure_;

số_nguyên getpeercred(số_nguyên fd, struct ucred *ucred);
số_nguyên getpeersec(số_nguyên fd, char **ret);
số_nguyên getpeergroups(số_nguyên fd, gid_t **ret);
số_nguyên getpeerpidfd(số_nguyên fd);
số_nguyên getpeerpidref(số_nguyên fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                số_nguyên transport_fd,
                số_nguyên fd,
                const struct iovec *iov, size_t iovlen,
                const struct sockaddr *sa, socklen_t len,
                số_nguyên flags);
số_nguyên send_one_fd_sa(số_nguyên transport_fd,
                   số_nguyên fd,
                   const struct sockaddr *sa, socklen_t len,
                   số_nguyên flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(số_nguyên transport_fd, struct iovec *iov, size_t iovlen, số_nguyên flags, số_nguyên *ret_fd);
số_nguyên receive_one_fd(số_nguyên transport_fd, số_nguyên flags);

ssize_t next_datagram_size_fd(số_nguyên fd);

số_nguyên flush_accept(số_nguyên fd);
ssize_t flush_mqueue(số_nguyên fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        cho ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

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

struct cmsghdr* cmsg_find(struct msghdr *mh, số_nguyên level, số_nguyên type, socklen_t length);
void* cmsg_find_and_copy_data(struct msghdr *mh, số_nguyên level, số_nguyên type, void *buf, size_t buf_len);

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

số_nguyên socket_ioctl_fd(void);

số_nguyên sockaddr_un_set_path(struct sockaddr_un *ret, const char *path);

static inline số_nguyên setsockopt_int(số_nguyên fd, số_nguyên level, số_nguyên optname, số_nguyên value) {
        nếu (setsockopt(fd, level, optname, &value, sizeof(value)) < 0)
                trả_về -errno;

        trả_về 0;
}

số_nguyên getsockopt_int(số_nguyên fd, số_nguyên level, số_nguyên optname, số_nguyên *ret);

số_nguyên socket_bind_to_ifname(số_nguyên fd, const char *ifname);
số_nguyên socket_bind_to_ifindex(số_nguyên fd, số_nguyên ifindex);

số_nguyên socket_autobind(số_nguyên fd, char **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(2 * sizeof(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(sizeof(struct timespec)) + CMSG_SPACE(2 * sizeof(uint64_t)))

ssize_t recvmsg_safe(số_nguyên sockfd, struct msghdr *msg, số_nguyên flags);

số_nguyên socket_get_family(số_nguyên fd);
số_nguyên socket_set_recvpktinfo(số_nguyên fd, số_nguyên af, bool b);
số_nguyên socket_set_unicast_if(số_nguyên fd, số_nguyên af, số_nguyên ifi);

số_nguyên socket_set_option(số_nguyên fd, số_nguyên af, số_nguyên opt_ipv4, số_nguyên opt_ipv6, số_nguyên val);
static inline số_nguyên socket_set_recverr(số_nguyên fd, số_nguyên af, bool b) {
        trả_về socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static inline số_nguyên socket_set_recvttl(số_nguyên fd, số_nguyên af, bool b) {
        trả_về socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static inline số_nguyên socket_set_ttl(số_nguyên fd, số_nguyên af, số_nguyên ttl) {
        trả_về socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static inline số_nguyên socket_set_freebind(số_nguyên fd, số_nguyên af, bool b) {
        trả_về socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static inline số_nguyên socket_set_transparent(số_nguyên fd, số_nguyên af, bool b) {
        trả_về socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static inline số_nguyên socket_set_recvfragsize(số_nguyên fd, số_nguyên af, bool b) {
        trả_về socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

số_nguyên socket_get_mtu(số_nguyên fd, số_nguyên af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

số_nguyên connect_unix_path(số_nguyên fd, số_nguyên dir_fd, const char *path);

static inline bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        trả_về !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

số_nguyên vsock_parse_port(const char *s, unsigned *ret);
số_nguyên vsock_parse_cid(const char *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
số_nguyên socket_address_parse_unix(SocketAddress *ret_address, const char *s);
số_nguyên socket_address_parse_vsock(SocketAddress *ret_address, const char *s);
số_nguyên socket_address_equal_unix(const char *a, const char *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

số_nguyên netlink_socket_get_multicast_groups(số_nguyên fd, size_t *ret_len, uint32_t **ret_groups);

số_nguyên socket_get_cookie(số_nguyên fd, uint64_t *ret);

void cmsg_close_all(struct msghdr *mh);

số_nguyên tos_to_priority(uint8_t tos);

số_nguyên socket_xattr_supported(void);
