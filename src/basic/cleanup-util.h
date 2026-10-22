/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "../fundamental/assert-util.h"
#ಅನ್ತರ್ಭಾವ "../fundamental/cleanup-util.h" /* IWYU pragma: export */

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*free_func_t)(ಶೂನ್ಯ *p);
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ* (*mfree_func_t)(ಶೂನ್ಯ *p);

#define free_and_replace_full(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = ರಿಕ್ತ;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#define unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#define _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                ಯದಿ (!p)                                                                 \
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                unsigned *q = &p->n_ref;                                                \
                ಪ್ರತಿಜ್ಞಾ(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                ಪ್ರತ್ಯಾಗಮ p;                                                               \
        }

#define _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                ಯದಿ (!p)                                                                 \
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;                                                    \
                                                                                        \
                ಪ್ರತಿಜ್ಞಾ(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                ಯದಿ (p->n_ref > 0)                                                       \
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;                                                    \
                                                                                        \
                ಪ್ರತ್ಯಾಗಮ free_func(p);                                                    \
        }

#define DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#define DEFINE_PRIVATE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ಸ್ಥಿರ)
#define DEFINE_PUBLIC_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, _public_)

#define DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#define DEFINE_PRIVATE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ಸ್ಥಿರ)
#define DEFINE_PUBLIC_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, _public_)

#define DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#define DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#define DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#define DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#define DEFINE_PRIVATE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_PRIVATE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_PRIVATE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#define DEFINE_PUBLIC_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_PUBLIC_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_PUBLIC_TRIVIAL_UNREF_FUNC(type, name, free_func);

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*void_func_t)(ಶೂನ್ಯ);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ dispatch_void_func(void_func_t *f) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#define DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#define _DEFER_VOID_CALL(uniq, x) _unused_ _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
