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

স্থির ধ্রুব সংরচনা capability_name* lookup_capability(পঞ্জিকা ধ্রুব বর্ণ *str, পঞ্জিকা GPERF_LEN_TYPE len);

#অন্তর্ভাব "capability-from-name.inc"
#অন্তর্ভাব "capability-to-name.inc"

ধ্রুব বর্ণ* capability_to_name(পূর্ণাঙ্ক id) {
        যদি (id < 0)
                প্রত্যাগম রিক্ত;
        যদি ((unsigned) id >= capability_list_length())
                প্রত্যাগম রিক্ত;

        প্রত্যাগম capability_names[id];
}

ধ্রুব বর্ণ* capability_to_string(পূর্ণাঙ্ক id, বর্ণ buf[স্থির CAPABILITY_TO_STRING_MAX]) {
        ধ্রুব বর্ণ *p;

        যদি (id < 0)
                প্রত্যাগম রিক্ত;
        যদি (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                প্রত্যাগম রিক্ত;

        p = capability_to_name(id);
        যদি (p)
                প্রত্যাগম p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        প্রত্যাগম buf;
}

পূর্ণাঙ্ক capability_from_name(ধ্রুব বর্ণ *name) {
        ধ্রুব সংরচনা capability_name *sc;
        পূর্ণাঙ্ক r, i;

        প্রতিজ্ঞা(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        যদি (r >= 0) {
                যদি (i < 0 || i > CAP_LIMIT)
                        প্রত্যাগম -EINVAL;

                প্রত্যাগম i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        যদি (!sc)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(শূন্য) {
        প্রত্যাগম MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

পূর্ণাঙ্ক capability_set_to_string(uint64_t set, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *str = রিক্ত;

        প্রতিজ্ঞা(ret);

        প্রতি (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুব বর্ণ *p;

                যদি (!BIT_SET(set, i))
                        অনুবর্ত;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                যদি (!strextend_with_separator(&str, " ", p))
                        প্রত্যাগম -ENOMEM;
        }

        যদি (!str) {
                str = new0(বর্ণ, 1);
                যদি (!str)
                        প্রত্যাগম -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক capability_set_to_string_negative(uint64_t set, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *a = রিক্ত, *b = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        যদি (r < 0)
                প্রত্যাগম r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        অন্যথা {
                বর্ণ *c = strjoin("~", b);
                যদি (!c)
                        প্রত্যাগম -ENOMEM;

                *ret = c;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক capability_set_to_strv(uint64_t set, বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        প্রতি (unsigned i = 0; i <= cap_last_cap(); i++) {
                ধ্রুব বর্ণ *p;

                যদি (!BIT_SET(set, i))
                        অনুবর্ত;

                p = CAPABILITY_TO_STRING(i);
                প্রতিজ্ঞা(p);

                r = strv_extend(&l, p);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        *ret = TAKE_PTR(l);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক capability_set_from_string(ধ্রুব বর্ণ *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        প্রতি (ধ্রুব বর্ণ *p = s;;) {
                _cleanup_free_ বর্ণ *word = রিক্ত;
                পূর্ণাঙ্ক r;

                r = extract_first_word(&p, &word, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                যদি (r < 0)
                        প্রত্যাগম r;
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

        প্রত্যাগম good;
}
