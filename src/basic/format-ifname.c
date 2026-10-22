/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ᰀᰧ (ifindex <= 0)
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (if_indextoname(ifindex, buf))
                ᰀᰧᰌ 0;

        ᰀᰧ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ᰀᰧᰌ -errno;

        ᰀᰧ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ᰀᰧᰀ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ᰀᰧᰌ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧᰌ strdup_to(ret, buf);
}
