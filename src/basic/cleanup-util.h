/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "../fundamental/assert-util.h"
#ضمن "../fundamental/cleanup-util.h" /* IWYU pragma: export */

تعريفنوع فراغ (*free_func_t)(فراغ *p);
تعريفنوع فراغ* (*mfree_func_t)(فراغ *p);

#تعریف_کړه free_and_replace_ډک(a, b, free_func)  \
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
#تعریف_کړه unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#تعریف_کړه _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                إذا (!p)                                                                 \
                        أرجع عدم;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                بینښه *q = &p->n_ref;                                                \
                أكد(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                أرجع p;                                                               \
        }

#تعریف_کړه _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                إذا (!p)                                                                 \
                        أرجع عدم;                                                    \
                                                                                        \
                أكد(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                إذا (p->n_ref > 0)                                                       \
                        أرجع عدم;                                                    \
                                                                                        \
                أرجع free_func(p);                                                    \
        }

#تعریف_کړه DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#تعریف_کړه DEFINE_شخصي_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ساكن)
#تعریف_کړه DEFINE_عامه_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, عامه)

#تعریف_کړه DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#تعریف_کړه DEFINE_شخصي_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ساكن)
#تعریف_کړه DEFINE_عامه_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, عامه)

#تعریف_کړه DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#تعریف_کړه DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#تعریف_کړه DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#تعریف_کړه DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#تعریف_کړه DEFINE_شخصي_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_شخصي_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_شخصي_TRIVIAL_UNREF_FUNC(type, name, free_func);

#تعریف_کړه DEFINE_عامه_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_عامه_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_عامه_TRIVIAL_UNREF_FUNC(type, name, free_func);

تعريفنوع فراغ (*void_func_t)(فراغ);

ساكن مضمن فراغ dispatch_void_func(void_func_t *f) {
        أكد(f);
        أكد(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#تعریف_کړه DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#تعریف_کړه _DEFER_VOID_CALL(uniq, x) ناکارول_شوی _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
