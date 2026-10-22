/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "format-ifname.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(पूर्णांक) <= IF_NAMESIZE);

पूर्णांक format_ifname_full(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर buf[अटल IF_NAMESIZE]) {
        अगर (ifindex <= 0)
                वापस -EINVAL;

        अगर (if_indextoname(ifindex, buf))
                वापस 0;

        अगर (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                वापस -त्रुटि_सं;

        अगर (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        अन्यथा
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        वापस 0;
}

पूर्णांक format_ifname_full_alloc(पूर्णांक ifindex, FormatIfnameFlag flag, अक्षर **ret) {
        अक्षर buf[IF_NAMESIZE];
        पूर्णांक r;

        निश्चित(ret);

        r = format_ifname_full(ifindex, flag, buf);
        अगर (r < 0)
                वापस r;

        वापस strdup_to(ret, buf);
}
