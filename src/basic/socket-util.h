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
        පූර්ණ_සංඛ්‍යා type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        පූර්ණ_සංඛ්‍යා protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, පූර්ණ_සංඛ්‍යා);

පූර්ණ_සංඛ්‍යා sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa);

static anta_hstha පූර්ණ_සංඛ්‍යා socket_address_unlink(const SocketAddress *a) {
        ආපසු socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

පූර්ණ_සංඛ්‍යා socket_address_verify(const SocketAddress *a, bool strict) _pure_;
පූර්ණ_සංඛ්‍යා socket_address_print(const SocketAddress *a, var_na **ret);
bool socket_address_matches_fd(const SocketAddress *a, පූර්ණ_සංඛ්‍යා fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const var_na* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

පූර්ණ_සංඛ්‍යා sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *port);
const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *sa);
පූර්ණ_සංඛ්‍යා sockaddr_set_in_addr(sa_ngha sockaddr_union *u, පූර්ණ_සංඛ්‍යා family, const sa_ngha in_addr_union *a, uint16_t port);

පූර්ණ_සංඛ්‍යා sockaddr_pretty(const sa_nracanaa sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, var_na **ret);
පූර්ණ_සංඛ්‍යා getpeername_pretty(පූර්ණ_සංඛ්‍යා fd, bool include_port, var_na **ret);
පූර්ණ_සංඛ්‍යා getsockname_pretty(පූර්ණ_සංඛ්‍යා fd, var_na **ret);

පූර්ණ_සංඛ්‍යා socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, පූර්ණ_සංඛ්‍යා);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b);

පූර්ණ_සංඛ්‍යා fd_set_sndbuf(පූර්ණ_සංඛ්‍යා fd, size_t n, bool increase);
static anta_hstha පූර්ණ_සංඛ්‍යා fd_inc_sndbuf(පූර්ණ_සංඛ්‍යා fd, size_t n) {
        ආපසු fd_set_sndbuf(fd, n, true);
}
පූර්ණ_සංඛ්‍යා fd_set_rcvbuf(පූර්ණ_සංඛ්‍යා fd, size_t n, bool increase);
static anta_hstha පූර්ණ_සංඛ්‍යා fd_increase_rxbuf(පූර්ණ_සංඛ්‍යා fd, size_t n) {
        ආපසු fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, පූර්ණ_සංඛ්‍යා);

typedef ga_nanaa {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(var_na a) _const_;
bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) _pure_;
static anta_hstha bool ifname_valid(const var_na *p) {
        ආපසු ifname_valid_full(p, 0);
}
bool address_label_valid(const var_na *p) _pure_;

පූර්ණ_සංඛ්‍යා getpeercred(පූර්ණ_සංඛ්‍යා fd, sa_nracanaa ucred *ucred);
පූර්ණ_සංඛ්‍යා getpeersec(පූර්ණ_සංඛ්‍යා fd, var_na **ret);
පූර්ණ_සංඛ්‍යා getpeergroups(පූර්ණ_සංඛ්‍යා fd, gid_t **ret);
පූර්ණ_සංඛ්‍යා getpeerpidfd(පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා getpeerpidref(පූර්ණ_සංඛ්‍යා fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                පූර්ණ_සංඛ්‍යා transport_fd,
                පූර්ණ_සංඛ්‍යා fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                පූර්ණ_සංඛ්‍යා flags);
පූර්ණ_සංඛ්‍යා send_one_fd_sa(පූර්ණ_සංඛ්‍යා transport_fd,
                   පූර්ණ_සංඛ්‍යා fd,
                   const sa_nracanaa sockaddr *sa, socklen_t len,
                   පූර්ණ_සංඛ්‍යා flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(පූර්ණ_සංඛ්‍යා transport_fd, sa_nracanaa iovec *iov, size_t iovlen, පූර්ණ_සංඛ්‍යා flags, පූර්ණ_සංඛ්‍යා *ret_fd);
පූර්ණ_සංඛ්‍යා receive_one_fd(පූර්ණ_සංඛ්‍යා transport_fd, පූර්ණ_සංඛ්‍යා flags);

ssize_t next_datagram_size_fd(පූර්ණ_සංඛ්‍යා fd);

පූර්ණ_සංඛ්‍යා flush_accept(පූර්ණ_සංඛ්‍යා fd);
ssize_t flush_mqueue(පූර්ණ_සංඛ්‍යා fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        සඳහා ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

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

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා type, socklen_t length);
void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා type, void *buf, size_t buf_len);

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

පූර්ණ_සංඛ්‍යා socket_ioctl_fd(void);

පූර්ණ_සංඛ්‍යා sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path);

