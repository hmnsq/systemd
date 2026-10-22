/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <stdio.h>

#ضمن "alloc-util.h"
#ضمن "env-util.h"
#ضمن "escape.h"
#ضمن "extract-word.h"
#ضمن "fileio.h"
#ضمن "gunicode.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "sort-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

حرف* strv_find(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (streq(*i, name))
                        إرجاع *i;

        إرجاع عدم;
}

حرف* strv_find_case(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (strcaseeq(*i, name))
                        إرجاع *i;

        إرجاع عدم;
}

حرف* strv_find_prefix(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                إذا (startswith(*i, name))
                        إرجاع *i;

        إرجاع عدم;
}

حرف* strv_find_startswith(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                حرف *e;

                e = startswith(*i, name);
                إذا (e)
                        إرجاع e;
        }

        إرجاع عدم;
}

ساكن حرف* strv_find_closest_prefix(حرف * ثابت *l, ثابت حرف *name) {
        size_t best_distance = SIZE_MAX;
        حرف *best = عدم;

        أكد(name);

        STRV_FOREACH(s, l) {
                حرف *e = startswith(*s, name);
                إذا (!e)
                        استمر;

                size_t n = strlen(e);
                إذا (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        إرجاع best;
}

ساكن حرف* strv_find_closest_by_levenshtein(حرف * ثابت *l, ثابت حرف *name) {
        ssize_t best_distance = SSIZE_MAX;
        حرف *best = عدم;

        أكد(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                إذا (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        إرجاع عدم;
                }

                إذا (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        استمر;

                إذا (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        إرجاع best;
}

حرف* strv_find_closest(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        حرف *found = strv_find_closest_prefix(l, name);
        إذا (found)
                إرجاع found;

        إرجاع strv_find_closest_by_levenshtein(l, name);
}

حرف* strv_find_first_field(حرف * ثابت *needles, حرف * ثابت *haystack) {
        STRV_FOREACH(k, needles) {
                حرف *قيمة = strv_env_pairs_get((حرف **)haystack, *k);
                إذا (قيمة)
                        إرجاع قيمة;
        }

        إرجاع عدم;
}

حرف** strv_free(حرف **l) {
        STRV_FOREACH(k, l)
                حرر(*k);

        إرجاع mfree(l);
}

حرف** strv_free_erase(حرف **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        إرجاع mfree(l);
}

فراغ strv_free_many(حرف ***strvs, size_t n) {
        أكد(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        حرر(strvs);
}

حرف** strv_copy_n(حرف * ثابت *l, size_t n) {
        _cleanup_strv_free_ حرف **result = عدم;
        حرف **k;

        result = new(حرف*, MIN(strv_length(l), n) + 1);
        إذا (!result)
                إرجاع عدم;

        k = result;
        STRV_FOREACH(i, l) {
                إذا (n == 0)
                        اكسر;

                *k = strdup(*i);
                إذا (!*k)
                        إرجاع عدم;
                k++;

                إذا (n != SIZE_MAX)
                        n--;
        }

        *k = عدم;
        إرجاع TAKE_PTR(result);
}

صحيح strv_copy_unless_empty(حرف * ثابت *l, حرف ***ret) {
        أكد(ret);

        إذا (strv_isempty(l)) {
                *ret = عدم;
                إرجاع 0;
        }

        حرف **copy = strv_copy(l);
        إذا (!copy)
                إرجاع -ENOMEM;

        *ret = TAKE_PTR(copy);
        إرجاع 1;
}

size_t strv_length(حرف * ثابت *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        إرجاع n;
}

حرف** strv_new_ap(ثابت حرف *x, va_list ap) {
        _cleanup_strv_free_ حرف **a = عدم;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        لكل (ثابت حرف *s = x; s; s = va_arg(aq, ثابت حرف*)) {
                إذا (s == STRV_IGNORE)
                        استمر;

                n++;
        }
        va_end(aq);

        a = new(حرف*, n+1);
        إذا (!a)
                إرجاع عدم;

        لكل (ثابت حرف *s = x; s; s = va_arg(ap, ثابت حرف*)) {
                إذا (s == STRV_IGNORE)
                        استمر;

                a[i] = strdup(s);
                إذا (!a[i])
                        إرجاع عدم;

                i++;
        }

        a[i] = عدم;

        إرجاع TAKE_PTR(a);
}

حرف** strv_new_internal(ثابت حرف *x, ...) {
        حرف **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        إرجاع r;
}

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        أكد(a);

        q = strv_length(b);
        إذا (q == 0)
                إرجاع 0;

        p = strv_length(*a);
        إذا (p >= SIZE_MAX - q)
                إرجاع -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), حجم(حرف *));
        إذا (!t)
                إرجاع -ENOMEM;

        t[p] = عدم;
        *a = t;

        STRV_FOREACH(s, b) {
                إذا (filter_duplicates && strv_contains(t, *s))
                        استمر;

                t[p+i] = strdup(*s);
                إذا (!t[p+i])
                        اذهب rollback;

                i++;
                t[p+i] = عدم;
        }

        أكد(i <= q);

        إرجاع (صحيح) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = عدم;
        إرجاع -ENOMEM;
}

صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates) {
        _cleanup_strv_free_ حرف **b_consume = b;
        size_t p, q, i;

        أكد(a);

        q = strv_length(b);
        إذا (q == 0)
                إرجاع 0;

        p = strv_length(*a);
        إذا (p == 0) {
                strv_free_and_replace(*a, b_consume);

                إذا (filter_duplicates)
                        strv_uniq(*a);

                إرجاع strv_length(*a);
        }

        إذا (p >= SIZE_MAX - q)
                إرجاع -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), حجم(حرف *));
        إذا (!t)
                إرجاع -ENOMEM;

        t[p] = عدم;
        *a = t;

        إذا (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = عدم;
                i = q;
        } وإلا {
                i = 0;

                STRV_FOREACH(s, b) {
                        إذا (strv_contains(t, *s)) {
                                حرر(*s);
                                استمر;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = عدم;
                }
        }

        أكد(i <= q);

        b_consume = mfree(b_consume);

        إرجاع (صحيح) i;
}

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *prefix, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        صحيح r;

        أكد(a);

        STRV_FOREACH(s, b) {
                حرف *v;

                v = strjoin(strempty(prefix), *s, suffix);
                إذا (!v)
                        إرجاع -ENOMEM;

                r = strv_consume(a, v);
                إذا (r < 0)
                        إرجاع r;
        }

        إرجاع 0;
}

