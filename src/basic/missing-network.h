/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#నిర్వచితం_కాకపోతే IPV4_MIN_MTU
#నిర్వచించు IPV4_MIN_MTU 68
#అయితే_ముగింపు

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#నిర్వచితం_కాకపోతే LOOPBACK_IFINDEX
#నిర్వచించు LOOPBACK_IFINDEX 1
#అయితే_ముగింపు

/* Not exposed but defined in linux/netdevice.h */
#నిర్వచితం_కాకపోతే MAX_PHYS_అంశం_ID_LEN
#నిర్వచించు MAX_PHYS_అంశం_ID_LEN 32
#అయితే_ముగింపు

/* Not exposed but defined in include/net/bonding.h */
#నిర్వచితం_కాకపోతే BOND_MAX_ARP_TARGETS
#నిర్వచించు BOND_MAX_ARP_TARGETS 16
#అయితే_ముగింపు

/* Not exposed but defined in include/linux/ieee80211.h */
#నిర్వచితం_కాకపోతే IEEE80211_MAX_SSID_LEN
#నిర్వచించు IEEE80211_MAX_SSID_LEN 32
#అయితే_ముగింపు

/* Not exposed but defined in include/net/netlabel.h */
#నిర్వచితం_కాకపోతే NETLBL_NLTYPE_UNLABELED_NAME
#నిర్వచించు NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#అయితే_ముగింపు

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
గణన {
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
గణన {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6_ముసుగు,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4_ముసుగు,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
