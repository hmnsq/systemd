/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "percent-util.h"
#അന്തര്ഭാവ "string-util.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_parts_മൂല്യം_whole(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *ചിഹ്നം) {
        സ്ഥിരം അക്ഷരം *pc, *n;
        പൂർണ്ണസംഖ്യ r, v;

        pc = endswith(p, ചിഹ്നം);
        എങ്കിൽ (!pc)
                തിരികെ -EINVAL;

        n = strndupa_safe(p, pc - p);
        r = safe_atoi(n, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (v < 0)
                തിരികെ -ERANGE;

        തിരികെ v;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_parts_മൂല്യം_with_tenths_place(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *ചിഹ്നം) {
        സ്ഥിരം അക്ഷരം *pc, *dot, *n;
        പൂർണ്ണസംഖ്യ r, q, v;

        pc = endswith(p, ചിഹ്നം);
        എങ്കിൽ (!pc)
                തിരികെ -EINVAL;

        dot = memchr(p, '.', pc - p);
        എങ്കിൽ (dot) {
                എങ്കിൽ (dot + 2 != pc)
                        തിരികെ -EINVAL;
                എങ്കിൽ (dot[1] < '0' || dot[1] > '9')
                        തിരികെ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_safe(p, dot - p);
        } അല്ലെങ്കിൽ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (v < 0)
                തിരികെ -ERANGE;
        എങ്കിൽ (v > (INT_MAX - q) / 10)
                തിരികെ -ERANGE;

        v = v * 10 + q;
        തിരികെ v;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_parts_മൂല്യം_with_hundredths_place(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *ചിഹ്നം) {
        സ്ഥിരം അക്ഷരം *pc, *dot, *n;
        പൂർണ്ണസംഖ്യ r, q, v;

        pc = endswith(p, ചിഹ്നം);
        എങ്കിൽ (!pc)
                തിരികെ -EINVAL;

        dot = memchr(p, '.', pc - p);
        എങ്കിൽ (dot) {
                എങ്കിൽ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        എങ്കിൽ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                തിരികെ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } അല്ലെങ്കിൽ എങ്കിൽ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        എങ്കിൽ (dot[1] < '0' || dot[1] > '9')
                                തിരികെ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } അല്ലെങ്കിൽ
                        /* We do not support zero or more than two places */
                        തിരികെ -EINVAL;

                n = strndupa_safe(p, dot - p);
        } അല്ലെങ്കിൽ {
                q = 0;
                n = strndupa_safe(p, pc - p);
        }
        r = safe_atoi(n, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (v < 0)
                തിരികെ -ERANGE;
        എങ്കിൽ (v > (INT_MAX - q) / 100)
                തിരികെ -ERANGE;

        v = v * 100 + q;
        തിരികെ v;
}

പൂർണ്ണസംഖ്യ parse_percent_unbounded(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ parse_parts_മൂല്യം_whole(p, "%");
}

പൂർണ്ണസംഖ്യ parse_percent(സ്ഥിരം അക്ഷരം *p) {
        പൂർണ്ണസംഖ്യ v;

        v = parse_percent_unbounded(p);
        എങ്കിൽ (v > 100)
                തിരികെ -ERANGE;

        തിരികെ v;
}

പൂർണ്ണസംഖ്യ parse_permille_unbounded(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *pm;

        pm = endswith(p, "‰");
        എങ്കിൽ (pm)
                തിരികെ parse_parts_മൂല്യം_whole(p, "‰");

        തിരികെ parse_parts_മൂല്യം_with_tenths_place(p, "%");
}

പൂർണ്ണസംഖ്യ parse_permille(സ്ഥിരം അക്ഷരം *p) {
        പൂർണ്ണസംഖ്യ v;

        v = parse_permille_unbounded(p);
        എങ്കിൽ (v > 1000)
                തിരികെ -ERANGE;

        തിരികെ v;
}

പൂർണ്ണസംഖ്യ parse_permyriad_unbounded(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *pm;

        pm = endswith(p, "‱");
        എങ്കിൽ (pm)
                തിരികെ parse_parts_മൂല്യം_whole(p, "‱");

        pm = endswith(p, "‰");
        എങ്കിൽ (pm)
                തിരികെ parse_parts_മൂല്യം_with_tenths_place(p, "‰");

        തിരികെ parse_parts_മൂല്യം_with_hundredths_place(p, "%");
}

പൂർണ്ണസംഖ്യ parse_permyriad(സ്ഥിരം അക്ഷരം *p) {
        പൂർണ്ണസംഖ്യ v;

        v = parse_permyriad_unbounded(p);
        എങ്കിൽ (v > 10000)
                തിരികെ -ERANGE;

        തിരികെ v;
}
