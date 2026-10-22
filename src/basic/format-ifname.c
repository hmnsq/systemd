/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-ifname.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحيح) <= IF_NAMESIZE);

صحيح format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        إذا (ifindex <= 0)
                إرجاع -EINVAL;

        إذا (if_indextoname(ifindex, buf))
                إرجاع 0;

        إذا (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                إرجاع -errno;

        إذا (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        وإلا
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        إرجاع 0;
}

صحيح format_ifname_full_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret) {
        حرف buf[IF_NAMESIZE];
        صحيح r;

        أكد(ret);

        r = format_ifname_full(ifindex, flag, buf);
        إذا (r < 0)
                إرجاع r;

        إرجاع strdup_to(ret, buf);
}
