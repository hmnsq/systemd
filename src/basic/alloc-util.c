/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <malloc.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"

ଶୂନ୍ୟ* memdup(ଅଚଳ ଶୂନ୍ୟ *p, size_t l) {
        ଶୂନ୍ୟ *ret;

        ପ୍ରତିଜ୍ଞା(l == 0 || p);

        ret = malloc(l ?: 1);
        ଯଦି (!ret)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ memcpy_safe(ret, p, l);
}

ଶୂନ୍ୟ* memdup_suffix0(ଅଚଳ ଶୂନ୍ୟ *p, size_t l) {
        ଶୂନ୍ୟ *ret;

        ପ୍ରତିଜ୍ଞା(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ଯଦି (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ଫେରନ୍ତୁ ଖାଲି;

        ret = malloc(l + 1);
        ଯଦି (!ret)
                ଫେରନ୍ତୁ ଖାଲି;

        ((uint8_t*) ret)[l] = 0;
        ଫେରନ୍ତୁ memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(ଶୂନ୍ୟ **xp) {
        POINTER_MAY_BE_NULL(xp);

        ଯଦି (_unlikely_(!xp || !*xp))
                ଫେରନ୍ତୁ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm ଅସ୍ଥିର("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ଯଦି (!*xp)
                assert_not_reached();
        ଫେରନ୍ତୁ sz;
}

ଶୂନ୍ୟ* expand_to_usable(ଶୂନ୍ୟ *ptr, size_t newsize _unused_) {
        ଫେରନ୍ତୁ ptr;
}

ଶୂନ୍ୟ* realloc0(ଶୂନ୍ୟ *p, size_t new_size) {
        size_t old_size;
        ଶୂନ୍ୟ *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        ଯଦି (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        ଫେରନ୍ତୁ q;
}

ଶୂନ୍ୟ* greedy_realloc(
                ଶୂନ୍ୟ **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        ଶୂନ୍ୟ *q;

        ପ୍ରତିଜ୍ଞା(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ଯଦି (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                ଫେରନ୍ତୁ *p;

        ଯଦି (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ଫେରନ୍ତୁ ଖାଲି;
        newalloc = need * 2;

        ଯଦି (!MUL_ASSIGN_SAFE(&newalloc, size))
                ଫେରନ୍ତୁ ଖାଲି;

        ଯଦି (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ *p = q;
}

ଶୂନ୍ୟ* greedy_realloc0(
                ଶୂନ୍ୟ **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        ପ୍ରତିଜ୍ଞା(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        after = MALLOC_SIZEOF_SAFE(q);

        ଯଦି (size == 0) /* avoid division by zero */
                before = 0;
        ଅନ୍ୟଥା
                before = (before / size) * size; /* Round down */

        ଯଦି (after > before)
                memzero(q + before, after - before);

        ଫେରନ୍ତୁ q;
}

ଶୂନ୍ୟ* greedy_realloc_append(
                ଶୂନ୍ୟ **p,
                size_t *n_p,
                ଅଚଳ ଶୂନ୍ୟ *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(n_p);
        ପ୍ରତିଜ୍ଞା(from || n_from == 0);

        ଯଦି (n_from > SIZE_MAX - *n_p)
                ଫେରନ୍ତୁ ଖାଲି;

        q = greedy_realloc(p, *n_p + n_from, size);
        ଯଦି (!q)
                ଫେରନ୍ତୁ ଖାଲି;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        ଫେରନ୍ତୁ q;
}
