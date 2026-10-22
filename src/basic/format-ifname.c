/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ᨀᨘ (ifindex <= 0)
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (if_indextoname(ifindex, buf))
                ᨀᨘᨒ 0;

        ᨀᨘ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ᨀᨘᨒ -errno;

        ᨀᨘ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ᨀᨘᨁ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ᨀᨘᨒ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ strdup_to(ret, buf);
}
