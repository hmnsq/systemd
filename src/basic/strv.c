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
                ܐܢ (streq(*i, name))
                        ܦܢܐ *i;

        ܦܢܐ عدم;
}

حرف* strv_find_case(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                ܐܢ (strcaseeq(*i, name))
                        ܦܢܐ *i;

        ܦܢܐ عدم;
}

حرف* strv_find_prefix(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                ܐܢ (startswith(*i, name))
                        ܦܢܐ *i;

        ܦܢܐ عدم;
}

حرف* strv_find_startswith(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                حرف *e;

                e = startswith(*i, name);
                ܐܢ (e)
                        ܦܢܐ e;
        }

        ܦܢܐ عدم;
}

ساكن حرف* strv_find_closest_prefix(حرف * ثابت *l, ثابت حرف *name) {
        size_t best_distance = SIZE_MAX;
        حرف *best = عدم;

        أكد(name);

        STRV_FOREACH(s, l) {
                حرف *e = startswith(*s, name);
                ܐܢ (!e)
                        استمر;

                size_t n = strlen(e);
                ܐܢ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ܦܢܐ best;
}

ساكن حرف* strv_find_closest_by_levenshtein(حرف * ثابت *l, ثابت حرف *name) {
        ssize_t best_distance = SSIZE_MAX;
        حرف *best = عدم;

        أكد(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                ܐܢ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ܦܢܐ عدم;
                }

                ܐܢ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        استمر;

                ܐܢ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ܦܢܐ best;
}

حرف* strv_find_closest(حرف * ثابت *l, ثابت حرف *name) {
        أكد(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        حرف *found = strv_find_closest_prefix(l, name);
        ܐܢ (found)
                ܦܢܐ found;

        ܦܢܐ strv_find_closest_by_levenshtein(l, name);
}

حرف* strv_find_first_field(حرف * ثابت *needles, حرف * ثابت *haystack) {
        STRV_FOREACH(k, needles) {
                حرف *value = strv_env_pairs_get((حرف **)haystack, *k);
                ܐܢ (value)
                        ܦܢܐ value;
        }

        ܦܢܐ عدم;
}

حرف** strv_free(حرف **l) {
        STRV_FOREACH(k, l)
                حرر(*k);

        ܦܢܐ mfree(l);
}

حرف** strv_free_erase(حرف **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ܦܢܐ mfree(l);
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
        ܐܢ (!result)
                ܦܢܐ عدم;

        k = result;
        STRV_FOREACH(i, l) {
                ܐܢ (n == 0)
                        اكسر;

                *k = strdup(*i);
                ܐܢ (!*k)
                        ܦܢܐ عدم;
                k++;

                ܐܢ (n != SIZE_MAX)
                        n--;
        }

        *k = عدم;
        ܦܢܐ TAKE_PTR(result);
}

صحيح strv_copy_unless_empty(حرف * ثابت *l, حرف ***ret) {
        أكد(ret);

        ܐܢ (strv_isempty(l)) {
                *ret = عدم;
                ܦܢܐ 0;
        }

        حرف **copy = strv_copy(l);
        ܐܢ (!copy)
                ܦܢܐ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ܦܢܐ 1;
}

size_t strv_length(حرف * ثابت *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ܦܢܐ n;
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
                ܐܢ (s == STRV_IGNORE)
                        استمر;

                n++;
        }
        va_end(aq);

        a = new(حرف*, n+1);
        ܐܢ (!a)
                ܦܢܐ عدم;

        لكل (ثابت حرف *s = x; s; s = va_arg(ap, ثابت حرف*)) {
                ܐܢ (s == STRV_IGNORE)
                        استمر;

                a[i] = strdup(s);
                ܐܢ (!a[i])
                        ܦܢܐ عدم;

                i++;
        }

        a[i] = عدم;

        ܦܢܐ TAKE_PTR(a);
}

حرف** strv_new_internal(ثابت حرف *x, ...) {
        حرف **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ܦܢܐ r;
}

صحيح strv_extend_strv(حرف ***a, حرف * ثابت *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        أكد(a);

        q = strv_length(b);
        ܐܢ (q == 0)
                ܦܢܐ 0;

        p = strv_length(*a);
        ܐܢ (p >= SIZE_MAX - q)
                ܦܢܐ -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), حجم(حرف *));
        ܐܢ (!t)
                ܦܢܐ -ENOMEM;

        t[p] = عدم;
        *a = t;

        STRV_FOREACH(s, b) {
                ܐܢ (filter_duplicates && strv_contains(t, *s))
                        استمر;

                t[p+i] = strdup(*s);
                ܐܢ (!t[p+i])
                        اذهب rollback;

                i++;
                t[p+i] = عدم;
        }

        أكد(i <= q);

        ܦܢܐ (صحيح) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = عدم;
        ܦܢܐ -ENOMEM;
}

