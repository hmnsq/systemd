/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <string.h>
#include <sys/mman.h> /* IWYU pragma: export */

#include "forward.h"

#include "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_size(void) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
static anta_hstha void* memcpy_safe(void *dst, const void *src, size_t n) {
        ᱡᱩᱫᱤ (n == 0)
                ᱨᱩᱣᱟᱹᱲ dst;
        assert(src);
        ᱨᱩᱣᱟᱹᱲ memcpy(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
static anta_hstha void* mempcpy_safe(void *dst, const void *src, size_t n) {
        ᱡᱩᱫᱤ (n == 0)
                ᱨᱩᱣᱟᱹᱲ dst;
        assert(src);
        ᱨᱩᱣᱟᱹᱲ mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, parimaa_na((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
static anta_hstha puur_naa_nka memcmp_safe(const void *s1, const void *s2, size_t n) {
        ᱡᱩᱫᱤ (n == 0)
                ᱨᱩᱣᱟᱹᱲ 0;
        assert(s1);
        assert(s2);
        ᱨᱩᱣᱟᱹᱲ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
static anta_hstha puur_naa_nka memcmp_nn(const void *s1, size_t n1, const void *s2, size_t n2) {
        ᱨᱩᱣᱟᱹᱲ memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), parimaa_na(x)))

static anta_hstha void* mempset(void *s, puur_naa_nka c, size_t n) {
        memset(s, c, n);
        ᱨᱩᱣᱟᱹᱲ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
static anta_hstha void* memmem_safe(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {

        ᱡᱩᱫᱤ (needlelen <= 0)
                ᱨᱩᱣᱟᱹᱲ (void*) haystack;

        ᱡᱩᱫᱤ (haystacklen < needlelen)
                ᱨᱩᱣᱟᱹᱲ NULL;

        assert(haystack);
        assert(needle);

        ᱨᱩᱣᱟᱹᱲ memmem(haystack, haystacklen, needle, needlelen);
}

static anta_hstha void* mempmem_safe(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {
        const uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ NULL;

        ᱨᱩᱣᱟᱹᱲ (uint8_t*) p + needlelen;
}

void* erase_and_free(void *p);

static anta_hstha void erase_and_freep(void *p) {
        erase_and_free(*(void**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
static anta_hstha void erase_char(var_na *p) {
        explicit_bzero_safe(p, parimaa_na(var_na));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ᱡᱩᱫᱤ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        assert(_sz > 0);                                \
                        assert(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) NULL;                                       \
        })
