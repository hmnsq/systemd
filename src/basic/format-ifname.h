/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <net/if.h>

প্রকারলেখ গণনা {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

পূর্ণসংখ্যা format_ifname_full(পূর্ণসংখ্যা ifindex, FormatIfnameFlag flag, অক্ষর buf[স্থির IF_NAMESIZE]);
পূর্ণসংখ্যা format_ifname_full_alloc(পূর্ণসংখ্যা ifindex, FormatIfnameFlag flag, অক্ষর **ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা format_ifname(পূর্ণসংখ্যা ifindex, অক্ষর buf[স্থির IF_NAMESIZE]) {
        ফেরত format_ifname_full(ifindex, 0, buf);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা format_ifname_alloc(পূর্ণসংখ্যা ifindex, অক্ষর **ret) {
        ফেরত format_ifname_full_alloc(ifindex, 0, ret);
}

স্থির অন্তঃরেখ অক্ষর* _format_ifname_full(পূর্ণসংখ্যা ifindex, FormatIfnameFlag flag, অক্ষর buf[স্থির IF_NAMESIZE]) {
        (শূন্য) format_ifname_full(ifindex, flag, buf);
        ফেরত buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (অক্ষর[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (অক্ষর[IF_NAMESIZE]){})
