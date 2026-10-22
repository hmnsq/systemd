/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅଚଳ ଅକ୍ଷର* string_table_lookup_to_string(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *ଚାବି) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *ଚାବି, ssize_t yes) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_table_lookup_to_string_fallback(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ssize_t i, size_t max, ଅକ୍ଷର **ret);
ssize_t string_table_lookup_from_string_fallback(ଅଚଳ ଅକ୍ଷର * ଅଚଳ *table, size_t len, ଅଚଳ ଅକ୍ଷର *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope ଅଚଳ ଅକ୍ଷର* name##_to_string(type i) {                                                    \
                ଫେରନ୍ତୁ string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ଅଚଳ ଅକ୍ଷର *s) {             \
                ଫେରନ୍ତୁ (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ଅଚଳ ଅକ୍ଷର *s) {                               \
                ଫେରନ୍ତୁ (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope ପୂର୍ଣ୍ଣସଂଖ୍ୟା name##_to_string_alloc(type i, ଅକ୍ଷର **ret) {                                          \
                ଫେରନ୍ତୁ string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ଅଚଳ ଅକ୍ଷର *s) {                           \
                ଫେରନ୍ତୁ (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
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
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, ସ୍ଥିର)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, ସ୍ଥିର)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, ସ୍ଥିର)

#define DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, ସ୍ଥିର)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, ସ୍ଥିର)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, ସ୍ଥିର)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, ସ୍ଥିର)

#define DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                ପାଇଁ (type _k = 0; _k < (max); _k++) {                   \
                        ଅଚଳ ଅକ୍ଷର *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        ଯଦି (!_t)                                        \
                                ଜାରି;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
