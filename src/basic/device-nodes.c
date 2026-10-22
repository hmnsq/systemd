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

पूर्णाङ्क allow_listed_char_for_devnode(वर्ण c, अचर वर्ण *additional) {
        फर्काऊ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                म_अक्षर("#+-.:=@_", c) ||
                (additional && म_अक्षर(additional, c));
}

पूर्णाङ्क encode_devnode_name(अचर वर्ण *str, वर्ण *str_enc, माप_प्रकार len) {
        माप_प्रकार i, j;

        यदि (!str || !str_enc)
                फर्काऊ -EINVAL;

        लागि (i = 0, j = 0; str[i] != '\0'; i++) {
                पूर्णाङ्क seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                यदि (seqlen > 1) {

                        यदि (len-j < (माप_प्रकार) seqlen)
                                फर्काऊ -EINVAL;

                        स_नकल(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } अन्यथा यदि (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], रिक्त)) {

                        यदि (len-j < 5)
                                फर्काऊ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (अचिन्हित वर्ण) str[i]));
                        j += 4;

                } अन्यथा {
                        यदि (len-j < 1)
                                फर्काऊ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        यदि (len-j < 1)
                फर्काऊ -EINVAL;

        str_enc[j] = '\0';
        फर्काऊ 0;
}

पूर्णाङ्क devnode_same(अचर वर्ण *a, अचर वर्ण *b) {
        संरचना stat sa, sb;
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        यदि (!valid_device_गाँठो_path(a) || !valid_device_गाँठो_path(b))
                फर्काऊ -EINVAL;

        यदि (stat(a, &sa) < 0)
                फर्काऊ -त्रुटि_सं;
        r = stat_verify_device_गाँठो(&sa);
        यदि (r < 0)
                फर्काऊ r;

        यदि (stat(b, &sb) < 0)
                फर्काऊ -त्रुटि_सं;
        r = stat_verify_device_गाँठो(&sb);
        यदि (r < 0)
                फर्काऊ r;

        यदि (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                फर्काऊ false;

        फर्काऊ sa.st_rdev == sb.st_rdev;
}
