/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <linux/if_packet.h>
#ضمن <linux/netlink.h>
#ضمن <linux/vm_sockets.h>
#ضمن <netinet/in.h>
#ضمن <sys/socket.h>
#ضمن <sys/un.h>

#ضمن "forward.h"
#ضمن "memory-util.h"
#ضمن "missing-network.h"

اتحاد sockaddr_union {
        /* The minimal, abstract version */
        بنية sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        بنية sockaddr_storage storage;

        /* Protocol-specific implementations */
        بنية sockaddr_in درون;
        بنية sockaddr_in6 in6;
        بنية sockaddr_un un;
        بنية sockaddr_nl nl;
        بنية sockaddr_ll ll;
        بنية sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_میانگیر[offsetof(بنية sockaddr_ll, sll_addr) + HW_ADDR_MAX_اندازه];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_میانگیر[حجم(بنية sockaddr_un) + 1];
};

#تعریف_کن SUN_مسیر_LEN (حجم(((بنية sockaddr_un){}).sun_path))

تعريفنوع بنية Socket_آدرس {
        اتحاد sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t اندازه;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        صحيح type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        صحيح protocol;
} Socket_آدرس;

#تعریف_کن socket_آدرس_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_جدول_LOOKUP(socket_آدرس_type, صحيح);

صحيح sockaddr_un_unlink(ثابت بنية sockaddr_un *sa);

