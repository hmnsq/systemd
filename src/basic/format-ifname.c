/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "format-ifname.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحیح) <= IF_NAMESIZE);

صحیح format_ifname_full(صحیح ifindex, FormatIfnameFlag flag, نویسه buf[ایستا IF_NAMESIZE]) {
        اگر (ifindex <= 0)
                بازگشت -EINVAL;

        اگر (if_indextoname(ifindex, buf))
                بازگشت 0;

        اگر (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                بازگشت -errno;

        اگر (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        وگرنه
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        بازگشت 0;
}

صحیح format_ifname_full_alloc(صحیح ifindex, FormatIfnameFlag flag, نویسه **ret) {
        نویسه buf[IF_NAMESIZE];
        صحیح r;

        أكد(ret);

        r = format_ifname_full(ifindex, flag, buf);
        اگر (r < 0)
                بازگشت r;

        بازگشت strdup_to(ret, buf);
}
