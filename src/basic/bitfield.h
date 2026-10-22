/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#تعریف_کړه شاخص_منزل_ماسک(type, i, uniq)                                   \
        ({                                                              \
                صحيح UNIQ_T(_i, uniq) = (i);                             \
                أكد(UNIQ_T(_i, uniq) >= 0);                          \
                أكد(UNIQ_T(_i, uniq) < (صحيح)حجم(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#تعریف_کړه شاخص_منزل_ماسک_2(type, i)                                          \
        ({                                                              \
                assert_cc(حجم(type) <= حجم(بینښه اوږد اوږد));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (صحيح)(حجم(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      شاخص_منزل_ماسک(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#تعریف_کړه INDEXES_منزل_ماسک(type, ...)                              \
        UNIQ_INDEXES_منزل_ماسک(type, UNIQ, ##__VA_ARGS__)
#تعریف_کړه UNIQ_INDEXES_منزل_ماسک(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(ماسک_2, uniq) = (type)0;             \
                صحيح UNIQ_T(_i, uniq);                                   \
                FOREACH_دلیل(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(ماسک_2, uniq) |= شاخص_منزل_ماسک_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(ماسک_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#تعریف_کړه سېټ_بټ(بټونه, i) سېټ_FLAG(بټونه, شاخص_منزل_ماسک_2(typeof(بټونه), i), true)
#تعریف_کړه CLEAR_بټ(بټونه, i) سېټ_FLAG(بټونه, شاخص_منزل_ماسک_2(typeof(بټونه), i), false)
#تعریف_کړه بټ_سېټ(بټونه, i) FLAGS_سېټ(بټونه, شاخص_منزل_ماسک_2(typeof(بټونه), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#تعریف_کړه سېټ_بټونه(بټونه, ...) سېټ_FLAG(بټونه, INDEXES_منزل_ماسک(typeof(بټونه), ##__VA_ARGS__), true)
#تعریف_کړه CLEAR_بټونه(بټونه, ...) سېټ_FLAG(بټونه, INDEXES_منزل_ماسک(typeof(بټونه), ##__VA_ARGS__), false)
#تعریف_کړه بټونه_سېټ(بټونه, ...) FLAGS_سېټ(بټونه, INDEXES_منزل_ماسک(typeof(بټونه), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#تعریف_کړه بټ_FOREACH(index, بټونه) بټ_FOREACH_2(index, بټونه, UNIQ)
#تعریف_کړه بټ_FOREACH_2(index, بټونه, uniq)                                 \
        لكل (صحيح UNIQ_T(وروستی, uniq) = -1, index;                       \
             (index = بټ_بل_سېټ(بټونه, UNIQ_T(وروستی, uniq))) >= 0;    \
             UNIQ_T(وروستی, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#تعریف_کړه بټ_لومړی_سېټ(بټونه) بټ_بل_سېټ(بټونه, -1)
#تعریف_کړه بټ_بل_سېټ(بټونه, prev)                        \
        UNIQ_بټ_بل_سېټ(بټونه, prev, UNIQ)
#تعریف_کړه UNIQ_بټ_بل_سېټ(بټونه, prev, uniq)                             \
        ({                                                              \
                typeof(بټونه) UNIQ_T(بټونه_2, uniq) = (بټونه);              \
                صحيح UNIQ_T(_prev, uniq) = (prev);                       \
                صحيح UNIQ_T(بل, uniq);                                \
                بټ_بل_سېټ_2(UNIQ_T(بټونه_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(بل, uniq));                     \
        })
#تعریف_کړه بټ_بل_سېټ_2(بټونه, prev, بل_2)                                 \
        ((صحيح)(prev + 1) == (صحيح)حجم(بټونه) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((بل_2 = __builtin_ffsll(((بینښه اوږد اوږد)(بټونه)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + بل_2))
