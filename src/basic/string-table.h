/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ssize_t i) _const_;

ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ) ಶುದ್ಧ;
ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string_with_boolean(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ssize_t yes) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string_fallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ssize_t i, size_t max, ಅಕ್ಷರ **ret);
ssize_t string_ಕೋಷ್ಟಕ_lookup_from_string_fallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *ಕೋಷ್ಟಕ, size_t len, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_2(name, type, scope)                                        \
        scope ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name##_to_string(type i) {                                                    \
                ಹಿಂತಿರುಗಿ string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {             \
                ಹಿಂತಿರುಗಿ (type) string_ಕೋಷ್ಟಕ_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {                               \
                ಹಿಂತಿರುಗಿ (type) string_ಕೋಷ್ಟಕ_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_FALLBACK(name, type, max, scope)                          \
        scope ಪೂರ್ಣಾಂಕ name##_to_string_alloc(type i, ಅಕ್ಷರ **ret) {                                          \
                ಹಿಂತಿರುಗಿ string_ಕೋಷ್ಟಕ_lookup_ಗಮ್ಯ_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {                           \
                ಹಿಂತಿರುಗಿ (type) string_ಕೋಷ್ಟಕ_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_2(name, type, scope)          \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING(name, type, scope)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_2(name, type, scope)          \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_2(name, type,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_2(name, type,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING(name, type,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_2(name, type, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_2(name, type, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING(name, type, ಸ್ಥಿರ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_BOOLEAN_2(name, type, yes, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, ಸ್ಥಿರ)

/* For string conversions where numbers are also acceptable */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING_FALLBACK(name, type, max, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_FROM_STRING_FALLBACK(name, type, max, ಸ್ಥಿರ)

#ವ್ಯಾಖ್ಯಾನಿಸು DUMP_STRING_ಕೋಷ್ಟಕ(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                ಗಾಗಿ (type _k = 0; _k < (max); _k++) {                   \
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        ಆದರೆ (!_t)                                        \
                                ಮುಂದುವರಿ;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
