/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-ifname.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحيح) <= IF_NAMESIZE);

صحيح format_ifname_ڀريل(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        إذا (ifindex <= 0)
                أرجع -EINVAL;

        إذا (if_indextoname(ifindex, buf))
                أرجع 0;

        إذا (!FLAGS_سيٽ(flag, FORMAT_IFNAME_IFINDEX))
                أرجع -errno;

        إذا (FLAGS_سيٽ(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        وإلا
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        أرجع 0;
}

صحيح format_ifname_ڀريل_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret) {
        حرف buf[IF_NAMESIZE];
        صحيح r;

        أكد(ret);

        r = format_ifname_ڀريل(ifindex, flag, buf);
        إذا (r < 0)
                أرجع r;

        أرجع strdup_منزل(ret, buf);
}
