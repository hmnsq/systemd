/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <sys/stat.h>

#समावेश "device-nodes.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"
#समावेश "utf8.h"

पूर्णांक allow_listed_char_for_devnode(अक्षर c, स्थिर अक्षर *additional) {
        वापस
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                म_अक्षर("#+-.:=@_", c) ||
                (additional && म_अक्षर(additional, c));
}

पूर्णांक encode_devnode_name(स्थिर अक्षर *str, अक्षर *str_enc, माप_प्रकार len) {
        माप_प्रकार i, j;

        अगर (!str || !str_enc)
                वापस -EINVAL;

        क्रम (i = 0, j = 0; str[i] != '\0'; i++) {
                पूर्णांक seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                अगर (seqlen > 1) {

                        अगर (len-j < (माप_प्रकार) seqlen)
                                वापस -EINVAL;

                        स_नकल(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } अन्यथा अगर (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], शून्य)) {

                        अगर (len-j < 5)
                                वापस -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (अचिन्हित अक्षर) str[i]));
                        j += 4;

                } अन्यथा {
                        अगर (len-j < 1)
                                वापस -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        अगर (len-j < 1)
                वापस -EINVAL;

        str_enc[j] = '\0';
        वापस 0;
}

पूर्णांक devnode_same(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        काष्ठा stat sa, sb;
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        अगर (!valid_device_गाँठ_path(a) || !valid_device_गाँठ_path(b))
                वापस -EINVAL;

        अगर (stat(a, &sa) < 0)
                वापस -त्रुटि_सं;
        r = stat_verify_device_गाँठ(&sa);
        अगर (r < 0)
                वापस r;

        अगर (stat(b, &sb) < 0)
                वापस -त्रुटि_सं;
        r = stat_verify_device_गाँठ(&sb);
        अगर (r < 0)
                वापस r;

        अगर (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                वापस false;

        वापस sa.st_rdev == sb.st_rdev;
}
