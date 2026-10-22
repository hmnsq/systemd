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

จำนวนเต็ม allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        คืน
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

จำนวนเต็ม encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ถ้า (!str || !str_enc)
                คืน -EINVAL;

        สำหรับ (i = 0, j = 0; str[i] != '\0'; i++) {
                จำนวนเต็ม seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ถ้า (seqlen > 1) {

                        ถ้า (len-j < (size_t) seqlen)
                                คืน -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } มิฉะนั้น ถ้า (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ถ้า (len-j < 5)
                                คืน -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } มิฉะนั้น {
                        ถ้า (len-j < 1)
                                คืน -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ถ้า (len-j < 1)
                คืน -EINVAL;

        str_enc[j] = '\0';
        คืน 0;
}

จำนวนเต็ม devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        จำนวนเต็ม r;

        assert(a);
        assert(b);

        ถ้า (!valid_device_node_path(a) || !valid_device_node_path(b))
                คืน -EINVAL;

        ถ้า (stat(a, &sa) < 0)
                คืน -errno;
        r = stat_verify_device_node(&sa);
        ถ้า (r < 0)
                คืน r;

        ถ้า (stat(b, &sb) < 0)
                คืน -errno;
        r = stat_verify_device_node(&sb);
        ถ้า (r < 0)
                คืน r;

        ถ้า (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                คืน false;

        คืน sa.st_rdev == sb.st_rdev;
}
