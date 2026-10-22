/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fnmatch.h>
#অন্তর্ভাৱ <stdio.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "gunicode.h"
#অন্তর্ভাৱ "hashmap.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "memory-util.h"
#অন্তর্ভাৱ "sort-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "utf8.h"

ৱর্ণ* strv_find(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (streq(*i, name))
                        ঘূৰাই *i;

        ঘূৰাই রিক্ত;
}

ৱর্ণ* strv_find_case(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (strcaseeq(*i, name))
                        ঘূৰাই *i;

        ঘূৰাই রিক্ত;
}

ৱর্ণ* strv_find_prefix(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (startswith(*i, name))
                        ঘূৰাই *i;

        ঘূৰাই রিক্ত;
}

ৱর্ণ* strv_find_startswith(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        প্রতিজ্ঞা(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                ৱর্ণ *e;

                e = startswith(*i, name);
                যদি (e)
                        ঘূৰাই e;
        }

        ঘূৰাই রিক্ত;
}

স্থির ৱর্ণ* strv_find_closest_prefix(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        size_t best_distance = SIZE_MAX;
        ৱর্ণ *best = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(s, l) {
                ৱর্ণ *e = startswith(*s, name);
                যদি (!e)
                        অনুৱর্ত;

                size_t n = strlen(e);
                যদি (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ঘূৰাই best;
}

স্থির ৱর্ণ* strv_find_closest_by_levenshtein(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        ssize_t best_distance = SSIZE_MAX;
        ৱর্ণ *best = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                যদি (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ঘূৰাই রিক্ত;
                }

                যদি (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        অনুৱর্ত;

                যদি (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ঘূৰাই best;
}

ৱর্ণ* strv_find_closest(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name) {
        প্রতিজ্ঞা(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        ৱর্ণ *found = strv_find_closest_prefix(l, name);
        যদি (found)
                ঘূৰাই found;

        ঘূৰাই strv_find_closest_by_levenshtein(l, name);
}

ৱর্ণ* strv_find_first_field(ৱর্ণ * ধ্রুৱ *needles, ৱর্ণ * ধ্রুৱ *haystack) {
        STRV_FOREACH(k, needles) {
                ৱর্ণ *মান = strv_env_pairs_get((ৱর্ণ **)haystack, *k);
                যদি (মান)
                        ঘূৰাই মান;
        }

        ঘূৰাই রিক্ত;
}

ৱর্ণ** strv_free(ৱর্ণ **l) {
        STRV_FOREACH(k, l)
                মুক্ত(*k);

        ঘূৰাই mfree(l);
}

ৱর্ণ** strv_free_erase(ৱর্ণ **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ঘূৰাই mfree(l);
}

শূন্য strv_free_many(ৱর্ণ ***strvs, size_t n) {
        প্রতিজ্ঞা(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        মুক্ত(strvs);
}

ৱর্ণ** strv_copy_n(ৱর্ণ * ধ্রুৱ *l, size_t n) {
        _cleanup_strv_free_ ৱর্ণ **result = রিক্ত;
        ৱর্ণ **k;

        result = new(ৱর্ণ*, MIN(strv_length(l), n) + 1);
        যদি (!result)
                ঘূৰাই রিক্ত;

        k = result;
        STRV_FOREACH(i, l) {
                যদি (n == 0)
                        ভঙ্গ;

                *k = strdup(*i);
                যদি (!*k)
                        ঘূৰাই রিক্ত;
                k++;

                যদি (n != SIZE_MAX)
                        n--;
        }

        *k = রিক্ত;
        ঘূৰাই TAKE_PTR(result);
}

পূর্ণাঙ্ক strv_copy_unless_empty(ৱর্ণ * ধ্রুৱ *l, ৱর্ণ ***ret) {
        প্রতিজ্ঞা(ret);

        যদি (strv_isempty(l)) {
                *ret = রিক্ত;
                ঘূৰাই 0;
        }

        ৱর্ণ **copy = strv_copy(l);
        যদি (!copy)
                ঘূৰাই -ENOMEM;

        *ret = TAKE_PTR(copy);
        ঘূৰাই 1;
}

size_t strv_length(ৱর্ণ * ধ্রুৱ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ঘূৰাই n;
}

ৱর্ণ** strv_new_ap(ধ্রুৱ ৱর্ণ *x, va_list ap) {
        _cleanup_strv_free_ ৱর্ণ **a = রিক্ত;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        বাবে (ধ্রুৱ ৱর্ণ *s = x; s; s = va_arg(aq, ধ্রুৱ ৱর্ণ*)) {
                যদি (s == STRV_IGNORE)
                        অনুৱর্ত;

                n++;
        }
        va_end(aq);

        a = new(ৱর্ণ*, n+1);
        যদি (!a)
                ঘূৰাই রিক্ত;

        বাবে (ধ্রুৱ ৱর্ণ *s = x; s; s = va_arg(ap, ধ্রুৱ ৱর্ণ*)) {
                যদি (s == STRV_IGNORE)
                        অনুৱর্ত;

                a[i] = strdup(s);
                যদি (!a[i])
                        ঘূৰাই রিক্ত;

                i++;
        }

        a[i] = রিক্ত;

        ঘূৰাই TAKE_PTR(a);
}

ৱর্ণ** strv_new_internal(ধ্রুৱ ৱর্ণ *x, ...) {
        ৱর্ণ **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক strv_extend_strv(ৱর্ণ ***a, ৱর্ণ * ধ্রুৱ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        প্রতিজ্ঞা(a);

        q = strv_length(b);
        যদি (q == 0)
                ঘূৰাই 0;

        p = strv_length(*a);
        যদি (p >= SIZE_MAX - q)
                ঘূৰাই -ENOMEM;

        ৱর্ণ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), পরিমাণ(ৱর্ণ *));
        যদি (!t)
                ঘূৰাই -ENOMEM;

        t[p] = রিক্ত;
        *a = t;

        STRV_FOREACH(s, b) {
                যদি (filter_duplicates && strv_contains(t, *s))
                        অনুৱর্ত;

                t[p+i] = strdup(*s);
                যদি (!t[p+i])
                        গচ্ছ rollback;

                i++;
                t[p+i] = রিক্ত;
        }

        প্রতিজ্ঞা(i <= q);

        ঘূৰাই (পূর্ণাঙ্ক) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = রিক্ত;
        ঘূৰাই -ENOMEM;
}

পূর্ণাঙ্ক strv_extend_strv_consume(ৱর্ণ ***a, ৱর্ণ **b, bool filter_duplicates) {
        _cleanup_strv_free_ ৱর্ণ **b_consume = b;
        size_t p, q, i;

        প্রতিজ্ঞা(a);

        q = strv_length(b);
        যদি (q == 0)
                ঘূৰাই 0;

        p = strv_length(*a);
        যদি (p == 0) {
                strv_free_and_replace(*a, b_consume);

                যদি (filter_duplicates)
                        strv_uniq(*a);

                ঘূৰাই strv_length(*a);
        }

        যদি (p >= SIZE_MAX - q)
                ঘূৰাই -ENOMEM;

        ৱর্ণ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), পরিমাণ(ৱর্ণ *));
        যদি (!t)
                ঘূৰাই -ENOMEM;

        t[p] = রিক্ত;
        *a = t;

        যদি (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = রিক্ত;
                i = q;
        } অন্যথা {
                i = 0;

                STRV_FOREACH(s, b) {
                        যদি (strv_contains(t, *s)) {
                                মুক্ত(*s);
                                অনুৱর্ত;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = রিক্ত;
                }
        }

        প্রতিজ্ঞা(i <= q);

        b_consume = mfree(b_consume);

        ঘূৰাই (পূর্ণাঙ্ক) i;
}

পূর্ণাঙ্ক strv_extend_strv_biconcat(ৱর্ণ ***a, ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ* ধ্রুৱ *b, ধ্রুৱ ৱর্ণ *suffix) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);

        STRV_FOREACH(s, b) {
                ৱর্ণ *v;

                v = strjoin(strempty(prefix), *s, suffix);
                যদি (!v)
                        ঘূৰাই -ENOMEM;

                r = strv_consume(a, v);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক strv_split_newlines_full(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *s, ExtractFlags flags) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        size_t n;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        যদি (r < 0)
                ঘূৰাই r;

        n = strv_length(l);
        যদি (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ঘূৰাই n;
}

ৱর্ণ** strv_split_newlines(ধ্রুৱ ৱর্ণ *s) {
        ৱর্ণ **ret;

        যদি (strv_split_newlines_full(&ret, s, 0) < 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই ret;
}

পূর্ণাঙ্ক strv_split_full(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, ExtractFlags flags) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *word = রিক্ত;

                r = extract_first_word(&s, &word, separators, flags);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                যদি (!GREEDY_REALLOC(l, n + 2))
                        ঘূৰাই -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = রিক্ত;
        }

        যদি (!l) {
                l = new0(ৱর্ণ*, 1);
                যদি (!l)
                        ঘূৰাই -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ঘূৰাই (পূর্ণাঙ্ক) n;
}

ৱর্ণ** strv_split(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators) {
        ৱর্ণ **ret;

        যদি (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই ret;
}

পূর্ণাঙ্ক strv_split_and_extend_full(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, bool filter_duplicates, ExtractFlags flags) {
        ৱর্ণ **l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        r = strv_split_full(&l, s, separators, flags);
        যদি (r < 0)
                ঘূৰাই r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই (পূর্ণাঙ্ক) strv_length(*t);
}

পূর্ণাঙ্ক strv_split_and_extend(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *separators, bool filter_duplicates) {
        ঘূৰাই strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

পূর্ণাঙ্ক strv_split_colon_pairs(ৱর্ণ ***t, ধ্রুৱ ৱর্ণ *s) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *first = রিক্ত, *second = রিক্ত, *tuple = রিক্ত, *second_or_empty = রিক্ত;

                r = extract_first_word(&s, &tuple, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                ধ্রুৱ ৱর্ণ *p = tuple;
                r = extract_many_শব্দগুলি(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        অনুৱর্ত;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                যদি (!isempty(p))
                        ঘূৰাই -EINVAL;

                second_or_empty = strdup(strempty(second));
                যদি (!second_or_empty)
                        ঘূৰাই -ENOMEM;

                যদি (!GREEDY_REALLOC(l, n + 3))
                        ঘূৰাই -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = রিক্ত;
        }

        যদি (!l) {
                l = new0(ৱর্ণ*, 1);
                যদি (!l)
                        ঘূৰাই -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ঘূৰাই (পূর্ণাঙ্ক) n;
}

ৱর্ণ* strv_join_full(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *separator, ধ্রুৱ ৱর্ণ *prefix) {
        ৱর্ণ *r, *e;
        size_t n, k, m;

        যদি (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                যদি (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(ৱর্ণ, n+1);
        যদি (!r)
                ঘূৰাই রিক্ত;

        e = r;
        STRV_FOREACH(s, l) {
                যদি (s != l)
                        e = stpcpy(e, separator);

                যদি (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ঘূৰাই r;
}

পূর্ণাঙ্ক strv_ঠেলো_with_size(ৱর্ণ ***l, size_t *n, ৱর্ণ *মান) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        প্রতিজ্ঞা(l);
        POINTER_MAY_BE_NULL(n);

        যদি (!মান)
                ঘূৰাই 0;

        size_t size = n ? *n : SIZE_MAX;
        যদি (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        যদি (size > SIZE_MAX-2)
                ঘূৰাই -ENOMEM;

        ৱর্ণ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), পরিমাণ(ৱর্ণ*));
        যদি (!c)
                ঘূৰাই -ENOMEM;

        c[size] = মান;
        c[size+1] = রিক্ত;

        *l = c;
        যদি (n)
                *n = size + 1;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক strv_ঠেলো_pair(ৱর্ণ ***l, ৱর্ণ *a, ৱর্ণ *b) {
        ৱর্ণ **c;
        size_t n;

        প্রতিজ্ঞা(l);

        যদি (!a && !b)
                ঘূৰাই 0;

        n = strv_length(*l);

        /* Check for overflow */
        যদি (n > SIZE_MAX-3)
                ঘূৰাই -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), পরিমাণ(ৱর্ণ*));
        যদি (!c)
                ঘূৰাই -ENOMEM;

        যদি (a)
                c[n++] = a;
        যদি (b)
                c[n++] = b;
        c[n] = রিক্ত;

        *l = c;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক strv_ঢোকাও(ৱর্ণ ***l, size_t position, ৱর্ণ *মান) {
        ৱর্ণ **c;
        size_t n, m;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                ঘূৰাই 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        যদি (n > SIZE_MAX - 2)
                ঘূৰাই -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), পরিমাণ(ৱর্ণ*));
        যদি (!c)
                ঘূৰাই -ENOMEM;

        যদি (n > position)
                memmove(c + position + 1, c + position, (n - position) * পরিমাণ(ৱর্ণ*));

        c[position] = মান;
        c[n + 1] = রিক্ত;

        *l = c;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক strv_consume_with_size(ৱর্ণ ***l, size_t *n, ৱর্ণ *মান) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_with_size(l, n, মান);
        যদি (r < 0)
                মুক্ত(মান);

        ঘূৰাই r;
}

পূর্ণাঙ্ক strv_consume_pair(ৱর্ণ ***l, ৱর্ণ *a, ৱর্ণ *b) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_pair(l, a, b);
        যদি (r < 0) {
                মুক্ত(a);
                মুক্ত(b);
        }

        ঘূৰাই r;
}

পূর্ণাঙ্ক strv_consume_prepend(ৱর্ণ ***l, ৱর্ণ *মান) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_prepend(l, মান);
        যদি (r < 0)
                মুক্ত(মান);

        ঘূৰাই r;
}

পূর্ণাঙ্ক strv_prepend(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান) {
        ৱর্ণ *v;

        যদি (!মান)
                ঘূৰাই 0;

        v = strdup(মান);
        যদি (!v)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume_prepend(l, v);
}

পূর্ণাঙ্ক strv_extend_with_size(ৱর্ণ ***l, size_t *n, ধ্রুৱ ৱর্ণ *মান) {
        ৱর্ণ *v;

        যদি (!মান)
                ঘূৰাই 0;

        v = strdup(মান);
        যদি (!v)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume_with_size(l, n, v);
}

পূর্ণাঙ্ক strv_extend_many_internal(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান, ...) {
        va_list ap;
        size_t n, m;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, মান);
        বাবে (ধ্রুৱ ৱর্ণ *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুৱ ৱর্ণ*)) {
                যদি (!s)
                        অনুৱর্ত;

                যদি (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        ভঙ্গ;
                }
                m++;
        }
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই r;
        যদি (m > SIZE_MAX-1)
                ঘূৰাই -ENOMEM;

        ৱর্ণ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), পরিমাণ(ৱর্ণ*));
        যদি (!c)
                ঘূৰাই -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, মান);
        বাবে (ধ্রুৱ ৱর্ণ *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুৱ ৱর্ণ*)) {
                যদি (!s)
                        অনুৱর্ত;

                c[i] = strdup(s);
                যদি (!c[i]) {
                        r = -ENOMEM;
                        ভঙ্গ;
                }
                i++;
        }
        va_end(ap);

        যদি (r < 0) {
                /* rollback on error */
                বাবে (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ঘূৰাই r;
        }

        c[i] = রিক্ত;
        ঘূৰাই 0;
}

