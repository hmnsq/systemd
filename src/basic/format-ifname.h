/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <net/if.h>

வகைவரை எண்ணிக்கை {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

முழு format_ifname_full(முழு ifindex, FormatIfnameFlag flag, எழுத்து buf[நிலையான IF_NAMESIZE]);
முழு format_ifname_full_alloc(முழு ifindex, FormatIfnameFlag flag, எழுத்து **ret);

நிலையான உள்வரி முழு format_ifname(முழு ifindex, எழுத்து buf[நிலையான IF_NAMESIZE]) {
        திரும்பு format_ifname_full(ifindex, 0, buf);
}
நிலையான உள்வரி முழு format_ifname_alloc(முழு ifindex, எழுத்து **ret) {
        திரும்பு format_ifname_full_alloc(ifindex, 0, ret);
}

நிலையான உள்வரி எழுத்து* _format_ifname_full(முழு ifindex, FormatIfnameFlag flag, எழுத்து buf[நிலையான IF_NAMESIZE]) {
        (வெறுமை) format_ifname_full(ifindex, flag, buf);
        திரும்பு buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (எழுத்து[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (எழுத்து[IF_NAMESIZE]){})
