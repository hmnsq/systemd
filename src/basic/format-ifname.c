/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "format-ifname.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ) <= IF_NAMESIZE);

ಪೂರ್ಣಾಂಕ format_ifname_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ ifindex, FormatIfnameFlag flag, ಅಕ್ಷರ buf[ಸ್ಥಿರ IF_NAMESIZE]) {
        ಆದರೆ (ifindex <= 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (if_indextoname(ifindex, buf))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!FLAGS_ಗಣ(flag, FORMAT_IFNAME_IFINDEX))
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (FLAGS_ಗಣ(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ಇಲ್ಲದಿದ್ದರೆ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ format_ifname_ತುಂಬಿದ_alloc(ಪೂರ್ಣಾಂಕ ifindex, FormatIfnameFlag flag, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ buf[IF_NAMESIZE];
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = format_ifname_ತುಂಬಿದ(ifindex, flag, buf);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, buf);
}
