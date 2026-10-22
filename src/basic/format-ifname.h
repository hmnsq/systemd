/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <net/if.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, FormatIfnameFlag flag, ଅକ୍ଷର buf[ସ୍ଥିର IF_NAMESIZE]);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname_full_alloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, FormatIfnameFlag flag, ଅକ୍ଷର **ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, ଅକ୍ଷର buf[ସ୍ଥିର IF_NAMESIZE]) {
        ଫେରନ୍ତୁ format_ifname_full(ifindex, 0, buf);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname_alloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ format_ifname_full_alloc(ifindex, 0, ret);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* _format_ifname_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, FormatIfnameFlag flag, ଅକ୍ଷର buf[ସ୍ଥିର IF_NAMESIZE]) {
        (ଶୂନ୍ୟ) format_ifname_full(ifindex, flag, buf);
        ଫେରନ୍ତୁ buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (ଅକ୍ଷର[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (ଅକ୍ଷର[IF_NAMESIZE]){})
