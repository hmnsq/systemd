/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IPV4_MIN_MTU
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPV4_MIN_MTU 68
#ਜੇ_ਅੰਤ

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOOPBACK_IFINDEX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOOPBACK_IFINDEX 1
#ਜੇ_ਅੰਤ

/* Not exposed but defined in linux/netdevice.h */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MAX_PHYS_ਵਸਤੂ_ID_LEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PHYS_ਵਸਤੂ_ID_LEN 32
#ਜੇ_ਅੰਤ

/* Not exposed but defined in include/net/bonding.h */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BOND_MAX_ARP_TARGETS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOND_MAX_ARP_TARGETS 16
#ਜੇ_ਅੰਤ

/* Not exposed but defined in include/linux/ieee80211.h */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IEEE80211_MAX_SSID_LEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IEEE80211_MAX_SSID_LEN 32
#ਜੇ_ਅੰਤ

/* Not exposed but defined in include/net/netlabel.h */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NETLBL_NLTYPE_UNLABELED_NAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#ਜੇ_ਅੰਤ

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
ਗਿਣਤੀ {
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
ਗਿਣਤੀ {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6_ਨਕਾਬ,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_ਨਕਾਬ,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
