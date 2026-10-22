/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(số_nguyên) <= IF_NAMESIZE);

số_nguyên format_ifname_full(số_nguyên ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        nếu (ifindex <= 0)
                trả_về -EINVAL;

        nếu (if_indextoname(ifindex, buf))
                trả_về 0;

        nếu (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                trả_về -errno;

        nếu (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        không_thì
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        trả_về 0;
}

số_nguyên format_ifname_full_alloc(số_nguyên ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        số_nguyên r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        nếu (r < 0)
                trả_về r;

        trả_về strdup_to(ret, buf);
}
