/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "memory-util.h"        /* IWYU pragma: keep */

#எனில் HAS_FEATURE_MEMORY_SANITIZER
#  include <sanitizer/msan_interface.h>
#endif

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#define ALLOCA_MAX (4U*1024U*1024U)

#define new(t, n) ((t*) malloc_multiply(n, அளவு(t)))

#define new0(t, n) ((t*) calloc((n) ?: 1, அளவு(t)))

#define alloca_safe(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#define newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, அளவு(t)));            \
                (t*) alloca_safe(_n_);                                  \
        })

#define newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_ASSIGN_SAFE(&_n_, அளவு(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#define newdup(t, p, n) ((t*) memdup_multiply(p, n, அளவு(t)))

#define newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, அளவு(t)))

#define malloc0(n) (calloc(1, (n) ?: 1))

#define free_and_replace(a, b)                  \
        free_and_replace_full(a, b, விடுவி)

வெறுமை* memdup(மாறிலி வெறுமை *p, size_t l) _alloc_(2);
வெறுமை* memdup_suffix0(மாறிலி வெறுமை *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#define memdupa(p, l)                           \
        ({                                      \
                வெறுமை *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_safe(_l_);         \
                memcpy_safe(_q_, p, _l_);       \
        })

#define memdupa_suffix0(p, l)                   \
        ({                                      \
                வெறுமை *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_safe(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_safe(_q_, p, _l_);       \
        })

நிலையான உள்வரி வெறுமை unsetp(வெறுமை *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(வெறுமை**)p = வெற்று;
}

நிலையான உள்வரி வெறுமை freep(வெறுமை *p) {
        *(வெறுமை**)p = mfree(*(வெறுமை**) p);
}

#define _cleanup_free_ _cleanup_(freep)

நிலையான உள்வரி bool size_multiply_overflow(size_t size, size_t need) {
        திரும்பு _unlikely_(need != 0 && size > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) நிலையான உள்வரி வெறுமை *malloc_multiply(size_t need, size_t size) {
        எனில் (size_multiply_overflow(size, need))
                திரும்பு வெற்று;

        திரும்பு malloc(size * need ?: 1);
}

_alloc_(2, 3) நிலையான உள்வரி வெறுமை *memdup_multiply(மாறிலி வெறுமை *p, size_t need, size_t size) {
        எனில் (size_multiply_overflow(size, need))
                திரும்பு வெற்று;

        திரும்பு memdup(p, size * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
நிலையான உள்வரி வெறுமை *memdup_suffix0_multiply(மாறிலி வெறுமை *p, size_t need, size_t size) {
        எனில் (size_multiply_overflow(size, need))
                திரும்பு வெற்று;

        திரும்பு memdup_suffix0(p, size * need);
}

size_t malloc_sizeof_safe(வெறுமை **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#define MALLOC_SIZEOF_SAFE(x) \
        malloc_sizeof_safe((வெறுமை**) &__builtin_choose_expr(__builtin_constant_p(x), (வெறுமை*) { வெற்று }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#define MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_SAFE(x)/அளவு((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
வெறுமை *expand_to_usable(வெறுமை *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

வெறுமை* realloc0(வெறுமை *p, size_t new_size) _alloc_(2);

நிலையான உள்வரி size_t GREEDY_ALLOC_ROUND_UP(size_t l) {
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

        எனில் (l <= 2)
                திரும்பு 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        எனில் (m == 0) /* overflow? */
                திரும்பு l;

        திரும்பு m;
}

வெறுமை* greedy_realloc(வெறுமை **p, size_t need, size_t size);
வெறுமை* greedy_realloc0(வெறுமை **p, size_t need, size_t size);
வெறுமை* greedy_realloc_append(வெறுமை **p, size_t *n_p, மாறிலி வெறுமை *from, size_t n_from, size_t size);

#define GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((வெறுமை**) &(array), (need), அளவு((array)[0]))

#define GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((வெறுமை**) &(array), (need), அளவு((array)[0]))

#define GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                மாறிலி typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((வெறுமை**) &(array), &(n_array), _from_, (n_from), அளவு((array)[0])); \
        })

#define alloca0(n)                                      \
        ({                                              \
                எழுத்து *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_safe(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#define alloca_align(size, align)                                       \
        ({                                                              \
                வெறுமை *_ptr_;                                            \
                size_t _mask_ = (align) - 1;                            \
                size_t _size_ = size;                                   \
                _ptr_ = alloca_safe(_size_ + _mask_);                   \
                (வெறுமை*)(((uintptr_t)_ptr_ + _mask_) & ~_mask_);         \
        })

#define alloca0_align(size, align)                                      \
        ({                                                              \
                வெறுமை *_new_;                                            \
                size_t _xsize_ = (size);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#எனில் HAS_FEATURE_MEMORY_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#இல்லையெனில்
#  define msan_unpoison(r, s)
#endif

/* Free every element of the array. */
நிலையான உள்வரி வெறுமை free_many(வெறுமை **p, size_t n) {
        ப்ரதிஜ்ஞா(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
நிலையான உள்வரி வெறுமை free_many_charp(எழுத்து **c, size_t n) {
        free_many((வெறுமை**) c, n);
}
