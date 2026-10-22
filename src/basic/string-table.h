/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <stdio.h>

#ضمن "forward.h"

ثابت حرف* string_جدول_lookup_منزل_string(ثابت حرف * ثابت *جدول, size_t len, ssize_t i) _const_;

ssize_t string_جدول_lookup_from_string(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلی) خالص;
ssize_t string_جدول_lookup_from_string_with_boolean(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلی, ssize_t yes) خالص;

صحيح string_جدول_lookup_منزل_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ssize_t i, size_t max, حرف **ret);
ssize_t string_جدول_lookup_from_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_منزل_STRING_2(name, type, scope)                                        \
        scope ثابت حرف* name##_to_string(type i) {                                                    \
                أرجع string_جدول_lookup_منزل_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ثابت حرف *s) {             \
                أرجع (type) string_جدول_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ثابت حرف *s) {                               \
                أرجع (type) string_جدول_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_منزل_STRING_FALLBACK(name, type, max, scope)                          \
        scope صحيح name##_to_string_alloc(type i, حرف **ret) {                                          \
                أرجع string_جدول_lookup_منزل_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ثابت حرف *s) {                           \
                أرجع (type) string_جدول_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_جدول_LOOKUP_منزل_STRING_2(name, type, scope)          \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, scope)

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_جدول_LOOKUP_منزل_STRING_2(name, type, scope)          \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP(name, type) DEFINE_STRING_جدول_LOOKUP_2(name, type,)
#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_منزل_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_منزل_STRING_2(name, type,)
#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type,)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP(name, type) DEFINE_STRING_جدول_LOOKUP_2(name, type, ساكن)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_منزل_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_منزل_STRING_2(name, type, ساكن)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, ساكن)

#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes, ساكن)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, ساكن)

/* For string conversions where numbers are also acceptable */
#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_جدول_LOOKUP_منزل_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#تعریف_کړه DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_منزل_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_جدول_LOOKUP_منزل_STRING_FALLBACK(name, type, max, ساكن)
#تعریف_کړه DEFINE_شخصي_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max, ساكن)

#تعریف_کړه DUMP_STRING_جدول(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                لكل (type _k = 0; _k < (max); _k++) {                   \
                        ثابت حرف *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        إذا (!_t)                                        \
                                استمر;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
