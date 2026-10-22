/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

स्थिरांक अक्षर* string_table_lookup_to_string(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *किल्ली) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *किल्ली, ssize_t yes) _pure_;

पूर्णांक string_table_lookup_to_string_fallback(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, ssize_t i, माप_प्रकार max, अक्षर **ret);
ssize_t string_table_lookup_from_string_fallback(स्थिरांक अक्षर * स्थिरांक *table, माप_प्रकार len, स्थिरांक अक्षर *s, माप_प्रकार max);

/* For basic lookup tables with strictly enumerated entries */
#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope स्थिरांक अक्षर* name##_to_string(type i) {                                                    \
                परत string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(स्थिरांक अक्षर *s) {             \
                परत (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(स्थिरांक अक्षर *s) {                               \
                परत (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope पूर्णांक name##_to_string_alloc(type i, अक्षर **ret) {                                          \
                परत string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(स्थिरांक अक्षर *s) {                           \
                परत (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP(name, type, scope)                    \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)          \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope)

#define _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, scope)  \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)          \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#define DEFINE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type,)
#define DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, स्थिर)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, स्थिर)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, स्थिर)

#define DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, स्थिर)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, स्थिर)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, स्थिर)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, स्थिर)

#define DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(मानक_निकास);                                      \
                साठी (type _k = 0; _k < (max); _k++) {                   \
                        स्थिरांक अक्षर *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        जर (!_t)                                        \
                                सुरूठेवा;                               \
                        fputs_unlocked(_t, मानक_निकास);                     \
                        fputc_unlocked('\n', मानक_निकास);                   \
                }                                                       \
                funlockfile(मानक_निकास);                                    \
                0;                                                      \
        })
