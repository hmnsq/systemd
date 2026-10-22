/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/assert-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/cleanup-util.h" /* IWYU pragma: export */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*free_func_t)(ਖਾਲੀ *p);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ* (*mfree_func_t)(ਖਾਲੀ *p);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ free_and_replace_ਭਰਿਆ(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = ਨਿਰਾ;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                ਜੇ (!p)                                                                 \
                        ਵਾਪਸ ਨਿਰਾ;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                ਬਿਨਾਨਿਸ਼ਾਨੀ *q = &p->n_ref;                                                \
                ਪ੍ਰਤਿਜ੍ਞਾ(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                ਵਾਪਸ p;                                                               \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                ਜੇ (!p)                                                                 \
                        ਵਾਪਸ ਨਿਰਾ;                                                    \
                                                                                        \
                ਪ੍ਰਤਿਜ੍ਞਾ(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                ਜੇ (p->n_ref > 0)                                                       \
                        ਵਾਪਸ ਨਿਰਾ;                                                    \
                                                                                        \
                ਵਾਪਸ free_func(p);                                                    \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਜਨਤਕ_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, ਜਨਤਕ_2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ਸਥਿਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਜਨਤਕ_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, ਜਨਤਕ_2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ਨਿੱਜੀ_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ਨਿੱਜੀ_TRIVIAL_UNREF_FUNC(type, name, free_func);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਜਨਤਕ_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_ਜਨਤਕ_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_ਜਨਤਕ_TRIVIAL_UNREF_FUNC(type, name, free_func);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*void_func_t)(ਖਾਲੀ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dispatch_void_func(void_func_t *f) {
        ਪ੍ਰਤਿਜ੍ਞਾ(f);
        ਪ੍ਰਤਿਜ੍ਞਾ(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFER_VOID_CALL(uniq, x) ਅਣਵਰਤਿਆ _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
