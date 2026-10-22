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

صحیح allow_listed_char_for_devnode(حرف c, ثابت حرف *additional) {
        واپس
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

صحیح encode_devnode_name(ثابت حرف *str, حرف *str_enc, size_t len) {
        size_t i, j;

        اگر (!str || !str_enc)
                واپس -EINVAL;

        برائے (i = 0, j = 0; str[i] != '\0'; i++) {
                صحیح seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                اگر (seqlen > 1) {

                        اگر (len-j < (size_t) seqlen)
                                واپس -EINVAL;

                        حافظہ_نقل(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ورنہ اگر (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], تہی)) {

                        اگر (len-j < 5)
                                واپس -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (غیرعلامتی حرف) str[i]));
                        j += 4;

                } ورنہ {
                        اگر (len-j < 1)
                                واپس -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        اگر (len-j < 1)
                واپس -EINVAL;

        str_enc[j] = '\0';
        واپس 0;
}

صحیح devnode_same(ثابت حرف *a, ثابت حرف *b) {
        ساخت stat sa, sb;
        صحیح r;

        أكد(a);
        أكد(b);

        اگر (!valid_device_گرہ_راستہ(a) || !valid_device_گرہ_راستہ(b))
                واپس -EINVAL;

        اگر (stat(a, &sa) < 0)
                واپس -errno;
        r = stat_verify_device_گرہ(&sa);
        اگر (r < 0)
                واپس r;

        اگر (stat(b, &sb) < 0)
                واپس -errno;
        r = stat_verify_device_گرہ(&sb);
        اگر (r < 0)
                واپس r;

        اگر (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                واپس false;

        واپس sa.st_rdev == sb.st_rdev;
}
