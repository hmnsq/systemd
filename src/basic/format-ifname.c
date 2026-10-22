/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(puur_naa_nka) <= IF_NAMESIZE);

puur_naa_nka format_ifname_full(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ꦪꦼꦤ꧀ (ifindex <= 0)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (if_indextoname(ifindex, buf))
                ꦮꦭꦶ 0;

        ꦪꦼꦤ꧀ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ꦮꦭꦶ -errno;

        ꦪꦼꦤ꧀ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ꦭꦶꦪ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ꦮꦭꦶ 0;
}

puur_naa_nka format_ifname_full_alloc(puur_naa_nka ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        puur_naa_nka r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ strdup_to(ret, buf);
}
