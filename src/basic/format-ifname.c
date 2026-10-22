/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ཧྲིལ) <= IF_NAMESIZE);

ཧྲིལ format_ifname_full(ཧྲིལ ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        གལ་ཏེ (ifindex <= 0)
                ཕྱིར -EINVAL;

        གལ་ཏེ (if_indextoname(ifindex, buf))
                ཕྱིར 0;

        གལ་ཏེ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ཕྱིར -errno;

        གལ་ཏེ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        དེ་མིན
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ཕྱིར 0;
}

ཧྲིལ format_ifname_full_alloc(ཧྲིལ ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        ཧྲིལ r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར strdup_to(ret, buf);
}
