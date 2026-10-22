/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <net/if.h>

تعريفنوع تعداد {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

صحيح format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]);
صحيح format_ifname_full_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret);

ساكن مضمن صحيح format_ifname(صحيح ifindex, حرف buf[ساكن IF_NAMESIZE]) {
        أرجع format_ifname_full(ifindex, 0, buf);
}
ساكن مضمن صحيح format_ifname_alloc(صحيح ifindex, حرف **ret) {
        أرجع format_ifname_full_alloc(ifindex, 0, ret);
}

ساكن مضمن حرف* _format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        (فراغ) format_ifname_full(ifindex, flag, buf);
        أرجع buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (حرف[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (حرف[IF_NAMESIZE]){})
