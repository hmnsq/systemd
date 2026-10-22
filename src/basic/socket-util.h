/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <linux/if_packet.h>
#ଅନ୍ତର୍ଭାଵ <linux/netlink.h>
#ଅନ୍ତର୍ଭାଵ <linux/vm_sockets.h>
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>
#ଅନ୍ତର୍ଭାଵ <sys/un.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "missing-network.h"

ସଂଘ sockaddr_union {
        /* The minimal, abstract version */
        ସଂରଚନା sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ସଂରଚନା sockaddr_storage storage;

        /* Protocol-specific implementations */
        ସଂରଚନା sockaddr_in in;
        ସଂରଚନା sockaddr_in6 in6;
        ସଂରଚନା sockaddr_un un;
        ସଂରଚନା sockaddr_nl nl;
        ସଂରଚନା sockaddr_ll ll;
        ସଂରଚନା sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_buffer[offsetof(ସଂରଚନା sockaddr_ll, sll_addr) + HW_ADDR_MAX_SIZE];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_buffer[ଆକାର(ସଂରଚନା sockaddr_un) + 1];
};

#define SUN_PATH_LEN (ଆକାର(((ସଂରଚନା sockaddr_un){}).sun_path))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା SocketAddress {
        ସଂଘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t size;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା protocol;
} SocketAddress;

#define socket_address_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_TABLE_LOOKUP(socket_address_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_un_unlink(ଅଚଳ ସଂରଚନା sockaddr_un *sa);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_unlink(ଅଚଳ SocketAddress *a) {
        ଫେରନ୍ତୁ socket_address_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_address_can_accept(ଅଚଳ SocketAddress *a) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_verify(ଅଚଳ SocketAddress *a, bool strict) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_print(ଅଚଳ SocketAddress *a, ଅକ୍ଷର **ret);
