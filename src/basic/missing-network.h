/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <netinet/in.h> /* IWYU pragma: export */

/* Not exposed but defined at include/net/ip.h */
#अगर_अघोषित IPV4_MIN_MTU
#घोषणा IPV4_MIN_MTU 68
#पूर्ण_अगर

/* Note that LOOPBACK_IFINDEX is currently not exposed by the
 * kernel/glibc, but hardcoded internally by the kernel.  However, as
 * it is exported to userspace indirectly via rtnetlink and the
 * ioctls, and made use of widely we define it here too, in a way that
 * is compatible with the kernel's internal definition. */
#अगर_अघोषित LOOPBACK_IFINDEX
#घोषणा LOOPBACK_IFINDEX 1
#पूर्ण_अगर

/* Not exposed but defined in linux/netdevice.h */
#अगर_अघोषित MAX_PHYS_ITEM_ID_LEN
#घोषणा MAX_PHYS_ITEM_ID_LEN 32
#पूर्ण_अगर

/* Not exposed but defined in include/net/bonding.h */
#अगर_अघोषित BOND_MAX_ARP_TARGETS
#घोषणा BOND_MAX_ARP_TARGETS 16
#पूर्ण_अगर

/* Not exposed but defined in include/linux/ieee80211.h */
#अगर_अघोषित IEEE80211_MAX_SSID_LEN
#घोषणा IEEE80211_MAX_SSID_LEN 32
#पूर्ण_अगर

/* Not exposed but defined in include/net/netlabel.h */
#अगर_अघोषित NETLBL_NLTYPE_UNLABELED_NAME
#घोषणा NETLBL_NLTYPE_UNLABELED_NAME "NLBL_UNLBL"
#पूर्ण_अगर

/* Not exposed but defined in net/netlabel/netlabel_unlabeled.h */
क्रमागत {
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
क्रमागत {
        NLBL_UNLABEL_A_UNSPEC,
        NLBL_UNLABEL_A_ACPTFLG,
        NLBL_UNLABEL_A_IPV6ADDR,
        NLBL_UNLABEL_A_IPV6MASK,
        NLBL_UNLABEL_A_IPV4ADDR,
        NLBL_UNLABEL_A_IPV4MASK,
        NLBL_UNLABEL_A_IFACE,
        NLBL_UNLABEL_A_SECCTX,
        __NLBL_UNLABEL_A_MAX,
};
