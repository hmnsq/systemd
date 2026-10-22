/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <malloc.h>

#అన్తర్భావ "alloc-util.h"

శూన్య* memdup(స్థిరరాశి శూన్య *p, size_t l) {
        శూన్య *ret;

        ప్రతిజ్ఞా(l == 0 || p);

        ret = జ్ఞాపకం_ఇవ్వు(l ?: 1);
        ఒకవేళ (!ret)
                తిరిగి ఖాళీ;

        తిరిగి memcpy_సురక్షిత(ret, p, l);
}

శూన్య* memdup_suffix0(స్థిరరాశి శూన్య *p, size_t l) {
        శూన్య *ret;

        ప్రతిజ్ఞా(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        ఒకవేళ (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                తిరిగి ఖాళీ;

        ret = జ్ఞాపకం_ఇవ్వు(l + 1);
        ఒకవేళ (!ret)
                తిరిగి ఖాళీ;

        ((uint8_t*) ret)[l] = 0;
        తిరిగి memcpy_సురక్షిత(ret, p, l);
}

size_t malloc_sizeof_సురక్షిత(శూన్య **xp) {
        సూచిక_MAY_BE_NULL(xp);

        ఒకవేళ (_unlikely_(!xp || !*xp))
                తిరిగి 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        asm అస్థిర("" : "+r"(*xp));

        size_t sz = malloc_usable_size(*xp);
        *xp = expand_గమ్యం_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        ఒకవేళ (!*xp)
                assert_not_reached();
        తిరిగి sz;
}

శూన్య* expand_గమ్యం_usable(శూన్య *ptr, size_t newsize వాడని) {
        తిరిగి ptr;
}

శూన్య* realloc0(శూన్య *p, size_t new_కొలత) {
        size_t old_కొలత;
        శూన్య *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_కొలత = MALLOC_SIZEOF_సురక్షిత(p);

        q = realloc(p, new_కొలత);
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        new_కొలత = MALLOC_SIZEOF_సురక్షిత(q); /* Update with actually allocated space */

        ఒకవేళ (new_కొలత > old_కొలత)
                memset((uint8_t*) q + old_కొలత, 0, new_కొలత - old_కొలత);

        తిరిగి q;
}

శూన్య* greedy_realloc(
                శూన్య **p,
                size_t need,
                size_t కొలత) {

        size_t newalloc;
        శూన్య *q;

        ప్రతిజ్ఞా(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        ఒకవేళ (*p && (కొలత == 0 || (MALLOC_SIZEOF_సురక్షిత(*p) / కొలత >= need)))
                తిరిగి *p;

        ఒకవేళ (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                తిరిగి ఖాళీ;
        newalloc = need * 2;

        ఒకవేళ (!MUL_కేటాయించు_సురక్షిత(&newalloc, కొలత))
                తిరిగి ఖాళీ;

        ఒకవేళ (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = realloc(*p, newalloc);
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        తిరిగి *p = q;
}

శూన్య* greedy_realloc0(
                శూన్య **p,
                size_t need,
                size_t కొలత) {

        size_t before, after;
        uint8_t *q;

        ప్రతిజ్ఞా(p);

        before = MALLOC_SIZEOF_సురక్షిత(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, కొలత);
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        after = MALLOC_SIZEOF_సురక్షిత(q);

        ఒకవేళ (కొలత == 0) /* avoid division by zero */
                before = 0;
        లేకపోతే
                before = (before / కొలత) * కొలత; /* Round down */

        ఒకవేళ (after > before)
                memzero(q + before, after - before);

        తిరిగి q;
}

శూన్య* greedy_realloc_append(
                శూన్య **p,
                size_t *n_p,
                స్థిరరాశి శూన్య *from,
                size_t n_from,
                size_t కొలత) {

        uint8_t *q;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(n_p);
        ప్రతిజ్ఞా(from || n_from == 0);

        ఒకవేళ (n_from > SIZE_MAX - *n_p)
                తిరిగి ఖాళీ;

        q = greedy_realloc(p, *n_p + n_from, కొలత);
        ఒకవేళ (!q)
                తిరిగి ఖాళీ;

        memcpy_సురక్షిత(q + *n_p * కొలత, from, n_from * కొలత);

        *n_p += n_from;

        తిరిగి q;
}
