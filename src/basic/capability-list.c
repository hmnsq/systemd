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

static const struct capability_name* lookup_capability(register const char *str, register GPERF_LEN_TYPE len);

#include "capability-from-name.inc"
#include "capability-to-name.inc"

const char* capability_to_name(số_nguyên id) {
        nếu (id < 0)
                trả_về NULL;
        nếu ((unsigned) id >= capability_list_length())
                trả_về NULL;

        trả_về capability_names[id];
}

const char* capability_to_string(số_nguyên id, char buf[static CAPABILITY_TO_STRING_MAX]) {
        const char *p;

        nếu (id < 0)
                trả_về NULL;
        nếu (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                trả_về NULL;

        p = capability_to_name(id);
        nếu (p)
                trả_về p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        trả_về buf;
}

số_nguyên capability_from_name(const char *name) {
        const struct capability_name *sc;
        số_nguyên r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        nếu (r >= 0) {
                nếu (i < 0 || i > CAP_LIMIT)
                        trả_về -EINVAL;

                trả_về i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        nếu (!sc)
                trả_về -EINVAL;

        trả_về sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        trả_về MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

số_nguyên capability_set_to_string(uint64_t set, char **ret) {
        _cleanup_free_ char *str = NULL;

        assert(ret);

        cho (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                nếu (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                nếu (!strextend_with_separator(&str, " ", p))
                        trả_về -ENOMEM;
        }

        nếu (!str) {
                str = new0(char, 1);
                nếu (!str)
                        trả_về -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        trả_về 0;
}

số_nguyên capability_set_to_string_negative(uint64_t set, char **ret) {
        _cleanup_free_ char *a = NULL, *b = NULL;
        số_nguyên r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        nếu (r < 0)
                trả_về r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        nếu (r < 0)
                trả_về r;

        nếu (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        không_thì {
                char *c = strjoin("~", b);
                nếu (!c)
                        trả_về -ENOMEM;

                *ret = c;
        }

        trả_về 0;
}

số_nguyên capability_set_to_strv(uint64_t set, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        số_nguyên r;

        assert(ret);

        cho (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                nếu (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                nếu (r < 0)
                        trả_về r;
        }

        *ret = TAKE_PTR(l);
        trả_về 0;
}

số_nguyên capability_set_from_string(const char *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        cho (const char *p = s;;) {
                _cleanup_free_ char *word = NULL;
                số_nguyên r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                r = capability_from_name(word);
                nếu (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } không_thì
                        val |= UINT64_C(1) << r;
        }

        nếu (ret)
                *ret = val;

        trả_về good;
}
