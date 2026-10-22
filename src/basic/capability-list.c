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

const char* capability_to_name(цяло id) {
        ако (id < 0)
                върни NULL;
        ако ((unsigned) id >= capability_list_length())
                върни NULL;

        върни capability_names[id];
}

const char* capability_to_string(цяло id, char buf[static CAPABILITY_TO_STRING_MAX]) {
        const char *p;

        ако (id < 0)
                върни NULL;
        ако (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                върни NULL;

        p = capability_to_name(id);
        ако (p)
                върни p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        върни buf;
}

цяло capability_from_name(const char *name) {
        const struct capability_name *sc;
        цяло r, i;

        assert(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ако (r >= 0) {
                ако (i < 0 || i > CAP_LIMIT)
                        върни -EINVAL;

                върни i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ако (!sc)
                върни -EINVAL;

        върни sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(void) {
        върни MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

цяло capability_set_to_string(uint64_t set, char **ret) {
        _cleanup_free_ char *str = NULL;

        assert(ret);

        за (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                ако (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                ако (!strextend_with_separator(&str, " ", p))
                        върни -ENOMEM;
        }

        ако (!str) {
                str = new0(char, 1);
                ако (!str)
                        върни -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        върни 0;
}

цяло capability_set_to_string_negative(uint64_t set, char **ret) {
        _cleanup_free_ char *a = NULL, *b = NULL;
        цяло r;

        assert(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ако (r < 0)
                върни r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ако (r < 0)
                върни r;

        ако (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        иначе {
                char *c = strjoin("~", b);
                ако (!c)
                        върни -ENOMEM;

                *ret = c;
        }

        върни 0;
}

цяло capability_set_to_strv(uint64_t set, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        цяло r;

        assert(ret);

        за (unsigned i = 0; i <= cap_last_cap(); i++) {
                const char *p;

                ако (!BIT_SET(set, i))
                        continue;

                p = CAPABILITY_TO_STRING(i);
                assert(p);

                r = strv_extend(&l, p);
                ако (r < 0)
                        върни r;
        }

        *ret = TAKE_PTR(l);
        върни 0;
}

цяло capability_set_from_string(const char *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        за (const char *p = s;;) {
                _cleanup_free_ char *word = NULL;
                цяло r;

                r = extract_first_word(&p, &word, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ако (r < 0)
                        върни r;
                ако (r == 0)
                        break;

                r = capability_from_name(word);
                ако (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } иначе
                        val |= UINT64_C(1) << r;
        }

        ако (ret)
                *ret = val;

        върни good;
}
