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

const var_na* capability_to_name(ཧྲིལ id) {
        གལ་ཏེ (id < 0)
                ཕྱིར NULL;
        གལ་ཏེ ((unsigned) id >= capability_list_length())
                ཕྱིར NULL;

        ཕྱིར capability_names[id];
}

const var_na* capability_to_string(ཧྲིལ id, var_na buf[static CAPABILITY_TO_STRING_MAX]) {
        const var_na *p;

        གལ་ཏེ (id < 0)
                ཕྱིར NULL;
        གལ་ཏེ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ཕྱིར NULL;

        p = capability_to_name(id);
        གལ་ཏེ (p)
                ཕྱིར p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ཕྱིར buf;
}

ཧྲིལ capability_from_name(const var_na *name) {
        const sa_nracanaa capability_name *sc;
        ཧྲིལ r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        གལ་ཏེ (r >= 0) {
                གལ་ཏེ (i < 0 || i > CAP_LIMIT)
                        ཕྱིར -EINVAL;

                ཕྱིར i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        གལ་ཏེ (!sc)
                ཕྱིར -EINVAL;

        ཕྱིར sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        ཕྱིར MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ཧྲིལ capability_set_to_string(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;

        assert(ret);

        ཆེད་དུ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                གལ་ཏེ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                གལ་ཏེ (!strextend_with_separator(&str, " ", p))
                        ཕྱིར -ENOMEM;
        }

        གལ་ཏེ (!str) {
                str = new0(var_na, 1);
                གལ་ཏེ (!str)
                        ཕྱིར -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ཕྱིར 0;
}

ཧྲིལ capability_set_to_string_negative(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *a = NULL, *b = NULL;
        ཧྲིལ r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        དེ་མིན {
                var_na *c = strjoin("~", b);
                གལ་ཏེ (!c)
                        ཕྱིར -ENOMEM;

                *ret = c;
        }

        ཕྱིར 0;
}

ཧྲིལ capability_set_to_strv(uint64_t set, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ཧྲིལ r;

        assert(ret);

        ཆེད་དུ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                གལ་ཏེ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        *ret = TAKE_PTR(l);
        ཕྱིར 0;
}

ཧྲིལ capability_set_from_string(const var_na *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ཆེད་དུ (const var_na *p = s;;) {
                _cleanup_free_ var_na *word = NULL;
                ཧྲིལ r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                r = capability_from_name(word);
                གལ་ཏེ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } དེ་མིན
                        val |= UINT64_C(1) << r;
        }

        གལ་ཏེ (ret)
                *ret = val;

        ཕྱིར good;
}
