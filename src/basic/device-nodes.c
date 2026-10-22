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

puur_naa_nka allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ᱨᱩᱣᱟᱹᱲ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ᱡᱩᱫᱤ (!str || !str_enc)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        for (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ᱡᱩᱫᱤ (seqlen > 1) {

                        ᱡᱩᱫᱤ (len-j < (size_t) seqlen)
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ᱵᱟᱝ ᱡᱩᱫᱤ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ᱡᱩᱫᱤ (len-j < 5)
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ᱵᱟᱝ {
                        ᱡᱩᱫᱤ (len-j < 1)
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ᱡᱩᱫᱤ (len-j < 1)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        str_enc[j] = '\0';
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ᱡᱩᱫᱤ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱩᱫᱤ (stat(a, &sa) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;
        r = stat_verify_device_node(&sa);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (stat(b, &sb) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;
        r = stat_verify_device_node(&sb);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ᱨᱩᱣᱟᱹᱲ false;

        ᱨᱩᱣᱟᱹᱲ sa.st_rdev == sb.st_rdev;
}
