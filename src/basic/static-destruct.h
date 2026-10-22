/* SPDX-License-Identifier: LGPL-2.1-or-later */

#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश शून्य (*free_func_t)(शून्य *p);

/* A framework for registering static variables that shall be freed on shutdown of a process. It's a bit like gcc's
 * destructor attribute, but allows us to precisely schedule when we want to free the variables. This is supposed to
 * feel a bit like the gcc cleanup attribute, but for static variables. Note that this does not work for static
 * variables declared in .so's, as the list is private to the same linking unit. But maybe that's a good thing. */

#परिभाषय _common_static_destruct_attrs_                                  \
        /* The actual destructor structure we place in a special section to find it. */ \
        _section_("SYSTEMD_STATIC_DESTRUCT")                            \
        /* Use pointer alignment, since that is apparently what gcc does for static variables. */ \
        _alignptr_                                                      \
        /* Make sure this is not dropped from the image despite not being explicitly referenced. */ \
        प्रयुक्तम्_2                                                          \
        /* Prevent garbage collection by the linker. */                 \
        _retain_                                                        \
        /* Make sure that AddressSanitizer doesn't pad this variable: we want everything in this section
         * packed next to each other so that we can enumerate it. */    \
        variable_no_sanitize_स्थानाङ्कः

प्रकारनिर्देश गणना StaticDestructorType {
        STATIC_DESTRUCTOR_SIMPLE,
        STATIC_DESTRUCTOR_ARRAY,
        _STATIC_DESTRUCTOR_TYPE_MAX,
        _STATIC_DESTRUCTOR_INVALID = -EINVAL,
} StaticDestructorType;

प्रकारनिर्देश संरचना SimpleCleanup {
        शून्य *दत्त;
        free_func_t destroy;
} SimpleCleanup;

/* Note: see the comment on struct Option in options.h for why _alignptr_ is required here. */
प्रकारनिर्देश संरचना _alignptr_ StaticDestructor {
        StaticDestructorType type;
        सङ्घ {
                SimpleCleanup simple;
                ArrayCleanup array;
        };
} StaticDestructor;

assert_cc(परिमाण(StaticDestructor) % परिमाण(शून्य*) == 0);

#परिभाषय STATIC_DESTRUCTOR_पञ्जिकाकोशः(variable, func) \
        STATIC_DESTRUCTOR_पञ्जिकाकोशः_2(UNIQ, variable, func)

#परिभाषय STATIC_DESTRUCTOR_पञ्जिकाकोशः_2(uq, variable, func)                 \
        /* Type-safe destructor */                                      \
        स्थिर शून्य UNIQ_T(static_destructor_wrapper, uq)(शून्य *p) {    \
                typeof(variable) *q = p;                                \
                func(q);                                                \
        }                                                               \
        _common_static_destruct_attrs_                                  \
        स्थिर ध्रुव StaticDestructor UNIQ_T(static_destructor_प्रविष्टिः, uq) = { \
                .type = STATIC_DESTRUCTOR_SIMPLE,                       \
                .simple.दत्त = &(variable),                             \
                .simple.destroy = UNIQ_T(static_destructor_wrapper, uq), \
        }

#परिभाषय STATIC_ARRAY_DESTRUCTOR_पञ्जिकाकोशः(a, n, func)            \
        STATIC_ARRAY_DESTRUCTOR_पञ्जिकाकोशः_2(UNIQ, a, n, func)

#परिभाषय STATIC_ARRAY_DESTRUCTOR_पञ्जिकाकोशः_2(uq, a, n, func)               \
        /* Type-safety check */                                         \
        अप्रयुक्तम् स्थिर शून्य (* UNIQ_T(static_destructor_wrapper, uq))(typeof(a[0]) *x, size_t y) = (func); \
        _common_static_destruct_attrs_                                  \
        स्थिर ध्रुव StaticDestructor UNIQ_T(static_destructor_प्रविष्टिः, uq) = { \
                .type = STATIC_DESTRUCTOR_ARRAY,                        \
                .array.parray = (शून्य**) &(a),                          \
                .array.pn = &(n),                                       \
                .array.pfunc = (free_array_func_t) (func),              \
        };

/* Beginning and end of our section listing the destructors. We define these as weak as we want this to work
 * even if no destructors are defined and the section is missing. */
बाह्य ध्रुव StaticDestructor _weak_ __start_SYSTEMD_STATIC_DESTRUCT[];
बाह्य ध्रुव StaticDestructor _weak_ __stop_SYSTEMD_STATIC_DESTRUCT[];

शून्य static_destruct_impl(ध्रुव StaticDestructor *start, ध्रुव StaticDestructor *अन्तः);

/* The function to destroy everything. (Note that this must be static inline, as it's key that it remains in
 * the same linking unit as the variables we want to destroy.) */
स्थिर अन्तःस्थ शून्य static_destruct(शून्य) {
        प्रत्यागम static_destruct_impl(__start_SYSTEMD_STATIC_DESTRUCT, __stop_SYSTEMD_STATIC_DESTRUCT);
}
