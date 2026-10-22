/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <net/if.h>

തരനിർവചനം എണ്ണൽ {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

പൂർണ്ണസംഖ്യ format_ifname_full(പൂർണ്ണസംഖ്യ ifindex, FormatIfnameFlag flag, അക്ഷരം buf[സ്ഥിതം IF_NAMESIZE]);
പൂർണ്ണസംഖ്യ format_ifname_full_alloc(പൂർണ്ണസംഖ്യ ifindex, FormatIfnameFlag flag, അക്ഷരം **ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ format_ifname(പൂർണ്ണസംഖ്യ ifindex, അക്ഷരം buf[സ്ഥിതം IF_NAMESIZE]) {
        തിരികെ format_ifname_full(ifindex, 0, buf);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ format_ifname_alloc(പൂർണ്ണസംഖ്യ ifindex, അക്ഷരം **ret) {
        തിരികെ format_ifname_full_alloc(ifindex, 0, ret);
}

സ്ഥിതം വരിയിൽ അക്ഷരം* _format_ifname_full(പൂർണ്ണസംഖ്യ ifindex, FormatIfnameFlag flag, അക്ഷരം buf[സ്ഥിതം IF_NAMESIZE]) {
        (ശൂന്യം) format_ifname_full(ifindex, flag, buf);
        തിരികെ buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (അക്ഷരം[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (അക്ഷരം[IF_NAMESIZE]){})
