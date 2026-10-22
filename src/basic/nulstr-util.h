/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <string.h>

#అన్తర్భావ "forward.h"

#నిర్వచించు NULSTR_FOREACH(i, l)                                    \
        కొరకు (typeof(*(l)) *(i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

#నిర్వచించు NULSTR_FOREACH_PAIR(i, j, l)                             \
        కొరకు (typeof(*(l)) *(i) = (l), *(j) = strchr((i), 0)+1; (i) && *(i); (i) = strchr((j), 0)+1, (j) = *(i) ? strchr((i), 0)+1 : (i))

స్థిరరాశి అక్షరం* nulstr_get(స్థిరరాశి అక్షరం *nulstr, స్థిరరాశి అక్షరం *needle);
స్థిర అంతఃవరుస bool nulstr_కలిగియుంది(స్థిరరాశి అక్షరం *nulstr, స్థిరరాశి అక్షరం *needle) {
        తిరిగి nulstr_get(nulstr, needle);
}

అక్షరం** strv_parse_nulstr_నిండిన(స్థిరరాశి అక్షరం *s, size_t l, bool drop_trailing_nuls);
స్థిర అంతఃవరుస అక్షరం** strv_parse_nulstr(స్థిరరాశి అక్షరం *s, size_t l) {
        తిరిగి strv_parse_nulstr_నిండిన(s, l, false);
}
అక్షరం** strv_విభజించు_nulstr(స్థిరరాశి అక్షరం *s);
స్థిర అంతఃవరుస పూర్ణాంకం strv_from_nulstr(అక్షరం ***ret, స్థిరరాశి అక్షరం *nulstr) {
        అక్షరం **t;

        ప్రతిజ్ఞా(ret);

        t = strv_విభజించు_nulstr(nulstr);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        *ret = t;
        తిరిగి 0;
}

పూర్ణాంకం strv_make_nulstr(అక్షరం * స్థిరరాశి *l, అక్షరం **ret, size_t *ret_కొలత);
పూర్ణాంకం సమితి_make_nulstr(సమితి_2 *s, అక్షరం **ret, size_t *ret_కొలత);
