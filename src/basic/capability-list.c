/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "bitfield.h"
#অন্তর্ভাব "capability-list.h"
#অন্তর্ভাব "capability-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

স্থির ধ্রুবক কাঠামো capability_name* lookup_capability(নিবন্ধ ধ্রুবক অক্ষর *str, নিবন্ধ GPERF_LEN_TYPE len);

#অন্তর্ভাব "capability-from-name.inc"
#অন্তর্ভাব "capability-to-name.inc"

ধ্রুবক অক্ষর* capability_to_name(পূর্ণসংখ্যা id) {
        যদি (id < 0)
                ফেরত ফাঁকা;
        যদি ((unsigned) id >= capability_list_দৈর্ঘ্য())
                ফেরত ফাঁকা;

        ফেরত capability_names[id];
}

ধ্রুবক অক্ষর* capability_to_string(পূর্ণসংখ্যা id, অক্ষর buf[স্থির CAPABILITY_TO_STRING_MAX]) {
        ধ্রুবক অক্ষর *p;

        যদি (id < 0)
                ফেরত ফাঁকা;
        যদি (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ফেরত ফাঁকা;

        p = capability_to_name(id);
        যদি (p)
                ফেরত p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ফেরত buf;
}

পূর্ণসংখ্যা capability_from_name(ধ্রুবক অক্ষর *name) {
        ধ্রুবক কাঠামো capability_name *sc;
        পূর্ণসংখ্যা r, i;

        প্রতিজ্ঞা(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        যদি (r >= 0) {
                যদি (i < 0 || i > CAP_LIMIT)
                        ফেরত -EINVAL;

                ফেরত i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        যদি (!sc)
                ফেরত -EINVAL;

        ফেরত sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_দৈর্ঘ্য(শূন্য) {
        ফেরত MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

পূর্ণসংখ্যা capability_set_to_string(uint64_t set, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *str = ফাঁকা;

        প্রতিজ্ঞা(ret);

        জন্য (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুবক অক্ষর *p;

                যদি (!BIT_SET(set, i))
                        চালু;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                যদি (!strextend_with_separator(&str, " ", p))
                        ফেরত -ENOMEM;
        }

        যদি (!str) {
                str = new0(অক্ষর, 1);
                যদি (!str)
                        ফেরত -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ফেরত 0;
}

পূর্ণসংখ্যা capability_set_to_string_negative(uint64_t set, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *a = ফাঁকা, *b = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        যদি (r < 0)
                ফেরত r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        যদি (r < 0)
                ফেরত r;

        যদি (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        অন্যথা {
                অক্ষর *c = strjoin("~", b);
                যদি (!c)
                        ফেরত -ENOMEM;

                *ret = c;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা capability_set_to_strv(uint64_t set, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        জন্য (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুবক অক্ষর *p;

                যদি (!BIT_SET(set, i))
                        চালু;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                r = strv_extend(&l, p);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(l);
        ফেরত 0;
}

পূর্ণসংখ্যা capability_set_from_string(ধ্রুবক অক্ষর *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        জন্য (ধ্রুবক অক্ষর *p = s;;) {
                _cleanup_free_ অক্ষর *word = ফাঁকা;
                পূর্ণসংখ্যা r;

                r = extract_first_word(&p, &word, ফাঁকা, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                r = capability_from_name(word);
                যদি (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } অন্যথা
                        val |= UINT64_C(1) << r;
        }

        যদি (ret)
                *ret = val;

        ফেরত good;
}
