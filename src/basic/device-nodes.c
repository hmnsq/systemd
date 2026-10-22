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

ចំនួនគត់ allow_listed_char_for_devnode(var_na c, const var_na *additional) {
        ត្រឡប់
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ចំនួនគត់ encode_devnode_name(const var_na *str, var_na *str_enc, size_t len) {
        size_t i, j;

        ប្រសិនបើ (!str || !str_enc)
                ត្រឡប់ -EINVAL;

        សម្រាប់ (i = 0, j = 0; str[i] != '\0'; i++) {
                ចំនួនគត់ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ប្រសិនបើ (seqlen > 1) {

                        ប្រសិនបើ (len-j < (size_t) seqlen)
                                ត្រឡប់ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } បើមិនដូច្នេះ ប្រសិនបើ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        ប្រសិនបើ (len-j < 5)
                                ត្រឡប់ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned var_na) str[i]));
                        j += 4;

                } បើមិនដូច្នេះ {
                        ប្រសិនបើ (len-j < 1)
                                ត្រឡប់ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ប្រសិនបើ (len-j < 1)
                ត្រឡប់ -EINVAL;

        str_enc[j] = '\0';
        ត្រឡប់ 0;
}

ចំនួនគត់ devnode_same(const var_na *a, const var_na *b) {
        sa_nracanaa stat sa, sb;
        ចំនួនគត់ r;

        assert(a);
        assert(b);

        ប្រសិនបើ (!valid_device_node_path(a) || !valid_device_node_path(b))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (stat(a, &sa) < 0)
                ត្រឡប់ -errno;
        r = stat_verify_device_node(&sa);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (stat(b, &sb) < 0)
                ត្រឡប់ -errno;
        r = stat_verify_device_node(&sb);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ត្រឡប់ false;

        ត្រឡប់ sa.st_rdev == sb.st_rdev;
}
