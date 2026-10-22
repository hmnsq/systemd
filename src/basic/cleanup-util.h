/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "../fundamental/assert-util.h"
#अन्तर्भाव "../fundamental/cleanup-util.h" /* IWYU pragma: export */

प्रकारनिर्देश शून्य (*free_func_t)(शून्य *p);
प्रकारनिर्देश शून्य* (*mfree_func_t)(शून्य *p);

#परिभाषय free_and_replace_पूर्णम्(a, b, free_func)  \
        ({                                      \
                typeof(a)* _a = &(a);           \
                typeof(b)* _b = &(b);           \
                free_func(*_a);                 \
                *_a = *_b;                      \
                *_b = रिक्त;                     \
                0;                              \
        })

/* This is similar to free_and_replace_full(), but NULL is not assigned to 'b', and its reference counter is
 * increased. */
#परिभाषय unref_and_replace_new_ref(a, b, ref_func, unref_func) \
        ({                                                    \
                typeof(a)* _a = &(a);                         \
                typeof(b) _b = ref_func(b);                   \
                unref_func(*_a);                              \
                *_a = _b;                                     \
                0;                                            \
        })

#परिभाषय _DEFINE_TRIVIAL_REF_FUNC(type, name, scope)                                     \
        scope type *name##_ref(type *p) {                                               \
                यदि (!p)                                                                 \
                        प्रत्यागम रिक्त;                                                    \
                                                                                        \
                /* For type check. */                                                   \
                अचिह्नित *q = &p->n_ref;                                                \
                प्रतिज्ञा(*q > 0);                                                         \
                assert_se(*q < UINT_MAX);                                               \
                                                                                        \
                (*q)++;                                                                 \
                प्रत्यागम p;                                                               \
        }

#परिभाषय _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, scope)                        \
        scope type *name##_unref(type *p) {                                             \
                यदि (!p)                                                                 \
                        प्रत्यागम रिक्त;                                                    \
                                                                                        \
                प्रतिज्ञा(p->n_ref > 0);                                                   \
                p->n_ref--;                                                             \
                यदि (p->n_ref > 0)                                                       \
                        प्रत्यागम रिक्त;                                                    \
                                                                                        \
                प्रत्यागम free_func(p);                                                    \
        }

#परिभाषय DEFINE_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name,)
#परिभाषय DEFINE_वैयक्तिकम्_TRIVIAL_REF_FUNC(type, name)     \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, स्थिर)
#परिभाषय DEFINE_लौकिकम्_TRIVIAL_REF_FUNC(type, name)      \
        _DEFINE_TRIVIAL_REF_FUNC(type, name, लौकिकम्)

#परिभाषय DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func,)
#परिभाषय DEFINE_वैयक्तिकम्_TRIVIAL_UNREF_FUNC(type, name, free_func)        \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, स्थिर)
#परिभाषय DEFINE_लौकिकम्_TRIVIAL_UNREF_FUNC(type, name, free_func)         \
        _DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func, लौकिकम्)

#परिभाषय DEFINE_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_TRIVIAL_UNREF_FUNC(type, name, free_func);

#परिभाषय DECLARE_TRIVIAL_REF_FUNC(type, name) \
        type* name##_ref(type *p)

#परिभाषय DECLARE_TRIVIAL_UNREF_FUNC(type, name) \
        type* name##_unref(type *p)

#परिभाषय DECLARE_TRIVIAL_REF_UNREF_FUNC(type, name)      \
        DECLARE_TRIVIAL_REF_FUNC(type, name);           \
        DECLARE_TRIVIAL_UNREF_FUNC(type, name)

#परिभाषय DEFINE_वैयक्तिकम्_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_वैयक्तिकम्_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_वैयक्तिकम्_TRIVIAL_UNREF_FUNC(type, name, free_func);

#परिभाषय DEFINE_लौकिकम्_TRIVIAL_REF_UNREF_FUNC(type, name, free_func)    \
        DEFINE_लौकिकम्_TRIVIAL_REF_FUNC(type, name);                    \
        DEFINE_लौकिकम्_TRIVIAL_UNREF_FUNC(type, name, free_func);

प्रकारनिर्देश शून्य (*void_func_t)(शून्य);

स्थिर अन्तःस्थ शून्य dispatch_void_func(void_func_t *f) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(*f);
        (*f)();
}

/* Inspired by Go's "defer" construct, but much more basic. This basically just calls a void function when
 * the current scope is left. Doesn't do function parameters (i.e. no closures). */
#परिभाषय DEFER_VOID_CALL(x) _DEFER_VOID_CALL(UNIQ, x)
#परिभाषय _DEFER_VOID_CALL(uniq, x) अप्रयुक्तम् _cleanup_(dispatch_void_func) void_func_t UNIQ_T(defer, uniq) = (x)
