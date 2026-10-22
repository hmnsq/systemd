/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fnmatch.h>
#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "gunicode.h"
#অন্তর্ভাব "hashmap.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "sort-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"

বর্ণ* strv_find(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (streq(*i, name))
                        প্রত্যাগম *i;

        প্রত্যাগম রিক্ত;
}

বর্ণ* strv_find_case(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (strcaseeq(*i, name))
                        প্রত্যাগম *i;

        প্রত্যাগম রিক্ত;
}

বর্ণ* strv_find_prefix(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (startswith(*i, name))
                        প্রত্যাগম *i;

        প্রত্যাগম রিক্ত;
}

বর্ণ* strv_find_startswith(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        প্রতিজ্ঞা(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                বর্ণ *e;

                e = startswith(*i, name);
                যদি (e)
                        প্রত্যাগম e;
        }

        প্রত্যাগম রিক্ত;
}

স্থির বর্ণ* strv_find_closest_prefix(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        size_t best_distance = SIZE_MAX;
        বর্ণ *best = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(s, l) {
                বর্ণ *e = startswith(*s, name);
                যদি (!e)
                        অনুবর্ত;

                size_t n = strlen(e);
                যদি (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        প্রত্যাগম best;
}

স্থির বর্ণ* strv_find_closest_by_levenshtein(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        ssize_t best_distance = SSIZE_MAX;
        বর্ণ *best = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                যদি (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        প্রত্যাগম রিক্ত;
                }

                যদি (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        অনুবর্ত;

                যদি (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        প্রত্যাগম best;
}

বর্ণ* strv_find_closest(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name) {
        প্রতিজ্ঞা(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        বর্ণ *found = strv_find_closest_prefix(l, name);
        যদি (found)
                প্রত্যাগম found;

        প্রত্যাগম strv_find_closest_by_levenshtein(l, name);
}

বর্ণ* strv_find_first_field(বর্ণ * ধ্রুব *needles, বর্ণ * ধ্রুব *haystack) {
        STRV_FOREACH(k, needles) {
                বর্ণ *মান = strv_env_pairs_get((বর্ণ **)haystack, *k);
                যদি (মান)
                        প্রত্যাগম মান;
        }

        প্রত্যাগম রিক্ত;
}

বর্ণ** strv_free(বর্ণ **l) {
        STRV_FOREACH(k, l)
                মুক্ত(*k);

        প্রত্যাগম mfree(l);
}

বর্ণ** strv_free_erase(বর্ণ **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        প্রত্যাগম mfree(l);
}

শূন্য strv_free_many(বর্ণ ***strvs, size_t n) {
        প্রতিজ্ঞা(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        মুক্ত(strvs);
}

বর্ণ** strv_copy_n(বর্ণ * ধ্রুব *l, size_t n) {
        _cleanup_strv_free_ বর্ণ **result = রিক্ত;
        বর্ণ **k;

        result = new(বর্ণ*, MIN(strv_length(l), n) + 1);
        যদি (!result)
                প্রত্যাগম রিক্ত;

        k = result;
        STRV_FOREACH(i, l) {
                যদি (n == 0)
                        ভঙ্গ;

                *k = strdup(*i);
                যদি (!*k)
                        প্রত্যাগম রিক্ত;
                k++;

                যদি (n != SIZE_MAX)
                        n--;
        }

        *k = রিক্ত;
        প্রত্যাগম TAKE_PTR(result);
}

পূর্ণাঙ্ক strv_copy_unless_empty(বর্ণ * ধ্রুব *l, বর্ণ ***ret) {
        প্রতিজ্ঞা(ret);

        যদি (strv_isempty(l)) {
                *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        বর্ণ **copy = strv_copy(l);
        যদি (!copy)
                প্রত্যাগম -ENOMEM;

        *ret = TAKE_PTR(copy);
        প্রত্যাগম 1;
}

size_t strv_length(বর্ণ * ধ্রুব *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        প্রত্যাগম n;
}

বর্ণ** strv_new_ap(ধ্রুব বর্ণ *x, va_list ap) {
        _cleanup_strv_free_ বর্ণ **a = রিক্ত;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        প্রতি (ধ্রুব বর্ণ *s = x; s; s = va_arg(aq, ধ্রুব বর্ণ*)) {
                যদি (s == STRV_IGNORE)
                        অনুবর্ত;

                n++;
        }
        va_end(aq);

        a = new(বর্ণ*, n+1);
        যদি (!a)
                প্রত্যাগম রিক্ত;

        প্রতি (ধ্রুব বর্ণ *s = x; s; s = va_arg(ap, ধ্রুব বর্ণ*)) {
                যদি (s == STRV_IGNORE)
                        অনুবর্ত;

                a[i] = strdup(s);
                যদি (!a[i])
                        প্রত্যাগম রিক্ত;

                i++;
        }

        a[i] = রিক্ত;

        প্রত্যাগম TAKE_PTR(a);
}

বর্ণ** strv_new_internal(ধ্রুব বর্ণ *x, ...) {
        বর্ণ **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক strv_extend_strv(বর্ণ ***a, বর্ণ * ধ্রুব *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        প্রতিজ্ঞা(a);

        q = strv_length(b);
        যদি (q == 0)
                প্রত্যাগম 0;

        p = strv_length(*a);
        যদি (p >= SIZE_MAX - q)
                প্রত্যাগম -ENOMEM;

        বর্ণ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), পরিমাণ(বর্ণ *));
        যদি (!t)
                প্রত্যাগম -ENOMEM;

        t[p] = রিক্ত;
        *a = t;

        STRV_FOREACH(s, b) {
                যদি (filter_duplicates && strv_contains(t, *s))
                        অনুবর্ত;

                t[p+i] = strdup(*s);
                যদি (!t[p+i])
                        গচ্ছ rollback;

                i++;
                t[p+i] = রিক্ত;
        }

        প্রতিজ্ঞা(i <= q);

        প্রত্যাগম (পূর্ণাঙ্ক) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = রিক্ত;
        প্রত্যাগম -ENOMEM;
}

পূর্ণাঙ্ক strv_extend_strv_consume(বর্ণ ***a, বর্ণ **b, bool filter_duplicates) {
        _cleanup_strv_free_ বর্ণ **b_consume = b;
        size_t p, q, i;

        প্রতিজ্ঞা(a);

        q = strv_length(b);
        যদি (q == 0)
                প্রত্যাগম 0;

        p = strv_length(*a);
        যদি (p == 0) {
                strv_free_and_replace(*a, b_consume);

                যদি (filter_duplicates)
                        strv_uniq(*a);

                প্রত্যাগম strv_length(*a);
        }

        যদি (p >= SIZE_MAX - q)
                প্রত্যাগম -ENOMEM;

        বর্ণ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), পরিমাণ(বর্ণ *));
        যদি (!t)
                প্রত্যাগম -ENOMEM;

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
                                অনুবর্ত;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = রিক্ত;
                }
        }

        প্রতিজ্ঞা(i <= q);

        b_consume = mfree(b_consume);

        প্রত্যাগম (পূর্ণাঙ্ক) i;
}

পূর্ণাঙ্ক strv_extend_strv_biconcat(বর্ণ ***a, ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ* ধ্রুব *b, ধ্রুব বর্ণ *suffix) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);

        STRV_FOREACH(s, b) {
                বর্ণ *v;

                v = strjoin(strempty(prefix), *s, suffix);
                যদি (!v)
                        প্রত্যাগম -ENOMEM;

                r = strv_consume(a, v);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক strv_split_newlines_full(বর্ণ ***ret, ধ্রুব বর্ণ *s, ExtractFlags flags) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        size_t n;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        যদি (r < 0)
                প্রত্যাগম r;

        n = strv_length(l);
        যদি (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        প্রত্যাগম n;
}

বর্ণ** strv_split_newlines(ধ্রুব বর্ণ *s) {
        বর্ণ **ret;

        যদি (strv_split_newlines_full(&ret, s, 0) < 0)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক strv_split_full(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, ExtractFlags flags) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *word = রিক্ত;

                r = extract_first_word(&s, &word, separators, flags);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                যদি (!GREEDY_REALLOC(l, n + 2))
                        প্রত্যাগম -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = রিক্ত;
        }

        যদি (!l) {
                l = new0(বর্ণ*, 1);
                যদি (!l)
                        প্রত্যাগম -ENOMEM;
        }

        *t = TAKE_PTR(l);

        প্রত্যাগম (পূর্ণাঙ্ক) n;
}

বর্ণ** strv_split(ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators) {
        বর্ণ **ret;

        যদি (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক strv_split_and_extend_full(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, bool filter_duplicates, ExtractFlags flags) {
        বর্ণ **l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        r = strv_split_full(&l, s, separators, flags);
        যদি (r < 0)
                প্রত্যাগম r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম (পূর্ণাঙ্ক) strv_length(*t);
}

পূর্ণাঙ্ক strv_split_and_extend(বর্ণ ***t, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separators, bool filter_duplicates) {
        প্রত্যাগম strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

পূর্ণাঙ্ক strv_split_colon_pairs(বর্ণ ***t, ধ্রুব বর্ণ *s) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *first = রিক্ত, *second = রিক্ত, *tuple = রিক্ত, *second_or_empty = রিক্ত;

                r = extract_first_word(&s, &tuple, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                ধ্রুব বর্ণ *p = tuple;
                r = extract_many_শব্দগুলি(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        অনুবর্ত;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                যদি (!isempty(p))
                        প্রত্যাগম -EINVAL;

                second_or_empty = strdup(strempty(second));
                যদি (!second_or_empty)
                        প্রত্যাগম -ENOMEM;

                যদি (!GREEDY_REALLOC(l, n + 3))
                        প্রত্যাগম -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = রিক্ত;
        }

        যদি (!l) {
                l = new0(বর্ণ*, 1);
                যদি (!l)
                        প্রত্যাগম -ENOMEM;
        }

        *t = TAKE_PTR(l);

        প্রত্যাগম (পূর্ণাঙ্ক) n;
}

বর্ণ* strv_join_full(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *separator, ধ্রুব বর্ণ *prefix) {
        বর্ণ *r, *e;
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

        r = new(বর্ণ, n+1);
        যদি (!r)
                প্রত্যাগম রিক্ত;

        e = r;
        STRV_FOREACH(s, l) {
                যদি (s != l)
                        e = stpcpy(e, separator);

                যদি (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        প্রত্যাগম r;
}

পূর্ণাঙ্ক strv_ঠেলো_with_size(বর্ণ ***l, size_t *n, বর্ণ *মান) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        প্রতিজ্ঞা(l);
        POINTER_MAY_BE_NULL(n);

        যদি (!মান)
                প্রত্যাগম 0;

        size_t size = n ? *n : SIZE_MAX;
        যদি (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        যদি (size > SIZE_MAX-2)
                প্রত্যাগম -ENOMEM;

        বর্ণ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), পরিমাণ(বর্ণ*));
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        c[size] = মান;
        c[size+1] = রিক্ত;

        *l = c;
        যদি (n)
                *n = size + 1;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক strv_ঠেলো_pair(বর্ণ ***l, বর্ণ *a, বর্ণ *b) {
        বর্ণ **c;
        size_t n;

        প্রতিজ্ঞা(l);

        যদি (!a && !b)
                প্রত্যাগম 0;

        n = strv_length(*l);

        /* Check for overflow */
        যদি (n > SIZE_MAX-3)
                প্রত্যাগম -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), পরিমাণ(বর্ণ*));
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        যদি (a)
                c[n++] = a;
        যদি (b)
                c[n++] = b;
        c[n] = রিক্ত;

        *l = c;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক strv_ঢোকাও(বর্ণ ***l, size_t position, বর্ণ *মান) {
        বর্ণ **c;
        size_t n, m;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                প্রত্যাগম 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        যদি (n > SIZE_MAX - 2)
                প্রত্যাগম -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), পরিমাণ(বর্ণ*));
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        যদি (n > position)
                memmove(c + position + 1, c + position, (n - position) * পরিমাণ(বর্ণ*));

        c[position] = মান;
        c[n + 1] = রিক্ত;

        *l = c;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক strv_consume_with_size(বর্ণ ***l, size_t *n, বর্ণ *মান) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_with_size(l, n, মান);
        যদি (r < 0)
                মুক্ত(মান);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক strv_consume_pair(বর্ণ ***l, বর্ণ *a, বর্ণ *b) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_pair(l, a, b);
        যদি (r < 0) {
                মুক্ত(a);
                মুক্ত(b);
        }

        প্রত্যাগম r;
}

পূর্ণাঙ্ক strv_consume_prepend(বর্ণ ***l, বর্ণ *মান) {
        পূর্ণাঙ্ক r;

        r = strv_ঠেলো_prepend(l, মান);
        যদি (r < 0)
                মুক্ত(মান);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক strv_prepend(বর্ণ ***l, ধ্রুব বর্ণ *মান) {
        বর্ণ *v;

        যদি (!মান)
                প্রত্যাগম 0;

        v = strdup(মান);
        যদি (!v)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume_prepend(l, v);
}

পূর্ণাঙ্ক strv_extend_with_size(বর্ণ ***l, size_t *n, ধ্রুব বর্ণ *মান) {
        বর্ণ *v;

        যদি (!মান)
                প্রত্যাগম 0;

        v = strdup(মান);
        যদি (!v)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume_with_size(l, n, v);
}

পূর্ণাঙ্ক strv_extend_many_internal(বর্ণ ***l, ধ্রুব বর্ণ *মান, ...) {
        va_list ap;
        size_t n, m;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, মান);
        প্রতি (ধ্রুব বর্ণ *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুব বর্ণ*)) {
                যদি (!s)
                        অনুবর্ত;

                যদি (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        ভঙ্গ;
                }
                m++;
        }
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম r;
        যদি (m > SIZE_MAX-1)
                প্রত্যাগম -ENOMEM;

        বর্ণ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), পরিমাণ(বর্ণ*));
        যদি (!c)
                প্রত্যাগম -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, মান);
        প্রতি (ধ্রুব বর্ণ *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুব বর্ণ*)) {
                যদি (!s)
                        অনুবর্ত;

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
                প্রতি (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                প্রত্যাগম r;
        }

        c[i] = রিক্ত;
        প্রত্যাগম 0;
}

