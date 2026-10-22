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

цяло allow_listed_char_for_devnode(char c, const char *additional) {
        върни
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

цяло encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        ако (!str || !str_enc)
                върни -EINVAL;

        за (i = 0, j = 0; str[i] != '\0'; i++) {
                цяло seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ако (seqlen > 1) {

                        ако (len-j < (size_t) seqlen)
                                върни -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } иначе ако (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ако (len-j < 5)
                                върни -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } иначе {
                        ако (len-j < 1)
                                върни -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ако (len-j < 1)
                върни -EINVAL;

        str_enc[j] = '\0';
        върни 0;
}

цяло devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        цяло r;

        assert(a);
        assert(b);

        ако (!valid_device_node_path(a) || !valid_device_node_path(b))
                върни -EINVAL;

        ако (stat(a, &sa) < 0)
                върни -errno;
        r = stat_verify_device_node(&sa);
        ако (r < 0)
                върни r;

        ако (stat(b, &sb) < 0)
                върни -errno;
        r = stat_verify_device_node(&sb);
        ако (r < 0)
                върни r;

        ако (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                върни false;

        върни sa.st_rdev == sb.st_rdev;
}
