/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "format-ifname.h"
#include "stdio-util.h"
#include "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ຈຳນວນເຕັມ) <= IF_NAMESIZE);

ຈຳນວນເຕັມ format_ifname_full(ຈຳນວນເຕັມ ifindex, FormatIfnameFlag flag, var_na buf[static IF_NAMESIZE]) {
        ຖ້າ (ifindex <= 0)
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (if_indextoname(ifindex, buf))
                ສົ່ງຄືນ 0;

        ຖ້າ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ສົ່ງຄືນ -errno;

        ຖ້າ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ບໍ່ດັ່ງນັ້ນ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ format_ifname_full_alloc(ຈຳນວນເຕັມ ifindex, FormatIfnameFlag flag, var_na **ret) {
        var_na buf[IF_NAMESIZE];
        ຈຳນວນເຕັມ r;

        assert(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ strdup_to(ret, buf);
}
