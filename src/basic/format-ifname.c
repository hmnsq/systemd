/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "format-ifname.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(முழு) <= IF_NAMESIZE);

முழு format_ifname_full(முழு ifindex, FormatIfnameFlag flag, எழுத்து buf[நிலையான IF_NAMESIZE]) {
        எனில் (ifindex <= 0)
                திரும்பு -EINVAL;

        எனில் (if_indextoname(ifindex, buf))
                திரும்பு 0;

        எனில் (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                திரும்பு -errno;

        எனில் (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        இல்லையெனில்
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        திரும்பு 0;
}

முழு format_ifname_full_alloc(முழு ifindex, FormatIfnameFlag flag, எழுத்து **ret) {
        எழுத்து buf[IF_NAMESIZE];
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = format_ifname_full(ifindex, flag, buf);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு strdup_to(ret, buf);
}