صحيح strv_extend_strv_consume(حرف ***a, حرف **b, bool filter_duplicates) {
        _cleanup_strv_free_ حرف **b_consume = b;
        size_t p, q, i;

        أكد(a);

        q = strv_length(b);
        ܐܢ (q == 0)
                ܦܢܐ 0;

        p = strv_length(*a);
        ܐܢ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ܐܢ (filter_duplicates)
                        strv_uniq(*a);

                ܦܢܐ strv_length(*a);
        }

        ܐܢ (p >= SIZE_MAX - q)
                ܦܢܐ -ENOMEM;

        حرف **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), حجم(حرف *));
        ܐܢ (!t)
                ܦܢܐ -ENOMEM;

        t[p] = عدم;
        *a = t;

        ܐܢ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = عدم;
                i = q;
        } ܐܚܪܢܐ {
                i = 0;

                STRV_FOREACH(s, b) {
                        ܐܢ (strv_contains(t, *s)) {
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

        ܦܢܐ (صحيح) i;
}

صحيح strv_extend_strv_biconcat(حرف ***a, ثابت حرف *prefix, ثابت حرف* ثابت *b, ثابت حرف *suffix) {
        صحيح r;

        أكد(a);

        STRV_FOREACH(s, b) {
                حرف *v;

                v = strjoin(strempty(prefix), *s, suffix);
                ܐܢ (!v)
                        ܦܢܐ -ENOMEM;

                r = strv_consume(a, v);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
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
        ܐܢ (r < 0)
                ܦܢܐ r;

        n = strv_length(l);
        ܐܢ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ܦܢܐ n;
}

حرف** strv_split_newlines(ثابت حرف *s) {
        حرف **ret;

        ܐܢ (strv_split_newlines_full(&ret, s, 0) < 0)
                ܦܢܐ عدم;

        ܦܢܐ ret;
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
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                ܐܢ (!GREEDY_REALLOC(l, n + 2))
                        ܦܢܐ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = عدم;
        }

        ܐܢ (!l) {
                l = new0(حرف*, 1);
                ܐܢ (!l)
                        ܦܢܐ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ܦܢܐ (صحيح) n;
}

حرف** strv_split(ثابت حرف *s, ثابت حرف *separators) {
        حرف **ret;

        ܐܢ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ܦܢܐ عدم;

        ܦܢܐ ret;
}

صحيح strv_split_and_extend_full(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates, ExtractFlags flags) {
        حرف **l;
        صحيح r;

        أكد(t);
        أكد(s);

        r = strv_split_full(&l, s, separators, flags);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ (صحيح) strv_length(*t);
}

صحيح strv_split_and_extend(حرف ***t, ثابت حرف *s, ثابت حرف *separators, bool filter_duplicates) {
        ܦܢܐ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
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
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                ثابت حرف *p = tuple;
                r = extract_many_words(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        استمر;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ܐܢ (!isempty(p))
                        ܦܢܐ -EINVAL;

                second_or_empty = strdup(strempty(second));
                ܐܢ (!second_or_empty)
                        ܦܢܐ -ENOMEM;

                ܐܢ (!GREEDY_REALLOC(l, n + 3))
                        ܦܢܐ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = عدم;
        }

        ܐܢ (!l) {
                l = new0(حرف*, 1);
                ܐܢ (!l)
                        ܦܢܐ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ܦܢܐ (صحيح) n;
}

حرف* strv_join_full(حرف * ثابت *l, ثابت حرف *separator, ثابت حرف *prefix) {
        حرف *r, *e;
        size_t n, k, m;

        ܐܢ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                ܐܢ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(حرف, n+1);
        ܐܢ (!r)
                ܦܢܐ عدم;

        e = r;
        STRV_FOREACH(s, l) {
                ܐܢ (s != l)
                        e = stpcpy(e, separator);

                ܐܢ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ܦܢܐ r;
}

صحيح strv_push_with_size(حرف ***l, size_t *n, حرف *value) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        أكد(l);
        POINTER_MAY_BE_NULL(n);

        ܐܢ (!value)
                ܦܢܐ 0;

        size_t size = n ? *n : SIZE_MAX;
        ܐܢ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        ܐܢ (size > SIZE_MAX-2)
                ܦܢܐ -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), حجم(حرف*));
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        c[size] = value;
        c[size+1] = عدم;

        *l = c;
        ܐܢ (n)
                *n = size + 1;
        ܦܢܐ 0;
}

صحيح strv_push_pair(حرف ***l, حرف *a, حرف *b) {
        حرف **c;
        size_t n;

        أكد(l);

        ܐܢ (!a && !b)
                ܦܢܐ 0;

        n = strv_length(*l);

        /* Check for overflow */
        ܐܢ (n > SIZE_MAX-3)
                ܦܢܐ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), حجم(حرف*));
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        ܐܢ (a)
                c[n++] = a;
        ܐܢ (b)
                c[n++] = b;
        c[n] = عدم;

        *l = c;
        ܦܢܐ 0;
}

