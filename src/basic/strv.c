/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "gunicode.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ವರ್ಣ* strv_find(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಯದಿ (streq(*i, name))
                        ಪ್ರತ್ಯಾಗಮ *i;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* strv_find_case(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಯದಿ (strcaseeq(*i, name))
                        ಪ್ರತ್ಯಾಗಮ *i;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* strv_find_prefix(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಯದಿ (startswith(*i, name))
                        ಪ್ರತ್ಯಾಗಮ *i;

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* strv_find_startswith(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                ವರ್ಣ *e;

                e = startswith(*i, name);
                ಯದಿ (e)
                        ಪ್ರತ್ಯಾಗಮ e;
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ವರ್ಣ* strv_find_closest_prefix(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        size_t best_distance = SIZE_MAX;
        ವರ್ಣ *best = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(s, l) {
                ವರ್ಣ *e = startswith(*s, name);
                ಯದಿ (!e)
                        ಅನುವರ್ತ;

                size_t n = strlen(e);
                ಯದಿ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ಪ್ರತ್ಯಾಗಮ best;
}

ಸ್ಥಿರ ವರ್ಣ* strv_find_closest_by_levenshtein(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ssize_t best_distance = SSIZE_MAX;
        ವರ್ಣ *best = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                ಯದಿ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }

                ಯದಿ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ಅನುವರ್ತ;

                ಯದಿ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ಪ್ರತ್ಯಾಗಮ best;
}

ವರ್ಣ* strv_find_closest(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        ವರ್ಣ *found = strv_find_closest_prefix(l, name);
        ಯದಿ (found)
                ಪ್ರತ್ಯಾಗಮ found;

        ಪ್ರತ್ಯಾಗಮ strv_find_closest_by_levenshtein(l, name);
}

ವರ್ಣ* strv_find_first_field(ವರ್ಣ * ಧ್ರುವ *needles, ವರ್ಣ * ಧ್ರುವ *haystack) {
        STRV_FOREACH(k, needles) {
                ವರ್ಣ *ಮೌಲ್ಯ = strv_env_pairs_get((ವರ್ಣ **)haystack, *k);
                ಯದಿ (ಮೌಲ್ಯ)
                        ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ** strv_free(ವರ್ಣ **l) {
        STRV_FOREACH(k, l)
                ಮುಕ್ತ(*k);

        ಪ್ರತ್ಯಾಗಮ mfree(l);
}

ವರ್ಣ** strv_free_erase(ವರ್ಣ **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ಪ್ರತ್ಯಾಗಮ mfree(l);
}

ಶೂನ್ಯ strv_free_many(ವರ್ಣ ***strvs, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        ಮುಕ್ತ(strvs);
}

ವರ್ಣ** strv_copy_n(ವರ್ಣ * ಧ್ರುವ *l, size_t n) {
        _cleanup_strv_free_ ವರ್ಣ **result = ರಿಕ್ತ;
        ವರ್ಣ **k;

        result = new(ವರ್ಣ*, MIN(strv_length(l), n) + 1);
        ಯದಿ (!result)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        k = result;
        STRV_FOREACH(i, l) {
                ಯದಿ (n == 0)
                        ಭಙ್ಗ;

                *k = strdup(*i);
                ಯದಿ (!*k)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                k++;

                ಯದಿ (n != SIZE_MAX)
                        n--;
        }

        *k = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(result);
}

ಪೂರ್ಣಾಙ್ಕ strv_copy_unless_empty(ವರ್ಣ * ಧ್ರುವ *l, ವರ್ಣ ***ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (strv_isempty(l)) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ವರ್ಣ **copy = strv_copy(l);
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ಪ್ರತ್ಯಾಗಮ 1;
}

size_t strv_length(ವರ್ಣ * ಧ್ರುವ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ಪ್ರತ್ಯಾಗಮ n;
}

ವರ್ಣ** strv_new_ap(ಧ್ರುವ ವರ್ಣ *x, va_list ap) {
        _cleanup_strv_free_ ವರ್ಣ **a = ರಿಕ್ತ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *s = x; s; s = va_arg(aq, ಧ್ರುವ ವರ್ಣ*)) {
                ಯದಿ (s == STRV_IGNORE)
                        ಅನುವರ್ತ;

                n++;
        }
        va_end(aq);

        a = new(ವರ್ಣ*, n+1);
        ಯದಿ (!a)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *s = x; s; s = va_arg(ap, ಧ್ರುವ ವರ್ಣ*)) {
                ಯದಿ (s == STRV_IGNORE)
                        ಅನುವರ್ತ;

                a[i] = strdup(s);
                ಯದಿ (!a[i])
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                i++;
        }

        a[i] = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(a);
}

ವರ್ಣ** strv_new_internal(ಧ್ರುವ ವರ್ಣ *x, ...) {
        ವರ್ಣ **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_strv(ವರ್ಣ ***a, ವರ್ಣ * ಧ್ರುವ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ಪ್ರತಿಜ್ಞಾ(a);

        q = strv_length(b);
        ಯದಿ (q == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        p = strv_length(*a);
        ಯದಿ (p >= SIZE_MAX - q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), ಪರಿಮಾಣ(ವರ್ಣ *));
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        t[p] = ರಿಕ್ತ;
        *a = t;

        STRV_FOREACH(s, b) {
                ಯದಿ (filter_duplicates && strv_contains(t, *s))
                        ಅನುವರ್ತ;

                t[p+i] = strdup(*s);
                ಯದಿ (!t[p+i])
                        ಗಚ್ಛ rollback;

                i++;
                t[p+i] = ರಿಕ್ತ;
        }

        ಪ್ರತಿಜ್ಞಾ(i <= q);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_strv_consume(ವರ್ಣ ***a, ವರ್ಣ **b, bool filter_duplicates) {
        _cleanup_strv_free_ ವರ್ಣ **b_consume = b;
        size_t p, q, i;

        ಪ್ರತಿಜ್ಞಾ(a);

        q = strv_length(b);
        ಯದಿ (q == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        p = strv_length(*a);
        ಯದಿ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ಯದಿ (filter_duplicates)
                        strv_uniq(*a);

                ಪ್ರತ್ಯಾಗಮ strv_length(*a);
        }

        ಯದಿ (p >= SIZE_MAX - q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), ಪರಿಮಾಣ(ವರ್ಣ *));
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        t[p] = ರಿಕ್ತ;
        *a = t;

        ಯದಿ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ರಿಕ್ತ;
                i = q;
        } ಅನ್ಯಥಾ {
                i = 0;

                STRV_FOREACH(s, b) {
                        ಯದಿ (strv_contains(t, *s)) {
                                ಮುಕ್ತ(*s);
                                ಅನುವರ್ತ;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ರಿಕ್ತ;
                }
        }

        ಪ್ರತಿಜ್ಞಾ(i <= q);

        b_consume = mfree(b_consume);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) i;
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_strv_biconcat(ವರ್ಣ ***a, ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ *b, ಧ್ರುವ ವರ್ಣ *suffix) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);

        STRV_FOREACH(s, b) {
                ವರ್ಣ *v;

                v = strjoin(strempty(prefix), *s, suffix);
                ಯದಿ (!v)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = strv_consume(a, v);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ strv_split_newlines_full(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *s, ExtractFlags flags) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        n = strv_length(l);
        ಯದಿ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ n;
}

ವರ್ಣ** strv_split_newlines(ಧ್ರುವ ವರ್ಣ *s) {
        ವರ್ಣ **ret;

        ಯದಿ (strv_split_newlines_full(&ret, s, 0) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ strv_split_full(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        size_t n = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ;

                r = extract_first_word(&s, &word, separators, flags);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (!GREEDY_REALLOC(l, n + 2))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = ರಿಕ್ತ;
        }

        ಯದಿ (!l) {
                l = new0(ವರ್ಣ*, 1);
                ಯದಿ (!l)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;
}

ವರ್ಣ** strv_split(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators) {
        ವರ್ಣ **ret;

        ಯದಿ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ strv_split_and_extend_full(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, bool filter_duplicates, ExtractFlags flags) {
        ವರ್ಣ **l;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        r = strv_split_full(&l, s, separators, flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) strv_length(*t);
}

ಪೂರ್ಣಾಙ್ಕ strv_split_and_extend(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separators, bool filter_duplicates) {
        ಪ್ರತ್ಯಾಗಮ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

ಪೂರ್ಣಾಙ್ಕ strv_split_colon_pairs(ವರ್ಣ ***t, ಧ್ರುವ ವರ್ಣ *s) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        size_t n = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *first = ರಿಕ್ತ, *second = ರಿಕ್ತ, *tuple = ರಿಕ್ತ, *second_or_empty = ರಿಕ್ತ;

                r = extract_first_word(&s, &tuple, ರಿಕ್ತ, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಧ್ರುವ ವರ್ಣ *p = tuple;
                r = extract_many_ಪದಗಳು(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಅನುವರ್ತ;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ಯದಿ (!isempty(p))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                second_or_empty = strdup(strempty(second));
                ಯದಿ (!second_or_empty)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (!GREEDY_REALLOC(l, n + 3))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = ರಿಕ್ತ;
        }

        ಯದಿ (!l) {
                l = new0(ವರ್ಣ*, 1);
                ಯದಿ (!l)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;
}

ವರ್ಣ* strv_join_full(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *separator, ಧ್ರುವ ವರ್ಣ *prefix) {
        ವರ್ಣ *r, *e;
        size_t n, k, m;

        ಯದಿ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                ಯದಿ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(ವರ್ಣ, n+1);
        ಯದಿ (!r)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = r;
        STRV_FOREACH(s, l) {
                ಯದಿ (s != l)
                        e = stpcpy(e, separator);

                ಯದಿ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು_with_size(ವರ್ಣ ***l, size_t *n, ವರ್ಣ *ಮೌಲ್ಯ) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ಪ್ರತಿಜ್ಞಾ(l);
        POINTER_MAY_BE_NULL(n);

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ 0;

        size_t size = n ? *n : SIZE_MAX;
        ಯದಿ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        ಯದಿ (size > SIZE_MAX-2)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), ಪರಿಮಾಣ(ವರ್ಣ*));
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        c[size] = ಮೌಲ್ಯ;
        c[size+1] = ರಿಕ್ತ;

        *l = c;
        ಯದಿ (n)
                *n = size + 1;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ strv_ತಳ್ಳು_pair(ವರ್ಣ ***l, ವರ್ಣ *a, ವರ್ಣ *b) {
        ವರ್ಣ **c;
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಯದಿ (!a && !b)
                ಪ್ರತ್ಯಾಗಮ 0;

        n = strv_length(*l);

        /* Check for overflow */
        ಯದಿ (n > SIZE_MAX-3)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), ಪರಿಮಾಣ(ವರ್ಣ*));
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (a)
                c[n++] = a;
        ಯದಿ (b)
                c[n++] = b;
        c[n] = ರಿಕ್ತ;

        *l = c;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ strv_ಸೇರಿಸು(ವರ್ಣ ***l, size_t position, ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ **c;
        size_t n, m;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ಯದಿ (n > SIZE_MAX - 2)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), ಪರಿಮಾಣ(ವರ್ಣ*));
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (n > position)
                memmove(c + position + 1, c + position, (n - position) * ಪರಿಮಾಣ(ವರ್ಣ*));

        c[position] = ಮೌಲ್ಯ;
        c[n + 1] = ರಿಕ್ತ;

        *l = c;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ strv_consume_with_size(ವರ್ಣ ***l, size_t *n, ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = strv_ತಳ್ಳು_with_size(l, n, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಮುಕ್ತ(ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ strv_consume_pair(ವರ್ಣ ***l, ವರ್ಣ *a, ವರ್ಣ *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = strv_ತಳ್ಳು_pair(l, a, b);
        ಯದಿ (r < 0) {
                ಮುಕ್ತ(a);
                ಮುಕ್ತ(b);
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ strv_consume_prepend(ವರ್ಣ ***l, ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = strv_ತಳ್ಳು_prepend(l, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಮುಕ್ತ(ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ strv_prepend(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ *v;

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ 0;

        v = strdup(ಮೌಲ್ಯ);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume_prepend(l, v);
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_with_size(ವರ್ಣ ***l, size_t *n, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ *v;

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ 0;

        v = strdup(ಮೌಲ್ಯ);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume_with_size(l, n, v);
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_many_internal(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, ...) {
        va_list ap;
        size_t n, m;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, ಮೌಲ್ಯ);
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *s = ಮೌಲ್ಯ; s != POINTER_MAX; s = va_arg(ap, ಧ್ರುವ ವರ್ಣ*)) {
                ಯದಿ (!s)
                        ಅನುವರ್ತ;

                ಯದಿ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        ಭಙ್ಗ;
                }
                m++;
        }
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (m > SIZE_MAX-1)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), ಪರಿಮಾಣ(ವರ್ಣ*));
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, ಮೌಲ್ಯ);
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *s = ಮೌಲ್ಯ; s != POINTER_MAX; s = va_arg(ap, ಧ್ರುವ ವರ್ಣ*)) {
                ಯದಿ (!s)
                        ಅನುವರ್ತ;

                c[i] = strdup(s);
                ಯದಿ (!c[i]) {
                        r = -ENOMEM;
                        ಭಙ್ಗ;
                }
                i++;
        }
        va_end(ap);

        ಯದಿ (r < 0) {
                /* rollback on error */
                ಪ್ರತಿ (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        c[i] = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ** strv_uniq(ವರ್ಣ **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ಪ್ರತ್ಯಾಗಮ l;
}

bool strv_is_uniq(ವರ್ಣ * ಧ್ರುವ *l) {
        STRV_FOREACH(i, l)
                ಯದಿ (strv_contains(i+1, *i))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ವರ್ಣ** strv_remove(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *s) {
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        ವರ್ಣ **f, **t;
        ಪ್ರತಿ (f = t = l; *f; f++)
                ಯದಿ (streq(*f, s))
                        ಮುಕ್ತ(*f);
                ಅನ್ಯಥಾ
                        *(t++) = *f;

        *t = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** strv_remove_strv(ವರ್ಣ **l, ವರ್ಣ *ಧ್ರುವ*ll) {

        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ಪ್ರತ್ಯಾಗಮ l;
}

bool strv_overlap(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ಯದಿ (strv_contains(b, *i))
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ str_compare(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ಪ್ರತ್ಯಾಗಮ strcmp(*a, *b);
}

ವರ್ಣ** strv_sort(ವರ್ಣ **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** strv_sort_uniq_full(ವರ್ಣ **l, ಪೂರ್ಣಾಙ್ಕ (*compare)(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b)) {
        ಪ್ರತಿಜ್ಞಾ(compare);

        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        ವರ್ಣ **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                ಯದಿ (compare(i, tail - 1) == 0)
                        ಮುಕ್ತ(*i);
                ಅನ್ಯಥಾ
                        *(tail++) = *i;

        *tail = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** strv_sort_uniq(ವರ್ಣ **l) {
        ಪ್ರತ್ಯಾಗಮ strv_sort_uniq_full(l, str_compare);
}

ಪೂರ್ಣಾಙ್ಕ strv_compare(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ಯದಿ (strv_isempty(a)) {
                ಯದಿ (strv_isempty(b))
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ -1;
        }

        ಯದಿ (strv_isempty(b))
                ಪ್ರತ್ಯಾಗಮ 1;

        ಪ್ರತಿ ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ಯದಿ (r != 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool strv_equal_ignore_order(ವರ್ಣ * ಧ್ರುವ *a, ವರ್ಣ * ಧ್ರುವ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ true;

        STRV_FOREACH(i, a)
                ಯದಿ (!strv_contains(b, *i))
                        ಪ್ರತ್ಯಾಗಮ false;

        STRV_FOREACH(i, b)
                ಯದಿ (!strv_contains(a, *i))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಶೂನ್ಯ strv_print_full(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

ಪೂರ್ಣಾಙ್ಕ strv_extendf_with_size(ವರ್ಣ ***l, size_t *n, ಧ್ರುವ ವರ್ಣ *format, ...) {
        va_list ap;
        ವರ್ಣ *x;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume_with_size(l, n, x);
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_joined_with_size_sentinel(ವರ್ಣ ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        ವರ್ಣ *x = strextendv_with_separator(/* x= */ ರಿಕ್ತ, /* separator= */ ರಿಕ್ತ, ap);
        va_end(ap);
        ಯದಿ (!x)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume_with_size(l, n, x);
}

ವರ್ಣ* startswith_strv_internal(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ * ಧ್ರುವ *l) {
        STRV_FOREACH(i, l) {
                ವರ್ಣ *found = (ವರ್ಣ*) startswith(s, *i);
                ಯದಿ (found)
                        ಪ್ರತ್ಯಾಗಮ found;
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* endswith_strv_internal(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ * ಧ್ರುವ *l) {
        STRV_FOREACH(i, l) {
                ವರ್ಣ *found = (ವರ್ಣ*) endswith(s, *i);
                ಯದಿ (found)
                        ಪ್ರತ್ಯಾಗಮ found;
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ** strv_reverse(ವರ್ಣ **l) {
        size_t n;

        n = strv_length(l);
        ಯದಿ (n <= 1)
                ಪ್ರತ್ಯಾಗಮ l;

        ಪ್ರತಿ (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** strv_shell_escape(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                ವರ್ಣ *v;

                v = shell_escape(*s, bad);
                ಯದಿ (!v)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                free_and_replace(*s, v);
        }

        ಪ್ರತ್ಯಾಗಮ l;
}

bool strv_fnmatch_full(
                ವರ್ಣ* ಧ್ರುವ* patterns,
                ಧ್ರುವ ವರ್ಣ *s,
                ಪೂರ್ಣಾಙ್ಕ flags,
                size_t *ret_matched_pos) {

        ಪ್ರತಿಜ್ಞಾ(s);

        ಯದಿ (patterns)
                ಪ್ರತಿ (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ಯದಿ (fnmatch(patterns[i], s, flags) == 0) {
                                ಯದಿ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ಪ್ರತ್ಯಾಗಮ true;
                        }

        ಯದಿ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ಪ್ರತ್ಯಾಗಮ false;
}

ವರ್ಣ** strv_skip(ವರ್ಣ **l, size_t n) {
        ಯಾವತ್ (n > 0) {
                ಯದಿ (strv_isempty(l))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ l;
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_n(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, size_t n) {
        size_t i, k;
        ವರ್ಣ **nl;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        ಯದಿ (n >= SIZE_MAX - k)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), ಪರಿಮಾಣ(ವರ್ಣ *));
        ಯದಿ (!nl)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *l = nl;

        ಪ್ರತಿ (i = k; i < k + n; i++) {
                nl[i] = strdup(ಮೌಲ್ಯ);
                ಯದಿ (!nl[i])
                        ಗಚ್ಛ rollback;
        }
        nl[i] = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ 0;

rollback:
        ಪ್ರತಿ (size_t j = k; j < i; j++)
                ಮುಕ್ತ(nl[j]);
        nl[k] = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
}

ಪೂರ್ಣಾಙ್ಕ strv_extend_assignment(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *lhs, ಧ್ರುವ ವರ್ಣ *rhs) {
        ವರ್ಣ *j;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(lhs);

        ಯದಿ (!rhs) /* value is optional, in which case we suppress the field */
                ಪ್ರತ್ಯಾಗಮ 0;

        j = strjoin(lhs, "=", rhs);
        ಯದಿ (!j)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume(l, j);
}

ಪೂರ್ಣಾಙ್ಕ fputstrv(FILE *f, ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *separator, bool *space) {
        bool b = false;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ಯದಿ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ string_strv_hashmap_remove(Hashmap *h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಯದಿ (ಮೌಲ್ಯ) {
                ವರ್ಣ **l = hashmap_get(h, ಕೀಲಿ);
                ಯದಿ (!l)
                        ಪ್ರತ್ಯಾಗಮ;

                strv_remove(l, ಮೌಲ್ಯ);
                ಯದಿ (!strv_isempty(l))
                        ಪ್ರತ್ಯಾಗಮ;
        }

        _unused_ _cleanup_free_ ವರ್ಣ *ಕೀಲಿ_free = ರಿಕ್ತ;
        strv_free(hashmap_remove2(h, ಕೀಲಿ, (ಶೂನ್ಯ**) &ಕೀಲಿ_free));
}

ಶೂನ್ಯ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ string_strv_hashmap_put_internal(Hashmap *h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ವರ್ಣ **l;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        l = hashmap_get(h, ಕೀಲಿ);
        ಯದಿ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ಯದಿ (strv_contains(l, ಮೌಲ್ಯ))
                        ಪ್ರತ್ಯಾಗಮ 0;

                r = strv_extend(&l, ಮೌಲ್ಯ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                assert_se(hashmap_update(h, ಕೀಲಿ, l) >= 0);
        } ಅನ್ಯಥಾ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ ವರ್ಣ **l2 = ರಿಕ್ತ;
                _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;

                t = strdup(ಕೀಲಿ);
                ಯದಿ (!t)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = strv_extend(&l2, ಮೌಲ್ಯ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = hashmap_put(h, t, l2);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ string_strv_hashmap_put(Hashmap **h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ string_strv_hashmap_put_internal(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ string_strv_ordered_hashmap_put(OrderedHashmap **h, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rebreak_flush_ಸಾಲು(
                ವರ್ಣ ***broken,
                ಧ್ರುವ ವರ್ಣ *start,
                ಧ್ರುವ ವರ್ಣ *end,
                bool in_prefix,
                ಧ್ರುವ ವರ್ಣ *whitespace_begin,
                ಧ್ರುವ ವರ್ಣ *whitespace_end) {

        ಪ್ರತಿಜ್ಞಾ(broken);
        ಪ್ರತಿಜ್ಞಾ(start);
        ಪ್ರತಿಜ್ಞಾ(end >= start);

        ಯದಿ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ಪ್ರತ್ಯಾಗಮ strv_extend(broken, "");

        ಯದಿ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ ವರ್ಣ *truncated = strndup(start, end - start);
        ಯದಿ (!truncated)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume(broken, TAKE_PTR(truncated));
}

ಪೂರ್ಣಾಙ್ಕ strv_rebreak_ಸಾಲುಗಳು(ವರ್ಣ **l, size_t width, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **broken = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ಯದಿ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ಯದಿ (!broken)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        STRV_FOREACH(i, l) {
                ಧ್ರುವ ವರ್ಣ *start = *i, *whitespace_begin = ರಿಕ್ತ, *whitespace_end = ರಿಕ್ತ;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = start; *p != 0; ) {
                        ಯದಿ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_ಸಾಲು(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = ರಿಕ್ತ;
                                w = 0;
                                ಅನುವರ್ತ;
                        } ಅನ್ಯಥಾ ಯದಿ (strchr(WHITESPACE, *p)) {
                                ಯದಿ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = ರಿಕ್ತ;
                                }
                        } ಅನ್ಯಥಾ {
                                ಯದಿ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        ಪೂರ್ಣಾಙ್ಕ cw = utf8_char_console_width(p);
                        ಯದಿ (cw < 0)
                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ಯದಿ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ ವರ್ಣ *truncated = ರಿಕ್ತ;

                                truncated = strndup(start, whitespace_begin - start);
                                ಯದಿ (!truncated)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = ರಿಕ್ತ;
                                w = 0;
                                ಅನುವರ್ತ;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ಪ್ರತಿಜ್ಞಾ(start);
                ಯದಿ (after_newline && in_prefix)
                        ಅನುವರ್ತ;

                r = rebreak_flush_ಸಾಲು(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(broken);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ** strv_filter_prefix(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ಯದಿ (isempty(prefix))
                ಪ್ರತ್ಯಾಗಮ strv_copy(l);

        _cleanup_strv_free_ ವರ್ಣ **f = ರಿಕ್ತ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ಯದಿ (!startswith(*i, prefix))
                        ಅನುವರ್ತ;

                ಯದಿ (strv_extend_with_size(&f, &sz, *i) < 0)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(f);
}

ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ strv_empty[] = { ರಿಕ್ತ };
