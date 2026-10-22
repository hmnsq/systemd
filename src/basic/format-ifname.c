/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "format-ifname.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(صحيح) <= IF_NAMESIZE);

صحيح format_ifname_full(صحيح ifindex, FormatIfnameFlag flag, حرف buf[ساكن IF_NAMESIZE]) {
        𐤀𐤌 (ifindex <= 0)
                أرجع -EINVAL;

        𐤀𐤌 (if_indextoname(ifindex, buf))
                أرجع 0;

        𐤀𐤌 (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                أرجع -errno;

        𐤀𐤌 (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
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
        𐤀𐤌 (r < 0)
                أرجع r;

        أرجع strdup_to(ret, buf);
}
