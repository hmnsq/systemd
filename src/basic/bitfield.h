/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ(type, i, uniq)                                   \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ UNIQ_T(_i, uniq) = (i);                             \
                ಪ್ರತಿಜ್ಞಾ(UNIQ_T(_i, uniq) >= 0);                          \
                ಪ್ರತಿಜ್ಞಾ(UNIQ_T(_i, uniq) < (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ_2(type, i)                                          \
        ({                                                              \
                assert_cc(ಗಾತ್ರ(type) <= ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#ವ್ಯಾಖ್ಯಾನಿಸು INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(type, ...)                              \
        UNIQ_INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(type, UNIQ, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು UNIQ_INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(ಮುಖವಾಡ_2, uniq) = (type)0;             \
                ಪೂರ್ಣಾಂಕ UNIQ_T(_i, uniq);                                   \
                FOREACH_ವಾದ(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(ಮುಖವಾಡ_2, uniq) |= ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(ಮುಖವಾಡ_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ಬಿಟ್(ಬಿಟ್ಟುಗಳು, i) ಗಣ_FLAG(ಬಿಟ್ಟುಗಳು, ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ_2(typeof(ಬಿಟ್ಟುಗಳು), i), true)
#ವ್ಯಾಖ್ಯಾನಿಸು CLEAR_ಬಿಟ್(ಬಿಟ್ಟುಗಳು, i) ಗಣ_FLAG(ಬಿಟ್ಟುಗಳು, ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ_2(typeof(ಬಿಟ್ಟುಗಳು), i), false)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_ಗಣ(ಬಿಟ್ಟುಗಳು, i) FLAGS_ಗಣ(ಬಿಟ್ಟುಗಳು, ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಮುಖವಾಡ_2(typeof(ಬಿಟ್ಟುಗಳು), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ಬಿಟ್ಟುಗಳು(ಬಿಟ್ಟುಗಳು, ...) ಗಣ_FLAG(ಬಿಟ್ಟುಗಳು, INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(typeof(ಬಿಟ್ಟುಗಳು), ##__VA_ARGS__), true)
#ವ್ಯಾಖ್ಯಾನಿಸು CLEAR_ಬಿಟ್ಟುಗಳು(ಬಿಟ್ಟುಗಳು, ...) ಗಣ_FLAG(ಬಿಟ್ಟುಗಳು, INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(typeof(ಬಿಟ್ಟುಗಳು), ##__VA_ARGS__), false)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್ಟುಗಳು_ಗಣ(ಬಿಟ್ಟುಗಳು, ...) FLAGS_ಗಣ(ಬಿಟ್ಟುಗಳು, INDEXES_ಗಮ್ಯ_ಮುಖವಾಡ(typeof(ಬಿಟ್ಟುಗಳು), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_FOREACH(index, ಬಿಟ್ಟುಗಳು) ಬಿಟ್_FOREACH_2(index, ಬಿಟ್ಟುಗಳು, UNIQ)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_FOREACH_2(index, ಬಿಟ್ಟುಗಳು, uniq)                                 \
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ UNIQ_T(ಕೊನೆಯ, uniq) = -1, index;                       \
             (index = ಬಿಟ್_ಮುಂದಿನ_ಗಣ(ಬಿಟ್ಟುಗಳು, UNIQ_T(ಕೊನೆಯ, uniq))) >= 0;    \
             UNIQ_T(ಕೊನೆಯ, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_ಮೊದಲ_ಗಣ(ಬಿಟ್ಟುಗಳು) ಬಿಟ್_ಮುಂದಿನ_ಗಣ(ಬಿಟ್ಟುಗಳು, -1)
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_ಮುಂದಿನ_ಗಣ(ಬಿಟ್ಟುಗಳು, prev)                        \
        UNIQ_ಬಿಟ್_ಮುಂದಿನ_ಗಣ(ಬಿಟ್ಟುಗಳು, prev, UNIQ)
#ವ್ಯಾಖ್ಯಾನಿಸು UNIQ_ಬಿಟ್_ಮುಂದಿನ_ಗಣ(ಬಿಟ್ಟುಗಳು, prev, uniq)                             \
        ({                                                              \
                typeof(ಬಿಟ್ಟುಗಳು) UNIQ_T(ಬಿಟ್ಟುಗಳು_2, uniq) = (ಬಿಟ್ಟುಗಳು);              \
                ಪೂರ್ಣಾಂಕ UNIQ_T(_prev, uniq) = (prev);                       \
                ಪೂರ್ಣಾಂಕ UNIQ_T(ಮುಂದಿನ, uniq);                                \
                ಬಿಟ್_ಮುಂದಿನ_ಗಣ_2(UNIQ_T(ಬಿಟ್ಟುಗಳು_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(ಮುಂದಿನ, uniq));                     \
        })
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಿಟ್_ಮುಂದಿನ_ಗಣ_2(ಬಿಟ್ಟುಗಳು, prev, ಮುಂದಿನ_2)                                 \
        ((ಪೂರ್ಣಾಂಕ)(prev + 1) == (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((ಮುಂದಿನ_2 = __builtin_ffsll(((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ)(ಬಿಟ್ಟುಗಳು)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + ಮುಂದಿನ_2))
