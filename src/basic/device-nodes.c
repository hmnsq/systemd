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
        पटिनिवत्त
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

पूर्णाङ्क encode_devnode_name(ध्रुव वर्ण *str, वर्ण *str_enc, size_t len) {
        size_t i, j;

        सचे (!str || !str_enc)
                पटिनिवत्त -EINVAL;

        अत्थाय (i = 0, j = 0; str[i] != '\0'; i++) {
                पूर्णाङ्क seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                सचे (seqlen > 1) {

                        सचे (len-j < (size_t) seqlen)
                                पटिनिवत्त -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } अञ्ञथा सचे (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], रिक्त)) {

                        सचे (len-j < 5)
                                पटिनिवत्त -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned वर्ण) str[i]));
                        j += 4;

                } अञ्ञथा {
                        सचे (len-j < 1)
                                पटिनिवत्त -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        सचे (len-j < 1)
                पटिनिवत्त -EINVAL;

        str_enc[j] = '\0';
        पटिनिवत्त 0;
}

पूर्णाङ्क devnode_same(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        संरचना stat sa, sb;
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        सचे (!valid_device_ग्रन्थिः_path(a) || !valid_device_ग्रन्थिः_path(b))
                पटिनिवत्त -EINVAL;

        सचे (stat(a, &sa) < 0)
                पटिनिवत्त -errno;
        r = stat_verify_device_ग्रन्थिः(&sa);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (stat(b, &sb) < 0)
                पटिनिवत्त -errno;
        r = stat_verify_device_ग्रन्थिः(&sb);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                पटिनिवत्त false;

        पटिनिवत्त sa.st_rdev == sb.st_rdev;
}
