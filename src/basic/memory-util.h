/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/mman.h> /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

#அந்தர்பாவ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(வெறுமை) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
நிலையான உள்வரி வெறுமை* memcpy_safe(வெறுமை *dst, மாறிலி வெறுமை *src, size_t n) {
        எனில் (n == 0)
                திரும்பு dst;
        ப்ரதிஜ்ஞா(src);
        திரும்பு memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
நிலையான உள்வரி வெறுமை* mempcpy_safe(வெறுமை *dst, மாறிலி வெறுமை *src, size_t n) {
        எனில் (n == 0)
                திரும்பு dst;
        ப்ரதிஜ்ஞா(src);
        திரும்பு mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, அளவு((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
நிலையான உள்வரி முழு memcmp_safe(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2, size_t n) {
        எனில் (n == 0)
                திரும்பு 0;
        ப்ரதிஜ்ஞா(s1);
        ப்ரதிஜ்ஞா(s2);
        திரும்பு memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
நிலையான உள்வரி முழு memcmp_nn(மாறிலி வெறுமை *s1, size_t n1, மாறிலி வெறுமை *s2, size_t n2) {
        திரும்பு memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), அளவு(x)))

நிலையான உள்வரி வெறுமை* mempset(வெறுமை *s, முழு c, size_t n) {
        memset(s, c, n);
        திரும்பு (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
நிலையான உள்வரி வெறுமை* memmem_safe(மாறிலி வெறுமை *haystack, size_t haystacklen, மாறிலி வெறுமை *needle, size_t needlelen) {

        எனில் (needlelen <= 0)
                திரும்பு (வெறுமை*) haystack;

        எனில் (haystacklen < needlelen)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(haystack);
        ப்ரதிஜ்ஞா(needle);

        திரும்பு memmem(haystack, haystacklen, needle, needlelen);
}

நிலையான உள்வரி வெறுமை* mempmem_safe(மாறிலி வெறுமை *haystack, size_t haystacklen, மாறிலி வெறுமை *needle, size_t needlelen) {
        மாறிலி uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        எனில் (!p)
                திரும்பு வெற்று;

        திரும்பு (uint8_t*) p + needlelen;
}

வெறுமை* erase_and_free(வெறுமை *p);

நிலையான உள்வரி வெறுமை erase_and_freep(வெறுமை *p) {
        erase_and_free(*(வெறுமை**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
நிலையான உள்வரி வெறுமை erase_char(எழுத்து *p) {
        explicit_bzero_safe(p, அளவு(எழுத்து));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                எனில் (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ப்ரதிஜ்ஞா(_sz > 0);                                \
                        ப்ரதிஜ்ஞா(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) வெற்று;                                       \
        })
