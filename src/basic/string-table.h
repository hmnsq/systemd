/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <stdio.h>

#समावेश "forward.h"

स्थिर अक्षर* string_table_lookup_to_string(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *कुंजी) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *कुंजी, ssize_t yes) _pure_;

पूर्णांक string_table_lookup_to_string_fallback(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, ssize_t i, माप_प्रकार max, अक्षर **ret);
ssize_t string_table_lookup_from_string_fallback(स्थिर अक्षर * स्थिर *table, माप_प्रकार len, स्थिर अक्षर *s, माप_प्रकार max);

/* For basic lookup tables with strictly enumerated entries */
#घोषणा _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope स्थिर अक्षर* name##_to_string(type i) {                                                    \
                वापस string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#घोषणा _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(स्थिर अक्षर *s) {             \
                वापस (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#घोषणा _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(स्थिर अक्षर *s) {                               \
                वापस (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#घोषणा _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope पूर्णांक name##_to_string_alloc(type i, अक्षर **ret) {                                          \
                वापस string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#घोषणा _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(स्थिर अक्षर *s) {                           \
                वापस (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#घोषणा _DEFINE_STRING_TABLE_LOOKUP(name, type, scope)                    \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)          \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope)

#घोषणा _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, scope)  \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)          \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#घोषणा DEFINE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type,)
#घोषणा DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type,)
#घोषणा DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type,)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, अटल)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, अटल)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, अटल)

#घोषणा DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, अटल)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, अटल)

/* For string conversions where numbers are also acceptable */
#घोषणा DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#घोषणा DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, अटल)
#घोषणा DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, अटल)

#घोषणा DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(मानक_निकास);                                      \
                क्रम (type _k = 0; _k < (max); _k++) {                   \
                        स्थिर अक्षर *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        अगर (!_t)                                        \
                                जारी;                               \
                        fputs_unlocked(_t, मानक_निकास);                     \
                        fputc_unlocked('\n', मानक_निकास);                   \
                }                                                       \
                funlockfile(मानक_निकास);                                    \
                0;                                                      \
        })
