/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <net/if.h>

प्रकारनिर्देश गणना {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

पूर्णाङ्क format_ifname_full(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण buf[स्थिर IF_NAMESIZE]);
पूर्णाङ्क format_ifname_full_alloc(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण **ret);

स्थिर अन्तःस्थ पूर्णाङ्क format_ifname(पूर्णाङ्क ifindex, वर्ण buf[स्थिर IF_NAMESIZE]) {
        पटिनिवत्त format_ifname_full(ifindex, 0, buf);
}
स्थिर अन्तःस्थ पूर्णाङ्क format_ifname_alloc(पूर्णाङ्क ifindex, वर्ण **ret) {
        पटिनिवत्त format_ifname_full_alloc(ifindex, 0, ret);
}

स्थिर अन्तःस्थ वर्ण* _format_ifname_full(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण buf[स्थिर IF_NAMESIZE]) {
        (शून्य) format_ifname_full(ifindex, flag, buf);
        पटिनिवत्त buf;
}

#define FORMAT_IFNAME_FULL(index, flag) _format_ifname_full(index, flag, (वर्ण[IF_NAMESIZE]){})
#define FORMAT_IFNAME(index) _format_ifname_full(index, 0, (वर्ण[IF_NAMESIZE]){})
