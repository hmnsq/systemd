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

정수 allow_listed_char_for_devnode(char c, const char *additional) {
        반환
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

정수 encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        만약 (!str || !str_enc)
                반환 -EINVAL;

        위해 (i = 0, j = 0; str[i] != '\0'; i++) {
                정수 seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                만약 (seqlen > 1) {

                        만약 (len-j < (size_t) seqlen)
                                반환 -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } 아니면 만약 (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        만약 (len-j < 5)
                                반환 -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } 아니면 {
                        만약 (len-j < 1)
                                반환 -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        만약 (len-j < 1)
                반환 -EINVAL;

        str_enc[j] = '\0';
        반환 0;
}

정수 devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        정수 r;

        assert(a);
        assert(b);

        만약 (!valid_device_node_path(a) || !valid_device_node_path(b))
                반환 -EINVAL;

        만약 (stat(a, &sa) < 0)
                반환 -errno;
        r = stat_verify_device_node(&sa);
        만약 (r < 0)
                반환 r;

        만약 (stat(b, &sb) < 0)
                반환 -errno;
        r = stat_verify_device_node(&sb);
        만약 (r < 0)
                반환 r;

        만약 (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                반환 false;

        반환 sa.st_rdev == sb.st_rdev;
}
