/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "format-ifname.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(পূর্ণাঙ্ক) <= IF_NAMESIZE);

পূর্ণাঙ্ক format_ifname_full(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, ৱর্ণ buf[স্থির IF_NAMESIZE]) {
        যদি (ifindex <= 0)
                ঘূৰাই -EINVAL;

        যদি (if_indextoname(ifindex, buf))
                ঘূৰাই 0;

        যদি (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ঘূৰাই -errno;

        যদি (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        অন্যথা
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ঘূৰাই 0;
}

পূর্ণাঙ্ক format_ifname_full_alloc(পূর্ণাঙ্ক ifindex, FormatIfnameFlag flag, ৱর্ণ **ret) {
        ৱর্ণ buf[IF_NAMESIZE];
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = format_ifname_full(ifindex, flag, buf);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই strdup_to(ret, buf);
}
