/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ᮚᮦᮔ᮪ (ifindex <= 0)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (if_indextoname(ifindex, buf))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ᮜᮤᮚ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ᮝᮜᮤ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ strdup_to(ret, buf);
}
