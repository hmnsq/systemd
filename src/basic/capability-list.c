/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "capability-list.h"
#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ capability_name* lookup_capability(ಪಞ್ಜಿಕಾ ಧ್ರುವ ವರ್ಣ *str, ಪಞ್ಜಿಕಾ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "capability-from-name.inc"
#ಅನ್ತರ್ಭಾವ "capability-to-name.inc"

ಧ್ರುವ ವರ್ಣ* capability_to_name(ಪೂರ್ಣಾಙ್ಕ id) {
        ಯದಿ (id < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಯದಿ ((unsigned) id >= capability_list_length())
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ capability_names[id];
}

ಧ್ರುವ ವರ್ಣ* capability_to_string(ಪೂರ್ಣಾಙ್ಕ id, ವರ್ಣ buf[ಸ್ಥಿರ CAPABILITY_TO_STRING_MAX]) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಯದಿ (id < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಯದಿ (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        p = capability_to_name(id);
        ಯದಿ (p)
                ಪ್ರತ್ಯಾಗಮ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ಪ್ರತ್ಯಾಗಮ buf;
}

ಪೂರ್ಣಾಙ್ಕ capability_from_name(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ಸಂರಚನಾ capability_name *sc;
        ಪೂರ್ಣಾಙ್ಕ r, i;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ಯದಿ (r >= 0) {
                ಯದಿ (i < 0 || i > CAP_LIMIT)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಪ್ರತ್ಯಾಗಮ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ಯದಿ (!sc)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ಪೂರ್ಣಾಙ್ಕ capability_set_to_string(uint64_t set, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *str = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ಧ್ರುವ ವರ್ಣ *p;

                ಯದಿ (!BIT_SET(set, i))
                        ಅನುವರ್ತ;

                p = CAPABILITY_TO_STRING(i);
                ಪ್ರತಿಜ್ಞಾ(p);

                ಯದಿ (!strextend_with_separator(&str, " ", p))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        ಯದಿ (!str) {
                str = new0(ವರ್ಣ, 1);
                ಯದಿ (!str)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ capability_set_to_string_negative(uint64_t set, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *a = ರಿಕ್ತ, *b = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        ಅನ್ಯಥಾ {
                ವರ್ಣ *c = strjoin("~", b);
                ಯದಿ (!c)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret = c;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ capability_set_to_strv(uint64_t set, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ಧ್ರುವ ವರ್ಣ *p;

                ಯದಿ (!BIT_SET(set, i))
                        ಅನುವರ್ತ;

                p = CAPABILITY_TO_STRING(i);
                ಪ್ರತಿಜ್ಞಾ(p);

                r = strv_extend(&l, p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ capability_set_from_string(ಧ್ರುವ ವರ್ಣ *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = s;;) {
                _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ;
                ಪೂರ್ಣಾಙ್ಕ r;

                r = extract_first_word(&p, &word, ರಿಕ್ತ, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                r = capability_from_name(word);
                ಯದಿ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } ಅನ್ಯಥಾ
                        val |= UINT64_C(1) << r;
        }

        ಯದಿ (ret)
                *ret = val;

        ಪ್ರತ್ಯಾಗಮ good;
}
