/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "format-ifname.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحیح) <= IF_NAMESIZE);

صحیح format_ifname_بھرا(صحیح ifindex, FormatIfnameFlag flag, حرف buf[ساکن IF_NAMESIZE]) {
        اگر (ifindex <= 0)
                واپس -EINVAL;

        اگر (if_indextoname(ifindex, buf))
                واپس 0;

        اگر (!FLAGS_سیٹ(flag, FORMAT_IFNAME_IFINDEX))
                واپس -errno;

        اگر (FLAGS_سیٹ(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ورنہ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        واپس 0;
}

صحیح format_ifname_بھرا_alloc(صحیح ifindex, FormatIfnameFlag flag, حرف **ret) {
        حرف buf[IF_NAMESIZE];
        صحیح r;

        أكد(ret);

        r = format_ifname_بھرا(ifindex, flag, buf);
        اگر (r < 0)
                واپس r;

        واپس strdup_مقصد(ret, buf);
}
