/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "capability-list.h"
#ଅନ୍ତର୍ଭାଵ "capability-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ସ୍ଥିର ଅଚଳ ସଂରଚନା capability_name* lookup_capability(ପଞ୍ଜିକରଣ ଅଚଳ ଅକ୍ଷର *str, ପଞ୍ଜିକରଣ GPERF_LEN_TYPE len);

#ଅନ୍ତର୍ଭାଵ "capability-from-name.inc"
#ଅନ୍ତର୍ଭାଵ "capability-to-name.inc"

ଅଚଳ ଅକ୍ଷର* capability_to_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) {
        ଯଦି (id < 0)
                ଫେରନ୍ତୁ ଖାଲି;
        ଯଦି ((unsigned) id >= capability_list_length())
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ capability_names[id];
}

ଅଚଳ ଅକ୍ଷର* capability_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ଅକ୍ଷର buf[ସ୍ଥିର CAPABILITY_TO_STRING_MAX]) {
        ଅଚଳ ଅକ୍ଷର *p;

        ଯଦି (id < 0)
                ଫେରନ୍ତୁ ଖାଲି;
        ଯଦି (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                ଫେରନ୍ତୁ ଖାଲି;

        p = capability_to_name(id);
        ଯଦି (p)
                ଫେରନ୍ତୁ p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        ଫେରନ୍ତୁ buf;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_from_name(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ସଂରଚନା capability_name *sc;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, i;

        ପ୍ରତିଜ୍ଞା(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        ଯଦି (r >= 0) {
                ଯଦି (i < 0 || i > CAP_LIMIT)
                        ଫେରନ୍ତୁ -EINVAL;

                ଫେରନ୍ତୁ i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        ଯଦି (!sc)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_string(uint64_t set, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *str = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ଅଚଳ ଅକ୍ଷର *p;

                ଯଦି (!BIT_SET(set, i))
                        ଜାରି;

                p = CAPABILITY_TO_STRING(i);
                ପ୍ରତିଜ୍ଞା(p);

                ଯଦି (!strextend_with_separator(&str, " ", p))
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        ଯଦି (!str) {
                str = new0(ଅକ୍ଷର, 1);
                ଯଦି (!str)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_string_negative(uint64_t set, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *a = ଖାଲି, *b = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        ଅନ୍ୟଥା {
                ଅକ୍ଷର *c = strjoin("~", b);
                ଯଦି (!c)
                        ଫେରନ୍ତୁ -ENOMEM;

                *ret = c;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_to_strv(uint64_t set, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (unsigned i = 0; i <= cap_last_cap(); i++) {
                ଅଚଳ ଅକ୍ଷର *p;

                ଯଦି (!BIT_SET(set, i))
                        ଜାରି;

                p = CAPABILITY_TO_STRING(i);
                ପ୍ରତିଜ୍ଞା(p);

                r = strv_extend(&l, p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(l);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା capability_set_from_string(ଅଚଳ ଅକ୍ଷର *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = s;;) {
                _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = extract_first_word(&p, &word, ଖାଲି, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                r = capability_from_name(word);
                ଯଦି (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } ଅନ୍ୟଥା
                        val |= UINT64_C(1) << r;
        }

        ଯଦି (ret)
                *ret = val;

        ଫେରନ୍ତୁ good;
}
