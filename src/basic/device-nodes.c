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

පූර්ණ_සංඛ්‍යා allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ආපසු
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

පූර්ණ_සංඛ්‍යා encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        නම් (!str || !str_enc)
                ආපසු -EINVAL;

        සඳහා (i = 0, j = 0; str[i] != '\0'; i++) {
                පූර්ණ_සංඛ්‍යා seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                නම් (seqlen > 1) {

                        නම් (len-j < (size_t) seqlen)
                                ආපසු -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } නැතහොත් නම් (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        නම් (len-j < 5)
                                ආපසු -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } නැතහොත් {
                        නම් (len-j < 1)
                                ආපසු -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        නම් (len-j < 1)
                ආපසු -EINVAL;

        str_enc[j] = '\0';
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        පූර්ණ_සංඛ්‍යා r;

        assert(a);
        assert(b);

        නම් (!valid_device_node_path(a) || !valid_device_node_path(b))
                ආපසු -EINVAL;

        නම් (stat(a, &sa) < 0)
                ආපසු -errno;
        r = stat_verify_device_node(&sa);
        නම් (r < 0)
                ආපසු r;

        නම් (stat(b, &sb) < 0)
                ආපසු -errno;
        r = stat_verify_device_node(&sb);
        නම් (r < 0)
                ආපසු r;

        නම් (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ආපසු false;

        ආපසු sa.st_rdev == sb.st_rdev;
}
