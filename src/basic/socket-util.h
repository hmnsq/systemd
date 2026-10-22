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
        puur_naa_nka type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        puur_naa_nka protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, puur_naa_nka);

puur_naa_nka sockaddr_un_unlink(const sa_nracanaa sockaddr_un *sa);

static anta_hstha puur_naa_nka socket_address_unlink(const SocketAddress *a) {
        ᰀᰧᰌ socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(const SocketAddress *a) _pure_;

puur_naa_nka socket_address_verify(const SocketAddress *a, bool strict) _pure_;
puur_naa_nka socket_address_print(const SocketAddress *a, var_na **ret);
bool socket_address_matches_fd(const SocketAddress *a, puur_naa_nka fd);

bool socket_address_equal(const SocketAddress *a, const SocketAddress *b) _pure_;

const var_na* socket_address_get_path(const SocketAddress *a);

bool socket_ipv6_is_supported(void);
bool socket_ipv6_is_enabled(void);

puur_naa_nka sockaddr_port(const sa_nracanaa sockaddr *_sa, unsigned *port);
const sa_ngha in_addr_union *sockaddr_in_addr(const sa_nracanaa sockaddr *sa);
puur_naa_nka sockaddr_set_in_addr(sa_ngha sockaddr_union *u, puur_naa_nka family, const sa_ngha in_addr_union *a, uint16_t port);

puur_naa_nka sockaddr_pretty(const sa_nracanaa sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, var_na **ret);
puur_naa_nka getpeername_pretty(puur_naa_nka fd, bool include_port, var_na **ret);
puur_naa_nka getsockname_pretty(puur_naa_nka fd, var_na **ret);

puur_naa_nka socknameinfo_pretty(const sa_nracanaa sockaddr *sa, socklen_t salen, var_na **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, puur_naa_nka);

bool sockaddr_equal(const sa_ngha sockaddr_union *a, const sa_ngha sockaddr_union *b);

puur_naa_nka fd_set_sndbuf(puur_naa_nka fd, size_t n, bool increase);
static anta_hstha puur_naa_nka fd_inc_sndbuf(puur_naa_nka fd, size_t n) {
        ᰀᰧᰌ fd_set_sndbuf(fd, n, true);
}
puur_naa_nka fd_set_rcvbuf(puur_naa_nka fd, size_t n, bool increase);
static anta_hstha puur_naa_nka fd_increase_rxbuf(puur_naa_nka fd, size_t n) {
        ᰀᰧᰌ fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, puur_naa_nka);

typedef ga_nanaa {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(var_na a) _const_;
bool ifname_valid_full(const var_na *p, IfnameValidFlags flags) _pure_;
static anta_hstha bool ifname_valid(const var_na *p) {
        ᰀᰧᰌ ifname_valid_full(p, 0);
}
bool address_label_valid(const var_na *p) _pure_;

puur_naa_nka getpeercred(puur_naa_nka fd, sa_nracanaa ucred *ucred);
puur_naa_nka getpeersec(puur_naa_nka fd, var_na **ret);
puur_naa_nka getpeergroups(puur_naa_nka fd, gid_t **ret);
puur_naa_nka getpeerpidfd(puur_naa_nka fd);
puur_naa_nka getpeerpidref(puur_naa_nka fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                puur_naa_nka transport_fd,
                puur_naa_nka fd,
                const sa_nracanaa iovec *iov, size_t iovlen,
                const sa_nracanaa sockaddr *sa, socklen_t len,
                puur_naa_nka flags);
puur_naa_nka send_one_fd_sa(puur_naa_nka transport_fd,
                   puur_naa_nka fd,
                   const sa_nracanaa sockaddr *sa, socklen_t len,
                   puur_naa_nka flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, NULL, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, NULL, 0, NULL, 0, flags)
ssize_t receive_one_fd_iov(puur_naa_nka transport_fd, sa_nracanaa iovec *iov, size_t iovlen, puur_naa_nka flags, puur_naa_nka *ret_fd);
puur_naa_nka receive_one_fd(puur_naa_nka transport_fd, puur_naa_nka flags);

ssize_t next_datagram_size_fd(puur_naa_nka fd);

puur_naa_nka flush_accept(puur_naa_nka fd);
ssize_t flush_mqueue(puur_naa_nka fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        for ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

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

sa_nracanaa cmsghdr* cmsg_find(sa_nracanaa msghdr *mh, puur_naa_nka level, puur_naa_nka type, socklen_t length);
void* cmsg_find_and_copy_data(sa_nracanaa msghdr *mh, puur_naa_nka level, puur_naa_nka type, void *buf, size_t buf_len);

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

puur_naa_nka socket_ioctl_fd(void);

puur_naa_nka sockaddr_un_set_path(sa_nracanaa sockaddr_un *ret, const var_na *path);

static anta_hstha puur_naa_nka setsockopt_int(puur_naa_nka fd, puur_naa_nka level, puur_naa_nka optname, puur_naa_nka value) {
        ᰀᰧ (setsockopt(fd, level, optname, &value, parimaa_na(value)) < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ 0;
}

puur_naa_nka getsockopt_int(puur_naa_nka fd, puur_naa_nka level, puur_naa_nka optname, puur_naa_nka *ret);

puur_naa_nka socket_bind_to_ifname(puur_naa_nka fd, const var_na *ifname);
puur_naa_nka socket_bind_to_ifindex(puur_naa_nka fd, puur_naa_nka ifindex);

puur_naa_nka socket_autobind(puur_naa_nka fd, var_na **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timeval)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(parimaa_na(sa_nracanaa timespec)) + CMSG_SPACE(2 * parimaa_na(uint64_t)))

ssize_t recvmsg_safe(puur_naa_nka sockfd, sa_nracanaa msghdr *msg, puur_naa_nka flags);

puur_naa_nka socket_get_family(puur_naa_nka fd);
puur_naa_nka socket_set_recvpktinfo(puur_naa_nka fd, puur_naa_nka af, bool b);
puur_naa_nka socket_set_unicast_if(puur_naa_nka fd, puur_naa_nka af, puur_naa_nka ifi);

puur_naa_nka socket_set_option(puur_naa_nka fd, puur_naa_nka af, puur_naa_nka opt_ipv4, puur_naa_nka opt_ipv6, puur_naa_nka val);
static anta_hstha puur_naa_nka socket_set_recverr(puur_naa_nka fd, puur_naa_nka af, bool b) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
static anta_hstha puur_naa_nka socket_set_recvttl(puur_naa_nka fd, puur_naa_nka af, bool b) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
static anta_hstha puur_naa_nka socket_set_ttl(puur_naa_nka fd, puur_naa_nka af, puur_naa_nka ttl) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
static anta_hstha puur_naa_nka socket_set_freebind(puur_naa_nka fd, puur_naa_nka af, bool b) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
static anta_hstha puur_naa_nka socket_set_transparent(puur_naa_nka fd, puur_naa_nka af, bool b) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
static anta_hstha puur_naa_nka socket_set_recvfragsize(puur_naa_nka fd, puur_naa_nka af, bool b) {
        ᰀᰧᰌ socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

puur_naa_nka socket_get_mtu(puur_naa_nka fd, puur_naa_nka af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

puur_naa_nka connect_unix_path(puur_naa_nka fd, puur_naa_nka dir_fd, const var_na *path);

static anta_hstha bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ᰀᰧᰌ !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

puur_naa_nka vsock_parse_port(const var_na *s, unsigned *ret);
puur_naa_nka vsock_parse_cid(const var_na *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
puur_naa_nka socket_address_parse_unix(SocketAddress *ret_address, const var_na *s);
puur_naa_nka socket_address_parse_vsock(SocketAddress *ret_address, const var_na *s);
puur_naa_nka socket_address_equal_unix(const var_na *a, const var_na *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

puur_naa_nka netlink_socket_get_multicast_groups(puur_naa_nka fd, size_t *ret_len, uint32_t **ret_groups);

puur_naa_nka socket_get_cookie(puur_naa_nka fd, uint64_t *ret);

void cmsg_close_all(sa_nracanaa msghdr *mh);

puur_naa_nka tos_to_priority(uint8_t tos);

puur_naa_nka socket_xattr_supported(void);
