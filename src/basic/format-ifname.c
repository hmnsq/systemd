/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "format-ifname.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(પૂર્ણાંક) <= IF_NAMESIZE);

પૂર્ણાંક format_ifname_full(પૂર્ણાંક ifindex, FormatIfnameFlag flag, અક્ષર buf[સ્થિર IF_NAMESIZE]) {
        જો (ifindex <= 0)
                પાછા -EINVAL;

        જો (if_indextoname(ifindex, buf))
                પાછા 0;

        જો (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                પાછા -errno;

        જો (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        નહીંતર
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        પાછા 0;
}

પૂર્ણાંક format_ifname_full_alloc(પૂર્ણાંક ifindex, FormatIfnameFlag flag, અક્ષર **ret) {
        અક્ષર buf[IF_NAMESIZE];
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = format_ifname_full(ifindex, flag, buf);
        જો (r < 0)
                પાછા r;

        પાછા strdup_to(ret, buf);
}
