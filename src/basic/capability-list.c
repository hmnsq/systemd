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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ capability_name* lookup_capability(ನೋಂದಣಿ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ನೋಂದಣಿ GPERF_LEN_TYPE len);

#ಅನ್ತರ್ಭಾವ "capability-from-name.inc"
#ಅನ್ತರ್ಭಾವ "capability-to-name.inc"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* capability_ಗಮ್ಯ_name(ಪೂರ್ಣಾಂಕ id) {
        ಆದರೆ (id < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ) id >= capability_list_ಉದ್ದಳತೆ())
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ capability_ಹೆಸರುಗಳು[id];
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* capability_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ id, ಅಕ್ಷರ buf[ಸ್ಥಿರ CAPABILITY_ಗಮ್ಯ_STRING_MAX]) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಆದರೆ (id < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ (id > CAP_ಎಲ್ಲೆ) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        p = capability_ಗಮ್ಯ_name(id);
        ಆದರೆ (p)
                ಹಿಂತಿರುಗಿ p;

        sprintf(buf, "0x%x", (ಚಿಹ್ನೆರಹಿತ) id); /* numerical fallback */
        ಹಿಂತಿರುಗಿ buf;
}

ಪೂರ್ಣಾಂಕ capability_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ capability_name *sc;
        ಪೂರ್ಣಾಂಕ r, i;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Try to parse numeric capability */
        r = ಸುರಕ್ಷಿತ_atoi(name, &i);
        ಆದರೆ (r >= 0) {
                ಆದರೆ (i < 0 || i > CAP_ಎಲ್ಲೆ)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಹಿಂತಿರುಗಿ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
        ಆದರೆ (!sc)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
ಚಿಹ್ನೆರಹಿತ capability_list_ಉದ್ದಳತೆ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ MIN((ಚಿಹ್ನೆರಹಿತ) ELEMENTSOF(capability_ಹೆಸರುಗಳು), (ಚಿಹ್ನೆರಹಿತ) (CAP_ಎಲ್ಲೆ + 1));
}

ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_string(uint64_t ಗಣ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *str = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

                ಆದರೆ (!ಬಿಟ್_ಗಣ(ಗಣ, i))
                        ಮುಂದುವರಿ;

                p = CAPABILITY_ಗಮ್ಯ_STRING(i);
                ಪ್ರತಿಜ್ಞಾ(p);

                ಆದರೆ (!strextend_with_separator(&str, " ", p))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        ಆದರೆ (!str) {
                str = new0(ಅಕ್ಷರ, 1);
                ಆದರೆ (!str)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_string_negative(uint64_t ಗಣ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *a = ಖಾಲಿ, *b = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_ಗಣ_ಗಮ್ಯ_string(ಗಣ, &a);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = capability_ಗಣ_ಗಮ್ಯ_string(~ಗಣ & all_capabilities(), &b);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(a) <= 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(b))
                *ret = TAKE_PTR(a);
        ಇಲ್ಲದಿದ್ದರೆ {
                ಅಕ್ಷರ *c = strjoin("~", b);
                ಆದರೆ (!c)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret = c;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ capability_ಗಣ_ಗಮ್ಯ_strv(uint64_t ಗಣ, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= cap_ಕೊನೆಯ_cap(); i++) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

                ಆದರೆ (!ಬಿಟ್_ಗಣ(ಗಣ, i))
                        ಮುಂದುವರಿ;

                p = CAPABILITY_ಗಮ್ಯ_STRING(i);
                ಪ್ರತಿಜ್ಞಾ(p);

                r = strv_extend(&l, p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ capability_ಗಣ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = s;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ;
                ಪೂರ್ಣಾಂಕ r;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&p, &ಶಬ್ದ, ಖಾಲಿ, ಹೊರತೆಗೆ_UNQUOTE|ಹೊರತೆಗೆ_RELAX);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                r = capability_from_name(ಶಬ್ದ);
                ಆದರೆ (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", ಶಬ್ದ);
                        good = false;
                } ಇಲ್ಲದಿದ್ದರೆ
                        val |= UINT64_C(1) << r;
        }

        ಆದರೆ (ret)
                *ret = val;

        ಹಿಂತಿರುಗಿ good;
}
