/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "format-ifname.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(পূর্ণসংখ্যা) <= IF_NAMESIZE);

পূর্ণসংখ্যা format_ifname_full(পূর্ণসংখ্যা ifindex, FormatIfnameFlag flag, অক্ষর buf[স্থির IF_NAMESIZE]) {
        যদি (ifindex <= 0)
                ফেরত -EINVAL;

        যদি (if_indextoname(ifindex, buf))
                ফেরত 0;

        যদি (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ফেরত -errno;

        যদি (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        অন্যথা
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ফেরত 0;
}

পূর্ণসংখ্যা format_ifname_full_alloc(পূর্ণসংখ্যা ifindex, FormatIfnameFlag flag, অক্ষর **ret) {
        অক্ষর buf[IF_NAMESIZE];
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = format_ifname_full(ifindex, flag, buf);
        যদি (r < 0)
                ফেরত r;

        ফেরত strdup_to(ret, buf);
}
