/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <malloc.h>

#अन्तर्भाव "alloc-util.h"

शून्य* memdup(स्थिरांक शून्य *p, माप_प्रकार l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        ret = दो_स्मृति(l ?: 1);
        जर (!ret)
                परत रिक्त;

        परत memcpy_safe(ret, p, l);
}

शून्य* memdup_suffix0(स्थिरांक शून्य *p, माप_प्रकार l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        जर (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                परत रिक्त;

        ret = दो_स्मृति(l + 1);
        जर (!ret)
                परत रिक्त;

        ((uint8_t*) ret)[l] = 0;
        परत memcpy_safe(ret, p, l);
}

माप_प्रकार malloc_sizeof_safe(शून्य **xp) {
        POINTER_MAY_BE_NULL(xp);

        जर (_unlikely_(!xp || !*xp))
                परत 0;

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
        जर (!*xp)
                assert_not_reached();
        परत sz;
}

शून्य* expand_to_usable(शून्य *ptr, माप_प्रकार newsize _unused_) {
        परत ptr;
}

शून्य* realloc0(शून्य *p, माप_प्रकार new_size) {
        माप_प्रकार old_size;
        शून्य *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = पुनः_स्मृति(p, new_size);
        जर (!q)
                परत रिक्त;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        जर (new_size > old_size)
                स_रखो((uint8_t*) q + old_size, 0, new_size - old_size);

        परत q;
}

शून्य* greedy_realloc(
                शून्य **p,
                माप_प्रकार need,
                माप_प्रकार size) {

        माप_प्रकार newalloc;
        शून्य *q;

        प्रतिज्ञा(p);

        /* We use malloc_usable_size() for determining the current allocated size. On all systems we care
         * about this should be safe to rely on. Should there ever arise the need to avoid relying on this we
         * can instead locally fall back to realloc() on every call, rounded up to the next exponent of 2 or
         * so. */

        जर (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                परत *p;

        जर (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                परत रिक्त;
        newalloc = need * 2;

        जर (!MUL_ASSIGN_SAFE(&newalloc, size))
                परत रिक्त;

        जर (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = पुनः_स्मृति(*p, newalloc);
        जर (!q)
                परत रिक्त;

        परत *p = q;
}

शून्य* greedy_realloc0(
                शून्य **p,
                माप_प्रकार need,
                माप_प्रकार size) {

        माप_प्रकार before, after;
        uint8_t *q;

        प्रतिज्ञा(p);

        before = MALLOC_SIZEOF_SAFE(*p); /* malloc_usable_size() will return 0 on NULL input, as per docs */

        q = greedy_realloc(p, need, size);
        जर (!q)
                परत रिक्त;

        after = MALLOC_SIZEOF_SAFE(q);

        जर (size == 0) /* avoid division by zero */
                before = 0;
        अन्यथा
                before = (before / size) * size; /* Round down */

        जर (after > before)
                memzero(q + before, after - before);

        परत q;
}

शून्य* greedy_realloc_append(
                शून्य **p,
                माप_प्रकार *n_p,
                स्थिरांक शून्य *from,
                माप_प्रकार n_from,
                माप_प्रकार size) {

        uint8_t *q;

        प्रतिज्ञा(p);
        प्रतिज्ञा(n_p);
        प्रतिज्ञा(from || n_from == 0);

        जर (n_from > SIZE_MAX - *n_p)
                परत रिक्त;

        q = greedy_realloc(p, *n_p + n_from, size);
        जर (!q)
                परत रिक्त;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        परत q;
}
