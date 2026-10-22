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
        цяло type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        цяло protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, цяло);

цяло sockaddr_un_unlink(const struct sockaddr_un *sa);

static inline цяло socket_address_unlink(const SocketAddress *a) {
        върни socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

цяло socket_address_verify(const SocketAddress *a, bool strict) _pure_;
цяло socket_address_print(const SocketAddress *a, char **ret);
bool socket_address_matches_fd(const SocketAddress *a, цяло fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const char* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

цяло sockaddr_port(const struct sockaddr *_sa, unsigned *port);
const union in_addr_union *sockaddr_in_addr(const struct sockaddr *sa);
цяло sockaddr_set_in_addr(union sockaddr_union *u, цяло family, const union in_addr_union *a, uint16_t port);

цяло sockaddr_pretty(const struct sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, char **ret);
цяло getpeername_pretty(цяло fd, bool include_port, char **ret);
цяло getsockname_pretty(цяло fd, char **ret);

цяло socknameinfo_pretty(const struct sockaddr *sa, socklen_t salen, char **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, цяло);

bool sockaddr_equal(const union sockaddr_union *a, const union sockaddr_union *b);

цяло fd_set_sndbuf(цяло fd, size_t n, bool increase);
static inline цяло fd_inc_sndbuf(цяло fd, size_t n) {
        върни fd_set_sndbuf(fd, n, true);
}
цяло fd_set_rcvbuf(цяло fd, size_t n, bool increase);
static inline цяло fd_increase_rxbuf(цяло fd, size_t n) {
        върни fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, цяло);

typedef enum {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(char a) _const_;
bool ifname_valid_full(const char *p, IfnameValidFlags flags) _pure_;
static inline bool ifname_valid(const char *p) {
        върни ifname_valid_full(p, 0);
}
bool address_label_valid(const char *p) _pure_;

цяло getpeercred(цяло fd, struct ucred *ucred);
цяло getpeersec(цяло fd, char **ret);
цяло getpeergroups(цяло fd, gid_t **ret);
цяло getpeerpidfd(цяло fd);
цяло getpeerpidref(цяло fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                цяло transport_fd,
                цяло fd,
                const struct iovec *iov, size_t iovlen,
                const struct sockaddr *sa, socklen_t len,
                цяло flags);
цяло send_one_fd_sa(цяло transport_fd,
                   цяло fd,
                   const struct sockaddr *sa, socklen_t len,
                   цяло flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(цяло transport_fd, struct iovec *iov, size_t iovlen, цяло flags, цяло *ret_fd);
цяло receive_one_fd(цяло transport_fd, цяло flags);

ssize_t next_datagram_size_fd(цяло fd);

цяло flush_accept(цяло fd);
ssize_t flush_mqueue(цяло fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        за ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

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

struct cmsghdr* cmsg_find(struct msghdr *mh, цяло level, цяло type, socklen_t length);
void* cmsg_find_and_copy_data(struct msghdr *mh, цяло level, цяло type, void *buf, size_t buf_len);

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

цяло socket_ioctl_fd(void);

цяло sockaddr_un_set_path(struct sockaddr_un *ret, const char *path);

static inline цяло setsockopt_int(цяло fd, цяло level, цяло optname, цяло value) {
        ако (setsockopt(fd, level, optname, &value, sizeof(value)) < 0)
                върни -errno;

        върни 0;
}

цяло getsockopt_int(цяло fd, цяло level, цяло optname, цяло *ret);

цяло socket_bind_to_ifname(цяло fd, const char *ifname);
цяло socket_bind_to_ifindex(цяло fd, цяло ifindex);

цяло socket_autobind(цяло fd, char **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(2 * sizeof(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(sizeof(struct timespec)) + CMSG_SPACE(2 * sizeof(uint64_t)))

ssize_t recvmsg_safe(цяло sockfd, struct msghdr *msg, цяло flags);

цяло socket_get_family(цяло fd);
цяло socket_set_recvpktinfo(цяло fd, цяло af, bool b);
цяло socket_set_unicast_if(цяло fd, цяло af, цяло ifi);

цяло socket_set_option(цяло fd, цяло af, цяло opt_ipv4, цяло opt_ipv6, цяло val);
static inline цяло socket_set_recverr(цяло fd, цяло af, bool b) {
        върни socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static inline цяло socket_set_recvttl(цяло fd, цяло af, bool b) {
        върни socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static inline цяло socket_set_ttl(цяло fd, цяло af, цяло ttl) {
        върни socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static inline цяло socket_set_freebind(цяло fd, цяло af, bool b) {
        върни socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static inline цяло socket_set_transparent(цяло fd, цяло af, bool b) {
        върни socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static inline цяло socket_set_recvfragsize(цяло fd, цяло af, bool b) {
        върни socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

цяло socket_get_mtu(цяло fd, цяло af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

цяло connect_unix_path(цяло fd, цяло dir_fd, const char *path);

static inline bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        върни !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

цяло vsock_parse_port(const char *s, unsigned *ret);
цяло vsock_parse_cid(const char *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
цяло socket_address_parse_unix(SocketAddress *ret_address, const char *s);
цяло socket_address_parse_vsock(SocketAddress *ret_address, const char *s);
цяло socket_address_equal_unix(const char *a, const char *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

цяло netlink_socket_get_multicast_groups(цяло fd, size_t *ret_len, uint32_t **ret_groups);

цяло socket_get_cookie(цяло fd, uint64_t *ret);

void cmsg_close_all(struct msghdr *mh);

цяло tos_to_priority(uint8_t tos);

цяло socket_xattr_supported(void);
