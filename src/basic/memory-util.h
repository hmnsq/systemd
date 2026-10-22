/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/mman.h> /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

#অন্তর্ভাব "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(শূন্য) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
স্থির অন্তঃস্থ শূন্য* memcpy_safe(শূন্য *dst, ধ্রুব শূন্য *src, size_t n) {
        যদি (n == 0)
                প্রত্যাগম dst;
        প্রতিজ্ঞা(src);
        প্রত্যাগম memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
স্থির অন্তঃস্থ শূন্য* mempcpy_safe(শূন্য *dst, ধ্রুব শূন্য *src, size_t n) {
        যদি (n == 0)
                প্রত্যাগম dst;
        প্রতিজ্ঞা(src);
        প্রত্যাগম mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, পরিমাণ((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
স্থির অন্তঃস্থ পূর্ণাঙ্ক memcmp_safe(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2, size_t n) {
        যদি (n == 0)
                প্রত্যাগম 0;
        প্রতিজ্ঞা(s1);
        প্রতিজ্ঞা(s2);
        প্রত্যাগম memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
স্থির অন্তঃস্থ পূর্ণাঙ্ক memcmp_nn(ধ্রুব শূন্য *s1, size_t n1, ধ্রুব শূন্য *s2, size_t n2) {
        প্রত্যাগম memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), পরিমাণ(x)))

স্থির অন্তঃস্থ শূন্য* mempset(শূন্য *s, পূর্ণাঙ্ক c, size_t n) {
        memset(s, c, n);
        প্রত্যাগম (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
স্থির অন্তঃস্থ শূন্য* memmem_safe(ধ্রুব শূন্য *haystack, size_t haystacklen, ধ্রুব শূন্য *needle, size_t needlelen) {

        যদি (needlelen <= 0)
                প্রত্যাগম (শূন্য*) haystack;

        যদি (haystacklen < needlelen)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(haystack);
        প্রতিজ্ঞা(needle);

        প্রত্যাগম memmem(haystack, haystacklen, needle, needlelen);
}

স্থির অন্তঃস্থ শূন্য* mempmem_safe(ধ্রুব শূন্য *haystack, size_t haystacklen, ধ্রুব শূন্য *needle, size_t needlelen) {
        ধ্রুব uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        যদি (!p)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম (uint8_t*) p + needlelen;
}

শূন্য* erase_and_free(শূন্য *p);

স্থির অন্তঃস্থ শূন্য erase_and_freep(শূন্য *p) {
        erase_and_free(*(শূন্য**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
স্থির অন্তঃস্থ শূন্য erase_char(বর্ণ *p) {
        explicit_bzero_safe(p, পরিমাণ(বর্ণ));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                যদি (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        প্রতিজ্ঞা(_sz > 0);                                \
                        প্রতিজ্ঞা(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) রিক্ত;                                       \
        })
