/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "format-ifname.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(पूर्णांक) <= IF_NAMESIZE);

पूर्णांक format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[स्थिर IF_NAMESIZE]) {
        जर (ifindex <= 0)
                परत -EINVAL;

        जर (if_indextoname(ifindex, buf))
                परत 0;

        जर (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                परत -त्रुटि_सं;

        जर (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        अन्यथा
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        परत 0;
}

पूर्णांक format_ifname_full_alloc(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर **ret) {
        अक्षर buf[IF_NAMESIZE];
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = format_ifname_full(ifindex, flag, buf);
        जर (r < 0)
                परत r;

        परत strdup_to(ret, buf);
}
