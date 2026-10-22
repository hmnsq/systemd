/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "../fundamental/assert-util.h"
#ಅನ್ತರ್ಭಾವ "../fundamental/cleanup-util.h" /* IWYU pragma: export */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*free_func_t)(ಶೂನ್ಯ *p);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ* (*mfree_func_t)(ಶೂನ್ಯ *p);

#ವ್ಯಾಖ್ಯಾನಿಸು free_and_replace_ತುಂಬಿದ(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = ಖಾಲಿ;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#ವ್ಯಾಖ್ಯಾನಿಸು unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                ಆದರೆ (!p)                                                                 \
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                ಚಿಹ್ನೆರಹಿತ *q = &p->n_ref;                                                \
                ಪ್ರತಿಜ್ಞಾ(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                ಹಿಂತಿರುಗಿ p;                                                               \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                ಆದರೆ (!p)                                                                 \
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;                                                    \
                                                                                        \
                ಪ್ರತಿಜ್ಞಾ(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                ಆದರೆ (p->n_ref > 0)                                                       \
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;                                                    \
                                                                                        \
                ಹಿಂತಿರುಗಿ free_func(p);                                                    \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಬಹಿರಂಗ_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ಬಹಿರಂಗ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ಸ್ಥಿರ)
#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಬಹಿರಂಗ_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ಬಹಿರಂಗ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#ವ್ಯಾಖ್ಯಾನಿಸು DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#ವ್ಯಾಖ್ಯಾನಿಸು DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#ವ್ಯಾಖ್ಯಾನಿಸು DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ಖಾಸಗಿ_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ಖಾಸಗಿ_TRIVIAL_UNREF_FUNC(type, name, free_func);

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಬಹಿರಂಗ_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ಬಹಿರಂಗ_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ಬಹಿರಂಗ_TRIVIAL_UNREF_FUNC(type, name, free_func);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*void_func_t)(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dispatch_void_func(void_func_t *f) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#ವ್ಯಾಖ್ಯಾನಿಸು _DEFER_VOID_CALL(uniq, x) ಬಳಸದ _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
