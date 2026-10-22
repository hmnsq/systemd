/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "device-nodes.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "utf8.h"

పూర్ణాంకం allow_listed_char_for_devnode(అక్షరం c, స్థిరరాశి అక్షరం *additional) {
        తిరిగి
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

పూర్ణాంకం encode_devnode_name(స్థిరరాశి అక్షరం *str, అక్షరం *str_enc, size_t len) {
        size_t i, j;

        ఒకవేళ (!str || !str_enc)
                తిరిగి -EINVAL;

        కొరకు (i = 0, j = 0; str[i] != '\0'; i++) {
                పూర్ణాంకం seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                ఒకవేళ (seqlen > 1) {

                        ఒకవేళ (len-j < (size_t) seqlen)
                                తిరిగి -EINVAL;

                        జ్ఞాపకం_నకలు(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } లేకపోతే ఒకవేళ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ఖాళీ)) {

                        ఒకవేళ (len-j < 5)
                                తిరిగి -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (గుర్తులేని అక్షరం) str[i]));
                        j += 4;

                } లేకపోతే {
                        ఒకవేళ (len-j < 1)
                                తిరిగి -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        ఒకవేళ (len-j < 1)
                తిరిగి -EINVAL;

        str_enc[j] = '\0';
        తిరిగి 0;
}

పూర్ణాంకం devnode_same(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        నిర్మాణం stat sa, sb;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        ఒకవేళ (!valid_device_కణుపు_మార్గం(a) || !valid_device_కణుపు_మార్గం(b))
                తిరిగి -EINVAL;

        ఒకవేళ (stat(a, &sa) < 0)
                తిరిగి -errno;
        r = stat_verify_device_కణుపు(&sa);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (stat(b, &sb) < 0)
                తిరిగి -errno;
        r = stat_verify_device_కణుపు(&sb);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                తిరిగి false;

        తిరిగి sa.st_rdev == sb.st_rdev;
}
