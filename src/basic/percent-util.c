/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "percent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਸਥਿਰ ਪੂਰਨ parse_parts_ਮੁੱਲ_ਸਾਰਾ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਚਿੰਨ੍ਹ) {
        ਅਚਲ ਅੱਖਰ *pc, *n;
        ਪੂਰਨ r, v;

        pc = endswith(p, ਚਿੰਨ੍ਹ);
        ਜੇ (!pc)
                ਵਾਪਸ -EINVAL;

        n = strndupa_ਸੁਰੱਖਿਅਤ(p, pc - p);
        r = ਸੁਰੱਖਿਅਤ_atoi(n, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (v < 0)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ v;
}

ਸਥਿਰ ਪੂਰਨ parse_parts_ਮੁੱਲ_with_tenths_ਰੱਖੋ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਚਿੰਨ੍ਹ) {
        ਅਚਲ ਅੱਖਰ *pc, *dot, *n;
        ਪੂਰਨ r, q, v;

        pc = endswith(p, ਚਿੰਨ੍ਹ);
        ਜੇ (!pc)
                ਵਾਪਸ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ਜੇ (dot) {
                ਜੇ (dot + 2 != pc)
                        ਵਾਪਸ -EINVAL;
                ਜੇ (dot[1] < '0' || dot[1] > '9')
                        ਵਾਪਸ -EINVAL;
                q = dot[1] - '0';
                n = strndupa_ਸੁਰੱਖਿਅਤ(p, dot - p);
        } ਨਹੀਂਤਾਂ {
                q = 0;
                n = strndupa_ਸੁਰੱਖਿਅਤ(p, pc - p);
        }
        r = ਸੁਰੱਖਿਅਤ_atoi(n, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (v < 0)
                ਵਾਪਸ -ERANGE;
        ਜੇ (v > (ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - q) / 10)
                ਵਾਪਸ -ERANGE;

        v = v * 10 + q;
        ਵਾਪਸ v;
}

ਸਥਿਰ ਪੂਰਨ parse_parts_ਮੁੱਲ_with_hundredths_ਰੱਖੋ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਚਿੰਨ੍ਹ) {
        ਅਚਲ ਅੱਖਰ *pc, *dot, *n;
        ਪੂਰਨ r, q, v;

        pc = endswith(p, ਚਿੰਨ੍ਹ);
        ਜੇ (!pc)
                ਵਾਪਸ -EINVAL;

        dot = memchr(p, '.', pc - p);
        ਜੇ (dot) {
                ਜੇ (dot + 3 == pc) {
                        /* Support two places after the dot */

                        ਜੇ (dot[1] < '0' || dot[1] > '9' || dot[2] < '0' || dot[2] > '9')
                                ਵਾਪਸ -EINVAL;
                        q = (dot[1] - '0') * 10 + (dot[2] - '0');

                } ਨਹੀਂਤਾਂ ਜੇ (dot + 2 == pc) {
                        /* Support one place after the dot */

                        ਜੇ (dot[1] < '0' || dot[1] > '9')
                                ਵਾਪਸ -EINVAL;
                        q = (dot[1] - '0') * 10;
                } ਨਹੀਂਤਾਂ
                        /* We do not support zero or more than two places */
                        ਵਾਪਸ -EINVAL;

                n = strndupa_ਸੁਰੱਖਿਅਤ(p, dot - p);
        } ਨਹੀਂਤਾਂ {
                q = 0;
                n = strndupa_ਸੁਰੱਖਿਅਤ(p, pc - p);
        }
        r = ਸੁਰੱਖਿਅਤ_atoi(n, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (v < 0)
                ਵਾਪਸ -ERANGE;
        ਜੇ (v > (ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - q) / 100)
                ਵਾਪਸ -ERANGE;

        v = v * 100 + q;
        ਵਾਪਸ v;
}

ਪੂਰਨ parse_percent_unbounded(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ parse_parts_ਮੁੱਲ_ਸਾਰਾ(p, "%");
}

ਪੂਰਨ parse_percent(ਅਚਲ ਅੱਖਰ *p) {
        ਪੂਰਨ v;

        v = parse_percent_unbounded(p);
        ਜੇ (v > 100)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ v;
}

ਪੂਰਨ parse_permille_unbounded(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *pm;

        pm = endswith(p, "‰");
        ਜੇ (pm)
                ਵਾਪਸ parse_parts_ਮੁੱਲ_ਸਾਰਾ(p, "‰");

        ਵਾਪਸ parse_parts_ਮੁੱਲ_with_tenths_ਰੱਖੋ(p, "%");
}

ਪੂਰਨ parse_permille(ਅਚਲ ਅੱਖਰ *p) {
        ਪੂਰਨ v;

        v = parse_permille_unbounded(p);
        ਜੇ (v > 1000)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ v;
}

ਪੂਰਨ parse_permyriad_unbounded(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *pm;

        pm = endswith(p, "‱");
        ਜੇ (pm)
                ਵਾਪਸ parse_parts_ਮੁੱਲ_ਸਾਰਾ(p, "‱");

        pm = endswith(p, "‰");
        ਜੇ (pm)
                ਵਾਪਸ parse_parts_ਮੁੱਲ_with_tenths_ਰੱਖੋ(p, "‰");

        ਵਾਪਸ parse_parts_ਮੁੱਲ_with_hundredths_ਰੱਖੋ(p, "%");
}

ਪੂਰਨ parse_permyriad(ਅਚਲ ਅੱਖਰ *p) {
        ਪੂਰਨ v;

        v = parse_permyriad_unbounded(p);
        ਜੇ (v > 10000)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ v;
}
