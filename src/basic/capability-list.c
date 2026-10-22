/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "alloc-util.h"
#include "bitfield.h"
#include "capability-list.h"
#include "capability-util.h"
#include "extract-word.h"
#include "log.h"
#include "parse-util.h"
#include "string-util.h"
#include "strv.h"

static const sa_nracanaa capability_name* lookup_capability(pa_njikaa const var_na *str, pa_njikaa GPERF_LEN_TYPE len);

#include "capability-from-name.inc"
#include "capability-to-name.inc"

const var_na* capability_to_name(puur_naa_nka id) {
        ꦪꦼꦤ꧀ (id < 0)
                ꦮꦭꦶ NULL;
        ꦪꦼꦤ꧀ ((unsigned) id >= capability_list_length())
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ capability_names[id];
}

const var_na* capability_to_string(puur_naa_nka id, var_na buf[static CAPABILITY_TO_STRING_MAX]) {
        const var_na *p;

        ꦪꦼꦤ꧀ (id < 0)
                ꦮꦭꦶ NULL;
        ꦪꦼꦤ꧀ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ꦮꦭꦶ NULL;

        p = capability_to_name(id);
        ꦪꦼꦤ꧀ (p)
                ꦮꦭꦶ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ꦮꦭꦶ buf;
}

puur_naa_nka capability_from_name(const var_na *name) {
        const sa_nracanaa capability_name *sc;
        puur_naa_nka r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ꦪꦼꦤ꧀ (r >= 0) {
                ꦪꦼꦤ꧀ (i < 0 || i > CAP_LIMIT)
                        ꦮꦭꦶ -EINVAL;

                ꦮꦭꦶ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ꦪꦼꦤ꧀ (!sc)
                ꦮꦭꦶ -EINVAL;

        ꦮꦭꦶ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        ꦮꦭꦶ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

puur_naa_nka capability_set_to_string(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;

        assert(ret);

        ꦏꦁꦒꦺꦴ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ꦪꦼꦤ꧀ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                ꦪꦼꦤ꧀ (!strextend_with_separator(&str, " ", p))
                        ꦮꦭꦶ -ENOMEM;
        }

        ꦪꦼꦤ꧀ (!str) {
                str = new0(var_na, 1);
                ꦪꦼꦤ꧀ (!str)
                        ꦮꦭꦶ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ꦮꦭꦶ 0;
}

puur_naa_nka capability_set_to_string_negative(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *a = NULL, *b = NULL;
        puur_naa_nka r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        ꦭꦶꦪ {
                var_na *c = strjoin("~", b);
                ꦪꦼꦤ꧀ (!c)
                        ꦮꦭꦶ -ENOMEM;

                *ret = c;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka capability_set_to_strv(uint64_t set, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(ret);

        ꦏꦁꦒꦺꦴ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ꦪꦼꦤ꧀ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        *ret = TAKE_PTR(l);
        ꦮꦭꦶ 0;
}

puur_naa_nka capability_set_from_string(const var_na *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ꦏꦁꦒꦺꦴ (const var_na *p = s;;) {
                _cleanup_free_ var_na *word = NULL;
                puur_naa_nka r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                r = capability_from_name(word);
                ꦪꦼꦤ꧀ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } ꦭꦶꦪ
                        val |= UINT64_C(1) << r;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = val;

        ꦮꦭꦶ good;
}
