/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "device-nodes.h"
#include "path-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "utf8.h"

ကိန်းပြည့် allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ပြန်ပေး
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ကိန်းပြည့် encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        အကယ်၍ (!str || !str_enc)
                ပြန်ပေး -EINVAL;

        အတွက် (i = 0, j = 0; str[i] != '\0'; i++) {
                ကိန်းပြည့် seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                အကယ်၍ (seqlen > 1) {

                        အကယ်၍ (len-j < (size_t) seqlen)
                                ပြန်ပေး -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } မဟုတ်ပါက အကယ်၍ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        အကယ်၍ (len-j < 5)
                                ပြန်ပေး -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } မဟုတ်ပါက {
                        အကယ်၍ (len-j < 1)
                                ပြန်ပေး -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        အကယ်၍ (len-j < 1)
                ပြန်ပေး -EINVAL;

        str_enc[j] = '\0';
        ပြန်ပေး 0;
}

ကိန်းပြည့် devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        အကယ်၍ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (stat(a, &sa) < 0)
                ပြန်ပေး -errno;
        r = stat_verify_device_node(&sa);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (stat(b, &sb) < 0)
                ပြန်ပေး -errno;
        r = stat_verify_device_node(&sb);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ပြန်ပေး false;

        ပြန်ပေး sa.st_rdev == sb.st_rdev;
}
