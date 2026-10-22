/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "forward.h"

ध्रुव वर्ण* string_सारणी_lookup_गन्तव्यम्_string(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ssize_t i) _const_;

ssize_t string_सारणी_lookup_from_string(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *कुञ्चिका) विशुद्धम्;
ssize_t string_सारणी_lookup_from_string_with_boolean(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *कुञ्चिका, ssize_t yes) विशुद्धम्;

पूर्णाङ्क string_सारणी_lookup_गन्तव्यम्_string_fallback(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ssize_t i, size_t max, वर्ण **ret);
ssize_t string_सारणी_lookup_from_string_fallback(ध्रुव वर्ण * ध्रुव *सारणी, size_t len, ध्रुव वर्ण *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#परिभाषय DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_2(name, type, scope)                                        \
        scope ध्रुव वर्ण* name##_to_string(type i) {                                                    \
                प्रत्यागम string_सारणी_lookup_गन्तव्यम्_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ध्रुव वर्ण *s) {             \
                प्रत्यागम (type) string_सारणी_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ध्रुव वर्ण *s) {                               \
                प्रत्यागम (type) string_सारणी_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_FALLBACK(name, type, max, scope)                          \
        scope पूर्णाङ्क name##_to_string_alloc(type i, वर्ण **ret) {                                          \
                प्रत्यागम string_सारणी_lookup_गन्तव्यम्_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ध्रुव वर्ण *s) {                           \
                प्रत्यागम (type) string_सारणी_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_2(name, type, scope)          \
        DEFINE_STRING_सारणी_LOOKUP_FROM_STRING(name, type, scope)

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_2(name, type, scope)          \
        DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#परिभाषय DEFINE_STRING_सारणी_LOOKUP(name, type) DEFINE_STRING_सारणी_LOOKUP_2(name, type,)
#परिभाषय DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(name, type) DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_2(name, type,)
#परिभाषय DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_सारणी_LOOKUP_FROM_STRING(name, type,)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP(name, type) DEFINE_STRING_सारणी_LOOKUP_2(name, type, स्थिर)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(name, type) DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_2(name, type, स्थिर)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_सारणी_LOOKUP_FROM_STRING(name, type, स्थिर)

#परिभाषय DEFINE_STRING_सारणी_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_सारणी_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_सारणी_LOOKUP_WITH_BOOLEAN_2(name, type, yes, स्थिर)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, स्थिर)

/* For string conversions where numbers are also acceptable */
#परिभाषय DEFINE_STRING_सारणी_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#परिभाषय DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING_FALLBACK(name, type, max, स्थिर)
#परिभाषय DEFINE_वैयक्तिकम्_STRING_सारणी_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_सारणी_LOOKUP_FROM_STRING_FALLBACK(name, type, max, स्थिर)

#परिभाषय DUMP_STRING_सारणी(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                प्रति (type _k = 0; _k < (max); _k++) {                   \
                        ध्रुव वर्ण *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        यदि (!_t)                                        \
                                अनुवर्त;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
