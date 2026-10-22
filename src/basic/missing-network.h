/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IPV4_MIN_MTU
#ವ್ಯಾಖ್ಯಾನಿಸು IPV4_MIN_MTU 68
#ಆದರೆ_ಅಂತ್ಯ

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOOPBACK_IFINDEX
#ವ್ಯಾಖ್ಯಾನಿಸು LOOPBACK_IFINDEX 1
#ಆದರೆ_ಅಂತ್ಯ

/* Not exposed but defined in linux/netdevice.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MAX_PHYS_ಅಂಶ_ID_LEN
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PHYS_ಅಂಶ_ID_LEN 32
#ಆದರೆ_ಅಂತ್ಯ

/* Not exposed but defined in include/net/bonding.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BOND_MAX_ARP_TARGETS
#ವ್ಯಾಖ್ಯಾನಿಸು BOND_MAX_ARP_TARGETS 16
#ಆದರೆ_ಅಂತ್ಯ

/* Not exposed but defined in include/linux/ieee80211.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IEEE80211_MAX_SSID_LEN
#ವ್ಯಾಖ್ಯಾನಿಸು IEEE80211_MAX_SSID_LEN 32
#ಆದರೆ_ಅಂತ್ಯ

/* Not exposed but defined in include/net/netlabel.h */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NETLBL_NLTYPE_UNLABELED_NAME
#ವ್ಯಾಖ್ಯಾನಿಸು NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#ಆದರೆ_ಅಂತ್ಯ

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
ಎಣಿಕೆ {
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
ಎಣಿಕೆ {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6_ಮುಖವಾಡ,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_ಮುಖವಾಡ,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
