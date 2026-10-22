/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <linux/if_packet.h>
#అన్తర్భావ <linux/netlink.h>
#అన్తర్భావ <linux/vm_sockets.h>
#అన్తర్భావ <netinet/in.h>
#అన్తర్భావ <sys/socket.h>
#అన్తర్భావ <sys/un.h>

#అన్తర్భావ "forward.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "missing-network.h"

సంఘం sockaddr_union {
        /* The minimal, abstract version */
        నిర్మాణం sockaddr sa;

        /* The libc provided version that allocates "enough room" for every protocol */
        నిర్మాణం sockaddr_storage storage;

        /* Protocol-specific implementations */
        నిర్మాణం sockaddr_in లోపల;
        నిర్మాణం sockaddr_in6 in6;
        నిర్మాణం sockaddr_un un;
        నిర్మాణం sockaddr_nl nl;
        నిర్మాణం sockaddr_ll ll;
        నిర్మాణం sockaddr_vm vm;

        /* Ensure there is enough space to store an arbitrary hardware address, e.g. Infiniband */
        uint8_t ll_బఫరు[offsetof(నిర్మాణం sockaddr_ll, sll_addr) + HW_ADDR_MAX_కొలత];

        /* Ensure there is enough space after the AF_UNIX sun_path for one more NUL byte, just to be sure that the path
         * component is always followed by at least one NUL byte. */
        uint8_t un_బఫరు[పరిమాణం(నిర్మాణం sockaddr_un) + 1];
};

#నిర్వచించు SUN_మార్గం_LEN (పరిమాణం(((నిర్మాణం sockaddr_un){}).sun_path))

రకనిర్వచనం నిర్మాణం Socket_చిరునామా {
        సంఘం sockaddr_union sockaddr;

        /* We store the size here explicitly due to the weird
         * sockaddr_un semantics for abstract sockets */
        socklen_t కొలత;

        /* Socket type, i.e. SOCK_STREAM, SOCK_DGRAM, ... */
        పూర్ణాంకం type;

        /* Socket protocol, IPPROTO_xxx, usually 0, except for netlink */
        పూర్ణాంకం protocol;
} Socket_చిరునామా;

#నిర్వచించు socket_చిరునామా_family(a) ((a)->sockaddr.sa.sa_family)

DECLARE_STRING_పట్టిక_LOOKUP(socket_చిరునామా_type, పూర్ణాంకం);

పూర్ణాంకం sockaddr_un_unlink(స్థిరరాశి నిర్మాణం sockaddr_un *sa);

స్థిర అంతఃవరుస పూర్ణాంకం socket_చిరునామా_unlink(స్థిరరాశి Socket_చిరునామా *a) {
        తిరిగి socket_చిరునామా_family(a) == AF_UNIX ? sockaddr_un_unlink(&a->sockaddr.un) : 0;
}

bool socket_చిరునామా_can_accept(స్థిరరాశి Socket_చిరునామా *a) శుద్ధ;

పూర్ణాంకం socket_చిరునామా_verify(స్థిరరాశి Socket_చిరునామా *a, bool strict) శుద్ధ;
పూర్ణాంకం socket_చిరునామా_print(స్థిరరాశి Socket_చిరునామా *a, అక్షరం **ret);
bool socket_చిరునామా_matches_fd(స్థిరరాశి Socket_చిరునామా *a, పూర్ణాంకం fd);

bool socket_చిరునామా_equal(స్థిరరాశి Socket_చిరునామా *a, స్థిరరాశి Socket_చిరునామా *b) శుద్ధ;

స్థిరరాశి అక్షరం* socket_చిరునామా_get_మార్గం(స్థిరరాశి Socket_చిరునామా *a);

bool socket_ipv6_is_supported(శూన్య);
bool socket_ipv6_is_enabled(శూన్య);

పూర్ణాంకం sockaddr_పోర్టు(స్థిరరాశి నిర్మాణం sockaddr *_sa, గుర్తులేని *port);
స్థిరరాశి సంఘం లోపల_addr_union *sockaddr_లోపల_addr(స్థిరరాశి నిర్మాణం sockaddr *sa);
పూర్ణాంకం sockaddr_సమితి_లోపల_addr(సంఘం sockaddr_union *u, పూర్ణాంకం family, స్థిరరాశి సంఘం లోపల_addr_union *a, uint16_t port);

పూర్ణాంకం sockaddr_pretty(స్థిరరాశి నిర్మాణం sockaddr *_sa, socklen_t salen, bool translate_ipv6, bool include_పోర్టు, అక్షరం **ret);
పూర్ణాంకం getpeername_pretty(పూర్ణాంకం fd, bool include_పోర్టు, అక్షరం **ret);
పూర్ణాంకం getsockname_pretty(పూర్ణాంకం fd, అక్షరం **ret);

