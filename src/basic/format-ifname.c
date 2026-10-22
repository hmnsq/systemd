/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-ifname.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحيح) <= IF_NAMESIZE);

صحيح format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        ܐܢ (ifindex <= 0)
                ܦܢܐ -EINVAL;

        ܐܢ (if_indextoname(ifindex, buf))
                ܦܢܐ 0;

        ܐܢ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ܦܢܐ -errno;

        ܐܢ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ܐܚܪܢܐ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ܦܢܐ 0;
}

صحيح format_ifname_full_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret) {
        حرف buf[IF_NAMESIZE];
        صحيح r;

        أكد(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ strdup_to(ret, buf);
}
