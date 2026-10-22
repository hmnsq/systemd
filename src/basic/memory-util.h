/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <string.h>
#ضمن <sys/mman.h> /* IWYU pragma: export */

#ضمن "forward.h"

#ضمن "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t صفحو_سائيز(فراغ) خالص;
#وصف_ڪريو صفحو_ALIGN(l)          ALIGN_منزل(l, صفحو_سائيز())
#وصف_ڪريو صفحو_ALIGN_U64(l)      ALIGN_منزل_U64(l, صفحو_سائيز())
#وصف_ڪريو صفحو_ALIGN_هيٺ_ڏانهن(l)     ALIGN_هيٺ_ڏانهن(l, صفحو_سائيز())
#وصف_ڪريو صفحو_ALIGN_هيٺ_ڏانهن_U64(l) ALIGN_هيٺ_ڏانهن_U64(l, صفحو_سائيز())
#وصف_ڪريو صفحو_وٿي(l)         ALIGN_وٿي(l, صفحو_سائيز())
#وصف_ڪريو صفحو_وٿي_U64(l)     ALIGN_وٿي_U64(l, صفحو_سائيز())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ساكن مضمن فراغ* memcpy_محفوظ(فراغ *dst, ثابت فراغ *src, size_t n) {
        إذا (n == 0)
                أرجع dst;
        أكد(src);
        أرجع يادگيري_نقل(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ساكن مضمن فراغ* mempcpy_محفوظ(فراغ *dst, ثابت فراغ *src, size_t n) {
        إذا (n == 0)
                أرجع dst;
        أكد(src);
        أرجع mempcpy(dst, src, n);
}

#وصف_ڪريو _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_محفوظ(&sz, حجم((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_محفوظ(dst, src, sz);         \
        })

#وصف_ڪريو mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ساكن مضمن صحيح memcmp_محفوظ(ثابت فراغ *s1, ثابت فراغ *s2, size_t n) {
        إذا (n == 0)
                أرجع 0;
        أكد(s1);
        أكد(s2);
        أرجع memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ساكن مضمن صحيح memcmp_nn(ثابت فراغ *s1, size_t n1, ثابت فراغ *s2, size_t n2) {
        أرجع memcmp_محفوظ(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#وصف_ڪريو zero(x) (memzero(&(x), حجم(x)))

ساكن مضمن فراغ* mempset(فراغ *s, صحيح c, size_t n) {
        memset(s, c, n);
        أرجع (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ساكن مضمن فراغ* memmem_محفوظ(ثابت فراغ *haystack, size_t haystacklen, ثابت فراغ *needle, size_t needlelen) {

        إذا (needlelen <= 0)
                أرجع (فراغ*) haystack;

        إذا (haystacklen < needlelen)
                أرجع عدم;

        أكد(haystack);
        أكد(needle);

        أرجع memmem(haystack, haystacklen, needle, needlelen);
}

ساكن مضمن فراغ* mempmem_محفوظ(ثابت فراغ *haystack, size_t haystacklen, ثابت فراغ *needle, size_t needlelen) {
        ثابت uint8_t *p;

        p = memmem_محفوظ(haystack, haystacklen, needle, needlelen);
        إذا (!p)
                أرجع عدم;

        أرجع (uint8_t*) p + needlelen;
}

فراغ* erase_and_free(فراغ *p);

ساكن مضمن فراغ erase_and_freep(فراغ *p) {
        erase_and_free(*(فراغ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ساكن مضمن فراغ erase_char(حرف *p) {
        explicit_bzero_محفوظ(p, حجم(حرف));
}

#وصف_ڪريو munmap_محفوظ(p, sz) munmap_محفوظ_2(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#وصف_ڪريو munmap_محفوظ_2(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                إذا (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        أكد(_sz > 0);                                \
                        أكد(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) عدم;                                       \
        })
