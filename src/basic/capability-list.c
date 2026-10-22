/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "bitfield.h"
#ضمن "capability-list.h"
#ضمن "capability-util.h"
#ضمن "extract-word.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

ساكن ثابت بنية capability_name* lookup_capability(سجل ثابت حرف *str, سجل GPERF_LEN_TYPE len);

#ضمن "capability-from-name.inc"
#ضمن "capability-to-name.inc"

ثابت حرف* capability_to_name(صحيح id) {
        إذا (id < 0)
                أرجع عدم;
        إذا ((unsigned) id >= capability_list_length())
                أرجع عدم;

        أرجع capability_names[id];
}

ثابت حرف* capability_to_string(صحيح id, حرف buf[ساكن CAPABILITY_TO_STRING_MAX]) {
        ثابت حرف *p;

        إذا (id < 0)
                أرجع عدم;
        إذا (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                أرجع عدم;

        p = capability_to_name(id);
        إذا (p)
                أرجع p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        أرجع buf;
}

صحيح capability_from_name(ثابت حرف *name) {
        ثابت بنية capability_name *sc;
        صحيح r, i;

        أكد(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        إذا (r >= 0) {
                إذا (i < 0 || i > CAP_LIMIT)
                        أرجع -EINVAL;

                أرجع i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        إذا (!sc)
                أرجع -EINVAL;

        أرجع sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(فراغ) {
        أرجع MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

صحيح capability_set_to_string(uint64_t set, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;

        أكد(ret);

        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                ثابت حرف *p;

                إذا (!BIT_SET(set, i))
                        استمر;

                p = CAPABILITY_TO_STRING(i);
                أكد(p);

                إذا (!strextend_with_separator(&str, " ", p))
                        أرجع -ENOMEM;
        }

        إذا (!str) {
                str = new0(حرف, 1);
                إذا (!str)
                        أرجع -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        أرجع 0;
}

صحيح capability_set_to_string_negative(uint64_t set, حرف **ret) {
        _cleanup_free_ حرف *a = عدم, *b = عدم;
        صحيح r;

        أكد(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        إذا (r < 0)
                أرجع r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        إذا (r < 0)
                أرجع r;

        إذا (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        وإلا {
                حرف *c = strjoin("~", b);
                إذا (!c)
                        أرجع -ENOMEM;

                *ret = c;
        }

        أرجع 0;
}

صحيح capability_set_to_strv(uint64_t set, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(ret);

        لكل (unsigned i = 0; i <= cap_last_cap(); i++) {
                ثابت حرف *p;

                إذا (!BIT_SET(set, i))
                        استمر;

                p = CAPABILITY_TO_STRING(i);
                أكد(p);

                r = strv_extend(&l, p);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(l);
        أرجع 0;
}

صحيح capability_set_from_string(ثابت حرف *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        لكل (ثابت حرف *p = s;;) {
                _cleanup_free_ حرف *word = عدم;
                صحيح r;

                r = extract_first_word(&p, &word, عدم, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                r = capability_from_name(word);
                إذا (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } وإلا
                        val |= UINT64_C(1) << r;
        }

        إذا (ret)
                *ret = val;

        أرجع good;
}
