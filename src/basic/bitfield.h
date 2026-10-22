/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ(type, i, uniq)                                   \
        ({                                                              \
                ਪੂਰਨ UNIQ_T(_i, uniq) = (i);                             \
                ਪ੍ਰਤਿਜ੍ਞਾ(UNIQ_T(_i, uniq) >= 0);                          \
                ਪ੍ਰਤਿਜ੍ਞਾ(UNIQ_T(_i, uniq) < (ਪੂਰਨ)ਆਕਾਰ(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ_2(type, i)                                          \
        ({                                                              \
                assert_cc(ਆਕਾਰ(type) <= ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (ਪੂਰਨ)(ਆਕਾਰ(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(type, ...)                              \
        UNIQ_INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(type, UNIQ, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNIQ_INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(ਨਕਾਬ_2, uniq) = (type)0;             \
                ਪੂਰਨ UNIQ_T(_i, uniq);                                   \
                FOREACH_ਦਲੀਲ(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(ਨਕਾਬ_2, uniq) |= ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(ਨਕਾਬ_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿੱਟ, i) ਸੈੱਟ_FLAG(ਬਿੱਟ, ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ_2(typeof(ਬਿੱਟ), i), true)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEAR_ਦੋਆਧਾਰੀ_ਅੰਕ(ਬਿੱਟ, i) ਸੈੱਟ_FLAG(ਬਿੱਟ, ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ_2(typeof(ਬਿੱਟ), i), false)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(ਬਿੱਟ, i) FLAGS_ਸੈੱਟ(ਬਿੱਟ, ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਨਕਾਬ_2(typeof(ਬਿੱਟ), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਬਿੱਟ(ਬਿੱਟ, ...) ਸੈੱਟ_FLAG(ਬਿੱਟ, INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(typeof(ਬਿੱਟ), ##__VA_ARGS__), true)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEAR_ਬਿੱਟ(ਬਿੱਟ, ...) ਸੈੱਟ_FLAG(ਬਿੱਟ, INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(typeof(ਬਿੱਟ), ##__VA_ARGS__), false)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਿੱਟ_ਸੈੱਟ(ਬਿੱਟ, ...) FLAGS_ਸੈੱਟ(ਬਿੱਟ, INDEXES_ਮੰਜ਼ਿਲ_ਨਕਾਬ(typeof(ਬਿੱਟ), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_FOREACH(index, ਬਿੱਟ) ਦੋਆਧਾਰੀ_ਅੰਕ_FOREACH_2(index, ਬਿੱਟ, UNIQ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_FOREACH_2(index, ਬਿੱਟ, uniq)                                 \
        ਲਈ (ਪੂਰਨ UNIQ_T(ਆਖਰੀ, uniq) = -1, index;                       \
             (index = ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ(ਬਿੱਟ, UNIQ_T(ਆਖਰੀ, uniq))) >= 0;    \
             UNIQ_T(ਆਖਰੀ, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_ਪਹਿਲਾ_ਸੈੱਟ(ਬਿੱਟ) ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ(ਬਿੱਟ, -1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ(ਬਿੱਟ, prev)                        \
        UNIQ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ(ਬਿੱਟ, prev, UNIQ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNIQ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ(ਬਿੱਟ, prev, uniq)                             \
        ({                                                              \
                typeof(ਬਿੱਟ) UNIQ_T(ਬਿੱਟ_2, uniq) = (ਬਿੱਟ);              \
                ਪੂਰਨ UNIQ_T(_prev, uniq) = (prev);                       \
                ਪੂਰਨ UNIQ_T(ਅਗਲਾ, uniq);                                \
                ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ_2(UNIQ_T(ਬਿੱਟ_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(ਅਗਲਾ, uniq));                     \
        })
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੋਆਧਾਰੀ_ਅੰਕ_ਅਗਲਾ_ਸੈੱਟ_2(ਬਿੱਟ, prev, ਅਗਲਾ_2)                                 \
        ((ਪੂਰਨ)(prev + 1) == (ਪੂਰਨ)ਆਕਾਰ(ਬਿੱਟ) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((ਅਗਲਾ_2 = __builtin_ffsll(((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ)(ਬਿੱਟ)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + ਅਗਲਾ_2))
