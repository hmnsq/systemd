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

পূর্ণাঙ্ক allow_listed_char_for_devnode(বর্ণ c, ধ্রুব বর্ণ *additional) {
        প্রত্যাগম
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

পূর্ণাঙ্ক encode_devnode_name(ধ্রুব বর্ণ *str, বর্ণ *str_enc, size_t len) {
        size_t i, j;

        যদি (!str || !str_enc)
                প্রত্যাগম -EINVAL;

        প্রতি (i = 0, j = 0; str[i] != '\0'; i++) {
                পূর্ণাঙ্ক seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                যদি (seqlen > 1) {

                        যদি (len-j < (size_t) seqlen)
                                প্রত্যাগম -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } অন্যথা যদি (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], রিক্ত)) {

                        যদি (len-j < 5)
                                প্রত্যাগম -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned বর্ণ) str[i]));
                        j += 4;

                } অন্যথা {
                        যদি (len-j < 1)
                                প্রত্যাগম -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        যদি (len-j < 1)
                প্রত্যাগম -EINVAL;

        str_enc[j] = '\0';
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক devnode_same(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b) {
        সংরচনা stat sa, sb;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!valid_device_গ্রন্থি_path(a) || !valid_device_গ্রন্থি_path(b))
                প্রত্যাগম -EINVAL;

        যদি (stat(a, &sa) < 0)
                প্রত্যাগম -errno;
        r = stat_verify_device_গ্রন্থি(&sa);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (stat(b, &sb) < 0)
                প্রত্যাগম -errno;
        r = stat_verify_device_গ্রন্থি(&sb);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                প্রত্যাগম false;

        প্রত্যাগম sa.st_rdev == sb.st_rdev;
}
