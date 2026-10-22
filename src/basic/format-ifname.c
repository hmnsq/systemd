/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "format-ifname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ਪੂਰਨ) <= IF_NAMESIZE);

ਪੂਰਨ format_ifname_ਭਰਿਆ(ਪੂਰਨ ifindex, FormatIfnameFlag flag, ਅੱਖਰ buf[ਸਥਿਰ IF_NAMESIZE]) {
        ਜੇ (ifindex <= 0)
                ਵਾਪਸ -EINVAL;

        ਜੇ (if_indextoname(ifindex, buf))
                ਵਾਪਸ 0;

        ਜੇ (!FLAGS_ਸੈੱਟ(flag, FORMAT_IFNAME_IFINDEX))
                ਵਾਪਸ -errno;

        ਜੇ (FLAGS_ਸੈੱਟ(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ਨਹੀਂਤਾਂ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ਵਾਪਸ 0;
}

ਪੂਰਨ format_ifname_ਭਰਿਆ_alloc(ਪੂਰਨ ifindex, FormatIfnameFlag flag, ਅੱਖਰ **ret) {
        ਅੱਖਰ buf[IF_NAMESIZE];
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = format_ifname_ਭਰਿਆ(ifindex, flag, buf);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, buf);
}
