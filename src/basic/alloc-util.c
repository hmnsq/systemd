/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <malloc.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"

ਖਾਲੀ* memdup(ਅਚਲ ਖਾਲੀ *p, size_t l) {
        ਖਾਲੀ *ret;

        ਪ੍ਰਤਿਜ੍ਞਾ(l == 0 || p);

        ret = ਸਮ੍ਰਿਤੀ_ਦਿਓ(l ?: 1);
        ਜੇ (!ret)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ memcpy_ਸੁਰੱਖਿਅਤ(ret, p, l);
}

ਖਾਲੀ* memdup_suffix0(ਅਚਲ ਖਾਲੀ *p, size_t l) {
        ਖਾਲੀ *ret;

        ਪ੍ਰਤਿਜ੍ਞਾ(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ਜੇ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                ਵਾਪਸ ਨਿਰਾ;

        ret = ਸਮ੍ਰਿਤੀ_ਦਿਓ(l + 1);
        ਜੇ (!ret)
                ਵਾਪਸ ਨਿਰਾ;

        ((uint8_t*) ret)[l] = 0;
        ਵਾਪਸ memcpy_ਸੁਰੱਖਿਅਤ(ret, p, l);
}

size_t malloc_sizeof_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ **xp) {
        ਸੂਚਕ_MAY_BE_NULL(xp);

        ਜੇ (_unlikely_(!xp || !*xp))
                ਵਾਪਸ 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm ਅਸਥਿਰ("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_ਮੰਜ਼ਿਲ_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ਜੇ (!*xp)
                assert_not_reached();
        ਵਾਪਸ sz;
}

ਖਾਲੀ* expand_ਮੰਜ਼ਿਲ_usable(ਖਾਲੀ *ptr, size_t newsize ਅਣਵਰਤਿਆ) {
        ਵਾਪਸ ptr;
}

ਖਾਲੀ* realloc0(ਖਾਲੀ *p, size_t new_ਮਾਪ) {
        size_t old_ਮਾਪ;
        ਖਾਲੀ *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_ਮਾਪ = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(p);

        q = realloc(p, new_ਮਾਪ);
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        new_ਮਾਪ = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(q); /* Update with actually allocated space */

        ਜੇ (new_ਮਾਪ > old_ਮਾਪ)
                memset((uint8_t*) q + old_ਮਾਪ, 0, new_ਮਾਪ - old_ਮਾਪ);

        ਵਾਪਸ q;
}

ਖਾਲੀ* greedy_realloc(
                ਖਾਲੀ **p,
                size_t need,
                size_t ਮਾਪ) {

        size_t newalloc;
        ਖਾਲੀ *q;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ਜੇ (*p && (ਮਾਪ == 0 || (MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*p) / ਮਾਪ >= need)))
                ਵਾਪਸ *p;

        ਜੇ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                ਵਾਪਸ ਨਿਰਾ;
        newalloc = need * 2;

        ਜੇ (!MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&newalloc, ਮਾਪ))
                ਵਾਪਸ ਨਿਰਾ;

        ਜੇ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ *p = q;
}

ਖਾਲੀ* greedy_realloc0(
                ਖਾਲੀ **p,
                size_t need,
                size_t ਮਾਪ) {

        size_t before, after;
        uint8_t *q;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        before = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, ਮਾਪ);
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        after = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(q);

        ਜੇ (ਮਾਪ == 0) /* avoid division by zero */
                before = 0;
        ਨਹੀਂਤਾਂ
                before = (before / ਮਾਪ) * ਮਾਪ; /* Round down */

        ਜੇ (after > before)
                memzero(q + before, after - before);

        ਵਾਪਸ q;
}

ਖਾਲੀ* greedy_realloc_append(
                ਖਾਲੀ **p,
                size_t *n_p,
                ਅਚਲ ਖਾਲੀ *from,
                size_t n_from,
                size_t ਮਾਪ) {

        uint8_t *q;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(n_p);
        ਪ੍ਰਤਿਜ੍ਞਾ(from || n_from == 0);

        ਜੇ (n_from > SIZE_MAX - *n_p)
                ਵਾਪਸ ਨਿਰਾ;

        q = greedy_realloc(p, *n_p + n_from, ਮਾਪ);
        ਜੇ (!q)
                ਵਾਪਸ ਨਿਰਾ;

        memcpy_ਸੁਰੱਖਿਅਤ(q + *n_p * ਮਾਪ, from, n_from * ਮਾਪ);

        *n_p += n_from;

        ਵਾਪਸ q;
}
