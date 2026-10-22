/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#تعریف_کن شاخص_مقصد_ماسک(type, i, uniq)                                   \
        ({                                                              \
                صحيح UNIQ_T(_i, uniq) = (i);                             \
                أكد(UNIQ_T(_i, uniq) >= 0);                          \
                أكد(UNIQ_T(_i, uniq) < (صحيح)حجم(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#تعریف_کن شاخص_مقصد_ماسک_2(type, i)                                          \
        ({                                                              \
                assert_cc(حجم(type) <= حجم(بیعلامت دراز دراز));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (صحيح)(حجم(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      شاخص_مقصد_ماسک(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#تعریف_کن INDEXES_مقصد_ماسک(type, ...)                              \
        UNIQ_INDEXES_مقصد_ماسک(type, UNIQ, ##__VA_ARGS__)
#تعریف_کن UNIQ_INDEXES_مقصد_ماسک(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(ماسک_2, uniq) = (type)0;             \
                صحيح UNIQ_T(_i, uniq);                                   \
                FOREACH_آرگومان(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(ماسک_2, uniq) |= شاخص_مقصد_ماسک_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(ماسک_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#تعریف_کن مجموعه_بیت(بیتها, i) مجموعه_FLAG(بیتها, شاخص_مقصد_ماسک_2(typeof(بیتها), i), true)
#تعریف_کن CLEAR_بیت(بیتها, i) مجموعه_FLAG(بیتها, شاخص_مقصد_ماسک_2(typeof(بیتها), i), false)
#تعریف_کن بیت_مجموعه(بیتها, i) FLAGS_مجموعه(بیتها, شاخص_مقصد_ماسک_2(typeof(بیتها), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#تعریف_کن مجموعه_بیتها(بیتها, ...) مجموعه_FLAG(بیتها, INDEXES_مقصد_ماسک(typeof(بیتها), ##__VA_ARGS__), true)
#تعریف_کن CLEAR_بیتها(بیتها, ...) مجموعه_FLAG(بیتها, INDEXES_مقصد_ماسک(typeof(بیتها), ##__VA_ARGS__), false)
#تعریف_کن بیتها_مجموعه(بیتها, ...) FLAGS_مجموعه(بیتها, INDEXES_مقصد_ماسک(typeof(بیتها), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#تعریف_کن بیت_FOREACH(index, بیتها) بیت_FOREACH_2(index, بیتها, UNIQ)
#تعریف_کن بیت_FOREACH_2(index, بیتها, uniq)                                 \
        لكل (صحيح UNIQ_T(آخرین, uniq) = -1, index;                       \
             (index = بیت_بعدی_مجموعه(بیتها, UNIQ_T(آخرین, uniq))) >= 0;    \
             UNIQ_T(آخرین, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#تعریف_کن بیت_اولی_مجموعه(بیتها) بیت_بعدی_مجموعه(بیتها, -1)
#تعریف_کن بیت_بعدی_مجموعه(بیتها, prev)                        \
        UNIQ_بیت_بعدی_مجموعه(بیتها, prev, UNIQ)
#تعریف_کن UNIQ_بیت_بعدی_مجموعه(بیتها, prev, uniq)                             \
        ({                                                              \
                typeof(بیتها) UNIQ_T(بیتها_2, uniq) = (بیتها);              \
                صحيح UNIQ_T(_prev, uniq) = (prev);                       \
                صحيح UNIQ_T(بعدی, uniq);                                \
                بیت_بعدی_مجموعه_2(UNIQ_T(بیتها_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(بعدی, uniq));                     \
        })
#تعریف_کن بیت_بعدی_مجموعه_2(بیتها, prev, بعدی_2)                                 \
        ((صحيح)(prev + 1) == (صحيح)حجم(بیتها) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((بعدی_2 = __builtin_ffsll(((بیعلامت دراز دراز)(بیتها)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + بعدی_2))