bool socket_address_matches_fd(ଅଚଳ SocketAddress *a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

bool socket_address_equal(ଅଚଳ SocketAddress *a, ଅଚଳ SocketAddress *b) _pure_;

ଅଚଳ ଅକ୍ଷର* socket_address_get_path(ଅଚଳ SocketAddress *a);

bool socket_ipv6_is_supported(ଶୂନ୍ୟ);
bool socket_ipv6_is_enabled(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_port(ଅଚଳ ସଂରଚନା sockaddr *_sa, unsigned *port);
ଅଚଳ ସଂଘ in_addr_union *sockaddr_in_addr(ଅଚଳ ସଂରଚନା sockaddr *sa);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_set_in_addr(ସଂଘ sockaddr_union *u, ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ସଂଘ in_addr_union *a, uint16_t port);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_pretty(ଅଚଳ ସଂରଚନା sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_port, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeername_pretty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool include_port, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getsockname_pretty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socknameinfo_pretty(ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t salen, ଅକ୍ଷର **_ret);

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(netlink_family, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

bool sockaddr_equal(ଅଚଳ ସଂଘ sockaddr_union *a, ଅଚଳ ସଂଘ sockaddr_union *b);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_set_sndbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n, bool increase);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_inc_sndbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n) {
        ଫେରନ୍ତୁ fd_set_sndbuf(fd, n, true);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_set_rcvbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n, bool increase);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_increase_rxbuf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t n) {
        ଫେରନ୍ତୁ fd_set_rcvbuf(fd, n, true);
}

DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(ip_tos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(ଅକ୍ଷର a) _const_;
bool ifname_valid_full(ଅଚଳ ଅକ୍ଷର *p, IfnameValidFlags flags) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool ifname_valid(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ ifname_valid_full(p, 0);
}
bool address_label_valid(ଅଚଳ ଅକ୍ଷର *p) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeercred(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା ucred *ucred);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeersec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeergroups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, gid_t **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeerpidfd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpeerpidref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ସଂରଚନା iovec *iov, size_t iovlen,
                ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t len,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା send_one_fd_sa(ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd,
                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                   ଅଚଳ ସଂରଚନା sockaddr *sa, socklen_t len,
                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
#define send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ଖାଲି, 0, flags)
#define send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ଖାଲି, 0, ଖାଲି, 0, flags)
ssize_t receive_one_fd_iov(ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd, ସଂରଚନା iovec *iov, size_t iovlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା receive_one_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା transport_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

ssize_t next_datagram_size_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flush_accept(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ssize_t flush_mqueue(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

#define CMSG_FOREACH(cmsg, mh)                                          \
        ପାଇଁ ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#define CMSG_TYPED_DATA(cmsg, type)                                     \
        ({                                                              \
                ସଂରଚନା cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ସଂରଚନା cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ଖାଲି; \
        })

ସଂରଚନା cmsghdr* cmsg_find(ସଂରଚନା msghdr *mh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, socklen_t length);
ଶୂନ୍ୟ* cmsg_find_and_copy_data(ସଂରଚନା msghdr *mh, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଶୂନ୍ୟ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#define CMSG_FIND_DATA(mh, level, type, ctype)                          \
        CMSG_TYPED_DATA(cmsg_find(mh, level, type, CMSG_LEN(ଆକାର(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#define CMSG_FIND_AND_COPY_DATA(mh, level, type, ctype)             \
        (ctype*) cmsg_find_and_copy_data(mh, level, type, &(ctype){}, ଆକାର(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#define CMSG_BUFFER_TYPE(size)                                          \
        ସଂଘ {                                                         \
                ସଂରଚନା cmsghdr cmsghdr;                                 \
                uint8_t buf[size];                                      \
                uint8_t align_check[(size) >= CMSG_SPACE(0) &&          \
                                    (size) == CMSG_ALIGN(size) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ଅଚଳ ସଂରଚନା sockaddr_ll *sa);
size_t sockaddr_un_len(ଅଚଳ ସଂରଚନା sockaddr_un *sa);
size_t sockaddr_len(ଅଚଳ ସଂଘ sockaddr_union *sa);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_ioctl_fd(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockaddr_un_set_path(ସଂରଚନା sockaddr_un *ret, ଅଚଳ ଅକ୍ଷର *path);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା setsockopt_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା optname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) {
        ଯଦି (setsockopt(fd, level, optname, &ମୂଲ୍ୟ, ଆକାର(ମୂଲ୍ୟ)) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getsockopt_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ପୂର୍ଣ୍ଣସଂଖ୍ୟା optname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_bind_to_ifname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *ifname);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_bind_to_ifindex(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_autobind(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#define CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(ଆକାର(ସଂରଚନା timeval)) + CMSG_SPACE(2 * ଆକାର(uint64_t)))
#define CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(ଆକାର(ସଂରଚନା timespec)) + CMSG_SPACE(2 * ଆକାର(uint64_t)))

ssize_t recvmsg_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockfd, ସଂରଚନା msghdr *msg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_family(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_recvpktinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_unicast_if(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifi);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_option(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_ipv4, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_ipv6, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_recverr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_recvttl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_ttl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ttl) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_freebind(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_transparent(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_set_recvfragsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, bool b) {
        ଫେରନ୍ତୁ socket_set_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_mtu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#define UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

ପୂର୍ଣ୍ଣସଂଖ୍ୟା connect_unix_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool VSOCK_CID_IS_REGULAR(unsigned cid) {
        /* Refuse special CID values */
        ଫେରନ୍ତୁ !IN_SET(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vsock_parse_port(ଅଚଳ ଅକ୍ଷର *s, unsigned *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vsock_parse_cid(ଅଚଳ ଅକ୍ଷର *s, unsigned *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_parse_unix(SocketAddress *ret_address, ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_parse_vsock(SocketAddress *ret_address, ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_address_equal_unix(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#define SOMAXCONN_DELUXE INT_MAX

ପୂର୍ଣ୍ଣସଂଖ୍ୟା netlink_socket_get_multicast_groups(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, size_t *ret_len, uint32_t **ret_groups);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_get_cookie(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);

ଶୂନ୍ୟ cmsg_close_all(ସଂରଚନା msghdr *mh);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tos_to_priority(uint8_t tos);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_xattr_supported(ଶୂନ୍ୟ);
