/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <malloc.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"

ಶೂನ್ಯ* memdup(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l) {
        ಶೂನ್ಯ *ret;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || p);

        ret = ಸ್ಮರಣೆ_ಕೊಡು(l ?: 1);
        ಆದರೆ (!ret)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ memcpy_ಸುರಕ್ಷಿತ(ret, p, l);
}

ಶೂನ್ಯ* memdup_suffix0(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l) {
        ಶೂನ್ಯ *ret;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ಆದರೆ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ret = ಸ್ಮರಣೆ_ಕೊಡು(l + 1);
        ಆದರೆ (!ret)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ((uint8_t*) ret)[l] = 0;
        ಹಿಂತಿರುಗಿ memcpy_ಸುರಕ್ಷಿತ(ret, p, l);
}

size_t malloc_sizeof_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ **xp) {
        ಸೂಚಕ_MAY_BE_NULL(xp);

        ಆದರೆ (_unlikely_(!xp || !*xp))
                ಹಿಂತಿರುಗಿ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm ಅಸ್ಥಿರ("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_ಗಮ್ಯ_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ಆದರೆ (!*xp)
                assert_not_reached();
        ಹಿಂತಿರುಗಿ sz;
}

ಶೂನ್ಯ* expand_ಗಮ್ಯ_usable(ಶೂನ್ಯ *ptr, size_t newsize ಬಳಸದ) {
        ಹಿಂತಿರುಗಿ ptr;
}

ಶೂನ್ಯ* realloc0(ಶೂನ್ಯ *p, size_t new_ಗಾತ್ರ) {
        size_t old_ಗಾತ್ರ;
        ಶೂನ್ಯ *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_ಗಾತ್ರ = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(p);

        q = realloc(p, new_ಗಾತ್ರ);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        new_ಗಾತ್ರ = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(q); /* Update with actually allocated space */

        ಆದರೆ (new_ಗಾತ್ರ > old_ಗಾತ್ರ)
                memset((uint8_t*) q + old_ಗಾತ್ರ, 0, new_ಗಾತ್ರ - old_ಗಾತ್ರ);

        ಹಿಂತಿರುಗಿ q;
}

ಶೂನ್ಯ* greedy_realloc(
                ಶೂನ್ಯ **p,
                size_t need,
                size_t ಗಾತ್ರ_2) {

        size_t newalloc;
        ಶೂನ್ಯ *q;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ಆದರೆ (*p && (ಗಾತ್ರ_2 == 0 || (MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*p) / ಗಾತ್ರ_2 >= need)))
                ಹಿಂತಿರುಗಿ *p;

        ಆದರೆ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        newalloc = need * 2;

        ಆದರೆ (!MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&newalloc, ಗಾತ್ರ_2))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆದರೆ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ *p = q;
}

ಶೂನ್ಯ* greedy_realloc0(
                ಶೂನ್ಯ **p,
                size_t need,
                size_t ಗಾತ್ರ_2) {

        size_t before, after;
        uint8_t *q;

        ಪ್ರತಿಜ್ಞಾ(p);

        before = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, ಗಾತ್ರ_2);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        after = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(q);

        ಆದರೆ (ಗಾತ್ರ_2 == 0) /* avoid division by zero */
                before = 0;
        ಇಲ್ಲದಿದ್ದರೆ
                before = (before / ಗಾತ್ರ_2) * ಗಾತ್ರ_2; /* Round down */

        ಆದರೆ (after > before)
                memzero(q + before, after - before);

        ಹಿಂತಿರುಗಿ q;
}

ಶೂನ್ಯ* greedy_realloc_append(
                ಶೂನ್ಯ **p,
                size_t *n_p,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *from,
                size_t n_from,
                size_t ಗಾತ್ರ_2) {

        uint8_t *q;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(n_p);
        ಪ್ರತಿಜ್ಞಾ(from || n_from == 0);

        ಆದರೆ (n_from > SIZE_MAX - *n_p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        q = greedy_realloc(p, *n_p + n_from, ಗಾತ್ರ_2);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        memcpy_ಸುರಕ್ಷಿತ(q + *n_p * ಗಾತ್ರ_2, from, n_from * ಗಾತ್ರ_2);

        *n_p += n_from;

        ಹಿಂತಿರುಗಿ q;
}
