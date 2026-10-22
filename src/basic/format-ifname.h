/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

ਪੂਰਨ format_ifname_ਭਰਿਆ(ਪੂਰਨ ifindex, FormatIfnameFlag flag, ਅੱਖਰ buf[ਸਥਿਰ IF_NAMESIZE]);
ਪੂਰਨ format_ifname_ਭਰਿਆ_alloc(ਪੂਰਨ ifindex, FormatIfnameFlag flag, ਅੱਖਰ **ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ format_ifname(ਪੂਰਨ ifindex, ਅੱਖਰ buf[ਸਥਿਰ IF_NAMESIZE]) {
        ਵਾਪਸ format_ifname_ਭਰਿਆ(ifindex, 0, buf);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ format_ifname_alloc(ਪੂਰਨ ifindex, ਅੱਖਰ **ret) {
        ਵਾਪਸ format_ifname_ਭਰਿਆ_alloc(ifindex, 0, ret);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* format_ifname_ਭਰਿਆ_2(ਪੂਰਨ ifindex, FormatIfnameFlag flag, ਅੱਖਰ buf[ਸਥਿਰ IF_NAMESIZE]) {
        (ਖਾਲੀ) format_ifname_ਭਰਿਆ(ifindex, flag, buf);
        ਵਾਪਸ buf;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_IFNAME_ਭਰਿਆ(index, flag) format_ifname_ਭਰਿਆ_2(index, flag, (ਅੱਖਰ[IF_NAMESIZE]){})
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_IFNAME(index) format_ifname_ਭਰਿਆ_2(index, 0, (ਅੱਖਰ[IF_NAMESIZE]){})
