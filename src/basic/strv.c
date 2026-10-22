/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fnmatch.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "gunicode.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ଅକ୍ଷର* strv_find(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH(i, l)
                ଯଦି (streq(*i, name))
                        ଫେରନ୍ତୁ *i;

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* strv_find_case(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH(i, l)
                ଯଦି (strcaseeq(*i, name))
                        ଫେରନ୍ତୁ *i;

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* strv_find_prefix(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH(i, l)
                ଯଦି (startswith(*i, name))
                        ଫେରନ୍ତୁ *i;

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* strv_find_startswith(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ପ୍ରତିଜ୍ଞା(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                ଅକ୍ଷର *e;

                e = startswith(*i, name);
                ଯଦି (e)
                        ଫେରନ୍ତୁ e;
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅକ୍ଷର* strv_find_closest_prefix(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        size_t best_distance = SIZE_MAX;
        ଅକ୍ଷର *best = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH(s, l) {
                ଅକ୍ଷର *e = startswith(*s, name);
                ଯଦି (!e)
                        ଜାରି;

                size_t n = strlen(e);
                ଯଦି (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        ଫେରନ୍ତୁ best;
}

ସ୍ଥିର ଅକ୍ଷର* strv_find_closest_by_levenshtein(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ssize_t best_distance = SSIZE_MAX;
        ଅକ୍ଷର *best = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                ଯଦି (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ଫେରନ୍ତୁ ଖାଲି;
                }

                ଯଦି (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ଜାରି;

                ଯଦି (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        ଫେରନ୍ତୁ best;
}

ଅକ୍ଷର* strv_find_closest(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name) {
        ପ୍ରତିଜ୍ଞା(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        ଅକ୍ଷର *found = strv_find_closest_prefix(l, name);
        ଯଦି (found)
                ଫେରନ୍ତୁ found;

        ଫେରନ୍ତୁ strv_find_closest_by_levenshtein(l, name);
}

ଅକ୍ଷର* strv_find_first_field(ଅକ୍ଷର * ଅଚଳ *needles, ଅକ୍ଷର * ଅଚଳ *haystack) {
        STRV_FOREACH(k, needles) {
                ଅକ୍ଷର *ମୂଲ୍ୟ = strv_env_pairs_get((ଅକ୍ଷର **)haystack, *k);
                ଯଦି (ମୂଲ୍ୟ)
                        ଫେରନ୍ତୁ ମୂଲ୍ୟ;
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର** strv_free(ଅକ୍ଷର **l) {
        STRV_FOREACH(k, l)
                ମୁକ୍ତ(*k);

        ଫେରନ୍ତୁ mfree(l);
}

ଅକ୍ଷର** strv_free_erase(ଅକ୍ଷର **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ଫେରନ୍ତୁ mfree(l);
}

ଶୂନ୍ୟ strv_free_many(ଅକ୍ଷର ***strvs, size_t n) {
        ପ୍ରତିଜ୍ଞା(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        ମୁକ୍ତ(strvs);
}

ଅକ୍ଷର** strv_copy_n(ଅକ୍ଷର * ଅଚଳ *l, size_t n) {
        _cleanup_strv_free_ ଅକ୍ଷର **result = ଖାଲି;
        ଅକ୍ଷର **k;

        result = new(ଅକ୍ଷର*, MIN(strv_length(l), n) + 1);
        ଯଦି (!result)
                ଫେରନ୍ତୁ ଖାଲି;

        k = result;
        STRV_FOREACH(i, l) {
                ଯଦି (n == 0)
                        ଭାଙ୍ଗ;

                *k = strdup(*i);
                ଯଦି (!*k)
                        ଫେରନ୍ତୁ ଖାଲି;
                k++;

                ଯଦି (n != SIZE_MAX)
                        n--;
        }

        *k = ଖାଲି;
        ଫେରନ୍ତୁ TAKE_PTR(result);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_copy_unless_empty(ଅକ୍ଷର * ଅଚଳ *l, ଅକ୍ଷର ***ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (strv_isempty(l)) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ଅକ୍ଷର **copy = strv_copy(l);
        ଯଦି (!copy)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ଫେରନ୍ତୁ 1;
}

size_t strv_length(ଅକ୍ଷର * ଅଚଳ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ଫେରନ୍ତୁ n;
}

ଅକ୍ଷର** strv_new_ap(ଅଚଳ ଅକ୍ଷର *x, va_list ap) {
        _cleanup_strv_free_ ଅକ୍ଷର **a = ଖାଲି;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *s = x; s; s = va_arg(aq, ଅଚଳ ଅକ୍ଷର*)) {
                ଯଦି (s == STRV_IGNORE)
                        ଜାରି;

                n++;
        }
        va_end(aq);

        a = new(ଅକ୍ଷର*, n+1);
        ଯଦି (!a)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *s = x; s; s = va_arg(ap, ଅଚଳ ଅକ୍ଷର*)) {
                ଯଦି (s == STRV_IGNORE)
                        ଜାରି;

                a[i] = strdup(s);
                ଯଦି (!a[i])
                        ଫେରନ୍ତୁ ଖାଲି;

                i++;
        }

        a[i] = ଖାଲି;

        ଫେରନ୍ତୁ TAKE_PTR(a);
}

ଅକ୍ଷର** strv_new_internal(ଅଚଳ ଅକ୍ଷର *x, ...) {
        ଅକ୍ଷର **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv(ଅକ୍ଷର ***a, ଅକ୍ଷର * ଅଚଳ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ପ୍ରତିଜ୍ଞା(a);

        q = strv_length(b);
        ଯଦି (q == 0)
                ଫେରନ୍ତୁ 0;

        p = strv_length(*a);
        ଯଦି (p >= SIZE_MAX - q)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), ଆକାର(ଅକ୍ଷର *));
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        t[p] = ଖାଲି;
        *a = t;

        STRV_FOREACH(s, b) {
                ଯଦି (filter_duplicates && strv_contains(t, *s))
                        ଜାରି;

                t[p+i] = strdup(*s);
                ଯଦି (!t[p+i])
                        ଯାଅ rollback;

                i++;
                t[p+i] = ଖାଲି;
        }

        ପ୍ରତିଜ୍ଞା(i <= q);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ଖାଲି;
        ଫେରନ୍ତୁ -ENOMEM;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv_consume(ଅକ୍ଷର ***a, ଅକ୍ଷର **b, bool filter_duplicates) {
        _cleanup_strv_free_ ଅକ୍ଷର **b_consume = b;
        size_t p, q, i;

        ପ୍ରତିଜ୍ଞା(a);

        q = strv_length(b);
        ଯଦି (q == 0)
                ଫେରନ୍ତୁ 0;

        p = strv_length(*a);
        ଯଦି (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ଯଦି (filter_duplicates)
                        strv_uniq(*a);

                ଫେରନ୍ତୁ strv_length(*a);
        }

        ଯଦି (p >= SIZE_MAX - q)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), ଆକାର(ଅକ୍ଷର *));
        ଯଦି (!t)
                ଫେରନ୍ତୁ -ENOMEM;

        t[p] = ଖାଲି;
        *a = t;

        ଯଦି (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ଖାଲି;
                i = q;
        } ଅନ୍ୟଥା {
                i = 0;

                STRV_FOREACH(s, b) {
                        ଯଦି (strv_contains(t, *s)) {
                                ମୁକ୍ତ(*s);
                                ଜାରି;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ଖାଲି;
                }
        }

        ପ୍ରତିଜ୍ଞା(i <= q);

        b_consume = mfree(b_consume);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) i;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_strv_biconcat(ଅକ୍ଷର ***a, ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର* ଅଚଳ *b, ଅଚଳ ଅକ୍ଷର *suffix) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);

        STRV_FOREACH(s, b) {
                ଅକ୍ଷର *v;

                v = strjoin(strempty(prefix), *s, suffix);
                ଯଦି (!v)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = strv_consume(a, v);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_newlines_full(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *s, ExtractFlags flags) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        n = strv_length(l);
        ଯଦି (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ଫେରନ୍ତୁ n;
}

ଅକ୍ଷର** strv_split_newlines(ଅଚଳ ଅକ୍ଷର *s) {
        ଅକ୍ଷର **ret;

        ଯଦି (strv_split_newlines_full(&ret, s, 0) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_full(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        size_t n = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି;

                r = extract_first_word(&s, &word, separators, flags);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (!GREEDY_REALLOC(l, n + 2))
                        ଫେରନ୍ତୁ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = ଖାଲି;
        }

        ଯଦି (!l) {
                l = new0(ଅକ୍ଷର*, 1);
                ଯଦି (!l)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;
}

ଅକ୍ଷର** strv_split(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators) {
        ଅକ୍ଷର **ret;

        ଯଦି (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_and_extend_full(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, bool filter_duplicates, ExtractFlags flags) {
        ଅକ୍ଷର **l;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(s);

        r = strv_split_full(&l, s, separators, flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) strv_length(*t);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_and_extend(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *separators, bool filter_duplicates) {
        ଫେରନ୍ତୁ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_split_colon_pairs(ଅକ୍ଷର ***t, ଅଚଳ ଅକ୍ଷର *s) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        size_t n = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *first = ଖାଲି, *second = ଖାଲି, *tuple = ଖାଲି, *second_or_empty = ଖାଲି;

                r = extract_first_word(&s, &tuple, ଖାଲି, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଅଚଳ ଅକ୍ଷର *p = tuple;
                r = extract_many_ଶବ୍ଦଗୁଡ଼ିକ(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଜାରି;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ଯଦି (!isempty(p))
                        ଫେରନ୍ତୁ -EINVAL;

                second_or_empty = strdup(strempty(second));
                ଯଦି (!second_or_empty)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (!GREEDY_REALLOC(l, n + 3))
                        ଫେରନ୍ତୁ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = ଖାଲି;
        }

        ଯଦି (!l) {
                l = new0(ଅକ୍ଷର*, 1);
                ଯଦି (!l)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;
}

ଅକ୍ଷର* strv_join_full(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *separator, ଅଚଳ ଅକ୍ଷର *prefix) {
        ଅକ୍ଷର *r, *e;
        size_t n, k, m;

        ଯଦି (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                ଯଦି (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(ଅକ୍ଷର, n+1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        e = r;
        STRV_FOREACH(s, l) {
                ଯଦି (s != l)
                        e = stpcpy(e, separator);

                ଯଦି (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ_with_size(ଅକ୍ଷର ***l, size_t *n, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ପ୍ରତିଜ୍ଞା(l);
        POINTER_MAY_BE_NULL(n);

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ 0;

        size_t size = n ? *n : SIZE_MAX;
        ଯଦି (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        ଯଦି (size > SIZE_MAX-2)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), ଆକାର(ଅକ୍ଷର*));
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        c[size] = ମୂଲ୍ୟ;
        c[size+1] = ଖାଲି;

        *l = c;
        ଯଦି (n)
                *n = size + 1;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଠେଲ_pair(ଅକ୍ଷର ***l, ଅକ୍ଷର *a, ଅକ୍ଷର *b) {
        ଅକ୍ଷର **c;
        size_t n;

        ପ୍ରତିଜ୍ଞା(l);

        ଯଦି (!a && !b)
                ଫେରନ୍ତୁ 0;

        n = strv_length(*l);

        /* Check for overflow */
        ଯଦି (n > SIZE_MAX-3)
                ଫେରନ୍ତୁ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), ଆକାର(ଅକ୍ଷର*));
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (a)
                c[n++] = a;
        ଯଦି (b)
                c[n++] = b;
        c[n] = ଖାଲି;

        *l = c;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_ଭର୍ତ୍ତି(ଅକ୍ଷର ***l, size_t position, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର **c;
        size_t n, m;

        ପ୍ରତିଜ୍ଞା(l);

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ଯଦି (n > SIZE_MAX - 2)
                ଫେରନ୍ତୁ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), ଆକାର(ଅକ୍ଷର*));
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (n > position)
                memmove(c + position + 1, c + position, (n - position) * ଆକାର(ଅକ୍ଷର*));

        c[position] = ମୂଲ୍ୟ;
        c[n + 1] = ଖାଲି;

        *l = c;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_with_size(ଅକ୍ଷର ***l, size_t *n, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = strv_ଠେଲ_with_size(l, n, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ମୁକ୍ତ(ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_pair(ଅକ୍ଷର ***l, ଅକ୍ଷର *a, ଅକ୍ଷର *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = strv_ଠେଲ_pair(l, a, b);
        ଯଦି (r < 0) {
                ମୁକ୍ତ(a);
                ମୁକ୍ତ(b);
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_consume_prepend(ଅକ୍ଷର ***l, ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = strv_ଠେଲ_prepend(l, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ମୁକ୍ତ(ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_prepend(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର *v;

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ 0;

        v = strdup(ମୂଲ୍ୟ);
        ଯଦି (!v)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume_prepend(l, v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_with_size(ଅକ୍ଷର ***l, size_t *n, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର *v;

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ 0;

        v = strdup(ମୂଲ୍ୟ);
        ଯଦି (!v)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume_with_size(l, n, v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_many_internal(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, ...) {
        va_list ap;
        size_t n, m;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, ମୂଲ୍ୟ);
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *s = ମୂଲ୍ୟ; s != POINTER_MAX; s = va_arg(ap, ଅଚଳ ଅକ୍ଷର*)) {
                ଯଦି (!s)
                        ଜାରି;

                ଯଦି (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        ଭାଙ୍ଗ;
                }
                m++;
        }
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (m > SIZE_MAX-1)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), ଆକାର(ଅକ୍ଷର*));
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, ମୂଲ୍ୟ);
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *s = ମୂଲ୍ୟ; s != POINTER_MAX; s = va_arg(ap, ଅଚଳ ଅକ୍ଷର*)) {
                ଯଦି (!s)
                        ଜାରି;

                c[i] = strdup(s);
                ଯଦି (!c[i]) {
                        r = -ENOMEM;
                        ଭାଙ୍ଗ;
                }
                i++;
        }
        va_end(ap);

        ଯଦି (r < 0) {
                /* rollback on error */
                ପାଇଁ (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ଫେରନ୍ତୁ r;
        }

        c[i] = ଖାଲି;
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର** strv_uniq(ଅକ୍ଷର **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ଫେରନ୍ତୁ l;
}

bool strv_is_uniq(ଅକ୍ଷର * ଅଚଳ *l) {
        STRV_FOREACH(i, l)
                ଯଦି (strv_contains(i+1, *i))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ଅକ୍ଷର** strv_remove(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *s) {
        ଯଦି (!l)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        ଅକ୍ଷର **f, **t;
        ପାଇଁ (f = t = l; *f; f++)
                ଯଦି (streq(*f, s))
                        ମୁକ୍ତ(*f);
                ଅନ୍ୟଥା
                        *(t++) = *f;

        *t = ଖାଲି;
        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** strv_remove_strv(ଅକ୍ଷର **l, ଅକ୍ଷର *ଅଚଳ*ll) {

        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ଫେରନ୍ତୁ l;
}

bool strv_overlap(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ଯଦି (strv_contains(b, *i))
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_compare(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ଫେରନ୍ତୁ strcmp(*a, *b);
}

ଅକ୍ଷର** strv_sort(ଅକ୍ଷର **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** strv_sort_uniq_full(ଅକ୍ଷର **l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*compare)(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b)) {
        ପ୍ରତିଜ୍ଞା(compare);

        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        ଅକ୍ଷର **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                ଯଦି (compare(i, tail - 1) == 0)
                        ମୁକ୍ତ(*i);
                ଅନ୍ୟଥା
                        *(tail++) = *i;

        *tail = ଖାଲି;
        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** strv_sort_uniq(ଅକ୍ଷର **l) {
        ଫେରନ୍ତୁ strv_sort_uniq_full(l, str_compare);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_compare(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        ଯଦି (strv_isempty(a)) {
                ଯଦି (strv_isempty(b))
                        ଫେରନ୍ତୁ 0;
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ -1;
        }

        ଯଦି (strv_isempty(b))
                ଫେରନ୍ତୁ 1;

        ପାଇଁ ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ଯଦି (r != 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

bool strv_equal_ignore_order(ଅକ୍ଷର * ଅଚଳ *a, ଅକ୍ଷର * ଅଚଳ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ଯଦି (a == b)
                ଫେରନ୍ତୁ true;

        STRV_FOREACH(i, a)
                ଯଦି (!strv_contains(b, *i))
                        ଫେରନ୍ତୁ false;

        STRV_FOREACH(i, b)
                ଯଦି (!strv_contains(a, *i))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ଶୂନ୍ୟ strv_print_full(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extendf_with_size(ଅକ୍ଷର ***l, size_t *n, ଅଚଳ ଅକ୍ଷର *format, ...) {
        va_list ap;
        ଅକ୍ଷର *x;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume_with_size(l, n, x);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_joined_with_size_sentinel(ଅକ୍ଷର ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        ଅକ୍ଷର *x = strextendv_with_separator(/* x= */ ଖାଲି, /* separator= */ ଖାଲି, ap);
        va_end(ap);
        ଯଦି (!x)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume_with_size(l, n, x);
}

ଅକ୍ଷର* startswith_strv_internal(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର * ଅଚଳ *l) {
        STRV_FOREACH(i, l) {
                ଅକ୍ଷର *found = (ଅକ୍ଷର*) startswith(s, *i);
                ଯଦି (found)
                        ଫେରନ୍ତୁ found;
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* endswith_strv_internal(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର * ଅଚଳ *l) {
        STRV_FOREACH(i, l) {
                ଅକ୍ଷର *found = (ଅକ୍ଷର*) endswith(s, *i);
                ଯଦି (found)
                        ଫେରନ୍ତୁ found;
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର** strv_reverse(ଅକ୍ଷର **l) {
        size_t n;

        n = strv_length(l);
        ଯଦି (n <= 1)
                ଫେରନ୍ତୁ l;

        ପାଇଁ (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** strv_shell_escape(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                ଅକ୍ଷର *v;

                v = shell_escape(*s, bad);
                ଯଦି (!v)
                        ଫେରନ୍ତୁ ଖାଲି;

                free_and_replace(*s, v);
        }

        ଫେରନ୍ତୁ l;
}

bool strv_fnmatch_full(
                ଅକ୍ଷର* ଅଚଳ* patterns,
                ଅଚଳ ଅକ୍ଷର *s,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                size_t *ret_matched_pos) {

        ପ୍ରତିଜ୍ଞା(s);

        ଯଦି (patterns)
                ପାଇଁ (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ଯଦି (fnmatch(patterns[i], s, flags) == 0) {
                                ଯଦି (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ଫେରନ୍ତୁ true;
                        }

        ଯଦି (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ଫେରନ୍ତୁ false;
}

ଅକ୍ଷର** strv_skip(ଅକ୍ଷର **l, size_t n) {
        ଯେତେବେଳେ (n > 0) {
                ଯଦି (strv_isempty(l))
                        ଫେରନ୍ତୁ ଖାଲି;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ ଖାଲି;
        ଫେରନ୍ତୁ l;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_n(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, size_t n) {
        size_t i, k;
        ଅକ୍ଷର **nl;

        ପ୍ରତିଜ୍ଞା(l);

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ 0;
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        ଯଦି (n >= SIZE_MAX - k)
                ଫେରନ୍ତୁ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), ଆକାର(ଅକ୍ଷର *));
        ଯଦି (!nl)
                ଫେରନ୍ତୁ -ENOMEM;

        *l = nl;

        ପାଇଁ (i = k; i < k + n; i++) {
                nl[i] = strdup(ମୂଲ୍ୟ);
                ଯଦି (!nl[i])
                        ଯାଅ rollback;
        }
        nl[i] = ଖାଲି;

        ଫେରନ୍ତୁ 0;

rollback:
        ପାଇଁ (size_t j = k; j < i; j++)
                ମୁକ୍ତ(nl[j]);
        nl[k] = ଖାଲି;

        ଫେରନ୍ତୁ -ENOMEM;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_assignment(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *lhs, ଅଚଳ ଅକ୍ଷର *rhs) {
        ଅକ୍ଷର *j;

        ପ୍ରତିଜ୍ଞା(l);
        ପ୍ରତିଜ୍ଞା(lhs);

        ଯଦି (!rhs) /* value is optional, in which case we suppress the field */
                ଫେରନ୍ତୁ 0;

        j = strjoin(lhs, "=", rhs);
        ଯଦି (!j)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume(l, j);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fputstrv(FILE *f, ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *separator, bool *space) {
        bool b = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ଯଦି (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ string_strv_hashmap_remove(Hashmap *h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପ୍ରତିଜ୍ଞା(ଚାବି);

        ଯଦି (ମୂଲ୍ୟ) {
                ଅକ୍ଷର **l = hashmap_get(h, ଚାବି);
                ଯଦି (!l)
                        ଫେରନ୍ତୁ;

                strv_remove(l, ମୂଲ୍ୟ);
                ଯଦି (!strv_isempty(l))
                        ଫେରନ୍ତୁ;
        }

        _unused_ _cleanup_free_ ଅକ୍ଷର *ଚାବି_free = ଖାଲି;
        strv_free(hashmap_remove2(h, ଚାବି, (ଶୂନ୍ୟ**) &ଚାବି_free));
}

ଶୂନ୍ୟ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), ଚାବି, ମୂଲ୍ୟ);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_strv_hashmap_put_internal(Hashmap *h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଅକ୍ଷର **l;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        l = hashmap_get(h, ଚାବି);
        ଯଦି (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ଯଦି (strv_contains(l, ମୂଲ୍ୟ))
                        ଫେରନ୍ତୁ 0;

                r = strv_extend(&l, ମୂଲ୍ୟ);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                assert_se(hashmap_update(h, ଚାବି, l) >= 0);
        } ଅନ୍ୟଥା {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ ଅକ୍ଷର **l2 = ଖାଲି;
                _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;

                t = strdup(ଚାବି);
                ଯଦି (!t)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = strv_extend(&l2, ମୂଲ୍ୟ);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = hashmap_put(h, t, l2);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_strv_hashmap_put(Hashmap **h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ string_strv_hashmap_put_internal(*h, ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା string_strv_ordered_hashmap_put(OrderedHashmap **h, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), ଚାବି, ମୂଲ୍ୟ);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rebreak_flush_ଧାଡ଼ି(
                ଅକ୍ଷର ***broken,
                ଅଚଳ ଅକ୍ଷର *start,
                ଅଚଳ ଅକ୍ଷର *end,
                bool in_prefix,
                ଅଚଳ ଅକ୍ଷର *whitespace_begin,
                ଅଚଳ ଅକ୍ଷର *whitespace_end) {

        ପ୍ରତିଜ୍ଞା(broken);
        ପ୍ରତିଜ୍ଞା(start);
        ପ୍ରତିଜ୍ଞା(end >= start);

        ଯଦି (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                ଫେରନ୍ତୁ strv_extend(broken, "");

        ଯଦି (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ ଅକ୍ଷର *truncated = strndup(start, end - start);
        ଯଦି (!truncated)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume(broken, TAKE_PTR(truncated));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_rebreak_ଧାଡ଼ିଗୁଡ଼ିକ(ଅକ୍ଷର **l, size_t width, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **broken = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ଯଦି (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ଯଦି (!broken)
                        ଫେରନ୍ତୁ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ଫେରନ୍ତୁ 0;
        }

        STRV_FOREACH(i, l) {
                ଅଚଳ ଅକ୍ଷର *start = *i, *whitespace_begin = ଖାଲି, *whitespace_end = ଖାଲି;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = start; *p != 0; ) {
                        ଯଦି (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_ଧାଡ଼ି(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = ଖାଲି;
                                w = 0;
                                ଜାରି;
                        } ଅନ୍ୟଥା ଯଦି (strchr(WHITESPACE, *p)) {
                                ଯଦି (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = ଖାଲି;
                                }
                        } ଅନ୍ୟଥା {
                                ଯଦି (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା cw = utf8_char_console_width(p);
                        ଯଦି (cw < 0)
                                ଫେରନ୍ତୁ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ଯଦି (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ ଅକ୍ଷର *truncated = ଖାଲି;

                                truncated = strndup(start, whitespace_begin - start);
                                ଯଦି (!truncated)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = ଖାଲି;
                                w = 0;
                                ଜାରି;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ପ୍ରତିଜ୍ଞା(start);
                ଯଦି (after_newline && in_prefix)
                        ଜାରି;

                r = rebreak_flush_ଧାଡ଼ି(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(broken);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର** strv_filter_prefix(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ଯଦି (isempty(prefix))
                ଫେରନ୍ତୁ strv_copy(l);

        _cleanup_strv_free_ ଅକ୍ଷର **f = ଖାଲି;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ଯଦି (!startswith(*i, prefix))
                        ଜାରି;

                ଯଦି (strv_extend_with_size(&f, &sz, *i) < 0)
                        ଫେରନ୍ତୁ ଖାଲି;
        }

        ଫେରନ୍ତୁ TAKE_PTR(f);
}

ଅଚଳ ଅକ୍ଷର* ଅଚଳ strv_empty[] = { ଖାଲି };
