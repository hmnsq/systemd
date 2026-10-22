/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <stdio.h>

#include "alloc-util.h"
#include "env-util.h"
#include "escape.h"
#include "extract-word.h"
#include "fileio.h"
#include "gunicode.h"
#include "hashmap.h"
#include "log.h"
#include "memory-util.h"
#include "sort-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

var_na* strv_find(var_na * const *l, const var_na *name) {
        assert(name);

        STRV_FOREACH(i, l)
                ᬬᬾᬦ᭄ (streq(*i, name))
                        ᬯᬮᬶ *i;

        ᬯᬮᬶ NULL;
}

var_na* strv_find_case(var_na * const *l, const var_na *name) {
        assert(name);

        STRV_FOREACH(i, l)
                ᬬᬾᬦ᭄ (strcaseeq(*i, name))
                        ᬯᬮᬶ *i;

        ᬯᬮᬶ NULL;
}

var_na* strv_find_prefix(var_na * const *l, const var_na *name) {
        assert(name);

        STRV_FOREACH(i, l)
                ᬬᬾᬦ᭄ (startswith(*i, name))
                        ᬯᬮᬶ *i;

        ᬯᬮᬶ NULL;
}

var_na* strv_find_startswith(var_na * const *l, const var_na *name) {
        assert(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                var_na *e;

                e = startswith(*i, name);
                ᬬᬾᬦ᭄ (e)
                        ᬯᬮᬶ e;
        }

        ᬯᬮᬶ NULL;
}

