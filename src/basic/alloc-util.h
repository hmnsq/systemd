/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "forward.h"
#అన్తర్భావ "memory-util.h"        /* IWYU pragma: keep */

#ఒకవేళ HAS_FEATURE_జ్ఞాపకం_SANITIZER
#  include <sanitizer/msan_interface.h>
#అయితే_ముగింపు

/* If for some reason more than 4M are allocated on the stack, let's abort immediately. It's better than
 * proceeding and smashing the stack limits. Note that by default RLIMIT_STACK is 8M on Linux. */
#నిర్వచించు ALLOCA_MAX (4U*1024U*1024U)

#నిర్వచించు new(t, n) ((t*) malloc_multiply(n, పరిమాణం(t)))

#నిర్వచించు new0(t, n) ((t*) సున్నా_జ్ఞాపకం_ఇవ్వు((n) ?: 1, పరిమాణం(t)))

#నిర్వచించు alloca_సురక్షిత(n)                                                  \
        ({                                                              \
                size_t _nn_ = (n);                                      \
                assert_se(_nn_ <= ALLOCA_MAX);                          \
                alloca(_nn_ == 0 ? 1 : _nn_);                           \
        })                                                              \

#నిర్వచించు newa(t, n)                                                      \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_కేటాయించు_సురక్షిత(&_n_, పరిమాణం(t)));            \
                (t*) alloca_సురక్షిత(_n_);                                  \
        })

#నిర్వచించు newa0(t, n)                                                     \
        ({                                                              \
                size_t _n_ = (n);                                       \
                assert_se(MUL_కేటాయించు_సురక్షిత(&_n_, పరిమాణం(t)));            \
                (t*) alloca0(_n_);                                      \
        })

#నిర్వచించు newdup(t, p, n) ((t*) memdup_multiply(p, n, పరిమాణం(t)))

#నిర్వచించు newdup_suffix0(t, p, n) ((t*) memdup_suffix0_multiply(p, n, పరిమాణం(t)))

#నిర్వచించు malloc0(n) (సున్నా_జ్ఞాపకం_ఇవ్వు(1, (n) ?: 1))

#నిర్వచించు free_and_replace(a, b)                  \
        free_and_replace_నిండిన(a, b, విడుదల)

శూన్య* memdup(స్థిరరాశి శూన్య *p, size_t l) _alloc_(2);
శూన్య* memdup_suffix0(స్థిరరాశి శూన్య *p, size_t l); /* We can't use _alloc_() here, since we return a buffer one byte larger than the specified size */

#నిర్వచించు memdupa(p, l)                           \
        ({                                      \
                శూన్య *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_సురక్షిత(_l_);         \
                memcpy_సురక్షిత(_q_, p, _l_);       \
        })

#నిర్వచించు memdupa_suffix0(p, l)                   \
        ({                                      \
                శూన్య *_q_;                      \
                size_t _l_ = l;                 \
                _q_ = alloca_సురక్షిత(_l_ + 1);     \
                ((uint8_t*) _q_)[_l_] = 0;      \
                memcpy_సురక్షిత(_q_, p, _l_);       \
        })

స్థిర అంతఃవరుస శూన్య unsetp(శూన్య *p) {
        /* A trivial "destructor" that can be used in cases where we want to
         * unset a pointer from a _cleanup_ function. */

        *(శూన్య**)p = ఖాళీ;
}

స్థిర అంతఃవరుస శూన్య freep(శూన్య *p) {
        *(శూన్య**)p = mfree(*(శూన్య**) p);
}

#నిర్వచించు _cleanup_free_ _cleanup_(freep)

స్థిర అంతఃవరుస bool కొలత_multiply_అధికప్రవాహం(size_t కొలత, size_t need) {
        తిరిగి _unlikely_(need != 0 && కొలత > (SIZE_MAX / need));
}

_malloc_ _alloc_(1, 2) స్థిర అంతఃవరుస శూన్య *malloc_multiply(size_t need, size_t కొలత) {
        ఒకవేళ (కొలత_multiply_అధికప్రవాహం(కొలత, need))
                తిరిగి ఖాళీ;

        తిరిగి జ్ఞాపకం_ఇవ్వు(కొలత * need ?: 1);
}

_alloc_(2, 3) స్థిర అంతఃవరుస శూన్య *memdup_multiply(స్థిరరాశి శూన్య *p, size_t need, size_t కొలత) {
        ఒకవేళ (కొలత_multiply_అధికప్రవాహం(కొలత, need))
                తిరిగి ఖాళీ;

        తిరిగి memdup(p, కొలత * need);
}

/* Note that we can't decorate this function with _alloc_() since the returned memory area is one byte larger
 * than the product of its parameters. */
స్థిర అంతఃవరుస శూన్య *memdup_suffix0_multiply(స్థిరరాశి శూన్య *p, size_t need, size_t కొలత) {
        ఒకవేళ (కొలత_multiply_అధికప్రవాహం(కొలత, need))
                తిరిగి ఖాళీ;

        తిరిగి memdup_suffix0(p, కొలత * need);
}

size_t malloc_sizeof_సురక్షిత(శూన్య **xp);

