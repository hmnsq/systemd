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

const var_na* capability_to_name(ကိန်းပြည့် id) {
        အကယ်၍ (id < 0)
                ပြန်ပေး NULL;
        အကယ်၍ ((unsigned) id >= capability_list_length())
                ပြန်ပေး NULL;

        ပြန်ပေး capability_names[id];
}

const var_na* capability_to_string(ကိန်းပြည့် id, var_na buf[static CAPABILITY_TO_STRING_MAX]) {
        const var_na *p;

        အကယ်၍ (id < 0)
                ပြန်ပေး NULL;
        အကယ်၍ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ပြန်ပေး NULL;

        p = capability_to_name(id);
        အကယ်၍ (p)
                ပြန်ပေး p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ပြန်ပေး buf;
}

ကိန်းပြည့် capability_from_name(const var_na *name) {
        const sa_nracanaa capability_name *sc;
        ကိန်းပြည့် r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        အကယ်၍ (r >= 0) {
                အကယ်၍ (i < 0 || i > CAP_LIMIT)
                        ပြန်ပေး -EINVAL;

                ပြန်ပေး i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        အကယ်၍ (!sc)
                ပြန်ပေး -EINVAL;

        ပြန်ပေး sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        ပြန်ပေး MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ကိန်းပြည့် capability_set_to_string(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;

        assert(ret);

        အတွက် (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                အကယ်၍ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                အကယ်၍ (!strextend_with_separator(&str, " ", p))
                        ပြန်ပေး -ENOMEM;
        }

        အကယ်၍ (!str) {
                str = new0(var_na, 1);
                အကယ်၍ (!str)
                        ပြန်ပေး -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ပြန်ပေး 0;
}

ကိန်းပြည့် capability_set_to_string_negative(uint64_t set, var_na **ret) {
        _cleanup_free_ var_na *a = NULL, *b = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        မဟုတ်ပါက {
                var_na *c = strjoin("~", b);
                အကယ်၍ (!c)
                        ပြန်ပေး -ENOMEM;

                *ret = c;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် capability_set_to_strv(uint64_t set, var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        အတွက် (unsigned i = 0; i <= cap_last_cap(); i++) {
                const var_na *p;

                အကယ်၍ (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        *ret = TAKE_PTR(l);
        ပြန်ပေး 0;
}

ကိန်းပြည့် capability_set_from_string(const var_na *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        အတွက် (const var_na *p = s;;) {
                _cleanup_free_ var_na *word = NULL;
                ကိန်းပြည့် r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;

                r = capability_from_name(word);
                အကယ်၍ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } မဟုတ်ပါက
                        val |= UINT64_C(1) << r;
        }

        အကယ်၍ (ret)
                *ret = val;

        ပြန်ပေး good;
}
