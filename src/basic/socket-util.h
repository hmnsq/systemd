/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <linux/if_packet.h>
#شامل <linux/netlink.h>
#شامل <linux/vm_sockets.h>
#شامل <netinet/in.h>
#شامل <sys/socket.h>
#شامل <sys/un.h>

#شامل "forward.h"
#شامل "memory-util.h"
#شامل "missing-network.h"

اتحاد sockaddr_union {
        /* The minimal, abstract version */
        ساخت sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        ساخت sockaddr_storage storage;

        /* Protocol-specific implementations */
        ساخت sockaddr_in اندر;
        ساخت sockaddr_in6 in6;
        ساخت sockaddr_un un;
        ساخت sockaddr_nl nl;
        ساخت sockaddr_ll ll;
        ساخت sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_بفر[offsetof(ساخت sockaddr_ll, sll_addr) + HW_ADDR_MAX_سائز];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_بفر[حجم(ساخت sockaddr_un) + 1];
};

#تعریف_کرو SUN_راستہ_LEN (حجم(((ساخت sockaddr_un){}).sun_path))

قسمتعریف ساخت Socket_پتہ {
        اتحاد sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t سائز;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        صحیح type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        صحیح protocol;
} Socket_پتہ;

#تعریف_کرو socket_پتہ_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_جدول_LOOKUP(socket_پتہ_type, صحیح);

صحیح sockaddr_un_unlink(ثابت ساخت sockaddr_un *sa);

ساکن اندرونی صحیح socket_پتہ_unlink(ثابت Socket_پتہ *a) {
        واپس socket_پتہ_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_پتہ_can_accept(ثابت Socket_پتہ *a) خالص;

صحیح socket_پتہ_verify(ثابت Socket_پتہ *a, bool strict) خالص;
صحیح socket_پتہ_print(ثابت Socket_پتہ *a, حرف **ret);
bool socket_پتہ_matches_fd(ثابت Socket_پتہ *a, صحیح fd);

bool socket_پتہ_equal(ثابت Socket_پتہ *a, ثابت Socket_پتہ *b) خالص;

ثابت حرف* socket_پتہ_get_راستہ(ثابت Socket_پتہ *a);

bool socket_ipv6_is_supported(خالی);
bool socket_ipv6_is_enabled(خالی);

صحیح sockaddr_پورٹ(ثابت ساخت sockaddr *_sa, غیرعلامتی *port);
ثابت اتحاد اندر_addr_union *sockaddr_اندر_addr(ثابت ساخت sockaddr *sa);
صحیح sockaddr_سیٹ_اندر_addr(اتحاد sockaddr_union *u, صحیح family, ثابت اتحاد اندر_addr_union *a, uint16_t port);

صحیح sockaddr_pretty(ثابت ساخت sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_پورٹ, حرف **ret);
صحیح getpeername_pretty(صحیح fd, bool include_پورٹ, حرف **ret);
صحیح getsockname_pretty(صحیح fd, حرف **ret);

صحیح socknameinfo_pretty(ثابت ساخت sockaddr *sa, socklen_t salen, حرف **_ret);

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(netlink_family, صحیح);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b);

صحیح fd_سیٹ_sndbuf(صحیح fd, size_t n, bool increase);
ساکن اندرونی صحیح fd_inc_sndbuf(صحیح fd, size_t n) {
        واپس fd_سیٹ_sndbuf(fd, n, true);
}
صحیح fd_سیٹ_rcvbuf(صحیح fd, size_t n, bool increase);
ساکن اندرونی صحیح fd_increase_rxbuf(صحیح fd, size_t n) {
        واپس fd_سیٹ_rcvbuf(fd, n, true);
}

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(ip_tos, صحیح);

