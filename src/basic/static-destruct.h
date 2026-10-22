/* SPDX-License-Identifier: LGPL-2.1-or-later */

#رهنمود once

#ضمن "forward.h"

تعريفنوع فراغ (*free_func_t)(فراغ *p);

/* A framework for registering static variables that shall be freed on shutdown of a process. It's a bit like gcc's
 * destructor attribute, but allows us to precisely schedule when we want to free the variables. This is supposed to
 * feel a bit like the gcc cleanup attribute, but for static variables. Note that this does not work for static
 * variables declared in .so's, as the list is private to the same linking unit. But maybe that's a good thing. */

#تعریف_کن _common_static_destruct_attrs_                                  \
        /* The actual destructor structure we place in a special section to find it. */ \
        _section_("SYSTEMD_STATIC_DESTRUCT")                            \
        /* Use pointer alignment, since that is apparently what gcc does for static variables. */ \
        _alignptr_                                                      \
        /* Make sure this is not dropped from the image despite not being explicitly referenced. */ \
        استفاده_شده_2                                                          \
        /* Prevent garbage collection by the linker. */                 \
        _retain_                                                        \
        /* Make sure that AddressSanitizer doesn't pad this variable: we want everything in this section
         * packed next to each other so that we can enumerate it. */    \
        variable_no_sanitize_آدرس

تعريفنوع تعداد StaticDestructorType {
        STATIC_DESTRUCTOR_SIMPLE,
        STATIC_DESTRUCTOR_ARRAY,
        _STATIC_DESTRUCTOR_TYPE_MAX,
        _STATIC_DESTRUCTOR_INVALID = -EINVAL,
} StaticDestructorType;

تعريفنوع بنية SimpleCleanup {
        فراغ *داده;
        free_func_t destroy;
} SimpleCleanup;

/* Note: see the comment on struct Option in options.h for why _alignptr_ is required here. */
تعريفنوع بنية _alignptr_ StaticDestructor {
        StaticDestructorType type;
        اتحاد {
                SimpleCleanup simple;
                ArrayCleanup array;
        };
} StaticDestructor;

assert_cc(حجم(StaticDestructor) % حجم(فراغ*) == 0);

#تعریف_کن STATIC_DESTRUCTOR_ثبات(variable, func) \
        STATIC_DESTRUCTOR_ثبات_2(UNIQ, variable, func)

#تعریف_کن STATIC_DESTRUCTOR_ثبات_2(uq, variable, func)                 \
        /* Type-safe destructor */                                      \
        ساكن فراغ UNIQ_T(static_destructor_wrapper, uq)(فراغ *p) {    \
                typeof(variable) *q = p;                                \
                func(q);                                                \
        }                                                               \
        _common_static_destruct_attrs_                                  \
        ساكن ثابت StaticDestructor UNIQ_T(static_destructor_مدخل, uq) = { \
                .type = STATIC_DESTRUCTOR_SIMPLE,                       \
                .simple.داده = &(variable),                             \
                .simple.destroy = UNIQ_T(static_destructor_wrapper, uq), \
        }

#تعریف_کن STATIC_ARRAY_DESTRUCTOR_ثبات(a, n, func)            \
        STATIC_ARRAY_DESTRUCTOR_ثبات_2(UNIQ, a, n, func)

#تعریف_کن STATIC_ARRAY_DESTRUCTOR_ثبات_2(uq, a, n, func)               \
        /* Type-safety check */                                         \
        استفادهنشده ساكن فراغ (* UNIQ_T(static_destructor_wrapper, uq))(typeof(a[0]) *x, size_t y) = (func); \
        _common_static_destruct_attrs_                                  \
        ساكن ثابت StaticDestructor UNIQ_T(static_destructor_مدخل, uq) = { \
                .type = STATIC_DESTRUCTOR_ARRAY,                        \
                .array.parray = (فراغ**) &(a),                          \
                .array.pn = &(n),                                       \
                .array.pfunc = (free_array_func_t) (func),              \
        };

/* Beginning and end of our section listing the destructors. We define these as weak as we want this to work
 * even if no destructors are defined and the section is missing. */
خارجي ثابت StaticDestructor _weak_ __start_SYSTEMD_STATIC_DESTRUCT[];
خارجي ثابت StaticDestructor _weak_ __stop_SYSTEMD_STATIC_DESTRUCT[];

فراغ static_destruct_impl(ثابت StaticDestructor *شروع, ثابت StaticDestructor *انتها);

/* The function to destroy everything. (Note that this must be static inline, as it's key that it remains in
 * the same linking unit as the variables we want to destroy.) */
ساكن مضمن فراغ static_destruct(فراغ) {
        أرجع static_destruct_impl(__start_SYSTEMD_STATIC_DESTRUCT, __stop_SYSTEMD_STATIC_DESTRUCT);
}
