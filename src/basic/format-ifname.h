/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <net/if.h>

प्रकारव्याख्या गणना {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

पूर्णांक format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[स्थिर IF_NAMESIZE]);
पूर्णांक format_ifname_full_alloc(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर **ret);

स्थिर ओळीत पूर्णांक format_ifname(पूर्णांक ifindex, अक्षर buf[स्थिर IF_NAMESIZE]) {
        परत format_ifname_full(ifindex, 0, buf);
}
स्थिर ओळीत पूर्णांक format_ifname_alloc(पूर्णांक ifindex, अक्षर **ret) {
        परत format_ifname_full_alloc(ifindex, 0, ret);
}

स्थिर ओळीत अक्षर* _format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[स्थिर IF_NAMESIZE]) {
        (शून्य) format_ifname_full(ifindex, flag, buf);
        परत buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (अक्षर[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (अक्षर[IF_NAMESIZE]){})
