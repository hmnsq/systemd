/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#تعریف_کرو اشاریہ_منزل_نقاب(type, i, uniq)                                   \
        ({                                                              \
                صحيح UNIQ_T(_i, uniq) = (i);                             \
                أكد(UNIQ_T(_i, uniq) >= 0);                          \
                أكد(UNIQ_T(_i, uniq) < (صحيح)حجم(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#تعریف_کرو اشاریہ_منزل_نقاب_2(type, i)                                          \
        ({                                                              \
                assert_cc(حجم(type) <= حجم(بنانشانی لما لما));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (صحيح)(حجم(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      اشاریہ_منزل_نقاب(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#تعریف_کرو INDEXES_منزل_نقاب(type, ...)                              \
        UNIQ_INDEXES_منزل_نقاب(type, UNIQ, ##__VA_ARGS__)
#تعریف_کرو UNIQ_INDEXES_منزل_نقاب(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(نقاب_2, uniq) = (type)0;             \
                صحيح UNIQ_T(_i, uniq);                                   \
                FOREACH_دلیل(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(نقاب_2, uniq) |= اشاریہ_منزل_نقاب_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(نقاب_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#تعریف_کرو سیٹ_ثنائی_ہندسہ(بٹ, i) سیٹ_FLAG(بٹ, اشاریہ_منزل_نقاب_2(typeof(بٹ), i), true)
#تعریف_کرو CLEAR_ثنائی_ہندسہ(بٹ, i) سیٹ_FLAG(بٹ, اشاریہ_منزل_نقاب_2(typeof(بٹ), i), false)
#تعریف_کرو ثنائی_ہندسہ_سیٹ(بٹ, i) FLAGS_سیٹ(بٹ, اشاریہ_منزل_نقاب_2(typeof(بٹ), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#تعریف_کرو سیٹ_بٹ(بٹ, ...) سیٹ_FLAG(بٹ, INDEXES_منزل_نقاب(typeof(بٹ), ##__VA_ARGS__), true)
#تعریف_کرو CLEAR_بٹ(بٹ, ...) سیٹ_FLAG(بٹ, INDEXES_منزل_نقاب(typeof(بٹ), ##__VA_ARGS__), false)
#تعریف_کرو بٹ_سیٹ(بٹ, ...) FLAGS_سیٹ(بٹ, INDEXES_منزل_نقاب(typeof(بٹ), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#تعریف_کرو ثنائی_ہندسہ_FOREACH(index, بٹ) ثنائی_ہندسہ_FOREACH_2(index, بٹ, UNIQ)
#تعریف_کرو ثنائی_ہندسہ_FOREACH_2(index, بٹ, uniq)                                 \
        لكل (صحيح UNIQ_T(آخری, uniq) = -1, index;                       \
             (index = ثنائی_ہندسہ_اگلا_سیٹ(بٹ, UNIQ_T(آخری, uniq))) >= 0;    \
             UNIQ_T(آخری, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#تعریف_کرو ثنائی_ہندسہ_پہلا_سیٹ(بٹ) ثنائی_ہندسہ_اگلا_سیٹ(بٹ, -1)
#تعریف_کرو ثنائی_ہندسہ_اگلا_سیٹ(بٹ, prev)                        \
        UNIQ_ثنائی_ہندسہ_اگلا_سیٹ(بٹ, prev, UNIQ)
#تعریف_کرو UNIQ_ثنائی_ہندسہ_اگلا_سیٹ(بٹ, prev, uniq)                             \
        ({                                                              \
                typeof(بٹ) UNIQ_T(بٹ_2, uniq) = (بٹ);              \
                صحيح UNIQ_T(_prev, uniq) = (prev);                       \
                صحيح UNIQ_T(اگلا, uniq);                                \
                ثنائی_ہندسہ_اگلا_سیٹ_2(UNIQ_T(بٹ_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(اگلا, uniq));                     \
        })
#تعریف_کرو ثنائی_ہندسہ_اگلا_سیٹ_2(بٹ, prev, اگلا_2)                                 \
        ((صحيح)(prev + 1) == (صحيح)حجم(بٹ) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((اگلا_2 = __builtin_ffsll(((بنانشانی لما لما)(بٹ)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + اگلا_2))
