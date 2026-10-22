/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <malloc.h>

#include "alloc-util.h"

void* memdup(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        ret = malloc(l ?: 1);
        གལ་སྲིད (!ret)
                ལོག NULL;

        ལོག memcpy_safe(ret, p, l);
}

void* memdup_suffix0(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        གལ་སྲིད (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ལོག NULL;

        ret = malloc(l + 1);
        གལ་སྲིད (!ret)
                ལོག NULL;

        ((uint8_t*) ret)[l] = 0;
        ལོག memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(void **xp) {
        POINTER_MAY_BE_NULL(xp);

        གལ་སྲིད (_unlikely_(!xp || !*xp))
                ལོག 0;

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
        གལ་སྲིད (!*xp)
                assert_not_reached();
        ལོག sz;
}

void* expand_to_usable(void *ptr, size_t newsize _unused_) {
        ལོག ptr;
}

void* realloc0(void *p, size_t new_size) {
        size_t old_size;
        void *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        གལ་སྲིད (!q)
                ལོག NULL;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        གལ་སྲིད (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        ལོག q;
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

        གལ་སྲིད (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                ལོག *p;

        གལ་སྲིད (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ལོག NULL;
        newalloc = need * 2;

        གལ་སྲིད (!MUL_ASSIGN_SAFE(&newalloc, size))
                ལོག NULL;

        གལ་སྲིད (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        གལ་སྲིད (!q)
                ལོག NULL;

        ལོག *p = q;
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
        གལ་སྲིད (!q)
                ལོག NULL;

        after = MALLOC_SIZEOF_SAFE(q);

        གལ་སྲིད (size == 0) /* avoid division by zero */
                before = 0;
        དེ་མིན
                before = (before / size) * size; /* Round down */

        གལ་སྲིད (after > before)
                memzero(q + before, after - before);

        ལོག q;
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

        གལ་སྲིད (n_from > SIZE_MAX - *n_p)
                ལོག NULL;

        q = greedy_realloc(p, *n_p + n_from, size);
        གལ་སྲིད (!q)
                ལོག NULL;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        ལོག q;
}