বর্ণ** strv_uniq(বর্ণ **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        প্রত্যাগম l;
}

bool strv_is_uniq(বর্ণ * ধ্রুব *l) {
        STRV_FOREACH(i, l)
                যদি (strv_contains(i+1, *i))
                        প্রত্যাগম false;

        প্রত্যাগম true;
}

বর্ণ** strv_remove(বর্ণ **l, ধ্রুব বর্ণ *s) {
        যদি (!l)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        বর্ণ **f, **t;
        প্রতি (f = t = l; *f; f++)
                যদি (streq(*f, s))
                        মুক্ত(*f);
                অন্যথা
                        *(t++) = *f;

        *t = রিক্ত;
        প্রত্যাগম l;
}

বর্ণ** strv_remove_strv(বর্ণ **l, বর্ণ *ধ্রুব*ll) {

        যদি (strv_isempty(l))
                প্রত্যাগম l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        প্রত্যাগম l;
}

bool strv_overlap(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                যদি (strv_contains(b, *i))
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

স্থির পূর্ণাঙ্ক str_compare(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        প্রত্যাগম strcmp(*a, *b);
}

বর্ণ** strv_sort(বর্ণ **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        প্রত্যাগম l;
}

বর্ণ** strv_sort_uniq_full(বর্ণ **l, পূর্ণাঙ্ক (*compare)(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b)) {
        প্রতিজ্ঞা(compare);

        যদি (strv_isempty(l))
                প্রত্যাগম l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        বর্ণ **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                যদি (compare(i, tail - 1) == 0)
                        মুক্ত(*i);
                অন্যথা
                        *(tail++) = *i;

        *tail = রিক্ত;
        প্রত্যাগম l;
}

বর্ণ** strv_sort_uniq(বর্ণ **l) {
        প্রত্যাগম strv_sort_uniq_full(l, str_compare);
}

পূর্ণাঙ্ক strv_compare(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) {
        পূর্ণাঙ্ক r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        যদি (strv_isempty(a)) {
                যদি (strv_isempty(b))
                        প্রত্যাগম 0;
                অন্যথা
                        প্রত্যাগম -1;
        }

        যদি (strv_isempty(b))
                প্রত্যাগম 1;

        প্রতি ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                যদি (r != 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

bool strv_equal_ignore_order(বর্ণ * ধ্রুব *a, বর্ণ * ধ্রুব *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        যদি (a == b)
                প্রত্যাগম true;

        STRV_FOREACH(i, a)
                যদি (!strv_contains(b, *i))
                        প্রত্যাগম false;

        STRV_FOREACH(i, b)
                যদি (!strv_contains(a, *i))
                        প্রত্যাগম false;

        প্রত্যাগম true;
}

শূন্য strv_print_full(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

পূর্ণাঙ্ক strv_extendf_with_size(বর্ণ ***l, size_t *n, ধ্রুব বর্ণ *format, ...) {
        va_list ap;
        বর্ণ *x;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume_with_size(l, n, x);
}

পূর্ণাঙ্ক strv_extend_joined_with_size_sentinel(বর্ণ ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        বর্ণ *x = strextendv_with_separator(/* x= */ রিক্ত, /* separator= */ রিক্ত, ap);
        va_end(ap);
        যদি (!x)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume_with_size(l, n, x);
}

বর্ণ* startswith_strv_internal(ধ্রুব বর্ণ *s, বর্ণ * ধ্রুব *l) {
        STRV_FOREACH(i, l) {
                বর্ণ *found = (বর্ণ*) startswith(s, *i);
                যদি (found)
                        প্রত্যাগম found;
        }

        প্রত্যাগম রিক্ত;
}

বর্ণ* endswith_strv_internal(ধ্রুব বর্ণ *s, বর্ণ * ধ্রুব *l) {
        STRV_FOREACH(i, l) {
                বর্ণ *found = (বর্ণ*) endswith(s, *i);
                যদি (found)
                        প্রত্যাগম found;
        }

        প্রত্যাগম রিক্ত;
}

বর্ণ** strv_reverse(বর্ণ **l) {
        size_t n;

        n = strv_length(l);
        যদি (n <= 1)
                প্রত্যাগম l;

        প্রতি (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        প্রত্যাগম l;
}

বর্ণ** strv_shell_escape(বর্ণ **l, ধ্রুব বর্ণ *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                বর্ণ *v;

                v = shell_escape(*s, bad);
                যদি (!v)
                        প্রত্যাগম রিক্ত;

                free_and_replace(*s, v);
        }

        প্রত্যাগম l;
}

bool strv_fnmatch_full(
                বর্ণ* ধ্রুব* patterns,
                ধ্রুব বর্ণ *s,
                পূর্ণাঙ্ক flags,
                size_t *ret_matched_pos) {

        প্রতিজ্ঞা(s);

        যদি (patterns)
                প্রতি (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        যদি (fnmatch(patterns[i], s, flags) == 0) {
                                যদি (ret_matched_pos)
                                        *ret_matched_pos = i;
                                প্রত্যাগম true;
                        }

        যদি (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        প্রত্যাগম false;
}

বর্ণ** strv_skip(বর্ণ **l, size_t n) {
        যাবত্ (n > 0) {
                যদি (strv_isempty(l))
                        প্রত্যাগম রিক্ত;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        যদি (strv_isempty(l))
                প্রত্যাগম রিক্ত;
        প্রত্যাগম l;
}

পূর্ণাঙ্ক strv_extend_n(বর্ণ ***l, ধ্রুব বর্ণ *মান, size_t n) {
        size_t i, k;
        বর্ণ **nl;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                প্রত্যাগম 0;
        যদি (n == 0)
                প্রত্যাগম 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        যদি (n >= SIZE_MAX - k)
                প্রত্যাগম -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), পরিমাণ(বর্ণ *));
        যদি (!nl)
                প্রত্যাগম -ENOMEM;

        *l = nl;

        প্রতি (i = k; i < k + n; i++) {
                nl[i] = strdup(মান);
                যদি (!nl[i])
                        গচ্ছ rollback;
        }
        nl[i] = রিক্ত;

        প্রত্যাগম 0;

rollback:
        প্রতি (size_t j = k; j < i; j++)
                মুক্ত(nl[j]);
        nl[k] = রিক্ত;

        প্রত্যাগম -ENOMEM;
}

পূর্ণাঙ্ক strv_extend_assignment(বর্ণ ***l, ধ্রুব বর্ণ *lhs, ধ্রুব বর্ণ *rhs) {
        বর্ণ *j;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(lhs);

        যদি (!rhs) /* value is optional, in which case we suppress the field */
                প্রত্যাগম 0;

        j = strjoin(lhs, "=", rhs);
        যদি (!j)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume(l, j);
}

পূর্ণাঙ্ক fputstrv(FILE *f, বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *separator, bool *space) {
        bool b = false;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        যদি (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        প্রতিজ্ঞা(চাবি);

        যদি (মান) {
                বর্ণ **l = hashmap_get(h, চাবি);
                যদি (!l)
                        প্রত্যাগম;

                strv_remove(l, মান);
                যদি (!strv_isempty(l))
                        প্রত্যাগম;
        }

        _unused_ _cleanup_free_ বর্ণ *চাবি_free = রিক্ত;
        strv_free(hashmap_remove2(h, চাবি, (শূন্য**) &চাবি_free));
}

শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), চাবি, মান);
}

স্থির পূর্ণাঙ্ক string_strv_hashmap_put_internal(Hashmap *h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        বর্ণ **l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        l = hashmap_get(h, চাবি);
        যদি (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                যদি (strv_contains(l, মান))
                        প্রত্যাগম 0;

                r = strv_extend(&l, মান);
                যদি (r < 0)
                        প্রত্যাগম r;

                assert_se(hashmap_update(h, চাবি, l) >= 0);
        } অন্যথা {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ বর্ণ **l2 = রিক্ত;
                _cleanup_free_ বর্ণ *t = রিক্ত;

                t = strdup(চাবি);
                যদি (!t)
                        প্রত্যাগম -ENOMEM;

                r = strv_extend(&l2, মান);
                যদি (r < 0)
                        প্রত্যাগম r;

                r = hashmap_put(h, t, l2);
                যদি (r < 0)
                        প্রত্যাগম r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক string_strv_hashmap_put(Hashmap **h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম string_strv_hashmap_put_internal(*h, চাবি, মান);
}

পূর্ণাঙ্ক string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), চাবি, মান);
}

স্থির পূর্ণাঙ্ক rebreak_flush_সারি(
                বর্ণ ***broken,
                ধ্রুব বর্ণ *start,
                ধ্রুব বর্ণ *end,
                bool in_prefix,
                ধ্রুব বর্ণ *whitespace_begin,
                ধ্রুব বর্ণ *whitespace_end) {

        প্রতিজ্ঞা(broken);
        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(end >= start);

        যদি (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                প্রত্যাগম strv_extend(broken, "");

        যদি (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ বর্ণ *truncated = strndup(start, end - start);
        যদি (!truncated)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume(broken, TAKE_PTR(truncated));
}

পূর্ণাঙ্ক strv_rebreak_সারিগুলি(বর্ণ **l, size_t width, বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **broken = রিক্ত;
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
                        প্রত্যাগম -ENOMEM;

                *ret = TAKE_PTR(broken);
                প্রত্যাগম 0;
        }

        STRV_FOREACH(i, l) {
                ধ্রুব বর্ণ *start = *i, *whitespace_begin = রিক্ত, *whitespace_end = রিক্ত;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                প্রতি (ধ্রুব বর্ণ *p = start; *p != 0; ) {
                        যদি (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_সারি(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = রিক্ত;
                                w = 0;
                                অনুবর্ত;
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
                                প্রত্যাগম log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        যদি (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ বর্ণ *truncated = রিক্ত;

                                truncated = strndup(start, whitespace_begin - start);
                                যদি (!truncated)
                                        প্রত্যাগম -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = রিক্ত;
                                w = 0;
                                অনুবর্ত;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                প্রতিজ্ঞা(start);
                যদি (after_newline && in_prefix)
                        অনুবর্ত;

                r = rebreak_flush_সারি(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        *ret = TAKE_PTR(broken);
        প্রত্যাগম 0;
}

বর্ণ** strv_filter_prefix(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        যদি (isempty(prefix))
                প্রত্যাগম strv_copy(l);

        _cleanup_strv_free_ বর্ণ **f = রিক্ত;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                যদি (!startswith(*i, prefix))
                        অনুবর্ত;

                যদি (strv_extend_with_size(&f, &sz, *i) < 0)
                        প্রত্যাগম রিক্ত;
        }

        প্রত্যাগম TAKE_PTR(f);
}

ধ্রুব বর্ণ* ধ্রুব strv_empty[] = { রিক্ত };
