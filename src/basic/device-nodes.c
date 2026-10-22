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

पूर्णांक allow_listed_char_for_devnode(अक्षर c, स्थिरांक अक्षर *additional) {
        परत
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                म_अक्षर("#+-.:=@_", c) ||
                (additional && म_अक्षर(additional, c));
}

पूर्णांक encode_devnode_name(स्थिरांक अक्षर *str, अक्षर *str_enc, माप_प्रकार len) {
        माप_प्रकार i, j;

        जर (!str || !str_enc)
                परत -EINVAL;

        साठी (i = 0, j = 0; str[i] != '\0'; i++) {
                पूर्णांक seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                जर (seqlen > 1) {

                        जर (len-j < (माप_प्रकार) seqlen)
                                परत -EINVAL;

                        स_नकल(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } अन्यथा जर (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], रिक्त)) {

                        जर (len-j < 5)
                                परत -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (अचिन्हित अक्षर) str[i]));
                        j += 4;

                } अन्यथा {
                        जर (len-j < 1)
                                परत -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        जर (len-j < 1)
                परत -EINVAL;

        str_enc[j] = '\0';
        परत 0;
}

पूर्णांक devnode_same(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        संरचना stat sa, sb;
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        जर (!valid_device_गाठ_path(a) || !valid_device_गाठ_path(b))
                परत -EINVAL;

        जर (stat(a, &sa) < 0)
                परत -त्रुटि_सं;
        r = stat_verify_device_गाठ(&sa);
        जर (r < 0)
                परत r;

        जर (stat(b, &sb) < 0)
                परत -त्रुटि_सं;
        r = stat_verify_device_गाठ(&sb);
        जर (r < 0)
                परत r;

        जर (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                परत false;

        परत sa.st_rdev == sb.st_rdev;
}
