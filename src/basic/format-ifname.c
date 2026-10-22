/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ကိန်းပြည့်) <= IF_NAMESIZE);

ကိန်းပြည့် format_ifname_full(ကိန်းပြည့် ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        အကယ်၍ (ifindex <= 0)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (if_indextoname(ifindex, buf))
                ပြန်ပေး 0;

        အကယ်၍ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ပြန်ပေး -errno;

        အကယ်၍ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        မဟုတ်ပါက
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ပြန်ပေး 0;
}

ကိန်းပြည့် format_ifname_full_alloc(ကိန်းပြည့် ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        ကိန်းပြည့် r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး strdup_to(ret, buf);
}