ৱর্ণ** strv_uniq(ৱর্ণ **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ঘূৰাই l;
}

bool strv_is_uniq(ৱর্ণ * ধ্রুৱ *l) {
        STRV_FOREACH(i, l)
                যদি (strv_contains(i+1, *i))
                        ঘূৰাই false;

        ঘূৰাই true;
}

ৱর্ণ** strv_remove(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *s) {
        যদি (!l)
                ঘূৰাই রিক্ত;

        প্রতিজ্ঞা(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        ৱর্ণ **f, **t;
        বাবে (f = t = l; *f; f++)
                যদি (streq(*f, s))
                        মুক্ত(*f);
                অন্যথা
                        *(t++) = *f;

        *t = রিক্ত;
        ঘূৰাই l;
}

ৱর্ণ** strv_remove_strv(ৱর্ণ **l, ৱর্ণ *ধ্রুৱ*ll) {

        যদি (strv_isempty(l))
                ঘূৰাই l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ঘূৰাই l;
}

bool strv_overlap(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                যদি (strv_contains(b, *i))
                        ঘূৰাই true;

        ঘূৰাই false;
}

স্থির পূর্ণাঙ্ক str_compare(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ঘূৰাই strcmp(*a, *b);
}

ৱর্ণ** strv_sort(ৱর্ণ **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ঘূৰাই l;
}

ৱর্ণ** strv_sort_uniq_full(ৱর্ণ **l, পূর্ণাঙ্ক (*compare)(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b)) {
        প্রতিজ্ঞা(compare);

        যদি (strv_isempty(l))
                ঘূৰাই l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        ৱর্ণ **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                যদি (compare(i, tail - 1) == 0)
                        মুক্ত(*i);
                অন্যথা
                        *(tail++) = *i;

        *tail = রিক্ত;
        ঘূৰাই l;
}

ৱর্ণ** strv_sort_uniq(ৱর্ণ **l) {
        ঘূৰাই strv_sort_uniq_full(l, str_compare);
}

পূর্ণাঙ্ক strv_compare(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) {
        পূর্ণাঙ্ক r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        যদি (strv_isempty(a)) {
                যদি (strv_isempty(b))
                        ঘূৰাই 0;
                অন্যথা
                        ঘূৰাই -1;
        }

        যদি (strv_isempty(b))
                ঘূৰাই 1;

        বাবে ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                যদি (r != 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 0;
}

bool strv_equal_ignore_order(ৱর্ণ * ধ্রুৱ *a, ৱর্ণ * ধ্রুৱ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        যদি (a == b)
                ঘূৰাই true;

        STRV_FOREACH(i, a)
                যদি (!strv_contains(b, *i))
                        ঘূৰাই false;

        STRV_FOREACH(i, b)
                যদি (!strv_contains(a, *i))
                        ঘূৰাই false;

        ঘূৰাই true;
}

শূন্য strv_print_full(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

পূর্ণাঙ্ক strv_extendf_with_size(ৱর্ণ ***l, size_t *n, ধ্রুৱ ৱর্ণ *format, ...) {
        va_list ap;
        ৱর্ণ *x;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume_with_size(l, n, x);
}

পূর্ণাঙ্ক strv_extend_joined_with_size_sentinel(ৱর্ণ ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        ৱর্ণ *x = strextendv_with_separator(/* x= */ রিক্ত, /* separator= */ রিক্ত, ap);
        va_end(ap);
        যদি (!x)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume_with_size(l, n, x);
}

ৱর্ণ* startswith_strv_internal(ধ্রুৱ ৱর্ণ *s, ৱর্ণ * ধ্রুৱ *l) {
        STRV_FOREACH(i, l) {
                ৱর্ণ *found = (ৱর্ণ*) startswith(s, *i);
                যদি (found)
                        ঘূৰাই found;
        }

        ঘূৰাই রিক্ত;
}

ৱর্ণ* endswith_strv_internal(ধ্রুৱ ৱর্ণ *s, ৱর্ণ * ধ্রুৱ *l) {
        STRV_FOREACH(i, l) {
                ৱর্ণ *found = (ৱর্ণ*) endswith(s, *i);
                যদি (found)
                        ঘূৰাই found;
        }

        ঘূৰাই রিক্ত;
}

ৱর্ণ** strv_reverse(ৱর্ণ **l) {
        size_t n;

        n = strv_length(l);
        যদি (n <= 1)
                ঘূৰাই l;

        বাবে (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ঘূৰাই l;
}

ৱর্ণ** strv_shell_escape(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                ৱর্ণ *v;

                v = shell_escape(*s, bad);
                যদি (!v)
                        ঘূৰাই রিক্ত;

                free_and_replace(*s, v);
        }

        ঘূৰাই l;
}

bool strv_fnmatch_full(
                ৱর্ণ* ধ্রুৱ* patterns,
                ধ্রুৱ ৱর্ণ *s,
                পূর্ণাঙ্ক flags,
                size_t *ret_matched_pos) {

        প্রতিজ্ঞা(s);

        যদি (patterns)
                বাবে (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        যদি (fnmatch(patterns[i], s, flags) == 0) {
                                যদি (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ঘূৰাই true;
                        }

        যদি (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ঘূৰাই false;
}

ৱর্ণ** strv_skip(ৱর্ণ **l, size_t n) {
        যেতিয়ালৈ (n > 0) {
                যদি (strv_isempty(l))
                        ঘূৰাই রিক্ত;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        যদি (strv_isempty(l))
                ঘূৰাই রিক্ত;
        ঘূৰাই l;
}

পূর্ণাঙ্ক strv_extend_n(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *মান, size_t n) {
        size_t i, k;
        ৱর্ণ **nl;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                ঘূৰাই 0;
        যদি (n == 0)
                ঘূৰাই 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        যদি (n >= SIZE_MAX - k)
                ঘূৰাই -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), পরিমাণ(ৱর্ণ *));
        যদি (!nl)
                ঘূৰাই -ENOMEM;

        *l = nl;

        বাবে (i = k; i < k + n; i++) {
                nl[i] = strdup(মান);
                যদি (!nl[i])
                        গচ্ছ rollback;
        }
        nl[i] = রিক্ত;

        ঘূৰাই 0;

rollback:
        বাবে (size_t j = k; j < i; j++)
                মুক্ত(nl[j]);
        nl[k] = রিক্ত;

        ঘূৰাই -ENOMEM;
}

পূর্ণাঙ্ক strv_extend_assignment(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *lhs, ধ্রুৱ ৱর্ণ *rhs) {
        ৱর্ণ *j;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(lhs);

        যদি (!rhs) /* value is optional, in which case we suppress the field */
                ঘূৰাই 0;

        j = strjoin(lhs, "=", rhs);
        যদি (!j)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume(l, j);
}

পূর্ণাঙ্ক fputstrv(FILE *f, ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *separator, bool *space) {
        bool b = false;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        যদি (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 0;
}

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        প্রতিজ্ঞা(চাবি);

        যদি (মান) {
                ৱর্ণ **l = hashmap_get(h, চাবি);
                যদি (!l)
                        ঘূৰাই;

                strv_remove(l, মান);
                যদি (!strv_isempty(l))
                        ঘূৰাই;
        }

        _unused_ _cleanup_free_ ৱর্ণ *চাবি_free = রিক্ত;
        strv_free(hashmap_remove2(h, চাবি, (শূন্য**) &চাবি_free));
}

শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), চাবি, মান);
}

স্থির পূর্ণাঙ্ক string_strv_hashmap_put_internal(Hashmap *h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        ৱর্ণ **l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        l = hashmap_get(h, চাবি);
        যদি (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                যদি (strv_contains(l, মান))
                        ঘূৰাই 0;

                r = strv_extend(&l, মান);
                যদি (r < 0)
                        ঘূৰাই r;

                assert_se(hashmap_update(h, চাবি, l) >= 0);
        } অন্যথা {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ ৱর্ণ **l2 = রিক্ত;
                _cleanup_free_ ৱর্ণ *t = রিক্ত;

                t = strdup(চাবি);
                যদি (!t)
                        ঘূৰাই -ENOMEM;

                r = strv_extend(&l2, মান);
                যদি (r < 0)
                        ঘূৰাই r;

                r = hashmap_put(h, t, l2);
                যদি (r < 0)
                        ঘূৰাই r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ঘূৰাই 1;
}

পূর্ণাঙ্ক string_strv_hashmap_put(Hashmap **h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই string_strv_hashmap_put_internal(*h, চাবি, মান);
}

পূর্ণাঙ্ক string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), চাবি, মান);
}

