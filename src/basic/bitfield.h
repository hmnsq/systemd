/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#وصف_ڪريو اشاريو_منزل_نقاب(type, i, uniq)                                   \
        ({                                                              \
                صحيح UNIQ_T(_i, uniq) = (i);                             \
                أكد(UNIQ_T(_i, uniq) >= 0);                          \
                أكد(UNIQ_T(_i, uniq) < (صحيح)حجم(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#وصف_ڪريو اشاريو_منزل_نقاب_2(type, i)                                          \
        ({                                                              \
                assert_cc(حجم(type) <= حجم(بنانشان ڊگهو ڊگهو));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (صحيح)(حجم(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      اشاريو_منزل_نقاب(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#وصف_ڪريو INDEXES_منزل_نقاب(type, ...)                              \
        UNIQ_INDEXES_منزل_نقاب(type, UNIQ, ##__VA_ARGS__)
#وصف_ڪريو UNIQ_INDEXES_منزل_نقاب(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(نقاب_2, uniq) = (type)0;             \
                صحيح UNIQ_T(_i, uniq);                                   \
                FOREACH_دليل(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(نقاب_2, uniq) |= اشاريو_منزل_نقاب_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(نقاب_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#وصف_ڪريو سيٽ_ٻٽو_انگ(بٽ, i) سيٽ_FLAG(بٽ, اشاريو_منزل_نقاب_2(typeof(بٽ), i), true)
#وصف_ڪريو CLEAR_ٻٽو_انگ(بٽ, i) سيٽ_FLAG(بٽ, اشاريو_منزل_نقاب_2(typeof(بٽ), i), false)
#وصف_ڪريو ٻٽو_انگ_سيٽ(بٽ, i) FLAGS_سيٽ(بٽ, اشاريو_منزل_نقاب_2(typeof(بٽ), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#وصف_ڪريو سيٽ_بٽ(بٽ, ...) سيٽ_FLAG(بٽ, INDEXES_منزل_نقاب(typeof(بٽ), ##__VA_ARGS__), true)
#وصف_ڪريو CLEAR_بٽ(بٽ, ...) سيٽ_FLAG(بٽ, INDEXES_منزل_نقاب(typeof(بٽ), ##__VA_ARGS__), false)
#وصف_ڪريو بٽ_سيٽ(بٽ, ...) FLAGS_سيٽ(بٽ, INDEXES_منزل_نقاب(typeof(بٽ), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#وصف_ڪريو ٻٽو_انگ_FOREACH(index, بٽ) ٻٽو_انگ_FOREACH_2(index, بٽ, UNIQ)
#وصف_ڪريو ٻٽو_انگ_FOREACH_2(index, بٽ, uniq)                                 \
        لكل (صحيح UNIQ_T(آخري, uniq) = -1, index;                       \
             (index = ٻٽو_انگ_ايندڙ_سيٽ(بٽ, UNIQ_T(آخري, uniq))) >= 0;    \
             UNIQ_T(آخري, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#وصف_ڪريو ٻٽو_انگ_پهريون_سيٽ(بٽ) ٻٽو_انگ_ايندڙ_سيٽ(بٽ, -1)
#وصف_ڪريو ٻٽو_انگ_ايندڙ_سيٽ(بٽ, prev)                        \
        UNIQ_ٻٽو_انگ_ايندڙ_سيٽ(بٽ, prev, UNIQ)
#وصف_ڪريو UNIQ_ٻٽو_انگ_ايندڙ_سيٽ(بٽ, prev, uniq)                             \
        ({                                                              \
                typeof(بٽ) UNIQ_T(بٽ_2, uniq) = (بٽ);              \
                صحيح UNIQ_T(_prev, uniq) = (prev);                       \
                صحيح UNIQ_T(ايندڙ, uniq);                                \
                ٻٽو_انگ_ايندڙ_سيٽ_2(UNIQ_T(بٽ_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(ايندڙ, uniq));                     \
        })
#وصف_ڪريو ٻٽو_انگ_ايندڙ_سيٽ_2(بٽ, prev, ايندڙ_2)                                 \
        ((صحيح)(prev + 1) == (صحيح)حجم(بٽ) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((ايندڙ_2 = __builtin_ffsll(((بنانشان ڊگهو ڊگهو)(بٽ)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + ايندڙ_2))
