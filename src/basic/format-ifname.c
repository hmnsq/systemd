/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(පූර්ණ_සංඛ්‍යා) <= IF_NAMESIZE);

පූර්ණ_සංඛ්‍යා format_ifname_full(පූර්ණ_සංඛ්‍යා ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        නම් (ifindex <= 0)
                ආපසු -EINVAL;

        නම් (if_indextoname(ifindex, buf))
                ආපසු 0;

        නම් (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ආපසු -errno;

        නම් (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        නැතහොත්
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා format_ifname_full_alloc(පූර්ණ_සංඛ්‍යා ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        නම් (r < 0)
                ආපසු r;

        ආපසු strdup_to(ret, buf);
}
