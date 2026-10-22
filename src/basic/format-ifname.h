/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <net/if.h>

प्रकार क्रमागत {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

पूर्णांक format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[अटल IF_NAMESIZE]);
पूर्णांक format_ifname_full_alloc(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर **ret);

अटल अंतरभूत पूर्णांक format_ifname(पूर्णांक ifindex, अक्षर buf[अटल IF_NAMESIZE]) {
        वापस format_ifname_full(ifindex, 0, buf);
}
अटल अंतरभूत पूर्णांक format_ifname_alloc(पूर्णांक ifindex, अक्षर **ret) {
        वापस format_ifname_full_alloc(ifindex, 0, ret);
}

अटल अंतरभूत अक्षर* _format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[अटल IF_NAMESIZE]) {
        (व्योम) format_ifname_full(ifindex, flag, buf);
        वापस buf;
}

#घोषणा FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (अक्षर[IF_NAMESIZE]){})
#घोषणा FORMAT_IFNAME(index) _format_ifname_full(index, 0, (अक्षर[IF_NAMESIZE]){})
