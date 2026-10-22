/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <stdlib.h>

#شامل "forward.h"
#شامل "memory-util.h"        /* IWYU pragma: keep */

#اگر HAS_FEATURE_حافظہ_SANITIZER
#  include <sanitizer/msan_interface.h>
#اگر_ختم

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#تعریف_کرو ALLOCA_MAX (4U*1024U*1024U)

#تعریف_کرو new(t, n) ((t*) malloc_multiply(n, حجم(t)))

#تعریف_کرو new0(t, n) ((t*) صفر_حافظہ_دو((n) ?: 1, حجم(t)))

#تعریف_کرو alloca_محفوظ(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#تعریف_کرو newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_تفویض_محفوظ(&_n_, حجم(t)));            \
                (t*) alloca_محفوظ(_n_);                                  \
        })

#تعریف_کرو newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_تفویض_محفوظ(&_n_, حجم(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#تعریف_کرو newdup(t, p, n) ((t*) memdup_multiply(p, n, حجم(t)))

#تعریف_کرو newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, حجم(t)))

#تعریف_کرو malloc0(n) (صفر_حافظہ_دو(1, (n) ?: 1))

#تعریف_کرو free_and_replace(a, b)                  \
        free_and_replace_بھرا(a, b, آزاد)

خالی* memdup(ثابت خالی *p, size_t l) _alloc_(2);
خالی* memdup_suffix0(ثابت خالی *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#تعریف_کرو memdupa(p, l)                           \
        ({                                      \
                خالی *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_محفوظ(_l_);         \
                memcpy_محفوظ(_q_, p, _l_);       \
        })

#تعریف_کرو memdupa_suffix0(p, l)                   \
        ({                                      \
                خالی *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_محفوظ(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_محفوظ(_q_, p, _l_);       \
        })

ساکن اندرونی خالی unsetp(خالی *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(خالی**)p = تہی;
}

ساکن اندرونی خالی freep(خالی *p) {
        *(خالی**)p = mfree(*(خالی**) p);
}

#تعریف_کرو _cleanup_free_ _cleanup_(freep)

ساکن اندرونی bool سائز_multiply_حد_سے_تجاوز(size_t سائز, size_t need) {
        واپس _unlikely_(need != 0 && سائز > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) ساکن اندرونی خالی *malloc_multiply(size_t need, size_t سائز) {
        اگر (سائز_multiply_حد_سے_تجاوز(سائز, need))
                واپس تہی;

        واپس حافظہ_دو(سائز * need ?: 1);
}

_alloc_(2, 3) ساکن اندرونی خالی *memdup_multiply(ثابت خالی *p, size_t need, size_t سائز) {
        اگر (سائز_multiply_حد_سے_تجاوز(سائز, need))
                واپس تہی;

        واپس memdup(p, سائز * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
ساکن اندرونی خالی *memdup_suffix0_multiply(ثابت خالی *p, size_t need, size_t سائز) {
        اگر (سائز_multiply_حد_سے_تجاوز(سائز, need))
                واپس تہی;

        واپس memdup_suffix0(p, سائز * need);
}

size_t malloc_sizeof_محفوظ(خالی **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#تعریف_کرو MALLOC_SIZEOF_محفوظ(x) \
        malloc_sizeof_محفوظ((خالی**) &__builtin_choose_expr(__builtin_constant_p(x), (خالی*) { تہی }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#تعریف_کرو MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_محفوظ(x)/حجم((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
خالی *expand_مقصد_usable(خالی *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

خالی* realloc0(خالی *p, size_t new_سائز) _alloc_(2);

ساکن اندرونی size_t GREEDY_ALLOC_باری_اوپر(size_t l) {
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

        اگر (l <= 2)
                واپس 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        اگر (m == 0) /* overflow? */
                واپس l;

        واپس m;
}

خالی* greedy_realloc(خالی **p, size_t need, size_t سائز);
خالی* greedy_realloc0(خالی **p, size_t need, size_t سائز);
خالی* greedy_realloc_append(خالی **p, size_t *n_p, ثابت خالی *from, size_t n_from, size_t سائز);

#تعریف_کرو GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((خالی**) &(array), (need), حجم((array)[0]))

#تعریف_کرو GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((خالی**) &(array), (need), حجم((array)[0]))

#تعریف_کرو GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                ثابت typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((خالی**) &(array), &(n_array), _from_, (n_from), حجم((array)[0])); \
        })

#تعریف_کرو alloca0(n)                                      \
        ({                                              \
                حرف *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_محفوظ(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#تعریف_کرو alloca_align(سائز, align)                                       \
        ({                                                              \
                خالی *_ptr_;                                            \
                size_t نقاب = (align) - 1;                            \
                size_t سائز_2 = سائز;                                   \
                _ptr_ = alloca_محفوظ(سائز_2 + نقاب);                   \
                (خالی*)(((uintptr_t)_ptr_ + نقاب) & ~نقاب);         \
        })

#تعریف_کرو alloca0_align(سائز, align)                                      \
        ({                                                              \
                خالی *_new_;                                            \
                size_t _xsize_ = (سائز);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#اگر HAS_FEATURE_حافظہ_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#ورنہ
#  define msan_unpoison(r, s)
#اگر_ختم

/* Free every element of the array. */
ساکن اندرونی خالی free_many(خالی **p, size_t n) {
        أكد(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
ساکن اندرونی خالی free_many_charp(حرف **c, size_t n) {
        free_many((خالی**) c, n);
}
