/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <net/if.h>

પ્રકારવ્યાખ્યા ગણના {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

પૂર્ણાંક format_ifname_full(પૂર્ણાંક ifindex, FormatIfnameFlag flag, અક્ષર buf[સ્થિર IF_NAMESIZE]);
પૂર્ણાંક format_ifname_full_alloc(પૂર્ણાંક ifindex, FormatIfnameFlag flag, અક્ષર **ret);

સ્થિર અંતઃરેખ પૂર્ણાંક format_ifname(પૂર્ણાંક ifindex, અક્ષર buf[સ્થિર IF_NAMESIZE]) {
        પાછા format_ifname_full(ifindex, 0, buf);
}
સ્થિર અંતઃરેખ પૂર્ણાંક format_ifname_alloc(પૂર્ણાંક ifindex, અક્ષર **ret) {
        પાછા format_ifname_full_alloc(ifindex, 0, ret);
}

સ્થિર અંતઃરેખ અક્ષર* _format_ifname_full(પૂર્ણાંક ifindex, FormatIfnameFlag flag, અક્ષર buf[સ્થિર IF_NAMESIZE]) {
        (શૂન્ય) format_ifname_full(ifindex, flag, buf);
        પાછા buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (અક્ષર[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (અક્ષર[IF_NAMESIZE]){})
