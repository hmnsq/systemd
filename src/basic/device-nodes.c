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
        ᮝᮜᮤ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ᮚᮦᮔ᮪ (!str || !str_enc)
                ᮝᮜᮤ -EINVAL;

        for (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ᮚᮦᮔ᮪ (seqlen > 1) {

                        ᮚᮦᮔ᮪ (len-j < (size_t) seqlen)
                                ᮝᮜᮤ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ᮜᮤᮚ ᮚᮦᮔ᮪ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ᮚᮦᮔ᮪ (len-j < 5)
                                ᮝᮜᮤ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ᮜᮤᮚ {
                        ᮚᮦᮔ᮪ (len-j < 1)
                                ᮝᮜᮤ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ᮚᮦᮔ᮪ (len-j < 1)
                ᮝᮜᮤ -EINVAL;

        str_enc[j] = '\0';
        ᮝᮜᮤ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ᮚᮦᮔ᮪ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (stat(a, &sa) < 0)
                ᮝᮜᮤ -errno;
        r = stat_verify_device_node(&sa);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (stat(b, &sb) < 0)
                ᮝᮜᮤ -errno;
        r = stat_verify_device_node(&sb);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ᮝᮜᮤ false;

        ᮝᮜᮤ sa.st_rdev == sb.st_rdev;
}
