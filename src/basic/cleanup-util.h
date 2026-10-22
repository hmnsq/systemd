/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "../fundamental/assert-util.h"
#ضمن "../fundamental/cleanup-util.h" /* IWYU pragma: export */

تعريفنوع فراغ (*free_func_t)(فراغ *p);
تعريفنوع فراغ* (*mfree_func_t)(فراغ *p);

#وصف_ڪريو free_and_replace_ڀريل(a, b, free_func)  \
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
#وصف_ڪريو unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#وصف_ڪريو _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                إذا (!p)                                                                 \
                        أرجع عدم;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                بنانشان *q = &p->n_ref;                                                \
                أكد(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                أرجع p;                                                               \
        }

#وصف_ڪريو _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
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

#وصف_ڪريو DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#وصف_ڪريو DEFINE_ذاتي_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ساكن)
#وصف_ڪريو DEFINE_عوامي_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, عوامي)

#وصف_ڪريو DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#وصف_ڪريو DEFINE_ذاتي_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ساكن)
#وصف_ڪريو DEFINE_عوامي_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, عوامي)

#وصف_ڪريو DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#وصف_ڪريو DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#وصف_ڪريو DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#وصف_ڪريو DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#وصف_ڪريو DEFINE_ذاتي_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ذاتي_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ذاتي_TRIVIAL_UNREF_FUNC(type, name, free_func);

#وصف_ڪريو DEFINE_عوامي_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_عوامي_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_عوامي_TRIVIAL_UNREF_FUNC(type, name, free_func);

تعريفنوع فراغ (*void_func_t)(فراغ);

ساكن مضمن فراغ dispatch_void_func(void_func_t *f) {
        أكد(f);
        أكد(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#وصف_ڪريو DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#وصف_ڪريو _DEFER_VOID_CALL(uniq, x) اڻ_استعمال_ٿيل _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
