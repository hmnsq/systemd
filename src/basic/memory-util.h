/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h> /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t ਪੰਨਾ_ਮਾਪ(ਖਾਲੀ) ਸ਼ੁੱਧ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ALIGN(l)          ALIGN_ਮੰਜ਼ਿਲ(l, ਪੰਨਾ_ਮਾਪ())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ALIGN_U64(l)      ALIGN_ਮੰਜ਼ਿਲ_U64(l, ਪੰਨਾ_ਮਾਪ())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ALIGN_ਥੱਲੇ(l)     ALIGN_ਥੱਲੇ(l, ਪੰਨਾ_ਮਾਪ())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ALIGN_ਥੱਲੇ_U64(l) ALIGN_ਥੱਲੇ_U64(l, ਪੰਨਾ_ਮਾਪ())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ਵਿੱਥ(l)         ALIGN_ਵਿੱਥ(l, ਪੰਨਾ_ਮਾਪ())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨਾ_ਵਿੱਥ_U64(l)     ALIGN_ਵਿੱਥ_U64(l, ਪੰਨਾ_ਮਾਪ())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* memcpy_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *dst, ਅਚਲ ਖਾਲੀ *src, size_t n) {
        ਜੇ (n == 0)
                ਵਾਪਸ dst;
        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਵਾਪਸ ਸਮ੍ਰਿਤੀ_ਨਕਲ(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* mempcpy_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ *dst, ਅਚਲ ਖਾਲੀ *src, size_t n) {
        ਜੇ (n == 0)
                ਵਾਪਸ dst;
        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਵਾਪਸ mempcpy(dst, src, n);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_ਸੁਰੱਖਿਅਤ(&sz, ਆਕਾਰ((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_ਸੁਰੱਖਿਅਤ(dst, src, sz);         \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ memcmp_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2, size_t n) {
        ਜੇ (n == 0)
                ਵਾਪਸ 0;
        ਪ੍ਰਤਿਜ੍ਞਾ(s1);
        ਪ੍ਰਤਿਜ੍ਞਾ(s2);
        ਵਾਪਸ memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ memcmp_nn(ਅਚਲ ਖਾਲੀ *s1, size_t n1, ਅਚਲ ਖਾਲੀ *s2, size_t n2) {
        ਵਾਪਸ memcmp_ਸੁਰੱਖਿਅਤ(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ zero(x) (memzero(&(x), ਆਕਾਰ(x)))

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* mempset(ਖਾਲੀ *s, ਪੂਰਨ c, size_t n) {
        memset(s, c, n);
        ਵਾਪਸ (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* memmem_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਖਾਲੀ *haystack, size_t haystacklen, ਅਚਲ ਖਾਲੀ *needle, size_t needlelen) {

        ਜੇ (needlelen <= 0)
                ਵਾਪਸ (ਖਾਲੀ*) haystack;

        ਜੇ (haystacklen < needlelen)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(haystack);
        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        ਵਾਪਸ memmem(haystack, haystacklen, needle, needlelen);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* mempmem_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਖਾਲੀ *haystack, size_t haystacklen, ਅਚਲ ਖਾਲੀ *needle, size_t needlelen) {
        ਅਚਲ uint8_t *p;

        p = memmem_ਸੁਰੱਖਿਅਤ(haystack, haystacklen, needle, needlelen);
        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (uint8_t*) p + needlelen;
}

ਖਾਲੀ* erase_and_free(ਖਾਲੀ *p);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ erase_and_freep(ਖਾਲੀ *p) {
        erase_and_free(*(ਖਾਲੀ**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ erase_char(ਅੱਖਰ *p) {
        explicit_bzero_ਸੁਰੱਖਿਅਤ(p, ਆਕਾਰ(ਅੱਖਰ));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ munmap_ਸੁਰੱਖਿਅਤ(p, sz) munmap_ਸੁਰੱਖਿਅਤ_2(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ munmap_ਸੁਰੱਖਿਅਤ_2(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ਜੇ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ਪ੍ਰਤਿਜ੍ਞਾ(_sz > 0);                                \
                        ਪ੍ਰਤਿਜ੍ਞਾ(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ਨਿਰਾ;                                       \
        })