static var_na* strv_find_closest_prefix(var_na * const *l, const var_na *name) {
        size_t best_distance = SIZE_MAX;
        var_na *best = NULL;

        assert(name);

        STRV_FOREACH(s, l) {
                var_na *e = startswith(*s, name);
                ᬬᬾᬦ᭄ (!e)
                        continue;

                size_t n = strlen(e);
                ᬬᬾᬦ᭄ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ᬯᬮᬶ best;
}

static var_na* strv_find_closest_by_levenshtein(var_na * const *l, const var_na *name) {
        ssize_t best_distance = SSIZE_MAX;
        var_na *best = NULL;

        assert(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                ᬬᬾᬦ᭄ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ᬯᬮᬶ NULL;
                }

                ᬬᬾᬦ᭄ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        continue;

                ᬬᬾᬦ᭄ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ᬯᬮᬶ best;
}

var_na* strv_find_closest(var_na * const *l, const var_na *name) {
        assert(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        var_na *found = strv_find_closest_prefix(l, name);
        ᬬᬾᬦ᭄ (found)
                ᬯᬮᬶ found;

        ᬯᬮᬶ strv_find_closest_by_levenshtein(l, name);
}

var_na* strv_find_first_field(var_na * const *needles, var_na * const *haystack) {
        STRV_FOREACH(k, needles) {
                var_na *value = strv_env_pairs_get((var_na **)haystack, *k);
                ᬬᬾᬦ᭄ (value)
                        ᬯᬮᬶ value;
        }

        ᬯᬮᬶ NULL;
}

var_na** strv_free(var_na **l) {
        STRV_FOREACH(k, l)
                free(*k);

        ᬯᬮᬶ mfree(l);
}

var_na** strv_free_erase(var_na **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ᬯᬮᬶ mfree(l);
}

void strv_free_many(var_na ***strvs, size_t n) {
        assert(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        free(strvs);
}

var_na** strv_copy_n(var_na * const *l, size_t n) {
        _cleanup_strv_free_ var_na **result = NULL;
        var_na **k;

        result = new(var_na*, MIN(strv_length(l), n) + 1);
        ᬬᬾᬦ᭄ (!result)
                ᬯᬮᬶ NULL;

        k = result;
        STRV_FOREACH(i, l) {
                ᬬᬾᬦ᭄ (n == 0)
                        bha_nga;

                *k = strdup(*i);
                ᬬᬾᬦ᭄ (!*k)
                        ᬯᬮᬶ NULL;
                k++;

                ᬬᬾᬦ᭄ (n != SIZE_MAX)
                        n--;
        }

        *k = NULL;
        ᬯᬮᬶ TAKE_PTR(result);
}

puur_naa_nka strv_copy_unless_empty(var_na * const *l, var_na ***ret) {
        assert(ret);

        ᬬᬾᬦ᭄ (strv_isempty(l)) {
                *ret = NULL;
                ᬯᬮᬶ 0;
        }

        var_na **copy = strv_copy(l);
        ᬬᬾᬦ᭄ (!copy)
                ᬯᬮᬶ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ᬯᬮᬶ 1;
}

size_t strv_length(var_na * const *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ᬯᬮᬶ n;
}

var_na** strv_new_ap(const var_na *x, va_list ap) {
        _cleanup_strv_free_ var_na **a = NULL;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        for (const var_na *s = x; s; s = va_arg(aq, const var_na*)) {
                ᬬᬾᬦ᭄ (s == STRV_IGNORE)
                        continue;

                n++;
        }
        va_end(aq);

        a = new(var_na*, n+1);
        ᬬᬾᬦ᭄ (!a)
                ᬯᬮᬶ NULL;

        for (const var_na *s = x; s; s = va_arg(ap, const var_na*)) {
                ᬬᬾᬦ᭄ (s == STRV_IGNORE)
                        continue;

                a[i] = strdup(s);
                ᬬᬾᬦ᭄ (!a[i])
                        ᬯᬮᬶ NULL;

                i++;
        }

        a[i] = NULL;

        ᬯᬮᬶ TAKE_PTR(a);
}

var_na** strv_new_internal(const var_na *x, ...) {
        var_na **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ᬯᬮᬶ r;
}

puur_naa_nka strv_extend_strv(var_na ***a, var_na * const *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        assert(a);

        q = strv_length(b);
        ᬬᬾᬦ᭄ (q == 0)
                ᬯᬮᬶ 0;

        p = strv_length(*a);
        ᬬᬾᬦ᭄ (p >= SIZE_MAX - q)
                ᬯᬮᬶ -ENOMEM;

        var_na **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), parimaa_na(var_na *));
        ᬬᬾᬦ᭄ (!t)
                ᬯᬮᬶ -ENOMEM;

        t[p] = NULL;
        *a = t;

        STRV_FOREACH(s, b) {
                ᬬᬾᬦ᭄ (filter_duplicates && strv_contains(t, *s))
                        continue;

                t[p+i] = strdup(*s);
                ᬬᬾᬦ᭄ (!t[p+i])
                        goto rollback;

                i++;
                t[p+i] = NULL;
        }

        assert(i <= q);

        ᬯᬮᬶ (puur_naa_nka) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = NULL;
        ᬯᬮᬶ -ENOMEM;
}

puur_naa_nka strv_extend_strv_consume(var_na ***a, var_na **b, bool filter_duplicates) {
        _cleanup_strv_free_ var_na **b_consume = b;
        size_t p, q, i;

        assert(a);

        q = strv_length(b);
        ᬬᬾᬦ᭄ (q == 0)
                ᬯᬮᬶ 0;

        p = strv_length(*a);
        ᬬᬾᬦ᭄ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ᬬᬾᬦ᭄ (filter_duplicates)
                        strv_uniq(*a);

                ᬯᬮᬶ strv_length(*a);
        }

        ᬬᬾᬦ᭄ (p >= SIZE_MAX - q)
                ᬯᬮᬶ -ENOMEM;

        var_na **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), parimaa_na(var_na *));
        ᬬᬾᬦ᭄ (!t)
                ᬯᬮᬶ -ENOMEM;

        t[p] = NULL;
        *a = t;

        ᬬᬾᬦ᭄ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = NULL;
                i = q;
        } ᬮᬶᬬ {
                i = 0;

                STRV_FOREACH(s, b) {
                        ᬬᬾᬦ᭄ (strv_contains(t, *s)) {
                                free(*s);
                                continue;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = NULL;
                }
        }

        assert(i <= q);

        b_consume = mfree(b_consume);

        ᬯᬮᬶ (puur_naa_nka) i;
}

