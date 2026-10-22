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

ཧྲིལ allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ལོག
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ཧྲིལ encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        གལ་སྲིད (!str || !str_enc)
                ལོག -EINVAL;

        དོན་ལུ (i = 0, j = 0; str[i] != '\0'; i++) {
                ཧྲིལ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                གལ་སྲིད (seqlen > 1) {

                        གལ་སྲིད (len-j < (size_t) seqlen)
                                ལོག -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } དེ་མིན གལ་སྲིད (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        གལ་སྲིད (len-j < 5)
                                ལོག -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } དེ་མིན {
                        གལ་སྲིད (len-j < 1)
                                ལོག -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        གལ་སྲིད (len-j < 1)
                ལོག -EINVAL;

        str_enc[j] = '\0';
        ལོག 0;
}

ཧྲིལ devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        ཧྲིལ r;

        assert(a);
        assert(b);

        གལ་སྲིད (!valid_device_node_path(a) || !valid_device_node_path(b))
                ལོག -EINVAL;

        གལ་སྲིད (stat(a, &sa) < 0)
                ལོག -errno;
        r = stat_verify_device_node(&sa);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (stat(b, &sb) < 0)
                ལོག -errno;
        r = stat_verify_device_node(&sb);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ལོག false;

        ལོག sa.st_rdev == sb.st_rdev;
}
