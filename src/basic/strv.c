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
                အကယ်၍ (streq(*i, name))
                        ပြန်ပေး *i;

        ပြန်ပေး NULL;
}

var_na* strv_find_case(var_na * const *l, const var_na *name) {
        assert(name);

        STRV_FOREACH(i, l)
                အကယ်၍ (strcaseeq(*i, name))
                        ပြန်ပေး *i;

        ပြန်ပေး NULL;
}

var_na* strv_find_prefix(var_na * const *l, const var_na *name) {
        assert(name);

        STRV_FOREACH(i, l)
                အကယ်၍ (startswith(*i, name))
                        ပြန်ပေး *i;

        ပြန်ပေး NULL;
}

var_na* strv_find_startswith(var_na * const *l, const var_na *name) {
        assert(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                var_na *e;

                e = startswith(*i, name);
                အကယ်၍ (e)
                        ပြန်ပေး e;
        }

        ပြန်ပေး NULL;
}

static var_na* strv_find_closest_prefix(var_na * const *l, const var_na *name) {
        size_t best_distance = SIZE_MAX;
        var_na *best = NULL;

        assert(name);

        STRV_FOREACH(s, l) {
                var_na *e = startswith(*s, name);
                အကယ်၍ (!e)
                        continue;

                size_t n = strlen(e);
                အကယ်၍ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ပြန်ပေး best;
}

static var_na* strv_find_closest_by_levenshtein(var_na * const *l, const var_na *name) {
        ssize_t best_distance = SSIZE_MAX;
        var_na *best = NULL;

        assert(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                အကယ်၍ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ပြန်ပေး NULL;
                }

                အကယ်၍ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        continue;

                အကယ်၍ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ပြန်ပေး best;
}

var_na* strv_find_closest(var_na * const *l, const var_na *name) {
        assert(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        var_na *found = strv_find_closest_prefix(l, name);
        အကယ်၍ (found)
                ပြန်ပေး found;

        ပြန်ပေး strv_find_closest_by_levenshtein(l, name);
}

var_na* strv_find_first_field(var_na * const *needles, var_na * const *haystack) {
        STRV_FOREACH(k, needles) {
                var_na *value = strv_env_pairs_get((var_na **)haystack, *k);
                အကယ်၍ (value)
                        ပြန်ပေး value;
        }

        ပြန်ပေး NULL;
}

var_na** strv_free(var_na **l) {
        STRV_FOREACH(k, l)
                free(*k);

        ပြန်ပေး mfree(l);
}

var_na** strv_free_erase(var_na **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ပြန်ပေး mfree(l);
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
        အကယ်၍ (!result)
                ပြန်ပေး NULL;

        k = result;
        STRV_FOREACH(i, l) {
                အကယ်၍ (n == 0)
                        bha_nga;

                *k = strdup(*i);
                အကယ်၍ (!*k)
                        ပြန်ပေး NULL;
                k++;

                အကယ်၍ (n != SIZE_MAX)
                        n--;
        }

        *k = NULL;
        ပြန်ပေး TAKE_PTR(result);
}

ကိန်းပြည့် strv_copy_unless_empty(var_na * const *l, var_na ***ret) {
        assert(ret);

        အကယ်၍ (strv_isempty(l)) {
                *ret = NULL;
                ပြန်ပေး 0;
        }

        var_na **copy = strv_copy(l);
        အကယ်၍ (!copy)
                ပြန်ပေး -ENOMEM;

        *ret = TAKE_PTR(copy);
        ပြန်ပေး 1;
}

size_t strv_length(var_na * const *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ပြန်ပေး n;
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
        အတွက် (const var_na *s = x; s; s = va_arg(aq, const var_na*)) {
                အကယ်၍ (s == STRV_IGNORE)
                        continue;

                n++;
        }
        va_end(aq);

        a = new(var_na*, n+1);
        အကယ်၍ (!a)
                ပြန်ပေး NULL;

        အတွက် (const var_na *s = x; s; s = va_arg(ap, const var_na*)) {
                အကယ်၍ (s == STRV_IGNORE)
                        continue;

                a[i] = strdup(s);
                အကယ်၍ (!a[i])
                        ပြန်ပေး NULL;

                i++;
        }

        a[i] = NULL;

        ပြန်ပေး TAKE_PTR(a);
}

var_na** strv_new_internal(const var_na *x, ...) {
        var_na **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ပြန်ပေး r;
}

ကိန်းပြည့် strv_extend_strv(var_na ***a, var_na * const *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        assert(a);

        q = strv_length(b);
        အကယ်၍ (q == 0)
                ပြန်ပေး 0;

        p = strv_length(*a);
        အကယ်၍ (p >= SIZE_MAX - q)
                ပြန်ပေး -ENOMEM;

        var_na **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), parimaa_na(var_na *));
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        t[p] = NULL;
        *a = t;

        STRV_FOREACH(s, b) {
                အကယ်၍ (filter_duplicates && strv_contains(t, *s))
                        continue;

                t[p+i] = strdup(*s);
                အကယ်၍ (!t[p+i])
                        goto rollback;

                i++;
                t[p+i] = NULL;
        }

        assert(i <= q);

        ပြန်ပေး (ကိန်းပြည့်) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = NULL;
        ပြန်ပေး -ENOMEM;
}

