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
        أرجع
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

صحيح encode_devnode_name(ثابت حرف *str, حرف *str_enc, size_t len) {
        size_t i, j;

        إذا (!str || !str_enc)
                أرجع -EINVAL;

        لكل (i = 0, j = 0; str[i] != '\0'; i++) {
                صحيح seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                إذا (seqlen > 1) {

                        إذا (len-j < (size_t) seqlen)
                                أرجع -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } وإلا إذا (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], عدم)) {

                        إذا (len-j < 5)
                                أرجع -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned حرف) str[i]));
                        j += 4;

                } وإلا {
                        إذا (len-j < 1)
                                أرجع -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        إذا (len-j < 1)
                أرجع -EINVAL;

        str_enc[j] = '\0';
        أرجع 0;
}

صحيح devnode_same(ثابت حرف *a, ثابت حرف *b) {
        بنية stat sa, sb;
        صحيح r;

        أكد(a);
        أكد(b);

        إذا (!valid_device_كلي_path(a) || !valid_device_كلي_path(b))
                أرجع -EINVAL;

        إذا (stat(a, &sa) < 0)
                أرجع -errno;
        r = stat_verify_device_كلي(&sa);
        إذا (r < 0)
                أرجع r;

        إذا (stat(b, &sb) < 0)
                أرجع -errno;
        r = stat_verify_device_كلي(&sb);
        إذا (r < 0)
                أرجع r;

        إذا (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                أرجع false;

        أرجع sa.st_rdev == sb.st_rdev;
}
