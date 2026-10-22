/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <malloc.h>

#அந்தர்பாவ "alloc-util.h"

வெறுமை* memdup(மாறிலி வெறுமை *p, size_t l) {
        வெறுமை *ret;

        ப்ரதிஜ்ஞா(l == 0 || p);

        ret = malloc(l ?: 1);
        எனில் (!ret)
                திரும்பு வெற்று;

        திரும்பு memcpy_safe(ret, p, l);
}

வெறுமை* memdup_suffix0(மாறிலி வெறுமை *p, size_t l) {
        வெறுமை *ret;

        ப்ரதிஜ்ஞா(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        எனில் (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                திரும்பு வெற்று;

        ret = malloc(l + 1);
        எனில் (!ret)
                திரும்பு வெற்று;

        ((uint8_t*) ret)[l] = 0;
        திரும்பு memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(வெறுமை **xp) {
        POINTER_MAY_BE_NULL(xp);

        எனில் (_unlikely_(!xp || !*xp))
                திரும்பு 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm நிலையற்ற("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        எனில் (!*xp)
                assert_not_reached();
        திரும்பு sz;
}

வெறுமை* expand_to_usable(வெறுமை *ptr, size_t newsize _unused_) {
        திரும்பு ptr;
}

வெறுமை* realloc0(வெறுமை *p, size_t new_size) {
        size_t old_size;
        வெறுமை *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        எனில் (!q)
                திரும்பு வெற்று;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        எனில் (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        திரும்பு q;
}

வெறுமை* greedy_realloc(
                வெறுமை **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        வெறுமை *q;

        ப்ரதிஜ்ஞா(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        எனில் (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                திரும்பு *p;

        எனில் (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                திரும்பு வெற்று;
        newalloc = need * 2;

        எனில் (!MUL_ASSIGN_SAFE(&newalloc, size))
                திரும்பு வெற்று;

        எனில் (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        எனில் (!q)
                திரும்பு வெற்று;

        திரும்பு *p = q;
}

வெறுமை* greedy_realloc0(
                வெறுமை **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        ப்ரதிஜ்ஞா(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        எனில் (!q)
                திரும்பு வெற்று;

        after = MALLOC_SIZEOF_SAFE(q);

        எனில் (size == 0) /* avoid division by zero */
                before = 0;
        இல்லையெனில்
                before = (before / size) * size; /* Round down */

        எனில் (after > before)
                memzero(q + before, after - before);

        திரும்பு q;
}

வெறுமை* greedy_realloc_append(
                வெறுமை **p,
                size_t *n_p,
                மாறிலி வெறுமை *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(n_p);
        ப்ரதிஜ்ஞா(from || n_from == 0);

        எனில் (n_from > SIZE_MAX - *n_p)
                திரும்பு வெற்று;

        q = greedy_realloc(p, *n_p + n_from, size);
        எனில் (!q)
                திரும்பு வெற்று;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        திரும்பு q;
}