স্থির পূর্ণাঙ্ক rebreak_flush_সাৰি(
                ৱর্ণ ***broken,
                ধ্রুৱ ৱর্ণ *start,
                ধ্রুৱ ৱর্ণ *end,
                bool in_prefix,
                ধ্রুৱ ৱর্ণ *whitespace_begin,
                ধ্রুৱ ৱর্ণ *whitespace_end) {

        প্রতিজ্ঞা(broken);
        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(end >= start);

        যদি (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ঘূৰাই strv_extend(broken, "");

        যদি (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ ৱর্ণ *truncated = strndup(start, end - start);
        যদি (!truncated)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume(broken, TAKE_PTR(truncated));
}

পূর্ণাঙ্ক strv_rebreak_সাৰিগুলি(ৱর্ণ **l, size_t width, ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **broken = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        যদি (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                যদি (!broken)
                        ঘূৰাই -ENOMEM;

                *ret = TAKE_PTR(broken);
                ঘূৰাই 0;
        }

        STRV_FOREACH(i, l) {
                ধ্রুৱ ৱর্ণ *start = *i, *whitespace_begin = রিক্ত, *whitespace_end = রিক্ত;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                বাবে (ধ্রুৱ ৱর্ণ *p = start; *p != 0; ) {
                        যদি (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_সাৰি(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = রিক্ত;
                                w = 0;
                                অনুৱর্ত;
                        } অন্যথা যদি (strchr(WHITESPACE, *p)) {
                                যদি (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = রিক্ত;
                                }
                        } অন্যথা {
                                যদি (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        পূর্ণাঙ্ক cw = utf8_char_console_width(p);
                        যদি (cw < 0)
                                ঘূৰাই log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        যদি (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ ৱর্ণ *truncated = রিক্ত;

                                truncated = strndup(start, whitespace_begin - start);
                                যদি (!truncated)
                                        ঘূৰাই -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                যদি (r < 0)
                                        ঘূৰাই r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = রিক্ত;
                                w = 0;
                                অনুৱর্ত;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                প্রতিজ্ঞা(start);
                যদি (after_newline && in_prefix)
                        অনুৱর্ত;

                r = rebreak_flush_সাৰি(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(broken);
        ঘূৰাই 0;
}

ৱর্ণ** strv_filter_prefix(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        যদি (isempty(prefix))
                ঘূৰাই strv_copy(l);

        _cleanup_strv_free_ ৱর্ণ **f = রিক্ত;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                যদি (!startswith(*i, prefix))
                        অনুৱর্ত;

                যদি (strv_extend_with_size(&f, &sz, *i) < 0)
                        ঘূৰাই রিক্ত;
        }

        ঘূৰাই TAKE_PTR(f);
}

ধ্রুৱ ৱর্ণ* ধ্রুৱ strv_empty[] = { রিক্ত };
