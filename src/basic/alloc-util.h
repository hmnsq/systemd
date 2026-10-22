/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"        /* IWYU pragma: keep */

#ಆದರೆ HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER
#  include <sanitizer/msan_interface.h>
#ಆದರೆ_ಅಂತ್ಯ

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#ವ್ಯಾಖ್ಯಾನಿಸು ALLOCA_MAX (4U*1024U*1024U)

#ವ್ಯಾಖ್ಯಾನಿಸು new(t, n) ((t*) malloc_multiply(n, ಗಾತ್ರ(t)))

#ವ್ಯಾಖ್ಯಾನಿಸು new0(t, n) ((t*) ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು((n) ?: 1, ಗಾತ್ರ(t)))

#ವ್ಯಾಖ್ಯಾನಿಸು alloca_ಸುರಕ್ಷಿತ(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#ವ್ಯಾಖ್ಯಾನಿಸು newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&_n_, ಗಾತ್ರ(t)));            \
                (t*) alloca_ಸುರಕ್ಷಿತ(_n_);                                  \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&_n_, ಗಾತ್ರ(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು newdup(t, p, n) ((t*) memdup_multiply(p, n, ಗಾತ್ರ(t)))

#ವ್ಯಾಖ್ಯಾನಿಸು newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, ಗಾತ್ರ(t)))

#ವ್ಯಾಖ್ಯಾನಿಸು malloc0(n) (ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು(1, (n) ?: 1))

#ವ್ಯಾಖ್ಯಾನಿಸು free_and_replace(a, b)                  \
        free_and_replace_ತುಂಬಿದ(a, b, ಬಿಡುಗಡೆ)

ಶೂನ್ಯ* memdup(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l) _alloc_(2);
ಶೂನ್ಯ* memdup_suffix0(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#ವ್ಯಾಖ್ಯಾನಿಸು memdupa(p, l)                           \
        ({                                      \
                ಶೂನ್ಯ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_ಸುರಕ್ಷಿತ(_l_);         \
                memcpy_ಸುರಕ್ಷಿತ(_q_, p, _l_);       \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು memdupa_suffix0(p, l)                   \
        ({                                      \
                ಶೂನ್ಯ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_ಸುರಕ್ಷಿತ(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_ಸುರಕ್ಷಿತ(_q_, p, _l_);       \
        })

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ unsetp(ಶೂನ್ಯ *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(ಶೂನ್ಯ**)p = ಖಾಲಿ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ freep(ಶೂನ್ಯ *p) {
        *(ಶೂನ್ಯ**)p = mfree(*(ಶೂನ್ಯ**) p);
}

#ವ್ಯಾಖ್ಯಾನಿಸು _cleanup_free_ _cleanup_(freep)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(size_t ಗಾತ್ರ_2, size_t need) {
        ಹಿಂತಿರುಗಿ _unlikely_(need != 0 && ಗಾತ್ರ_2 > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *malloc_multiply(size_t need, size_t ಗಾತ್ರ_2) {
        ಆದರೆ (ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(ಗಾತ್ರ_2, need))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ_2 * need ?: 1);
}

_alloc_(2, 3) ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *memdup_multiply(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t need, size_t ಗಾತ್ರ_2) {
        ಆದರೆ (ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(ಗಾತ್ರ_2, need))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ memdup(p, ಗಾತ್ರ_2 * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ *memdup_suffix0_multiply(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t need, size_t ಗಾತ್ರ_2) {
        ಆದರೆ (ಗಾತ್ರ_multiply_ಅತಿಪ್ರವಾಹ(ಗಾತ್ರ_2, need))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ memdup_suffix0(p, ಗಾತ್ರ_2 * need);
}

size_t malloc_sizeof_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#ವ್ಯಾಖ್ಯಾನಿಸು MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(x) \
        malloc_sizeof_ಸುರಕ್ಷಿತ((ಶೂನ್ಯ**) &__builtin_choose_expr(__builtin_constant_p(x), (ಶೂನ್ಯ*) { ಖಾಲಿ }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#ವ್ಯಾಖ್ಯಾನಿಸು MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(x)/ಗಾತ್ರ((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
ಶೂನ್ಯ *expand_ಗಮ್ಯ_usable(ಶೂನ್ಯ *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

ಶೂನ್ಯ* realloc0(ಶೂನ್ಯ *p, size_t new_ಗಾತ್ರ) _alloc_(2);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(size_t l) {
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

        ಆದರೆ (l <= 2)
                ಹಿಂತಿರುಗಿ 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        ಆದರೆ (m == 0) /* overflow? */
                ಹಿಂತಿರುಗಿ l;

        ಹಿಂತಿರುಗಿ m;
}

ಶೂನ್ಯ* greedy_realloc(ಶೂನ್ಯ **p, size_t need, size_t ಗಾತ್ರ_2);
ಶೂನ್ಯ* greedy_realloc0(ಶೂನ್ಯ **p, size_t need, size_t ಗಾತ್ರ_2);
ಶೂನ್ಯ* greedy_realloc_append(ಶೂನ್ಯ **p, size_t *n_p, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *from, size_t n_from, size_t ಗಾತ್ರ_2);

#ವ್ಯಾಖ್ಯಾನಿಸು GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((ಶೂನ್ಯ**) &(array), (need), ಗಾತ್ರ((array)[0]))

#ವ್ಯಾಖ್ಯಾನಿಸು GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((ಶೂನ್ಯ**) &(array), (need), ಗಾತ್ರ((array)[0]))

#ವ್ಯಾಖ್ಯಾನಿಸು GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((ಶೂನ್ಯ**) &(array), &(n_array), _from_, (n_from), ಗಾತ್ರ((array)[0])); \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು alloca0(n)                                      \
        ({                                              \
                ಅಕ್ಷರ *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_ಸುರಕ್ಷಿತ(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#ವ್ಯಾಖ್ಯಾನಿಸು alloca_align(ಗಾತ್ರ_2, align)                                       \
        ({                                                              \
                ಶೂನ್ಯ *_ptr_;                                            \
                size_t ಮುಖವಾಡ = (align) - 1;                            \
                size_t ಗಾತ್ರ_3 = ಗಾತ್ರ_2;                                   \
                _ptr_ = alloca_ಸುರಕ್ಷಿತ(ಗಾತ್ರ_3 + ಮುಖವಾಡ);                   \
                (ಶೂನ್ಯ*)(((uintptr_t)_ptr_ + ಮುಖವಾಡ) & ~ಮುಖವಾಡ);         \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು alloca0_align(ಗಾತ್ರ_2, align)                                      \
        ({                                                              \
                ಶೂನ್ಯ *_new_;                                            \
                size_t _xsize_ = (ಗಾತ್ರ_2);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#ಆದರೆ HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#ಇಲ್ಲದಿದ್ದರೆ
#  define msan_unpoison(r, s)
#ಆದರೆ_ಅಂತ್ಯ

/* Free every element of the array. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ free_many(ಶೂನ್ಯ **p, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ free_many_charp(ಅಕ್ಷರ **c, size_t n) {
        free_many((ಶೂನ್ಯ**) c, n);
}
