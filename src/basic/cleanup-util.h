/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "../fundamental/assert-util.h"
#समावेश "../fundamental/cleanup-util.h" /* IWYU pragma: export */

प्रकार व्योम (*free_func_t)(व्योम *p);
प्रकार व्योम* (*mfree_func_t)(व्योम *p);

#घोषणा free_and_replace_full(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = शून्य;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#घोषणा unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#घोषणा _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                अगर (!p)                                                                 \
                        वापस शून्य;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                अचिन्हित *q = &p->n_ref;                                                \
                निश्चित(*q > 0);                                                         \
                assert_se(*q < अच_पूर्णांक_उच्च);                                               \
                                                                                        \
                (*q)++;                                                                 \
                वापस p;                                                               \
        }

#घोषणा _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                अगर (!p)                                                                 \
                        वापस शून्य;                                                    \
                                                                                        \
                निश्चित(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                अगर (p->n_ref > 0)                                                       \
                        वापस शून्य;                                                    \
                                                                                        \
                वापस free_func(p);                                                    \
        }

#घोषणा DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#घोषणा DEFINE_PRIVATE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, अटल)
#घोषणा DEFINE_PUBLIC_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, _public_)

#घोषणा DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#घोषणा DEFINE_PRIVATE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, अटल)
#घोषणा DEFINE_PUBLIC_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, _public_)

#घोषणा DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#घोषणा DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#घोषणा DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#घोषणा DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#घोषणा DEFINE_PRIVATE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_PRIVATE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_PRIVATE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#घोषणा DEFINE_PUBLIC_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_PUBLIC_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_PUBLIC_TRIVIAL_UNREF_FUNC(type, name, free_func);

प्रकार व्योम (*void_func_t)(व्योम);

अटल अंतरभूत व्योम dispatch_void_func(void_func_t *f) {
        निश्चित(f);
        निश्चित(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#घोषणा DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#घोषणा _DEFER_VOID_CALL(uniq, x) _unused_ _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
