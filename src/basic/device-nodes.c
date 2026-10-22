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

целое allow_listed_char_for_devnode(char c, const char *additional) {
        вернуть
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

целое encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        если (!str || !str_enc)
                вернуть -EINVAL;

        для (i = 0, j = 0; str[i] != '\0'; i++) {
                целое seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                если (seqlen > 1) {

                        если (len-j < (size_t) seqlen)
                                вернуть -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } иначе если (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        если (len-j < 5)
                                вернуть -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } иначе {
                        если (len-j < 1)
                                вернуть -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        если (len-j < 1)
                вернуть -EINVAL;

        str_enc[j] = '\0';
        вернуть 0;
}

целое devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        целое r;

        assert(a);
        assert(b);

        если (!valid_device_node_path(a) || !valid_device_node_path(b))
                вернуть -EINVAL;

        если (stat(a, &sa) < 0)
                вернуть -errno;
        r = stat_verify_device_node(&sa);
        если (r < 0)
                вернуть r;

        если (stat(b, &sb) < 0)
                вернуть -errno;
        r = stat_verify_device_node(&sb);
        если (r < 0)
                вернуть r;

        если (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                вернуть false;

        вернуть sa.st_rdev == sb.st_rdev;
}
