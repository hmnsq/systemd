/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਅਚਲ ਅੱਖਰ* string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ssize_t i) _const_;

ssize_t string_ਸਾਰਣੀ_lookup_from_string(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ) ਸ਼ੁੱਧ;
ssize_t string_ਸਾਰਣੀ_lookup_from_string_with_boolean(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ssize_t yes) ਸ਼ੁੱਧ;

ਪੂਰਨ string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string_fallback(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ssize_t i, size_t max, ਅੱਖਰ **ret);
ssize_t string_ਸਾਰਣੀ_lookup_from_string_fallback(ਅਚਲ ਅੱਖਰ * ਅਚਲ *ਸਾਰਣੀ, size_t len, ਅਚਲ ਅੱਖਰ *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_2(name, type, scope)                                        \
        scope ਅਚਲ ਅੱਖਰ* name##_to_string(type i) {                                                    \
                ਵਾਪਸ string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ਅਚਲ ਅੱਖਰ *s) {             \
                ਵਾਪਸ (type) string_ਸਾਰਣੀ_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ਅਚਲ ਅੱਖਰ *s) {                               \
                ਵਾਪਸ (type) string_ਸਾਰਣੀ_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_FALLBACK(name, type, max, scope)                          \
        scope ਪੂਰਨ name##_to_string_alloc(type i, ਅੱਖਰ **ret) {                                          \
                ਵਾਪਸ string_ਸਾਰਣੀ_lookup_ਮੰਜ਼ਿਲ_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ਅਚਲ ਅੱਖਰ *s) {                           \
                ਵਾਪਸ (type) string_ਸਾਰਣੀ_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_2(name, type, scope)          \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(name, type, scope)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_2(name, type, scope)          \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_2(name, type,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_2(name, type,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(name, type,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_2(name, type, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_2(name, type, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING(name, type, ਸਥਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_BOOLEAN_2(name, type, yes, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, ਸਥਿਰ)

/* For string conversions where numbers are also acceptable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING_FALLBACK(name, type, max, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_FROM_STRING_FALLBACK(name, type, max, ਸਥਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUMP_STRING_ਸਾਰਣੀ(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                ਲਈ (type _k = 0; _k < (max); _k++) {                   \
                        ਅਚਲ ਅੱਖਰ *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        ਜੇ (!_t)                                        \
                                ਜਾਰੀ;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
