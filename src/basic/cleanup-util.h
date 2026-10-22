/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "../fundamental/assert-util.h"
#ضمن "../fundamental/cleanup-util.h" /* IWYU pragma: export */

تعريفنوع فراغ (*free_func_t)(فراغ *p);
تعريفنوع فراغ* (*mfree_func_t)(فراغ *p);

#define free_and_replace_full(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = عدم;                     \
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
                ܐܢ (!p)                                                                 \
                        ܦܢܐ عدم;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                unsigned *q = &p->n_ref;                                                \
                أكد(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                ܦܢܐ p;                                                               \
        }

#define _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                ܐܢ (!p)                                                                 \
                        ܦܢܐ عدم;                                                    \
                                                                                        \
                أكد(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                ܐܢ (p->n_ref > 0)                                                       \
                        ܦܢܐ عدم;                                                    \
                                                                                        \
                ܦܢܐ free_func(p);                                                    \
        }

#define DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#define DEFINE_PRIVATE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ساكن)
#define DEFINE_PUBLIC_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, _public_)

#define DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#define DEFINE_PRIVATE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ساكن)
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

تعريفنوع فراغ (*void_func_t)(فراغ);

ساكن مضمن فراغ dispatch_void_func(void_func_t *f) {
        أكد(f);
        أكد(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#define DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#define _DEFER_VOID_CALL(uniq, x) _unused_ _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
