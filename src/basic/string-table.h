/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <stdio.h>

#شامل "forward.h"

ثابت حرف* string_جدول_lookup_مقصد_string(ثابت حرف * ثابت *جدول, size_t len, ssize_t i) _const_;

ssize_t string_جدول_lookup_from_string(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلید) خالص;
ssize_t string_جدول_lookup_from_string_with_boolean(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *کلید, ssize_t yes) خالص;

صحیح string_جدول_lookup_مقصد_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ssize_t i, size_t max, حرف **ret);
ssize_t string_جدول_lookup_from_string_fallback(ثابت حرف * ثابت *جدول, size_t len, ثابت حرف *s, size_t max);

/* For basic lookup tables with strictly enumerated entries */
#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_2(name, type, scope)                                        \
        scope ثابت حرف* name##_to_string(type i) {                                                    \
                واپس string_جدول_lookup_مقصد_string(name##_table, ELEMENTSOF(name##_table), i);        \
        }

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, scope) \
        scope type name##_from_string(ثابت حرف *s) {             \
                واپس (type) string_جدول_lookup_from_string(name##_table, ELEMENTSOF(name##_table), s); \
        }

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope) \
        scope type name##_from_string(ثابت حرف *s) {                               \
                واپس (type) string_جدول_lookup_from_string_with_boolean(name##_table, ELEMENTSOF(name##_table), s, yes); \
        }

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_FALLBACK(name, type, max, scope)                          \
        scope صحیح name##_to_string_alloc(type i, حرف **ret) {                                          \
                واپس string_جدول_lookup_مقصد_string_fallback(name##_table, ELEMENTSOF(name##_table), i, max, ret); \
        }

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max, scope) \
        scope type name##_from_string(ثابت حرف *s) {                           \
                واپس (type) string_جدول_lookup_from_string_fallback(name##_table, ELEMENTSOF(name##_table), s, max); \
        }

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_2(name, type, scope)                    \
        DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_2(name, type, scope)          \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, scope)

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes, scope)  \
        DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_2(name, type, scope)          \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, scope)

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP(name, type) DEFINE_STRING_جدول_LOOKUP_2(name, type,)
#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_مقصد_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_2(name, type,)
#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_FROM_STRING_2(name, type) DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type,)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP(name, type) DEFINE_STRING_جدول_LOOKUP_2(name, type, ساکن)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_2(name, type, ساکن)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_FROM_STRING(name, type) DEFINE_STRING_جدول_LOOKUP_FROM_STRING(name, type, ساکن)

#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes,)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_WITH_BOOLEAN(name, type, yes) DEFINE_STRING_جدول_LOOKUP_WITH_BOOLEAN_2(name, type, yes, ساکن)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes) \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_BOOLEAN(name, type, yes, ساکن)

/* For string conversions where numbers are also acceptable */
#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(name, type, max)         \
        DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_FALLBACK(name, type, max,)  \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)
#تعریف_کرو DEFINE_STRING_جدول_LOOKUP_FROM_STRING_WITH_FALLBACK(name, type, max) DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max,)

#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_جدول_LOOKUP_مقصد_STRING_FALLBACK(name, type, max, ساکن)
#تعریف_کرو DEFINE_نجی_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max) \
        DEFINE_STRING_جدول_LOOKUP_FROM_STRING_FALLBACK(name, type, max, ساکن)

#تعریف_کرو DUMP_STRING_جدول(name, type, max)                              \
        ({                                                              \
                flockfile(stdout);                                      \
                برائے (type _k = 0; _k < (max); _k++) {                   \
                        ثابت حرف *_t;                                 \
                        _t = name##_to_string(_k);                      \
                        اگر (!_t)                                        \
                                جاری;                               \
                        fputs_unlocked(_t, stdout);                     \
                        fputc_unlocked('\n', stdout);                   \
                }                                                       \
                funlockfile(stdout);                                    \
                0;                                                      \
        })
