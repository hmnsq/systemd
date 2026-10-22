/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <malloc.h>

#അന്തര്ഭാവ "alloc-util.h"

ശൂന്യം* memdup(സ്ഥിരം ശൂന്യം *p, size_t l) {
        ശൂന്യം *ret;

        പ്രതിജ്ഞാ(l == 0 || p);

        ret = malloc(l ?: 1);
        എങ്കിൽ (!ret)
                തിരികെ ശൂന്യ;

        തിരികെ memcpy_safe(ret, p, l);
}

ശൂന്യം* memdup_suffix0(സ്ഥിരം ശൂന്യം *p, size_t l) {
        ശൂന്യം *ret;

        പ്രതിജ്ഞാ(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        എങ്കിൽ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                തിരികെ ശൂന്യ;

        ret = malloc(l + 1);
        എങ്കിൽ (!ret)
                തിരികെ ശൂന്യ;

        ((uint8_t*) ret)[l] = 0;
        തിരികെ memcpy_safe(ret, p, l);
}

size_t malloc_sizeof_safe(ശൂന്യം **xp) {
        POINTER_MAY_BE_NULL(xp);

        എങ്കിൽ (_unlikely_(!xp || !*xp))
                തിരികെ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm അസ്ഥിര("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        എങ്കിൽ (!*xp)
                assert_not_reached();
        തിരികെ sz;
}

ശൂന്യം* expand_to_usable(ശൂന്യം *ptr, size_t newsize _unused_) {
        തിരികെ ptr;
}

ശൂന്യം* realloc0(ശൂന്യം *p, size_t new_size) {
        size_t old_size;
        ശൂന്യം *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = realloc(p, new_size);
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        എങ്കിൽ (new_size > old_size)
                memset((uint8_t*) q + old_size, 0, new_size - old_size);

        തിരികെ q;
}

ശൂന്യം* greedy_realloc(
                ശൂന്യം **p,
                size_t need,
                size_t size) {

        size_t newalloc;
        ശൂന്യം *q;

        പ്രതിജ്ഞാ(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        എങ്കിൽ (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                തിരികെ *p;

        എങ്കിൽ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                തിരികെ ശൂന്യ;
        newalloc = need * 2;

        എങ്കിൽ (!MUL_ASSIGN_SAFE(&newalloc, size))
                തിരികെ ശൂന്യ;

        എങ്കിൽ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        തിരികെ *p = q;
}

ശൂന്യം* greedy_realloc0(
                ശൂന്യം **p,
                size_t need,
                size_t size) {

        size_t before, after;
        uint8_t *q;

        പ്രതിജ്ഞാ(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        after = MALLOC_SIZEOF_SAFE(q);

        എങ്കിൽ (size == 0) /* avoid division by zero */
                before = 0;
        അല്ലെങ്കിൽ
                before = (before / size) * size; /* Round down */

        എങ്കിൽ (after > before)
                memzero(q + before, after - before);

        തിരികെ q;
}

ശൂന്യം* greedy_realloc_append(
                ശൂന്യം **p,
                size_t *n_p,
                സ്ഥിരം ശൂന്യം *from,
                size_t n_from,
                size_t size) {

        uint8_t *q;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(n_p);
        പ്രതിജ്ഞാ(from || n_from == 0);

        എങ്കിൽ (n_from > SIZE_MAX - *n_p)
                തിരികെ ശൂന്യ;

        q = greedy_realloc(p, *n_p + n_from, size);
        എങ്കിൽ (!q)
                തിരികെ ശൂന്യ;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        തിരികെ q;
}
