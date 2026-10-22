/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <malloc.h>

#अन्तर्भाव "alloc-util.h"

शून्य* memdup(ध्रुव शून्य *p, size_t l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        ret = malloc(l ?: 1);
        सचे (!ret)
                पटिनिवत्त रिक्त;

        पटिनिवत्त memcpy_safe(ret, p, l);
}

शून्य* memdup_suffix0(ध्रुव शून्य *p, size_t l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        सचे (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                पटिनिवत्त रिक्त;

        ret = malloc(l + 1);
        सचे (!ret)
                पटिनिवत्त रिक्त;

        ((uint8_t*) ret)[l] = 0;
        पटिनिवत्त memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(शून्य **xp) {
        POINTER_MAY_BE_NULL(xp);

        सचे (_unlikely_(!xp || !*xp))
                पटिनिवत्त 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm चञ्चल("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        सचे (!*xp)
                assert_not_reached();
        पटिनिवत्त sz;
}

शून्य* expand_to_usable(शून्य *ptr, size_t newsize _unused_) {
        पटिनिवत्त ptr;
}

शून्य* realloc0(शून्य *p, size_t new_size) {
        size_t old_size;
        शून्य *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        सचे (!q)
                पटिनिवत्त रिक्त;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        सचे (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        पटिनिवत्त q;
}

शून्य* greedy_realloc(
                शून्य **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        शून्य *q;

        प्रतिज्ञा(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        सचे (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                पटिनिवत्त *p;

        सचे (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                पटिनिवत्त रिक्त;
        newalloc = need * 2;

        सचे (!MUL_ASSIGN_SAFE(&newalloc, size))
                पटिनिवत्त रिक्त;

        सचे (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        सचे (!q)
                पटिनिवत्त रिक्त;

        पटिनिवत्त *p = q;
}

शून्य* greedy_realloc0(
                शून्य **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        प्रतिज्ञा(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        सचे (!q)
                पटिनिवत्त रिक्त;

        after = MALLOC_SIZEOF_SAFE(q);

        सचे (size == 0) /* avoid division by zero */
                before = 0;
        अञ्ञथा
                before = (before / size) * size; /* Round down */

        सचे (after > before)
                memzero(q + before, after - before);

        पटिनिवत्त q;
}

शून्य* greedy_realloc_append(
                शून्य **p,
                size_t *n_p,
                ध्रुव शून्य *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        प्रतिज्ञा(p);
        प्रतिज्ञा(n_p);
        प्रतिज्ञा(from || n_from == 0);

        सचे (n_from > SIZE_MAX - *n_p)
                पटिनिवत्त रिक्त;

        q = greedy_realloc(p, *n_p + n_from, size);
        सचे (!q)
                पटिनिवत्त रिक्त;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        पटिनिवत्त q;
}