ساكن مضمن صحيح socket_آدرس_unlink(ثابت Socket_آدرس *a) {
        أرجع socket_آدرس_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_آدرس_can_accept(ثابت Socket_آدرس *a) خالص;

صحيح socket_آدرس_verify(ثابت Socket_آدرس *a, bool strict) خالص;
صحيح socket_آدرس_print(ثابت Socket_آدرس *a, حرف **ret);
bool socket_آدرس_matches_fd(ثابت Socket_آدرس *a, صحيح fd);

bool socket_آدرس_equal(ثابت Socket_آدرس *a, ثابت Socket_آدرس *b) خالص;

ثابت حرف* socket_آدرس_get_مسیر(ثابت Socket_آدرس *a);

bool socket_ipv6_is_supported(فراغ);
bool socket_ipv6_is_enabled(فراغ);

صحيح sockaddr_پورت(ثابت بنية sockaddr *_sa, بیعلامت *port);
ثابت اتحاد درون_addr_union *sockaddr_درون_addr(ثابت بنية sockaddr *sa);
صحيح sockaddr_مجموعه_درون_addr(اتحاد sockaddr_union *u, صحيح family, ثابت اتحاد درون_addr_union *a, uint16_t port);

صحيح sockaddr_pretty(ثابت بنية sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_پورت, حرف **ret);
صحيح getpeername_pretty(صحيح fd, bool include_پورت, حرف **ret);
صحيح getsockname_pretty(صحيح fd, حرف **ret);

صحيح socknameinfo_pretty(ثابت بنية sockaddr *sa, socklen_t salen, حرف **_ret);

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(netlink_family, صحيح);

bool sockaddr_equal(ثابت اتحاد sockaddr_union *a, ثابت اتحاد sockaddr_union *b);

صحيح fd_مجموعه_sndbuf(صحيح fd, size_t n, bool increase);
ساكن مضمن صحيح fd_inc_sndbuf(صحيح fd, size_t n) {
        أرجع fd_مجموعه_sndbuf(fd, n, true);
}
صحيح fd_مجموعه_rcvbuf(صحيح fd, size_t n, bool increase);
ساكن مضمن صحيح fd_increase_rxbuf(صحيح fd, size_t n) {
        أرجع fd_مجموعه_rcvbuf(fd, n, true);
}

DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(ip_tos, صحيح);

تعريفنوع تعداد {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(حرف a) _const_;
bool ifname_valid_پر(ثابت حرف *p, IfnameValidFlags flags) خالص;
ساكن مضمن bool ifname_valid(ثابت حرف *p) {
        أرجع ifname_valid_پر(p, 0);
}
bool آدرس_label_valid(ثابت حرف *p) خالص;

صحيح getpeercred(صحيح fd, بنية ucred *ucred);
صحيح getpeersec(صحيح fd, حرف **ret);
صحيح getpeergroups(صحيح fd, gid_t **ret);
صحيح getpeerpidfd(صحيح fd);
صحيح getpeerpidref(صحيح fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                صحيح transport_fd,
                صحيح fd,
                ثابت بنية iovec *iov, size_t iovlen,
                ثابت بنية sockaddr *sa, socklen_t len,
                صحيح flags);
صحيح send_one_fd_sa(صحيح transport_fd,
                   صحيح fd,
                   ثابت بنية sockaddr *sa, socklen_t len,
                   صحيح flags);
#تعریف_کن send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, عدم, 0, flags)
#تعریف_کن send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, عدم, 0, عدم, 0, flags)
ssize_t receive_one_fd_iov(صحيح transport_fd, بنية iovec *iov, size_t iovlen, صحيح flags, صحيح *ret_fd);
صحيح receive_one_fd(صحيح transport_fd, صحيح flags);

ssize_t بعدی_datagram_اندازه_fd(صحيح fd);

صحيح flush_accept(صحيح fd);
ssize_t flush_mqueue(صحيح fd);

#تعریف_کن CMSG_FOREACH(cmsg, mh)                                          \
        لكل ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#تعریف_کن CMSG_TYPED_داده(cmsg, type)                                     \
        ({                                                              \
                بنية cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(بنية cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) عدم; \
        })

بنية cmsghdr* cmsg_پیدا_کن(بنية msghdr *mh, صحيح level, صحيح type, socklen_t length);
فراغ* cmsg_پیدا_کن_and_copy_داده(بنية msghdr *mh, صحيح level, صحيح type, فراغ *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#تعریف_کن CMSG_پیدا_کن_داده(mh, level, type, ctype)                          \
        CMSG_TYPED_داده(cmsg_پیدا_کن(mh, level, type, CMSG_LEN(حجم(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#تعریف_کن CMSG_پیدا_کن_AND_COPY_داده(mh, level, type, ctype)             \
        (ctype*) cmsg_پیدا_کن_and_copy_داده(mh, level, type, &(ctype){}, حجم(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#تعریف_کن CMSG_میانگیر_TYPE(اندازه)                                          \
        اتحاد {                                                         \
                بنية cmsghdr cmsghdr;                                 \
                uint8_t buf[اندازه];                                      \
                uint8_t align_check[(اندازه) >= CMSG_SPACE(0) &&          \
                                    (اندازه) == CMSG_ALIGN(اندازه) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(ثابت بنية sockaddr_ll *sa);
size_t sockaddr_un_len(ثابت بنية sockaddr_un *sa);
size_t sockaddr_len(ثابت اتحاد sockaddr_union *sa);

صحيح socket_ioctl_fd(فراغ);

صحيح sockaddr_un_مجموعه_مسیر(بنية sockaddr_un *ret, ثابت حرف *مسیر);

ساكن مضمن صحيح setsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح مقدار) {
        إذا (setsockopt(fd, level, optname, &مقدار, حجم(مقدار)) < 0)
                أرجع -errno;

        أرجع 0;
}

صحيح getsockopt_int(صحيح fd, صحيح level, صحيح optname, صحيح *ret);

صحيح socket_bind_مقصد_ifname(صحيح fd, ثابت حرف *ifname);
صحيح socket_bind_مقصد_ifindex(صحيح fd, صحيح ifindex);

صحيح socket_autobind(صحيح fd, حرف **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#تعریف_کن CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(حجم(بنية timeval)) + CMSG_SPACE(2 * حجم(uint64_t)))
#تعریف_کن CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(حجم(بنية timespec)) + CMSG_SPACE(2 * حجم(uint64_t)))

ssize_t recvmsg_امن(صحيح sockfd, بنية msghdr *msg, صحيح flags);

صحيح socket_get_family(صحيح fd);
صحيح socket_مجموعه_recvpktinfo(صحيح fd, صحيح af, bool b);
صحيح socket_مجموعه_unicast_if(صحيح fd, صحيح af, صحيح ifi);

صحيح socket_مجموعه_option(صحيح fd, صحيح af, صحيح opt_ipv4, صحيح opt_ipv6, صحيح val);
ساكن مضمن صحيح socket_مجموعه_recverr(صحيح fd, صحيح af, bool b) {
        أرجع socket_مجموعه_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
ساكن مضمن صحيح socket_مجموعه_recvttl(صحيح fd, صحيح af, bool b) {
        أرجع socket_مجموعه_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
ساكن مضمن صحيح socket_مجموعه_ttl(صحيح fd, صحيح af, صحيح ttl) {
        أرجع socket_مجموعه_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
ساكن مضمن صحيح socket_مجموعه_freebind(صحيح fd, صحيح af, bool b) {
        أرجع socket_مجموعه_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
ساكن مضمن صحيح socket_مجموعه_transparent(صحيح fd, صحيح af, bool b) {
        أرجع socket_مجموعه_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
ساكن مضمن صحيح socket_مجموعه_recvfragsize(صحيح fd, صحيح af, bool b) {
        أرجع socket_مجموعه_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

صحيح socket_get_mtu(صحيح fd, صحيح af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#تعریف_کن UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

صحيح connect_unix_مسیر(صحيح fd, صحيح dir_fd, ثابت حرف *مسیر);

ساكن مضمن bool VSOCK_CID_IS_REGULAR(بیعلامت cid) {
        /* Refuse special CID values */
        أرجع !درون_مجموعه(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

صحيح vsock_parse_پورت(ثابت حرف *s, بیعلامت *ret);
صحيح vsock_parse_cid(ثابت حرف *s, بیعلامت *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
صحيح socket_آدرس_parse_unix(Socket_آدرس *ret_آدرس, ثابت حرف *s);
صحيح socket_آدرس_parse_vsock(Socket_آدرس *ret_آدرس, ثابت حرف *s);
صحيح socket_آدرس_equal_unix(ثابت حرف *a, ثابت حرف *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#تعریف_کن SOMAXCONN_DELUXE بیشترین_صحیح

صحيح netlink_socket_get_multicast_groups(صحيح fd, size_t *ret_len, uint32_t **ret_groups);

صحيح socket_get_cookie(صحيح fd, uint64_t *ret);

فراغ cmsg_close_all(بنية msghdr *mh);

صحيح tos_مقصد_اولویت(uint8_t tos);

صحيح socket_xattr_supported(فراغ);
