/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <malloc.h>

#समावेश "alloc-util.h"

व्योम* memdup(स्थिर व्योम *p, माप_प्रकार l) {
        व्योम *ret;

        निश्चित(l == 0 || p);

        ret = दो_स्मृति(l ?: 1);
        अगर (!ret)
                वापस शून्य;

        वापस memcpy_safe(ret, p, l);
}

व्योम* memdup_suffix0(स्थिर व्योम *p, माप_प्रकार l) {
        व्योम *ret;

        निश्चित(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        अगर (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                वापस शून्य;

        ret = दो_स्मृति(l + 1);
        अगर (!ret)
                वापस शून्य;

        ((uint8_t*) ret)[l] = 0;
        वापस memcpy_safe(ret, p, l);
}

माप_प्रकार malloc_sizeof_safe(व्योम **xp) {
        POINTER_MAY_BE_NULL(xp);

        अगर (_unlikely_(!xp || !*xp))
                वापस 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        यंत्र अस्थिर("" : "+r"(*xp));

        माप_प्रकार sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        अगर (!*xp)
                assert_not_reached();
        वापस sz;
}

व्योम* expand_to_usable(व्योम *ptr, माप_प्रकार newsize _unused_) {
        वापस ptr;
}

व्योम* realloc0(व्योम *p, माप_प्रकार new_आकार) {
        माप_प्रकार old_आकार;
        व्योम *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_आकार = MALLOC_SIZEOF_SAFE(p);

        q = पुनः_स्मृति(p, new_आकार);
        अगर (!q)
                वापस शून्य;

        new_आकार = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        अगर (new_आकार > old_आकार)
                स_रखो((uint8_t*) q + old_आकार, 0, new_आकार - old_आकार);

        वापस q;
}

व्योम* greedy_realloc(
                व्योम **p,
                माप_प्रकार need,
                माप_प्रकार आकार) {

        माप_प्रकार newalloc;
        व्योम *q;

        निश्चित(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        अगर (*p && (आकार == 0 || (MALLOC_SIZEOF_SAFE(*p) / आकार >= need)))
                वापस *p;

        अगर (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                वापस शून्य;
        newalloc = need * 2;

        अगर (!MUL_ASSIGN_SAFE(&newalloc, आकार))
                वापस शून्य;

        अगर (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = पुनः_स्मृति(*p, newalloc);
        अगर (!q)
                वापस शून्य;

        वापस *p = q;
}

व्योम* greedy_realloc0(
                व्योम **p,
                माप_प्रकार need,
                माप_प्रकार आकार) {

        माप_प्रकार before, after;
        uint8_t *q;

        निश्चित(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, आकार);
        अगर (!q)
                वापस शून्य;

        after = MALLOC_SIZEOF_SAFE(q);

        अगर (आकार == 0) /* avoid division by zero */
                before = 0;
        अन्यथा
                before = (before / आकार) * आकार; /* Round down */

        अगर (after > before)
                memzero(q + before, after - before);

        वापस q;
}

व्योम* greedy_realloc_append(
                व्योम **p,
                माप_प्रकार *n_p,
                स्थिर व्योम *from,
                माप_प्रकार n_from,
                माप_प्रकार आकार) {

        uint8_t *q;

        निश्चित(p);
        निश्चित(n_p);
        निश्चित(from || n_from == 0);

        अगर (n_from > SIZE_MAX - *n_p)
                वापस शून्य;

        q = greedy_realloc(p, *n_p + n_from, आकार);
        अगर (!q)
                वापस शून्य;

        memcpy_safe(q + *n_p * आकार, from, n_from * आकार);

        *n_p += n_from;

        वापस q;
}