قسمتعریف شمار {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(حرف a) _const_;
bool ifname_valid_بھرا(ثابت حرف *p, IfnameValidFlags flags) خالص;
ساکن اندرونی bool ifname_valid(ثابت حرف *p) {
        واپس ifname_valid_بھرا(p, 0);
}
bool پتہ_label_valid(ثابت حرف *p) خالص;

صحیح getpeercred(صحیح fd, ساخت ucred *ucred);
صحیح getpeersec(صحیح fd, حرف **ret);
صحیح getpeergroups(صحیح fd, gid_t **ret);
صحیح getpeerpidfd(صحیح fd);
صحیح getpeerpidref(صحیح fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                صحیح transport_fd,
                صحیح fd,
                ثابت ساخت iovec *iov, size_t iovlen,
                ثابت ساخت sockaddr *sa, socklen_t len,
                صحیح flags);
صحیح send_one_fd_sa(صحیح transport_fd,
                   صحیح fd,
                   ثابت ساخت sockaddr *sa, socklen_t len,
                   صحیح flags);
#تعریف_کرو send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, تہی, 0, flags)
#تعریف_کرو send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, تہی, 0, تہی, 0, flags)
ssize_t receive_one_fd_iov(صحیح transport_fd, ساخت iovec *iov, size_t iovlen, صحیح flags, صحیح *ret_fd);
صحیح receive_one_fd(صحیح transport_fd, صحیح flags);

ssize_t اگلا_datagram_سائز_fd(صحیح fd);

صحیح flush_accept(صحیح fd);
ssize_t flush_mqueue(صحیح fd);

#تعریف_کرو CMSG_FOREACH(cmsg, mh)                                          \
        برائے ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#تعریف_کرو CMSG_TYPED_ڈیٹا(cmsg, type)                                     \
        ({                                                              \
                ساخت cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(ساخت cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) تہی; \
        })

ساخت cmsghdr* cmsg_ڈھونڈو(ساخت msghdr *mh, صحیح level, صحیح type, socklen_t length);
خالی* cmsg_ڈھونڈو_and_copy_ڈیٹا(ساخت msghdr *mh, صحیح level, صحیح type, خالی *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#تعریف_کرو CMSG_ڈھونڈو_ڈیٹا(mh, level, type, ctype)                          \
        CMSG_TYPED_ڈیٹا(cmsg_ڈھونڈو(mh, level, type, CMSG_LEN(حجم(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#تعریف_کرو CMSG_ڈھونڈو_AND_COPY_ڈیٹا(mh, level, type, ctype)             \
        (ctype*) cmsg_ڈھونڈو_and_copy_ڈیٹا(mh, level, type, &(ctype){}, حجم(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#تعریف_کرو CMSG_بفر_TYPE(سائز)                                          \
        اتحاد {                                                         \
                ساخت cmsghdr cmsghdr;                                 \
                uint8_t buf[سائز];                                      \
                uint8_t align_check[(سائز) >= CMSG_SPACE(0) &&          \
                                    (سائز) == CMSG_ALIGN(سائز) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ثابت ساخت sockaddr_ll *sa);
size_t sockaddr_un_len(ثابت ساخت sockaddr_un *sa);
size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa);

صحیح socket_ioctl_fd(خالی);

صحیح sockaddr_un_سیٹ_راستہ(ساخت sockaddr_un *ret, ثابت حرف *راستہ);

ساکن اندرونی صحیح setsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح قدر) {
        اگر (setsockopt(fd, level, optname, &قدر, حجم(قدر)) < 0)
                واپس -errno;

        واپس 0;
}

صحیح getsockopt_int(صحیح fd, صحیح level, صحیح optname, صحیح *ret);

صحیح socket_bind_مقصد_ifname(صحیح fd, ثابت حرف *ifname);
صحیح socket_bind_مقصد_ifindex(صحیح fd, صحیح ifindex);

صحیح socket_autobind(صحیح fd, حرف **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#تعریف_کرو CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(حجم(ساخت timeval)) + CMSG_SPACE(2 * حجم(uint64_t)))
#تعریف_کرو CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(حجم(ساخت timespec)) + CMSG_SPACE(2 * حجم(uint64_t)))

ssize_t recvmsg_محفوظ(صحیح sockfd, ساخت msghdr *msg, صحیح flags);

صحیح socket_get_family(صحیح fd);
صحیح socket_سیٹ_recvpktinfo(صحیح fd, صحیح af, bool b);
صحیح socket_سیٹ_unicast_if(صحیح fd, صحیح af, صحیح ifi);

صحیح socket_سیٹ_option(صحیح fd, صحیح af, صحیح opt_ipv4, صحیح opt_ipv6, صحیح val);
ساکن اندرونی صحیح socket_سیٹ_recverr(صحیح fd, صحیح af, bool b) {
        واپس socket_سیٹ_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ساکن اندرونی صحیح socket_سیٹ_recvttl(صحیح fd, صحیح af, bool b) {
        واپس socket_سیٹ_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ساکن اندرونی صحیح socket_سیٹ_ttl(صحیح fd, صحیح af, صحیح ttl) {
        واپس socket_سیٹ_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ساکن اندرونی صحیح socket_سیٹ_freebind(صحیح fd, صحیح af, bool b) {
        واپس socket_سیٹ_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ساکن اندرونی صحیح socket_سیٹ_transparent(صحیح fd, صحیح af, bool b) {
        واپس socket_سیٹ_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ساکن اندرونی صحیح socket_سیٹ_recvfragsize(صحیح fd, صحیح af, bool b) {
        واپس socket_سیٹ_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

صحیح socket_get_mtu(صحیح fd, صحیح af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#تعریف_کرو UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

صحیح connect_unix_راستہ(صحیح fd, صحیح dir_fd, ثابت حرف *راستہ);

ساکن اندرونی bool VSOCK_CID_IS_REGULAR(غیرعلامتی cid) {
        /* Refuse special CID values */
        واپس !اندر_سیٹ(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

صحیح vsock_parse_پورٹ(ثابت حرف *s, غیرعلامتی *ret);
صحیح vsock_parse_cid(ثابت حرف *s, غیرعلامتی *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
صحیح socket_پتہ_parse_unix(Socket_پتہ *ret_پتہ, ثابت حرف *s);
صحیح socket_پتہ_parse_vsock(Socket_پتہ *ret_پتہ, ثابت حرف *s);
صحیح socket_پتہ_equal_unix(ثابت حرف *a, ثابت حرف *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#تعریف_کرو SOMAXCONN_DELUXE زیادہ_سے_زیادہ_عدد

صحیح netlink_socket_get_multicast_groups(صحیح fd, size_t *ret_len, uint32_t **ret_groups);

صحیح socket_get_cookie(صحیح fd, uint64_t *ret);

خالی cmsg_close_all(ساخت msghdr *mh);

صحیح tos_مقصد_ترجیح(uint8_t tos);

صحیح socket_xattr_supported(خالی);
