/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(целое) <= IF_NAMESIZE);

целое format_ifname_full(целое ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        если (ifindex <= 0)
                вернуть -EINVAL;

        если (if_indextoname(ifindex, buf))
                вернуть 0;

        если (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                вернуть -errno;

        если (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        иначе
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        вернуть 0;
}

целое format_ifname_full_alloc(целое ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        целое r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        если (r < 0)
                вернуть r;

        вернуть strdup_to(ret, buf);
}
