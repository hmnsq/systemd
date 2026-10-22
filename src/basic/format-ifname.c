/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(จำนวนเต็ม) <= IF_NAMESIZE);

จำนวนเต็ม format_ifname_full(จำนวนเต็ม ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ถ้า (ifindex <= 0)
                คืน -EINVAL;

        ถ้า (if_indextoname(ifindex, buf))
                คืน 0;

        ถ้า (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                คืน -errno;

        ถ้า (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        มิฉะนั้น
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        คืน 0;
}

จำนวนเต็ม format_ifname_full_alloc(จำนวนเต็ม ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        จำนวนเต็ม r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ถ้า (r < 0)
                คืน r;

        คืน strdup_to(ret, buf);
}
