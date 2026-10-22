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

цео allow_listed_char_for_devnode(char c, const char *additional) {
        врати
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

цео encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        ако (!str || !str_enc)
                врати -EINVAL;

        за (i = 0, j = 0; str[i] != '\0'; i++) {
                цео seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ако (seqlen > 1) {

                        ако (len-j < (size_t) seqlen)
                                врати -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } инако ако (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ако (len-j < 5)
                                врати -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } инако {
                        ако (len-j < 1)
                                врати -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ако (len-j < 1)
                врати -EINVAL;

        str_enc[j] = '\0';
        врати 0;
}

цео devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        цео r;

        assert(a);
        assert(b);

        ако (!valid_device_node_path(a) || !valid_device_node_path(b))
                врати -EINVAL;

        ако (stat(a, &sa) < 0)
                врати -errno;
        r = stat_verify_device_node(&sa);
        ако (r < 0)
                врати r;

        ако (stat(b, &sb) < 0)
                врати -errno;
        r = stat_verify_device_node(&sb);
        ако (r < 0)
                врати r;

        ако (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                врати false;

        врати sa.st_rdev == sb.st_rdev;
}
