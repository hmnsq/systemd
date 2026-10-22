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
        ᬯᬮᬶ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ᬬᬾᬦ᭄ (!str || !str_enc)
                ᬯᬮᬶ -EINVAL;

        for (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ᬬᬾᬦ᭄ (seqlen > 1) {

                        ᬬᬾᬦ᭄ (len-j < (size_t) seqlen)
                                ᬯᬮᬶ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ᬮᬶᬬ ᬬᬾᬦ᭄ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ᬬᬾᬦ᭄ (len-j < 5)
                                ᬯᬮᬶ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ᬮᬶᬬ {
                        ᬬᬾᬦ᭄ (len-j < 1)
                                ᬯᬮᬶ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ᬬᬾᬦ᭄ (len-j < 1)
                ᬯᬮᬶ -EINVAL;

        str_enc[j] = '\0';
        ᬯᬮᬶ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ᬬᬾᬦ᭄ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (stat(a, &sa) < 0)
                ᬯᬮᬶ -errno;
        r = stat_verify_device_node(&sa);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (stat(b, &sb) < 0)
                ᬯᬮᬶ -errno;
        r = stat_verify_device_node(&sb);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ᬯᬮᬶ false;

        ᬯᬮᬶ sa.st_rdev == sb.st_rdev;
}