puur_naa_nka strv_extend_strv_biconcat(var_na ***a, const var_na *prefix, const var_na* const *b, const var_na *suffix) {
        puur_naa_nka r;

        assert(a);

        STRV_FOREACH(s, b) {
                var_na *v;

                v = strjoin(strempty(prefix), *s, suffix);
                ᬬᬾᬦ᭄ (!v)
                        ᬯᬮᬶ -ENOMEM;

                r = strv_consume(a, v);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        ᬯᬮᬶ 0;
}

puur_naa_nka strv_split_newlines_full(var_na ***ret, const var_na *s, ExtractFlags flags) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n;
        puur_naa_nka r;

        assert(ret);
        assert(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        n = strv_length(l);
        ᬬᬾᬦ᭄ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ᬯᬮᬶ n;
}

var_na** strv_split_newlines(const var_na *s) {
        var_na **ret;

        ᬬᬾᬦ᭄ (strv_split_newlines_full(&ret, s, 0) < 0)
                ᬯᬮᬶ NULL;

        ᬯᬮᬶ ret;
}

puur_naa_nka strv_split_full(var_na ***t, const var_na *s, const var_na *separators, ExtractFlags flags) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n = 0;
        puur_naa_nka r;

        assert(t);
        assert(s);

        for (;;) {
                _cleanup_free_ var_na *word = NULL;

                r = extract_first_word(&s, &word, separators, flags);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                ᬬᬾᬦ᭄ (r == 0)
                        bha_nga;

                ᬬᬾᬦ᭄ (!GREEDY_REALLOC(l, n + 2))
                        ᬯᬮᬶ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = NULL;
        }

        ᬬᬾᬦ᭄ (!l) {
                l = new0(var_na*, 1);
                ᬬᬾᬦ᭄ (!l)
                        ᬯᬮᬶ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ᬯᬮᬶ (puur_naa_nka) n;
}

var_na** strv_split(const var_na *s, const var_na *separators) {
        var_na **ret;

        ᬬᬾᬦ᭄ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ᬯᬮᬶ NULL;

        ᬯᬮᬶ ret;
}

puur_naa_nka strv_split_and_extend_full(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates, ExtractFlags flags) {
        var_na **l;
        puur_naa_nka r;

        assert(t);
        assert(s);

        r = strv_split_full(&l, s, separators, flags);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ (puur_naa_nka) strv_length(*t);
}

puur_naa_nka strv_split_and_extend(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates) {
        ᬯᬮᬶ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

puur_naa_nka strv_split_colon_pairs(var_na ***t, const var_na *s) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n = 0;
        puur_naa_nka r;

        assert(t);
        assert(s);

        for (;;) {
                _cleanup_free_ var_na *first = NULL, *second = NULL, *tuple = NULL, *second_or_empty = NULL;

                r = extract_first_word(&s, &tuple, NULL, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                ᬬᬾᬦ᭄ (r == 0)
                        bha_nga;

                const var_na *p = tuple;
                r = extract_many_words(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                ᬬᬾᬦ᭄ (r == 0)
                        continue;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ᬬᬾᬦ᭄ (!isempty(p))
                        ᬯᬮᬶ -EINVAL;

                second_or_empty = strdup(strempty(second));
                ᬬᬾᬦ᭄ (!second_or_empty)
                        ᬯᬮᬶ -ENOMEM;

                ᬬᬾᬦ᭄ (!GREEDY_REALLOC(l, n + 3))
                        ᬯᬮᬶ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = NULL;
        }

        ᬬᬾᬦ᭄ (!l) {
                l = new0(var_na*, 1);
                ᬬᬾᬦ᭄ (!l)
                        ᬯᬮᬶ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ᬯᬮᬶ (puur_naa_nka) n;
}

var_na* strv_join_full(var_na * const *l, const var_na *separator, const var_na *prefix) {
        var_na *r, *e;
        size_t n, k, m;

        ᬬᬾᬦ᭄ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                ᬬᬾᬦ᭄ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(var_na, n+1);
        ᬬᬾᬦ᭄ (!r)
                ᬯᬮᬶ NULL;

        e = r;
        STRV_FOREACH(s, l) {
                ᬬᬾᬦ᭄ (s != l)
                        e = stpcpy(e, separator);

                ᬬᬾᬦ᭄ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ᬯᬮᬶ r;
}

puur_naa_nka strv_push_with_size(var_na ***l, size_t *n, var_na *value) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        assert(l);
        POINTER_MAY_BE_NULL(n);

        ᬬᬾᬦ᭄ (!value)
                ᬯᬮᬶ 0;

        size_t size = n ? *n : SIZE_MAX;
        ᬬᬾᬦ᭄ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        ᬬᬾᬦ᭄ (size > SIZE_MAX-2)
                ᬯᬮᬶ -ENOMEM;

        var_na **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), parimaa_na(var_na*));
        ᬬᬾᬦ᭄ (!c)
                ᬯᬮᬶ -ENOMEM;

        c[size] = value;
        c[size+1] = NULL;

        *l = c;
        ᬬᬾᬦ᭄ (n)
                *n = size + 1;
        ᬯᬮᬶ 0;
}

puur_naa_nka strv_push_pair(var_na ***l, var_na *a, var_na *b) {
        var_na **c;
        size_t n;

        assert(l);

        ᬬᬾᬦ᭄ (!a && !b)
                ᬯᬮᬶ 0;

        n = strv_length(*l);

        /* Check for overflow */
        ᬬᬾᬦ᭄ (n > SIZE_MAX-3)
                ᬯᬮᬶ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), parimaa_na(var_na*));
        ᬬᬾᬦ᭄ (!c)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (a)
                c[n++] = a;
        ᬬᬾᬦ᭄ (b)
                c[n++] = b;
        c[n] = NULL;

        *l = c;
        ᬯᬮᬶ 0;
}

