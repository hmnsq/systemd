/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#परिभाषय सूचकांक_गन्तव्यम्_आवरणम्(type, i, uniq)                                   \
        ({                                                              \
                पूर्णाङ्क UNIQ_T(_i, uniq) = (i);                             \
                प्रतिज्ञा(UNIQ_T(_i, uniq) >= 0);                          \
                प्रतिज्ञा(UNIQ_T(_i, uniq) < (पूर्णाङ्क)परिमाण(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#परिभाषय सूचकांक_गन्तव्यम्_आवरणम्_2(type, i)                                          \
        ({                                                              \
                assert_cc(परिमाण(type) <= परिमाण(अचिह्नित दीर्घ दीर्घ));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (पूर्णाङ्क)(परिमाण(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      सूचकांक_गन्तव्यम्_आवरणम्(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#परिभाषय INDEXES_गन्तव्यम्_आवरणम्(type, ...)                              \
        UNIQ_INDEXES_गन्तव्यम्_आवरणम्(type, UNIQ, ##__VA_ARGS__)
#परिभाषय UNIQ_INDEXES_गन्तव्यम्_आवरणम्(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(आवरणम्_2, uniq) = (type)0;             \
                पूर्णाङ्क UNIQ_T(_i, uniq);                                   \
                FOREACH_तर्कः(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(आवरणम्_2, uniq) |= सूचकांक_गन्तव्यम्_आवरणम्_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(आवरणम्_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#परिभाषय समुच्चयः_BIT(द्वयङ्काः, i) समुच्चयः_FLAG(द्वयङ्काः, सूचकांक_गन्तव्यम्_आवरणम्_2(typeof(द्वयङ्काः), i), true)
#परिभाषय CLEAR_BIT(द्वयङ्काः, i) समुच्चयः_FLAG(द्वयङ्काः, सूचकांक_गन्तव्यम्_आवरणम्_2(typeof(द्वयङ्काः), i), false)
#परिभाषय BIT_समुच्चयः(द्वयङ्काः, i) FLAGS_समुच्चयः(द्वयङ्काः, सूचकांक_गन्तव्यम्_आवरणम्_2(typeof(द्वयङ्काः), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#परिभाषय समुच्चयः_द्वयङ्काः(द्वयङ्काः, ...) समुच्चयः_FLAG(द्वयङ्काः, INDEXES_गन्तव्यम्_आवरणम्(typeof(द्वयङ्काः), ##__VA_ARGS__), true)
#परिभाषय CLEAR_द्वयङ्काः(द्वयङ्काः, ...) समुच्चयः_FLAG(द्वयङ्काः, INDEXES_गन्तव्यम्_आवरणम्(typeof(द्वयङ्काः), ##__VA_ARGS__), false)
#परिभाषय द्वयङ्काः_समुच्चयः(द्वयङ्काः, ...) FLAGS_समुच्चयः(द्वयङ्काः, INDEXES_गन्तव्यम्_आवरणम्(typeof(द्वयङ्काः), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#परिभाषय BIT_FOREACH(index, द्वयङ्काः) _BIT_FOREACH(index, द्वयङ्काः, UNIQ)
#परिभाषय _BIT_FOREACH(index, द्वयङ्काः, uniq)                                 \
        प्रति (पूर्णाङ्क UNIQ_T(चरमः, uniq) = -1, index;                       \
             (index = BIT_अग्र_समुच्चयः(द्वयङ्काः, UNIQ_T(चरमः, uniq))) >= 0;    \
             UNIQ_T(चरमः, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#परिभाषय BIT_FIRST_समुच्चयः(द्वयङ्काः) BIT_अग्र_समुच्चयः(द्वयङ्काः, -1)
#परिभाषय BIT_अग्र_समुच्चयः(द्वयङ्काः, पूर्व)                        \
        UNIQ_BIT_अग्र_समुच्चयः(द्वयङ्काः, पूर्व, UNIQ)
#परिभाषय UNIQ_BIT_अग्र_समुच्चयः(द्वयङ्काः, पूर्व, uniq)                             \
        ({                                                              \
                typeof(द्वयङ्काः) UNIQ_T(द्वयङ्काः_2, uniq) = (द्वयङ्काः);              \
                पूर्णाङ्क UNIQ_T(पूर्व_2, uniq) = (पूर्व);                       \
                पूर्णाङ्क UNIQ_T(अग्र, uniq);                                \
                BIT_अग्र_समुच्चयः_2(UNIQ_T(द्वयङ्काः_2, uniq),                      \
                              UNIQ_T(पूर्व_2, uniq),                      \
                              UNIQ_T(अग्र, uniq));                     \
        })
#परिभाषय BIT_अग्र_समुच्चयः_2(द्वयङ्काः, पूर्व, अग्र_2)                                 \
        ((पूर्णाङ्क)(पूर्व + 1) == (पूर्णाङ्क)परिमाण(द्वयङ्काः) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((अग्र_2 = __builtin_ffsll(((अचिह्नित दीर्घ दीर्घ)(द्वयङ्काः)) >> (पूर्व + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : पूर्व + अग्र_2))
