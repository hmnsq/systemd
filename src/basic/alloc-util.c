/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <malloc.h>

#include "alloc-util.h"

void* memdup(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        ret = malloc(l ?: 1);
        ꦪꦼꦤ꧀ (!ret)
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ memcpy_safe(ret, p, l);
}

void* memdup_suffix0(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ꦪꦼꦤ꧀ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ꦮꦭꦶ NULL;

        ret = malloc(l + 1);
        ꦪꦼꦤ꧀ (!ret)
                ꦮꦭꦶ NULL;

        ((uint8_t*) ret)[l] = 0;
        ꦮꦭꦶ memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(void **xp) {
        POINTER_MAY_BE_NULL(xp);

        ꦪꦼꦤ꧀ (_unlikely_(!xp || !*xp))
                ꦮꦭꦶ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm ca_ncala("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ꦪꦼꦤ꧀ (!*xp)
                assert_not_reached();
        ꦮꦭꦶ sz;
}

void* expand_to_usable(void *ptr, size_t newsize _unused_) {
        ꦮꦭꦶ ptr;
}

void* realloc0(void *p, size_t new_size) {
        size_t old_size;
        void *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        ꦪꦼꦤ꧀ (!q)
                ꦮꦭꦶ NULL;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        ꦪꦼꦤ꧀ (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        ꦮꦭꦶ q;
}

void* greedy_realloc(
                void **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        void *q;

        assert(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ꦪꦼꦤ꧀ (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                ꦮꦭꦶ *p;

        ꦪꦼꦤ꧀ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ꦮꦭꦶ NULL;
        newalloc = need * 2;

        ꦪꦼꦤ꧀ (!MUL_ASSIGN_SAFE(&newalloc, size))
                ꦮꦭꦶ NULL;

        ꦪꦼꦤ꧀ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ꦪꦼꦤ꧀ (!q)
                ꦮꦭꦶ NULL;

        ꦮꦭꦶ *p = q;
}

void* greedy_realloc0(
                void **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        assert(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        ꦪꦼꦤ꧀ (!q)
                ꦮꦭꦶ NULL;

        after = MALLOC_SIZEOF_SAFE(q);

        ꦪꦼꦤ꧀ (size == 0) /* avoid division by zero */
                before = 0;
        ꦭꦶꦪ
                before = (before / size) * size; /* Round down */

        ꦪꦼꦤ꧀ (after > before)
                memzero(q + before, after - before);

        ꦮꦭꦶ q;
}

void* greedy_realloc_append(
                void **p,
                size_t *n_p,
                const void *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        assert(p);
        assert(n_p);
        assert(from || n_from == 0);

        ꦪꦼꦤ꧀ (n_from > SIZE_MAX - *n_p)
                ꦮꦭꦶ NULL;

        q = greedy_realloc(p, *n_p + n_from, size);
        ꦪꦼꦤ꧀ (!q)
                ꦮꦭꦶ NULL;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        ꦮꦭꦶ q;
}
