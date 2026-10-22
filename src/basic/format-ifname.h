/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <net/if.h>

প্রকারনির্দেশ গণনা {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

পূর্ণাঙ্ক format_ifname_full(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, ৱর্ণ buf[স্থির IF_NAMESIZE]);
পূর্ণাঙ্ক format_ifname_full_alloc(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, ৱর্ণ **ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক format_ifname(পূর্ণাঙ্ক ifindex, ৱর্ণ buf[স্থির IF_NAMESIZE]) {
        ঘূৰাই format_ifname_full(ifindex, 0, buf);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক format_ifname_alloc(পূর্ণাঙ্ক ifindex, ৱর্ণ **ret) {
        ঘূৰাই format_ifname_full_alloc(ifindex, 0, ret);
}

স্থির অন্তঃস্থ ৱর্ণ* _format_ifname_full(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, ৱর্ণ buf[স্থির IF_NAMESIZE]) {
        (শূন্য) format_ifname_full(ifindex, flag, buf);
        ঘূৰাই buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (ৱর্ণ[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (ৱর্ণ[IF_NAMESIZE]){})
