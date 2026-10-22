/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "format-ifname.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ) <= IF_NAMESIZE);

ಪೂರ್ಣಾಙ್ಕ format_ifname_full(ಪೂರ್ಣಾಙ್ಕ ifindex, FormatIfnameFlag flag, ವರ್ಣ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        ಯದಿ (ifindex <= 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (if_indextoname(ifindex, buf))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ಅನ್ಯಥಾ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ format_ifname_full_alloc(ಪೂರ್ಣಾಙ್ಕ ifindex, FormatIfnameFlag flag, ವರ್ಣ **ret) {
        ವರ್ಣ buf[IF_NAMESIZE];
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, buf);
}
