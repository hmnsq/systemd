/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#اگر_غیر_تعریف_شدہ IPV4_MIN_MTU
#تعریف_کرو IPV4_MIN_MTU 68
#اگر_ختم

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#اگر_غیر_تعریف_شدہ LOOPBACK_IFINDEX
#تعریف_کرو LOOPBACK_IFINDEX 1
#اگر_ختم

/* Not exposed but defined in linux/netdevice.h */
#اگر_غیر_تعریف_شدہ MAX_PHYS_شے_ID_LEN
#تعریف_کرو MAX_PHYS_شے_ID_LEN 32
#اگر_ختم

/* Not exposed but defined in include/net/bonding.h */
#اگر_غیر_تعریف_شدہ BOND_MAX_ARP_TARGETS
#تعریف_کرو BOND_MAX_ARP_TARGETS 16
#اگر_ختم

/* Not exposed but defined in include/linux/ieee80211.h */
#اگر_غیر_تعریف_شدہ IEEE80211_MAX_SSID_LEN
#تعریف_کرو IEEE80211_MAX_SSID_LEN 32
#اگر_ختم

/* Not exposed but defined in include/net/netlabel.h */
#اگر_غیر_تعریف_شدہ NETLBL_NLTYPE_UNLABELED_NAME
#تعریف_کرو NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#اگر_ختم

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
شمار {
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
شمار {
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