పూర్ణాంకం socknameinfo_pretty(స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t salen, అక్షరం **_ret);

DECLARE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(netlink_family, పూర్ణాంకం);

bool sockaddr_equal(స్థిరరాశి సంఘం sockaddr_union *a, స్థిరరాశి సంఘం sockaddr_union *b);

పూర్ణాంకం fd_సమితి_sndbuf(పూర్ణాంకం fd, size_t n, bool increase);
స్థిర అంతఃవరుస పూర్ణాంకం fd_inc_sndbuf(పూర్ణాంకం fd, size_t n) {
        తిరిగి fd_సమితి_sndbuf(fd, n, true);
}
పూర్ణాంకం fd_సమితి_rcvbuf(పూర్ణాంకం fd, size_t n, bool increase);
స్థిర అంతఃవరుస పూర్ణాంకం fd_increase_rxbuf(పూర్ణాంకం fd, size_t n) {
        తిరిగి fd_సమితి_rcvbuf(fd, n, true);
}

DECLARE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(ip_tos, పూర్ణాంకం);

రకనిర్వచనం గణన {
        IFNAME_VALID_ALTERNATIVE = 1 << 0, /* Allow "altnames" too */
        IFNAME_VALID_NUMERIC     = 1 << 1, /* Allow decimal formatted ifindexes too */
        IFNAME_VALID_SPECIAL     = 1 << 2, /* Allow the special names "all" and "default" */
        _IFNAME_VALID_ALL        = IFNAME_VALID_ALTERNATIVE | IFNAME_VALID_NUMERIC | IFNAME_VALID_SPECIAL,
} IfnameValidFlags;
bool ifname_valid_char(అక్షరం a) _const_;
bool ifname_valid_నిండిన(స్థిరరాశి అక్షరం *p, IfnameValidFlags flags) శుద్ధ;
స్థిర అంతఃవరుస bool ifname_valid(స్థిరరాశి అక్షరం *p) {
        తిరిగి ifname_valid_నిండిన(p, 0);
}
bool చిరునామా_label_valid(స్థిరరాశి అక్షరం *p) శుద్ధ;

పూర్ణాంకం getpeercred(పూర్ణాంకం fd, నిర్మాణం ucred *ucred);
పూర్ణాంకం getpeersec(పూర్ణాంకం fd, అక్షరం **ret);
పూర్ణాంకం getpeergroups(పూర్ణాంకం fd, gid_t **ret);
పూర్ణాంకం getpeerpidfd(పూర్ణాంకం fd);
పూర్ణాంకం getpeerpidref(పూర్ణాంకం fd, PidRef *ret);

ssize_t send_one_fd_iov_sa(
                పూర్ణాంకం transport_fd,
                పూర్ణాంకం fd,
                స్థిరరాశి నిర్మాణం iovec *iov, size_t iovlen,
                స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t len,
                పూర్ణాంకం flags);
పూర్ణాంకం send_one_fd_sa(పూర్ణాంకం transport_fd,
                   పూర్ణాంకం fd,
                   స్థిరరాశి నిర్మాణం sockaddr *sa, socklen_t len,
                   పూర్ణాంకం flags);
#నిర్వచించు send_one_fd_iov(transport_fd, fd, iov, iovlen, flags) send_one_fd_iov_sa(transport_fd, fd, iov, iovlen, ఖాళీ, 0, flags)
#నిర్వచించు send_one_fd(transport_fd, fd, flags) send_one_fd_iov_sa(transport_fd, fd, ఖాళీ, 0, ఖాళీ, 0, flags)
ssize_t receive_one_fd_iov(పూర్ణాంకం transport_fd, నిర్మాణం iovec *iov, size_t iovlen, పూర్ణాంకం flags, పూర్ణాంకం *ret_fd);
పూర్ణాంకం receive_one_fd(పూర్ణాంకం transport_fd, పూర్ణాంకం flags);

ssize_t తదుపరి_datagram_కొలత_fd(పూర్ణాంకం fd);

పూర్ణాంకం flush_accept(పూర్ణాంకం fd);
ssize_t flush_mqueue(పూర్ణాంకం fd);

#నిర్వచించు CMSG_FOREACH(cmsg, mh)                                          \
        కొరకు ((cmsg) = CMSG_FIRSTHDR(mh); (cmsg); (cmsg) = CMSG_NXTHDR((mh), (cmsg)))

