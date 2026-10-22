/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை வெறுமை (*free_func_t)(வெறுமை *p);

/* A framework for registering static variables that shall be freed on shutdown of a process. It's a bit like gcc's
 * destructor attribute, but allows us to precisely schedule when we want to free the variables. This is supposed to
 * feel a bit like the gcc cleanup attribute, but for static variables. Note that this does not work for static
 * variables declared in .so's, as the list is private to the same linking unit. But maybe that's a good thing. */

#define _common_static_destruct_attrs_                                  \
        /* The actual destructor structure we place in a special section to find it. */ \
        _section_("SYSTEMD_STATIC_DESTRUCT")                            \
        /* Use pointer alignment, since that is apparently what gcc does for static variables. */ \
        _alignptr_                                                      \
        /* Make sure this is not dropped from the image despite not being explicitly referenced. */ \
        _used_                                                          \
        /* Prevent garbage collection by the linker. */                 \
        _retain_                                                        \
        /* Make sure that AddressSanitizer doesn't pad this variable: we want everything in this section
         * packed next to each other so that we can enumerate it. */    \
        _variable_no_sanitize_address_

வகைவரை எண்ணிக்கை StaticDestructorType {
        STATIC_DESTRUCTOR_SIMPLE,
        STATIC_DESTRUCTOR_ARRAY,
        _STATIC_DESTRUCTOR_TYPE_MAX,
        _STATIC_DESTRUCTOR_INVALID = -EINVAL,
} StaticDestructorType;

வகைவரை கட்டமைப்பு SimpleCleanup {
        வெறுமை *data;
        free_func_t destroy;
} SimpleCleanup;

/* Note: see the comment on struct Option in options.h for why _alignptr_ is required here. */
வகைவரை கட்டமைப்பு _alignptr_ StaticDestructor {
        StaticDestructorType type;
        ஒன்றிப்பு {
                SimpleCleanup simple;
                ArrayCleanup array;
        };
} StaticDestructor;

assert_cc(அளவு(StaticDestructor) % அளவு(வெறுமை*) == 0);

#define STATIC_DESTRUCTOR_REGISTER(variable, func) \
        _STATIC_DESTRUCTOR_REGISTER(UNIQ, variable, func)

#define _STATIC_DESTRUCTOR_REGISTER(uq, variable, func)                 \
        /* Type-safe destructor */                                      \
        நிலையான வெறுமை UNIQ_T(static_destructor_wrapper, uq)(வெறுமை *p) {    \
                typeof(variable) *q = p;                                \
                func(q);                                                \
        }                                                               \
        _common_static_destruct_attrs_                                  \
        நிலையான மாறிலி StaticDestructor UNIQ_T(static_destructor_entry, uq) = { \
                .type = STATIC_DESTRUCTOR_SIMPLE,                       \
                .simple.data = &(variable),                             \
                .simple.destroy = UNIQ_T(static_destructor_wrapper, uq), \
        }

#define STATIC_ARRAY_DESTRUCTOR_REGISTER(a, n, func)            \
        _STATIC_ARRAY_DESTRUCTOR_REGISTER(UNIQ, a, n, func)

#define _STATIC_ARRAY_DESTRUCTOR_REGISTER(uq, a, n, func)               \
        /* Type-safety check */                                         \
        _unused_ நிலையான வெறுமை (* UNIQ_T(static_destructor_wrapper, uq))(typeof(a[0]) *x, size_t y) = (func); \
        _common_static_destruct_attrs_                                  \
        நிலையான மாறிலி StaticDestructor UNIQ_T(static_destructor_entry, uq) = { \
                .type = STATIC_DESTRUCTOR_ARRAY,                        \
                .array.parray = (வெறுமை**) &(a),                          \
                .array.pn = &(n),                                       \
                .array.pfunc = (free_array_func_t) (func),              \
        };

/* Beginning and end of our section listing the destructors. We define these as weak as we want this to work
 * even if no destructors are defined and the section is missing. */
வெளி மாறிலி StaticDestructor _weak_ __start_SYSTEMD_STATIC_DESTRUCT[];
வெளி மாறிலி StaticDestructor _weak_ __stop_SYSTEMD_STATIC_DESTRUCT[];

வெறுமை static_destruct_impl(மாறிலி StaticDestructor *start, மாறிலி StaticDestructor *end);

/* The function to destroy everything. (Note that this must be static inline, as it's key that it remains in
 * the same linking unit as the variables we want to destroy.) */
நிலையான உள்வரி வெறுமை static_destruct(வெறுமை) {
        திரும்பு static_destruct_impl(__start_SYSTEMD_STATIC_DESTRUCT, __stop_SYSTEMD_STATIC_DESTRUCT);
}
