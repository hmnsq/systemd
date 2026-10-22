/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#यदि_अपरिभाषितम् IPV4_MIN_MTU
#परिभाषय IPV4_MIN_MTU 68
#यदि_अन्तः

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#यदि_अपरिभाषितम् LOOPBACK_IFINDEX
#परिभाषय LOOPBACK_IFINDEX 1
#यदि_अन्तः

/* Not exposed but defined in linux/netdevice.h */
#यदि_अपरिभाषितम् MAX_PHYS_ITEM_ID_LEN
#परिभाषय MAX_PHYS_ITEM_ID_LEN 32
#यदि_अन्तः

/* Not exposed but defined in include/net/bonding.h */
#यदि_अपरिभाषितम् BOND_MAX_ARP_TARGETS
#परिभाषय BOND_MAX_ARP_TARGETS 16
#यदि_अन्तः

/* Not exposed but defined in include/linux/ieee80211.h */
#यदि_अपरिभाषितम् IEEE80211_MAX_SSID_LEN
#परिभाषय IEEE80211_MAX_SSID_LEN 32
#यदि_अन्तः

/* Not exposed but defined in include/net/netlabel.h */
#यदि_अपरिभाषितम् NETLBL_NLTYPE_UNLABELED_NAME
#परिभाषय NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#यदि_अन्तः

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
गणना {
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
गणना {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6_आवरणम्,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_आवरणम्,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
