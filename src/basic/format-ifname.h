/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <net/if.h>

typedef enum {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

целое format_ifname_full(целое ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]);
целое format_ifname_full_alloc(целое ifindex, FormatIfnameFlag flag, char **ret);

static inline целое format_ifname(целое ifindex, char buf[static IF_NAMESIZE]) {
        вернуть format_ifname_full(ifindex, 0, buf);
}
static inline целое format_ifname_alloc(целое ifindex, char **ret) {
        вернуть format_ifname_full_alloc(ifindex, 0, ret);
}

static inline char* _format_ifname_full(целое ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        (void) format_ifname_full(ifindex, flag, buf);
        вернуть buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (char[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (char[IF_NAMESIZE]){})
