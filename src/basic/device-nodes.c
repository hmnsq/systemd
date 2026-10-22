/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "device-nodes.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪೂರ್ಣಾಙ್ಕ allow_listed_char_for_devnode(ವರ್ಣ c, ಧ್ರುವ ವರ್ಣ *additional) {
        ಪ್ರತ್ಯಾಗಮ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ಪೂರ್ಣಾಙ್ಕ encode_devnode_name(ಧ್ರುವ ವರ್ಣ *str, ವರ್ಣ *str_enc, size_t len) {
        size_t i, j;

        ಯದಿ (!str || !str_enc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತಿ (i = 0, j = 0; str[i] != '\0'; i++) {
                ಪೂರ್ಣಾಙ್ಕ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ಯದಿ (seqlen > 1) {

                        ಯದಿ (len-j < (size_t) seqlen)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ಅನ್ಯಥಾ ಯದಿ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ರಿಕ್ತ)) {

                        ಯದಿ (len-j < 5)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned ವರ್ಣ) str[i]));
                        j += 4;

                } ಅನ್ಯಥಾ {
                        ಯದಿ (len-j < 1)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ಯದಿ (len-j < 1)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        str_enc[j] = '\0';
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ devnode_same(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಸಂರಚನಾ stat sa, sb;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಯದಿ (!valid_device_ಗಂಟು_path(a) || !valid_device_ಗಂಟು_path(b))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (stat(a, &sa) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        r = stat_verify_device_ಗಂಟು(&sa);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (stat(b, &sb) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        r = stat_verify_device_ಗಂಟು(&sb);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ sa.st_rdev == sb.st_rdev;
}
