/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(цео) <= IF_NAMESIZE);

цео format_ifname_full(цео ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        ако (ifindex <= 0)
                врати -EINVAL;

        ако (if_indextoname(ifindex, buf))
                врати 0;

        ако (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                врати -errno;

        ако (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        инако
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        врати 0;
}

цео format_ifname_full_alloc(цео ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        цео r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ако (r < 0)
                врати r;

        врати strdup_to(ret, buf);
}
