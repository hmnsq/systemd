/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(정수) <= IF_NAMESIZE);

정수 format_ifname_full(정수 ifindex, FormatIfnameFlag flag, char buf[static IF_NAMESIZE]) {
        만약 (ifindex <= 0)
                반환 -EINVAL;

        만약 (if_indextoname(ifindex, buf))
                반환 0;

        만약 (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                반환 -errno;

        만약 (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        아니면
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        반환 0;
}

정수 format_ifname_full_alloc(정수 ifindex, FormatIfnameFlag flag, char **ret) {
        char buf[IF_NAMESIZE];
        정수 r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        만약 (r < 0)
                반환 r;

        반환 strdup_to(ret, buf);
}
