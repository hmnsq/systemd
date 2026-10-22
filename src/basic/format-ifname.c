/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "format-ifname.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"

assert_cc(STRLEN("%") + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ) <= IF_NAMESIZE);

പൂർണ്ണസംഖ്യ format_ifname_full(പൂർണ്ണസംഖ്യ ifindex, FormatIfnameFlag flag, അക്ഷരം buf[സ്ഥിതം IF_NAMESIZE]) {
        എങ്കിൽ (ifindex <= 0)
                തിരികെ -EINVAL;

        എങ്കിൽ (if_indextoname(ifindex, buf))
                തിരികെ 0;

        എങ്കിൽ (!FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX))
                തിരികെ -errno;

        എങ്കിൽ (FLAGS_SET(flag, FORMAT_IFNAME_IFINDEX_WITH_PERCENT))
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%%%d", ifindex));
        അല്ലെങ്കിൽ
                assert_se(snprintf_ok(buf, IF_NAMESIZE, "%d", ifindex));

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ format_ifname_full_alloc(പൂർണ്ണസംഖ്യ ifindex, FormatIfnameFlag flag, അക്ഷരം **ret) {
        അക്ഷരം buf[IF_NAMESIZE];
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = format_ifname_full(ifindex, flag, buf);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ strdup_to(ret, buf);
}