/* This returns the number of usable bytes in a malloc()ed region as per malloc_usable_size(), which may
 * return a value larger than the size that was actually allocated. Access to that additional memory is
 * discouraged because it violates the C standard; a compiler cannot see that this as valid. To help the
 * compiler out, the MALLOC_SIZEOF_SAFE macro 'allocates' the usable size using a dummy allocator function
 * expand_to_usable. There is a possibility of malloc_usable_size() returning different values during the
 * lifetime of an object, which may cause problems, but the glibc allocator does not do that at the moment. */
#నిర్వచించు MALLOC_SIZEOF_సురక్షిత(x) \
        malloc_sizeof_సురక్షిత((శూన్య**) &__builtin_choose_expr(__builtin_constant_p(x), (శూన్య*) { ఖాళీ }, (x)))

/* Inspired by ELEMENTSOF() but operates on malloc()'ed memory areas: typesafely returns the number of items
 * that fit into the specified memory block */
#నిర్వచించు MALLOC_ELEMENTSOF(x) \
        (__builtin_choose_expr(                                         \
                __builtin_types_compatible_p(typeof(x), typeof(&*(x))), \
                MALLOC_SIZEOF_సురక్షిత(x)/పరిమాణం((x)[0]),                   \
                VOID_0))

/* Dummy allocator to tell the compiler that the new size of p is newsize. The implementation returns the
 * pointer as is; the only reason for its existence is as a conduit for the _alloc_ attribute.  This must not
 * be inlined (hence a non-static function with _noinline_ because LTO otherwise tries to inline it) because
 * gcc then loses the attributes on the function.
 * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96503 */
శూన్య *expand_గమ్యం_usable(శూన్య *p, size_t newsize) _alloc_(2) _returns_nonnull_ _noinline_;

శూన్య* realloc0(శూన్య *p, size_t new_కొలత) _alloc_(2);

స్థిర అంతఃవరుస size_t GREEDY_ALLOC_విడత_పైకి(size_t l) {
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

        ఒకవేళ (l <= 2)
                తిరిగి 2; /* Never allocate less than 2 of something.  */

        m = ALIGN_POWER2(l);
        ఒకవేళ (m == 0) /* overflow? */
                తిరిగి l;

        తిరిగి m;
}

శూన్య* greedy_realloc(శూన్య **p, size_t need, size_t కొలత);
శూన్య* greedy_realloc0(శూన్య **p, size_t need, size_t కొలత);
శూన్య* greedy_realloc_append(శూన్య **p, size_t *n_p, స్థిరరాశి శూన్య *from, size_t n_from, size_t కొలత);

#నిర్వచించు GREEDY_REALLOC(array, need)                                     \
        greedy_realloc((శూన్య**) &(array), (need), పరిమాణం((array)[0]))

#నిర్వచించు GREEDY_REALLOC0(array, need)                                    \
        greedy_realloc0((శూన్య**) &(array), (need), పరిమాణం((array)[0]))

#నిర్వచించు GREEDY_REALLOC_APPEND(array, n_array, from, n_from)             \
        ({                                                              \
                స్థిరరాశి typeof(*(array)) *_from_ = (from);                \
                greedy_realloc_append((శూన్య**) &(array), &(n_array), _from_, (n_from), పరిమాణం((array)[0])); \
        })

#నిర్వచించు alloca0(n)                                      \
        ({                                              \
                అక్షరం *_new_;                            \
                size_t _len_ = n;                       \
                _new_ = alloca_సురక్షిత(_len_);             \
                memset(_new_, 0, _len_);                \
        })

/* It's not clear what alignment glibc/gcc alloca() guarantee, hence provide a guaranteed safe version */
#నిర్వచించు alloca_align(కొలత, align)                                       \
        ({                                                              \
                శూన్య *_ptr_;                                            \
                size_t ముసుగు = (align) - 1;                            \
                size_t కొలత_2 = కొలత;                                   \
                _ptr_ = alloca_సురక్షిత(కొలత_2 + ముసుగు);                   \
                (శూన్య*)(((uintptr_t)_ptr_ + ముసుగు) & ~ముసుగు);         \
        })

#నిర్వచించు alloca0_align(కొలత, align)                                      \
        ({                                                              \
                శూన్య *_new_;                                            \
                size_t _xsize_ = (కొలత);                                \
                _new_ = alloca_align(_xsize_, (align));                 \
                memset(_new_, 0, _xsize_);                              \
        })

#ఒకవేళ HAS_FEATURE_జ్ఞాపకం_SANITIZER
#  define msan_unpoison(r, s) __msan_unpoison(r, s)
#లేకపోతే
#  define msan_unpoison(r, s)
#అయితే_ముగింపు

/* Free every element of the array. */
స్థిర అంతఃవరుస శూన్య free_many(శూన్య **p, size_t n) {
        ప్రతిజ్ఞా(p || n == 0);

        FOREACH_ARRAY(i, p, n)
                *i = mfree(*i);
}

/* Typesafe wrapper for char** rather than void**. Unfortunately C won't implicitly cast this. */
స్థిర అంతఃవరుస శూన్య free_many_charp(అక్షరం **c, size_t n) {
        free_many((శూన్య**) c, n);
}
