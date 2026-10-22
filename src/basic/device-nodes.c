/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "device-nodes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਪੂਰਨ allow_listed_char_for_devnode(ਅੱਖਰ c, ਅਚਲ ਅੱਖਰ *additional) {
        ਵਾਪਸ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

ਪੂਰਨ encode_devnode_name(ਅਚਲ ਅੱਖਰ *str, ਅੱਖਰ *str_enc, size_t len) {
        size_t i, j;

        ਜੇ (!str || !str_enc)
                ਵਾਪਸ -EINVAL;

        ਲਈ (i = 0, j = 0; str[i] != '\0'; i++) {
                ਪੂਰਨ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ਜੇ (seqlen > 1) {

                        ਜੇ (len-j < (size_t) seqlen)
                                ਵਾਪਸ -EINVAL;

                        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } ਨਹੀਂਤਾਂ ਜੇ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ਨਿਰਾ)) {

                        ਜੇ (len-j < 5)
                                ਵਾਪਸ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) str[i]));
                        j += 4;

                } ਨਹੀਂਤਾਂ {
                        ਜੇ (len-j < 1)
                                ਵਾਪਸ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ਜੇ (len-j < 1)
                ਵਾਪਸ -EINVAL;

        str_enc[j] = '\0';
        ਵਾਪਸ 0;
}

ਪੂਰਨ devnode_same(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਬਣਤਰ stat sa, sb;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਜੇ (!valid_device_ਗੰਢ_ਰਸਤਾ(a) || !valid_device_ਗੰਢ_ਰਸਤਾ(b))
                ਵਾਪਸ -EINVAL;

        ਜੇ (stat(a, &sa) < 0)
                ਵਾਪਸ -errno;
        r = stat_verify_device_ਗੰਢ(&sa);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (stat(b, &sb) < 0)
                ਵਾਪਸ -errno;
        r = stat_verify_device_ਗੰਢ(&sb);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                ਵਾਪਸ false;

        ਵਾਪਸ sa.st_rdev == sb.st_rdev;
}
