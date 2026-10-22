/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/mman.h> /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/memory-util.h" /* IWYU pragma: export */

size_t page_आकार(शून्य) विशुद्धम्;
#परिभाषय PAGE_ALIGN(l)          ALIGN_गन्तव्यम्(l, page_आकार())
#परिभाषय PAGE_ALIGN_U64(l)      ALIGN_गन्तव्यम्_U64(l, page_आकार())
#परिभाषय PAGE_ALIGN_अधस्तात्(l)     ALIGN_अधस्तात्(l, page_आकार())
#परिभाषय PAGE_ALIGN_अधस्तात्_U64(l) ALIGN_अधस्तात्_U64(l, page_आकार())
#परिभाषय PAGE_विस्थापनम्(l)         ALIGN_विस्थापनम्(l, page_आकार())
#परिभाषय PAGE_विस्थापनम्_U64(l)     ALIGN_विस्थापनम्_U64(l, page_आकार())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
स्थिर अन्तःस्थ शून्य* memcpy_safe(शून्य *dst, ध्रुव शून्य *src, size_t n) {
        यदि (n == 0)
                प्रत्यागम dst;
        प्रतिज्ञा(src);
        प्रत्यागम स्मृतिं_प्रतिलिख(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
स्थिर अन्तःस्थ शून्य* mempcpy_safe(शून्य *dst, ध्रुव शून्य *src, size_t n) {
        यदि (n == 0)
                प्रत्यागम dst;
        प्रतिज्ञा(src);
        प्रत्यागम mempcpy(dst, src, n);
}

#परिभाषय _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                size_t sz;                                              \
                assert_se(MUL_SAFE(&sz, परिमाण((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#परिभाषय mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
स्थिर अन्तःस्थ पूर्णाङ्क memcmp_safe(ध्रुव शून्य *s1, ध्रुव शून्य *s2, size_t n) {
        यदि (n == 0)
                प्रत्यागम 0;
        प्रतिज्ञा(s1);
        प्रतिज्ञा(s2);
        प्रत्यागम memcmp(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
स्थिर अन्तःस्थ पूर्णाङ्क memcmp_nn(ध्रुव शून्य *s1, size_t n1, ध्रुव शून्य *s2, size_t n2) {
        प्रत्यागम memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#परिभाषय zero(x) (memzero(&(x), परिमाण(x)))

स्थिर अन्तःस्थ शून्य* mempset(शून्य *s, पूर्णाङ्क c, size_t n) {
        स्मृतिं_पूरय(s, c, n);
        प्रत्यागम (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
स्थिर अन्तःस्थ शून्य* memmem_safe(ध्रुव शून्य *haystack, size_t haystacklen, ध्रुव शून्य *needle, size_t needlelen) {

        यदि (needlelen <= 0)
                प्रत्यागम (शून्य*) haystack;

        यदि (haystacklen < needlelen)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        प्रत्यागम memmem(haystack, haystacklen, needle, needlelen);
}

स्थिर अन्तःस्थ शून्य* mempmem_safe(ध्रुव शून्य *haystack, size_t haystacklen, ध्रुव शून्य *needle, size_t needlelen) {
        ध्रुव uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        यदि (!p)
                प्रत्यागम रिक्त;

        प्रत्यागम (uint8_t*) p + needlelen;
}

शून्य* erase_and_free(शून्य *p);

स्थिर अन्तःस्थ शून्य erase_and_freep(शून्य *p) {
        erase_and_free(*(शून्य**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
स्थिर अन्तःस्थ शून्य erase_char(वर्ण *p) {
        explicit_bzero_safe(p, परिमाण(वर्ण));
}

#परिभाषय munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#परिभाषय _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                यदि (_p && _p != MAP_FAILED) {                           \
                        size_t _sz = (sz);                              \
                        प्रतिज्ञा(_sz > 0);                                \
                        प्रतिज्ञा(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) रिक्त;                                       \
        })
