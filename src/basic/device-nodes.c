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
        إرجاع
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

صحيح encode_devnode_name(ثابت حرف *str, حرف *str_enc, size_t len) {
        size_t i, j;

        إذا (!str || !str_enc)
                إرجاع -EINVAL;

        لكل (i = 0, j = 0; str[i] != '\0'; i++) {
                صحيح seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                إذا (seqlen > 1) {

                        إذا (len-j < (size_t) seqlen)
                                إرجاع -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } وإلا إذا (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], عدم)) {

                        إذا (len-j < 5)
                                إرجاع -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned حرف) str[i]));
                        j += 4;

                } وإلا {
                        إذا (len-j < 1)
                                إرجاع -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        إذا (len-j < 1)
                إرجاع -EINVAL;

        str_enc[j] = '\0';
        إرجاع 0;
}

صحيح devnode_same(ثابت حرف *a, ثابت حرف *b) {
        بنية stat sa, sb;
        صحيح r;

        أكد(a);
        أكد(b);

        إذا (!valid_device_عقدة_path(a) || !valid_device_عقدة_path(b))
                إرجاع -EINVAL;

        إذا (stat(a, &sa) < 0)
                إرجاع -errno;
        r = stat_verify_device_عقدة(&sa);
        إذا (r < 0)
                إرجاع r;

        إذا (stat(b, &sb) < 0)
                إرجاع -errno;
        r = stat_verify_device_عقدة(&sb);
        إذا (r < 0)
                إرجاع r;

        إذا (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                إرجاع false;

        إرجاع sa.st_rdev == sb.st_rdev;
}