صحيح strv_insert(حرف ***l, size_t position, حرف *value) {
        حرف **c;
        size_t n, m;

        أكد(l);

        ܐܢ (!value)
                ܦܢܐ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ܐܢ (n > SIZE_MAX - 2)
                ܦܢܐ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), حجم(حرف*));
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        ܐܢ (n > position)
                memmove(c + position + 1, c + position, (n - position) * حجم(حرف*));

        c[position] = value;
        c[n + 1] = عدم;

        *l = c;
        ܦܢܐ 0;
}

صحيح strv_consume_with_size(حرف ***l, size_t *n, حرف *value) {
        صحيح r;

        r = strv_push_with_size(l, n, value);
        ܐܢ (r < 0)
                حرر(value);

        ܦܢܐ r;
}

صحيح strv_consume_pair(حرف ***l, حرف *a, حرف *b) {
        صحيح r;

        r = strv_push_pair(l, a, b);
        ܐܢ (r < 0) {
                حرر(a);
                حرر(b);
        }

        ܦܢܐ r;
}

صحيح strv_consume_prepend(حرف ***l, حرف *value) {
        صحيح r;

        r = strv_push_prepend(l, value);
        ܐܢ (r < 0)
                حرر(value);

        ܦܢܐ r;
}

صحيح strv_prepend(حرف ***l, ثابت حرف *value) {
        حرف *v;

        ܐܢ (!value)
                ܦܢܐ 0;

        v = strdup(value);
        ܐܢ (!v)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume_prepend(l, v);
}

صحيح strv_extend_with_size(حرف ***l, size_t *n, ثابت حرف *value) {
        حرف *v;

        ܐܢ (!value)
                ܦܢܐ 0;

        v = strdup(value);
        ܐܢ (!v)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume_with_size(l, n, v);
}

