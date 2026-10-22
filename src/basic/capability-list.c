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

ساکن ثابت ساخت capability_name* lookup_capability(اندراج ثابت حرف *str, اندراج GPERF_LEN_TYPE len);

#شامل "capability-from-name.inc"
#شامل "capability-to-name.inc"

ثابت حرف* capability_مقصد_name(صحیح id) {
        اگر (id < 0)
                واپس تہی;
        اگر ((غیرعلامتی) id >= capability_list_لمبائی())
                واپس تہی;

        واپس capability_نام[id];
}

ثابت حرف* capability_مقصد_string(صحیح id, حرف buf[ساکن CAPABILITY_مقصد_STRING_MAX]) {
        ثابت حرف *p;

        اگر (id < 0)
                واپس تہی;
        اگر (id > CAP_حد_بندی) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                واپس تہی;

        p = capability_مقصد_name(id);
        اگر (p)
                واپس p;

        sprintf(buf, "0x%x", (غیرعلامتی) id); /* numerical fallback */
        واپس buf;
}

صحیح capability_from_name(ثابت حرف *name) {
        ثابت ساخت capability_name *sc;
        صحیح r, i;

        أكد(name);

        /* Try to parse numeric capability */
        r = محفوظ_atoi(name, &i);
        اگر (r >= 0) {
                اگر (i < 0 || i > CAP_حد_بندی)
                        واپس -EINVAL;

                واپس i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, لڑی_لمبائی(name));
        اگر (!sc)
                واپس -EINVAL;

        واپس sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
غیرعلامتی capability_list_لمبائی(خالی) {
        واپس MIN((غیرعلامتی) ELEMENTSOF(capability_نام), (غیرعلامتی) (CAP_حد_بندی + 1));
}

صحیح capability_سیٹ_مقصد_string(uint64_t سیٹ, حرف **ret) {
        _cleanup_free_ حرف *str = تہی;

        أكد(ret);

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                ثابت حرف *p;

                اگر (!ثنائی_ہندسہ_سیٹ(سیٹ, i))
                        جاری;

                p = CAPABILITY_مقصد_STRING(i);
                أكد(p);

                اگر (!strextend_with_separator(&str, " ", p))
                        واپس -ENOMEM;
        }

        اگر (!str) {
                str = new0(حرف, 1);
                اگر (!str)
                        واپس -ENOMEM;
        }

        *ret = TAKE_PTR(str);
        واپس 0;
}

صحیح capability_سیٹ_مقصد_string_negative(uint64_t سیٹ, حرف **ret) {
        _cleanup_free_ حرف *a = تہی, *b = تہی;
        صحیح r;

        أكد(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_سیٹ_مقصد_string(سیٹ, &a);
        اگر (r < 0)
                واپس r;

        r = capability_سیٹ_مقصد_string(~سیٹ & all_capabilities(), &b);
        اگر (r < 0)
                واپس r;

        اگر (لڑی_لمبائی(a) <= 1 + لڑی_لمبائی(b))
                *ret = TAKE_PTR(a);
        ورنہ {
                حرف *c = strjoin("~", b);
                اگر (!c)
                        واپس -ENOMEM;

                *ret = c;
        }

        واپس 0;
}

صحیح capability_سیٹ_مقصد_strv(uint64_t سیٹ, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = تہی;
        صحیح r;

        أكد(ret);

        برائے (غیرعلامتی i = 0; i <= cap_آخری_cap(); i++) {
                ثابت حرف *p;

                اگر (!ثنائی_ہندسہ_سیٹ(سیٹ, i))
                        جاری;

                p = CAPABILITY_مقصد_STRING(i);
                أكد(p);

                r = strv_extend(&l, p);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(l);
        واپس 0;
}

صحیح capability_سیٹ_from_string(ثابت حرف *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        برائے (ثابت حرف *p = s;;) {
                _cleanup_free_ حرف *لفظ = تہی;
                صحیح r;

                r = نکالو_پہلا_لفظ(&p, &لفظ, تہی, نکالو_UNQUOTE|نکالو_RELAX);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                r = capability_from_name(لفظ);
                اگر (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", لفظ);
                        good = false;
                } ورنہ
                        val |= UINT64_C(1) << r;
        }

        اگر (ret)
                *ret = val;

        واپس good;
}