static anta_hstha පූර්ණ_සංඛ්‍යා setsockopt_int(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා optname, පූර්ණ_සංඛ්‍යා value) {
        නම් (setsockopt(fd, level, optname, &value, parimaa_na(value)) < 0)
                ආපසු -errno;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා getsockopt_int(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා level, පූර්ණ_සංඛ්‍යා optname, පූර්ණ_සංඛ්‍යා *ret);

පූර්ණ_සංඛ්‍යා socket_bind_to_ifname(පූර්ණ_සංඛ්‍යා fd, const var_na *ifname);
පූර්ණ_සංඛ්‍යා socket_bind_to_ifindex(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා ifindex);

පූර්ණ_සංඛ්‍යා socket_autobind(පූර්ණ_සංඛ්‍යා fd, var_na **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timeval)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timespec)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))

ssize_t recvmsg_safe(පූර්ණ_සංඛ්‍යා sockfd, sa_nracanaa msghdr *msg, පූර්ණ_සංඛ්‍යා flags);

පූර්ණ_සංඛ්‍යා socket_get_family(පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා socket_set_recvpktinfo(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b);
පූර්ණ_සංඛ්‍යා socket_set_unicast_if(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, පූර්ණ_සංඛ්‍යා ifi);

පූර්ණ_සංඛ්‍යා socket_set_option(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, පූර්ණ_සංඛ්‍යා opt_ipv4, පූර්ණ_සංඛ්‍යා opt_ipv6, පූර්ණ_සංඛ්‍යා val);
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_recverr(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b) {
        ආපසු socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_recvttl(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b) {
        ආපසු socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_ttl(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, පූර්ණ_සංඛ්‍යා ttl) {
        ආපසු socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_freebind(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b) {
        ආපසු socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_transparent(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b) {
        ආපසු socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static anta_hstha පූර්ණ_සංඛ්‍යා socket_set_recvfragsize(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, bool b) {
        ආපසු socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

පූර්ණ_සංඛ්‍යා socket_get_mtu(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

පූර්ණ_සංඛ්‍යා connect_unix_path(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path);

static anta_hstha bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ආපසු !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

පූර්ණ_සංඛ්‍යා vsock_parse_port(const var_na *s, unsigned *ret);
පූර්ණ_සංඛ්‍යා vsock_parse_cid(const var_na *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
පූර්ණ_සංඛ්‍යා socket_address_parse_unix(SocketAddress *ret_address, const var_na *s);
පූර්ණ_සංඛ්‍යා socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s);
පූර්ණ_සංඛ්‍යා socket_address_equal_unix(const var_na *a, const var_na *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

පූර්ණ_සංඛ්‍යා netlink_socket_get_multicast_groups(පූර්ණ_සංඛ්‍යා fd, size_t *ret_len, uint32_t **ret_groups);

පූර්ණ_සංඛ්‍යා socket_get_cookie(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret);

void cmsg_close_all(sa_nracanaa msghdr *mh);

පූර්ණ_සංඛ්‍යා tos_to_priority(uint8_t tos);

පූර්ණ_සංඛ්‍යා socket_xattr_supported(void);
