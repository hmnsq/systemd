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

const var_na* capability_to_name(ចំនួនគត់ id) {
        ប្រសិនបើ (id < 0)
                ត្រឡប់ NULL;
        ប្រសិនបើ ((unsigned) id >= capability_list_length())
                ត្រឡប់ NULL;

        ត្រឡប់ capability_names[id];
}

const var_na* capability_to_string(ចំនួនគត់ id, var_na buf[static CAPABILITY_TO_STRING_MAX]) {
        const var_na *p;

        ប្រសិនបើ (id < 0)
                ត្រឡប់ NULL;
        ប្រសិនបើ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ត្រឡប់ NULL;

        p = capability_to_name(id);
        ប្រសិនបើ (p)
                ត្រឡប់ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ត្រឡប់ buf;
}

ចំនួនគត់ capability_from_name(const var_na *name) {
        const sa_nracanaa capability_name *sc;
        ចំនួនគត់ r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ប្រសិនបើ (r >= 0) {
                ប្រសិនបើ (i < 0 || i > CAP_LIMIT)
                        ត្រឡប់ -EINVAL;

                ត្រឡប់ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ប្រសិនបើ (!sc)
                ត្រឡប់ -EINVAL;

        ត្រឡប់ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        ត្រឡប់ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ចំនួនគត់ capability_set_to_string(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;

        assert(ret);

        សម្រាប់ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ប្រសិនបើ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                ប្រសិនបើ (!strextend_with_separator(&str, " ", p))
                        ត្រឡប់ -ENOMEM;
        }

        ប្រសិនបើ (!str) {
                str = new0(var_na, 1);
                ប្រសិនបើ (!str)
                        ត្រឡប់ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ត្រឡប់ 0;
}

ចំនួនគត់ capability_set_to_string_negative(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *a = NULL, *b = NULL;
        ចំនួនគត់ r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        បើមិនដូច្នេះ {
                var_na *c = strjoin("~", b);
                ប្រសិនបើ (!c)
                        ត្រឡប់ -ENOMEM;

                *ret = c;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ capability_set_to_strv(uint64_t set, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ចំនួនគត់ r;

        assert(ret);

        សម្រាប់ (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                ប្រសិនបើ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        *ret = TAKE_PTR(l);
        ត្រឡប់ 0;
}

ចំនួនគត់ capability_set_from_string(const var_na *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        សម្រាប់ (const var_na *p = s;;) {
                _cleanup_free_ var_na *word = NULL;
                ចំនួនគត់ r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        bha_nga;

                r = capability_from_name(word);
                ប្រសិនបើ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } បើមិនដូច្នេះ
                        val |= UINT64_C(1) << r;
        }

        ប្រសិនបើ (ret)
                *ret = val;

        ត្រឡប់ good;
}
