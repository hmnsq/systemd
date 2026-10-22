/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "bitfield.h"
#অন্তর্ভাৱ "capability-list.h"
#অন্তর্ভাৱ "capability-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

স্থির ধ্রুৱ সংরচনা capability_name* lookup_capability(পঞ্জিকা ধ্রুৱ ৱর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাৱ "capability-from-name.inc"
#অন্তর্ভাৱ "capability-to-name.inc"

ধ্রুৱ ৱর্ণ* capability_to_name(পূর্ণাঙ্ক id) {
        যদি (id < 0)
                ঘূৰাই রিক্ত;
        যদি ((unsigned) id >= capability_list_length())
                ঘূৰাই রিক্ত;

        ঘূৰাই capability_names[id];
}

ধ্রুৱ ৱর্ণ* capability_to_string(পূর্ণাঙ্ক id, ৱর্ণ buf[স্থির CAPABILITY_TO_STRING_MAX]) {
        ধ্রুৱ ৱর্ণ *p;

        যদি (id < 0)
                ঘূৰাই রিক্ত;
        যদি (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ঘূৰাই রিক্ত;

        p = capability_to_name(id);
        যদি (p)
                ঘূৰাই p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ঘূৰাই buf;
}

পূর্ণাঙ্ক capability_from_name(ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ সংরচনা capability_name *sc;
        পূর্ণাঙ্ক r, i;

        প্রতিজ্ঞা(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        যদি (r >= 0) {
                যদি (i < 0 || i > CAP_LIMIT)
                        ঘূৰাই -EINVAL;

                ঘূৰাই i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        যদি (!sc)
                ঘূৰাই -EINVAL;

        ঘূৰাই sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(শূন্য) {
        ঘূৰাই MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

পূর্ণাঙ্ক capability_set_to_string(uint64_t set, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *str = রিক্ত;

        প্রতিজ্ঞা(ret);

        বাবে (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুৱ ৱর্ণ *p;

                যদি (!BIT_SET(set, i))
                        অনুৱর্ত;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                যদি (!strextend_with_separator(&str, " ", p))
                        ঘূৰাই -ENOMEM;
        }

        যদি (!str) {
                str = new0(ৱর্ণ, 1);
                যদি (!str)
                        ঘূৰাই -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক capability_set_to_string_negative(uint64_t set, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *a = রিক্ত, *b = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        যদি (r < 0)
                ঘূৰাই r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        অন্যথা {
                ৱর্ণ *c = strjoin("~", b);
                যদি (!c)
                        ঘূৰাই -ENOMEM;

                *ret = c;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক capability_set_to_strv(uint64_t set, ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        বাবে (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুৱ ৱর্ণ *p;

                যদি (!BIT_SET(set, i))
                        অনুৱর্ত;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                r = strv_extend(&l, p);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(l);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক capability_set_from_string(ধ্রুৱ ৱর্ণ *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        বাবে (ধ্রুৱ ৱর্ণ *p = s;;) {
                _cleanup_free_ ৱর্ণ *word = রিক্ত;
                পূর্ণাঙ্ক r;

                r = extract_first_word(&p, &word, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                r = capability_from_name(word);
                যদি (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } অন্যথা
                        val |= UINT64_C(1) << r;
        }

        যদি (ret)
                *ret = val;

        ঘূৰাই good;
}
