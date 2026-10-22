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

ຈຳນວນເຕັມ allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ສົ່ງຄືນ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ຈຳນວນເຕັມ encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ຖ້າ (!str || !str_enc)
                ສົ່ງຄືນ -EINVAL;

        ສຳລັບ (i = 0, j = 0; str[i] != '\0'; i++) {
                ຈຳນວນເຕັມ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ຖ້າ (seqlen > 1) {

                        ຖ້າ (len-j < (size_t) seqlen)
                                ສົ່ງຄືນ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ຖ້າ (len-j < 5)
                                ສົ່ງຄືນ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } ບໍ່ດັ່ງນັ້ນ {
                        ຖ້າ (len-j < 1)
                                ສົ່ງຄືນ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ຖ້າ (len-j < 1)
                ສົ່ງຄືນ -EINVAL;

        str_enc[j] = '\0';
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        ຈຳນວນເຕັມ r;

        assert(a);
        assert(b);

        ຖ້າ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (stat(a, &sa) < 0)
                ສົ່ງຄືນ -errno;
        r = stat_verify_device_node(&sa);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (stat(b, &sb) < 0)
                ສົ່ງຄືນ -errno;
        r = stat_verify_device_node(&sb);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ sa.st_rdev == sb.st_rdev;
}