صحيح strv_extend_many_internal(حرف ***l, ثابت حرف *value, ...) {
        va_list ap;
        size_t n, m;
        صحيح r;

        أكد(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, value);
        لكل (ثابت حرف *s = value; s != POINTER_MAX; s = va_arg(ap, ثابت حرف*)) {
                ܐܢ (!s)
                        استمر;

                ܐܢ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        اكسر;
                }
                m++;
        }
        va_end(ap);

        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (m > SIZE_MAX-1)
                ܦܢܐ -ENOMEM;

        حرف **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), حجم(حرف*));
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, value);
        لكل (ثابت حرف *s = value; s != POINTER_MAX; s = va_arg(ap, ثابت حرف*)) {
                ܐܢ (!s)
                        استمر;

                c[i] = strdup(s);
                ܐܢ (!c[i]) {
                        r = -ENOMEM;
                        اكسر;
                }
                i++;
        }
        va_end(ap);

        ܐܢ (r < 0) {
                /* rollback on error */
                لكل (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ܦܢܐ r;
        }

        c[i] = عدم;
        ܦܢܐ 0;
}

حرف** strv_uniq(حرف **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ܦܢܐ l;
}

bool strv_is_uniq(حرف * ثابت *l) {
        STRV_FOREACH(i, l)
                ܐܢ (strv_contains(i+1, *i))
                        ܦܢܐ false;

        ܦܢܐ true;
}

