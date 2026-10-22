/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ចំនួនគត់) <= IF_NAMESIZE);

ចំនួនគត់ format_ifname_full(ចំនួនគត់ ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ប្រសិនបើ (ifindex <= 0)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (if_indextoname(ifindex, buf))
                ត្រឡប់ 0;

        ប្រសិនបើ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ត្រឡប់ -errno;

        ប្រសិនបើ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        បើមិនដូច្នេះ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ត្រឡប់ 0;
}

ចំនួនគត់ format_ifname_full_alloc(ចំនួនគត់ ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        ចំនួនគត់ r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ strdup_to(ret, buf);
}
