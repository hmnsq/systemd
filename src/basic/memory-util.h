/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t ಪುಟ_ಗಾತ್ರ(ಶೂನ್ಯ) ಶುದ್ಧ;
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ALIGN(l)          ALIGN_ಗಮ್ಯ(l, ಪುಟ_ಗಾತ್ರ())
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ALIGN_U64(l)      ALIGN_ಗಮ್ಯ_U64(l, ಪುಟ_ಗಾತ್ರ())
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ALIGN_ಕೆಳಕ್ಕೆ(l)     ALIGN_ಕೆಳಕ್ಕೆ(l, ಪುಟ_ಗಾತ್ರ())
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ALIGN_ಕೆಳಕ್ಕೆ_U64(l) ALIGN_ಕೆಳಕ್ಕೆ_U64(l, ಪುಟ_ಗಾತ್ರ())
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ಸ್ಥಾನಾಂತರ(l)         ALIGN_ಸ್ಥಾನಾಂತರ(l, ಪುಟ_ಗಾತ್ರ())
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟ_ಸ್ಥಾನಾಂತರ_U64(l)     ALIGN_ಸ್ಥಾನಾಂತರ_U64(l, ಪುಟ_ಗಾತ್ರ())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* memcpy_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *dst, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, size_t n) {
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ dst;
        ಪ್ರತಿಜ್ಞಾ(src);
        ಹಿಂತಿರುಗಿ ಸ್ಮರಣೆ_ನಕಲು(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* mempcpy_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ *dst, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, size_t n) {
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ dst;
        ಪ್ರತಿಜ್ಞಾ(src);
        ಹಿಂತಿರುಗಿ mempcpy(dst, src, n);
}

#ವ್ಯಾಖ್ಯಾನಿಸು _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_ಸುರಕ್ಷಿತ(&sz, ಗಾತ್ರ((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_ಸುರಕ್ಷಿತ(dst, src, sz);         \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ memcmp_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2, size_t n) {
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ 0;
        ಪ್ರತಿಜ್ಞಾ(s1);
        ಪ್ರತಿಜ್ಞಾ(s2);
        ಹಿಂತಿರುಗಿ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ memcmp_nn(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, size_t n1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2, size_t n2) {
        ಹಿಂತಿರುಗಿ memcmp_ಸುರಕ್ಷಿತ(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#ವ್ಯಾಖ್ಯಾನಿಸು zero(x) (memzero(&(x), ಗಾತ್ರ(x)))

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* mempset(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಂಕ c, size_t n) {
        memset(s, c, n);
        ಹಿಂತಿರುಗಿ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* memmem_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *haystack, size_t haystacklen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *needle, size_t needlelen) {

        ಆದರೆ (needlelen <= 0)
                ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) haystack;

        ಆದರೆ (haystacklen < needlelen)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        ಹಿಂತಿರುಗಿ memmem(haystack, haystacklen, needle, needlelen);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* mempmem_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *haystack, size_t haystacklen, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *needle, size_t needlelen) {
        ಸ್ಥಿರಾಂಕ uint8_t *p;

        p = memmem_ಸುರಕ್ಷಿತ(haystack, haystacklen, needle, needlelen);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (uint8_t*) p + needlelen;
}

ಶೂನ್ಯ* erase_and_free(ಶೂನ್ಯ *p);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ erase_and_freep(ಶೂನ್ಯ *p) {
        erase_and_free(*(ಶೂನ್ಯ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ erase_char(ಅಕ್ಷರ *p) {
        explicit_bzero_ಸುರಕ್ಷಿತ(p, ಗಾತ್ರ(ಅಕ್ಷರ));
}

#ವ್ಯಾಖ್ಯಾನಿಸು munmap_ಸುರಕ್ಷಿತ(p, sz) munmap_ಸುರಕ್ಷಿತ_2(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#ವ್ಯಾಖ್ಯಾನಿಸು munmap_ಸುರಕ್ಷಿತ_2(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ಆದರೆ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ಪ್ರತಿಜ್ಞಾ(_sz > 0);                                \
                        ಪ್ರತಿಜ್ಞಾ(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ಖಾಲಿ;                                       \
        })
