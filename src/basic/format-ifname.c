/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ᯀᯮᯝ (ifindex <= 0)
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (if_indextoname(ifindex, buf))
                ᯀᯮᯞ 0;

        ᯀᯮᯝ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ᯀᯮᯞ -errno;

        ᯀᯮᯝ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ᯀᯮᯝᯂᯪ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ᯀᯮᯞ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ strdup_to(ret, buf);
}
