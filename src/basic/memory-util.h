/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/mman.h> /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

#ଅନ୍ତର୍ଭାଵ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(ଶୂନ୍ୟ) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* memcpy_safe(ଶୂନ୍ୟ *dst, ଅଚଳ ଶୂନ୍ୟ *src, size_t n) {
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ dst;
        ପ୍ରତିଜ୍ଞା(src);
        ଫେରନ୍ତୁ memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* mempcpy_safe(ଶୂନ୍ୟ *dst, ଅଚଳ ଶୂନ୍ୟ *src, size_t n) {
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ dst;
        ପ୍ରତିଜ୍ଞା(src);
        ଫେରନ୍ତୁ mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, ଆକାର((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା memcmp_safe(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2, size_t n) {
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ 0;
        ପ୍ରତିଜ୍ଞା(s1);
        ପ୍ରତିଜ୍ଞା(s2);
        ଫେରନ୍ତୁ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା memcmp_nn(ଅଚଳ ଶୂନ୍ୟ *s1, size_t n1, ଅଚଳ ଶୂନ୍ୟ *s2, size_t n2) {
        ଫେରନ୍ତୁ memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), ଆକାର(x)))

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* mempset(ଶୂନ୍ୟ *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, size_t n) {
        memset(s, c, n);
        ଫେରନ୍ତୁ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* memmem_safe(ଅଚଳ ଶୂନ୍ୟ *haystack, size_t haystacklen, ଅଚଳ ଶୂନ୍ୟ *needle, size_t needlelen) {

        ଯଦି (needlelen <= 0)
                ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) haystack;

        ଯଦି (haystacklen < needlelen)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(haystack);
        ପ୍ରତିଜ୍ଞା(needle);

        ଫେରନ୍ତୁ memmem(haystack, haystacklen, needle, needlelen);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* mempmem_safe(ଅଚଳ ଶୂନ୍ୟ *haystack, size_t haystacklen, ଅଚଳ ଶୂନ୍ୟ *needle, size_t needlelen) {
        ଅଚଳ uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (uint8_t*) p + needlelen;
}

ଶୂନ୍ୟ* erase_and_free(ଶୂନ୍ୟ *p);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ erase_and_freep(ଶୂନ୍ୟ *p) {
        erase_and_free(*(ଶୂନ୍ୟ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ erase_char(ଅକ୍ଷର *p) {
        explicit_bzero_safe(p, ଆକାର(ଅକ୍ଷର));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ଯଦି (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ପ୍ରତିଜ୍ଞା(_sz > 0);                                \
                        ପ୍ରତିଜ୍ଞା(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ଖାଲି;                                       \
        })
