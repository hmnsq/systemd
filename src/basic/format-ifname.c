/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "format-ifname.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(పూర్ణాంకం) <= IF_NAMESIZE);

పూర్ణాంకం format_ifname_నిండిన(పూర్ణాంకం ifindex, FormatIfnameFlag flag, అక్షరం buf[స్థిర IF_NAMESIZE]) {
        ఒకవేళ (ifindex <= 0)
                తిరిగి -EINVAL;

        ఒకవేళ (if_indextoname(ifindex, buf))
                తిరిగి 0;

        ఒకవేళ (!FLAGS_సమితి(flag, FORMAT_IFNAME_IFINDEX))
                తిరిగి -errno;

        ఒకవేళ (FLAGS_సమితి(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        లేకపోతే
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        తిరిగి 0;
}

పూర్ణాంకం format_ifname_నిండిన_alloc(పూర్ణాంకం ifindex, FormatIfnameFlag flag, అక్షరం **ret) {
        అక్షరం buf[IF_NAMESIZE];
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = format_ifname_నిండిన(ifindex, flag, buf);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి strdup_గమ్యం(ret, buf);
}
