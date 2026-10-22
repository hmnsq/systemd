/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <net/if.h>

प्रकारनिर्देश गणना {
        FORMAT_IFNAME_IFINDEX              = 1 << 0,
        FORMAT_IFNAME_IFINDEX_WITH_PERCENT = (1 << 1) | FORMAT_IFNAME_IFINDEX,
} FormatIfnameFlag;

पूर्णाङ्क format_ifname_पूर्णम्(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण buf[स्थिर IF_NAMESIZE]);
पूर्णाङ्क format_ifname_पूर्णम्_alloc(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण **ret);

स्थिर अन्तःस्थ पूर्णाङ्क format_ifname(पूर्णाङ्क ifindex, वर्ण buf[स्थिर IF_NAMESIZE]) {
        प्रत्यागम format_ifname_पूर्णम्(ifindex, 0, buf);
}
स्थिर अन्तःस्थ पूर्णाङ्क format_ifname_alloc(पूर्णाङ्क ifindex, वर्ण **ret) {
        प्रत्यागम format_ifname_पूर्णम्_alloc(ifindex, 0, ret);
}

स्थिर अन्तःस्थ वर्ण* format_ifname_पूर्णम्_2(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण buf[स्थिर IF_NAMESIZE]) {
        (शून्य) format_ifname_पूर्णम्(ifindex, flag, buf);
        प्रत्यागम buf;
}

#परिभाषय FORMAT_IFNAME_पूर्णम्(index, flag) format_ifname_पूर्णम्_2(index, flag, (वर्ण[IF_NAMESIZE]){})
#परिभाषय FORMAT_IFNAME(index) format_ifname_पूर्णम्_2(index, 0, (वर्ण[IF_NAMESIZE]){})
