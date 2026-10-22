/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <malloc.h>

#شامل "alloc-util.h"

تهی* memdup(ثابت تهی *p, size_t l) {
        تهی *ret;

        أكد(l == 0 || p);

        ret = malloc(l ?: 1);
        اگر (!ret)
                بازگشت پوچ;

        بازگشت memcpy_safe(ret, p, l);
}

تهی* memdup_suffix0(ثابت تهی *p, size_t l) {
        تهی *ret;

        أكد(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        اگر (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                بازگشت پوچ;

        ret = malloc(l + 1);
        اگر (!ret)
                بازگشت پوچ;

        ((uint8_t*) ret)[l] = 0;
        بازگشت memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(تهی **xp) {
        POINTER_MAY_BE_NULL(xp);

        اگر (_unlikely_(!xp || !*xp))
                بازگشت 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm فرار("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        اگر (!*xp)
                assert_not_reached();
        بازگشت sz;
}

تهی* expand_to_usable(تهی *ptr, size_t newsize _unused_) {
        بازگشت ptr;
}

تهی* realloc0(تهی *p, size_t new_size) {
        size_t old_size;
        تهی *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        اگر (!q)
                بازگشت پوچ;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        اگر (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        بازگشت q;
}

تهی* greedy_realloc(
                تهی **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        تهی *q;

        أكد(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        اگر (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                بازگشت *p;

        اگر (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                بازگشت پوچ;
        newalloc = need * 2;

        اگر (!MUL_ASSIGN_SAFE(&newalloc, size))
                بازگشت پوچ;

        اگر (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        اگر (!q)
                بازگشت پوچ;

        بازگشت *p = q;
}

تهی* greedy_realloc0(
                تهی **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        أكد(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        اگر (!q)
                بازگشت پوچ;

        after = MALLOC_SIZEOF_SAFE(q);

        اگر (size == 0) /* avoid division by zero */
                before = 0;
        وگرنه
                before = (before / size) * size; /* Round down */

        اگر (after > before)
                memzero(q + before, after - before);

        بازگشت q;
}

تهی* greedy_realloc_append(
                تهی **p,
                size_t *n_p,
                ثابت تهی *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        أكد(p);
        أكد(n_p);
        أكد(from || n_from == 0);

        اگر (n_from > SIZE_MAX - *n_p)
                بازگشت پوچ;

        q = greedy_realloc(p, *n_p + n_from, size);
        اگر (!q)
                بازگشت پوچ;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        بازگشت q;
}
