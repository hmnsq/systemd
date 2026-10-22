/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <stdlib.h>

#समावेश "forward.h"
#समावेश "memory-util.h"        /* IWYU pragma: keep */

#अगर HAS_FEATURE_MEMORY_SANITIZER
#  include <sanitizer/msan_interface.h>
#पूर्ण_अगर

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#घोषणा ALLOCA_MAX (4U*1024U*1024U)

#घोषणा new(t, n) ((t*) malloc_multiply(n, माप(t)))

#घोषणा new0(t, n) ((t*) सुस्मृति((n) ?: 1, माप(t)))

#घोषणा alloca_safe(n)                                                  \
        ({                                                              \
                माप_प्रकार _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#घोषणा newa(t, n)                                                      \
        ({                                                              \
                माप_प्रकार _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, माप(t)));            \
                (t*) alloca_safe(_n_);                                  \
        })

#घोषणा newa0(t, n)                                                     \
        ({                                                              \
                माप_प्रकार _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, माप(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#घोषणा newdup(t, p, n) ((t*) memdup_multiply(p, n, माप(t)))

#घोषणा newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, माप(t)))

#घोषणा malloc0(n) (सुस्मृति(1, (n) ?: 1))

#घोषणा free_and_replace(a, b)                  \
        free_and_replace_full(a, b, मुक्त)

व्योम* memdup(स्थिर व्योम *p, माप_प्रकार l) _alloc_(2);
व्योम* memdup_suffix0(स्थिर व्योम *p, माप_प्रकार l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#घोषणा memdupa(p, l)                           \
        ({                                      \
                व्योम *_q_;                      \
                माप_प्रकार _l_ = l;                 \
                _q_ = alloca_safe(_l_);         \
                memcpy_safe(_q_, p, _l_);       \
        })

#घोषणा memdupa_suffix0(p, l)                   \
        ({                                      \
                व्योम *_q_;                      \
                माप_प्रकार _l_ = l;                 \
                _q_ = alloca_safe(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_safe(_q_, p, _l_);       \
        })

अटल अंतरभूत व्योम unsetp(व्योम *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(व्योम**)p = शून्य;
}

अटल अंतरभूत व्योम freep(व्योम *p) {
        *(व्योम**)p = mfree(*(व्योम**) p);
}

#घोषणा _cleanup_free_ _cleanup_(freep)

अटल अंतरभूत bool आकार_multiply_overflow(माप_प्रकार आकार, माप_प्रकार need) {
        वापस _unlikely_(need != 0 && आकार > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) अटल अंतरभूत व्योम *malloc_multiply(माप_प्रकार need, माप_प्रकार आकार) {
        अगर (आकार_multiply_overflow(आकार, need))
                वापस शून्य;

        वापस दो_स्मृति(आकार * need ?: 1);
}

_alloc_(2, 3) अटल अंतरभूत व्योम *memdup_multiply(स्थिर व्योम *p, माप_प्रकार need, माप_प्रकार आकार) {
        अगर (आकार_multiply_overflow(आकार, need))
                वापस शून्य;

        वापस memdup(p, आकार * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
अटल अंतरभूत व्योम *memdup_suffix0_multiply(स्थिर व्योम *p, माप_प्रकार need, माप_प्रकार आकार) {
        अगर (आकार_multiply_overflow(आकार, need))
                वापस शून्य;

        वापस memdup_suffix0(p, आकार * need);
}

माप_प्रकार malloc_sizeof_safe(व्योम **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#घोषणा MALLOC_SIZEOF_SAFE(x) \
        malloc_sizeof_safe((व्योम**) &__builtin_choose_expr(__builtin_constant_p(x), (व्योम*) { शून्य }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#घोषणा MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_SAFE(x)/माप((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
व्योम *expand_to_usable(व्योम *p, माप_प्रकार newsize) _alloc_(2) _returns_nonnull_ _noinline_;

व्योम* realloc0(व्योम *p, माप_प्रकार new_आकार) _alloc_(2);

अटल अंतरभूत माप_प्रकार GREEDY_ALLOC_ROUND_UP(माप_प्रकार l) {
        माप_प्रकार m;

        /* Round up allocation sizes a bit to some reasonable, likely larger value. This is supposed to be
         * used for cases which are likely called in an allocation loop of some form, i.e. that repetitively
         * grow stuff, for example strv_extend() and suchlike.
         *
         * Note the difference to GREEDY_REALLOC() here, as this helper operates on a single size value only,
         * and rounds up to next multiple of 2, needing no further counter.
         *
         * Note the benefits of direct ALIGN_POWER2() usage: type-safety for size_t, sane handling for very
         * small (i.e. <= 2) and safe handling for very large (i.e. > SSIZE_MAX) values. */

        अगर (l <= 2)
                वापस 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        अगर (m == 0) /* overflow? */
                वापस l;

        वापस m;
}

व्योम* greedy_realloc(व्योम **p, माप_प्रकार need, माप_प्रकार आकार);
व्योम* greedy_realloc0(व्योम **p, माप_प्रकार need, माप_प्रकार आकार);
व्योम* greedy_realloc_append(व्योम **p, माप_प्रकार *n_p, स्थिर व्योम *from, माप_प्रकार n_from, माप_प्रकार आकार);

#घोषणा GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((व्योम**) &(array), (need), माप((array)[0]))

#घोषणा GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((व्योम**) &(array), (need), माप((array)[0]))

#घोषणा GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                स्थिर typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((व्योम**) &(array), &(n_array), _from_, (n_from), माप((array)[0])); \
        })

#घोषणा alloca0(n)                                      \
        ({                                              \
                अक्षर *_new_;                            \
                माप_प्रकार _len_ = n;                       \
                _new_ = alloca_safe(_len_);             \
                स_रखो(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#घोषणा alloca_align(आकार, align)                                       \
        ({                                                              \
                व्योम *_ptr_;                                            \
                माप_प्रकार _mask_ = (align) - 1;                            \
                माप_प्रकार आकार_2 = आकार;                                   \
                _ptr_ = alloca_safe(आकार_2 + _mask_);                   \
                (व्योम*)(((uintptr_t)_ptr_ + _mask_) & ~_mask_);         \
        })

#घोषणा alloca0_align(आकार, align)                                      \
        ({                                                              \
                व्योम *_new_;                                            \
                माप_प्रकार _xsize_ = (आकार);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                स_रखो(_new_, 0, _xsize_);                              \
        })

#अगर HAS_FEATURE_MEMORY_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#अन्यथा
#  define msan_unpoison(r, s)
#पूर्ण_अगर

/* Free every element of the array. */
अटल अंतरभूत व्योम free_many(व्योम **p, माप_प्रकार n) {
        निश्चित(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
अटल अंतरभूत व्योम free_many_charp(अक्षर **c, माप_प्रकार n) {
        free_many((व्योम**) c, n);
}
