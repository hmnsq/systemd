/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "device-nodes.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "utf8.h"

পূর্ণসংখ্যা allow_listed_char_for_devnode(অক্ষর c, ধ্রুবক অক্ষর *additional) {
        ফেরত
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

পূর্ণসংখ্যা encode_devnode_name(ধ্রুবক অক্ষর *str, অক্ষর *str_enc, size_t len) {
        size_t i, j;

        যদি (!str || !str_enc)
                ফেরত -EINVAL;

        জন্য (i = 0, j = 0; str[i] != '\0'; i++) {
                পূর্ণসংখ্যা seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                যদি (seqlen > 1) {

                        যদি (len-j < (size_t) seqlen)
                                ফেরত -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } অন্যথা যদি (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ফাঁকা)) {

                        যদি (len-j < 5)
                                ফেরত -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned অক্ষর) str[i]));
                        j += 4;

                } অন্যথা {
                        যদি (len-j < 1)
                                ফেরত -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        যদি (len-j < 1)
                ফেরত -EINVAL;

        str_enc[j] = '\0';
        ফেরত 0;
}

পূর্ণসংখ্যা devnode_same(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        কাঠামো stat sa, sb;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!valid_device_গ্রন্থি_path(a) || !valid_device_গ্রন্থি_path(b))
                ফেরত -EINVAL;

        যদি (stat(a, &sa) < 0)
                ফেরত -errno;
        r = stat_verify_device_গ্রন্থি(&sa);
        যদি (r < 0)
                ফেরত r;

        যদি (stat(b, &sb) < 0)
                ফেরত -errno;
        r = stat_verify_device_গ্রন্থি(&sb);
        যদি (r < 0)
                ফেরত r;

        যদি (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ফেরত false;

        ফেরত sa.st_rdev == sb.st_rdev;
}
