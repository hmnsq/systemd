/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/sysmacros.h>

#అన్తర్భావ "hash-funcs.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "strv.h"

శూన్య string_hash_func(స్థిరరాశి అక్షరం *p, నిర్మాణం siphash *state) {
        siphash24_compress(p, అక్షరశ్రేణి_పొడవు(p) + 1, state);
}

DEFINE_HASH_OPS(string_hash_ops,
                అక్షరం, string_hash_func, string_compare_func);
DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(
                string_hash_ops_free,
                అక్షరం, string_hash_func, string_compare_func, విడుదల);
DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR(
                string_hash_ops_విలువ_free,
                అక్షరం, string_hash_func, string_compare_func,
                శూన్య, విడుదల);
DEFINE_HASH_OPS_నిండిన(
                string_hash_ops_free_free,
                అక్షరం, string_hash_func, string_compare_func, విడుదల,
                శూన్య, విడుదల);
DEFINE_HASH_OPS_నిండిన(
                string_hash_ops_free_strv_free,
                అక్షరం, string_hash_func, string_compare_func, విడుదల,
                అక్షరం*, strv_free);

శూన్య మార్గం_hash_func(స్థిరరాశి అక్షరం *p, నిర్మాణం siphash *state) {
        bool కూడు_slash = false;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(state);

        /* Calculates a hash for a path in a way this duplicate inner slashes don't make a differences, and also
         * whether there's a trailing slash or not. This fits well with the semantics of path_compare(), which does
         * similar checks and also doesn't care for trailing slashes. Note that relative and absolute paths (i.e. those
         * which begin in a slash or not) will hash differently though. */

        /* if path is absolute, add one "/" to the hash. */
        ఒకవేళ (మార్గం_is_absolute(p))
                siphash24_compress_byte('/', state);

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *e;
                పూర్ణాంకం r;

                r = మార్గం_కనుగొను_మొదటి_component(&p, true, &e);
                ఒకవేళ (r == 0)
                        తిరిగి;

                ఒకవేళ (కూడు_slash)
                        siphash24_compress_byte('/', state);

                ఒకవేళ (r < 0) {
                        /* if a component is invalid, then add remaining part as a string. */
                        string_hash_func(p, state);
                        తిరిగి;
                }

                /* Add this component to the hash. */
                siphash24_compress(e, r, state);

                కూడు_slash = true;
        }
}

DEFINE_HASH_OPS(మార్గం_hash_ops,
                అక్షరం, మార్గం_hash_func, మార్గం_compare);
DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(
                మార్గం_hash_ops_free,
                అక్షరం, మార్గం_hash_func, మార్గం_compare, విడుదల);
DEFINE_HASH_OPS_నిండిన(
                మార్గం_hash_ops_free_free,
                అక్షరం, మార్గం_hash_func, మార్గం_compare, విడుదల,
                శూన్య, విడుదల);

శూన్య trivial_hash_func(స్థిరరాశి శూన్య *p, నిర్మాణం siphash *state) {
        siphash24_compress_typesafe(p, state);
}

పూర్ణాంకం trivial_compare_func(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b) {
        తిరిగి CMP(a, b);
}

DEFINE_HASH_OPS(trivial_hash_ops,
                శూన్య, trivial_hash_func, trivial_compare_func);
DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(
                trivial_hash_ops_free,
                శూన్య, trivial_hash_func, trivial_compare_func, విడుదల);
DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR(
                trivial_hash_ops_విలువ_free,
                శూన్య, trivial_hash_func, trivial_compare_func,
                శూన్య, విడుదల);
DEFINE_HASH_OPS_నిండిన(
                trivial_hash_ops_free_free,
                శూన్య, trivial_hash_func, trivial_compare_func, విడుదల,
                శూన్య, విడుదల);

శూన్య uint64_hash_func(స్థిరరాశి uint64_t *p, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(p);

        siphash24_compress_typesafe(*p, state);
}

పూర్ణాంకం uint64_compare_func(స్థిరరాశి uint64_t *a, స్థిరరాశి uint64_t *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి CMP(*a, *b);
}

DEFINE_HASH_OPS(uint64_hash_ops,
                uint64_t, uint64_hash_func, uint64_compare_func);
DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR(
                uint64_hash_ops_విలువ_free,
                uint64_t, uint64_hash_func, uint64_compare_func,
                శూన్య, విడుదల);

#ఒకవేళ SIZEOF_DEV_T != 8
శూన్య devt_hash_func(స్థిరరాశి dev_t *p, నిర్మాణం siphash *state) {
        ప్రతిజ్ఞా(p);

        siphash24_compress_typesafe(*p, state);
}
#అయితే_ముగింపు

పూర్ణాంకం devt_compare_func(స్థిరరాశి dev_t *a, స్థిరరాశి dev_t *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = CMP(major(*a), major(*b));
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి CMP(minor(*a), minor(*b));
}

DEFINE_HASH_OPS(devt_hash_ops, dev_t, devt_hash_func, devt_compare_func);
