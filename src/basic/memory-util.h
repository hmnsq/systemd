/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/mman.h> /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

#అన్తర్భావ "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t పుట_కొలత(శూన్య) శుద్ధ;
#నిర్వచించు పుట_ALIGN(l)          ALIGN_గమ్యం(l, పుట_కొలత())
#నిర్వచించు పుట_ALIGN_U64(l)      ALIGN_గమ్యం_U64(l, పుట_కొలత())
#నిర్వచించు పుట_ALIGN_కిందికి(l)     ALIGN_కిందికి(l, పుట_కొలత())
#నిర్వచించు పుట_ALIGN_కిందికి_U64(l) ALIGN_కిందికి_U64(l, పుట_కొలత())
#నిర్వచించు పుట_స్థానభ్రంశం(l)         ALIGN_స్థానభ్రంశం(l, పుట_కొలత())
#నిర్వచించు పుట_స్థానభ్రంశం_U64(l)     ALIGN_స్థానభ్రంశం_U64(l, పుట_కొలత())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
స్థిర అంతఃవరుస శూన్య* memcpy_సురక్షిత(శూన్య *dst, స్థిరరాశి శూన్య *src, size_t n) {
        ఒకవేళ (n == 0)
                తిరిగి dst;
        ప్రతిజ్ఞా(src);
        తిరిగి జ్ఞాపకం_నకలు(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
స్థిర అంతఃవరుస శూన్య* mempcpy_సురక్షిత(శూన్య *dst, స్థిరరాశి శూన్య *src, size_t n) {
        ఒకవేళ (n == 0)
                తిరిగి dst;
        ప్రతిజ్ఞా(src);
        తిరిగి mempcpy(dst, src, n);
}

#నిర్వచించు _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_సురక్షిత(&sz, పరిమాణం((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_సురక్షిత(dst, src, sz);         \
        })

#నిర్వచించు mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
స్థిర అంతఃవరుస పూర్ణాంకం memcmp_సురక్షిత(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2, size_t n) {
        ఒకవేళ (n == 0)
                తిరిగి 0;
        ప్రతిజ్ఞా(s1);
        ప్రతిజ్ఞా(s2);
        తిరిగి memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
స్థిర అంతఃవరుస పూర్ణాంకం memcmp_nn(స్థిరరాశి శూన్య *s1, size_t n1, స్థిరరాశి శూన్య *s2, size_t n2) {
        తిరిగి memcmp_సురక్షిత(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#నిర్వచించు zero(x) (memzero(&(x), పరిమాణం(x)))

స్థిర అంతఃవరుస శూన్య* mempset(శూన్య *s, పూర్ణాంకం c, size_t n) {
        memset(s, c, n);
        తిరిగి (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
స్థిర అంతఃవరుస శూన్య* memmem_సురక్షిత(స్థిరరాశి శూన్య *haystack, size_t haystacklen, స్థిరరాశి శూన్య *needle, size_t needlelen) {

        ఒకవేళ (needlelen <= 0)
                తిరిగి (శూన్య*) haystack;

        ఒకవేళ (haystacklen < needlelen)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(haystack);
        ప్రతిజ్ఞా(needle);

        తిరిగి memmem(haystack, haystacklen, needle, needlelen);
}

స్థిర అంతఃవరుస శూన్య* mempmem_సురక్షిత(స్థిరరాశి శూన్య *haystack, size_t haystacklen, స్థిరరాశి శూన్య *needle, size_t needlelen) {
        స్థిరరాశి uint8_t *p;

        p = memmem_సురక్షిత(haystack, haystacklen, needle, needlelen);
        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        తిరిగి (uint8_t*) p + needlelen;
}

శూన్య* erase_and_free(శూన్య *p);

స్థిర అంతఃవరుస శూన్య erase_and_freep(శూన్య *p) {
        erase_and_free(*(శూన్య**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
స్థిర అంతఃవరుస శూన్య erase_char(అక్షరం *p) {
        explicit_bzero_సురక్షిత(p, పరిమాణం(అక్షరం));
}

#నిర్వచించు munmap_సురక్షిత(p, sz) munmap_సురక్షిత_2(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#నిర్వచించు munmap_సురక్షిత_2(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                ఒకవేళ (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        ప్రతిజ్ఞా(_sz > 0);                                \
                        ప్రతిజ్ఞా(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) ఖాళీ;                                       \
        })
