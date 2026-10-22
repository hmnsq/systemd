/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/if_packet.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/netlink.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/vm_sockets.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/un.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "missing-network.h"

ਸੰਘ sockaddr_union {
        /* The minimal, abstract version */
        ਬਣਤਰ sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ਬਣਤਰ sockaddr_storage storage;

        /* Protocol-specific implementations */
        ਬਣਤਰ sockaddr_in ਵਿੱਚ;
        ਬਣਤਰ sockaddr_in6 in6;
        ਬਣਤਰ sockaddr_un un;
        ਬਣਤਰ sockaddr_nl nl;
        ਬਣਤਰ sockaddr_ll ll;
        ਬਣਤਰ sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_ਬਫਰ[offsetof(ਬਣਤਰ sockaddr_ll, sll_addr) + HW_ADDR_MAX_ਮਾਪ];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_ਬਫਰ[ਆਕਾਰ(ਬਣਤਰ sockaddr_un) + 1];
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUN_ਰਸਤਾ_LEN (ਆਕਾਰ(((ਬਣਤਰ sockaddr_un){}).sun_path))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Socket_ਪਤਾ {
        ਸੰਘ sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t ਮਾਪ;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        ਪੂਰਨ type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        ਪੂਰਨ protocol;
} Socket_ਪਤਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ socket_ਪਤਾ_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(socket_ਪਤਾ_type, ਪੂਰਨ);

ਪੂਰਨ sockaddr_un_unlink(ਅਚਲ ਬਣਤਰ sockaddr_un *sa);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਪਤਾ_unlink(ਅਚਲ Socket_ਪਤਾ *a) {
        ਵਾਪਸ socket_ਪਤਾ_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_ਪਤਾ_can_accept(ਅਚਲ Socket_ਪਤਾ *a) ਸ਼ੁੱਧ;

ਪੂਰਨ socket_ਪਤਾ_verify(ਅਚਲ Socket_ਪਤਾ *a, bool strict) ਸ਼ੁੱਧ;
ਪੂਰਨ socket_ਪਤਾ_print(ਅਚਲ Socket_ਪਤਾ *a, ਅੱਖਰ **ret);
bool socket_ਪਤਾ_matches_fd(ਅਚਲ Socket_ਪਤਾ *a, ਪੂਰਨ fd);

bool socket_ਪਤਾ_equal(ਅਚਲ Socket_ਪਤਾ *a, ਅਚਲ Socket_ਪਤਾ *b) ਸ਼ੁੱਧ;

ਅਚਲ ਅੱਖਰ* socket_ਪਤਾ_get_ਰਸਤਾ(ਅਚਲ Socket_ਪਤਾ *a);

bool socket_ipv6_is_supported(ਖਾਲੀ);
bool socket_ipv6_is_enabled(ਖਾਲੀ);

ਪੂਰਨ sockaddr_ਪੋਰਟ(ਅਚਲ ਬਣਤਰ sockaddr *_sa, ਬਿਨਾਨਿਸ਼ਾਨੀ *port);
ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *sockaddr_ਵਿੱਚ_addr(ਅਚਲ ਬਣਤਰ sockaddr *sa);
ਪੂਰਨ sockaddr_ਸੈੱਟ_ਵਿੱਚ_addr(ਸੰਘ sockaddr_union *u, ਪੂਰਨ family, ਅਚਲ ਸੰਘ ਵਿੱਚ_addr_union *a, uint16_t port);

ਪੂਰਨ sockaddr_pretty(ਅਚਲ ਬਣਤਰ sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_ਪੋਰਟ, ਅੱਖਰ **ret);
ਪੂਰਨ getpeername_pretty(ਪੂਰਨ fd, bool include_ਪੋਰਟ, ਅੱਖਰ **ret);
ਪੂਰਨ getsockname_pretty(ਪੂਰਨ fd, ਅੱਖਰ **ret);

ਪੂਰਨ socknameinfo_pretty(ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t salen, ਅੱਖਰ **_ret);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(netlink_family, ਪੂਰਨ);

bool sockaddr_equal(ਅਚਲ ਸੰਘ sockaddr_union *a, ਅਚਲ ਸੰਘ sockaddr_union *b);

ਪੂਰਨ fd_ਸੈੱਟ_sndbuf(ਪੂਰਨ fd, size_t n, bool increase);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_inc_sndbuf(ਪੂਰਨ fd, size_t n) {
        ਵਾਪਸ fd_ਸੈੱਟ_sndbuf(fd, n, true);
}
ਪੂਰਨ fd_ਸੈੱਟ_rcvbuf(ਪੂਰਨ fd, size_t n, bool increase);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_increase_rxbuf(ਪੂਰਨ fd, size_t n) {
        ਵਾਪਸ fd_ਸੈੱਟ_rcvbuf(fd, n, true);
}

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(ip_tos, ਪੂਰਨ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(ਅੱਖਰ a) _const_;
bool ifname_valid_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, IfnameValidFlags flags) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ifname_valid(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ ifname_valid_ਭਰਿਆ(p, 0);
}
bool ਪਤਾ_label_valid(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;

ਪੂਰਨ getpeercred(ਪੂਰਨ fd, ਬਣਤਰ ucred *ucred);
ਪੂਰਨ getpeersec(ਪੂਰਨ fd, ਅੱਖਰ **ret);
ਪੂਰਨ getpeergroups(ਪੂਰਨ fd, gid_t **ret);
ਪੂਰਨ getpeerpidfd(ਪੂਰਨ fd);
ਪੂਰਨ getpeerpidref(ਪੂਰਨ fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                ਪੂਰਨ transport_fd,
                ਪੂਰਨ fd,
                ਅਚਲ ਬਣਤਰ iovec *iov, size_t iovlen,
                ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t len,
                ਪੂਰਨ flags);
ਪੂਰਨ send_one_fd_sa(ਪੂਰਨ transport_fd,
                   ਪੂਰਨ fd,
                   ਅਚਲ ਬਣਤਰ sockaddr *sa, socklen_t len,
                   ਪੂਰਨ flags);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ਨਿਰਾ, 0, flags)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ਨਿਰਾ, 0, ਨਿਰਾ, 0, flags)
ssize_t receive_one_fd_iov(ਪੂਰਨ transport_fd, ਬਣਤਰ iovec *iov, size_t iovlen, ਪੂਰਨ flags, ਪੂਰਨ *ret_fd);
ਪੂਰਨ receive_one_fd(ਪੂਰਨ transport_fd, ਪੂਰਨ flags);

ssize_t ਅਗਲਾ_datagram_ਮਾਪ_fd(ਪੂਰਨ fd);

ਪੂਰਨ flush_accept(ਪੂਰਨ fd);
ssize_t flush_mqueue(ਪੂਰਨ fd);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_FOREACH(cmsg, mh)                                          \
        ਲਈ ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_TYPED_ਅੰਕੜੇ(cmsg, type)                                     \
        ({                                                              \
                ਬਣਤਰ cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ਬਣਤਰ cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ਨਿਰਾ; \
        })

ਬਣਤਰ cmsghdr* cmsg_ਲੱਭੋ(ਬਣਤਰ msghdr *mh, ਪੂਰਨ level, ਪੂਰਨ type, socklen_t length);
ਖਾਲੀ* cmsg_ਲੱਭੋ_and_copy_ਅੰਕੜੇ(ਬਣਤਰ msghdr *mh, ਪੂਰਨ level, ਪੂਰਨ type, ਖਾਲੀ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਲੱਭੋ_ਅੰਕੜੇ(mh, level, type, ctype)                          \
        CMSG_TYPED_ਅੰਕੜੇ(cmsg_ਲੱਭੋ(mh, level, type, CMSG_LEN(ਆਕਾਰ(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਲੱਭੋ_AND_COPY_ਅੰਕੜੇ(mh, level, type, ctype)             \
        (ctype*) cmsg_ਲੱਭੋ_and_copy_ਅੰਕੜੇ(mh, level, type, &(ctype){}, ਆਕਾਰ(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_ਬਫਰ_TYPE(ਮਾਪ)                                          \
        ਸੰਘ {                                                         \
                ਬਣਤਰ cmsghdr cmsghdr;                                 \
                uint8_t buf[ਮਾਪ];                                      \
                uint8_t align_check[(ਮਾਪ) >= CMSG_SPACE(0) &&          \
                                    (ਮਾਪ) == CMSG_ALIGN(ਮਾਪ) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ਅਚਲ ਬਣਤਰ sockaddr_ll *sa);
size_t sockaddr_un_len(ਅਚਲ ਬਣਤਰ sockaddr_un *sa);
size_t sockaddr_len(ਅਚਲ ਸੰਘ sockaddr_union *sa);

ਪੂਰਨ socket_ioctl_fd(ਖਾਲੀ);

ਪੂਰਨ sockaddr_un_ਸੈੱਟ_ਰਸਤਾ(ਬਣਤਰ sockaddr_un *ret, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ setsockopt_int(ਪੂਰਨ fd, ਪੂਰਨ level, ਪੂਰਨ optname, ਪੂਰਨ ਮੁੱਲ) {
        ਜੇ (setsockopt(fd, level, optname, &ਮੁੱਲ, ਆਕਾਰ(ਮੁੱਲ)) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਪੂਰਨ getsockopt_int(ਪੂਰਨ fd, ਪੂਰਨ level, ਪੂਰਨ optname, ਪੂਰਨ *ret);

ਪੂਰਨ socket_bind_ਮੰਜ਼ਿਲ_ifname(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ifname);
ਪੂਰਨ socket_bind_ਮੰਜ਼ਿਲ_ifindex(ਪੂਰਨ fd, ਪੂਰਨ ifindex);

ਪੂਰਨ socket_autobind(ਪੂਰਨ fd, ਅੱਖਰ **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(ਆਕਾਰ(ਬਣਤਰ timeval)) + CMSG_SPACE(2 * ਆਕਾਰ(uint64_t)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(ਆਕਾਰ(ਬਣਤਰ timespec)) + CMSG_SPACE(2 * ਆਕਾਰ(uint64_t)))

ssize_t recvmsg_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ sockfd, ਬਣਤਰ msghdr *msg, ਪੂਰਨ flags);

ਪੂਰਨ socket_get_family(ਪੂਰਨ fd);
ਪੂਰਨ socket_ਸੈੱਟ_recvpktinfo(ਪੂਰਨ fd, ਪੂਰਨ af, bool b);
ਪੂਰਨ socket_ਸੈੱਟ_unicast_if(ਪੂਰਨ fd, ਪੂਰਨ af, ਪੂਰਨ ifi);

ਪੂਰਨ socket_ਸੈੱਟ_option(ਪੂਰਨ fd, ਪੂਰਨ af, ਪੂਰਨ opt_ipv4, ਪੂਰਨ opt_ipv6, ਪੂਰਨ val);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_recverr(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_recvttl(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_ttl(ਪੂਰਨ fd, ਪੂਰਨ af, ਪੂਰਨ ttl) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_freebind(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_transparent(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ socket_ਸੈੱਟ_recvfragsize(ਪੂਰਨ fd, ਪੂਰਨ af, bool b) {
        ਵਾਪਸ socket_ਸੈੱਟ_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

ਪੂਰਨ socket_get_mtu(ਪੂਰਨ fd, ਪੂਰਨ af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

ਪੂਰਨ connect_unix_ਰਸਤਾ(ਪੂਰਨ fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool VSOCK_CID_IS_REGULAR(ਬਿਨਾਨਿਸ਼ਾਨੀ cid) {
        /* Refuse special CID values */
        ਵਾਪਸ !ਵਿੱਚ_ਸੈੱਟ(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

ਪੂਰਨ vsock_parse_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret);
ਪੂਰਨ vsock_parse_cid(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
ਪੂਰਨ socket_ਪਤਾ_parse_unix(Socket_ਪਤਾ *ret_ਪਤਾ, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ socket_ਪਤਾ_parse_vsock(Socket_ਪਤਾ *ret_ਪਤਾ, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ socket_ਪਤਾ_equal_unix(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SOMAXCONN_DELUXE ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ

ਪੂਰਨ netlink_socket_get_multicast_groups(ਪੂਰਨ fd, size_t *ret_len, uint32_t **ret_groups);

ਪੂਰਨ socket_get_cookie(ਪੂਰਨ fd, uint64_t *ret);

ਖਾਲੀ cmsg_close_all(ਬਣਤਰ msghdr *mh);

ਪੂਰਨ tos_ਮੰਜ਼ਿਲ_ਤਰਜੀਹ(uint8_t tos);

ਪੂਰਨ socket_xattr_supported(ਖਾਲੀ);