ကိန်းပြည့် strv_extend_strv_consume(var_na ***a, var_na **b, bool filter_duplicates) {
        _cleanup_strv_free_ var_na **b_consume = b;
        size_t p, q, i;

        assert(a);

        q = strv_length(b);
        အကယ်၍ (q == 0)
                ပြန်ပေး 0;

        p = strv_length(*a);
        အကယ်၍ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                အကယ်၍ (filter_duplicates)
                        strv_uniq(*a);

                ပြန်ပေး strv_length(*a);
        }

        အကယ်၍ (p >= SIZE_MAX - q)
                ပြန်ပေး -ENOMEM;

        var_na **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), parimaa_na(var_na *));
        အကယ်၍ (!t)
                ပြန်ပေး -ENOMEM;

        t[p] = NULL;
        *a = t;

        အကယ်၍ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = NULL;
                i = q;
        } မဟုတ်ပါက {
                i = 0;

                STRV_FOREACH(s, b) {
                        အကယ်၍ (strv_contains(t, *s)) {
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

        ပြန်ပေး (ကိန်းပြည့်) i;
}

ကိန်းပြည့် strv_extend_strv_biconcat(var_na ***a, const var_na *prefix, const var_na* const *b, const var_na *suffix) {
        ကိန်းပြည့် r;

        assert(a);

        STRV_FOREACH(s, b) {
                var_na *v;

                v = strjoin(strempty(prefix), *s, suffix);
                အကယ်၍ (!v)
                        ပြန်ပေး -ENOMEM;

                r = strv_consume(a, v);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် strv_split_newlines_full(var_na ***ret, const var_na *s, ExtractFlags flags) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n;
        ကိန်းပြည့် r;

        assert(ret);
        assert(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        n = strv_length(l);
        အကယ်၍ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ပြန်ပေး n;
}

var_na** strv_split_newlines(const var_na *s) {
        var_na **ret;

        အကယ်၍ (strv_split_newlines_full(&ret, s, 0) < 0)
                ပြန်ပေး NULL;

        ပြန်ပေး ret;
}

ကိန်းပြည့် strv_split_full(var_na ***t, const var_na *s, const var_na *separators, ExtractFlags flags) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n = 0;
        ကိန်းပြည့် r;

        assert(t);
        assert(s);

        အတွက် (;;) {
                _cleanup_free_ var_na *word = NULL;

                r = extract_first_word(&s, &word, separators, flags);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;

                အကယ်၍ (!GREEDY_REALLOC(l, n + 2))
                        ပြန်ပေး -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = NULL;
        }

        အကယ်၍ (!l) {
                l = new0(var_na*, 1);
                အကယ်၍ (!l)
                        ပြန်ပေး -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ပြန်ပေး (ကိန်းပြည့်) n;
}

var_na** strv_split(const var_na *s, const var_na *separators) {
        var_na **ret;

        အကယ်၍ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ပြန်ပေး NULL;

        ပြန်ပေး ret;
}

ကိန်းပြည့် strv_split_and_extend_full(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates, ExtractFlags flags) {
        var_na **l;
        ကိန်းပြည့် r;

        assert(t);
        assert(s);

        r = strv_split_full(&l, s, separators, flags);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး (ကိန်းပြည့်) strv_length(*t);
}

ကိန်းပြည့် strv_split_and_extend(var_na ***t, const var_na *s, const var_na *separators, bool filter_duplicates) {
        ပြန်ပေး strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

ကိန်းပြည့် strv_split_colon_pairs(var_na ***t, const var_na *s) {
        _cleanup_strv_free_ var_na **l = NULL;
        size_t n = 0;
        ကိန်းပြည့် r;

        assert(t);
        assert(s);

        အတွက် (;;) {
                _cleanup_free_ var_na *first = NULL, *second = NULL, *tuple = NULL, *second_or_empty = NULL;

                r = extract_first_word(&s, &tuple, NULL, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        bha_nga;

                const var_na *p = tuple;
                r = extract_many_words(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        continue;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                အကယ်၍ (!isempty(p))
                        ပြန်ပေး -EINVAL;

                second_or_empty = strdup(strempty(second));
                အကယ်၍ (!second_or_empty)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (!GREEDY_REALLOC(l, n + 3))
                        ပြန်ပေး -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = NULL;
        }

        အကယ်၍ (!l) {
                l = new0(var_na*, 1);
                အကယ်၍ (!l)
                        ပြန်ပေး -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ပြန်ပေး (ကိန်းပြည့်) n;
}

var_na* strv_join_full(var_na * const *l, const var_na *separator, const var_na *prefix) {
        var_na *r, *e;
        size_t n, k, m;

        အကယ်၍ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                အကယ်၍ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(var_na, n+1);
        အကယ်၍ (!r)
                ပြန်ပေး NULL;

        e = r;
        STRV_FOREACH(s, l) {
                အကယ်၍ (s != l)
                        e = stpcpy(e, separator);

                အကယ်၍ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ပြန်ပေး r;
}

ကိန်းပြည့် strv_push_with_size(var_na ***l, size_t *n, var_na *value) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        assert(l);
        POINTER_MAY_BE_NULL(n);

        အကယ်၍ (!value)
                ပြန်ပေး 0;

        size_t size = n ? *n : SIZE_MAX;
        အကယ်၍ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        အကယ်၍ (size > SIZE_MAX-2)
                ပြန်ပေး -ENOMEM;

        var_na **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), parimaa_na(var_na*));
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        c[size] = value;
        c[size+1] = NULL;

        *l = c;
        အကယ်၍ (n)
                *n = size + 1;
        ပြန်ပေး 0;
}

ကိန်းပြည့် strv_push_pair(var_na ***l, var_na *a, var_na *b) {
        var_na **c;
        size_t n;

        assert(l);

        အကယ်၍ (!a && !b)
                ပြန်ပေး 0;

        n = strv_length(*l);

        /* Check for overflow */
        အကယ်၍ (n > SIZE_MAX-3)
                ပြန်ပေး -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), parimaa_na(var_na*));
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (a)
                c[n++] = a;
        အကယ်၍ (b)
                c[n++] = b;
        c[n] = NULL;

        *l = c;
        ပြန်ပေး 0;
}

ကိန်းပြည့် strv_insert(var_na ***l, size_t position, var_na *value) {
        var_na **c;
        size_t n, m;

        assert(l);

        အကယ်၍ (!value)
                ပြန်ပေး 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        အကယ်၍ (n > SIZE_MAX - 2)
                ပြန်ပေး -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), parimaa_na(var_na*));
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (n > position)
                memmove(c + position + 1, c + position, (n - position) * parimaa_na(var_na*));

        c[position] = value;
        c[n + 1] = NULL;

        *l = c;
        ပြန်ပေး 0;
}

ကိန်းပြည့် strv_consume_with_size(var_na ***l, size_t *n, var_na *value) {
        ကိန်းပြည့် r;

        r = strv_push_with_size(l, n, value);
        အကယ်၍ (r < 0)
                free(value);

        ပြန်ပေး r;
}

ကိန်းပြည့် strv_consume_pair(var_na ***l, var_na *a, var_na *b) {
        ကိန်းပြည့် r;

        r = strv_push_pair(l, a, b);
        အကယ်၍ (r < 0) {
                free(a);
                free(b);
        }

        ပြန်ပေး r;
}

ကိန်းပြည့် strv_consume_prepend(var_na ***l, var_na *value) {
        ကိန်းပြည့် r;

        r = strv_push_prepend(l, value);
        အကယ်၍ (r < 0)
                free(value);

        ပြန်ပေး r;
}

ကိန်းပြည့် strv_prepend(var_na ***l, const var_na *value) {
        var_na *v;

        အကယ်၍ (!value)
                ပြန်ပေး 0;

        v = strdup(value);
        အကယ်၍ (!v)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume_prepend(l, v);
}

ကိန်းပြည့် strv_extend_with_size(var_na ***l, size_t *n, const var_na *value) {
        var_na *v;

        အကယ်၍ (!value)
                ပြန်ပေး 0;

        v = strdup(value);
        အကယ်၍ (!v)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume_with_size(l, n, v);
}

ကိန်းပြည့် strv_extend_many_internal(var_na ***l, const var_na *value, ...) {
        va_list ap;
        size_t n, m;
        ကိန်းပြည့် r;

        assert(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, value);
        အတွက် (const var_na *s = value; s != POINTER_MAX; s = va_arg(ap, const var_na*)) {
                အကယ်၍ (!s)
                        continue;

                အကယ်၍ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        bha_nga;
                }
                m++;
        }
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (m > SIZE_MAX-1)
                ပြန်ပေး -ENOMEM;

        var_na **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), parimaa_na(var_na*));
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, value);
        အတွက် (const var_na *s = value; s != POINTER_MAX; s = va_arg(ap, const var_na*)) {
                အကယ်၍ (!s)
                        continue;

                c[i] = strdup(s);
                အကယ်၍ (!c[i]) {
                        r = -ENOMEM;
                        bha_nga;
                }
                i++;
        }
        va_end(ap);

        အကယ်၍ (r < 0) {
                /* rollback on error */
                အတွက် (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ပြန်ပေး r;
        }

        c[i] = NULL;
        ပြန်ပေး 0;
}

