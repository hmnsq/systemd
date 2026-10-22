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
        ཕྱིར
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ཧྲིལ encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        གལ་ཏེ (!str || !str_enc)
                ཕྱིར -EINVAL;

        ཆེད་དུ (i = 0, j = 0; str[i] != '\0'; i++) {
                ཧྲིལ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                གལ་ཏེ (seqlen > 1) {

                        གལ་ཏེ (len-j < (size_t) seqlen)
                                ཕྱིར -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } དེ་མིན གལ་ཏེ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        གལ་ཏེ (len-j < 5)
                                ཕྱིར -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } དེ་མིན {
                        གལ་ཏེ (len-j < 1)
                                ཕྱིར -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        གལ་ཏེ (len-j < 1)
                ཕྱིར -EINVAL;

        str_enc[j] = '\0';
        ཕྱིར 0;
}

ཧྲིལ devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        ཧྲིལ r;

        assert(a);
        assert(b);

        གལ་ཏེ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ཕྱིར -EINVAL;

        གལ་ཏེ (stat(a, &sa) < 0)
                ཕྱིར -errno;
        r = stat_verify_device_node(&sa);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (stat(b, &sb) < 0)
                ཕྱིར -errno;
        r = stat_verify_device_node(&sb);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ཕྱིར false;

        ཕྱིར sa.st_rdev == sb.st_rdev;
}