حرف** strv_remove(حرف **l, ثابت حرف *s) {
        ܐܢ (!l)
                ܦܢܐ عدم;

        أكد(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        حرف **f, **t;
        لكل (f = t = l; *f; f++)
                ܐܢ (streq(*f, s))
                        حرر(*f);
                ܐܚܪܢܐ
                        *(t++) = *f;

        *t = عدم;
        ܦܢܐ l;
}

حرف** strv_remove_strv(حرف **l, حرف *ثابت*ll) {

        ܐܢ (strv_isempty(l))
                ܦܢܐ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ܦܢܐ l;
}

bool strv_overlap(حرف * ثابت *a, حرف * ثابت *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ܐܢ (strv_contains(b, *i))
                        ܦܢܐ true;

        ܦܢܐ false;
}

ساكن صحيح str_compare(حرف * ثابت *a, حرف * ثابت *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ܦܢܐ strcmp(*a, *b);
}

حرف** strv_sort(حرف **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ܦܢܐ l;
}

حرف** strv_sort_uniq_full(حرف **l, صحيح (*compare)(حرف * ثابت *a, حرف * ثابت *b)) {
        أكد(compare);

        ܐܢ (strv_isempty(l))
                ܦܢܐ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        حرف **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                ܐܢ (compare(i, tail - 1) == 0)
                        حرر(*i);
                ܐܚܪܢܐ
                        *(tail++) = *i;

        *tail = عدم;
        ܦܢܐ l;
}

حرف** strv_sort_uniq(حرف **l) {
        ܦܢܐ strv_sort_uniq_full(l, str_compare);
}

صحيح strv_compare(حرف * ثابت *a, حرف * ثابت *b) {
        صحيح r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ܐܢ (strv_isempty(a)) {
                ܐܢ (strv_isempty(b))
                        ܦܢܐ 0;
                ܐܚܪܢܐ
                        ܦܢܐ -1;
        }

        ܐܢ (strv_isempty(b))
                ܦܢܐ 1;

        لكل ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ܐܢ (r != 0)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

bool strv_equal_ignore_order(حرف * ثابت *a, حرف * ثابت *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ܐܢ (a == b)
                ܦܢܐ true;

        STRV_FOREACH(i, a)
                ܐܢ (!strv_contains(b, *i))
                        ܦܢܐ false;

        STRV_FOREACH(i, b)
                ܐܢ (!strv_contains(a, *i))
                        ܦܢܐ false;

        ܦܢܐ true;
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

        ܐܢ (r < 0)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume_with_size(l, n, x);
}

صحيح strv_extend_joined_with_size_sentinel(حرف ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        حرف *x = strextendv_with_separator(/* x= */ عدم, /* separator= */ عدم, ap);
        va_end(ap);
        ܐܢ (!x)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume_with_size(l, n, x);
}

حرف* startswith_strv_internal(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *found = (حرف*) startswith(s, *i);
                ܐܢ (found)
                        ܦܢܐ found;
        }

        ܦܢܐ عدم;
}

حرف* endswith_strv_internal(ثابت حرف *s, حرف * ثابت *l) {
        STRV_FOREACH(i, l) {
                حرف *found = (حرف*) endswith(s, *i);
                ܐܢ (found)
                        ܦܢܐ found;
        }

        ܦܢܐ عدم;
}

حرف** strv_reverse(حرف **l) {
        size_t n;

        n = strv_length(l);
        ܐܢ (n <= 1)
                ܦܢܐ l;

        لكل (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ܦܢܐ l;
}

حرف** strv_shell_escape(حرف **l, ثابت حرف *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                حرف *v;

                v = shell_escape(*s, bad);
                ܐܢ (!v)
                        ܦܢܐ عدم;

                free_and_replace(*s, v);
        }

        ܦܢܐ l;
}

bool strv_fnmatch_full(
                حرف* ثابت* patterns,
                ثابت حرف *s,
                صحيح flags,
                size_t *ret_matched_pos) {

        أكد(s);

        ܐܢ (patterns)
                لكل (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ܐܢ (fnmatch(patterns[i], s, flags) == 0) {
                                ܐܢ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ܦܢܐ true;
                        }

        ܐܢ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ܦܢܐ false;
}

حرف** strv_skip(حرف **l, size_t n) {
        ܟܡܐ (n > 0) {
                ܐܢ (strv_isempty(l))
                        ܦܢܐ عدم;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ܐܢ (strv_isempty(l))
                ܦܢܐ عدم;
        ܦܢܐ l;
}

صحيح strv_extend_n(حرف ***l, ثابت حرف *value, size_t n) {
        size_t i, k;
        حرف **nl;

        أكد(l);

        ܐܢ (!value)
                ܦܢܐ 0;
        ܐܢ (n == 0)
                ܦܢܐ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        ܐܢ (n >= SIZE_MAX - k)
                ܦܢܐ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), حجم(حرف *));
        ܐܢ (!nl)
                ܦܢܐ -ENOMEM;

        *l = nl;

        لكل (i = k; i < k + n; i++) {
                nl[i] = strdup(value);
                ܐܢ (!nl[i])
                        اذهب rollback;
        }
        nl[i] = عدم;

        ܦܢܐ 0;

rollback:
        لكل (size_t j = k; j < i; j++)
                حرر(nl[j]);
        nl[k] = عدم;

        ܦܢܐ -ENOMEM;
}

صحيح strv_extend_assignment(حرف ***l, ثابت حرف *lhs, ثابت حرف *rhs) {
        حرف *j;

        أكد(l);
        أكد(lhs);

        ܐܢ (!rhs) /* value is optional, in which case we suppress the field */
                ܦܢܐ 0;

        j = strjoin(lhs, "=", rhs);
        ܐܢ (!j)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume(l, j);
}

صحيح fputstrv(FILE *f, حرف * ثابت *l, ثابت حرف *separator, bool *space) {
        bool b = false;
        صحيح r;

        أكد(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ܐܢ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

فراغ string_strv_hashmap_remove(Hashmap *h, ثابت حرف *key, ثابت حرف *value) {
        أكد(key);

        ܐܢ (value) {
                حرف **l = hashmap_get(h, key);
                ܐܢ (!l)
                        ܦܢܐ;

                strv_remove(l, value);
                ܐܢ (!strv_isempty(l))
                        ܦܢܐ;
        }

        _unused_ _cleanup_free_ حرف *key_free = عدم;
        strv_free(hashmap_remove2(h, key, (فراغ**) &key_free));
}

فراغ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت حرف *key, ثابت حرف *value) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), key, value);
}

