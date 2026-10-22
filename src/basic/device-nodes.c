/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/stat.h>

#شامل "device-nodes.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"
#شامل "utf8.h"

صحیح allow_listed_char_for_devnode(نویسه c, ثابت نویسه *additional) {
        بازگشت
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

صحیح encode_devnode_name(ثابت نویسه *str, نویسه *str_enc, size_t len) {
        size_t i, j;

        اگر (!str || !str_enc)
                بازگشت -EINVAL;

        برای (i = 0, j = 0; str[i] != '\0'; i++) {
                صحیح seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                اگر (seqlen > 1) {

                        اگر (len-j < (size_t) seqlen)
                                بازگشت -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } وگرنه اگر (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], پوچ)) {

                        اگر (len-j < 5)
                                بازگشت -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned نویسه) str[i]));
                        j += 4;

                } وگرنه {
                        اگر (len-j < 1)
                                بازگشت -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        اگر (len-j < 1)
                بازگشت -EINVAL;

        str_enc[j] = '\0';
        بازگشت 0;
}

صحیح devnode_same(ثابت نویسه *a, ثابت نویسه *b) {
        ساختار stat sa, sb;
        صحیح r;

        أكد(a);
        أكد(b);

        اگر (!valid_device_گره_path(a) || !valid_device_گره_path(b))
                بازگشت -EINVAL;

        اگر (stat(a, &sa) < 0)
                بازگشت -errno;
        r = stat_verify_device_گره(&sa);
        اگر (r < 0)
                بازگشت r;

        اگر (stat(b, &sb) < 0)
                بازگشت -errno;
        r = stat_verify_device_گره(&sb);
        اگر (r < 0)
                بازگشت r;

        اگر (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                بازگشت false;

        بازگشت sa.st_rdev == sb.st_rdev;
}
