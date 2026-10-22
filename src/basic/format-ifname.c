/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "format-ifname.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(पूर्णाङ्क) <= IF_NAMESIZE);

पूर्णाङ्क format_ifname_full(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण buf[स्थिर IF_NAMESIZE]) {
        यदि (ifindex <= 0)
                फर्काऊ -EINVAL;

        यदि (if_indextoname(ifindex, buf))
                फर्काऊ 0;

        यदि (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                फर्काऊ -त्रुटि_सं;

        यदि (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        अन्यथा
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        फर्काऊ 0;
}

पूर्णाङ्क format_ifname_full_alloc(पूर्णाङ्क ifindex, FormatIfnameFlag flag, वर्ण **ret) {
        वर्ण buf[IF_NAMESIZE];
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = format_ifname_full(ifindex, flag, buf);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ strdup_to(ret, buf);
}
