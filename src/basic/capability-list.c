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

ثابت حرف* capability_منزل_name(صحيح id) {
        إذا (id < 0)
                أرجع عدم;
        إذا ((بنانشانی) id >= capability_list_لمائی())
                أرجع عدم;

        أرجع capability_ناں[id];
}

ثابت حرف* capability_منزل_string(صحيح id, حرف buf[ساكن CAPABILITY_منزل_STRING_MAX]) {
        ثابت حرف *p;

        إذا (id < 0)
                أرجع عدم;
        إذا (id > CAP_حد_بندی) /* refuse caps > 62 since we can't store them in a uint64_t mask anymore, and still retain UINT64_MAX as marker for "unset" */
                أرجع عدم;

        p = capability_منزل_name(id);
        إذا (p)
                أرجع p;

        sprintf(buf, "0x%x", (بنانشانی) id); /* numerical fallback */
        أرجع buf;
}

صحيح capability_from_name(ثابت حرف *name) {
        ثابت بنية capability_name *sc;
        صحيح r, i;

        أكد(name);

        /* Try to parse numeric capability */
        r = محفوظ_atoi(name, &i);
        إذا (r >= 0) {
                إذا (i < 0 || i > CAP_حد_بندی)
                        أرجع -EINVAL;

                أرجع i;
        }

        /* Try to parse string capability */
        sc = lookup_capability(name, لڑی_لمبائی(name));
        إذا (!sc)
                أرجع -EINVAL;

        أرجع sc->id;
}

/* This is the number of capability names we are *compiled* with. For the max capability number of the
 * currently-running kernel, use cap_last_cap(). Note that this one returns the size of the array, i.e. one
 * value larger than the last known capability. This is different from cap_last_cap() which returns the
 * highest supported capability. Hence with everyone agreeing on the same capabilities list, this function
 * will return one higher than cap_last_cap(). */
بنانشانی capability_list_لمائی(فراغ) {
        أرجع MIN((بنانشانی) ELEMENTSOF(capability_ناں), (بنانشانی) (CAP_حد_بندی + 1));
}

صحيح capability_سیٹ_منزل_string(uint64_t سیٹ, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;

        أكد(ret);

        لكل (بنانشانی i = 0; i <= cap_آخری_cap(); i++) {
                ثابت حرف *p;

                إذا (!ثنائی_ہندسہ_سیٹ(سیٹ, i))
                        استمر;

                p = CAPABILITY_منزل_STRING(i);
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

صحيح capability_سیٹ_منزل_string_negative(uint64_t سیٹ, حرف **ret) {
        _cleanup_free_ حرف *a = عدم, *b = عدم;
        صحيح r;

        أكد(ret);

        /* Format the specified capability mask both in positive way (i.e. just listing caps) and in negative
         * way (i.e. listing only caps that are missing from the full set) and return the shorter version of
         * the two. */

        r = capability_سیٹ_منزل_string(سیٹ, &a);
        إذا (r < 0)
                أرجع r;

        r = capability_سیٹ_منزل_string(~سیٹ & all_capabilities(), &b);
        إذا (r < 0)
                أرجع r;

        إذا (لڑی_لمبائی(a) <= 1 + لڑی_لمبائی(b))
                *ret = TAKE_PTR(a);
        وإلا {
                حرف *c = strjoin("~", b);
                إذا (!c)
                        أرجع -ENOMEM;

                *ret = c;
        }

        أرجع 0;
}

صحيح capability_سیٹ_منزل_strv(uint64_t سیٹ, حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        صحيح r;

        أكد(ret);

        لكل (بنانشانی i = 0; i <= cap_آخری_cap(); i++) {
                ثابت حرف *p;

                إذا (!ثنائی_ہندسہ_سیٹ(سیٹ, i))
                        استمر;

                p = CAPABILITY_منزل_STRING(i);
                أكد(p);

                r = strv_extend(&l, p);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(l);
        أرجع 0;
}

صحيح capability_سیٹ_from_string(ثابت حرف *s, uint64_t *ret) {
        uint64_t val = 0;
        bool good = true;

        لكل (ثابت حرف *p = s;;) {
                _cleanup_free_ حرف *لفظ = عدم;
                صحيح r;

                r = کڈھو_پہلا_لفظ(&p, &لفظ, عدم, کڈھو_UNQUOTE|کڈھو_RELAX);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                r = capability_from_name(لفظ);
                إذا (r < 0) {
                        log_debug_errno(r, "Failed to parse capability '%s', ignoring: %m", لفظ);
                        good = false;
                } وإلا
                        val |= UINT64_C(1) << r;
        }

        إذا (ret)
                *ret = val;

        أرجع good;
}
