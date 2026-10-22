/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#నిర్వచించు సూచీ_గమ్యం_ముసుగు(type, i, uniq)                                   \
        ({                                                              \
                పూర్ణాంకం UNIQ_T(_i, uniq) = (i);                             \
                ప్రతిజ్ఞా(UNIQ_T(_i, uniq) >= 0);                          \
                ప్రతిజ్ఞా(UNIQ_T(_i, uniq) < (పూర్ణాంకం)పరిమాణం(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#నిర్వచించు సూచీ_గమ్యం_ముసుగు_2(type, i)                                          \
        ({                                                              \
                assert_cc(పరిమాణం(type) <= పరిమాణం(గుర్తులేని దీర్ఘ దీర్ఘ));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (పూర్ణాంకం)(పరిమాణం(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      సూచీ_గమ్యం_ముసుగు(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#నిర్వచించు INDEXES_గమ్యం_ముసుగు(type, ...)                              \
        UNIQ_INDEXES_గమ్యం_ముసుగు(type, UNIQ, ##__VA_ARGS__)
#నిర్వచించు UNIQ_INDEXES_గమ్యం_ముసుగు(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(ముసుగు_2, uniq) = (type)0;             \
                పూర్ణాంకం UNIQ_T(_i, uniq);                                   \
                FOREACH_వాదం(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(ముసుగు_2, uniq) |= సూచీ_గమ్యం_ముసుగు_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(ముసుగు_2, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#నిర్వచించు సమితి_బిట్టు(బిట్లు, i) సమితి_FLAG(బిట్లు, సూచీ_గమ్యం_ముసుగు_2(typeof(బిట్లు), i), true)
#నిర్వచించు CLEAR_బిట్టు(బిట్లు, i) సమితి_FLAG(బిట్లు, సూచీ_గమ్యం_ముసుగు_2(typeof(బిట్లు), i), false)
#నిర్వచించు బిట్టు_సమితి(బిట్లు, i) FLAGS_సమితి(బిట్లు, సూచీ_గమ్యం_ముసుగు_2(typeof(బిట్లు), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#నిర్వచించు సమితి_బిట్లు(బిట్లు, ...) సమితి_FLAG(బిట్లు, INDEXES_గమ్యం_ముసుగు(typeof(బిట్లు), ##__VA_ARGS__), true)
#నిర్వచించు CLEAR_బిట్లు(బిట్లు, ...) సమితి_FLAG(బిట్లు, INDEXES_గమ్యం_ముసుగు(typeof(బిట్లు), ##__VA_ARGS__), false)
#నిర్వచించు బిట్లు_సమితి(బిట్లు, ...) FLAGS_సమితి(బిట్లు, INDEXES_గమ్యం_ముసుగు(typeof(బిట్లు), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#నిర్వచించు బిట్టు_FOREACH(index, బిట్లు) బిట్టు_FOREACH_2(index, బిట్లు, UNIQ)
#నిర్వచించు బిట్టు_FOREACH_2(index, బిట్లు, uniq)                                 \
        కొరకు (పూర్ణాంకం UNIQ_T(చివరి, uniq) = -1, index;                       \
             (index = బిట్టు_తదుపరి_సమితి(బిట్లు, UNIQ_T(చివరి, uniq))) >= 0;    \
             UNIQ_T(చివరి, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#నిర్వచించు బిట్టు_మొదటి_సమితి(బిట్లు) బిట్టు_తదుపరి_సమితి(బిట్లు, -1)
#నిర్వచించు బిట్టు_తదుపరి_సమితి(బిట్లు, prev)                        \
        UNIQ_బిట్టు_తదుపరి_సమితి(బిట్లు, prev, UNIQ)
#నిర్వచించు UNIQ_బిట్టు_తదుపరి_సమితి(బిట్లు, prev, uniq)                             \
        ({                                                              \
                typeof(బిట్లు) UNIQ_T(బిట్లు_2, uniq) = (బిట్లు);              \
                పూర్ణాంకం UNIQ_T(_prev, uniq) = (prev);                       \
                పూర్ణాంకం UNIQ_T(తదుపరి, uniq);                                \
                బిట్టు_తదుపరి_సమితి_2(UNIQ_T(బిట్లు_2, uniq),                      \
                              UNIQ_T(_prev, uniq),                      \
                              UNIQ_T(తదుపరి, uniq));                     \
        })
#నిర్వచించు బిట్టు_తదుపరి_సమితి_2(బిట్లు, prev, తదుపరి_2)                                 \
        ((పూర్ణాంకం)(prev + 1) == (పూర్ణాంకం)పరిమాణం(బిట్లు) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((తదుపరి_2 = __builtin_ffsll(((గుర్తులేని దీర్ఘ దీర్ఘ)(బిట్లు)) >> (prev + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : prev + తదుపరి_2))
