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
        ᮚᮦᮔ᮪ (id < 0)
                ᮝᮜᮤ NULL;
        ᮚᮦᮔ᮪ ((unsigned) id >= capability_list_length())
                ᮝᮜᮤ NULL;

        ᮝᮜᮤ capability_names[id];
}

const var_na* capability_to_string(puur_naa_nka id, var_na buf[static CAPABILITY_TO_STRING_MAX]) {
        const var_na *p;

        ᮚᮦᮔ᮪ (id < 0)
                ᮝᮜᮤ NULL;
        ᮚᮦᮔ᮪ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ᮝᮜᮤ NULL;

        p = capability_to_name(id);
        ᮚᮦᮔ᮪ (p)
                ᮝᮜᮤ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ᮝᮜᮤ buf;
}

puur_naa_nka capability_from_name(const var_na *name) {
        const sa_nracanaa capability_name *sc;
        puur_naa_nka r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ᮚᮦᮔ᮪ (r >= 0) {
                ᮚᮦᮔ᮪ (i < 0 || i > CAP_LIMIT)
                        ᮝᮜᮤ -EINVAL;

                ᮝᮜᮤ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ᮚᮦᮔ᮪ (!sc)
                ᮝᮜᮤ -EINVAL;

        ᮝᮜᮤ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        ᮝᮜᮤ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

puur_naa_nka capability_set_to_string(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;

        assert(ret);

        for (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ᮚᮦᮔ᮪ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                ᮚᮦᮔ᮪ (!strextend_with_separator(&str, " ", p))
                        ᮝᮜᮤ -ENOMEM;
        }

        ᮚᮦᮔ᮪ (!str) {
                str = new0(var_na, 1);
                ᮚᮦᮔ᮪ (!str)
                        ᮝᮜᮤ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ᮝᮜᮤ 0;
}

puur_naa_nka capability_set_to_string_negative(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *a = NULL, *b = NULL;
        puur_naa_nka r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        ᮜᮤᮚ {
                var_na *c = strjoin("~", b);
                ᮚᮦᮔ᮪ (!c)
                        ᮝᮜᮤ -ENOMEM;

                *ret = c;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka capability_set_to_strv(uint64_t set, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        puur_naa_nka r;

        assert(ret);

        for (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ᮚᮦᮔ᮪ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        *ret = TAKE_PTR(l);
        ᮝᮜᮤ 0;
}

puur_naa_nka capability_set_from_string(const var_na *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        for (const var_na *p = s;;) {
                _cleanup_free_ var_na *word = NULL;
                puur_naa_nka r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (r == 0)
                        bha_nga;

                r = capability_from_name(word);
                ᮚᮦᮔ᮪ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } ᮜᮤᮚ
                        val |= UINT64_C(1) << r;
        }

        ᮚᮦᮔ᮪ (ret)
                *ret = val;

        ᮝᮜᮤ good;
}