ساكن صحيح string_strv_hashmap_put_internal(Hashmap *h, ثابت حرف *key, ثابت حرف *value) {
        حرف **l;
        صحيح r;

        أكد(h);
        أكد(key);
        أكد(value);

        l = hashmap_get(h, key);
        ܐܢ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ܐܢ (strv_contains(l, value))
                        ܦܢܐ 0;

                r = strv_extend(&l, value);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                assert_se(hashmap_update(h, key, l) >= 0);
        } ܐܚܪܢܐ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ حرف **l2 = عدم;
                _cleanup_free_ حرف *t = عدم;

                t = strdup(key);
                ܐܢ (!t)
                        ܦܢܐ -ENOMEM;

                r = strv_extend(&l2, value);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                r = hashmap_put(h, t, l2);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ܦܢܐ 1;
}

صحيح string_strv_hashmap_put(Hashmap **h, ثابت حرف *key, ثابت حرف *value) {
        صحيح r;

        أكد(h);
        أكد(key);
        أكد(value);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ string_strv_hashmap_put_internal(*h, key, value);
}

صحيح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت حرف *key, ثابت حرف *value) {
        صحيح r;

        أكد(h);
        أكد(key);
        أكد(value);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), key, value);
}

ساكن صحيح rebreak_flush_line(
                حرف ***broken,
                ثابت حرف *start,
                ثابت حرف *end,
                bool in_prefix,
                ثابت حرف *whitespace_begin,
                ثابت حرف *whitespace_end) {

        أكد(broken);
        أكد(start);
        أكد(end >= start);

        ܐܢ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ܦܢܐ strv_extend(broken, "");

        ܐܢ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ حرف *truncated = strndup(start, end - start);
        ܐܢ (!truncated)
                ܦܢܐ -ENOMEM;

        ܦܢܐ strv_consume(broken, TAKE_PTR(truncated));
}

صحيح strv_rebreak_lines(حرف **l, size_t width, حرف ***ret) {
        _cleanup_strv_free_ حرف **broken = عدم;
        صحيح r;

        أكد(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ܐܢ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ܐܢ (!broken)
                        ܦܢܐ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ܦܢܐ 0;
        }

        STRV_FOREACH(i, l) {
                ثابت حرف *start = *i, *whitespace_begin = عدم, *whitespace_end = عدم;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                لكل (ثابت حرف *p = start; *p != 0; ) {
                        ܐܢ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_line(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                ܐܢ (r < 0)
                                        ܦܢܐ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = عدم;
                                w = 0;
                                استمر;
                        } ܐܚܪܢܐ ܐܢ (strchr(WHITESPACE, *p)) {
                                ܐܢ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = عدم;
                                }
                        } ܐܚܪܢܐ {
                                ܐܢ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        صحيح cw = utf8_char_console_width(p);
                        ܐܢ (cw < 0)
                                ܦܢܐ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ܐܢ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ حرف *truncated = عدم;

                                truncated = strndup(start, whitespace_begin - start);
                                ܐܢ (!truncated)
                                        ܦܢܐ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ܐܢ (r < 0)
                                        ܦܢܐ r;

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
                ܐܢ (after_newline && in_prefix)
                        استمر;

                r = rebreak_flush_line(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        *ret = TAKE_PTR(broken);
        ܦܢܐ 0;
}

حرف** strv_filter_prefix(حرف * ثابت *l, ثابت حرف *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ܐܢ (isempty(prefix))
                ܦܢܐ strv_copy(l);

        _cleanup_strv_free_ حرف **f = عدم;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ܐܢ (!startswith(*i, prefix))
                        استمر;

                ܐܢ (strv_extend_with_size(&f, &sz, *i) < 0)
                        ܦܢܐ عدم;
        }

        ܦܢܐ TAKE_PTR(f);
}

ثابت حرف* ثابت strv_empty[] = { عدم };