صحيح strv_split_newlines_full(حرف ***ret, ثابت حرف *s, ExtractFlags flags) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n;
        صحيح r;

        أكد(ret);
        أكد(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        إذا (r < 0)
                إرجاع r;

        n = strv_length(l);
        إذا (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        إرجاع n;
}

حرف** strv_split_newlines(ثابت حرف *s) {
        حرف **ret;

        إذا (strv_split_newlines_full(&ret, s, 0) < 0)
                إرجاع عدم;

        إرجاع ret;
}

صحيح strv_split_full(حرف ***t, ثابت حرف *s, ثابت حرف *separators, ExtractFlags flags) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n = 0;
        صحيح r;

        أكد(t);
        أكد(s);

        لكل (;;) {
                _cleanup_free_ حرف *word = عدم;

                r = extract_first_word(&s, &word, separators, flags);
                إذا (r < 0)
                        إرجاع r;
                إذا (r == 0)
                        اكسر;

                إذا (!GREEDY_REALLOC(l, n + 2))
                        إرجاع -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = عدم;
        }

        إذا (!l) {
                l = new0(حرف*, 1);
                إذا (!l)
                        إرجاع -ENOMEM;
        }

        *t = TAKE_PTR(l);

        إرجاع (صحيح) n;
}

حرف** strv_split(ثابت حرف *s, ثابت حرف *separators) {
        حرف **ret;

        إذا (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                إرجاع عدم;

        إرجاع ret;
}

صحيح strv_split_and_extend_full(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, ExtractFlags flags) {
        حرف **l;
        صحيح r;

        أكد(t);
        أكد(s);

        r = strv_split_full(&l, s, separators, flags);
        إذا (r < 0)
                إرجاع r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        إذا (r < 0)
                إرجاع r;

        إرجاع (صحيح) strv_length(*t);
}

صحيح strv_split_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates) {
        إرجاع strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

صحيح strv_split_colon_pairs(حرف ***t, ثابت حرف *s) {
        _cleanup_strv_free_ حرف **l = عدم;
        size_t n = 0;
        صحيح r;

        أكد(t);
        أكد(s);

        لكل (;;) {
                _cleanup_free_ حرف *first = عدم, *second = عدم, *tuple = عدم, *second_or_empty = عدم;

                r = extract_first_word(&s, &tuple, عدم, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                إذا (r < 0)
                        إرجاع r;
                إذا (r == 0)
                        اكسر;

                ثابت حرف *p = tuple;
                r = extract_many_كلمات(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                إذا (r < 0)
                        إرجاع r;
                إذا (r == 0)
                        استمر;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                إذا (!isempty(p))
                        إرجاع -EINVAL;

                second_or_empty = strdup(strempty(second));
                إذا (!second_or_empty)
                        إرجاع -ENOMEM;

                إذا (!GREEDY_REALLOC(l, n + 3))
                        إرجاع -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = عدم;
        }

        إذا (!l) {
                l = new0(حرف*, 1);
                إذا (!l)
                        إرجاع -ENOMEM;
        }

        *t = TAKE_PTR(l);

        إرجاع (صحيح) n;
}

حرف* strv_join_full(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *prefix) {
        حرف *r, *e;
        size_t n, k, m;

        إذا (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                إذا (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(حرف, n+1);
        إذا (!r)
                إرجاع عدم;

        e = r;
        STRV_FOREACH(s, l) {
                إذا (s != l)
                        e = stpcpy(e, separator);

                إذا (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        إرجاع r;
}

صحيح strv_ادفع_with_size(حرف ***l, size_t *n, حرف *قيمة) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        أكد(l);
        POINTER_MAY_BE_NULL(n);

        إذا (!قيمة)
                إرجاع 0;

        size_t size = n ? *n : SIZE_MAX;
        إذا (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        إذا (size > SIZE_MAX-2)
                إرجاع -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), حجم(حرف*));
        إذا (!c)
                إرجاع -ENOMEM;

        c[size] = قيمة;
        c[size+1] = عدم;

        *l = c;
        إذا (n)
                *n = size + 1;
        إرجاع 0;
}

صحيح strv_ادفع_pair(حرف ***l, حرف *a, حرف *b) {
        حرف **c;
        size_t n;

        أكد(l);

        إذا (!a && !b)
                إرجاع 0;

        n = strv_length(*l);

        /* Check for overflow */
        إذا (n > SIZE_MAX-3)
                إرجاع -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), حجم(حرف*));
        إذا (!c)
                إرجاع -ENOMEM;

        إذا (a)
                c[n++] = a;
        إذا (b)
                c[n++] = b;
        c[n] = عدم;

        *l = c;
        إرجاع 0;
}

صحيح strv_أدرج(حرف ***l, size_t position, حرف *قيمة) {
        حرف **c;
        size_t n, m;

        أكد(l);

        إذا (!قيمة)
                إرجاع 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        إذا (n > SIZE_MAX - 2)
                إرجاع -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), حجم(حرف*));
        إذا (!c)
                إرجاع -ENOMEM;

        إذا (n > position)
                memmove(c + position + 1, c + position, (n - position) * حجم(حرف*));

        c[position] = قيمة;
        c[n + 1] = عدم;

        *l = c;
        إرجاع 0;
}

صحيح strv_consume_with_size(حرف ***l, size_t *n, حرف *قيمة) {
        صحيح r;

        r = strv_ادفع_with_size(l, n, قيمة);
        إذا (r < 0)
                حرر(قيمة);

        إرجاع r;
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b) {
        صحيح r;

        r = strv_ادفع_pair(l, a, b);
        إذا (r < 0) {
                حرر(a);
                حرر(b);
        }

        إرجاع r;
}

صحيح strv_consume_prepend(حرف ***l, حرف *قيمة) {
        صحيح r;

        r = strv_ادفع_prepend(l, قيمة);
        إذا (r < 0)
                حرر(قيمة);

        إرجاع r;
}

صحيح strv_prepend(حرف ***l, ثابت حرف *قيمة) {
        حرف *v;

        إذا (!قيمة)
                إرجاع 0;

        v = strdup(قيمة);
        إذا (!v)
                إرجاع -ENOMEM;

        إرجاع strv_consume_prepend(l, v);
}

صحيح strv_extend_with_size(حرف ***l, size_t *n, ثابت حرف *قيمة) {
        حرف *v;

        إذا (!قيمة)
                إرجاع 0;

        v = strdup(قيمة);
        إذا (!v)
                إرجاع -ENOMEM;

        إرجاع strv_consume_with_size(l, n, v);
}

صحيح strv_extend_many_internal(حرف ***l, ثابت حرف *قيمة, ...) {
        va_list ap;
        size_t n, m;
        صحيح r;

        أكد(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, قيمة);
        لكل (ثابت حرف *s = قيمة; s != POINTER_MAX; s = va_arg(ap, ثابت حرف*)) {
                إذا (!s)
                        استمر;

                إذا (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        اكسر;
                }
                m++;
        }
        va_end(ap);

        إذا (r < 0)
                إرجاع r;
        إذا (m > SIZE_MAX-1)
                إرجاع -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), حجم(حرف*));
        إذا (!c)
                إرجاع -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, قيمة);
        لكل (ثابت حرف *s = قيمة; s != POINTER_MAX; s = va_arg(ap, ثابت حرف*)) {
                إذا (!s)
                        استمر;

                c[i] = strdup(s);
                إذا (!c[i]) {
                        r = -ENOMEM;
                        اكسر;
                }
                i++;
        }
        va_end(ap);

        إذا (r < 0) {
                /* rollback on error */
                لكل (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                إرجاع r;
        }

        c[i] = عدم;
        إرجاع 0;
}

