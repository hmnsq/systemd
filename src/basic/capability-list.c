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

const char* capability_to_name(整数 id) {
        如果 (id < 0)
                返回 NULL;
        如果 ((unsigned) id >= capability_list_length())
                返回 NULL;

        返回 capability_names[id];
}

const char* capability_to_string(整数 id, char buf[static CAPABILITY_TO_STRING_MAX]) {
        const char *p;

        如果 (id < 0)
                返回 NULL;
        如果 (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                返回 NULL;

        p = capability_to_name(id);
        如果 (p)
                返回 p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        返回 buf;
}

整数 capability_from_name(const char *name) {
        const struct capability_name *sc;
        整数 r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        如果 (r >= 0) {
                如果 (i < 0 || i > CAP_LIMIT)
                        返回 -EINVAL;

                返回 i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        如果 (!sc)
                返回 -EINVAL;

        返回 sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        返回 MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

整数 capability_set_to_string(uint64_t set, char **ret) {
        _cleanup_free_ char *str = NULL;

        assert(ret);

        为 (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                如果 (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                如果 (!strextend_with_separator(&str, " ", p))
                        返回 -ENOMEM;
        }

        如果 (!str) {
                str = new0(char, 1);
                如果 (!str)
                        返回 -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        返回 0;
}

整数 capability_set_to_string_negative(uint64_t set, char **ret) {
        _cleanup_free_ char *a = NULL, *b = NULL;
        整数 r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        如果 (r < 0)
                返回 r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        如果 (r < 0)
                返回 r;

        如果 (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        否则 {
                char *c = strjoin("~", b);
                如果 (!c)
                        返回 -ENOMEM;

                *ret = c;
        }

        返回 0;
}

整数 capability_set_to_strv(uint64_t set, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        整数 r;

        assert(ret);

        为 (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                如果 (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                如果 (r < 0)
                        返回 r;
        }

        *ret = TAKE_PTR(l);
        返回 0;
}

整数 capability_set_from_string(const char *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        为 (const char *p = s;;) {
                _cleanup_free_ char *word = NULL;
                整数 r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                r = capability_from_name(word);
                如果 (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } 否则
                        val |= UINT64_C(1) << r;
        }

        如果 (ret)
                *ret = val;

        返回 good;
}
