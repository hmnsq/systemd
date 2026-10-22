/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "device-nodes.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "utf8.h"

പൂർണ്ണസംഖ്യ allow_listed_char_for_devnode(അക്ഷരം c, സ്ഥിരം അക്ഷരം *additional) {
        തിരികെ
                ascii_isdigit(c) ||
                ascii_isalpha(c) ||
                strchr("#+-.:=@_", c) ||
                (additional && strchr(additional, c));
}

പൂർണ്ണസംഖ്യ encode_devnode_name(സ്ഥിരം അക്ഷരം *str, അക്ഷരം *str_enc, size_t len) {
        size_t i, j;

        എങ്കിൽ (!str || !str_enc)
                തിരികെ -EINVAL;

        വേണ്ടി (i = 0, j = 0; str[i] != '\0'; i++) {
                പൂർണ്ണസംഖ്യ seqlen;

                seqlen = utf8_encoded_valid_unichar(str + i, SIZE_MAX);
                എങ്കിൽ (seqlen > 1) {

                        എങ്കിൽ (len-j < (size_t) seqlen)
                                തിരികെ -EINVAL;

                        memcpy(&str_enc[j], &str[i], seqlen);
                        j += seqlen;
                        i += (seqlen-1);

                } അല്ലെങ്കിൽ എങ്കിൽ (str[i] == '\\' || !allow_listed_char_for_devnode(str[i], ശൂന്യ)) {

                        എങ്കിൽ (len-j < 5)
                                തിരികെ -EINVAL;

                        assert_se(snprintf_ok(&str_enc[j], 5, "\\x%02x", (unsigned അക്ഷരം) str[i]));
                        j += 4;

                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (len-j < 1)
                                തിരികെ -EINVAL;

                        str_enc[j] = str[i];
                        j++;
                }
        }

        എങ്കിൽ (len-j < 1)
                തിരികെ -EINVAL;

        str_enc[j] = '\0';
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ devnode_same(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        ഘടന stat sa, sb;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        എങ്കിൽ (!valid_device_കണ്ണി_path(a) || !valid_device_കണ്ണി_path(b))
                തിരികെ -EINVAL;

        എങ്കിൽ (stat(a, &sa) < 0)
                തിരികെ -errno;
        r = stat_verify_device_കണ്ണി(&sa);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (stat(b, &sb) < 0)
                തിരികെ -errno;
        r = stat_verify_device_കണ്ണി(&sb);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (((sa.st_mode ^ sb.st_mode) & S_IFMT) != 0) /* both inode same device node type? */
                തിരികെ false;

        തിരികെ sa.st_rdev == sb.st_rdev;
}
