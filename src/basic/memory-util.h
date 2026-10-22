/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/mman.h> /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

#अन्तर्भाव "../fundamental/memory-util.h" /* IWYU pragma: export */

माप_प्रकार page_size(शून्य) _pure_;
#define PAGE_ALIGN(l)          ALIGN_TO(l, page_size())
#define PAGE_ALIGN_U64(l)      ALIGN_TO_U64(l, page_size())
#define PAGE_ALIGN_DOWN(l)     ALIGN_DOWN(l, page_size())
#define PAGE_ALIGN_DOWN_U64(l) ALIGN_DOWN_U64(l, page_size())
#define PAGE_OFFSET(l)         ALIGN_OFFSET(l, page_size())
#define PAGE_OFFSET_U64(l)     ALIGN_OFFSET_U64(l, page_size())

/* Normal memcpy() requires src to be nonnull. We do nothing if n is 0. */
स्थिर ओळीत शून्य* memcpy_safe(शून्य *dst, स्थिरांक शून्य *src, माप_प्रकार n) {
        जर (n == 0)
                परत dst;
        प्रतिज्ञा(src);
        परत स_नकल(dst, src, n);
}

/* Normal mempcpy() requires src to be nonnull. We do nothing if n is 0. */
स्थिर ओळीत शून्य* mempcpy_safe(शून्य *dst, स्थिरांक शून्य *src, माप_प्रकार n) {
        जर (n == 0)
                परत dst;
        प्रतिज्ञा(src);
        परत mempcpy(dst, src, n);
}

#define _mempcpy_typesafe(dst, src, n, sz)                              \
        ({                                                              \
                माप_प्रकार sz;                                              \
                assert_se(MUL_SAFE(&sz, आकार((dst)[0]), n));          \
                (typeof((dst)[0])*) mempcpy_safe(dst, src, sz);         \
        })

#define mempcpy_typesafe(dst, src, n)                                   \
        _mempcpy_typesafe(dst, src, n, UNIQ_T(sz, UNIQ))

/* Normal memcmp() requires s1 and s2 to be nonnull. We do nothing if n is 0. */
स्थिर ओळीत पूर्णांक memcmp_safe(स्थिरांक शून्य *s1, स्थिरांक शून्य *s2, माप_प्रकार n) {
        जर (n == 0)
                परत 0;
        प्रतिज्ञा(s1);
        प्रतिज्ञा(s2);
        परत स_भेद(s1, s2, n);
}

/* Compare s1 (length n1) with s2 (length n2) in lexicographic order. */
स्थिर ओळीत पूर्णांक memcmp_nn(स्थिरांक शून्य *s1, माप_प्रकार n1, स्थिरांक शून्य *s2, माप_प्रकार n2) {
        परत memcmp_safe(s1, s2, MIN(n1, n2))
            ?: CMP(n1, n2);
}

#define zero(x) (memzero(&(x), आकार(x)))

स्थिर ओळीत शून्य* mempset(शून्य *s, पूर्णांक c, माप_प्रकार n) {
        स_रखो(s, c, n);
        परत (uint8_t*) s + n;
}

/* Normal memmem() requires haystack to be nonnull, which is annoying for zero-length buffers */
स्थिर ओळीत शून्य* memmem_safe(स्थिरांक शून्य *haystack, माप_प्रकार haystacklen, स्थिरांक शून्य *needle, माप_प्रकार needlelen) {

        जर (needlelen <= 0)
                परत (शून्य*) haystack;

        जर (haystacklen < needlelen)
                परत रिक्त;

        प्रतिज्ञा(haystack);
        प्रतिज्ञा(needle);

        परत memmem(haystack, haystacklen, needle, needlelen);
}

स्थिर ओळीत शून्य* mempmem_safe(स्थिरांक शून्य *haystack, माप_प्रकार haystacklen, स्थिरांक शून्य *needle, माप_प्रकार needlelen) {
        स्थिरांक uint8_t *p;

        p = memmem_safe(haystack, haystacklen, needle, needlelen);
        जर (!p)
                परत रिक्त;

        परत (uint8_t*) p + needlelen;
}

शून्य* erase_and_free(शून्य *p);

स्थिर ओळीत शून्य erase_and_freep(शून्य *p) {
        erase_and_free(*(शून्य**) p);
}

/* Use with _cleanup_ to erase a single 'char' when leaving scope */
स्थिर ओळीत शून्य erase_char(अक्षर *p) {
        explicit_bzero_safe(p, आकार(अक्षर));
}

#define munmap_safe(p, sz) _munmap_safe(UNIQ_T(P, UNIQ), (p), UNIQ_T(SZ, UNIQ), (sz))
#define _munmap_safe(_p, p, _sz, sz)                                    \
        ({                                                              \
                typeof(p) _p = (p);                                     \
                जर (_p && _p != MAP_FAILED) {                           \
                        माप_प्रकार _sz = (sz);                              \
                        प्रतिज्ञा(_sz > 0);                                \
                        प्रतिज्ञा(_sz != SIZE_MAX); /* catch PAGE_ALIGN() overflows */ \
                        assert_se(munmap(_p, _sz) >= 0);                \
                }                                                       \
                (typeof(p)) रिक्त;                                       \
        })