puur_naa_nka strv_insert(var_na ***l, size_t position, var_na *value) {
        var_na **c;
        size_t n, m;

        assert(l);

        ᬬᬾᬦ᭄ (!value)
                ᬯᬮᬶ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ᬬᬾᬦ᭄ (n > SIZE_MAX - 2)
                ᬯᬮᬶ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), parimaa_na(var_na*));
        ᬬᬾᬦ᭄ (!c)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (n > position)
                memmove(c + position + 1, c + position, (n - position) * parimaa_na(var_na*));

        c[position] = value;
        c[n + 1] = NULL;

        *l = c;
        ᬯᬮᬶ 0;
}

puur_naa_nka strv_consume_with_size(var_na ***l, size_t *n, var_na *value) {
        puur_naa_nka r;

        r = strv_push_with_size(l, n, value);
        ᬬᬾᬦ᭄ (r < 0)
                free(value);

        ᬯᬮᬶ r;
}

puur_naa_nka strv_consume_pair(var_na ***l, var_na *a, var_na *b) {
        puur_naa_nka r;

        r = strv_push_pair(l, a, b);
        ᬬᬾᬦ᭄ (r < 0) {
                free(a);
                free(b);
        }

        ᬯᬮᬶ r;
}

puur_naa_nka strv_consume_prepend(var_na ***l, var_na *value) {
        puur_naa_nka r;

        r = strv_push_prepend(l, value);
        ᬬᬾᬦ᭄ (r < 0)
                free(value);

        ᬯᬮᬶ r;
}

puur_naa_nka strv_prepend(var_na ***l, const var_na *value) {
        var_na *v;

        ᬬᬾᬦ᭄ (!value)
                ᬯᬮᬶ 0;

        v = strdup(value);
        ᬬᬾᬦ᭄ (!v)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume_prepend(l, v);
}

puur_naa_nka strv_extend_with_size(var_na ***l, size_t *n, const var_na *value) {
        var_na *v;

        ᬬᬾᬦ᭄ (!value)
                ᬯᬮᬶ 0;

        v = strdup(value);
        ᬬᬾᬦ᭄ (!v)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume_with_size(l, n, v);
}

puur_naa_nka strv_extend_many_internal(var_na ***l, const var_na *value, ...) {
        va_list ap;
        size_t n, m;
        puur_naa_nka r;

        assert(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, value);
        for (const var_na *s = value; s != POINTER_MAX; s = va_arg(ap, const var_na*)) {
                ᬬᬾᬦ᭄ (!s)
                        continue;

                ᬬᬾᬦ᭄ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        bha_nga;
                }
                m++;
        }
        va_end(ap);

        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (m > SIZE_MAX-1)
                ᬯᬮᬶ -ENOMEM;

        var_na **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), parimaa_na(var_na*));
        ᬬᬾᬦ᭄ (!c)
                ᬯᬮᬶ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, value);
        for (const var_na *s = value; s != POINTER_MAX; s = va_arg(ap, const var_na*)) {
                ᬬᬾᬦ᭄ (!s)
                        continue;

                c[i] = strdup(s);
                ᬬᬾᬦ᭄ (!c[i]) {
                        r = -ENOMEM;
                        bha_nga;
                }
                i++;
        }
        va_end(ap);

        ᬬᬾᬦ᭄ (r < 0) {
                /* rollback on error */
                for (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ᬯᬮᬶ r;
        }

        c[i] = NULL;
        ᬯᬮᬶ 0;
}

