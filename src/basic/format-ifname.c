/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ціле) <= IF_NAMESIZE);

ціле format_ifname_full(ціле ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        якщо (ifindex <= 0)
                повернути -EINVAL;

        якщо (if_indextoname(ifindex, buf))
                повернути 0;

        якщо (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                повернути -errno;

        якщо (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        інакше
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        повернути 0;
}

ціле format_ifname_full_alloc(ціле ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        ціле r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        якщо (r < 0)
                повернути r;

        повернути strdup_to(ret, buf);
}
