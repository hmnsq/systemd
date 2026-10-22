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
        ꦮꦭꦶ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

puur_naa_nka encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ꦪꦼꦤ꧀ (!str || !str_enc)
                ꦮꦭꦶ -EINVAL;

        ꦏꦁꦒꦺꦴ (i = 0, j = 0; str[i] != '\0'; i++) {
                puur_naa_nka seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ꦪꦼꦤ꧀ (seqlen > 1) {

                        ꦪꦼꦤ꧀ (len-j < (size_t) seqlen)
                                ꦮꦭꦶ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ꦭꦶꦪ ꦪꦼꦤ꧀ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ꦪꦼꦤ꧀ (len-j < 5)
                                ꦮꦭꦶ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ꦭꦶꦪ {
                        ꦪꦼꦤ꧀ (len-j < 1)
                                ꦮꦭꦶ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ꦪꦼꦤ꧀ (len-j < 1)
                ꦮꦭꦶ -EINVAL;

        str_enc[j] = '\0';
        ꦮꦭꦶ 0;
}

puur_naa_nka devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        puur_naa_nka r;

        assert(a);
        assert(b);

        ꦪꦼꦤ꧀ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (stat(a, &sa) < 0)
                ꦮꦭꦶ -errno;
        r = stat_verify_device_node(&sa);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (stat(b, &sb) < 0)
                ꦮꦭꦶ -errno;
        r = stat_verify_device_node(&sb);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ꦮꦭꦶ false;

        ꦮꦭꦶ sa.st_rdev == sb.st_rdev;
}
