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

অক্ষর* strv_find(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (streq(*i, name))
                        ফেরত *i;

        ফেরত ফাঁকা;
}

অক্ষর* strv_find_case(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (strcaseeq(*i, name))
                        ফেরত *i;

        ফেরত ফাঁকা;
}

অক্ষর* strv_find_prefix(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l)
                যদি (startswith(*i, name))
                        ফেরত *i;

        ফেরত ফাঁকা;
}

অক্ষর* strv_find_startswith(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        প্রতিজ্ঞা(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                অক্ষর *e;

                e = startswith(*i, name);
                যদি (e)
                        ফেরত e;
        }

        ফেরত ফাঁকা;
}

স্থির অক্ষর* strv_find_closest_prefix(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        size_t best_distance = SIZE_MAX;
        অক্ষর *best = ফাঁকা;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(s, l) {
                অক্ষর *e = startswith(*s, name);
                যদি (!e)
                        চালু;

                size_t n = strlen(e);
                যদি (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ফেরত best;
}

স্থির অক্ষর* strv_find_closest_by_levenshtein(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        ssize_t best_distance = SSIZE_MAX;
        অক্ষর *best = ফাঁকা;

        প্রতিজ্ঞা(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                যদি (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ফেরত ফাঁকা;
                }

                যদি (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        চালু;

                যদি (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ফেরত best;
}

অক্ষর* strv_find_closest(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name) {
        প্রতিজ্ঞা(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        অক্ষর *found = strv_find_closest_prefix(l, name);
        যদি (found)
                ফেরত found;

        ফেরত strv_find_closest_by_levenshtein(l, name);
}

অক্ষর* strv_find_first_field(অক্ষর * ধ্রুবক *needles, অক্ষর * ধ্রুবক *haystack) {
        STRV_FOREACH(k, needles) {
                অক্ষর *মান = strv_env_pairs_get((অক্ষর **)haystack, *k);
                যদি (মান)
                        ফেরত মান;
        }

        ফেরত ফাঁকা;
}

অক্ষর** strv_free(অক্ষর **l) {
        STRV_FOREACH(k, l)
                মুক্ত(*k);

        ফেরত mfree(l);
}

অক্ষর** strv_free_erase(অক্ষর **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ফেরত mfree(l);
}

শূন্য strv_free_many(অক্ষর ***strvs, size_t n) {
        প্রতিজ্ঞা(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        মুক্ত(strvs);
}

অক্ষর** strv_copy_n(অক্ষর * ধ্রুবক *l, size_t n) {
        _cleanup_strv_free_ অক্ষর **ফলাফল = ফাঁকা;
        অক্ষর **k;

        ফলাফল = new(অক্ষর*, MIN(strv_দৈর্ঘ্য(l), n) + 1);
        যদি (!ফলাফল)
                ফেরত ফাঁকা;

        k = ফলাফল;
        STRV_FOREACH(i, l) {
                যদি (n == 0)
                        বিরতি;

                *k = strdup(*i);
                যদি (!*k)
                        ফেরত ফাঁকা;
                k++;

                যদি (n != SIZE_MAX)
                        n--;
        }

        *k = ফাঁকা;
        ফেরত TAKE_PTR(ফলাফল);
}

পূর্ণসংখ্যা strv_copy_unless_empty(অক্ষর * ধ্রুবক *l, অক্ষর ***ret) {
        প্রতিজ্ঞা(ret);

        যদি (strv_isempty(l)) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        অক্ষর **copy = strv_copy(l);
        যদি (!copy)
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(copy);
        ফেরত 1;
}

size_t strv_দৈর্ঘ্য(অক্ষর * ধ্রুবক *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ফেরত n;
}

অক্ষর** strv_new_ap(ধ্রুবক অক্ষর *x, va_list ap) {
        _cleanup_strv_free_ অক্ষর **a = ফাঁকা;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        জন্য (ধ্রুবক অক্ষর *s = x; s; s = va_arg(aq, ধ্রুবক অক্ষর*)) {
                যদি (s == STRV_IGNORE)
                        চালু;

                n++;
        }
        va_end(aq);

        a = new(অক্ষর*, n+1);
        যদি (!a)
                ফেরত ফাঁকা;

        জন্য (ধ্রুবক অক্ষর *s = x; s; s = va_arg(ap, ধ্রুবক অক্ষর*)) {
                যদি (s == STRV_IGNORE)
                        চালু;

                a[i] = strdup(s);
                যদি (!a[i])
                        ফেরত ফাঁকা;

                i++;
        }

        a[i] = ফাঁকা;

        ফেরত TAKE_PTR(a);
}

অক্ষর** strv_new_internal(ধ্রুবক অক্ষর *x, ...) {
        অক্ষর **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা strv_extend_strv(অক্ষর ***a, অক্ষর * ধ্রুবক *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        প্রতিজ্ঞা(a);

        q = strv_দৈর্ঘ্য(b);
        যদি (q == 0)
                ফেরত 0;

        p = strv_দৈর্ঘ্য(*a);
        যদি (p >= SIZE_MAX - q)
                ফেরত -ENOMEM;

        অক্ষর **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), আকার(অক্ষর *));
        যদি (!t)
                ফেরত -ENOMEM;

        t[p] = ফাঁকা;
        *a = t;

        STRV_FOREACH(s, b) {
                যদি (filter_duplicates && strv_contains(t, *s))
                        চালু;

                t[p+i] = strdup(*s);
                যদি (!t[p+i])
                        যাও rollback;

                i++;
                t[p+i] = ফাঁকা;
        }

        প্রতিজ্ঞা(i <= q);

        ফেরত (পূর্ণসংখ্যা) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ফাঁকা;
        ফেরত -ENOMEM;
}

পূর্ণসংখ্যা strv_extend_strv_consume(অক্ষর ***a, অক্ষর **b, bool filter_duplicates) {
        _cleanup_strv_free_ অক্ষর **b_consume = b;
        size_t p, q, i;

        প্রতিজ্ঞা(a);

        q = strv_দৈর্ঘ্য(b);
        যদি (q == 0)
                ফেরত 0;

        p = strv_দৈর্ঘ্য(*a);
        যদি (p == 0) {
                strv_free_and_replace(*a, b_consume);

                যদি (filter_duplicates)
                        strv_uniq(*a);

                ফেরত strv_দৈর্ঘ্য(*a);
        }

        যদি (p >= SIZE_MAX - q)
                ফেরত -ENOMEM;

        অক্ষর **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), আকার(অক্ষর *));
        যদি (!t)
                ফেরত -ENOMEM;

        t[p] = ফাঁকা;
        *a = t;

        যদি (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ফাঁকা;
                i = q;
        } অন্যথা {
                i = 0;

                STRV_FOREACH(s, b) {
                        যদি (strv_contains(t, *s)) {
                                মুক্ত(*s);
                                চালু;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ফাঁকা;
                }
        }

        প্রতিজ্ঞা(i <= q);

        b_consume = mfree(b_consume);

        ফেরত (পূর্ণসংখ্যা) i;
}

পূর্ণসংখ্যা strv_extend_strv_biconcat(অক্ষর ***a, ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর* ধ্রুবক *b, ধ্রুবক অক্ষর *suffix) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);

        STRV_FOREACH(s, b) {
                অক্ষর *v;

                v = strjoin(strempty(prefix), *s, suffix);
                যদি (!v)
                        ফেরত -ENOMEM;

                r = strv_consume(a, v);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা strv_split_newlines_full(অক্ষর ***ret, ধ্রুবক অক্ষর *s, ExtractFlags flags) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        যদি (r < 0)
                ফেরত r;

        n = strv_দৈর্ঘ্য(l);
        যদি (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ফেরত n;
}

অক্ষর** strv_split_newlines(ধ্রুবক অক্ষর *s) {
        অক্ষর **ret;

        যদি (strv_split_newlines_full(&ret, s, 0) < 0)
                ফেরত ফাঁকা;

        ফেরত ret;
}

পূর্ণসংখ্যা strv_split_full(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, ExtractFlags flags) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        size_t n = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        জন্য (;;) {
                _cleanup_free_ অক্ষর *word = ফাঁকা;

                r = extract_first_word(&s, &word, separators, flags);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                যদি (!GREEDY_REALLOC(l, n + 2))
                        ফেরত -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = ফাঁকা;
        }

        যদি (!l) {
                l = new0(অক্ষর*, 1);
                যদি (!l)
                        ফেরত -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ফেরত (পূর্ণসংখ্যা) n;
}

অক্ষর** strv_split(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators) {
        অক্ষর **ret;

        যদি (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ফেরত ফাঁকা;

        ফেরত ret;
}

পূর্ণসংখ্যা strv_split_and_extend_full(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, bool filter_duplicates, ExtractFlags flags) {
        অক্ষর **l;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        r = strv_split_full(&l, s, separators, flags);
        যদি (r < 0)
                ফেরত r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        যদি (r < 0)
                ফেরত r;

        ফেরত (পূর্ণসংখ্যা) strv_দৈর্ঘ্য(*t);
}

পূর্ণসংখ্যা strv_split_and_extend(অক্ষর ***t, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separators, bool filter_duplicates) {
        ফেরত strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

পূর্ণসংখ্যা strv_split_colon_pairs(অক্ষর ***t, ধ্রুবক অক্ষর *s) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        size_t n = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(s);

        জন্য (;;) {
                _cleanup_free_ অক্ষর *first = ফাঁকা, *second = ফাঁকা, *tuple = ফাঁকা, *second_or_empty = ফাঁকা;

                r = extract_first_word(&s, &tuple, ফাঁকা, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                ধ্রুবক অক্ষর *p = tuple;
                r = extract_many_শব্দগুলি(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        চালু;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                যদি (!isempty(p))
                        ফেরত -EINVAL;

                second_or_empty = strdup(strempty(second));
                যদি (!second_or_empty)
                        ফেরত -ENOMEM;

                যদি (!GREEDY_REALLOC(l, n + 3))
                        ফেরত -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = ফাঁকা;
        }

        যদি (!l) {
                l = new0(অক্ষর*, 1);
                যদি (!l)
                        ফেরত -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ফেরত (পূর্ণসংখ্যা) n;
}

অক্ষর* strv_join_full(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *separator, ধ্রুবক অক্ষর *prefix) {
        অক্ষর *r, *e;
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

        r = new(অক্ষর, n+1);
        যদি (!r)
                ফেরত ফাঁকা;

        e = r;
        STRV_FOREACH(s, l) {
                যদি (s != l)
                        e = stpcpy(e, separator);

                যদি (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ফেরত r;
}

পূর্ণসংখ্যা strv_ঠেলো_with_আকার(অক্ষর ***l, size_t *n, অক্ষর *মান) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        প্রতিজ্ঞা(l);
        POINTER_MAY_BE_NULL(n);

        যদি (!মান)
                ফেরত 0;

        size_t আকার_2 = n ? *n : SIZE_MAX;
        যদি (আকার_2 == SIZE_MAX)
                আকার_2 = strv_দৈর্ঘ্য(*l);

        /* Check for overflow */
        যদি (আকার_2 > SIZE_MAX-2)
                ফেরত -ENOMEM;

        অক্ষর **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(আকার_2 + 2), আকার(অক্ষর*));
        যদি (!c)
                ফেরত -ENOMEM;

        c[আকার_2] = মান;
        c[আকার_2+1] = ফাঁকা;

        *l = c;
        যদি (n)
                *n = আকার_2 + 1;
        ফেরত 0;
}

পূর্ণসংখ্যা strv_ঠেলো_pair(অক্ষর ***l, অক্ষর *a, অক্ষর *b) {
        অক্ষর **c;
        size_t n;

        প্রতিজ্ঞা(l);

        যদি (!a && !b)
                ফেরত 0;

        n = strv_দৈর্ঘ্য(*l);

        /* Check for overflow */
        যদি (n > SIZE_MAX-3)
                ফেরত -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), আকার(অক্ষর*));
        যদি (!c)
                ফেরত -ENOMEM;

        যদি (a)
                c[n++] = a;
        যদি (b)
                c[n++] = b;
        c[n] = ফাঁকা;

        *l = c;
        ফেরত 0;
}

পূর্ণসংখ্যা strv_ঢোকাও(অক্ষর ***l, size_t position, অক্ষর *মান) {
        অক্ষর **c;
        size_t n, m;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                ফেরত 0;

        n = strv_দৈর্ঘ্য(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        যদি (n > SIZE_MAX - 2)
                ফেরত -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), আকার(অক্ষর*));
        যদি (!c)
                ফেরত -ENOMEM;

        যদি (n > position)
                memmove(c + position + 1, c + position, (n - position) * আকার(অক্ষর*));

        c[position] = মান;
        c[n + 1] = ফাঁকা;

        *l = c;
        ফেরত 0;
}

পূর্ণসংখ্যা strv_consume_with_আকার(অক্ষর ***l, size_t *n, অক্ষর *মান) {
        পূর্ণসংখ্যা r;

        r = strv_ঠেলো_with_আকার(l, n, মান);
        যদি (r < 0)
                মুক্ত(মান);

        ফেরত r;
}

পূর্ণসংখ্যা strv_consume_pair(অক্ষর ***l, অক্ষর *a, অক্ষর *b) {
        পূর্ণসংখ্যা r;

        r = strv_ঠেলো_pair(l, a, b);
        যদি (r < 0) {
                মুক্ত(a);
                মুক্ত(b);
        }

        ফেরত r;
}

পূর্ণসংখ্যা strv_consume_prepend(অক্ষর ***l, অক্ষর *মান) {
        পূর্ণসংখ্যা r;

        r = strv_ঠেলো_prepend(l, মান);
        যদি (r < 0)
                মুক্ত(মান);

        ফেরত r;
}

পূর্ণসংখ্যা strv_prepend(অক্ষর ***l, ধ্রুবক অক্ষর *মান) {
        অক্ষর *v;

        যদি (!মান)
                ফেরত 0;

        v = strdup(মান);
        যদি (!v)
                ফেরত -ENOMEM;

        ফেরত strv_consume_prepend(l, v);
}

পূর্ণসংখ্যা strv_extend_with_আকার(অক্ষর ***l, size_t *n, ধ্রুবক অক্ষর *মান) {
        অক্ষর *v;

        যদি (!মান)
                ফেরত 0;

        v = strdup(মান);
        যদি (!v)
                ফেরত -ENOMEM;

        ফেরত strv_consume_with_আকার(l, n, v);
}

পূর্ণসংখ্যা strv_extend_many_internal(অক্ষর ***l, ধ্রুবক অক্ষর *মান, ...) {
        va_list ap;
        size_t n, m;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(l);

        m = n = strv_দৈর্ঘ্য(*l);

        r = 0;
        va_start(ap, মান);
        জন্য (ধ্রুবক অক্ষর *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুবক অক্ষর*)) {
                যদি (!s)
                        চালু;

                যদি (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        বিরতি;
                }
                m++;
        }
        va_end(ap);

        যদি (r < 0)
                ফেরত r;
        যদি (m > SIZE_MAX-1)
                ফেরত -ENOMEM;

        অক্ষর **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), আকার(অক্ষর*));
        যদি (!c)
                ফেরত -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, মান);
        জন্য (ধ্রুবক অক্ষর *s = মান; s != POINTER_MAX; s = va_arg(ap, ধ্রুবক অক্ষর*)) {
                যদি (!s)
                        চালু;

                c[i] = strdup(s);
                যদি (!c[i]) {
                        r = -ENOMEM;
                        বিরতি;
                }
                i++;
        }
        va_end(ap);

        যদি (r < 0) {
                /* rollback on error */
                জন্য (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ফেরত r;
        }

        c[i] = ফাঁকা;
        ফেরত 0;
}

অক্ষর** strv_uniq(অক্ষর **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ফেরত l;
}

bool strv_is_uniq(অক্ষর * ধ্রুবক *l) {
        STRV_FOREACH(i, l)
                যদি (strv_contains(i+1, *i))
                        ফেরত false;

        ফেরত true;
}

অক্ষর** strv_remove(অক্ষর **l, ধ্রুবক অক্ষর *s) {
        যদি (!l)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        অক্ষর **f, **t;
        জন্য (f = t = l; *f; f++)
                যদি (streq(*f, s))
                        মুক্ত(*f);
                অন্যথা
                        *(t++) = *f;

        *t = ফাঁকা;
        ফেরত l;
}

অক্ষর** strv_remove_strv(অক্ষর **l, অক্ষর *ধ্রুবক*ll) {

        যদি (strv_isempty(l))
                ফেরত l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ফেরত l;
}

bool strv_overlap(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                যদি (strv_contains(b, *i))
                        ফেরত true;

        ফেরত false;
}

স্থির পূর্ণসংখ্যা str_compare(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ফেরত strcmp(*a, *b);
}

অক্ষর** strv_sort(অক্ষর **l) {
        typesafe_qsort(l, strv_দৈর্ঘ্য(l), str_compare);
        ফেরত l;
}

অক্ষর** strv_sort_uniq_full(অক্ষর **l, পূর্ণসংখ্যা (*compare)(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b)) {
        প্রতিজ্ঞা(compare);

        যদি (strv_isempty(l))
                ফেরত l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_দৈর্ঘ্য(l), compare);

        অক্ষর **লেজ_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                যদি (compare(i, লেজ_2 - 1) == 0)
                        মুক্ত(*i);
                অন্যথা
                        *(লেজ_2++) = *i;

        *লেজ_2 = ফাঁকা;
        ফেরত l;
}

অক্ষর** strv_sort_uniq(অক্ষর **l) {
        ফেরত strv_sort_uniq_full(l, str_compare);
}

পূর্ণসংখ্যা strv_compare(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) {
        পূর্ণসংখ্যা r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        যদি (strv_isempty(a)) {
                যদি (strv_isempty(b))
                        ফেরত 0;
                অন্যথা
                        ফেরত -1;
        }

        যদি (strv_isempty(b))
                ফেরত 1;

        জন্য ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                যদি (r != 0)
                        ফেরত r;
        }

        ফেরত 0;
}

bool strv_equal_ignore_order(অক্ষর * ধ্রুবক *a, অক্ষর * ধ্রুবক *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        যদি (a == b)
                ফেরত true;

        STRV_FOREACH(i, a)
                যদি (!strv_contains(b, *i))
                        ফেরত false;

        STRV_FOREACH(i, b)
                যদি (!strv_contains(a, *i))
                        ফেরত false;

        ফেরত true;
}

শূন্য strv_print_full(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

পূর্ণসংখ্যা strv_extendf_with_আকার(অক্ষর ***l, size_t *n, ধ্রুবক অক্ষর *format, ...) {
        va_list ap;
        অক্ষর *x;
        পূর্ণসংখ্যা r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত -ENOMEM;

        ফেরত strv_consume_with_আকার(l, n, x);
}

পূর্ণসংখ্যা strv_extend_joined_with_আকার_sentinel(অক্ষর ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        অক্ষর *x = strextendv_with_separator(/* x= */ ফাঁকা, /* separator= */ ফাঁকা, ap);
        va_end(ap);
        যদি (!x)
                ফেরত -ENOMEM;

        ফেরত strv_consume_with_আকার(l, n, x);
}

অক্ষর* startswith_strv_internal(ধ্রুবক অক্ষর *s, অক্ষর * ধ্রুবক *l) {
        STRV_FOREACH(i, l) {
                অক্ষর *found = (অক্ষর*) startswith(s, *i);
                যদি (found)
                        ফেরত found;
        }

        ফেরত ফাঁকা;
}

অক্ষর* endswith_strv_internal(ধ্রুবক অক্ষর *s, অক্ষর * ধ্রুবক *l) {
        STRV_FOREACH(i, l) {
                অক্ষর *found = (অক্ষর*) endswith(s, *i);
                যদি (found)
                        ফেরত found;
        }

        ফেরত ফাঁকা;
}

অক্ষর** strv_reverse(অক্ষর **l) {
        size_t n;

        n = strv_দৈর্ঘ্য(l);
        যদি (n <= 1)
                ফেরত l;

        জন্য (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ফেরত l;
}

অক্ষর** strv_shell_escape(অক্ষর **l, ধ্রুবক অক্ষর *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                অক্ষর *v;

                v = shell_escape(*s, bad);
                যদি (!v)
                        ফেরত ফাঁকা;

                free_and_replace(*s, v);
        }

        ফেরত l;
}

bool strv_fnmatch_full(
                অক্ষর* ধ্রুবক* patterns,
                ধ্রুবক অক্ষর *s,
                পূর্ণসংখ্যা flags,
                size_t *ret_matched_pos) {

        প্রতিজ্ঞা(s);

        যদি (patterns)
                জন্য (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        যদি (fnmatch(patterns[i], s, flags) == 0) {
                                যদি (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ফেরত true;
                        }

        যদি (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ফেরত false;
}

অক্ষর** strv_skip(অক্ষর **l, size_t n) {
        যখন (n > 0) {
                যদি (strv_isempty(l))
                        ফেরত ফাঁকা;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        যদি (strv_isempty(l))
                ফেরত ফাঁকা;
        ফেরত l;
}

পূর্ণসংখ্যা strv_extend_n(অক্ষর ***l, ধ্রুবক অক্ষর *মান, size_t n) {
        size_t i, k;
        অক্ষর **nl;

        প্রতিজ্ঞা(l);

        যদি (!মান)
                ফেরত 0;
        যদি (n == 0)
                ফেরত 0;

        /* Adds the value n times to l */

        k = strv_দৈর্ঘ্য(*l);
        যদি (n >= SIZE_MAX - k)
                ফেরত -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), আকার(অক্ষর *));
        যদি (!nl)
                ফেরত -ENOMEM;

        *l = nl;

        জন্য (i = k; i < k + n; i++) {
                nl[i] = strdup(মান);
                যদি (!nl[i])
                        যাও rollback;
        }
        nl[i] = ফাঁকা;

        ফেরত 0;

rollback:
        জন্য (size_t j = k; j < i; j++)
                মুক্ত(nl[j]);
        nl[k] = ফাঁকা;

        ফেরত -ENOMEM;
}

পূর্ণসংখ্যা strv_extend_assignment(অক্ষর ***l, ধ্রুবক অক্ষর *lhs, ধ্রুবক অক্ষর *rhs) {
        অক্ষর *j;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(lhs);

        যদি (!rhs) /* value is optional, in which case we suppress the field */
                ফেরত 0;

        j = strjoin(lhs, "=", rhs);
        যদি (!j)
                ফেরত -ENOMEM;

        ফেরত strv_consume(l, j);
}

পূর্ণসংখ্যা fputstrv(FILE *f, অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *separator, bool *space) {
        bool b = false;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        যদি (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

শূন্য string_strv_hashmap_remove(Hashmap *h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        প্রতিজ্ঞা(চাবি);

        যদি (মান) {
                অক্ষর **l = hashmap_get(h, চাবি);
                যদি (!l)
                        ফেরত;

                strv_remove(l, মান);
                যদি (!strv_isempty(l))
                        ফেরত;
        }

        _unused_ _cleanup_free_ অক্ষর *চাবি_free = ফাঁকা;
        strv_free(hashmap_remove2(h, চাবি, (শূন্য**) &চাবি_free));
}

শূন্য string_strv_ordered_hashmap_remove(OrderedHashmap *h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), চাবি, মান);
}

স্থির পূর্ণসংখ্যা string_strv_hashmap_put_internal(Hashmap *h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        অক্ষর **l;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        l = hashmap_get(h, চাবি);
        যদি (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                যদি (strv_contains(l, মান))
                        ফেরত 0;

                r = strv_extend(&l, মান);
                যদি (r < 0)
                        ফেরত r;

                assert_se(hashmap_update(h, চাবি, l) >= 0);
        } অন্যথা {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ অক্ষর **l2 = ফাঁকা;
                _cleanup_free_ অক্ষর *t = ফাঁকা;

                t = strdup(চাবি);
                যদি (!t)
                        ফেরত -ENOMEM;

                r = strv_extend(&l2, মান);
                যদি (r < 0)
                        ফেরত r;

                r = hashmap_put(h, t, l2);
                যদি (r < 0)
                        ফেরত r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ফেরত 1;
}

পূর্ণসংখ্যা string_strv_hashmap_put(Hashmap **h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                ফেরত r;

        ফেরত string_strv_hashmap_put_internal(*h, চাবি, মান);
}

পূর্ণসংখ্যা string_strv_ordered_hashmap_put(OrderedHashmap **h, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(মান);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        যদি (r < 0)
                ফেরত r;

        ফেরত string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), চাবি, মান);
}

স্থির পূর্ণসংখ্যা rebreak_flush_সারি(
                অক্ষর ***broken,
                ধ্রুবক অক্ষর *start,
                ধ্রুবক অক্ষর *end,
                bool in_prefix,
                ধ্রুবক অক্ষর *whitespace_begin,
                ধ্রুবক অক্ষর *whitespace_end) {

        প্রতিজ্ঞা(broken);
        প্রতিজ্ঞা(start);
        প্রতিজ্ঞা(end >= start);

        যদি (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ফেরত strv_extend(broken, "");

        যদি (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ অক্ষর *truncated = strndup(start, end - start);
        যদি (!truncated)
                ফেরত -ENOMEM;

        ফেরত strv_consume(broken, TAKE_PTR(truncated));
}

পূর্ণসংখ্যা strv_rebreak_সারিগুলি(অক্ষর **l, size_t width, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **broken = ফাঁকা;
        পূর্ণসংখ্যা r;

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
                        ফেরত -ENOMEM;

                *ret = TAKE_PTR(broken);
                ফেরত 0;
        }

        STRV_FOREACH(i, l) {
                ধ্রুবক অক্ষর *start = *i, *whitespace_begin = ফাঁকা, *whitespace_end = ফাঁকা;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                জন্য (ধ্রুবক অক্ষর *p = start; *p != 0; ) {
                        যদি (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_সারি(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                যদি (r < 0)
                                        ফেরত r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = ফাঁকা;
                                w = 0;
                                চালু;
                        } অন্যথা যদি (strchr(WHITESPACE, *p)) {
                                যদি (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = ফাঁকা;
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
                        পূর্ণসংখ্যা cw = utf8_char_console_width(p);
                        যদি (cw < 0)
                                ফেরত log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        যদি (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ অক্ষর *truncated = ফাঁকা;

                                truncated = strndup(start, whitespace_begin - start);
                                যদি (!truncated)
                                        ফেরত -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                যদি (r < 0)
                                        ফেরত r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = ফাঁকা;
                                w = 0;
                                চালু;
                        }

                        p = utf8_পরবর্তী_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                প্রতিজ্ঞা(start);
                যদি (after_newline && in_prefix)
                        চালু;

                r = rebreak_flush_সারি(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(broken);
        ফেরত 0;
}

অক্ষর** strv_filter_prefix(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        যদি (isempty(prefix))
                ফেরত strv_copy(l);

        _cleanup_strv_free_ অক্ষর **f = ফাঁকা;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                যদি (!startswith(*i, prefix))
                        চালু;

                যদি (strv_extend_with_আকার(&f, &sz, *i) < 0)
                        ফেরত ফাঁকা;
        }

        ফেরত TAKE_PTR(f);
}

ধ্রুবক অক্ষর* ধ্রুবক strv_empty[] = { ফাঁকা };
