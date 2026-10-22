/* SPDX-License-Identifier: LGPL-2.1-or-later */

#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం శూన్య (*free_func_t)(శూన్య *p);

/* A framework for registering static variables that shall be freed on shutdown of a process. It's a bit like gcc's
 * destructor attribute, but allows us to precisely schedule when we want to free the variables. This is supposed to
 * feel a bit like the gcc cleanup attribute, but for static variables. Note that this does not work for static
 * variables declared in .so's, as the list is private to the same linking unit. But maybe that's a good thing. */

#నిర్వచించు _common_static_destruct_attrs_                                  \
        /* The actual destructor structure we place in a special section to find it. */ \
        _section_("SYSTEMD_STATIC_DESTRUCT")                            \
        /* Use pointer alignment, since that is apparently what gcc does for static variables. */ \
        _alignptr_                                                      \
        /* Make sure this is not dropped from the image despite not being explicitly referenced. */ \
        వాడిన_2                                                          \
        /* Prevent garbage collection by the linker. */                 \
        _retain_                                                        \
        /* Make sure that AddressSanitizer doesn't pad this variable: we want everything in this section
         * packed next to each other so that we can enumerate it. */    \
        variable_no_sanitize_చిరునామా

రకనిర్వచనం గణన StaticDestructorType {
        STATIC_DESTRUCTOR_SIMPLE,
        STATIC_DESTRUCTOR_ARRAY,
        _STATIC_DESTRUCTOR_TYPE_MAX,
        _STATIC_DESTRUCTOR_INVALID = -EINVAL,
} StaticDestructorType;

రకనిర్వచనం నిర్మాణం SimpleCleanup {
        శూన్య *దత్తాంశం;
        free_func_t destroy;
} SimpleCleanup;

/* Note: see the comment on struct Option in options.h for why _alignptr_ is required here. */
రకనిర్వచనం నిర్మాణం _alignptr_ StaticDestructor {
        StaticDestructorType type;
        సంఘం {
                SimpleCleanup simple;
                ArrayCleanup array;
        };
} StaticDestructor;

assert_cc(పరిమాణం(StaticDestructor) % పరిమాణం(శూన్య*) == 0);

#నిర్వచించు STATIC_DESTRUCTOR_రిజిస్టరు(variable, func) \
        STATIC_DESTRUCTOR_రిజిస్టరు_2(UNIQ, variable, func)

#నిర్వచించు STATIC_DESTRUCTOR_రిజిస్టరు_2(uq, variable, func)                 \
        /* Type-safe destructor */                                      \
        స్థిర శూన్య UNIQ_T(static_destructor_wrapper, uq)(శూన్య *p) {    \
                typeof(variable) *q = p;                                \
                func(q);                                                \
        }                                                               \
        _common_static_destruct_attrs_                                  \
        స్థిర స్థిరరాశి StaticDestructor UNIQ_T(static_destructor_నమోదు, uq) = { \
                .type = STATIC_DESTRUCTOR_SIMPLE,                       \
                .simple.దత్తాంశం = &(variable),                             \
                .simple.destroy = UNIQ_T(static_destructor_wrapper, uq), \
        }

#నిర్వచించు STATIC_ARRAY_DESTRUCTOR_రిజిస్టరు(a, n, func)            \
        STATIC_ARRAY_DESTRUCTOR_రిజిస్టరు_2(UNIQ, a, n, func)

#నిర్వచించు STATIC_ARRAY_DESTRUCTOR_రిజిస్టరు_2(uq, a, n, func)               \
        /* Type-safety check */                                         \
        వాడని స్థిర శూన్య (* UNIQ_T(static_destructor_wrapper, uq))(typeof(a[0]) *x, size_t y) = (func); \
        _common_static_destruct_attrs_                                  \
        స్థిర స్థిరరాశి StaticDestructor UNIQ_T(static_destructor_నమోదు, uq) = { \
                .type = STATIC_DESTRUCTOR_ARRAY,                        \
                .array.parray = (శూన్య**) &(a),                          \
                .array.pn = &(n),                                       \
                .array.pfunc = (free_array_func_t) (func),              \
        };

/* Beginning and end of our section listing the destructors. We define these as weak as we want this to work
 * even if no destructors are defined and the section is missing. */
బాహ్య స్థిరరాశి StaticDestructor _weak_ __start_SYSTEMD_STATIC_DESTRUCT[];
బాహ్య స్థిరరాశి StaticDestructor _weak_ __stop_SYSTEMD_STATIC_DESTRUCT[];

శూన్య static_destruct_impl(స్థిరరాశి StaticDestructor *ప్రారంభం, స్థిరరాశి StaticDestructor *చివర);

/* The function to destroy everything. (Note that this must be static inline, as it's key that it remains in
 * the same linking unit as the variables we want to destroy.) */
స్థిర అంతఃవరుస శూన్య static_destruct(శూన్య) {
        తిరిగి static_destruct_impl(__start_SYSTEMD_STATIC_DESTRUCT, __stop_SYSTEMD_STATIC_DESTRUCT);
}
