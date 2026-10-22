/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "../fundamental/assert-util.h"
#అన్తర్భావ "../fundamental/cleanup-util.h" /* IWYU pragma: export */

రకనిర్వచనం శూన్య (*free_func_t)(శూన్య *p);
రకనిర్వచనం శూన్య* (*mfree_func_t)(శూన్య *p);

#నిర్వచించు free_and_replace_నిండిన(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = ఖాళీ;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#నిర్వచించు unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#నిర్వచించు _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                ఒకవేళ (!p)                                                                 \
                        తిరిగి ఖాళీ;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                గుర్తులేని *q = &p->n_ref;                                                \
                ప్రతిజ్ఞా(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                తిరిగి p;                                                               \
        }

#నిర్వచించు _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                ఒకవేళ (!p)                                                                 \
                        తిరిగి ఖాళీ;                                                    \
                                                                                        \
                ప్రతిజ్ఞా(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                ఒకవేళ (p->n_ref > 0)                                                       \
                        తిరిగి ఖాళీ;                                                    \
                                                                                        \
                తిరిగి free_func(p);                                                    \
        }

#నిర్వచించు DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#నిర్వచించు DEFINE_వ్యక్తిగత_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, స్థిర)
#నిర్వచించు DEFINE_ప్రజా_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ప్రజా)

#నిర్వచించు DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#నిర్వచించు DEFINE_వ్యక్తిగత_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, స్థిర)
#నిర్వచించు DEFINE_ప్రజా_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ప్రజా)

#నిర్వచించు DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#నిర్వచించు DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#నిర్వచించు DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#నిర్వచించు DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#నిర్వచించు DEFINE_వ్యక్తిగత_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_వ్యక్తిగత_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_వ్యక్తిగత_TRIVIAL_UNREF_FUNC(type, name, free_func);

#నిర్వచించు DEFINE_ప్రజా_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ప్రజా_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ప్రజా_TRIVIAL_UNREF_FUNC(type, name, free_func);

రకనిర్వచనం శూన్య (*void_func_t)(శూన్య);

స్థిర అంతఃవరుస శూన్య dispatch_void_func(void_func_t *f) {
        ప్రతిజ్ఞా(f);
        ప్రతిజ్ఞా(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#నిర్వచించు DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#నిర్వచించు _DEFER_VOID_CALL(uniq, x) వాడని _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
