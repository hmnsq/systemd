/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#جيڪڏهن_وصف_نه_ٿيل IPV4_MIN_MTU
#وصف_ڪريو IPV4_MIN_MTU 68
#جيڪڏهن_پورو

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#جيڪڏهن_وصف_نه_ٿيل LOOPBACK_IFINDEX
#وصف_ڪريو LOOPBACK_IFINDEX 1
#جيڪڏهن_پورو

/* Not exposed but defined in linux/netdevice.h */
#جيڪڏهن_وصف_نه_ٿيل MAX_PHYS_شي_ID_LEN
#وصف_ڪريو MAX_PHYS_شي_ID_LEN 32
#جيڪڏهن_پورو

/* Not exposed but defined in include/net/bonding.h */
#جيڪڏهن_وصف_نه_ٿيل BOND_MAX_ARP_TARGETS
#وصف_ڪريو BOND_MAX_ARP_TARGETS 16
#جيڪڏهن_پورو

/* Not exposed but defined in include/linux/ieee80211.h */
#جيڪڏهن_وصف_نه_ٿيل IEEE80211_MAX_SSID_LEN
#وصف_ڪريو IEEE80211_MAX_SSID_LEN 32
#جيڪڏهن_پورو

/* Not exposed but defined in include/net/netlabel.h */
#جيڪڏهن_وصف_نه_ٿيل NETLBL_NLTYPE_UNLABELED_NAME
#وصف_ڪريو NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#جيڪڏهن_پورو

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
تعداد {
        NLBL_UNLABEL_C_UNSPEC,
        NLBL_UNLABEL_C_ACCEPT,
        NLBL_UNLABEL_C_LIST,
        NLBL_UNLABEL_C_STATICADD,
        NLBL_UNLABEL_C_STATICREMOVE,
        NLBL_UNLABEL_C_STATICLIST,
        NLBL_UNLABEL_C_STATICADDDEF,
        NLBL_UNLABEL_C_STATICREMOVEDEF,
        NLBL_UNLABEL_C_STATICLISTDEF,
        __NLBL_UNLABEL_C_MAX,
};

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
تعداد {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6_نقاب,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_نقاب,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
