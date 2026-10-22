/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "device-nodes.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_listed_char_for_devnode(ଅକ୍ଷର c, ଅଚଳ ଅକ୍ଷର *additional) {
        ଫେରନ୍ତୁ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା encode_devnode_name(ଅଚଳ ଅକ୍ଷର *str, ଅକ୍ଷର *str_enc, size_t len) {
        size_t i, j;

        ଯଦି (!str || !str_enc)
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (i = 0, j = 0; str[i] != '\0'; i++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ଯଦି (seqlen > 1) {

                        ଯଦି (len-j < (size_t) seqlen)
                                ଫେରନ୍ତୁ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ଅନ୍ୟଥା ଯଦି (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ଖାଲି)) {

                        ଯଦି (len-j < 5)
                                ଫେରନ୍ତୁ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned ଅକ୍ଷର) str[i]));
                        j += 4;

                } ଅନ୍ୟଥା {
                        ଯଦି (len-j < 1)
                                ଫେରନ୍ତୁ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ଯଦି (len-j < 1)
                ଫେରନ୍ତୁ -EINVAL;

        str_enc[j] = '\0';
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା devnode_same(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ସଂରଚନା stat sa, sb;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଯଦି (!valid_device_ଗଣ୍ଠି_path(a) || !valid_device_ଗଣ୍ଠି_path(b))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (stat(a, &sa) < 0)
                ଫେରନ୍ତୁ -errno;
        r = stat_verify_device_ଗଣ୍ଠି(&sa);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (stat(b, &sb) < 0)
                ଫେରନ୍ତୁ -errno;
        r = stat_verify_device_ଗଣ୍ଠି(&sb);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ sa.st_rdev == sb.st_rdev;
}