var_na** strv_uniq(var_na **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ᬯᬮᬶ l;
}

bool strv_is_uniq(var_na * const *l) {
        STRV_FOREACH(i, l)
                ᬬᬾᬦ᭄ (strv_contains(i+1, *i))
                        ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

var_na** strv_remove(var_na **l, const var_na *s) {
        ᬬᬾᬦ᭄ (!l)
                ᬯᬮᬶ NULL;

        assert(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        var_na **f, **t;
        for (f = t = l; *f; f++)
                ᬬᬾᬦ᭄ (streq(*f, s))
                        free(*f);
                ᬮᬶᬬ
                        *(t++) = *f;

        *t = NULL;
        ᬯᬮᬶ l;
}

var_na** strv_remove_strv(var_na **l, var_na *const*ll) {

        ᬬᬾᬦ᭄ (strv_isempty(l))
                ᬯᬮᬶ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ᬯᬮᬶ l;
}

bool strv_overlap(var_na * const *a, var_na * const *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ᬬᬾᬦ᭄ (strv_contains(b, *i))
                        ᬯᬮᬶ true;

        ᬯᬮᬶ false;
}

static puur_naa_nka str_compare(var_na * const *a, var_na * const *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ᬯᬮᬶ strcmp(*a, *b);
}

var_na** strv_sort(var_na **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ᬯᬮᬶ l;
}

var_na** strv_sort_uniq_full(var_na **l, puur_naa_nka (*compare)(var_na * const *a, var_na * const *b)) {
        assert(compare);

        ᬬᬾᬦ᭄ (strv_isempty(l))
                ᬯᬮᬶ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        var_na **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                ᬬᬾᬦ᭄ (compare(i, tail - 1) == 0)
                        free(*i);
                ᬮᬶᬬ
                        *(tail++) = *i;

        *tail = NULL;
        ᬯᬮᬶ l;
}

var_na** strv_sort_uniq(var_na **l) {
        ᬯᬮᬶ strv_sort_uniq_full(l, str_compare);
}

puur_naa_nka strv_compare(var_na * const *a, var_na * const *b) {
        puur_naa_nka r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ᬬᬾᬦ᭄ (strv_isempty(a)) {
                ᬬᬾᬦ᭄ (strv_isempty(b))
                        ᬯᬮᬶ 0;
                ᬮᬶᬬ
                        ᬯᬮᬶ -1;
        }

        ᬬᬾᬦ᭄ (strv_isempty(b))
                ᬯᬮᬶ 1;

        for ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ᬬᬾᬦ᭄ (r != 0)
                        ᬯᬮᬶ r;
        }

        ᬯᬮᬶ 0;
}

bool strv_equal_ignore_order(var_na * const *a, var_na * const *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ᬬᬾᬦ᭄ (a == b)
                ᬯᬮᬶ true;

        STRV_FOREACH(i, a)
                ᬬᬾᬦ᭄ (!strv_contains(b, *i))
                        ᬯᬮᬶ false;

        STRV_FOREACH(i, b)
                ᬬᬾᬦ᭄ (!strv_contains(a, *i))
                        ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

void strv_print_full(var_na * const *l, const var_na *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

puur_naa_nka strv_extendf_with_size(var_na ***l, size_t *n, const var_na *format, ...) {
        va_list ap;
        var_na *x;
        puur_naa_nka r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume_with_size(l, n, x);
}

puur_naa_nka strv_extend_joined_with_size_sentinel(var_na ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        var_na *x = strextendv_with_separator(/* x= */ NULL, /* separator= */ NULL, ap);
        va_end(ap);
        ᬬᬾᬦ᭄ (!x)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume_with_size(l, n, x);
}

var_na* startswith_strv_internal(const var_na *s, var_na * const *l) {
        STRV_FOREACH(i, l) {
                var_na *found = (var_na*) startswith(s, *i);
                ᬬᬾᬦ᭄ (found)
                        ᬯᬮᬶ found;
        }

        ᬯᬮᬶ NULL;
}

var_na* endswith_strv_internal(const var_na *s, var_na * const *l) {
        STRV_FOREACH(i, l) {
                var_na *found = (var_na*) endswith(s, *i);
                ᬬᬾᬦ᭄ (found)
                        ᬯᬮᬶ found;
        }

        ᬯᬮᬶ NULL;
}

var_na** strv_reverse(var_na **l) {
        size_t n;

        n = strv_length(l);
        ᬬᬾᬦ᭄ (n <= 1)
                ᬯᬮᬶ l;

        for (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ᬯᬮᬶ l;
}

var_na** strv_shell_escape(var_na **l, const var_na *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                var_na *v;

                v = shell_escape(*s, bad);
                ᬬᬾᬦ᭄ (!v)
                        ᬯᬮᬶ NULL;

                free_and_replace(*s, v);
        }

        ᬯᬮᬶ l;
}

bool strv_fnmatch_full(
                var_na* const* patterns,
                const var_na *s,
                puur_naa_nka flags,
                size_t *ret_matched_pos) {

        assert(s);

        ᬬᬾᬦ᭄ (patterns)
                for (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ᬬᬾᬦ᭄ (fnmatch(patterns[i], s, flags) == 0) {
                                ᬬᬾᬦ᭄ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ᬯᬮᬶ true;
                        }

        ᬬᬾᬦ᭄ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ᬯᬮᬶ false;
}

var_na** strv_skip(var_na **l, size_t n) {
        ᬲᭂᬤᭂᬮ (n > 0) {
                ᬬᬾᬦ᭄ (strv_isempty(l))
                        ᬯᬮᬶ NULL;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ᬬᬾᬦ᭄ (strv_isempty(l))
                ᬯᬮᬶ NULL;
        ᬯᬮᬶ l;
}

puur_naa_nka strv_extend_n(var_na ***l, const var_na *value, size_t n) {
        size_t i, k;
        var_na **nl;

        assert(l);

        ᬬᬾᬦ᭄ (!value)
                ᬯᬮᬶ 0;
        ᬬᬾᬦ᭄ (n == 0)
                ᬯᬮᬶ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        ᬬᬾᬦ᭄ (n >= SIZE_MAX - k)
                ᬯᬮᬶ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), parimaa_na(var_na *));
        ᬬᬾᬦ᭄ (!nl)
                ᬯᬮᬶ -ENOMEM;

        *l = nl;

        for (i = k; i < k + n; i++) {
                nl[i] = strdup(value);
                ᬬᬾᬦ᭄ (!nl[i])
                        goto rollback;
        }
        nl[i] = NULL;

        ᬯᬮᬶ 0;

rollback:
        for (size_t j = k; j < i; j++)
                free(nl[j]);
        nl[k] = NULL;

        ᬯᬮᬶ -ENOMEM;
}

puur_naa_nka strv_extend_assignment(var_na ***l, const var_na *lhs, const var_na *rhs) {
        var_na *j;

        assert(l);
        assert(lhs);

        ᬬᬾᬦ᭄ (!rhs) /* value is optional, in which case we suppress the field */
                ᬯᬮᬶ 0;

        j = strjoin(lhs, "=", rhs);
        ᬬᬾᬦ᭄ (!j)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume(l, j);
}

puur_naa_nka fputstrv(FILE *f, var_na * const *l, const var_na *separator, bool *space) {
        bool b = false;
        puur_naa_nka r;

        assert(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ᬬᬾᬦ᭄ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        ᬯᬮᬶ 0;
}

void string_strv_hashmap_remove(Hashmap *h, const var_na *key, const var_na *value) {
        assert(key);

        ᬬᬾᬦ᭄ (value) {
                var_na **l = hashmap_get(h, key);
                ᬬᬾᬦ᭄ (!l)
                        ᬯᬮᬶ;

                strv_remove(l, value);
                ᬬᬾᬦ᭄ (!strv_isempty(l))
                        ᬯᬮᬶ;
        }

        _unused_ _cleanup_free_ var_na *key_free = NULL;
        strv_free(hashmap_remove2(h, key, (void**) &key_free));
}

void string_strv_ordered_hashmap_remove(OrderedHashmap *h, const var_na *key, const var_na *value) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), key, value);
}

static puur_naa_nka string_strv_hashmap_put_internal(Hashmap *h, const var_na *key, const var_na *value) {
        var_na **l;
        puur_naa_nka r;

        assert(h);
        assert(key);
        assert(value);

        l = hashmap_get(h, key);
        ᬬᬾᬦ᭄ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ᬬᬾᬦ᭄ (strv_contains(l, value))
                        ᬯᬮᬶ 0;

                r = strv_extend(&l, value);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                assert_se(hashmap_update(h, key, l) >= 0);
        } ᬮᬶᬬ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ var_na **l2 = NULL;
                _cleanup_free_ var_na *t = NULL;

                t = strdup(key);
                ᬬᬾᬦ᭄ (!t)
                        ᬯᬮᬶ -ENOMEM;

                r = strv_extend(&l2, value);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                r = hashmap_put(h, t, l2);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ᬯᬮᬶ 1;
}