/* Returns the cmsghdr's data pointer, but safely cast to the specified type. Does two alignment checks: one
 * at compile time, that the requested type has a smaller or same alignment as 'struct cmsghdr', and one
 * during runtime, that the actual pointer matches the alignment too. This is supposed to catch cases such as
 * 'struct timeval' is embedded into 'struct cmsghdr' on architectures where the alignment of the former is 8
 * bytes (because of a 64-bit time_t), but of the latter is 4 bytes (because size_t is 32 bits), such as
 * riscv32. */
#నిర్వచించు CMSG_TYPED_దత్తాంశం(cmsg, type)                                     \
        ({                                                              \
                నిర్మాణం cmsghdr *_cmsg = (cmsg);                         \
                assert_cc(alignof(type) <= alignof(నిర్మాణం cmsghdr));    \
                _cmsg ? CAST_ALIGN_PTR(type, CMSG_DATA(_cmsg)) : (type*) ఖాళీ; \
        })

నిర్మాణం cmsghdr* cmsg_కనుగొను(నిర్మాణం msghdr *mh, పూర్ణాంకం level, పూర్ణాంకం type, socklen_t length);
శూన్య* cmsg_కనుగొను_and_copy_దత్తాంశం(నిర్మాణం msghdr *mh, పూర్ణాంకం level, పూర్ణాంకం type, శూన్య *buf, size_t buf_len);

/* Type-safe, dereferencing version of cmsg_find() */
#నిర్వచించు CMSG_కనుగొను_దత్తాంశం(mh, level, type, ctype)                          \
        CMSG_TYPED_దత్తాంశం(cmsg_కనుగొను(mh, level, type, CMSG_LEN(పరిమాణం(ctype))), ctype)

/* Type-safe version of cmsg_find_and_copy_data() */
#నిర్వచించు CMSG_కనుగొను_AND_COPY_దత్తాంశం(mh, level, type, ctype)             \
        (ctype*) cmsg_కనుగొను_and_copy_దత్తాంశం(mh, level, type, &(ctype){}, పరిమాణం(ctype))

/* Resolves to a type that can carry cmsghdr structures. Make sure things are properly aligned, i.e. the type
 * itself is placed properly in memory and the size is also aligned to what's appropriate for "cmsghdr"
 * structures. */
#నిర్వచించు CMSG_బఫరు_TYPE(కొలత)                                          \
        సంఘం {                                                         \
                నిర్మాణం cmsghdr cmsghdr;                                 \
                uint8_t buf[కొలత];                                      \
                uint8_t align_check[(కొలత) >= CMSG_SPACE(0) &&          \
                                    (కొలత) == CMSG_ALIGN(కొలత) ? 1 : -1]; \
        }

size_t sockaddr_ll_len(స్థిరరాశి నిర్మాణం sockaddr_ll *sa);
size_t sockaddr_un_len(స్థిరరాశి నిర్మాణం sockaddr_un *sa);
size_t sockaddr_len(స్థిరరాశి సంఘం sockaddr_union *sa);

పూర్ణాంకం socket_ioctl_fd(శూన్య);

పూర్ణాంకం sockaddr_un_సమితి_మార్గం(నిర్మాణం sockaddr_un *ret, స్థిరరాశి అక్షరం *మార్గం);

