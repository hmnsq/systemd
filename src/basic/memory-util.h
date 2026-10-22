/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/mman.h> /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

#અન્તર્ભાવ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(શૂન્ય) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
સ્થિર અંતઃરેખ શૂન્ય* memcpy_safe(શૂન્ય *dst, અચલ શૂન્ય *src, size_t n) {
        જો (n == 0)
                પાછા dst;
        પ્રતિજ્ઞા(src);
        પાછા memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
સ્થિર અંતઃરેખ શૂન્ય* mempcpy_safe(શૂન્ય *dst, અચલ શૂન્ય *src, size_t n) {
        જો (n == 0)
                પાછા dst;
        પ્રતિજ્ઞા(src);
        પાછા mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, કદ((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
સ્થિર અંતઃરેખ પૂર્ણાંક memcmp_safe(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2, size_t n) {
        જો (n == 0)
                પાછા 0;
        પ્રતિજ્ઞા(s1);
        પ્રતિજ્ઞા(s2);
        પાછા memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
સ્થિર અંતઃરેખ પૂર્ણાંક memcmp_nn(અચલ શૂન્ય *s1, size_t n1, અચલ શૂન્ય *s2, size_t n2) {
        પાછા memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), કદ(x)))

સ્થિર અંતઃરેખ શૂન્ય* mempset(શૂન્ય *s, પૂર્ણાંક c, size_t n) {
        memset(s, c, n);
        પાછા (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
સ્થિર અંતઃરેખ શૂન્ય* memmem_safe(અચલ શૂન્ય *haystack, size_t haystacklen, અચલ શૂન્ય *needle, size_t needlelen) {

        જો (needlelen <= 0)
                પાછા (શૂન્ય*) haystack;

        જો (haystacklen < needlelen)
                પાછા ખાલી;

        પ્રતિજ્ઞા(haystack);
        પ્રતિજ્ઞા(needle);

        પાછા memmem(haystack, haystacklen, needle, needlelen);
}

સ્થિર અંતઃરેખ શૂન્ય* mempmem_safe(અચલ શૂન્ય *haystack, size_t haystacklen, અચલ શૂન્ય *needle, size_t needlelen) {
        અચલ uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        જો (!p)
                પાછા ખાલી;

        પાછા (uint8_t*) p + needlelen;
}

શૂન્ય* erase_and_free(શૂન્ય *p);

સ્થિર અંતઃરેખ શૂન્ય erase_and_freep(શૂન્ય *p) {
        erase_and_free(*(શૂન્ય**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
સ્થિર અંતઃરેખ શૂન્ય erase_char(અક્ષર *p) {
        explicit_bzero_safe(p, કદ(અક્ષર));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                જો (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        પ્રતિજ્ઞા(_sz > 0);                                \
                        પ્રતિજ્ઞા(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ખાલી;                                       \
        })
