/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "format-ifname.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) <= IF_NAMESIZE);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, FormatIfnameFlag flag, ଅକ୍ଷର buf[ସ୍ଥିର IF_NAMESIZE]) {
        ଯଦି (ifindex <= 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (if_indextoname(ifindex, buf))
                ଫେରନ୍ତୁ 0;

        ଯଦି (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                ଫେରନ୍ତୁ -errno;

        ଯଦି (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        ଅନ୍ୟଥା
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା format_ifname_full_alloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifindex, FormatIfnameFlag flag, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର buf[IF_NAMESIZE];
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = format_ifname_full(ifindex, flag, buf);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ strdup_to(ret, buf);
}
