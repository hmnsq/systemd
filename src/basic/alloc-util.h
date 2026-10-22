/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"        /* IWYU pragma: keep */

#ਜੇ HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER
#  include <sanitizer/msan_interface.h>
#ਜੇ_ਅੰਤ

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALLOCA_MAX (4U*1024U*1024U)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ new(t, n) ((t*) malloc_multiply(n, ਆਕਾਰ(t)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ new0(t, n) ((t*) ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ((n) ?: 1, ਆਕਾਰ(t)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ alloca_ਸੁਰੱਖਿਅਤ(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&_n_, ਆਕਾਰ(t)));            \
                (t*) alloca_ਸੁਰੱਖਿਅਤ(_n_);                                  \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&_n_, ਆਕਾਰ(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ newdup(t, p, n) ((t*) memdup_multiply(p, n, ਆਕਾਰ(t)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, ਆਕਾਰ(t)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ malloc0(n) (ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ(1, (n) ?: 1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ free_and_replace(a, b)                  \
        free_and_replace_ਭਰਿਆ(a, b, ਮੁਕਤ)

ਖਾਲੀ* memdup(ਅਚਲ ਖਾਲੀ *p, size_t l) _alloc_(2);
ਖਾਲੀ* memdup_suffix0(ਅਚਲ ਖਾਲੀ *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memdupa(p, l)                           \
        ({                                      \
                ਖਾਲੀ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_ਸੁਰੱਖਿਅਤ(_l_);         \
                memcpy_ਸੁਰੱਖਿਅਤ(_q_, p, _l_);       \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ memdupa_suffix0(p, l)                   \
        ({                                      \
                ਖਾਲੀ *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_ਸੁਰੱਖਿਅਤ(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_ਸੁਰੱਖਿਅਤ(_q_, p, _l_);       \
        })

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ unsetp(ਖਾਲੀ *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(ਖਾਲੀ**)p = ਨਿਰਾ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ freep(ਖਾਲੀ *p) {
        *(ਖਾਲੀ**)p = mfree(*(ਖਾਲੀ**) p);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_free_ _cleanup_(freep)

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਮਾਪ_multiply_ਉਛਾਲ(size_t ਮਾਪ, size_t need) {
        ਵਾਪਸ _unlikely_(need != 0 && ਮਾਪ > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *malloc_multiply(size_t need, size_t ਮਾਪ) {
        ਜੇ (ਮਾਪ_multiply_ਉਛਾਲ(ਮਾਪ, need))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਮਾਪ * need ?: 1);
}

_alloc_(2, 3) ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *memdup_multiply(ਅਚਲ ਖਾਲੀ *p, size_t need, size_t ਮਾਪ) {
        ਜੇ (ਮਾਪ_multiply_ਉਛਾਲ(ਮਾਪ, need))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ memdup(p, ਮਾਪ * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ *memdup_suffix0_multiply(ਅਚਲ ਖਾਲੀ *p, size_t need, size_t ਮਾਪ) {
        ਜੇ (ਮਾਪ_multiply_ਉਛਾਲ(ਮਾਪ, need))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ memdup_suffix0(p, ਮਾਪ * need);
}

size_t malloc_sizeof_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(x) \
        malloc_sizeof_ਸੁਰੱਖਿਅਤ((ਖਾਲੀ**) &__builtin_choose_expr(__builtin_constant_p(x), (ਖਾਲੀ*) { ਨਿਰਾ }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(x)/ਆਕਾਰ((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
ਖਾਲੀ *expand_ਮੰਜ਼ਿਲ_usable(ਖਾਲੀ *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

ਖਾਲੀ* realloc0(ਖਾਲੀ *p, size_t new_ਮਾਪ) _alloc_(2);

ਸਥਿਰ ਸਤਰਅੰਦਰ size_t GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(size_t l) {
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

        ਜੇ (l <= 2)
                ਵਾਪਸ 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        ਜੇ (m == 0) /* overflow? */
                ਵਾਪਸ l;

        ਵਾਪਸ m;
}

ਖਾਲੀ* greedy_realloc(ਖਾਲੀ **p, size_t need, size_t ਮਾਪ);
ਖਾਲੀ* greedy_realloc0(ਖਾਲੀ **p, size_t need, size_t ਮਾਪ);
ਖਾਲੀ* greedy_realloc_append(ਖਾਲੀ **p, size_t *n_p, ਅਚਲ ਖਾਲੀ *from, size_t n_from, size_t ਮਾਪ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((ਖਾਲੀ**) &(array), (need), ਆਕਾਰ((array)[0]))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((ਖਾਲੀ**) &(array), (need), ਆਕਾਰ((array)[0]))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                ਅਚਲ typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((ਖਾਲੀ**) &(array), &(n_array), _from_, (n_from), ਆਕਾਰ((array)[0])); \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ alloca0(n)                                      \
        ({                                              \
                ਅੱਖਰ *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_ਸੁਰੱਖਿਅਤ(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ alloca_align(ਮਾਪ, align)                                       \
        ({                                                              \
                ਖਾਲੀ *_ptr_;                                            \
                size_t ਨਕਾਬ = (align) - 1;                            \
                size_t ਮਾਪ_2 = ਮਾਪ;                                   \
                _ptr_ = alloca_ਸੁਰੱਖਿਅਤ(ਮਾਪ_2 + ਨਕਾਬ);                   \
                (ਖਾਲੀ*)(((uintptr_t)_ptr_ + ਨਕਾਬ) & ~ਨਕਾਬ);         \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ alloca0_align(ਮਾਪ, align)                                      \
        ({                                                              \
                ਖਾਲੀ *_new_;                                            \
                size_t _xsize_ = (ਮਾਪ);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#ਜੇ HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#ਨਹੀਂਤਾਂ
#  define msan_unpoison(r, s)
#ਜੇ_ਅੰਤ

/* Free every element of the array. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ free_many(ਖਾਲੀ **p, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ free_many_charp(ਅੱਖਰ **c, size_t n) {
        free_many((ਖਾਲੀ**) c, n);
}
