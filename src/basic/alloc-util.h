/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"        /* IWYU pragma: keep */

#ಯದಿ HAS_FEATURE_MEMORY_SANITIZER
#  include <sanitizer/msan_interface.h>
#endif

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#define ALLOCA_MAX (4U*1024U*1024U)

#define new(t, n) ((t*) malloc_multiply(n, ಪರಿಮಾಣ(t)))

#define new0(t, n) ((t*) calloc((n) ?: 1, ಪರಿಮಾಣ(t)))

#define alloca_safe(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#define newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, ಪರಿಮಾಣ(t)));            \
                (t*) alloca_safe(_n_);                                  \
        })

#define newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, ಪರಿಮಾಣ(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#define newdup(t, p, n) ((t*) memdup_multiply(p, n, ಪರಿಮಾಣ(t)))

#define newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, ಪರಿಮಾಣ(t)))

#define malloc0(n) (calloc(1, (n) ?: 1))

#define free_and_replace(a, b)                  \
        free_and_replace_full(a, b, ಮುಕ್ತ)

ಶೂನ್ಯ* memdup(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l) _alloc_(2);
ಶೂನ್ಯ* memdup_suffix0(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#define memdupa(p, l)                           \
        ({                                      \
                ಶೂನ್ಯ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_safe(_l_);         \
                memcpy_safe(_q_, p, _l_);       \
        })

#define memdupa_suffix0(p, l)                   \
        ({                                      \
                ಶೂನ್ಯ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_safe(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_safe(_q_, p, _l_);       \
        })

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ unsetp(ಶೂನ್ಯ *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(ಶೂನ್ಯ**)p = ರಿಕ್ತ;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ freep(ಶೂನ್ಯ *p) {
        *(ಶೂನ್ಯ**)p = mfree(*(ಶೂನ್ಯ**) p);
}

#define _cleanup_free_ _cleanup_(freep)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool size_multiply_overflow(size_t size, size_t need) {
        ಪ್ರತ್ಯಾಗಮ _unlikely_(need != 0 && size > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *malloc_multiply(size_t need, size_t size) {
        ಯದಿ (size_multiply_overflow(size, need))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ malloc(size * need ?: 1);
}

_alloc_(2, 3) ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *memdup_multiply(ಧ್ರುವ ಶೂನ್ಯ *p, size_t need, size_t size) {
        ಯದಿ (size_multiply_overflow(size, need))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ memdup(p, size * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ *memdup_suffix0_multiply(ಧ್ರುವ ಶೂನ್ಯ *p, size_t need, size_t size) {
        ಯದಿ (size_multiply_overflow(size, need))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ memdup_suffix0(p, size * need);
}

size_t malloc_sizeof_safe(ಶೂನ್ಯ **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#define MALLOC_SIZEOF_SAFE(x) \
        malloc_sizeof_safe((ಶೂನ್ಯ**) &__builtin_choose_expr(__builtin_constant_p(x), (ಶೂನ್ಯ*) { ರಿಕ್ತ }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#define MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_SAFE(x)/ಪರಿಮಾಣ((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
ಶೂನ್ಯ *expand_to_usable(ಶೂನ್ಯ *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

ಶೂನ್ಯ* realloc0(ಶೂನ್ಯ *p, size_t new_size) _alloc_(2);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t GREEDY_ALLOC_ROUND_UP(size_t l) {
        size_t m;

        /* Round up allocation sizes a bit to some reasonable, likely larger value. This is supposed to be
         * used for cases which are likely called in an allocation loop of some form, i.e. that repetitively
         * grow stuff, for example strv_extend() and suchlike.
         *
         * Note the difference to GREEDY_REALLOC() here, as this helper operates on a single size value only,
         * and rounds up to next multiple of 2, needing no further counter.
         *
         * Note the benefits of direct ALIGN_POWER2() usage: type-safety for size_t, sane handling for very
         * small (i.e. <= 2) and safe handling for very large (i.e. > SSIZE_MAX) values. */

        ಯದಿ (l <= 2)
                ಪ್ರತ್ಯಾಗಮ 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        ಯದಿ (m == 0) /* overflow? */
                ಪ್ರತ್ಯಾಗಮ l;

        ಪ್ರತ್ಯಾಗಮ m;
}

ಶೂನ್ಯ* greedy_realloc(ಶೂನ್ಯ **p, size_t need, size_t size);
ಶೂನ್ಯ* greedy_realloc0(ಶೂನ್ಯ **p, size_t need, size_t size);
ಶೂನ್ಯ* greedy_realloc_append(ಶೂನ್ಯ **p, size_t *n_p, ಧ್ರುವ ಶೂನ್ಯ *from, size_t n_from, size_t size);

#define GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((ಶೂನ್ಯ**) &(array), (need), ಪರಿಮಾಣ((array)[0]))

#define GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((ಶೂನ್ಯ**) &(array), (need), ಪರಿಮಾಣ((array)[0]))

#define GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                ಧ್ರುವ typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((ಶೂನ್ಯ**) &(array), &(n_array), _from_, (n_from), ಪರಿಮಾಣ((array)[0])); \
        })

#define alloca0(n)                                      \
        ({                                              \
                ವರ್ಣ *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_safe(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#define alloca_align(size, align)                                       \
        ({                                                              \
                ಶೂನ್ಯ *_ptr_;                                            \
                size_t _mask_ = (align) - 1;                            \
                size_t _size_ = size;                                   \
                _ptr_ = alloca_safe(_size_ + _mask_);                   \
                (ಶೂನ್ಯ*)(((uintptr_t)_ptr_ + _mask_) & ~_mask_);         \
        })

#define alloca0_align(size, align)                                      \
        ({                                                              \
                ಶೂನ್ಯ *_new_;                                            \
                size_t _xsize_ = (size);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#ಯದಿ HAS_FEATURE_MEMORY_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#ಅನ್ಯಥಾ
#  define msan_unpoison(r, s)
#endif

/* Free every element of the array. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ free_many(ಶೂನ್ಯ **p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ free_many_charp(ವರ್ಣ **c, size_t n) {
        free_many((ಶೂನ್ಯ**) c, n);
}
