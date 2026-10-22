/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "forward.h"
#अन्तर्भाव "memory-util.h"        /* IWYU pragma: keep */

#यदि HAS_FEATURE_MEMORY_SANITIZER
#  include <sanitizer/msan_interface.h>
#endif

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#define ALLOCA_MAX (4U*1024U*1024U)

#define new(t, n) ((t*) malloc_multiply(n, परिमाण(t)))

#define new0(t, n) ((t*) सुस्मृति((n) ?: 1, परिमाण(t)))

#define alloca_safe(n)                                                  \
        ({                                                              \
                माप_प्रकार _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#define newa(t, n)                                                      \
        ({                                                              \
                माप_प्रकार _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, परिमाण(t)));            \
                (t*) alloca_safe(_n_);                                  \
        })

#define newa0(t, n)                                                     \
        ({                                                              \
                माप_प्रकार _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, परिमाण(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#define newdup(t, p, n) ((t*) memdup_multiply(p, n, परिमाण(t)))

#define newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, परिमाण(t)))

#define malloc0(n) (सुस्मृति(1, (n) ?: 1))

#define free_and_replace(a, b)                  \
        free_and_replace_full(a, b, मुक्त)

शून्य* memdup(अचर शून्य *p, माप_प्रकार l) _alloc_(2);
शून्य* memdup_suffix0(अचर शून्य *p, माप_प्रकार l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#define memdupa(p, l)                           \
        ({                                      \
                शून्य *_q_;                      \
                माप_प्रकार _l_ = l;                 \
                _q_ = alloca_safe(_l_);         \
                memcpy_safe(_q_, p, _l_);       \
        })

#define memdupa_suffix0(p, l)                   \
        ({                                      \
                शून्य *_q_;                      \
                माप_प्रकार _l_ = l;                 \
                _q_ = alloca_safe(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_safe(_q_, p, _l_);       \
        })

स्थिर अन्तःस्थ शून्य unsetp(शून्य *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(शून्य**)p = रिक्त;
}

स्थिर अन्तःस्थ शून्य freep(शून्य *p) {
        *(शून्य**)p = mfree(*(शून्य**) p);
}

#define _cleanup_free_ _cleanup_(freep)

स्थिर अन्तःस्थ bool size_multiply_overflow(माप_प्रकार size, माप_प्रकार need) {
        फर्काऊ _unlikely_(need != 0 && size > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) स्थिर अन्तःस्थ शून्य *malloc_multiply(माप_प्रकार need, माप_प्रकार size) {
        यदि (size_multiply_overflow(size, need))
                फर्काऊ रिक्त;

        फर्काऊ दो_स्मृति(size * need ?: 1);
}

_alloc_(2, 3) स्थिर अन्तःस्थ शून्य *memdup_multiply(अचर शून्य *p, माप_प्रकार need, माप_प्रकार size) {
        यदि (size_multiply_overflow(size, need))
                फर्काऊ रिक्त;

        फर्काऊ memdup(p, size * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
स्थिर अन्तःस्थ शून्य *memdup_suffix0_multiply(अचर शून्य *p, माप_प्रकार need, माप_प्रकार size) {
        यदि (size_multiply_overflow(size, need))
                फर्काऊ रिक्त;

        फर्काऊ memdup_suffix0(p, size * need);
}

माप_प्रकार malloc_sizeof_safe(शून्य **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#define MALLOC_SIZEOF_SAFE(x) \
        malloc_sizeof_safe((शून्य**) &__builtin_choose_expr(__builtin_constant_p(x), (शून्य*) { रिक्त }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#define MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_SAFE(x)/परिमाण((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
शून्य *expand_to_usable(शून्य *p, माप_प्रकार newsize) _alloc_(2) _returns_nonnull_ _noinline_;

शून्य* realloc0(शून्य *p, माप_प्रकार new_size) _alloc_(2);

स्थिर अन्तःस्थ माप_प्रकार GREEDY_ALLOC_ROUND_UP(माप_प्रकार l) {
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

        यदि (l <= 2)
                फर्काऊ 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        यदि (m == 0) /* overflow? */
                फर्काऊ l;

        फर्काऊ m;
}

शून्य* greedy_realloc(शून्य **p, माप_प्रकार need, माप_प्रकार size);
शून्य* greedy_realloc0(शून्य **p, माप_प्रकार need, माप_प्रकार size);
शून्य* greedy_realloc_append(शून्य **p, माप_प्रकार *n_p, अचर शून्य *from, माप_प्रकार n_from, माप_प्रकार size);

#define GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((शून्य**) &(array), (need), परिमाण((array)[0]))

#define GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((शून्य**) &(array), (need), परिमाण((array)[0]))

#define GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                अचर typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((शून्य**) &(array), &(n_array), _from_, (n_from), परिमाण((array)[0])); \
        })

#define alloca0(n)                                      \
        ({                                              \
                वर्ण *_new_;                            \
                माप_प्रकार _len_ = n;                       \
                _new_ = alloca_safe(_len_);             \
                स_रखो(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#define alloca_align(size, align)                                       \
        ({                                                              \
                शून्य *_ptr_;                                            \
                माप_प्रकार _mask_ = (align) - 1;                            \
                माप_प्रकार _size_ = size;                                   \
                _ptr_ = alloca_safe(_size_ + _mask_);                   \
                (शून्य*)(((uintptr_t)_ptr_ + _mask_) & ~_mask_);         \
        })

#define alloca0_align(size, align)                                      \
        ({                                                              \
                शून्य *_new_;                                            \
                माप_प्रकार _xsize_ = (size);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                स_रखो(_new_, 0, _xsize_);                              \
        })

#यदि HAS_FEATURE_MEMORY_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#अन्यथा
#  define msan_unpoison(r, s)
#endif

/* Free every element of the array. */
स्थिर अन्तःस्थ शून्य free_many(शून्य **p, माप_प्रकार n) {
        प्रतिज्ञा(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
स्थिर अन्तःस्थ शून्य free_many_charp(वर्ण **c, माप_प्रकार n) {
        free_many((शून्य**) c, n);
}
