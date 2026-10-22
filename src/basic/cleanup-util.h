/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "../fundamental/assert-util.h"
#شامل "../fundamental/cleanup-util.h" /* IWYU pragma: export */

قسمتعریف خالی (*free_func_t)(خالی *p);
قسمتعریف خالی* (*mfree_func_t)(خالی *p);

#تعریف_کرو free_and_replace_بھرا(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = تہی;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#تعریف_کرو unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#تعریف_کرو _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                اگر (!p)                                                                 \
                        واپس تہی;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                غیرعلامتی *q = &p->n_ref;                                                \
                أكد(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                واپس p;                                                               \
        }

#تعریف_کرو _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                اگر (!p)                                                                 \
                        واپس تہی;                                                    \
                                                                                        \
                أكد(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                اگر (p->n_ref > 0)                                                       \
                        واپس تہی;                                                    \
                                                                                        \
                واپس free_func(p);                                                    \
        }

#تعریف_کرو DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#تعریف_کرو DEFINE_نجی_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ساکن)
#تعریف_کرو DEFINE_عوامی_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, عوامی_2)

#تعریف_کرو DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#تعریف_کرو DEFINE_نجی_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ساکن)
#تعریف_کرو DEFINE_عوامی_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, عوامی_2)

#تعریف_کرو DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#تعریف_کرو DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#تعریف_کرو DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#تعریف_کرو DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#تعریف_کرو DEFINE_نجی_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_نجی_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_نجی_TRIVIAL_UNREF_FUNC(type, name, free_func);

#تعریف_کرو DEFINE_عوامی_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_عوامی_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_عوامی_TRIVIAL_UNREF_FUNC(type, name, free_func);

قسمتعریف خالی (*void_func_t)(خالی);

ساکن اندرونی خالی dispatch_void_func(void_func_t *f) {
        أكد(f);
        أكد(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#تعریف_کرو DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#تعریف_کرو _DEFER_VOID_CALL(uniq, x) غیر_مستعمل _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
