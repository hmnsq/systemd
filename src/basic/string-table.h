/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "forward.h"

ধ্রুব বর্ণ* string_table_lookup_to_string(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *চাবি) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *চাবি, ssize_t yes) _pure_;

পূর্ণাঙ্ক string_table_lookup_to_string_fallback(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ssize_t i, size_t max, বর্ণ **ret);
ssize_t string_table_lookup_from_string_fallback(ধ্রুব বর্ণ * ধ্রুব *table, size_t len, ধ্রুব বর্ণ *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope ধ্রুব বর্ণ* name##_to_string(type i) {                                                    \
                প্রত্যাগম string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ধ্রুব বর্ণ *s) {             \
                প্রত্যাগম (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ধ্রুব বর্ণ *s) {                               \
                প্রত্যাগম (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope পূর্ণাঙ্ক name##_to_string_alloc(type i, বর্ণ **ret) {                                          \
                প্রত্যাগম string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ধ্রুব বর্ণ *s) {                           \
                প্রত্যাগম (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
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
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, স্থির)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, স্থির)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, স্থির)

#define DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, স্থির)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, স্থির)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, স্থির)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, স্থির)

#define DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                প্রতি (type _k = 0; _k < (max); _k++) {                   \
                        ধ্রুব বর্ণ *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        যদি (!_t)                                        \
                                অনুবর্ত;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
