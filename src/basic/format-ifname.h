/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <net/if.h>

تعريفنوع تعداد {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

صحيح format_ifname_ډک(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]);
صحيح format_ifname_ډک_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret);

ساكن مضمن صحيح format_ifname(صحيح ifindex, حرف buf[ساكن IF_NAMESIZE]) {
        أرجع format_ifname_ډک(ifindex, 0, buf);
}
ساكن مضمن صحيح format_ifname_alloc(صحيح ifindex, حرف **ret) {
        أرجع format_ifname_ډک_alloc(ifindex, 0, ret);
}

ساكن مضمن حرف* format_ifname_ډک_2(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        (فراغ) format_ifname_ډک(ifindex, flag, buf);
        أرجع buf;
}

#تعریف_کړه FORMAT_IFNAME_ډک(index, flag) format_ifname_ډک_2(index, flag, (حرف[IF_NAMESIZE]){})
#تعریف_کړه FORMAT_IFNAME(index) format_ifname_ډک_2(index, 0, (حرف[IF_NAMESIZE]){})
