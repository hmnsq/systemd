/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#اگر_تعریف_نشده IPV4_MIN_MTU
#تعریف_کن IPV4_MIN_MTU 68
#پایان_اگر

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#اگر_تعریف_نشده LOOPBACK_IFINDEX
#تعریف_کن LOOPBACK_IFINDEX 1
#پایان_اگر

/* Not exposed but defined in linux/netdevice.h */
#اگر_تعریف_نشده MAX_PHYS_قلم_ID_LEN
#تعریف_کن MAX_PHYS_قلم_ID_LEN 32
#پایان_اگر

/* Not exposed but defined in include/net/bonding.h */
#اگر_تعریف_نشده BOND_MAX_ARP_TARGETS
#تعریف_کن BOND_MAX_ARP_TARGETS 16
#پایان_اگر

/* Not exposed but defined in include/linux/ieee80211.h */
#اگر_تعریف_نشده IEEE80211_MAX_SSID_LEN
#تعریف_کن IEEE80211_MAX_SSID_LEN 32
#پایان_اگر

/* Not exposed but defined in include/net/netlabel.h */
#اگر_تعریف_نشده NETLBL_NLTYPE_UNLABELED_NAME
#تعریف_کن NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#پایان_اگر

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
        NLBL_UNLABEL_A_IPV6_ماسک,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_ماسک,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
