/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "capability-list.h"
#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

స్థిర స్థిరరాశి నిర్మాణం capability_name* lookup_capability(నమోదు స్థిరరాశి అక్షరం *str, నమోదు GPERF_LEN_TYPE len);

#అన్తర్భావ "capability-from-name.inc"
#అన్తర్భావ "capability-to-name.inc"

స్థిరరాశి అక్షరం* capability_గమ్యం_name(పూర్ణాంకం id) {
        ఒకవేళ (id < 0)
                తిరిగి ఖాళీ;
        ఒకవేళ ((గుర్తులేని) id >= capability_list_నిడివి())
                తిరిగి ఖాళీ;

        తిరిగి capability_పేర్లు[id];
}

స్థిరరాశి అక్షరం* capability_గమ్యం_string(పూర్ణాంకం id, అక్షరం buf[స్థిర CAPABILITY_గమ్యం_STRING_MAX]) {
        స్థిరరాశి అక్షరం *p;

        ఒకవేళ (id < 0)
                తిరిగి ఖాళీ;
        ఒకవేళ (id > CAP_హద్దు) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                తిరిగి ఖాళీ;

        p = capability_గమ్యం_name(id);
        ఒకవేళ (p)
                తిరిగి p;

        sprintf(buf, "0x%x", (గుర్తులేని) id); /* numerical fallback */
        తిరిగి buf;
}

పూర్ణాంకం capability_from_name(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి నిర్మాణం capability_name *sc;
        పూర్ణాంకం r, i;

        ప్రతిజ్ఞా(name);

        /* Try to parse numeric capability */
        r = సురక్షిత_atoi(name, &i);
        ఒకవేళ (r >= 0) {
                ఒకవేళ (i < 0 || i > CAP_హద్దు)
                        తిరిగి -EINVAL;

                తిరిగి i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, అక్షరశ్రేణి_పొడవు(name));
        ఒకవేళ (!sc)
                తిరిగి -EINVAL;

        తిరిగి sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
గుర్తులేని capability_list_నిడివి(శూన్య) {
        తిరిగి MIN((గుర్తులేని) ELEMENTSOF(capability_పేర్లు), (గుర్తులేని) (CAP_హద్దు + 1));
}

పూర్ణాంకం capability_సమితి_గమ్యం_string(uint64_t సమితి, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *str = ఖాళీ;

        ప్రతిజ్ఞా(ret);

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                స్థిరరాశి అక్షరం *p;

                ఒకవేళ (!బిట్టు_సమితి(సమితి, i))
                        కొనసాగు;

                p = CAPABILITY_గమ్యం_STRING(i);
                ప్రతిజ్ఞా(p);

                ఒకవేళ (!strextend_with_separator(&str, " ", p))
                        తిరిగి -ENOMEM;
        }

        ఒకవేళ (!str) {
                str = new0(అక్షరం, 1);
                ఒకవేళ (!str)
                        తిరిగి -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        తిరిగి 0;
}

పూర్ణాంకం capability_సమితి_గమ్యం_string_negative(uint64_t సమితి, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *a = ఖాళీ, *b = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_సమితి_గమ్యం_string(సమితి, &a);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = capability_సమితి_గమ్యం_string(~సమితి & all_capabilities(), &b);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(a) <= 1 + అక్షరశ్రేణి_పొడవు(b))
                *ret = TAKE_PTR(a);
        లేకపోతే {
                అక్షరం *c = strjoin("~", b);
                ఒకవేళ (!c)
                        తిరిగి -ENOMEM;

                *ret = c;
        }

        తిరిగి 0;
}

పూర్ణాంకం capability_సమితి_గమ్యం_strv(uint64_t సమితి, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        కొరకు (గుర్తులేని i = 0; i <= cap_చివరి_cap(); i++) {
                స్థిరరాశి అక్షరం *p;

                ఒకవేళ (!బిట్టు_సమితి(సమితి, i))
                        కొనసాగు;

                p = CAPABILITY_గమ్యం_STRING(i);
                ప్రతిజ్ఞా(p);

                r = strv_extend(&l, p);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(l);
        తిరిగి 0;
}

పూర్ణాంకం capability_సమితి_from_string(స్థిరరాశి అక్షరం *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        కొరకు (స్థిరరాశి అక్షరం *p = s;;) {
                _cleanup_free_ అక్షరం *మాట = ఖాళీ;
                పూర్ణాంకం r;

                r = వెలికితీయి_మొదటి_మాట(&p, &మాట, ఖాళీ, వెలికితీయి_UNQUOTE|వెలికితీయి_RELAX);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                r = capability_from_name(మాట);
                ఒకవేళ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", మాట);
                        good = false;
                } లేకపోతే
                        val |= UINT64_C(1) << r;
        }

        ఒకవేళ (ret)
                *ret = val;

        తిరిగి good;
}