حرف** strv_uniq(حرف **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        إرجاع l;
}

bool strv_is_uniq(حرف * ثابت *l) {
        STRV_FOREACH(i, l)
                إذا (strv_contains(i+1, *i))
                        إرجاع false;

        إرجاع true;
}

حرف** strv_remove(حرف **l, ثابت حرف *s) {
        إذا (!l)
                إرجاع عدم;

        أكد(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        حرف **f, **t;
        لكل (f = t = l; *f; f++)
                إذا (streq(*f, s))
                        حرر(*f);
                وإلا
                        *(t++) = *f;

        *t = عدم;
        إرجاع l;
}

حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll) {

        إذا (strv_isempty(l))
                إرجاع l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        إرجاع l;
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                إذا (strv_contains(b, *i))
                        إرجاع true;

        إرجاع false;
}

ساكن صحيح str_compare(حرف * ثابت *a, حرف * ثابت *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        إرجاع strcmp(*a, *b);
}

حرف** strv_sort(حرف **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        إرجاع l;
}

حرف** strv_sort_uniq_full(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b)) {
        أكد(compare);

        إذا (strv_isempty(l))
                إرجاع l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        حرف **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                إذا (compare(i, tail - 1) == 0)
                        حرر(*i);
                وإلا
                        *(tail++) = *i;

        *tail = عدم;
        إرجاع l;
}

