/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "device-nodes.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "utf8.h"

पूर्णाङ्क allow_listed_char_for_devnode(वर्ण c, ध्रुव वर्ण *additional) {
        प्रत्यागम
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

पूर्णाङ्क encode_devnode_name(ध्रुव वर्ण *str, वर्ण *str_enc, size_t len) {
        size_t i, j;

        यदि (!str || !str_enc)
                प्रत्यागम -EINVAL;

        प्रति (i = 0, j = 0; str[i] != '\0'; i++) {
                पूर्णाङ्क seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                यदि (seqlen > 1) {

                        यदि (len-j < (size_t) seqlen)
                                प्रत्यागम -EINVAL;

                        स्मृतिं_प्रतिलिख(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } अन्यथा यदि (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], रिक्त)) {

                        यदि (len-j < 5)
                                प्रत्यागम -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (अचिह्नित वर्ण) str[i]));
                        j += 4;

                } अन्यथा {
                        यदि (len-j < 1)
                                प्रत्यागम -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        यदि (len-j < 1)
                प्रत्यागम -EINVAL;

        str_enc[j] = '\0';
        प्रत्यागम 0;
}

पूर्णाङ्क devnode_same(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        संरचना stat sa, sb;
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        यदि (!valid_device_ग्रन्थिः_मार्गः(a) || !valid_device_ग्रन्थिः_मार्गः(b))
                प्रत्यागम -EINVAL;

        यदि (stat(a, &sa) < 0)
                प्रत्यागम -errno;
        r = stat_verify_device_ग्रन्थिः(&sa);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (stat(b, &sb) < 0)
                प्रत्यागम -errno;
        r = stat_verify_device_ग्रन्थिः(&sb);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                प्रत्यागम false;

        प्रत्यागम sa.st_rdev == sb.st_rdev;
}
