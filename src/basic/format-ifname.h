/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <net/if.h>

تعریفنوع شمارش {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

صحیح format_ifname_full(صحیح ifindex, FormatIfnameFlag flag, نویسه buf[ایستا IF_NAMESIZE]);
صحیح format_ifname_full_alloc(صحیح ifindex, FormatIfnameFlag flag, نویسه **ret);

ایستا درونخطی صحیح format_ifname(صحیح ifindex, نویسه buf[ایستا IF_NAMESIZE]) {
        بازگشت format_ifname_full(ifindex, 0, buf);
}
ایستا درونخطی صحیح format_ifname_alloc(صحیح ifindex, نویسه **ret) {
        بازگشت format_ifname_full_alloc(ifindex, 0, ret);
}

ایستا درونخطی نویسه* _format_ifname_full(صحیح ifindex, FormatIfnameFlag flag, نویسه buf[ایستا IF_NAMESIZE]) {
        (تهی) format_ifname_full(ifindex, flag, buf);
        بازگشت buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (نویسه[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (نویسه[IF_NAMESIZE]){})
