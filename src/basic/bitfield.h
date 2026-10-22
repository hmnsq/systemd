/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* Bit index (0-based) to mask of specified type. Assertion failure if index is out of range. */
#घोषणा सूचकांक_TO_MASK(type, i, uniq)                                   \
        ({                                                              \
                पूर्णांक UNIQ_T(_i, uniq) = (i);                             \
                निश्चित(UNIQ_T(_i, uniq) >= 0);                          \
                निश्चित(UNIQ_T(_i, uniq) < (पूर्णांक)माप(type) * 8);       \
                ((type)1) << UNIQ_T(_i, uniq);                          \
        })
#घोषणा सूचकांक_TO_MASK_2(type, i)                                          \
        ({                                                              \
                assert_cc(माप(type) <= माप(अचिन्हित दीर्घ दीर्घ));  \
                assert_cc(__builtin_choose_expr(__builtin_constant_p(i), i, 0) < (पूर्णांक)(माप(type) * 8)); \
                __builtin_choose_expr(__builtin_constant_p(i),          \
                                      ((type)1) << (i),                 \
                                      सूचकांक_TO_MASK(type, i, UNIQ));   \
        })

/* Builds a mask of specified type with multiple bits set. Note the result will not be constant, even if all
 * indexes are constant. */
#घोषणा INDEXES_TO_MASK(type, ...)                              \
        UNIQ_INDEXES_TO_MASK(type, UNIQ, ##__VA_ARGS__)
#घोषणा UNIQ_INDEXES_TO_MASK(type, uniq, ...)                           \
        ({                                                              \
                typeof(type) UNIQ_T(_mask, uniq) = (type)0;             \
                पूर्णांक UNIQ_T(_i, uniq);                                   \
                FOREACH_ARGUMENT(UNIQ_T(_i, uniq), ##__VA_ARGS__)       \
                        UNIQ_T(_mask, uniq) |= सूचकांक_TO_MASK_2(type, UNIQ_T(_i, uniq)); \
                UNIQ_T(_mask, uniq);                                    \
        })

/* Same as the FLAG macros, but accept a 0-based bit index instead of a mask. Results in assertion failure if
 * index is out of range for the type. */
#घोषणा SET_BIT(bits, i) SET_FLAG(bits, सूचकांक_TO_MASK_2(typeof(bits), i), true)
#घोषणा CLEAR_BIT(bits, i) SET_FLAG(bits, सूचकांक_TO_MASK_2(typeof(bits), i), false)
#घोषणा BIT_SET(bits, i) FLAGS_SET(bits, सूचकांक_TO_MASK_2(typeof(bits), i))

/* As above, but accepts multiple indexes. Note the result will not be constant, even if all indexes are
 * constant. */
#घोषणा SET_BITS(bits, ...) SET_FLAG(bits, INDEXES_TO_MASK(typeof(bits), ##__VA_ARGS__), true)
#घोषणा CLEAR_BITS(bits, ...) SET_FLAG(bits, INDEXES_TO_MASK(typeof(bits), ##__VA_ARGS__), false)
#घोषणा BITS_SET(bits, ...) FLAGS_SET(bits, INDEXES_TO_MASK(typeof(bits), ##__VA_ARGS__))

/* Iterate through each set bit. Index is 0-based and type int. */
#घोषणा BIT_FOREACH(index, bits) _BIT_FOREACH(index, bits, UNIQ)
#घोषणा _BIT_FOREACH(index, bits, uniq)                                 \
        क्रम (पूर्णांक UNIQ_T(_last, uniq) = -1, index;                       \
             (index = BIT_अग्र_SET(bits, UNIQ_T(_last, uniq))) >= 0;    \
             UNIQ_T(_last, uniq) = index)

/* Find the next set bit after 0-based index 'prev'. Result is 0-based index of next set bit, or -1 if no
 * more bits are set. */
#घोषणा BIT_FIRST_SET(bits) BIT_अग्र_SET(bits, -1)
#घोषणा BIT_अग्र_SET(bits, पूर्व)                        \
        UNIQ_BIT_अग्र_SET(bits, पूर्व, UNIQ)
#घोषणा UNIQ_BIT_अग्र_SET(bits, पूर्व, uniq)                             \
        ({                                                              \
                typeof(bits) UNIQ_T(_bits, uniq) = (bits);              \
                पूर्णांक UNIQ_T(पूर्व_2, uniq) = (पूर्व);                       \
                पूर्णांक UNIQ_T(अग्र, uniq);                                \
                BIT_अग्र_SET_2(UNIQ_T(_bits, uniq),                      \
                              UNIQ_T(पूर्व_2, uniq),                      \
                              UNIQ_T(अग्र, uniq));                     \
        })
#घोषणा BIT_अग्र_SET_2(bits, पूर्व, अग्र_2)                                 \
        ((पूर्णांक)(पूर्व + 1) == (पूर्णांक)माप(bits) * 8                       \
         ? -1 /* Prev index was msb. */                                 \
         : ((अग्र_2 = __builtin_ffsll(((अचिन्हित दीर्घ दीर्घ)(bits)) >> (पूर्व + 1))) == 0 \
            ? -1 /* No more bits set. */                                \
            : पूर्व + अग्र_2))
