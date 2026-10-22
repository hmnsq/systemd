/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fnmatch.h>
#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "gunicode.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "sort-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "utf8.h"

અક્ષર* strv_find(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        પ્રતિજ્ઞા(name);

        STRV_FOREACH(i, l)
                જો (streq(*i, name))
                        પાછા *i;

        પાછા ખાલી;
}

અક્ષર* strv_find_case(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        પ્રતિજ્ઞા(name);

        STRV_FOREACH(i, l)
                જો (strcaseeq(*i, name))
                        પાછા *i;

        પાછા ખાલી;
}

અક્ષર* strv_find_prefix(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        પ્રતિજ્ઞા(name);

        STRV_FOREACH(i, l)
                જો (startswith(*i, name))
                        પાછા *i;

        પાછા ખાલી;
}

અક્ષર* strv_find_startswith(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        પ્રતિજ્ઞા(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                અક્ષર *e;

                e = startswith(*i, name);
                જો (e)
                        પાછા e;
        }

        પાછા ખાલી;
}

સ્થિર અક્ષર* strv_find_closest_prefix(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        size_t best_distance = SIZE_MAX;
        અક્ષર *best = ખાલી;

        પ્રતિજ્ઞા(name);

        STRV_FOREACH(s, l) {
                અક્ષર *e = startswith(*s, name);
                જો (!e)
                        ચાલુ;

                size_t n = strlen(e);
                જો (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        પાછા best;
}

સ્થિર અક્ષર* strv_find_closest_by_levenshtein(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        ssize_t best_distance = SSIZE_MAX;
        અક્ષર *best = ખાલી;

        પ્રતિજ્ઞા(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                જો (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        પાછા ખાલી;
                }

                જો (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ચાલુ;

                જો (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        પાછા best;
}

અક્ષર* strv_find_closest(અક્ષર * અચલ *l, અચલ અક્ષર *name) {
        પ્રતિજ્ઞા(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        અક્ષર *found = strv_find_closest_prefix(l, name);
        જો (found)
                પાછા found;

        પાછા strv_find_closest_by_levenshtein(l, name);
}

અક્ષર* strv_find_first_field(અક્ષર * અચલ *needles, અક્ષર * અચલ *haystack) {
        STRV_FOREACH(k, needles) {
                અક્ષર *કિંમત = strv_env_pairs_get((અક્ષર **)haystack, *k);
                જો (કિંમત)
                        પાછા કિંમત;
        }

        પાછા ખાલી;
}

અક્ષર** strv_free(અક્ષર **l) {
        STRV_FOREACH(k, l)
                મુક્ત(*k);

        પાછા mfree(l);
}

અક્ષર** strv_free_erase(અક્ષર **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        પાછા mfree(l);
}

શૂન્ય strv_free_many(અક્ષર ***strvs, size_t n) {
        પ્રતિજ્ઞા(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        મુક્ત(strvs);
}

અક્ષર** strv_copy_n(અક્ષર * અચલ *l, size_t n) {
        _cleanup_strv_free_ અક્ષર **result = ખાલી;
        અક્ષર **k;

        result = new(અક્ષર*, MIN(strv_length(l), n) + 1);
        જો (!result)
                પાછા ખાલી;

        k = result;
        STRV_FOREACH(i, l) {
                જો (n == 0)
                        તોડો;

                *k = strdup(*i);
                જો (!*k)
                        પાછા ખાલી;
                k++;

                જો (n != SIZE_MAX)
                        n--;
        }

        *k = ખાલી;
        પાછા TAKE_PTR(result);
}

પૂર્ણાંક strv_copy_unless_empty(અક્ષર * અચલ *l, અક્ષર ***ret) {
        પ્રતિજ્ઞા(ret);

        જો (strv_isempty(l)) {
                *ret = ખાલી;
                પાછા 0;
        }

        અક્ષર **copy = strv_copy(l);
        જો (!copy)
                પાછા -ENOMEM;

        *ret = TAKE_PTR(copy);
        પાછા 1;
}

size_t strv_length(અક્ષર * અચલ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        પાછા n;
}

અક્ષર** strv_new_ap(અચલ અક્ષર *x, va_list ap) {
        _cleanup_strv_free_ અક્ષર **a = ખાલી;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        માટે (અચલ અક્ષર *s = x; s; s = va_arg(aq, અચલ અક્ષર*)) {
                જો (s == STRV_IGNORE)
                        ચાલુ;

                n++;
        }
        va_end(aq);

        a = new(અક્ષર*, n+1);
        જો (!a)
                પાછા ખાલી;

        માટે (અચલ અક્ષર *s = x; s; s = va_arg(ap, અચલ અક્ષર*)) {
                જો (s == STRV_IGNORE)
                        ચાલુ;

                a[i] = strdup(s);
                જો (!a[i])
                        પાછા ખાલી;

                i++;
        }

        a[i] = ખાલી;

        પાછા TAKE_PTR(a);
}

અક્ષર** strv_new_internal(અચલ અક્ષર *x, ...) {
        અક્ષર **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક strv_extend_strv(અક્ષર ***a, અક્ષર * અચલ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        પ્રતિજ્ઞા(a);

        q = strv_length(b);
        જો (q == 0)
                પાછા 0;

        p = strv_length(*a);
        જો (p >= SIZE_MAX - q)
                પાછા -ENOMEM;

        અક્ષર **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), કદ(અક્ષર *));
        જો (!t)
                પાછા -ENOMEM;

        t[p] = ખાલી;
        *a = t;

        STRV_FOREACH(s, b) {
                જો (filter_duplicates && strv_contains(t, *s))
                        ચાલુ;

                t[p+i] = strdup(*s);
                જો (!t[p+i])
                        જાઓ rollback;

                i++;
                t[p+i] = ખાલી;
        }

        પ્રતિજ્ઞા(i <= q);

        પાછા (પૂર્ણાંક) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ખાલી;
        પાછા -ENOMEM;
}

પૂર્ણાંક strv_extend_strv_consume(અક્ષર ***a, અક્ષર **b, bool filter_duplicates) {
        _cleanup_strv_free_ અક્ષર **b_consume = b;
        size_t p, q, i;

        પ્રતિજ્ઞા(a);

        q = strv_length(b);
        જો (q == 0)
                પાછા 0;

        p = strv_length(*a);
        જો (p == 0) {
                strv_free_and_replace(*a, b_consume);

                જો (filter_duplicates)
                        strv_uniq(*a);

                પાછા strv_length(*a);
        }

        જો (p >= SIZE_MAX - q)
                પાછા -ENOMEM;

        અક્ષર **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), કદ(અક્ષર *));
        જો (!t)
                પાછા -ENOMEM;

        t[p] = ખાલી;
        *a = t;

        જો (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ખાલી;
                i = q;
        } નહીંતર {
                i = 0;

                STRV_FOREACH(s, b) {
                        જો (strv_contains(t, *s)) {
                                મુક્ત(*s);
                                ચાલુ;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ખાલી;
                }
        }

        પ્રતિજ્ઞા(i <= q);

        b_consume = mfree(b_consume);

        પાછા (પૂર્ણાંક) i;
}

પૂર્ણાંક strv_extend_strv_biconcat(અક્ષર ***a, અચલ અક્ષર *prefix, અચલ અક્ષર* અચલ *b, અચલ અક્ષર *suffix) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);

        STRV_FOREACH(s, b) {
                અક્ષર *v;

                v = strjoin(strempty(prefix), *s, suffix);
                જો (!v)
                        પાછા -ENOMEM;

                r = strv_consume(a, v);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક strv_split_newlines_full(અક્ષર ***ret, અચલ અક્ષર *s, ExtractFlags flags) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        જો (r < 0)
                પાછા r;

        n = strv_length(l);
        જો (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        પાછા n;
}

અક્ષર** strv_split_newlines(અચલ અક્ષર *s) {
        અક્ષર **ret;

        જો (strv_split_newlines_full(&ret, s, 0) < 0)
                પાછા ખાલી;

        પાછા ret;
}

પૂર્ણાંક strv_split_full(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, ExtractFlags flags) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        size_t n = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(s);

        માટે (;;) {
                _cleanup_free_ અક્ષર *word = ખાલી;

                r = extract_first_word(&s, &word, separators, flags);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                જો (!GREEDY_REALLOC(l, n + 2))
                        પાછા -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = ખાલી;
        }

        જો (!l) {
                l = new0(અક્ષર*, 1);
                જો (!l)
                        પાછા -ENOMEM;
        }

        *t = TAKE_PTR(l);

        પાછા (પૂર્ણાંક) n;
}

અક્ષર** strv_split(અચલ અક્ષર *s, અચલ અક્ષર *separators) {
        અક્ષર **ret;

        જો (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                પાછા ખાલી;

        પાછા ret;
}

પૂર્ણાંક strv_split_and_extend_full(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, bool filter_duplicates, ExtractFlags flags) {
        અક્ષર **l;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(s);

        r = strv_split_full(&l, s, separators, flags);
        જો (r < 0)
                પાછા r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        જો (r < 0)
                પાછા r;

        પાછા (પૂર્ણાંક) strv_length(*t);
}

પૂર્ણાંક strv_split_and_extend(અક્ષર ***t, અચલ અક્ષર *s, અચલ અક્ષર *separators, bool filter_duplicates) {
        પાછા strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

પૂર્ણાંક strv_split_colon_pairs(અક્ષર ***t, અચલ અક્ષર *s) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        size_t n = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(s);

        માટે (;;) {
                _cleanup_free_ અક્ષર *first = ખાલી, *second = ખાલી, *tuple = ખાલી, *second_or_empty = ખાલી;

                r = extract_first_word(&s, &tuple, ખાલી, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                અચલ અક્ષર *p = tuple;
                r = extract_many_શબ્દો(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        ચાલુ;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                જો (!isempty(p))
                        પાછા -EINVAL;

                second_or_empty = strdup(strempty(second));
                જો (!second_or_empty)
                        પાછા -ENOMEM;

                જો (!GREEDY_REALLOC(l, n + 3))
                        પાછા -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = ખાલી;
        }

        જો (!l) {
                l = new0(અક્ષર*, 1);
                જો (!l)
                        પાછા -ENOMEM;
        }

        *t = TAKE_PTR(l);

        પાછા (પૂર્ણાંક) n;
}

અક્ષર* strv_join_full(અક્ષર * અચલ *l, અચલ અક્ષર *separator, અચલ અક્ષર *prefix) {
        અક્ષર *r, *e;
        size_t n, k, m;

        જો (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                જો (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(અક્ષર, n+1);
        જો (!r)
                પાછા ખાલી;

        e = r;
        STRV_FOREACH(s, l) {
                જો (s != l)
                        e = stpcpy(e, separator);

                જો (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        પાછા r;
}

પૂર્ણાંક strv_ધકેલો_with_size(અક્ષર ***l, size_t *n, અક્ષર *કિંમત) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        પ્રતિજ્ઞા(l);
        POINTER_MAY_BE_NULL(n);

        જો (!કિંમત)
                પાછા 0;

        size_t size = n ? *n : SIZE_MAX;
        જો (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        જો (size > SIZE_MAX-2)
                પાછા -ENOMEM;

        અક્ષર **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), કદ(અક્ષર*));
        જો (!c)
                પાછા -ENOMEM;

        c[size] = કિંમત;
        c[size+1] = ખાલી;

        *l = c;
        જો (n)
                *n = size + 1;
        પાછા 0;
}

પૂર્ણાંક strv_ધકેલો_pair(અક્ષર ***l, અક્ષર *a, અક્ષર *b) {
        અક્ષર **c;
        size_t n;

        પ્રતિજ્ઞા(l);

        જો (!a && !b)
                પાછા 0;

        n = strv_length(*l);

        /* Check for overflow */
        જો (n > SIZE_MAX-3)
                પાછા -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), કદ(અક્ષર*));
        જો (!c)
                પાછા -ENOMEM;

        જો (a)
                c[n++] = a;
        જો (b)
                c[n++] = b;
        c[n] = ખાલી;

        *l = c;
        પાછા 0;
}

પૂર્ણાંક strv_ઉમેરો(અક્ષર ***l, size_t position, અક્ષર *કિંમત) {
        અક્ષર **c;
        size_t n, m;

        પ્રતિજ્ઞા(l);

        જો (!કિંમત)
                પાછા 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        જો (n > SIZE_MAX - 2)
                પાછા -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), કદ(અક્ષર*));
        જો (!c)
                પાછા -ENOMEM;

        જો (n > position)
                memmove(c + position + 1, c + position, (n - position) * કદ(અક્ષર*));

        c[position] = કિંમત;
        c[n + 1] = ખાલી;

        *l = c;
        પાછા 0;
}

પૂર્ણાંક strv_consume_with_size(અક્ષર ***l, size_t *n, અક્ષર *કિંમત) {
        પૂર્ણાંક r;

        r = strv_ધકેલો_with_size(l, n, કિંમત);
        જો (r < 0)
                મુક્ત(કિંમત);

        પાછા r;
}

પૂર્ણાંક strv_consume_pair(અક્ષર ***l, અક્ષર *a, અક્ષર *b) {
        પૂર્ણાંક r;

        r = strv_ધકેલો_pair(l, a, b);
        જો (r < 0) {
                મુક્ત(a);
                મુક્ત(b);
        }

        પાછા r;
}

પૂર્ણાંક strv_consume_prepend(અક્ષર ***l, અક્ષર *કિંમત) {
        પૂર્ણાંક r;

        r = strv_ધકેલો_prepend(l, કિંમત);
        જો (r < 0)
                મુક્ત(કિંમત);

        પાછા r;
}

પૂર્ણાંક strv_prepend(અક્ષર ***l, અચલ અક્ષર *કિંમત) {
        અક્ષર *v;

        જો (!કિંમત)
                પાછા 0;

        v = strdup(કિંમત);
        જો (!v)
                પાછા -ENOMEM;

        પાછા strv_consume_prepend(l, v);
}

પૂર્ણાંક strv_extend_with_size(અક્ષર ***l, size_t *n, અચલ અક્ષર *કિંમત) {
        અક્ષર *v;

        જો (!કિંમત)
                પાછા 0;

        v = strdup(કિંમત);
        જો (!v)
                પાછા -ENOMEM;

        પાછા strv_consume_with_size(l, n, v);
}

પૂર્ણાંક strv_extend_many_internal(અક્ષર ***l, અચલ અક્ષર *કિંમત, ...) {
        va_list ap;
        size_t n, m;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, કિંમત);
        માટે (અચલ અક્ષર *s = કિંમત; s != POINTER_MAX; s = va_arg(ap, અચલ અક્ષર*)) {
                જો (!s)
                        ચાલુ;

                જો (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        તોડો;
                }
                m++;
        }
        va_end(ap);

        જો (r < 0)
                પાછા r;
        જો (m > SIZE_MAX-1)
                પાછા -ENOMEM;

        અક્ષર **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), કદ(અક્ષર*));
        જો (!c)
                પાછા -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, કિંમત);
        માટે (અચલ અક્ષર *s = કિંમત; s != POINTER_MAX; s = va_arg(ap, અચલ અક્ષર*)) {
                જો (!s)
                        ચાલુ;

                c[i] = strdup(s);
                જો (!c[i]) {
                        r = -ENOMEM;
                        તોડો;
                }
                i++;
        }
        va_end(ap);

        જો (r < 0) {
                /* rollback on error */
                માટે (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                પાછા r;
        }

        c[i] = ખાલી;
        પાછા 0;
}

અક્ષર** strv_uniq(અક્ષર **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        પાછા l;
}

bool strv_is_uniq(અક્ષર * અચલ *l) {
        STRV_FOREACH(i, l)
                જો (strv_contains(i+1, *i))
                        પાછા false;

        પાછા true;
}

અક્ષર** strv_remove(અક્ષર **l, અચલ અક્ષર *s) {
        જો (!l)
                પાછા ખાલી;

        પ્રતિજ્ઞા(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        અક્ષર **f, **t;
        માટે (f = t = l; *f; f++)
                જો (streq(*f, s))
                        મુક્ત(*f);
                નહીંતર
                        *(t++) = *f;

        *t = ખાલી;
        પાછા l;
}

અક્ષર** strv_remove_strv(અક્ષર **l, અક્ષર *અચલ*ll) {

        જો (strv_isempty(l))
                પાછા l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        પાછા l;
}

bool strv_overlap(અક્ષર * અચલ *a, અક્ષર * અચલ *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                જો (strv_contains(b, *i))
                        પાછા true;

        પાછા false;
}

સ્થિર પૂર્ણાંક str_compare(અક્ષર * અચલ *a, અક્ષર * અચલ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        પાછા strcmp(*a, *b);
}

અક્ષર** strv_sort(અક્ષર **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        પાછા l;
}

અક્ષર** strv_sort_uniq_full(અક્ષર **l, પૂર્ણાંક (*compare)(અક્ષર * અચલ *a, અક્ષર * અચલ *b)) {
        પ્રતિજ્ઞા(compare);

        જો (strv_isempty(l))
                પાછા l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        અક્ષર **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                જો (compare(i, tail - 1) == 0)
                        મુક્ત(*i);
                નહીંતર
                        *(tail++) = *i;

        *tail = ખાલી;
        પાછા l;
}

અક્ષર** strv_sort_uniq(અક્ષર **l) {
        પાછા strv_sort_uniq_full(l, str_compare);
}

પૂર્ણાંક strv_compare(અક્ષર * અચલ *a, અક્ષર * અચલ *b) {
        પૂર્ણાંક r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        જો (strv_isempty(a)) {
                જો (strv_isempty(b))
                        પાછા 0;
                નહીંતર
                        પાછા -1;
        }

        જો (strv_isempty(b))
                પાછા 1;

        માટે ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                જો (r != 0)
                        પાછા r;
        }

        પાછા 0;
}

bool strv_equal_ignore_order(અક્ષર * અચલ *a, અક્ષર * અચલ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        જો (a == b)
                પાછા true;

        STRV_FOREACH(i, a)
                જો (!strv_contains(b, *i))
                        પાછા false;

        STRV_FOREACH(i, b)
                જો (!strv_contains(a, *i))
                        પાછા false;

        પાછા true;
}

શૂન્ય strv_print_full(અક્ષર * અચલ *l, અચલ અક્ષર *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

પૂર્ણાંક strv_extendf_with_size(અક્ષર ***l, size_t *n, અચલ અક્ષર *format, ...) {
        va_list ap;
        અક્ષર *x;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        જો (r < 0)
                પાછા -ENOMEM;

        પાછા strv_consume_with_size(l, n, x);
}

પૂર્ણાંક strv_extend_joined_with_size_sentinel(અક્ષર ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        અક્ષર *x = strextendv_with_separator(/* x= */ ખાલી, /* separator= */ ખાલી, ap);
        va_end(ap);
        જો (!x)
                પાછા -ENOMEM;

        પાછા strv_consume_with_size(l, n, x);
}

અક્ષર* startswith_strv_internal(અચલ અક્ષર *s, અક્ષર * અચલ *l) {
        STRV_FOREACH(i, l) {
                અક્ષર *found = (અક્ષર*) startswith(s, *i);
                જો (found)
                        પાછા found;
        }

        પાછા ખાલી;
}

અક્ષર* endswith_strv_internal(અચલ અક્ષર *s, અક્ષર * અચલ *l) {
        STRV_FOREACH(i, l) {
                અક્ષર *found = (અક્ષર*) endswith(s, *i);
                જો (found)
                        પાછા found;
        }

        પાછા ખાલી;
}

અક્ષર** strv_reverse(અક્ષર **l) {
        size_t n;

        n = strv_length(l);
        જો (n <= 1)
                પાછા l;

        માટે (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        પાછા l;
}

અક્ષર** strv_shell_escape(અક્ષર **l, અચલ અક્ષર *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                અક્ષર *v;

                v = shell_escape(*s, bad);
                જો (!v)
                        પાછા ખાલી;

                free_and_replace(*s, v);
        }

        પાછા l;
}

bool strv_fnmatch_full(
                અક્ષર* અચલ* patterns,
                અચલ અક્ષર *s,
                પૂર્ણાંક flags,
                size_t *ret_matched_pos) {

        પ્રતિજ્ઞા(s);

        જો (patterns)
                માટે (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        જો (fnmatch(patterns[i], s, flags) == 0) {
                                જો (ret_matched_pos)
                                        *ret_matched_pos = i;
                                પાછા true;
                        }

        જો (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        પાછા false;
}

અક્ષર** strv_skip(અક્ષર **l, size_t n) {
        જ્યારે (n > 0) {
                જો (strv_isempty(l))
                        પાછા ખાલી;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        જો (strv_isempty(l))
                પાછા ખાલી;
        પાછા l;
}

પૂર્ણાંક strv_extend_n(અક્ષર ***l, અચલ અક્ષર *કિંમત, size_t n) {
        size_t i, k;
        અક્ષર **nl;

        પ્રતિજ્ઞા(l);

        જો (!કિંમત)
                પાછા 0;
        જો (n == 0)
                પાછા 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        જો (n >= SIZE_MAX - k)
                પાછા -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), કદ(અક્ષર *));
        જો (!nl)
                પાછા -ENOMEM;

        *l = nl;

        માટે (i = k; i < k + n; i++) {
                nl[i] = strdup(કિંમત);
                જો (!nl[i])
                        જાઓ rollback;
        }
        nl[i] = ખાલી;

        પાછા 0;

rollback:
        માટે (size_t j = k; j < i; j++)
                મુક્ત(nl[j]);
        nl[k] = ખાલી;

        પાછા -ENOMEM;
}

પૂર્ણાંક strv_extend_assignment(અક્ષર ***l, અચલ અક્ષર *lhs, અચલ અક્ષર *rhs) {
        અક્ષર *j;

        પ્રતિજ્ઞા(l);
        પ્રતિજ્ઞા(lhs);

        જો (!rhs) /* value is optional, in which case we suppress the field */
                પાછા 0;

        j = strjoin(lhs, "=", rhs);
        જો (!j)
                પાછા -ENOMEM;

        પાછા strv_consume(l, j);
}

પૂર્ણાંક fputstrv(FILE *f, અક્ષર * અચલ *l, અચલ અક્ષર *separator, bool *space) {
        bool b = false;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        જો (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

શૂન્ય string_strv_hashmap_remove(Hashmap *h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પ્રતિજ્ઞા(ચાવી);

        જો (કિંમત) {
                અક્ષર **l = hashmap_get(h, ચાવી);
                જો (!l)
                        પાછા;

                strv_remove(l, કિંમત);
                જો (!strv_isempty(l))
                        પાછા;
        }

        _unused_ _cleanup_free_ અક્ષર *ચાવી_free = ખાલી;
        strv_free(hashmap_remove2(h, ચાવી, (શૂન્ય**) &ચાવી_free));
}

શૂન્ય string_strv_ordered_hashmap_remove(OrderedHashmap *h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), ચાવી, કિંમત);
}

સ્થિર પૂર્ણાંક string_strv_hashmap_put_internal(Hashmap *h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        અક્ષર **l;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(કિંમત);

        l = hashmap_get(h, ચાવી);
        જો (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                જો (strv_contains(l, કિંમત))
                        પાછા 0;

                r = strv_extend(&l, કિંમત);
                જો (r < 0)
                        પાછા r;

                assert_se(hashmap_update(h, ચાવી, l) >= 0);
        } નહીંતર {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ અક્ષર **l2 = ખાલી;
                _cleanup_free_ અક્ષર *t = ખાલી;

                t = strdup(ચાવી);
                જો (!t)
                        પાછા -ENOMEM;

                r = strv_extend(&l2, કિંમત);
                જો (r < 0)
                        પાછા r;

                r = hashmap_put(h, t, l2);
                જો (r < 0)
                        પાછા r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        પાછા 1;
}

પૂર્ણાંક string_strv_hashmap_put(Hashmap **h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(કિંમત);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        જો (r < 0)
                પાછા r;

        પાછા string_strv_hashmap_put_internal(*h, ચાવી, કિંમત);
}

પૂર્ણાંક string_strv_ordered_hashmap_put(OrderedHashmap **h, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(કિંમત);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        જો (r < 0)
                પાછા r;

        પાછા string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), ચાવી, કિંમત);
}

સ્થિર પૂર્ણાંક rebreak_flush_લીટી(
                અક્ષર ***broken,
                અચલ અક્ષર *start,
                અચલ અક્ષર *end,
                bool in_prefix,
                અચલ અક્ષર *whitespace_begin,
                અચલ અક્ષર *whitespace_end) {

        પ્રતિજ્ઞા(broken);
        પ્રતિજ્ઞા(start);
        પ્રતિજ્ઞા(end >= start);

        જો (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                પાછા strv_extend(broken, "");

        જો (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ અક્ષર *truncated = strndup(start, end - start);
        જો (!truncated)
                પાછા -ENOMEM;

        પાછા strv_consume(broken, TAKE_PTR(truncated));
}

પૂર્ણાંક strv_rebreak_લીટીઓ(અક્ષર **l, size_t width, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **broken = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        જો (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                જો (!broken)
                        પાછા -ENOMEM;

                *ret = TAKE_PTR(broken);
                પાછા 0;
        }

        STRV_FOREACH(i, l) {
                અચલ અક્ષર *start = *i, *whitespace_begin = ખાલી, *whitespace_end = ખાલી;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                માટે (અચલ અક્ષર *p = start; *p != 0; ) {
                        જો (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_લીટી(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                જો (r < 0)
                                        પાછા r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = ખાલી;
                                w = 0;
                                ચાલુ;
                        } નહીંતર જો (strchr(WHITESPACE, *p)) {
                                જો (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = ખાલી;
                                }
                        } નહીંતર {
                                જો (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        પૂર્ણાંક cw = utf8_char_console_width(p);
                        જો (cw < 0)
                                પાછા log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        જો (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ અક્ષર *truncated = ખાલી;

                                truncated = strndup(start, whitespace_begin - start);
                                જો (!truncated)
                                        પાછા -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                જો (r < 0)
                                        પાછા r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = ખાલી;
                                w = 0;
                                ચાલુ;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                પ્રતિજ્ઞા(start);
                જો (after_newline && in_prefix)
                        ચાલુ;

                r = rebreak_flush_લીટી(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(broken);
        પાછા 0;
}

અક્ષર** strv_filter_prefix(અક્ષર * અચલ *l, અચલ અક્ષર *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        જો (isempty(prefix))
                પાછા strv_copy(l);

        _cleanup_strv_free_ અક્ષર **f = ખાલી;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                જો (!startswith(*i, prefix))
                        ચાલુ;

                જો (strv_extend_with_size(&f, &sz, *i) < 0)
                        પાછા ખાલી;
        }

        પાછા TAKE_PTR(f);
}

અચલ અક્ષર* અચલ strv_empty[] = { ખાલી };
