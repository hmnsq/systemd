/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <net/if.h>

typedef enum {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

整数 format_ifname_full(整数 ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]);
整数 format_ifname_full_alloc(整数 ifindex, FormatIfnameFlag flag, char **ret);

static inline 整数 format_ifname(整数 ifindex, char buf[static IF_NAMESIZE]) {
        返回 format_ifname_full(ifindex, 0, buf);
}
static inline 整数 format_ifname_alloc(整数 ifindex, char **ret) {
        返回 format_ifname_full_alloc(ifindex, 0, ret);
}

static inline char* _format_ifname_full(整数 ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        (void) format_ifname_full(ifindex, flag, buf);
        返回 buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (char[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (char[IF_NAMESIZE]){})
