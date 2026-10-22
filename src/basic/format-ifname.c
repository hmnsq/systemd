/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "format-ifname.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(পূর্ণাঙ্ক) <= IF_NAMESIZE);

পূর্ণাঙ্ক format_ifname_full(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, বর্ণ buf[স্থির IF_NAMESIZE]) {
        যদি (ifindex <= 0)
                প্রত্যাগম -EINVAL;

        যদি (if_indextoname(ifindex, buf))
                প্রত্যাগম 0;

        যদি (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                প্রত্যাগম -errno;

        যদি (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        অন্যথা
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক format_ifname_full_alloc(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, বর্ণ **ret) {
        বর্ণ buf[IF_NAMESIZE];
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = format_ifname_full(ifindex, flag, buf);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম strdup_to(ret, buf);
}
