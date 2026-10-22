/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <net/if.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

ಪೂರ್ಣಾಂಕ format_ifname_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ ifindex, FormatIfnameFlag flag, ಅಕ್ಷರ buf[ಸ್ಥಿರ IF_NAMESIZE]);
ಪೂರ್ಣಾಂಕ format_ifname_ತುಂಬಿದ_alloc(ಪೂರ್ಣಾಂಕ ifindex, FormatIfnameFlag flag, ಅಕ್ಷರ **ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ format_ifname(ಪೂರ್ಣಾಂಕ ifindex, ಅಕ್ಷರ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        ಹಿಂತಿರುಗಿ format_ifname_ತುಂಬಿದ(ifindex, 0, buf);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ format_ifname_alloc(ಪೂರ್ಣಾಂಕ ifindex, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ format_ifname_ತುಂಬಿದ_alloc(ifindex, 0, ret);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* format_ifname_ತುಂಬಿದ_2(ಪೂರ್ಣಾಂಕ ifindex, FormatIfnameFlag flag, ಅಕ್ಷರ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        (ಶೂನ್ಯ) format_ifname_ತುಂಬಿದ(ifindex, flag, buf);
        ಹಿಂತಿರುಗಿ buf;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_IFNAME_ತುಂಬಿದ(index, flag) format_ifname_ತುಂಬಿದ_2(index, flag, (ಅಕ್ಷರ[IF_NAMESIZE]){})
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_IFNAME(index) format_ifname_ತುಂಬಿದ_2(index, 0, (ಅಕ್ಷರ[IF_NAMESIZE]){})
