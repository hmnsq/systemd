/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <string.h>
#ضمن <sys/mman.h> /* IWYU pragma: export */

#ضمن "forward.h"

#ضمن "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(فراغ) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ساكن مضمن فراغ* memcpy_safe(فراغ *dst, ثابت فراغ *src, size_t n) {
        ܐܢ (n == 0)
                ܦܢܐ dst;
        أكد(src);
        ܦܢܐ memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ساكن مضمن فراغ* mempcpy_safe(فراغ *dst, ثابت فراغ *src, size_t n) {
        ܐܢ (n == 0)
                ܦܢܐ dst;
        أكد(src);
        ܦܢܐ mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, حجم((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ساكن مضمن صحيح memcmp_safe(ثابت فراغ *s1, ثابت فراغ *s2, size_t n) {
        ܐܢ (n == 0)
                ܦܢܐ 0;
        أكد(s1);
        أكد(s2);
        ܦܢܐ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ساكن مضمن صحيح memcmp_nn(ثابت فراغ *s1, size_t n1, ثابت فراغ *s2, size_t n2) {
        ܦܢܐ memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), حجم(x)))

ساكن مضمن فراغ* mempset(فراغ *s, صحيح c, size_t n) {
        memset(s, c, n);
        ܦܢܐ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ساكن مضمن فراغ* memmem_safe(ثابت فراغ *haystack, size_t haystacklen, ثابت فراغ *needle, size_t needlelen) {

        ܐܢ (needlelen <= 0)
                ܦܢܐ (فراغ*) haystack;

        ܐܢ (haystacklen < needlelen)
                ܦܢܐ عدم;

        أكد(haystack);
        أكد(needle);

        ܦܢܐ memmem(haystack, haystacklen, needle, needlelen);
}

ساكن مضمن فراغ* mempmem_safe(ثابت فراغ *haystack, size_t haystacklen, ثابت فراغ *needle, size_t needlelen) {
        ثابت uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        ܐܢ (!p)
                ܦܢܐ عدم;

        ܦܢܐ (uint8_t*) p + needlelen;
}

فراغ* erase_and_free(فراغ *p);

ساكن مضمن فراغ erase_and_freep(فراغ *p) {
        erase_and_free(*(فراغ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ساكن مضمن فراغ erase_char(حرف *p) {
        explicit_bzero_safe(p, حجم(حرف));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ܐܢ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        أكد(_sz > 0);                                \
                        أكد(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) عدم;                                       \
        })
