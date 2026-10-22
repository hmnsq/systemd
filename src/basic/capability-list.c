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

const char* capability_to_name(ціле id) {
        якщо (id < 0)
                повернути NULL;
        якщо ((unsigned) id >= capability_list_length())
                повернути NULL;

        повернути capability_names[id];
}

const char* capability_to_string(ціле id, char buf[static CAPABILITY_TO_STRING_MAX]) {
        const char *p;

        якщо (id < 0)
                повернути NULL;
        якщо (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                повернути NULL;

        p = capability_to_name(id);
        якщо (p)
                повернути p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        повернути buf;
}

ціле capability_from_name(const char *name) {
        const struct capability_name *sc;
        ціле r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        якщо (r >= 0) {
                якщо (i < 0 || i > CAP_LIMIT)
                        повернути -EINVAL;

                повернути i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        якщо (!sc)
                повернути -EINVAL;

        повернути sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        повернути MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ціле capability_set_to_string(uint64_t set, char **ret) {
        _cleanup_free_ char *str = NULL;

        assert(ret);

        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                якщо (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                якщо (!strextend_with_separator(&str, " ", p))
                        повернути -ENOMEM;
        }

        якщо (!str) {
                str = new0(char, 1);
                якщо (!str)
                        повернути -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        повернути 0;
}

ціле capability_set_to_string_negative(uint64_t set, char **ret) {
        _cleanup_free_ char *a = NULL, *b = NULL;
        ціле r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        якщо (r < 0)
                повернути r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        якщо (r < 0)
                повернути r;

        якщо (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        інакше {
                char *c = strjoin("~", b);
                якщо (!c)
                        повернути -ENOMEM;

                *ret = c;
        }

        повернути 0;
}

ціле capability_set_to_strv(uint64_t set, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        ціле r;

        assert(ret);

        для (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                якщо (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                якщо (r < 0)
                        повернути r;
        }

        *ret = TAKE_PTR(l);
        повернути 0;
}

ціле capability_set_from_string(const char *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        для (const char *p = s;;) {
                _cleanup_free_ char *word = NULL;
                ціле r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0)
                        break;

                r = capability_from_name(word);
                якщо (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } інакше
                        val |= UINT64_C(1) << r;
        }

        якщо (ret)
                *ret = val;

        повернути good;
}
