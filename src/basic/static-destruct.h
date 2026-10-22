/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*free_func_t)(ಶೂನ್ಯ *p);

/* A framework for registering static variables that shall be freed on shutdown of a process. It's a bit like gcc's
 * destructor attribute, but allows us to precisely schedule when we want to free the variables. This is supposed to
 * feel a bit like the gcc cleanup attribute, but for static variables. Note that this does not work for static
 * variables declared in .so's, as the list is private to the same linking unit. But maybe that's a good thing. */

#ವ್ಯಾಖ್ಯಾನಿಸು _common_static_destruct_attrs_                                  \
        /* The actual destructor structure we place in a special section to find it. */ \
        _section_("SYSTEMD_STATIC_DESTRUCT")                            \
        /* Use pointer alignment, since that is apparently what gcc does for static variables. */ \
        _alignptr_                                                      \
        /* Make sure this is not dropped from the image despite not being explicitly referenced. */ \
        ಬಳಸಿದ_2                                                          \
        /* Prevent garbage collection by the linker. */                 \
        _retain_                                                        \
        /* Make sure that AddressSanitizer doesn't pad this variable: we want everything in this section
         * packed next to each other so that we can enumerate it. */    \
        variable_no_sanitize_ವಿಳಾಸ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ StaticDestructorType {
        STATIC_DESTRUCTOR_SIMPLE,
        STATIC_DESTRUCTOR_ARRAY,
        _STATIC_DESTRUCTOR_TYPE_MAX,
        _STATIC_DESTRUCTOR_INVALID = -EINVAL,
} StaticDestructorType;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ SimpleCleanup {
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;
        free_func_t destroy;
} SimpleCleanup;

/* Note: see the comment on struct Option in options.h for why _alignptr_ is required here. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _alignptr_ StaticDestructor {
        StaticDestructorType type;
        ಒಕ್ಕೂಟ {
                SimpleCleanup simple;
                ArrayCleanup array;
        };
} StaticDestructor;

assert_cc(ಗಾತ್ರ(StaticDestructor) % ಗಾತ್ರ(ಶೂನ್ಯ*) == 0);

#ವ್ಯಾಖ್ಯಾನಿಸು STATIC_DESTRUCTOR_ರಿಜಿಸ್ಟರ್(variable, func) \
        STATIC_DESTRUCTOR_ರಿಜಿಸ್ಟರ್_2(UNIQ, variable, func)

#ವ್ಯಾಖ್ಯಾನಿಸು STATIC_DESTRUCTOR_ರಿಜಿಸ್ಟರ್_2(uq, variable, func)                 \
        /* Type-safe destructor */                                      \
        ಸ್ಥಿರ ಶೂನ್ಯ UNIQ_T(static_destructor_wrapper, uq)(ಶೂನ್ಯ *p) {    \
                typeof(variable) *q = p;                                \
                func(q);                                                \
        }                                                               \
        _common_static_destruct_attrs_                                  \
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ StaticDestructor UNIQ_T(static_destructor_ನಮೂದು, uq) = { \
                .type = STATIC_DESTRUCTOR_SIMPLE,                       \
                .simple.ದತ್ತಾಂಶ = &(variable),                             \
                .simple.destroy = UNIQ_T(static_destructor_wrapper, uq), \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು STATIC_ARRAY_DESTRUCTOR_ರಿಜಿಸ್ಟರ್(a, n, func)            \
        STATIC_ARRAY_DESTRUCTOR_ರಿಜಿಸ್ಟರ್_2(UNIQ, a, n, func)

#ವ್ಯಾಖ್ಯಾನಿಸು STATIC_ARRAY_DESTRUCTOR_ರಿಜಿಸ್ಟರ್_2(uq, a, n, func)               \
        /* Type-safety check */                                         \
        ಬಳಸದ ಸ್ಥಿರ ಶೂನ್ಯ (* UNIQ_T(static_destructor_wrapper, uq))(typeof(a[0]) *x, size_t y) = (func); \
        _common_static_destruct_attrs_                                  \
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ StaticDestructor UNIQ_T(static_destructor_ನಮೂದು, uq) = { \
                .type = STATIC_DESTRUCTOR_ARRAY,                        \
                .array.parray = (ಶೂನ್ಯ**) &(a),                          \
                .array.pn = &(n),                                       \
                .array.pfunc = (free_array_func_t) (func),              \
        };

/* Beginning and end of our section listing the destructors. We define these as weak as we want this to work
 * even if no destructors are defined and the section is missing. */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ StaticDestructor _weak_ __start_SYSTEMD_STATIC_DESTRUCT[];
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ StaticDestructor _weak_ __stop_SYSTEMD_STATIC_DESTRUCT[];

ಶೂನ್ಯ static_destruct_impl(ಸ್ಥಿರಾಂಕ StaticDestructor *ಆರಂಭ, ಸ್ಥಿರಾಂಕ StaticDestructor *ಕೊನೆ);

/* The function to destroy everything. (Note that this must be static inline, as it's key that it remains in
 * the same linking unit as the variables we want to destroy.) */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ static_destruct(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ static_destruct_impl(__start_SYSTEMD_STATIC_DESTRUCT, __stop_SYSTEMD_STATIC_DESTRUCT);
}
