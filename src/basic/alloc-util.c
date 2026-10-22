/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <malloc.h>

#अन्तर्भाव "alloc-util.h"

शून्य* memdup(ध्रुव शून्य *p, माप_प्रकार l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        ret = दो_स्मृति(l ?: 1);
        यदि (!ret)
                प्रत्यागम रिक्त;

        प्रत्यागम memcpy_safe(ret, p, l);
}

शून्य* memdup_suffix0(ध्रुव शून्य *p, माप_प्रकार l) {
        शून्य *ret;

        प्रतिज्ञा(l == 0 || p);

        /* The same as memdup() but place a safety NUL byte after the allocated memory */

        यदि (_unlikely_(l == SIZE_MAX)) /* prevent overflow */
                प्रत्यागम रिक्त;

        ret = दो_स्मृति(l + 1);
        यदि (!ret)
                प्रत्यागम रिक्त;

        ((uint8_t*) ret)[l] = 0;
        प्रत्यागम memcpy_safe(ret, p, l);
}

माप_प्रकार malloc_sizeof_safe(शून्य **xp) {
        POINTER_MAY_BE_NULL(xp);

        यदि (_unlikely_(!xp || !*xp))
                प्रत्यागम 0;

        /* Prevent the compiler from retaining a tighter object-size bound on *xp from a prior allocator call
         * (e.g. realloc()). This adds a simple no-op barrier after which the *xp pointer becomes clobbered,
         * which forces clang to skip fortify checks. Without this, clang's __builtin_dynamic_object_size()
         * (used with _FORTIFY_SOURCE=3) keeps the requested-size bound from realloc() and ignores
         * expand_to_usable()'s __alloc_size__, causing false-positive fortify failures when we zero the
         * malloc_usable_size() region in realloc0()/greedy_realloc0(). */
        यंत्र चञ्चल("" : "+r"(*xp));

        माप_प्रकार sz = malloc_usable_size(*xp);
        *xp = expand_to_usable(*xp, sz);
        /* GCC doesn't see the _returns_nonnull_ when built with ubsan, so yet another hint to make it doubly
         * clear that expand_to_usable won't return NULL.
         * See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79265 */
        यदि (!*xp)
                assert_not_reached();
        प्रत्यागम sz;
}

शून्य* expand_to_usable(शून्य *ptr, माप_प्रकार newsize _unused_) {
        प्रत्यागम ptr;
}

शून्य* realloc0(शून्य *p, माप_प्रकार new_size) {
        माप_प्रकार old_size;
        शून्य *q;

        /* Like realloc(), but initializes anything appended to zero */

        old_size = MALLOC_SIZEOF_SAFE(p);

        q = पुनः_स्मृति(p, new_size);
        यदि (!q)
                प्रत्यागम रिक्त;

        new_size = MALLOC_SIZEOF_SAFE(q); /* Update with actually allocated space */

        यदि (new_size > old_size)
                स_रखो((uint8_t*) q + old_size, 0, new_size - old_size);

        प्रत्यागम q;
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

        यदि (*p && (size == 0 || (MALLOC_SIZEOF_SAFE(*p) / size >= need)))
                प्रत्यागम *p;

        यदि (_unlikely_(need > SIZE_MAX/2)) /* Overflow check */
                प्रत्यागम रिक्त;
        newalloc = need * 2;

        यदि (!MUL_ASSIGN_SAFE(&newalloc, size))
                प्रत्यागम रिक्त;

        यदि (newalloc < 64) /* Allocate at least 64 bytes */
                newalloc = 64;

        q = पुनः_स्मृति(*p, newalloc);
        यदि (!q)
                प्रत्यागम रिक्त;

        प्रत्यागम *p = q;
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
        यदि (!q)
                प्रत्यागम रिक्त;

        after = MALLOC_SIZEOF_SAFE(q);

        यदि (size == 0) /* avoid division by zero */
                before = 0;
        अन्यथा
                before = (before / size) * size; /* Round down */

        यदि (after > before)
                memzero(q + before, after - before);

        प्रत्यागम q;
}

शून्य* greedy_realloc_append(
                शून्य **p,
                माप_प्रकार *n_p,
                ध्रुव शून्य *from,
                माप_प्रकार n_from,
                माप_प्रकार size) {

        uint8_t *q;

        प्रतिज्ञा(p);
        प्रतिज्ञा(n_p);
        प्रतिज्ञा(from || n_from == 0);

        यदि (n_from > SIZE_MAX - *n_p)
                प्रत्यागम रिक्त;

        q = greedy_realloc(p, *n_p + n_from, size);
        यदि (!q)
                प्रत्यागम रिक्त;

        memcpy_safe(q + *n_p * size, from, n_from * size);

        *n_p += n_from;

        प्रत्यागम q;
}
