/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "forward.h"

மாறிலி எழுத்து* string_table_lookup_to_string(மாறிலி எழுத்து * மாறிலி *table, size_t len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *சாவி) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *சாவி, ssize_t yes) _pure_;

முழு string_table_lookup_to_string_fallback(மாறிலி எழுத்து * மாறிலி *table, size_t len, ssize_t i, size_t max, எழுத்து **ret);
ssize_t string_table_lookup_from_string_fallback(மாறிலி எழுத்து * மாறிலி *table, size_t len, மாறிலி எழுத்து *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope மாறிலி எழுத்து* name##_to_string(type i) {                                                    \
                திரும்பு string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(மாறிலி எழுத்து *s) {             \
                திரும்பு (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(மாறிலி எழுத்து *s) {                               \
                திரும்பு (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope முழு name##_to_string_alloc(type i, எழுத்து **ret) {                                          \
                திரும்பு string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(மாறிலி எழுத்து *s) {                           \
                திரும்பு (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
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
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, நிலையான)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, நிலையான)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, நிலையான)

#define DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, நிலையான)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, நிலையான)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, நிலையான)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, நிலையான)

#define DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                க்கு (type _k = 0; _k < (max); _k++) {                   \
                        மாறிலி எழுத்து *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        எனில் (!_t)                                        \
                                தொடர்;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
