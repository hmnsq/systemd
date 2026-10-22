/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <malloc.h>

#include "alloc-util.h"

void* memdup(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        ret = malloc(l ?: 1);
        ប្រសិនបើ (!ret)
                ត្រឡប់ NULL;

        ត្រឡប់ memcpy_safe(ret, p, l);
}

void* memdup_suffix0(const void *p, size_t l) {
        void *ret;

        assert(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ប្រសិនបើ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ត្រឡប់ NULL;

        ret = malloc(l + 1);
        ប្រសិនបើ (!ret)
                ត្រឡប់ NULL;

        ((uint8_t*) ret)[l] = 0;
        ត្រឡប់ memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(void **xp) {
        POINTER_MAY_BE_NULL(xp);

        ប្រសិនបើ (_unlikely_(!xp || !*xp))
                ត្រឡប់ 0;

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
        ប្រសិនបើ (!*xp)
                assert_not_reached();
        ត្រឡប់ sz;
}

void* expand_to_usable(void *ptr, size_t newsize _unused_) {
        ត្រឡប់ ptr;
}

void* realloc0(void *p, size_t new_size) {
        size_t old_size;
        void *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        ប្រសិនបើ (!q)
                ត្រឡប់ NULL;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        ប្រសិនបើ (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        ត្រឡប់ q;
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

        ប្រសិនបើ (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                ត្រឡប់ *p;

        ប្រសិនបើ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ត្រឡប់ NULL;
        newalloc = need * 2;

        ប្រសិនបើ (!MUL_ASSIGN_SAFE(&newalloc, size))
                ត្រឡប់ NULL;

        ប្រសិនបើ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ប្រសិនបើ (!q)
                ត្រឡប់ NULL;

        ត្រឡប់ *p = q;
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
        ប្រសិនបើ (!q)
                ត្រឡប់ NULL;

        after = MALLOC_SIZEOF_SAFE(q);

        ប្រសិនបើ (size == 0) /* avoid division by zero */
                before = 0;
        បើមិនដូច្នេះ
                before = (before / size) * size; /* Round down */

        ប្រសិនបើ (after > before)
                memzero(q + before, after - before);

        ត្រឡប់ q;
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

        ប្រសិនបើ (n_from > SIZE_MAX - *n_p)
                ត្រឡប់ NULL;

        q = greedy_realloc(p, *n_p + n_from, size);
        ប្រសិនបើ (!q)
                ត្រឡប់ NULL;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        ត្រឡប់ q;
}
