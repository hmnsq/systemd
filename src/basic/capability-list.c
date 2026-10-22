/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "capability-list.h"
#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

நிலையான மாறிலி கட்டமைப்பு capability_name* lookup_capability(பதிவு மாறிலி எழுத்து *str, பதிவு GPERF_LEN_TYPE len);

#அந்தர்பாவ "capability-from-name.inc"
#அந்தர்பாவ "capability-to-name.inc"

மாறிலி எழுத்து* capability_to_name(முழு id) {
        எனில் (id < 0)
                திரும்பு வெற்று;
        எனில் ((unsigned) id >= capability_list_length())
                திரும்பு வெற்று;

        திரும்பு capability_names[id];
}

மாறிலி எழுத்து* capability_to_string(முழு id, எழுத்து buf[நிலையான CAPABILITY_TO_STRING_MAX]) {
        மாறிலி எழுத்து *p;

        எனில் (id < 0)
                திரும்பு வெற்று;
        எனில் (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                திரும்பு வெற்று;

        p = capability_to_name(id);
        எனில் (p)
                திரும்பு p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        திரும்பு buf;
}

முழு capability_from_name(மாறிலி எழுத்து *name) {
        மாறிலி கட்டமைப்பு capability_name *sc;
        முழு r, i;

        ப்ரதிஜ்ஞா(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        எனில் (r >= 0) {
                எனில் (i < 0 || i > CAP_LIMIT)
                        திரும்பு -EINVAL;

                திரும்பு i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        எனில் (!sc)
                திரும்பு -EINVAL;

        திரும்பு sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(வெறுமை) {
        திரும்பு MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

முழு capability_set_to_string(uint64_t set, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *str = வெற்று;

        ப்ரதிஜ்ஞா(ret);

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                மாறிலி எழுத்து *p;

                எனில் (!BIT_SET(set, i))
                        தொடர்;

                p = CAPABILITY_TO_STRING(i);
                ப்ரதிஜ்ஞா(p);

                எனில் (!strextend_with_separator(&str, " ", p))
                        திரும்பு -ENOMEM;
        }

        எனில் (!str) {
                str = new0(எழுத்து, 1);
                எனில் (!str)
                        திரும்பு -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        திரும்பு 0;
}

முழு capability_set_to_string_negative(uint64_t set, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *a = வெற்று, *b = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        எனில் (r < 0)
                திரும்பு r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        இல்லையெனில் {
                எழுத்து *c = strjoin("~", b);
                எனில் (!c)
                        திரும்பு -ENOMEM;

                *ret = c;
        }

        திரும்பு 0;
}

முழு capability_set_to_strv(uint64_t set, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        க்கு (unsigned i = 0; i <= cap_last_cap(); i++) {
                மாறிலி எழுத்து *p;

                எனில் (!BIT_SET(set, i))
                        தொடர்;

                p = CAPABILITY_TO_STRING(i);
                ப்ரதிஜ்ஞா(p);

                r = strv_extend(&l, p);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(l);
        திரும்பு 0;
}

முழு capability_set_from_string(மாறிலி எழுத்து *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        க்கு (மாறிலி எழுத்து *p = s;;) {
                _cleanup_free_ எழுத்து *word = வெற்று;
                முழு r;

                r = extract_first_word(&p, &word, வெற்று, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                r = capability_from_name(word);
                எனில் (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } இல்லையெனில்
                        val |= UINT64_C(1) << r;
        }

        எனில் (ret)
                *ret = val;

        திரும்பு good;
}
