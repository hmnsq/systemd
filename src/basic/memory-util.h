/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <string.h>
#شامل <sys/mman.h> /* IWYU pragma: export */

#شامل "forward.h"

#شامل "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(تهی) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ایستا درونخطی تهی* memcpy_safe(تهی *dst, ثابت تهی *src, size_t n) {
        اگر (n == 0)
                بازگشت dst;
        أكد(src);
        بازگشت memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ایستا درونخطی تهی* mempcpy_safe(تهی *dst, ثابت تهی *src, size_t n) {
        اگر (n == 0)
                بازگشت dst;
        أكد(src);
        بازگشت mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, اندازه((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ایستا درونخطی صحیح memcmp_safe(ثابت تهی *s1, ثابت تهی *s2, size_t n) {
        اگر (n == 0)
                بازگشت 0;
        أكد(s1);
        أكد(s2);
        بازگشت memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ایستا درونخطی صحیح memcmp_nn(ثابت تهی *s1, size_t n1, ثابت تهی *s2, size_t n2) {
        بازگشت memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), اندازه(x)))

ایستا درونخطی تهی* mempset(تهی *s, صحیح c, size_t n) {
        memset(s, c, n);
        بازگشت (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ایستا درونخطی تهی* memmem_safe(ثابت تهی *haystack, size_t haystacklen, ثابت تهی *needle, size_t needlelen) {

        اگر (needlelen <= 0)
                بازگشت (تهی*) haystack;

        اگر (haystacklen < needlelen)
                بازگشت پوچ;

        أكد(haystack);
        أكد(needle);

        بازگشت memmem(haystack, haystacklen, needle, needlelen);
}

ایستا درونخطی تهی* mempmem_safe(ثابت تهی *haystack, size_t haystacklen, ثابت تهی *needle, size_t needlelen) {
        ثابت uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        اگر (!p)
                بازگشت پوچ;

        بازگشت (uint8_t*) p + needlelen;
}

تهی* erase_and_free(تهی *p);

ایستا درونخطی تهی erase_and_freep(تهی *p) {
        erase_and_free(*(تهی**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ایستا درونخطی تهی erase_char(نویسه *p) {
        explicit_bzero_safe(p, اندازه(نویسه));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                اگر (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        أكد(_sz > 0);                                \
                        أكد(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) پوچ;                                       \
        })