var_na** strv_uniq(var_na **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ပြန်ပေး l;
}

bool strv_is_uniq(var_na * const *l) {
        STRV_FOREACH(i, l)
                အကယ်၍ (strv_contains(i+1, *i))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

var_na** strv_remove(var_na **l, const var_na *s) {
        အကယ်၍ (!l)
                ပြန်ပေး NULL;

        assert(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        var_na **f, **t;
        အတွက် (f = t = l; *f; f++)
                အကယ်၍ (streq(*f, s))
                        free(*f);
                မဟုတ်ပါက
                        *(t++) = *f;

        *t = NULL;
        ပြန်ပေး l;
}

var_na** strv_remove_strv(var_na **l, var_na *const*ll) {

        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ပြန်ပေး l;
}

bool strv_overlap(var_na * const *a, var_na * const *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                အကယ်၍ (strv_contains(b, *i))
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

static ကိန်းပြည့် str_compare(var_na * const *a, var_na * const *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ပြန်ပေး strcmp(*a, *b);
}

var_na** strv_sort(var_na **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ပြန်ပေး l;
}

var_na** strv_sort_uniq_full(var_na **l, ကိန်းပြည့် (*compare)(var_na * const *a, var_na * const *b)) {
        assert(compare);

        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        var_na **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                အကယ်၍ (compare(i, tail - 1) == 0)
                        free(*i);
                မဟုတ်ပါက
                        *(tail++) = *i;

        *tail = NULL;
        ပြန်ပေး l;
}

var_na** strv_sort_uniq(var_na **l) {
        ပြန်ပေး strv_sort_uniq_full(l, str_compare);
}

ကိန်းပြည့် strv_compare(var_na * const *a, var_na * const *b) {
        ကိန်းပြည့် r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        အကယ်၍ (strv_isempty(a)) {
                အကယ်၍ (strv_isempty(b))
                        ပြန်ပေး 0;
                မဟုတ်ပါက
                        ပြန်ပေး -1;
        }

        အကယ်၍ (strv_isempty(b))
                ပြန်ပေး 1;

        အတွက် ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                အကယ်၍ (r != 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

bool strv_equal_ignore_order(var_na * const *a, var_na * const *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        အကယ်၍ (a == b)
                ပြန်ပေး true;

        STRV_FOREACH(i, a)
                အကယ်၍ (!strv_contains(b, *i))
                        ပြန်ပေး false;

        STRV_FOREACH(i, b)
                အကယ်၍ (!strv_contains(a, *i))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

void strv_print_full(var_na * const *l, const var_na *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

ကိန်းပြည့် strv_extendf_with_size(var_na ***l, size_t *n, const var_na *format, ...) {
        va_list ap;
        var_na *x;
        ကိန်းပြည့် r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume_with_size(l, n, x);
}

ကိန်းပြည့် strv_extend_joined_with_size_sentinel(var_na ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        var_na *x = strextendv_with_separator(/* x= */ NULL, /* separator= */ NULL, ap);
        va_end(ap);
        အကယ်၍ (!x)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume_with_size(l, n, x);
}

var_na* startswith_strv_internal(const var_na *s, var_na * const *l) {
        STRV_FOREACH(i, l) {
                var_na *found = (var_na*) startswith(s, *i);
                အကယ်၍ (found)
                        ပြန်ပေး found;
        }

        ပြန်ပေး NULL;
}

var_na* endswith_strv_internal(const var_na *s, var_na * const *l) {
        STRV_FOREACH(i, l) {
                var_na *found = (var_na*) endswith(s, *i);
                အကယ်၍ (found)
                        ပြန်ပေး found;
        }

        ပြန်ပေး NULL;
}

var_na** strv_reverse(var_na **l) {
        size_t n;

        n = strv_length(l);
        အကယ်၍ (n <= 1)
                ပြန်ပေး l;

        အတွက် (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ပြန်ပေး l;
}

var_na** strv_shell_escape(var_na **l, const var_na *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                var_na *v;

                v = shell_escape(*s, bad);
                အကယ်၍ (!v)
                        ပြန်ပေး NULL;

                free_and_replace(*s, v);
        }

        ပြန်ပေး l;
}

bool strv_fnmatch_full(
                var_na* const* patterns,
                const var_na *s,
                ကိန်းပြည့် flags,
                size_t *ret_matched_pos) {

        assert(s);

        အကယ်၍ (patterns)
                အတွက် (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        အကယ်၍ (fnmatch(patterns[i], s, flags) == 0) {
                                အကယ်၍ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ပြန်ပေး true;
                        }

        အကယ်၍ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ပြန်ပေး false;
}

var_na** strv_skip(var_na **l, size_t n) {
        နေစဉ် (n > 0) {
                အကယ်၍ (strv_isempty(l))
                        ပြန်ပေး NULL;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး NULL;
        ပြန်ပေး l;
}

ကိန်းပြည့် strv_extend_n(var_na ***l, const var_na *value, size_t n) {
        size_t i, k;
        var_na **nl;

        assert(l);

        အကယ်၍ (!value)
                ပြန်ပေး 0;
        အကယ်၍ (n == 0)
                ပြန်ပေး 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        အကယ်၍ (n >= SIZE_MAX - k)
                ပြန်ပေး -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), parimaa_na(var_na *));
        အကယ်၍ (!nl)
                ပြန်ပေး -ENOMEM;

        *l = nl;

        အတွက် (i = k; i < k + n; i++) {
                nl[i] = strdup(value);
                အကယ်၍ (!nl[i])
                        goto rollback;
        }
        nl[i] = NULL;

        ပြန်ပေး 0;

rollback:
        အတွက် (size_t j = k; j < i; j++)
                free(nl[j]);
        nl[k] = NULL;

        ပြန်ပေး -ENOMEM;
}

ကိန်းပြည့် strv_extend_assignment(var_na ***l, const var_na *lhs, const var_na *rhs) {
        var_na *j;

        assert(l);
        assert(lhs);

        အကယ်၍ (!rhs) /* value is optional, in which case we suppress the field */
                ပြန်ပေး 0;

        j = strjoin(lhs, "=", rhs);
        အကယ်၍ (!j)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume(l, j);
}

ကိန်းပြည့် fputstrv(FILE *f, var_na * const *l, const var_na *separator, bool *space) {
        bool b = false;
        ကိန်းပြည့် r;

        assert(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        အကယ်၍ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

void string_strv_hashmap_remove(Hashmap *h, const var_na *key, const var_na *value) {
        assert(key);

        အကယ်၍ (value) {
                var_na **l = hashmap_get(h, key);
                အကယ်၍ (!l)
                        ပြန်ပေး;

                strv_remove(l, value);
                အကယ်၍ (!strv_isempty(l))
                        ပြန်ပေး;
        }

        _unused_ _cleanup_free_ var_na *key_free = NULL;
        strv_free(hashmap_remove2(h, key, (void**) &key_free));
}

void string_strv_ordered_hashmap_remove(OrderedHashmap *h, const var_na *key, const var_na *value) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), key, value);
}

static ကိန်းပြည့် string_strv_hashmap_put_internal(Hashmap *h, const var_na *key, const var_na *value) {
        var_na **l;
        ကိန်းပြည့် r;

        assert(h);
        assert(key);
        assert(value);

        l = hashmap_get(h, key);
        အကယ်၍ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                အကယ်၍ (strv_contains(l, value))
                        ပြန်ပေး 0;

                r = strv_extend(&l, value);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                assert_se(hashmap_update(h, key, l) >= 0);
        } မဟုတ်ပါက {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ var_na **l2 = NULL;
                _cleanup_free_ var_na *t = NULL;

                t = strdup(key);
                အကယ်၍ (!t)
                        ပြန်ပေး -ENOMEM;

                r = strv_extend(&l2, value);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                r = hashmap_put(h, t, l2);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ပြန်ပေး 1;
}

ကိန်းပြည့် string_strv_hashmap_put(Hashmap **h, const var_na *key, const var_na *value) {
        ကိန်းပြည့် r;

        assert(h);
        assert(key);
        assert(value);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး string_strv_hashmap_put_internal(*h, key, value);
}

ကိန်းပြည့် string_strv_ordered_hashmap_put(OrderedHashmap **h, const var_na *key, const var_na *value) {
        ကိန်းပြည့် r;

        assert(h);
        assert(key);
        assert(value);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), key, value);
}

static ကိန်းပြည့် rebreak_flush_line(
                var_na ***broken,
                const var_na *start,
                const var_na *end,
                bool in_prefix,
                const var_na *whitespace_begin,
                const var_na *whitespace_end) {

        assert(broken);
        assert(start);
        assert(end >= start);

        အကယ်၍ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ပြန်ပေး strv_extend(broken, "");

        အကယ်၍ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ var_na *truncated = strndup(start, end - start);
        အကယ်၍ (!truncated)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume(broken, TAKE_PTR(truncated));
}

ကိန်းပြည့် strv_rebreak_lines(var_na **l, size_t width, var_na ***ret) {
        _cleanup_strv_free_ var_na **broken = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        အကယ်၍ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                အကယ်၍ (!broken)
                        ပြန်ပေး -ENOMEM;

                *ret = TAKE_PTR(broken);
                ပြန်ပေး 0;
        }

        STRV_FOREACH(i, l) {
                const var_na *start = *i, *whitespace_begin = NULL, *whitespace_end = NULL;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                အတွက် (const var_na *p = start; *p != 0; ) {
                        အကယ်၍ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_line(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        } မဟုတ်ပါက အကယ်၍ (strchr(WHITESPACE, *p)) {
                                အကယ်၍ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = NULL;
                                }
                        } မဟုတ်ပါက {
                                အကယ်၍ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        ကိန်းပြည့် cw = utf8_char_console_width(p);
                        အကယ်၍ (cw < 0)
                                ပြန်ပေး log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        အကယ်၍ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ var_na *truncated = NULL;

                                truncated = strndup(start, whitespace_begin - start);
                                အကယ်၍ (!truncated)
                                        ပြန်ပေး -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

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
                အကယ်၍ (after_newline && in_prefix)
                        continue;

                r = rebreak_flush_line(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        *ret = TAKE_PTR(broken);
        ပြန်ပေး 0;
}

var_na** strv_filter_prefix(var_na * const *l, const var_na *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        အကယ်၍ (isempty(prefix))
                ပြန်ပေး strv_copy(l);

        _cleanup_strv_free_ var_na **f = NULL;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                အကယ်၍ (!startswith(*i, prefix))
                        continue;

                အကယ်၍ (strv_extend_with_size(&f, &sz, *i) < 0)
                        ပြန်ပေး NULL;
        }

        ပြန်ပေး TAKE_PTR(f);
}

const var_na* const strv_empty[] = { NULL };
