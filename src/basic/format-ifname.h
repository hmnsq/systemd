/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <net/if.h>

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

ಪೂರ್ಣಾಙ್ಕ format_ifname_full(ಪೂರ್ಣಾಙ್ಕ ifindex, FormatIfnameFlag flag, ವರ್ಣ buf[ಸ್ಥಿರ IF_NAMESIZE]);
ಪೂರ್ಣಾಙ್ಕ format_ifname_full_alloc(ಪೂರ್ಣಾಙ್ಕ ifindex, FormatIfnameFlag flag, ವರ್ಣ **ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ format_ifname(ಪೂರ್ಣಾಙ್ಕ ifindex, ವರ್ಣ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        ಪ್ರತ್ಯಾಗಮ format_ifname_full(ifindex, 0, buf);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ format_ifname_alloc(ಪೂರ್ಣಾಙ್ಕ ifindex, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ format_ifname_full_alloc(ifindex, 0, ret);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* _format_ifname_full(ಪೂರ್ಣಾಙ್ಕ ifindex, FormatIfnameFlag flag, ವರ್ಣ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        (ಶೂನ್ಯ) format_ifname_full(ifindex, flag, buf);
        ಪ್ರತ್ಯಾಗಮ buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (ವರ್ಣ[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (ವರ್ಣ[IF_NAMESIZE]){})
