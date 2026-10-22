/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(整数) <= IF_NAMESIZE);

整数 format_ifname_full(整数 ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        如果 (ifindex <= 0)
                返回 -EINVAL;

        如果 (if_indextoname(ifindex, buf))
                返回 0;

        如果 (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                返回 -errno;

        如果 (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        否则
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        返回 0;
}

整数 format_ifname_full_alloc(整数 ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        整数 r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        如果 (r < 0)
                返回 r;

        返回 strdup_to(ret, buf);
}
