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

整数 allow_listed_char_for_devnode(char c, const char *additional) {
        返回
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

整数 encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        如果 (!str || !str_enc)
                返回 -EINVAL;

        为 (i = 0, j = 0; str[i] != '\0'; i++) {
                整数 seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                如果 (seqlen > 1) {

                        如果 (len-j < (size_t) seqlen)
                                返回 -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } 否则 如果 (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        如果 (len-j < 5)
                                返回 -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } 否则 {
                        如果 (len-j < 1)
                                返回 -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        如果 (len-j < 1)
                返回 -EINVAL;

        str_enc[j] = '\0';
        返回 0;
}

整数 devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        整数 r;

        assert(a);
        assert(b);

        如果 (!valid_device_node_path(a) || !valid_device_node_path(b))
                返回 -EINVAL;

        如果 (stat(a, &sa) < 0)
                返回 -errno;
        r = stat_verify_device_node(&sa);
        如果 (r < 0)
                返回 r;

        如果 (stat(b, &sb) < 0)
                返回 -errno;
        r = stat_verify_device_node(&sb);
        如果 (r < 0)
                返回 r;

        如果 (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                返回 false;

        返回 sa.st_rdev == sb.st_rdev;
}
