/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <net/if.h>

typedef ga_nanaa {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

පූර්ණ_සංඛ්‍යා format_ifname_full(පූර්ණ_සංඛ්‍යා ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]);
පූර්ණ_සංඛ්‍යා format_ifname_full_alloc(පූර්ණ_සංඛ්‍යා ifindex, FormatIfnameFlag flag, var_na **ret);

static anta_hstha පූර්ණ_සංඛ්‍යා format_ifname(පූර්ණ_සංඛ්‍යා ifindex, var_na buf[static IF_NAMESIZE]) {
        ආපසු format_ifname_full(ifindex, 0, buf);
}
static anta_hstha පූර්ණ_සංඛ්‍යා format_ifname_alloc(පූර්ණ_සංඛ්‍යා ifindex, var_na **ret) {
        ආපසු format_ifname_full_alloc(ifindex, 0, ret);
}

static anta_hstha var_na* _format_ifname_full(පූර්ණ_සංඛ්‍යා ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        (void) format_ifname_full(ifindex, flag, buf);
        ආපසු buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (var_na[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (var_na[IF_NAMESIZE]){})