حرف** strv_sort_uniq(حرف **l) {
        إرجاع strv_sort_uniq_full(l, str_compare);
}

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) {
        صحيح r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        إذا (strv_isempty(a)) {
                إذا (strv_isempty(b))
                        إرجاع 0;
                وإلا
                        إرجاع -1;
        }

        إذا (strv_isempty(b))
                إرجاع 1;

        لكل ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                إذا (r != 0)
                        إرجاع r;
        }

        إرجاع 0;
}

bool strv_equal_ignore_order(حرف * ثابت *a, حرف * ثابت *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        إذا (a == b)
                إرجاع true;

        STRV_FOREACH(i, a)
                إذا (!strv_contains(b, *i))
                        إرجاع false;

        STRV_FOREACH(i, b)
                إذا (!strv_contains(a, *i))
                        إرجاع false;

        إرجاع true;
}

فراغ strv_print_full(حرف * ثابت *l, ثابت حرف *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

صحيح strv_extendf_with_size(حرف ***l, size_t *n, ثابت حرف *format, ...) {
        va_list ap;
        حرف *x;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        إذا (r < 0)
                إرجاع -ENOMEM;

        إرجاع strv_consume_with_size(l, n, x);
}

صحيح strv_extend_joined_with_size_sentinel(حرف ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        حرف *x = strextendv_with_separator(/* x= */ عدم, /* separator= */ عدم, ap);
        va_end(ap);
        إذا (!x)
                إرجاع -ENOMEM;

        إرجاع strv_consume_with_size(l, n, x);
}

حرف* startswith_strv_internal(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *found = (حرف*) startswith(s, *i);
                إذا (found)
                        إرجاع found;
        }

        إرجاع عدم;
}

