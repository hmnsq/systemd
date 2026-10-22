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
        ᯀᯮᯞ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ᯀᯮᯝ (!str || !str_enc)
                ᯀᯮᯞ -EINVAL;

        for (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ᯀᯮᯝ (seqlen > 1) {

                        ᯀᯮᯝ (len-j < (size_t) seqlen)
                                ᯀᯮᯞ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ᯀᯮᯝ (len-j < 5)
                                ᯀᯮᯞ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ᯀᯮᯝᯂᯪ {
                        ᯀᯮᯝ (len-j < 1)
                                ᯀᯮᯞ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ᯀᯮᯝ (len-j < 1)
                ᯀᯮᯞ -EINVAL;

        str_enc[j] = '\0';
        ᯀᯮᯞ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ᯀᯮᯝ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ᯀᯮᯞ -EINVAL;

        ᯀᯮᯝ (stat(a, &sa) < 0)
                ᯀᯮᯞ -errno;
        r = stat_verify_device_node(&sa);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (stat(b, &sb) < 0)
                ᯀᯮᯞ -errno;
        r = stat_verify_device_node(&sb);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ᯀᯮᯞ false;

        ᯀᯮᯞ sa.st_rdev == sb.st_rdev;
}