స్థిర అంతఃవరుస పూర్ణాంకం setsockopt_int(పూర్ణాంకం fd, పూర్ణాంకం level, పూర్ణాంకం optname, పూర్ణాంకం విలువ) {
        ఒకవేళ (setsockopt(fd, level, optname, &విలువ, పరిమాణం(విలువ)) < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

పూర్ణాంకం getsockopt_int(పూర్ణాంకం fd, పూర్ణాంకం level, పూర్ణాంకం optname, పూర్ణాంకం *ret);

పూర్ణాంకం socket_bind_గమ్యం_ifname(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *ifname);
పూర్ణాంకం socket_bind_గమ్యం_ifindex(పూర్ణాంకం fd, పూర్ణాంకం ifindex);

పూర్ణాంకం socket_autobind(పూర్ణాంకం fd, అక్షరం **ret_name);

/* glibc duplicates timespec/timeval on certain 32-bit arches, once in 32-bit and once in 64-bit.
 * See __convert_scm_timestamps() in glibc source code. Hence, we need additional buffer space for them
 * to prevent truncating control msg (recvmsg() MSG_CTRUNC). */
#నిర్వచించు CMSG_SPACE_TIMEVAL                                              \
        (CMSG_SPACE(పరిమాణం(నిర్మాణం timeval)) + CMSG_SPACE(2 * పరిమాణం(uint64_t)))
#నిర్వచించు CMSG_SPACE_TIMESPEC                                             \
        (CMSG_SPACE(పరిమాణం(నిర్మాణం timespec)) + CMSG_SPACE(2 * పరిమాణం(uint64_t)))

ssize_t recvmsg_సురక్షిత(పూర్ణాంకం sockfd, నిర్మాణం msghdr *msg, పూర్ణాంకం flags);

పూర్ణాంకం socket_get_family(పూర్ణాంకం fd);
పూర్ణాంకం socket_సమితి_recvpktinfo(పూర్ణాంకం fd, పూర్ణాంకం af, bool b);
పూర్ణాంకం socket_సమితి_unicast_if(పూర్ణాంకం fd, పూర్ణాంకం af, పూర్ణాంకం ifi);

పూర్ణాంకం socket_సమితి_option(పూర్ణాంకం fd, పూర్ణాంకం af, పూర్ణాంకం opt_ipv4, పూర్ణాంకం opt_ipv6, పూర్ణాంకం val);
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_recverr(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {
        తిరిగి socket_సమితి_option(fd, af, IP_RECVERR, IPV6_RECVERR, b);
}
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_recvttl(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {
        తిరిగి socket_సమితి_option(fd, af, IP_RECVTTL, IPV6_RECVHOPLIMIT, b);
}
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_ttl(పూర్ణాంకం fd, పూర్ణాంకం af, పూర్ణాంకం ttl) {
        తిరిగి socket_సమితి_option(fd, af, IP_TTL, IPV6_UNICAST_HOPS, ttl);
}
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_freebind(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {
        తిరిగి socket_సమితి_option(fd, af, IP_FREEBIND, IPV6_FREEBIND, b);
}
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_transparent(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {
        తిరిగి socket_సమితి_option(fd, af, IP_TRANSPARENT, IPV6_TRANSPARENT, b);
}
స్థిర అంతఃవరుస పూర్ణాంకం socket_సమితి_recvfragsize(పూర్ణాంకం fd, పూర్ణాంకం af, bool b) {
        తిరిగి socket_సమితి_option(fd, af, IP_RECVFRAGSIZE, IPV6_RECVFRAGSIZE, b);
}

పూర్ణాంకం socket_get_mtu(పూర్ణాంకం fd, పూర్ణాంకం af, size_t *ret);

/* an initializer for struct ucred that initialized all fields to the invalid value appropriate for each */
#నిర్వచించు UCRED_INVALID { .pid = 0, .uid = UID_INVALID, .gid = GID_INVALID }

పూర్ణాంకం connect_unix_మార్గం(పూర్ణాంకం fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం);

స్థిర అంతఃవరుస bool VSOCK_CID_IS_REGULAR(గుర్తులేని cid) {
        /* Refuse special CID values */
        తిరిగి !లోపల_సమితి(cid,
                       VMADDR_CID_HYPERVISOR,
                       VMADDR_CID_LOCAL,
                       VMADDR_CID_HOST,
                       VMADDR_CID_ANY);
}

పూర్ణాంకం vsock_parse_పోర్టు(స్థిరరాశి అక్షరం *s, గుర్తులేని *ret);
పూర్ణాంకం vsock_parse_cid(స్థిరరాశి అక్షరం *s, గుర్తులేని *ret);

/* Parses AF_UNIX and AF_VSOCK addresses. AF_INET[6] require some netlink calls, so it cannot be in
 * src/basic/ and is done from 'socket_local_address from src/shared/. Return -EPROTO in case of
 * protocol mismatch. */
పూర్ణాంకం socket_చిరునామా_parse_unix(Socket_చిరునామా *ret_చిరునామా, స్థిరరాశి అక్షరం *s);
పూర్ణాంకం socket_చిరునామా_parse_vsock(Socket_చిరునామా *ret_చిరునామా, స్థిరరాశి అక్షరం *s);
పూర్ణాంకం socket_చిరునామా_equal_unix(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b);

/* libc's SOMAXCONN is defined to 128 or 4096 (at least on glibc). But actually, the value can be much
 * larger. In our codebase we want to set it to the max usually, since nowadays socket memory is properly
 * tracked by memcg, and hence we don't need to enforce extra limits here. Moreover, the kernel caps it to
 * /proc/sys/net/core/somaxconn anyway, thus by setting this to unbounded we just make that sysctl file
 * authoritative. */
#నిర్వచించు SOMAXCONN_DELUXE గరిష్ఠ_పూర్ణాంకం

పూర్ణాంకం netlink_socket_get_multicast_groups(పూర్ణాంకం fd, size_t *ret_len, uint32_t **ret_groups);

పూర్ణాంకం socket_get_cookie(పూర్ణాంకం fd, uint64_t *ret);

శూన్య cmsg_close_all(నిర్మాణం msghdr *mh);

పూర్ణాంకం tos_గమ్యం_ప్రాధాన్యత(uint8_t tos);

పూర్ణాంకం socket_xattr_supported(శూన్య);
