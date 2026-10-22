/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(ಶೂನ್ಯ) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* memcpy_safe(ಶೂನ್ಯ *dst, ಧ್ರುವ ಶೂನ್ಯ *src, size_t n) {
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ dst;
        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತ್ಯಾಗಮ memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* mempcpy_safe(ಶೂನ್ಯ *dst, ಧ್ರುವ ಶೂನ್ಯ *src, size_t n) {
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ dst;
        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತ್ಯಾಗಮ mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, ಪರಿಮಾಣ((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ memcmp_safe(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2, size_t n) {
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪ್ರತಿಜ್ಞಾ(s1);
        ಪ್ರತಿಜ್ಞಾ(s2);
        ಪ್ರತ್ಯಾಗಮ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ memcmp_nn(ಧ್ರುವ ಶೂನ್ಯ *s1, size_t n1, ಧ್ರುವ ಶೂನ್ಯ *s2, size_t n2) {
        ಪ್ರತ್ಯಾಗಮ memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), ಪರಿಮಾಣ(x)))

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* mempset(ಶೂನ್ಯ *s, ಪೂರ್ಣಾಙ್ಕ c, size_t n) {
        memset(s, c, n);
        ಪ್ರತ್ಯಾಗಮ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* memmem_safe(ಧ್ರುವ ಶೂನ್ಯ *haystack, size_t haystacklen, ಧ್ರುವ ಶೂನ್ಯ *needle, size_t needlelen) {

        ಯದಿ (needlelen <= 0)
                ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) haystack;

        ಯದಿ (haystacklen < needlelen)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(haystack);
        ಪ್ರತಿಜ್ಞಾ(needle);

        ಪ್ರತ್ಯಾಗಮ memmem(haystack, haystacklen, needle, needlelen);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* mempmem_safe(ಧ್ರುವ ಶೂನ್ಯ *haystack, size_t haystacklen, ಧ್ರುವ ಶೂನ್ಯ *needle, size_t needlelen) {
        ಧ್ರುವ uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (uint8_t*) p + needlelen;
}

ಶೂನ್ಯ* erase_and_free(ಶೂನ್ಯ *p);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ erase_and_freep(ಶೂನ್ಯ *p) {
        erase_and_free(*(ಶೂನ್ಯ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ erase_char(ವರ್ಣ *p) {
        explicit_bzero_safe(p, ಪರಿಮಾಣ(ವರ್ಣ));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ಯದಿ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ಪ್ರತಿಜ್ಞಾ(_sz > 0);                                \
                        ಪ್ರತಿಜ್ಞಾ(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ರಿಕ್ತ;                                       \
        })
