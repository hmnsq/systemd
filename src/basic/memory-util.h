/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/mman.h> /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

#അന്തര്ഭാവ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(ശൂന്യം) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
സ്ഥിതം വരിയിൽ ശൂന്യം* memcpy_safe(ശൂന്യം *dst, സ്ഥിരം ശൂന്യം *src, size_t n) {
        എങ്കിൽ (n == 0)
                തിരികെ dst;
        പ്രതിജ്ഞാ(src);
        തിരികെ memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
സ്ഥിതം വരിയിൽ ശൂന്യം* mempcpy_safe(ശൂന്യം *dst, സ്ഥിരം ശൂന്യം *src, size_t n) {
        എങ്കിൽ (n == 0)
                തിരികെ dst;
        പ്രതിജ്ഞാ(src);
        തിരികെ mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, വലുപ്പം((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ memcmp_safe(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2, size_t n) {
        എങ്കിൽ (n == 0)
                തിരികെ 0;
        പ്രതിജ്ഞാ(s1);
        പ്രതിജ്ഞാ(s2);
        തിരികെ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ memcmp_nn(സ്ഥിരം ശൂന്യം *s1, size_t n1, സ്ഥിരം ശൂന്യം *s2, size_t n2) {
        തിരികെ memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), വലുപ്പം(x)))

സ്ഥിതം വരിയിൽ ശൂന്യം* mempset(ശൂന്യം *s, പൂർണ്ണസംഖ്യ c, size_t n) {
        memset(s, c, n);
        തിരികെ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
സ്ഥിതം വരിയിൽ ശൂന്യം* memmem_safe(സ്ഥിരം ശൂന്യം *haystack, size_t haystacklen, സ്ഥിരം ശൂന്യം *needle, size_t needlelen) {

        എങ്കിൽ (needlelen <= 0)
                തിരികെ (ശൂന്യം*) haystack;

        എങ്കിൽ (haystacklen < needlelen)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(haystack);
        പ്രതിജ്ഞാ(needle);

        തിരികെ memmem(haystack, haystacklen, needle, needlelen);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* mempmem_safe(സ്ഥിരം ശൂന്യം *haystack, size_t haystacklen, സ്ഥിരം ശൂന്യം *needle, size_t needlelen) {
        സ്ഥിരം uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        തിരികെ (uint8_t*) p + needlelen;
}

ശൂന്യം* erase_and_free(ശൂന്യം *p);

സ്ഥിതം വരിയിൽ ശൂന്യം erase_and_freep(ശൂന്യം *p) {
        erase_and_free(*(ശൂന്യം**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
സ്ഥിതം വരിയിൽ ശൂന്യം erase_char(അക്ഷരം *p) {
        explicit_bzero_safe(p, വലുപ്പം(അക്ഷരം));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                എങ്കിൽ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        പ്രതിജ്ഞാ(_sz > 0);                                \
                        പ്രതിജ്ഞാ(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ശൂന്യ;                                       \
        })
