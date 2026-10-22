/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ཧྲིལ) <= IF_NAMESIZE);

ཧྲིལ format_ifname_full(ཧྲིལ ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        གལ་སྲིད (ifindex <= 0)
                ལོག -EINVAL;

        གལ་སྲིད (if_indextoname(ifindex, buf))
                ལོག 0;

        གལ་སྲིད (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ལོག -errno;

        གལ་སྲིད (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        དེ་མིན
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ལོག 0;
}

ཧྲིལ format_ifname_full_alloc(ཧྲིལ ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        ཧྲིལ r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག strdup_to(ret, buf);
}
