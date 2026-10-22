/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ပေႃး (ifindex <= 0)
                ပွၵ်ႈ -EINVAL;

        ပေႃး (if_indextoname(ifindex, buf))
                ပွၵ်ႈ 0;

        ပေႃး (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ပွၵ်ႈ -errno;

        ပေႃး (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ပေႃးဢမ်ႇ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ပွၵ်ႈ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        ပွၵ်ႈ strdup_to(ret, buf);
}
