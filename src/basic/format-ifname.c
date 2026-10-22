/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-ifname.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحيح) <= IF_NAMESIZE);

صحيح format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        𐡄𐡍 (ifindex <= 0)
                أرجع -EINVAL;

        𐡄𐡍 (if_indextoname(ifindex, buf))
                أرجع 0;

        𐡄𐡍 (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                أرجع -errno;

        𐡄𐡍 (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        وإلا
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        أرجع 0;
}

صحيح format_ifname_full_alloc(صحيح ifindex, FormatIfnameFlag flag, حرف **ret) {
        حرف buf[IF_NAMESIZE];
        صحيح r;

        أكد(ret);

        r = format_ifname_full(ifindex, flag, buf);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع strdup_to(ret, buf);
}
