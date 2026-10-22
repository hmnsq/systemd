/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(цяло) <= IF_NAMESIZE);

цяло format_ifname_full(цяло ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        ако (ifindex <= 0)
                върни -EINVAL;

        ако (if_indextoname(ifindex, buf))
                върни 0;

        ако (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                върни -errno;

        ако (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        иначе
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        върни 0;
}

цяло format_ifname_full_alloc(цяло ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        цяло r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ако (r < 0)
                върни r;

        върни strdup_to(ret, buf);
}
