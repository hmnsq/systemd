/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <malloc.h>

#અન્તર્ભાવ "alloc-util.h"

શૂન્ય* memdup(અચલ શૂન્ય *p, size_t l) {
        શૂન્ય *ret;

        પ્રતિજ્ઞા(l == 0 || p);

        ret = malloc(l ?: 1);
        જો (!ret)
                પાછા ખાલી;

        પાછા memcpy_safe(ret, p, l);
}

શૂન્ય* memdup_suffix0(અચલ શૂન્ય *p, size_t l) {
        શૂન્ય *ret;

        પ્રતિજ્ઞા(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        જો (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                પાછા ખાલી;

        ret = malloc(l + 1);
        જો (!ret)
                પાછા ખાલી;

        ((uint8_t*) ret)[l] = 0;
        પાછા memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(શૂન્ય **xp) {
        POINTER_MAY_BE_NULL(xp);

        જો (_unlikely_(!xp || !*xp))
                પાછા 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm અસ્થિર("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        જો (!*xp)
                assert_not_reached();
        પાછા sz;
}

શૂન્ય* expand_to_usable(શૂન્ય *ptr, size_t newsize _unused_) {
        પાછા ptr;
}

શૂન્ય* realloc0(શૂન્ય *p, size_t new_size) {
        size_t old_size;
        શૂન્ય *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        જો (!q)
                પાછા ખાલી;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        જો (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        પાછા q;
}

શૂન્ય* greedy_realloc(
                શૂન્ય **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        શૂન્ય *q;

        પ્રતિજ્ઞા(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        જો (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                પાછા *p;

        જો (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                પાછા ખાલી;
        newalloc = need * 2;

        જો (!MUL_ASSIGN_SAFE(&newalloc, size))
                પાછા ખાલી;

        જો (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        જો (!q)
                પાછા ખાલી;

        પાછા *p = q;
}

શૂન્ય* greedy_realloc0(
                શૂન્ય **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        પ્રતિજ્ઞા(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        જો (!q)
                પાછા ખાલી;

        after = MALLOC_SIZEOF_SAFE(q);

        જો (size == 0) /* avoid division by zero */
                before = 0;
        નહીંતર
                before = (before / size) * size; /* Round down */

        જો (after > before)
                memzero(q + before, after - before);

        પાછા q;
}

શૂન્ય* greedy_realloc_append(
                શૂન્ય **p,
                size_t *n_p,
                અચલ શૂન્ય *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(n_p);
        પ્રતિજ્ઞા(from || n_from == 0);

        જો (n_from > SIZE_MAX - *n_p)
                પાછા ખાલી;

        q = greedy_realloc(p, *n_p + n_from, size);
        જો (!q)
                પાછા ખાલી;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        પાછા q;
}
