/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <net/if.h>

قسمتعریف شمار {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

صحیح format_ifname_بھرا(صحیح ifindex, FormatIfnameFlag flag, حرف buf[ساکن IF_NAMESIZE]);
صحیح format_ifname_بھرا_alloc(صحیح ifindex, FormatIfnameFlag flag, حرف **ret);

ساکن اندرونی صحیح format_ifname(صحیح ifindex, حرف buf[ساکن IF_NAMESIZE]) {
        واپس format_ifname_بھرا(ifindex, 0, buf);
}
ساکن اندرونی صحیح format_ifname_alloc(صحیح ifindex, حرف **ret) {
        واپس format_ifname_بھرا_alloc(ifindex, 0, ret);
}

ساکن اندرونی حرف* format_ifname_بھرا_2(صحیح ifindex, FormatIfnameFlag flag, حرف buf[ساکن IF_NAMESIZE]) {
        (خالی) format_ifname_بھرا(ifindex, flag, buf);
        واپس buf;
}

#تعریف_کرو FORMAT_IFNAME_بھرا(index, flag) format_ifname_بھرا_2(index, flag, (حرف[IF_NAMESIZE]){})
#تعریف_کرو FORMAT_IFNAME(index) format_ifname_بھرا_2(index, 0, (حرف[IF_NAMESIZE]){})
