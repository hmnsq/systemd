/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "device-nodes.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "utf8.h"

পূর্ণাঙ্ক allow_listed_char_for_devnode(ৱর্ণ c, ধ্রুৱ ৱর্ণ *additional) {
        ঘূৰাই
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

পূর্ণাঙ্ক encode_devnode_name(ধ্রুৱ ৱর্ণ *str, ৱর্ণ *str_enc, size_t len) {
        size_t i, j;

        যদি (!str || !str_enc)
                ঘূৰাই -EINVAL;

        বাবে (i = 0, j = 0; str[i] != '\0'; i++) {
                পূর্ণাঙ্ক seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                যদি (seqlen > 1) {

                        যদি (len-j < (size_t) seqlen)
                                ঘূৰাই -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } অন্যথা যদি (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], রিক্ত)) {

                        যদি (len-j < 5)
                                ঘূৰাই -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned ৱর্ণ) str[i]));
                        j += 4;

                } অন্যথা {
                        যদি (len-j < 1)
                                ঘূৰাই -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        যদি (len-j < 1)
                ঘূৰাই -EINVAL;

        str_enc[j] = '\0';
        ঘূৰাই 0;
}

পূর্ণাঙ্ক devnode_same(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) {
        সংরচনা stat sa, sb;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        যদি (!valid_device_গ্ৰন্থি_path(a) || !valid_device_গ্ৰন্থি_path(b))
                ঘূৰাই -EINVAL;

        যদি (stat(a, &sa) < 0)
                ঘূৰাই -errno;
        r = stat_verify_device_গ্ৰন্থি(&sa);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (stat(b, &sb) < 0)
                ঘূৰাই -errno;
        r = stat_verify_device_গ্ৰন্থি(&sb);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ঘূৰাই false;

        ঘূৰাই sa.st_rdev == sb.st_rdev;
}
