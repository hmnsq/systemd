/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <malloc.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"

ಶೂನ್ಯ* memdup(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l) {
        ಶೂನ್ಯ *ret;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || p);

        ret = malloc(l ?: 1);
        ಯದಿ (!ret)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ memcpy_safe(ret, p, l);
}

ಶೂನ್ಯ* memdup_suffix0(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l) {
        ಶೂನ್ಯ *ret;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ಯದಿ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ret = malloc(l + 1);
        ಯದಿ (!ret)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ((uint8_t*) ret)[l] = 0;
        ಪ್ರತ್ಯಾಗಮ memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(ಶೂನ್ಯ **xp) {
        POINTER_MAY_BE_NULL(xp);

        ಯದಿ (_unlikely_(!xp || !*xp))
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm ಚಞ್ಚಲ("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ಯದಿ (!*xp)
                assert_not_reached();
        ಪ್ರತ್ಯಾಗಮ sz;
}

ಶೂನ್ಯ* expand_to_usable(ಶೂನ್ಯ *ptr, size_t newsize _unused_) {
        ಪ್ರತ್ಯಾಗಮ ptr;
}

ಶೂನ್ಯ* realloc0(ಶೂನ್ಯ *p, size_t new_size) {
        size_t old_size;
        ಶೂನ್ಯ *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        ಯದಿ (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        ಪ್ರತ್ಯಾಗಮ q;
}

ಶೂನ್ಯ* greedy_realloc(
                ಶೂನ್ಯ **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        ಶೂನ್ಯ *q;

        ಪ್ರತಿಜ್ಞಾ(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ಯದಿ (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                ಪ್ರತ್ಯಾಗಮ *p;

        ಯದಿ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        newalloc = need * 2;

        ಯದಿ (!MUL_ASSIGN_SAFE(&newalloc, size))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಯದಿ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ *p = q;
}

ಶೂನ್ಯ* greedy_realloc0(
                ಶೂನ್ಯ **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        ಪ್ರತಿಜ್ಞಾ(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        after = MALLOC_SIZEOF_SAFE(q);

        ಯದಿ (size == 0) /* avoid division by zero */
                before = 0;
        ಅನ್ಯಥಾ
                before = (before / size) * size; /* Round down */

        ಯದಿ (after > before)
                memzero(q + before, after - before);

        ಪ್ರತ್ಯಾಗಮ q;
}

ಶೂನ್ಯ* greedy_realloc_append(
                ಶೂನ್ಯ **p,
                size_t *n_p,
                ಧ್ರುವ ಶೂನ್ಯ *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(n_p);
        ಪ್ರತಿಜ್ಞಾ(from || n_from == 0);

        ಯದಿ (n_from > SIZE_MAX - *n_p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        q = greedy_realloc(p, *n_p + n_from, size);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        ಪ್ರತ್ಯಾಗಮ q;
}
