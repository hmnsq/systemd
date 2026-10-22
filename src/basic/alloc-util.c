/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <malloc.h>

#অন্তর্ভাব "alloc-util.h"

শূন্য* memdup(ধ্রুবক শূন্য *p, size_t l) {
        শূন্য *ret;

        প্রতিজ্ঞা(l == 0 || p);

        ret = malloc(l ?: 1);
        যদি (!ret)
                ফেরত ফাঁকা;

        ফেরত memcpy_safe(ret, p, l);
}

শূন্য* memdup_suffix0(ধ্রুবক শূন্য *p, size_t l) {
        শূন্য *ret;

        প্রতিজ্ঞা(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        যদি (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ফেরত ফাঁকা;

        ret = malloc(l + 1);
        যদি (!ret)
                ফেরত ফাঁকা;

        ((uint8_t*) ret)[l] = 0;
        ফেরত memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(শূন্য **xp) {
        POINTER_MAY_BE_NULL(xp);

        যদি (_unlikely_(!xp || !*xp))
                ফেরত 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm অস্থির("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        যদি (!*xp)
                assert_not_reached();
        ফেরত sz;
}

শূন্য* expand_to_usable(শূন্য *ptr, size_t newsize _unused_) {
        ফেরত ptr;
}

শূন্য* realloc0(শূন্য *p, size_t new_আকার) {
        size_t old_আকার;
        শূন্য *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_আকার = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_আকার);
        যদি (!q)
                ফেরত ফাঁকা;

        new_আকার = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        যদি (new_আকার > old_আকার)
                memset((uint8_t*) q + old_আকার, 0, new_আকার - old_আকার);

        ফেরত q;
}

শূন্য* greedy_realloc(
                শূন্য **p,
                size_t need,
                size_t আকার_2) {

        size_t newalloc;
        শূন্য *q;

        প্রতিজ্ঞা(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        যদি (*p && (আকার_2 == 0 || (MALLOC_SIZEOF_SAFE(*p) / আকার_2 >= need)))
                ফেরত *p;

        যদি (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ফেরত ফাঁকা;
        newalloc = need * 2;

        যদি (!MUL_ASSIGN_SAFE(&newalloc, আকার_2))
                ফেরত ফাঁকা;

        যদি (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        যদি (!q)
                ফেরত ফাঁকা;

        ফেরত *p = q;
}

শূন্য* greedy_realloc0(
                শূন্য **p,
                size_t need,
                size_t আকার_2) {

        size_t before, after;
        uint8_t *q;

        প্রতিজ্ঞা(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, আকার_2);
        যদি (!q)
                ফেরত ফাঁকা;

        after = MALLOC_SIZEOF_SAFE(q);

        যদি (আকার_2 == 0) /* avoid division by zero */
                before = 0;
        অন্যথা
                before = (before / আকার_2) * আকার_2; /* Round down */

        যদি (after > before)
                memzero(q + before, after - before);

        ফেরত q;
}

শূন্য* greedy_realloc_append(
                শূন্য **p,
                size_t *n_p,
                ধ্রুবক শূন্য *from,
                size_t n_from,
                size_t আকার_2) {

        uint8_t *q;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(n_p);
        প্রতিজ্ঞা(from || n_from == 0);

        যদি (n_from > SIZE_MAX - *n_p)
                ফেরত ফাঁকা;

        q = greedy_realloc(p, *n_p + n_from, আকার_2);
        যদি (!q)
                ফেরত ফাঁকা;

        memcpy_safe(q + *n_p * আকার_2, from, n_from * আকার_2);

        *n_p += n_from;

        ফেরত q;
}
