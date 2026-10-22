/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/stat.h>

#ضمن "device-nodes.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "utf8.h"

صحيح allow_listed_char_for_devnode(حرف c, ثابت حرف *additional) {
        ܦܢܐ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

صحيح encode_devnode_name(ثابت حرف *str, حرف *str_enc, size_t len) {
        size_t i, j;

        ܐܢ (!str || !str_enc)
                ܦܢܐ -EINVAL;

        لكل (i = 0, j = 0; str[i] != '\0'; i++) {
                صحيح seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ܐܢ (seqlen > 1) {

                        ܐܢ (len-j < (size_t) seqlen)
                                ܦܢܐ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ܐܚܪܢܐ ܐܢ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], عدم)) {

                        ܐܢ (len-j < 5)
                                ܦܢܐ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned حرف) str[i]));
                        j += 4;

                } ܐܚܪܢܐ {
                        ܐܢ (len-j < 1)
                                ܦܢܐ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ܐܢ (len-j < 1)
                ܦܢܐ -EINVAL;

        str_enc[j] = '\0';
        ܦܢܐ 0;
}

صحيح devnode_same(ثابت حرف *a, ثابت حرف *b) {
        بنية stat sa, sb;
        صحيح r;

        أكد(a);
        أكد(b);

        ܐܢ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ܦܢܐ -EINVAL;

        ܐܢ (stat(a, &sa) < 0)
                ܦܢܐ -errno;
        r = stat_verify_device_node(&sa);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (stat(b, &sb) < 0)
                ܦܢܐ -errno;
        r = stat_verify_device_node(&sb);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ܦܢܐ false;

        ܦܢܐ sa.st_rdev == sb.st_rdev;
}
