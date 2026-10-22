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
        ᰀᰧᰌ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ᰀᰧ (!str || !str_enc)
                ᰀᰧᰌ -EINVAL;

        for (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ᰀᰧ (seqlen > 1) {

                        ᰀᰧ (len-j < (size_t) seqlen)
                                ᰀᰧᰌ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ᰀᰧᰀ ᰀᰧ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ᰀᰧ (len-j < 5)
                                ᰀᰧᰌ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ᰀᰧᰀ {
                        ᰀᰧ (len-j < 1)
                                ᰀᰧᰌ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ᰀᰧ (len-j < 1)
                ᰀᰧᰌ -EINVAL;

        str_enc[j] = '\0';
        ᰀᰧᰌ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ᰀᰧ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (stat(a, &sa) < 0)
                ᰀᰧᰌ -errno;
        r = stat_verify_device_node(&sa);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (stat(b, &sb) < 0)
                ᰀᰧᰌ -errno;
        r = stat_verify_device_node(&sb);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ᰀᰧᰌ false;

        ᰀᰧᰌ sa.st_rdev == sb.st_rdev;
}
