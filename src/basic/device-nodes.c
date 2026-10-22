/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "device-nodes.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "utf8.h"

முழு allow_listed_char_for_devnode(எழுத்து c, மாறிலி எழுத்து *additional) {
        திரும்பு
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

முழு encode_devnode_name(மாறிலி எழுத்து *str, எழுத்து *str_enc, size_t len) {
        size_t i, j;

        எனில் (!str || !str_enc)
                திரும்பு -EINVAL;

        க்கு (i = 0, j = 0; str[i] != '\0'; i++) {
                முழு seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                எனில் (seqlen > 1) {

                        எனில் (len-j < (size_t) seqlen)
                                திரும்பு -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } இல்லையெனில் எனில் (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], வெற்று)) {

                        எனில் (len-j < 5)
                                திரும்பு -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned எழுத்து) str[i]));
                        j += 4;

                } இல்லையெனில் {
                        எனில் (len-j < 1)
                                திரும்பு -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        எனில் (len-j < 1)
                திரும்பு -EINVAL;

        str_enc[j] = '\0';
        திரும்பு 0;
}

முழு devnode_same(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        கட்டமைப்பு stat sa, sb;
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        எனில் (!valid_device_முனை_path(a) || !valid_device_முனை_path(b))
                திரும்பு -EINVAL;

        எனில் (stat(a, &sa) < 0)
                திரும்பு -errno;
        r = stat_verify_device_முனை(&sa);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (stat(b, &sb) < 0)
                திரும்பு -errno;
        r = stat_verify_device_முனை(&sb);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                திரும்பு false;

        திரும்பு sa.st_rdev == sb.st_rdev;
}
