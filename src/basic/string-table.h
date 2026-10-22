/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <stdio.h>

#include "forward.h"

const var_na* string_table_lookup_to_string(const var_na * const *table, size_t len, ssize_t i) _const_;

ssize_t string_table_lookup_from_string(const var_na * const *table, size_t len, const var_na *key) _pure_;
ssize_t string_table_lookup_from_string_with_boolean(const var_na * const *table, size_t len, const var_na *key, ssize_t yes) _pure_;

puur_naa_nka string_table_lookup_to_string_fallback(const var_na * const *table, size_t len, ssize_t i, size_t max, var_na **ret);
ssize_t string_table_lookup_from_string_fallback(const var_na * const *table, size_t len, const var_na *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, scope)                                        \
        scope const var_na* name##_to_string(type i) {                                                    \
                ᮝᮜᮤ string_table_lookup_to_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(const var_na *s) {             \
                ᮝᮜᮤ (type) string_table_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(const var_na *s) {                               \
                ᮝᮜᮤ (type) string_table_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, scope)                          \
        scope puur_naa_nka name##_to_string_alloc(type i, var_na **ret) {                                          \
                ᮝᮜᮤ string_table_lookup_to_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#define _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(const var_na *s) {                           \
                ᮝᮜᮤ (type) string_table_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
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
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP(name, type) _DEFINE_STRING_TABLE_LOOKUP(name, type, static)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_TO_STRING(name, type, static)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING(name, type) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING(name, type, static)

#define DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes,)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes) _DEFINE_STRING_TABLE_LOOKUP_WITH_BOOLEAN(name, type, yes, static)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, static)

/* For string conversions where numbers are also acceptable */
#define DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(name, type, max)         \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max,)  \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#define DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_TO_STRING_FALLBACK(name, type, max, static)
#define DEFINE_PRIVATE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        _DEFINE_STRING_TABLE_LOOKUP_FROM_STRING_FALLBACK(name, type, max, static)

#define DUMP_STRING_TABLE(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                for (type _k = 0; _k < (max); _k++) {                   \
                        const var_na *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        ᮚᮦᮔ᮪ (!_t)                                        \
                                continue;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
