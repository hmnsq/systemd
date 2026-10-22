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

số_nguyên allow_listed_char_for_devnode(char c, const char *additional) {
        trả_về
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

số_nguyên encode_devnode_name(const char *str, char *str_enc, size_t len) {
        size_t i, j;

        nếu (!str || !str_enc)
                trả_về -EINVAL;

        cho (i = 0, j = 0; str[i] != '\0'; i++) {
                số_nguyên seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                nếu (seqlen > 1) {

                        nếu (len-j < (size_t) seqlen)
                                trả_về -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } không_thì nếu (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], NULL)) {

                        nếu (len-j < 5)
                                trả_về -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned char) str[i]));
                        j += 4;

                } không_thì {
                        nếu (len-j < 1)
                                trả_về -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        nếu (len-j < 1)
                trả_về -EINVAL;

        str_enc[j] = '\0';
        trả_về 0;
}

số_nguyên devnode_same(const char *a, const char *b) {
        struct stat sa, sb;
        số_nguyên r;

        assert(a);
        assert(b);

        nếu (!valid_device_node_path(a) || !valid_device_node_path(b))
                trả_về -EINVAL;

        nếu (stat(a, &sa) < 0)
                trả_về -errno;
        r = stat_verify_device_node(&sa);
        nếu (r < 0)
                trả_về r;

        nếu (stat(b, &sb) < 0)
                trả_về -errno;
        r = stat_verify_device_node(&sb);
        nếu (r < 0)
                trả_về r;

        nếu (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                trả_về false;

        trả_về sa.st_rdev == sb.st_rdev;
}