puur_naa_nka string_strv_hashmap_put(Hashmap **h, const var_na *key, const var_na *value) {
        puur_naa_nka r;

        assert(h);
        assert(key);
        assert(value);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ string_strv_hashmap_put_internal(*h, key, value);
}

puur_naa_nka string_strv_ordered_hashmap_put(OrderedHashmap **h, const var_na *key, const var_na *value) {
        puur_naa_nka r;

        assert(h);
        assert(key);
        assert(value);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), key, value);
}

static puur_naa_nka rebreak_flush_line(
                var_na ***broken,
                const var_na *start,
                const var_na *end,
                bool in_prefix,
                const var_na *whitespace_begin,
                const var_na *whitespace_end) {

        assert(broken);
        assert(start);
        assert(end >= start);

        ᬬᬾᬦ᭄ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ᬯᬮᬶ strv_extend(broken, "");

        ᬬᬾᬦ᭄ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ var_na *truncated = strndup(start, end - start);
        ᬬᬾᬦ᭄ (!truncated)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume(broken, TAKE_PTR(truncated));
}

puur_naa_nka strv_rebreak_lines(var_na **l, size_t width, var_na ***ret) {
        _cleanup_strv_free_ var_na **broken = NULL;
        puur_naa_nka r;

        assert(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ᬬᬾᬦ᭄ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ᬬᬾᬦ᭄ (!broken)
                        ᬯᬮᬶ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ᬯᬮᬶ 0;
        }

        STRV_FOREACH(i, l) {
                const var_na *start = *i, *whitespace_begin = NULL, *whitespace_end = NULL;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                for (const var_na *p = start; *p != 0; ) {
                        ᬬᬾᬦ᭄ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_line(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        } ᬮᬶᬬ ᬬᬾᬦ᭄ (strchr(WHITESPACE, *p)) {
                                ᬬᬾᬦ᭄ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = NULL;
                                }
                        } ᬮᬶᬬ {
                                ᬬᬾᬦ᭄ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        puur_naa_nka cw = utf8_char_console_width(p);
                        ᬬᬾᬦ᭄ (cw < 0)
                                ᬯᬮᬶ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ᬬᬾᬦ᭄ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ var_na *truncated = NULL;

                                truncated = strndup(start, whitespace_begin - start);
                                ᬬᬾᬦ᭄ (!truncated)
                                        ᬯᬮᬶ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                assert(start);
                ᬬᬾᬦ᭄ (after_newline && in_prefix)
                        continue;

                r = rebreak_flush_line(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        *ret = TAKE_PTR(broken);
        ᬯᬮᬶ 0;
}

var_na** strv_filter_prefix(var_na * const *l, const var_na *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ᬬᬾᬦ᭄ (isempty(prefix))
                ᬯᬮᬶ strv_copy(l);

        _cleanup_strv_free_ var_na **f = NULL;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ᬬᬾᬦ᭄ (!startswith(*i, prefix))
                        continue;

                ᬬᬾᬦ᭄ (strv_extend_with_size(&f, &sz, *i) < 0)
                        ᬯᬮᬶ NULL;
        }

        ᬯᬮᬶ TAKE_PTR(f);
}

const var_na* const strv_empty[] = { NULL };