حرف* endswith_strv_internal(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *found = (حرف*) endswith(s, *i);
                إذا (found)
                        إرجاع found;
        }

        إرجاع عدم;
}

حرف** strv_reverse(حرف **l) {
        size_t n;

        n = strv_length(l);
        إذا (n <= 1)
                إرجاع l;

        لكل (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        إرجاع l;
}

حرف** strv_shell_escape(حرف **l, ثابت حرف *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                حرف *v;

                v = shell_escape(*s, bad);
                إذا (!v)
                        إرجاع عدم;

                free_and_replace(*s, v);
        }

        إرجاع l;
}

bool strv_fnmatch_full(
                حرف* ثابت* patterns,
                ثابت حرف *s,
                صحيح flags,
                size_t *ret_matched_pos) {

        أكد(s);

        إذا (patterns)
                لكل (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        إذا (fnmatch(patterns[i], s, flags) == 0) {
                                إذا (ret_matched_pos)
                                        *ret_matched_pos = i;
                                إرجاع true;
                        }

        إذا (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        إرجاع false;
}

حرف** strv_skip(حرف **l, size_t n) {
        بينما (n > 0) {
                إذا (strv_isempty(l))
                        إرجاع عدم;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        إذا (strv_isempty(l))
                إرجاع عدم;
        إرجاع l;
}

صحيح strv_extend_n(حرف ***l, ثابت حرف *قيمة, size_t n) {
        size_t i, k;
        حرف **nl;

        أكد(l);

        إذا (!قيمة)
                إرجاع 0;
        إذا (n == 0)
                إرجاع 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        إذا (n >= SIZE_MAX - k)
                إرجاع -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), حجم(حرف *));
        إذا (!nl)
                إرجاع -ENOMEM;

        *l = nl;

        لكل (i = k; i < k + n; i++) {
                nl[i] = strdup(قيمة);
                إذا (!nl[i])
                        اذهب rollback;
        }
        nl[i] = عدم;

        إرجاع 0;

rollback:
        لكل (size_t j = k; j < i; j++)
                حرر(nl[j]);
        nl[k] = عدم;

        إرجاع -ENOMEM;
}

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs) {
        حرف *j;

        أكد(l);
        أكد(lhs);

        إذا (!rhs) /* value is optional, in which case we suppress the field */
                إرجاع 0;

        j = strjoin(lhs, "=", rhs);
        إذا (!j)
                إرجاع -ENOMEM;

        إرجاع strv_consume(l, j);
}

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space) {
        bool b = false;
        صحيح r;

        أكد(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        إذا (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                إذا (r < 0)
                        إرجاع r;
        }

        إرجاع 0;
}

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        أكد(مفتاح);

        إذا (قيمة) {
                حرف **l = hashmap_get(h, مفتاح);
                إذا (!l)
                        إرجاع;

                strv_remove(l, قيمة);
                إذا (!strv_isempty(l))
                        إرجاع;
        }

        _unused_ _cleanup_free_ حرف *مفتاح_free = عدم;
        strv_free(hashmap_remove2(h, مفتاح, (فراغ**) &مفتاح_free));
}

فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), مفتاح, قيمة);
}

ساكن صحيح string_strv_hashmap_put_internal(Hashmap *h, ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        حرف **l;
        صحيح r;

        أكد(h);
        أكد(مفتاح);
        أكد(قيمة);

        l = hashmap_get(h, مفتاح);
        إذا (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                إذا (strv_contains(l, قيمة))
                        إرجاع 0;

                r = strv_extend(&l, قيمة);
                إذا (r < 0)
                        إرجاع r;

                assert_se(hashmap_update(h, مفتاح, l) >= 0);
        } وإلا {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ حرف **l2 = عدم;
                _cleanup_free_ حرف *t = عدم;

                t = strdup(مفتاح);
                إذا (!t)
                        إرجاع -ENOMEM;

                r = strv_extend(&l2, قيمة);
                إذا (r < 0)
                        إرجاع r;

                r = hashmap_put(h, t, l2);
                إذا (r < 0)
                        إرجاع r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        إرجاع 1;
}

صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        صحيح r;

        أكد(h);
        أكد(مفتاح);
        أكد(قيمة);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        إذا (r < 0)
                إرجاع r;

        إرجاع string_strv_hashmap_put_internal(*h, مفتاح, قيمة);
}

صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        صحيح r;

        أكد(h);
        أكد(مفتاح);
        أكد(قيمة);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        إذا (r < 0)
                إرجاع r;

        إرجاع string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), مفتاح, قيمة);
}

ساكن صحيح rebreak_flush_سطر(
                حرف ***broken,
                ثابت حرف *start,
                ثابت حرف *end,
                bool in_prefix,
                ثابت حرف *whitespace_begin,
                ثابت حرف *whitespace_end) {

        أكد(broken);
        أكد(start);
        أكد(end >= start);

        إذا (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                إرجاع strv_extend(broken, "");

        إذا (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ حرف *truncated = strndup(start, end - start);
        إذا (!truncated)
                إرجاع -ENOMEM;

        إرجاع strv_consume(broken, TAKE_PTR(truncated));
}

صحيح strv_rebreak_أسطر(حرف **l, size_t width, حرف ***ret) {
        _cleanup_strv_free_ حرف **broken = عدم;
        صحيح r;

        أكد(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        إذا (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                إذا (!broken)
                        إرجاع -ENOMEM;

                *ret = TAKE_PTR(broken);
                إرجاع 0;
        }

        STRV_FOREACH(i, l) {
                ثابت حرف *start = *i, *whitespace_begin = عدم, *whitespace_end = عدم;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                لكل (ثابت حرف *p = start; *p != 0; ) {
                        إذا (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_سطر(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                إذا (r < 0)
                                        إرجاع r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = عدم;
                                w = 0;
                                استمر;
                        } وإلا إذا (strchr(WHITESPACE, *p)) {
                                إذا (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = عدم;
                                }
                        } وإلا {
                                إذا (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        صحيح cw = utf8_char_console_width(p);
                        إذا (cw < 0)
                                إرجاع log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        إذا (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ حرف *truncated = عدم;

                                truncated = strndup(start, whitespace_begin - start);
                                إذا (!truncated)
                                        إرجاع -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                إذا (r < 0)
                                        إرجاع r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = عدم;
                                w = 0;
                                استمر;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                أكد(start);
                إذا (after_newline && in_prefix)
                        استمر;

                r = rebreak_flush_سطر(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                إذا (r < 0)
                        إرجاع r;
        }

        *ret = TAKE_PTR(broken);
        إرجاع 0;
}

حرف** strv_filter_prefix(حرف * ثابت *l, ثابت حرف *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        إذا (isempty(prefix))
                إرجاع strv_copy(l);

        _cleanup_strv_free_ حرف **f = عدم;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                إذا (!startswith(*i, prefix))
                        استمر;

                إذا (strv_extend_with_size(&f, &sz, *i) < 0)
                        إرجاع عدم;
        }

        إرجاع TAKE_PTR(f);
}

ثابت حرف* ثابت strv_empty[] = { عدم };
