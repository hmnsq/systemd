/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <malloc.h>

#ضمن "alloc-util.h"

فراغ* memdup(ثابت فراغ *p, size_t l) {
        فراغ *ret;

        أكد(l == 0 || p);

        ret = malloc(l ?: 1);
        𐡄𐡍 (!ret)
                أرجع عدم;

        أرجع memcpy_safe(ret, p, l);
}

فراغ* memdup_suffix0(ثابت فراغ *p, size_t l) {
        فراغ *ret;

        أكد(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        𐡄𐡍 (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                أرجع عدم;

        ret = malloc(l + 1);
        𐡄𐡍 (!ret)
                أرجع عدم;

        ((uint8_t*) ret)[l] = 0;
        أرجع memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(فراغ **xp) {
        POINTER_MAY_BE_NULL(xp);

        𐡄𐡍 (_unlikely_(!xp || !*xp))
                أرجع 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm متطاير("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        𐡄𐡍 (!*xp)
                assert_not_reached();
        أرجع sz;
}

فراغ* expand_to_usable(فراغ *ptr, size_t newsize _unused_) {
        أرجع ptr;
}

فراغ* realloc0(فراغ *p, size_t new_size) {
        size_t old_size;
        فراغ *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        𐡄𐡍 (!q)
                أرجع عدم;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        𐡄𐡍 (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        أرجع q;
}

فراغ* greedy_realloc(
                فراغ **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        فراغ *q;

        أكد(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        𐡄𐡍 (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                أرجع *p;

        𐡄𐡍 (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                أرجع عدم;
        newalloc = need * 2;

        𐡄𐡍 (!MUL_ASSIGN_SAFE(&newalloc, size))
                أرجع عدم;

        𐡄𐡍 (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        𐡄𐡍 (!q)
                أرجع عدم;

        أرجع *p = q;
}

فراغ* greedy_realloc0(
                فراغ **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        أكد(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        𐡄𐡍 (!q)
                أرجع عدم;

        after = MALLOC_SIZEOF_SAFE(q);

        𐡄𐡍 (size == 0) /* avoid division by zero */
                before = 0;
        وإلا
                before = (before / size) * size; /* Round down */

        𐡄𐡍 (after > before)
                memzero(q + before, after - before);

        أرجع q;
}

فراغ* greedy_realloc_append(
                فراغ **p,
                size_t *n_p,
                ثابت فراغ *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        أكد(p);
        أكد(n_p);
        أكد(from || n_from == 0);

        𐡄𐡍 (n_from > SIZE_MAX - *n_p)
                أرجع عدم;

        q = greedy_realloc(p, *n_p + n_from, size);
        𐡄𐡍 (!q)
                أرجع عدم;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        أرجع q;
}
