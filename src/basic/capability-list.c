/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "bitfield.h"
#شامل "capability-list.h"
#شامل "capability-util.h"
#شامل "extract-word.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "string-util.h"
#شامل "strv.h"

ایستا ثابت ساختار capability_name* lookup_capability(ثبات ثابت نویسه *str, ثبات GPERF_LEN_TYPE len);

#شامل "capability-from-name.inc"
#شامل "capability-to-name.inc"

ثابت نویسه* capability_to_name(صحیح id) {
        اگر (id < 0)
                بازگشت پوچ;
        اگر ((unsigned) id >= capability_list_length())
                بازگشت پوچ;

        بازگشت capability_names[id];
}

ثابت نویسه* capability_to_string(صحیح id, نویسه buf[ایستا CAPABILITY_TO_STRING_MAX]) {
        ثابت نویسه *p;

        اگر (id < 0)
                بازگشت پوچ;
        اگر (id > CAP_LIMIT) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                بازگشت پوچ;

        p = capability_to_name(id);
        اگر (p)
                بازگشت p;

        sprintf(buf, "0x%x", (unsigned) id); /* numerical fallback */
        بازگشت buf;
}

صحیح capability_from_name(ثابت نویسه *name) {
        ثابت ساختار capability_name *sc;
        صحیح r, i;

        أكد(name);

        /* Try to parse numeric capability */
        r = safe_atoi(name, &i);
        اگر (r >= 0) {
                اگر (i < 0 || i > CAP_LIMIT)
                        بازگشت -EINVAL;

                بازگشت i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, strlen(name));
        اگر (!sc)
                بازگشت -EINVAL;

        بازگشت sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
unsigned capability_list_length(تهی) {
        بازگشت MIN((unsigned) ELEMENTSOF(capability_names), (unsigned) (CAP_LIMIT + 1));
}

صحیح capability_set_to_string(uint64_t set, نویسه **ret) {
        _cleanup_free_ نویسه *str = پوچ;

        أكد(ret);

        برای (unsigned i = 0; i <= cap_last_cap(); i++) {
                ثابت نویسه *p;

                اگر (!BIT_SET(set, i))
                        ادامه;

                p = CAPABILITY_TO_STRING(i);
                أكد(p);

                اگر (!strextend_with_separator(&str, " ", p))
                        بازگشت -ENOMEM;
        }

        اگر (!str) {
                str = new0(نویسه, 1);
                اگر (!str)
                        بازگشت -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        بازگشت 0;
}

صحیح capability_set_to_string_negative(uint64_t set, نویسه **ret) {
        _cleanup_free_ نویسه *a = پوچ, *b = پوچ;
        صحیح r;

        أكد(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_set_to_string(set, &a);
        اگر (r < 0)
                بازگشت r;

        r = capability_set_to_string(~set & all_capabilities(), &b);
        اگر (r < 0)
                بازگشت r;

        اگر (strlen(a) <= 1 + strlen(b))
                *ret = TAKE_PTR(a);
        وگرنه {
                نویسه *c = strjoin("~", b);
                اگر (!c)
                        بازگشت -ENOMEM;

                *ret = c;
        }

        بازگشت 0;
}

صحیح capability_set_to_strv(uint64_t set, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        صحیح r;

        أكد(ret);

        برای (unsigned i = 0; i <= cap_last_cap(); i++) {
                ثابت نویسه *p;

                اگر (!BIT_SET(set, i))
                        ادامه;

                p = CAPABILITY_TO_STRING(i);
                أكد(p);

                r = strv_extend(&l, p);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(l);
        بازگشت 0;
}

صحیح capability_set_from_string(ثابت نویسه *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        برای (ثابت نویسه *p = s;;) {
                _cleanup_free_ نویسه *word = پوچ;
                صحیح r;

                r = extract_first_word(&p, &word, پوچ, EXTRACT_UNQUOTE|EXTRACT_RELAX);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                r = capability_from_name(word);
                اگر (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", word);
                        good = false;
                } وگرنه
                        val |= UINT64_C(1) << r;
        }

        اگر (ret)
                *ret = val;

        بازگشت good;
}
