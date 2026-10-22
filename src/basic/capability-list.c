/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "capability-list.h"
#અન્તર્ભાવ "capability-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

સ્થિર અચલ સંરચના capability_name* lookup_capability(નોંધણી અચલ અક્ષર *str, નોંધણી GPERF_LEN_TYPE len);

#અન્તર્ભાવ "capability-from-name.inc"
#અન્તર્ભાવ "capability-to-name.inc"

અચલ અક્ષર* capability_to_name(પૂર્ણાંક id) {
        જો (id < 0)
                પાછા ખાલી;
        જો ((unsigned) id >= capability_list_length())
                પાછા ખાલી;

        પાછા capability_names[id];
}

અચલ અક્ષર* capability_to_string(પૂર્ણાંક id, અક્ષર buf[સ્થિર CAPABILITY_TO_STRING_MAX]) {
        અચલ અક્ષર *p;

        જો (id < 0)
                પાછા ખાલી;
        જો (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                પાછા ખાલી;

        p = capability_to_name(id);
        જો (p)
                પાછા p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        પાછા buf;
}

પૂર્ણાંક capability_from_name(અચલ અક્ષર *name) {
        અચલ સંરચના capability_name *sc;
        પૂર્ણાંક r, i;

        પ્રતિજ્ઞા(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        જો (r >= 0) {
                જો (i < 0 || i > CAP_LIMIT)
                        પાછા -EINVAL;

                પાછા i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        જો (!sc)
                પાછા -EINVAL;

        પાછા sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(શૂન્ય) {
        પાછા MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

પૂર્ણાંક capability_set_to_string(uint64_t set, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *str = ખાલી;

        પ્રતિજ્ઞા(ret);

        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                અચલ અક્ષર *p;

                જો (!BIT_SET(set, i))
                        ચાલુ;

                p = CAPABILITY_TO_STRING(i);
                પ્રતિજ્ઞા(p);

                જો (!strextend_with_separator(&str, " ", p))
                        પાછા -ENOMEM;
        }

        જો (!str) {
                str = new0(અક્ષર, 1);
                જો (!str)
                        પાછા -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        પાછા 0;
}

પૂર્ણાંક capability_set_to_string_negative(uint64_t set, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *a = ખાલી, *b = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        જો (r < 0)
                પાછા r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        જો (r < 0)
                પાછા r;

        જો (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        નહીંતર {
                અક્ષર *c = strjoin("~", b);
                જો (!c)
                        પાછા -ENOMEM;

                *ret = c;
        }

        પાછા 0;
}

પૂર્ણાંક capability_set_to_strv(uint64_t set, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        માટે (unsigned i = 0; i <= cap_last_cap(); i++) {
                અચલ અક્ષર *p;

                જો (!BIT_SET(set, i))
                        ચાલુ;

                p = CAPABILITY_TO_STRING(i);
                પ્રતિજ્ઞા(p);

                r = strv_extend(&l, p);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(l);
        પાછા 0;
}

પૂર્ણાંક capability_set_from_string(અચલ અક્ષર *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        માટે (અચલ અક્ષર *p = s;;) {
                _cleanup_free_ અક્ષર *word = ખાલી;
                પૂર્ણાંક r;

                r = extract_first_word(&p, &word, ખાલી, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                r = capability_from_name(word);
                જો (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } નહીંતર
                        val |= UINT64_C(1) << r;
        }

        જો (ret)
                *ret = val;

        પાછા good;
}
