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

ಪೂರ್ಣಾಂಕ allow_listed_char_for_devnode(ಅಕ್ಷರ c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *additional) {
        ಹಿಂತಿರುಗಿ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ಪೂರ್ಣಾಂಕ encode_devnode_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಅಕ್ಷರ *str_enc, size_t len) {
        size_t i, j;

        ಆದರೆ (!str || !str_enc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (i = 0, j = 0; str[i] != '\0'; i++) {
                ಪೂರ್ಣಾಂಕ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ಆದರೆ (seqlen > 1) {

                        ಆದರೆ (len-j < (size_t) seqlen)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ಸ್ಮರಣೆ_ನಕಲು(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ಖಾಲಿ)) {

                        ಆದರೆ (len-j < 5)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) str[i]));
                        j += 4;

                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (len-j < 1)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ಆದರೆ (len-j < 1)
                ಹಿಂತಿರುಗಿ -EINVAL;

        str_enc[j] = '\0';
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ devnode_same(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ರಚನೆ stat sa, sb;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಆದರೆ (!valid_device_ಗಂಟು_ಮಾರ್ಗ(a) || !valid_device_ಗಂಟು_ಮಾರ್ಗ(b))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (stat(a, &sa) < 0)
                ಹಿಂತಿರುಗಿ -errno;
        r = stat_verify_device_ಗಂಟು(&sa);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (stat(b, &sb) < 0)
                ಹಿಂತಿರುಗಿ -errno;
        r = stat_verify_device_ಗಂಟು(&sb);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ sa.st_rdev == sb.st_rdev;
}
