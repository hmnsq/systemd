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

ціле allow_listed_char_for_devnode(char c, const char *additional) {
        повернути
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ціле encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        якщо (!str || !str_enc)
                повернути -EINVAL;

        для (i = 0, j = 0; str[i] != '\0'; i++) {
                ціле seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                якщо (seqlen > 1) {

                        якщо (len-j < (size_t) seqlen)
                                повернути -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } інакше якщо (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        якщо (len-j < 5)
                                повернути -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } інакше {
                        якщо (len-j < 1)
                                повернути -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        якщо (len-j < 1)
                повернути -EINVAL;

        str_enc[j] = '\0';
        повернути 0;
}

ціле devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        ціле r;

        assert(a);
        assert(b);

        якщо (!valid_device_node_path(a) || !valid_device_node_path(b))
                повернути -EINVAL;

        якщо (stat(a, &sa) < 0)
                повернути -errno;
        r = stat_verify_device_node(&sa);
        якщо (r < 0)
                повернути r;

        якщо (stat(b, &sb) < 0)
                повернути -errno;
        r = stat_verify_device_node(&sb);
        якщо (r < 0)
                повернути r;

        якщо (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                повернути false;

        повернути sa.st_rdev == sb.st_rdev;
}
