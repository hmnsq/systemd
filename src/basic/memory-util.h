/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <string.h>
#شامل <sys/mman.h> /* IWYU pragma: export */

#شامل "forward.h"

#شامل "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t صفحہ_سائز(خالی) خالص;
#تعریف_کرو صفحہ_ALIGN(l)          ALIGN_مقصد(l, صفحہ_سائز())
#تعریف_کرو صفحہ_ALIGN_U64(l)      ALIGN_مقصد_U64(l, صفحہ_سائز())
#تعریف_کرو صفحہ_ALIGN_نیچے_کو(l)     ALIGN_نیچے_کو(l, صفحہ_سائز())
#تعریف_کرو صفحہ_ALIGN_نیچے_کو_U64(l) ALIGN_نیچے_کو_U64(l, صفحہ_سائز())
#تعریف_کرو صفحہ_انحراف(l)         ALIGN_انحراف(l, صفحہ_سائز())
#تعریف_کرو صفحہ_انحراف_U64(l)     ALIGN_انحراف_U64(l, صفحہ_سائز())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ساکن اندرونی خالی* memcpy_محفوظ(خالی *dst, ثابت خالی *src, size_t n) {
        اگر (n == 0)
                واپس dst;
        أكد(src);
        واپس حافظہ_نقل(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ساکن اندرونی خالی* mempcpy_محفوظ(خالی *dst, ثابت خالی *src, size_t n) {
        اگر (n == 0)
                واپس dst;
        أكد(src);
        واپس mempcpy(dst, src, n);
}

#تعریف_کرو _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_محفوظ(&sz, حجم((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_محفوظ(dst, src, sz);         \
        })

#تعریف_کرو mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ساکن اندرونی صحیح memcmp_محفوظ(ثابت خالی *s1, ثابت خالی *s2, size_t n) {
        اگر (n == 0)
                واپس 0;
        أكد(s1);
        أكد(s2);
        واپس memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ساکن اندرونی صحیح memcmp_nn(ثابت خالی *s1, size_t n1, ثابت خالی *s2, size_t n2) {
        واپس memcmp_محفوظ(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#تعریف_کرو zero(x) (memzero(&(x), حجم(x)))

ساکن اندرونی خالی* mempset(خالی *s, صحیح c, size_t n) {
        memset(s, c, n);
        واپس (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ساکن اندرونی خالی* memmem_محفوظ(ثابت خالی *haystack, size_t haystacklen, ثابت خالی *needle, size_t needlelen) {

        اگر (needlelen <= 0)
                واپس (خالی*) haystack;

        اگر (haystacklen < needlelen)
                واپس تہی;

        أكد(haystack);
        أكد(needle);

        واپس memmem(haystack, haystacklen, needle, needlelen);
}

ساکن اندرونی خالی* mempmem_محفوظ(ثابت خالی *haystack, size_t haystacklen, ثابت خالی *needle, size_t needlelen) {
        ثابت uint8_t *p;

        p = memmem_محفوظ(haystack, haystacklen, needle, needlelen);
        اگر (!p)
                واپس تہی;

        واپس (uint8_t*) p + needlelen;
}

خالی* erase_and_free(خالی *p);

ساکن اندرونی خالی erase_and_freep(خالی *p) {
        erase_and_free(*(خالی**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ساکن اندرونی خالی erase_char(حرف *p) {
        explicit_bzero_محفوظ(p, حجم(حرف));
}

#تعریف_کرو munmap_محفوظ(p, sz) munmap_محفوظ_2(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#تعریف_کرو munmap_محفوظ_2(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                اگر (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        أكد(_sz > 0);                                \
                        أكد(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) تہی;                                       \
        })
