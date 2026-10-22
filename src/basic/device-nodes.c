/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "device-nodes.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "utf8.h"

પૂર્ણાંક allow_listed_char_for_devnode(અક્ષર c, અચલ અક્ષર *additional) {
        પાછા
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

પૂર્ણાંક encode_devnode_name(અચલ અક્ષર *str, અક્ષર *str_enc, size_t len) {
        size_t i, j;

        જો (!str || !str_enc)
                પાછા -EINVAL;

        માટે (i = 0, j = 0; str[i] != '\0'; i++) {
                પૂર્ણાંક seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                જો (seqlen > 1) {

                        જો (len-j < (size_t) seqlen)
                                પાછા -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } નહીંતર જો (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ખાલી)) {

                        જો (len-j < 5)
                                પાછા -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned અક્ષર) str[i]));
                        j += 4;

                } નહીંતર {
                        જો (len-j < 1)
                                પાછા -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        જો (len-j < 1)
                પાછા -EINVAL;

        str_enc[j] = '\0';
        પાછા 0;
}

પૂર્ણાંક devnode_same(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        સંરચના stat sa, sb;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        જો (!valid_device_ગાંઠ_path(a) || !valid_device_ગાંઠ_path(b))
                પાછા -EINVAL;

        જો (stat(a, &sa) < 0)
                પાછા -errno;
        r = stat_verify_device_ગાંઠ(&sa);
        જો (r < 0)
                પાછા r;

        જો (stat(b, &sb) < 0)
                પાછા -errno;
        r = stat_verify_device_ગાંઠ(&sb);
        જો (r < 0)
                પાછા r;

        જો (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                પાછા false;

        પાછા sa.st_rdev == sb.st_rdev;
}
