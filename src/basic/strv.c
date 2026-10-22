/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fnmatch.h>
#شامل <stdio.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "escape.h"
#شامل "extract-word.h"
#شامل "fileio.h"
#شامل "gunicode.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "sort-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

نویسه* strv_find(نویسه * ثابت *l, ثابت نویسه *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (streq(*i, name))
                        بازگشت *i;

        بازگشت پوچ;
}

نویسه* strv_find_case(نویسه * ثابت *l, ثابت نویسه *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (strcaseeq(*i, name))
                        بازگشت *i;

        بازگشت پوچ;
}

نویسه* strv_find_prefix(نویسه * ثابت *l, ثابت نویسه *name) {
        أكد(name);

        STRV_FOREACH(i, l)
                اگر (startswith(*i, name))
                        بازگشت *i;

        بازگشت پوچ;
}

نویسه* strv_find_startswith(نویسه * ثابت *l, ثابت نویسه *name) {
        أكد(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                نویسه *e;

                e = startswith(*i, name);
                اگر (e)
                        بازگشت e;
        }

        بازگشت پوچ;
}

ایستا نویسه* strv_find_closest_prefix(نویسه * ثابت *l, ثابت نویسه *name) {
        size_t best_distance = SIZE_MAX;
        نویسه *best = پوچ;

        أكد(name);

        STRV_FOREACH(s, l) {
                نویسه *e = startswith(*s, name);
                اگر (!e)
                        ادامه;

                size_t n = strlen(e);
                اگر (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        بازگشت best;
}

ایستا نویسه* strv_find_closest_by_levenshtein(نویسه * ثابت *l, ثابت نویسه *name) {
        ssize_t best_distance = SSIZE_MAX;
        نویسه *best = پوچ;

        أكد(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                اگر (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        بازگشت پوچ;
                }

                اگر (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ادامه;

                اگر (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        بازگشت best;
}

نویسه* strv_find_closest(نویسه * ثابت *l, ثابت نویسه *name) {
        أكد(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        نویسه *found = strv_find_closest_prefix(l, name);
        اگر (found)
                بازگشت found;

        بازگشت strv_find_closest_by_levenshtein(l, name);
}

نویسه* strv_find_first_field(نویسه * ثابت *needles, نویسه * ثابت *haystack) {
        STRV_FOREACH(k, needles) {
                نویسه *مقدار = strv_env_pairs_get((نویسه **)haystack, *k);
                اگر (مقدار)
                        بازگشت مقدار;
        }

        بازگشت پوچ;
}

نویسه** strv_free(نویسه **l) {
        STRV_FOREACH(k, l)
                آزاد(*k);

        بازگشت mfree(l);
}

نویسه** strv_free_erase(نویسه **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        بازگشت mfree(l);
}

تهی strv_free_many(نویسه ***strvs, size_t n) {
        أكد(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        آزاد(strvs);
}

نویسه** strv_copy_n(نویسه * ثابت *l, size_t n) {
        _cleanup_strv_free_ نویسه **result = پوچ;
        نویسه **k;

        result = new(نویسه*, MIN(strv_length(l), n) + 1);
        اگر (!result)
                بازگشت پوچ;

        k = result;
        STRV_FOREACH(i, l) {
                اگر (n == 0)
                        بشکن;

                *k = strdup(*i);
                اگر (!*k)
                        بازگشت پوچ;
                k++;

                اگر (n != SIZE_MAX)
                        n--;
        }

        *k = پوچ;
        بازگشت TAKE_PTR(result);
}

صحیح strv_copy_unless_empty(نویسه * ثابت *l, نویسه ***ret) {
        أكد(ret);

        اگر (strv_isempty(l)) {
                *ret = پوچ;
                بازگشت 0;
        }

        نویسه **copy = strv_copy(l);
        اگر (!copy)
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(copy);
        بازگشت 1;
}

size_t strv_length(نویسه * ثابت *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        بازگشت n;
}

نویسه** strv_new_ap(ثابت نویسه *x, va_list ap) {
        _cleanup_strv_free_ نویسه **a = پوچ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        برای (ثابت نویسه *s = x; s; s = va_arg(aq, ثابت نویسه*)) {
                اگر (s == STRV_IGNORE)
                        ادامه;

                n++;
        }
        va_end(aq);

        a = new(نویسه*, n+1);
        اگر (!a)
                بازگشت پوچ;

        برای (ثابت نویسه *s = x; s; s = va_arg(ap, ثابت نویسه*)) {
                اگر (s == STRV_IGNORE)
                        ادامه;

                a[i] = strdup(s);
                اگر (!a[i])
                        بازگشت پوچ;

                i++;
        }

        a[i] = پوچ;

        بازگشت TAKE_PTR(a);
}

نویسه** strv_new_internal(ثابت نویسه *x, ...) {
        نویسه **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        بازگشت r;
}

صحیح strv_extend_strv(نویسه ***a, نویسه * ثابت *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        أكد(a);

        q = strv_length(b);
        اگر (q == 0)
                بازگشت 0;

        p = strv_length(*a);
        اگر (p >= SIZE_MAX - q)
                بازگشت -ENOMEM;

        نویسه **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), اندازه(نویسه *));
        اگر (!t)
                بازگشت -ENOMEM;

        t[p] = پوچ;
        *a = t;

        STRV_FOREACH(s, b) {
                اگر (filter_duplicates && strv_contains(t, *s))
                        ادامه;

                t[p+i] = strdup(*s);
                اگر (!t[p+i])
                        برو rollback;

                i++;
                t[p+i] = پوچ;
        }

        أكد(i <= q);

        بازگشت (صحیح) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = پوچ;
        بازگشت -ENOMEM;
}

صحیح strv_extend_strv_consume(نویسه ***a, نویسه **b, bool filter_duplicates) {
        _cleanup_strv_free_ نویسه **b_consume = b;
        size_t p, q, i;

        أكد(a);

        q = strv_length(b);
        اگر (q == 0)
                بازگشت 0;

        p = strv_length(*a);
        اگر (p == 0) {
                strv_free_and_replace(*a, b_consume);

                اگر (filter_duplicates)
                        strv_uniq(*a);

                بازگشت strv_length(*a);
        }

        اگر (p >= SIZE_MAX - q)
                بازگشت -ENOMEM;

        نویسه **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), اندازه(نویسه *));
        اگر (!t)
                بازگشت -ENOMEM;

        t[p] = پوچ;
        *a = t;

        اگر (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = پوچ;
                i = q;
        } وگرنه {
                i = 0;

                STRV_FOREACH(s, b) {
                        اگر (strv_contains(t, *s)) {
                                آزاد(*s);
                                ادامه;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = پوچ;
                }
        }

        أكد(i <= q);

        b_consume = mfree(b_consume);

        بازگشت (صحیح) i;
}

صحیح strv_extend_strv_biconcat(نویسه ***a, ثابت نویسه *prefix, ثابت نویسه* ثابت *b, ثابت نویسه *suffix) {
        صحیح r;

        أكد(a);

        STRV_FOREACH(s, b) {
                نویسه *v;

                v = strjoin(strempty(prefix), *s, suffix);
                اگر (!v)
                        بازگشت -ENOMEM;

                r = strv_consume(a, v);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح strv_split_newlines_full(نویسه ***ret, ثابت نویسه *s, ExtractFlags flags) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        size_t n;
        صحیح r;

        أكد(ret);
        أكد(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        اگر (r < 0)
                بازگشت r;

        n = strv_length(l);
        اگر (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        بازگشت n;
}

نویسه** strv_split_newlines(ثابت نویسه *s) {
        نویسه **ret;

        اگر (strv_split_newlines_full(&ret, s, 0) < 0)
                بازگشت پوچ;

        بازگشت ret;
}

صحیح strv_split_full(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, ExtractFlags flags) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        size_t n = 0;
        صحیح r;

        أكد(t);
        أكد(s);

        برای (;;) {
                _cleanup_free_ نویسه *word = پوچ;

                r = extract_first_word(&s, &word, separators, flags);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                اگر (!GREEDY_REALLOC(l, n + 2))
                        بازگشت -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = پوچ;
        }

        اگر (!l) {
                l = new0(نویسه*, 1);
                اگر (!l)
                        بازگشت -ENOMEM;
        }

        *t = TAKE_PTR(l);

        بازگشت (صحیح) n;
}

نویسه** strv_split(ثابت نویسه *s, ثابت نویسه *separators) {
        نویسه **ret;

        اگر (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                بازگشت پوچ;

        بازگشت ret;
}

صحیح strv_split_and_extend_full(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, bool filter_duplicates, ExtractFlags flags) {
        نویسه **l;
        صحیح r;

        أكد(t);
        أكد(s);

        r = strv_split_full(&l, s, separators, flags);
        اگر (r < 0)
                بازگشت r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        اگر (r < 0)
                بازگشت r;

        بازگشت (صحیح) strv_length(*t);
}

صحیح strv_split_and_extend(نویسه ***t, ثابت نویسه *s, ثابت نویسه *separators, bool filter_duplicates) {
        بازگشت strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

صحیح strv_split_colon_pairs(نویسه ***t, ثابت نویسه *s) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        size_t n = 0;
        صحیح r;

        أكد(t);
        أكد(s);

        برای (;;) {
                _cleanup_free_ نویسه *first = پوچ, *second = پوچ, *tuple = پوچ, *second_or_empty = پوچ;

                r = extract_first_word(&s, &tuple, پوچ, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                ثابت نویسه *p = tuple;
                r = extract_many_واژگان(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        ادامه;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                اگر (!isempty(p))
                        بازگشت -EINVAL;

                second_or_empty = strdup(strempty(second));
                اگر (!second_or_empty)
                        بازگشت -ENOMEM;

                اگر (!GREEDY_REALLOC(l, n + 3))
                        بازگشت -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = پوچ;
        }

        اگر (!l) {
                l = new0(نویسه*, 1);
                اگر (!l)
                        بازگشت -ENOMEM;
        }

        *t = TAKE_PTR(l);

        بازگشت (صحیح) n;
}

نویسه* strv_join_full(نویسه * ثابت *l, ثابت نویسه *separator, ثابت نویسه *prefix) {
        نویسه *r, *e;
        size_t n, k, m;

        اگر (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                اگر (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(نویسه, n+1);
        اگر (!r)
                بازگشت پوچ;

        e = r;
        STRV_FOREACH(s, l) {
                اگر (s != l)
                        e = stpcpy(e, separator);

                اگر (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        بازگشت r;
}

صحیح strv_افزودن_with_size(نویسه ***l, size_t *n, نویسه *مقدار) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        أكد(l);
        POINTER_MAY_BE_NULL(n);

        اگر (!مقدار)
                بازگشت 0;

        size_t size = n ? *n : SIZE_MAX;
        اگر (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        اگر (size > SIZE_MAX-2)
                بازگشت -ENOMEM;

        نویسه **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), اندازه(نویسه*));
        اگر (!c)
                بازگشت -ENOMEM;

        c[size] = مقدار;
        c[size+1] = پوچ;

        *l = c;
        اگر (n)
                *n = size + 1;
        بازگشت 0;
}

صحیح strv_افزودن_pair(نویسه ***l, نویسه *a, نویسه *b) {
        نویسه **c;
        size_t n;

        أكد(l);

        اگر (!a && !b)
                بازگشت 0;

        n = strv_length(*l);

        /* Check for overflow */
        اگر (n > SIZE_MAX-3)
                بازگشت -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), اندازه(نویسه*));
        اگر (!c)
                بازگشت -ENOMEM;

        اگر (a)
                c[n++] = a;
        اگر (b)
                c[n++] = b;
        c[n] = پوچ;

        *l = c;
        بازگشت 0;
}

صحیح strv_درج(نویسه ***l, size_t position, نویسه *مقدار) {
        نویسه **c;
        size_t n, m;

        أكد(l);

        اگر (!مقدار)
                بازگشت 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        اگر (n > SIZE_MAX - 2)
                بازگشت -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), اندازه(نویسه*));
        اگر (!c)
                بازگشت -ENOMEM;

        اگر (n > position)
                memmove(c + position + 1, c + position, (n - position) * اندازه(نویسه*));

        c[position] = مقدار;
        c[n + 1] = پوچ;

        *l = c;
        بازگشت 0;
}

صحیح strv_consume_with_size(نویسه ***l, size_t *n, نویسه *مقدار) {
        صحیح r;

        r = strv_افزودن_with_size(l, n, مقدار);
        اگر (r < 0)
                آزاد(مقدار);

        بازگشت r;
}

صحیح strv_consume_pair(نویسه ***l, نویسه *a, نویسه *b) {
        صحیح r;

        r = strv_افزودن_pair(l, a, b);
        اگر (r < 0) {
                آزاد(a);
                آزاد(b);
        }

        بازگشت r;
}

صحیح strv_consume_prepend(نویسه ***l, نویسه *مقدار) {
        صحیح r;

        r = strv_افزودن_prepend(l, مقدار);
        اگر (r < 0)
                آزاد(مقدار);

        بازگشت r;
}

صحیح strv_prepend(نویسه ***l, ثابت نویسه *مقدار) {
        نویسه *v;

        اگر (!مقدار)
                بازگشت 0;

        v = strdup(مقدار);
        اگر (!v)
                بازگشت -ENOMEM;

        بازگشت strv_consume_prepend(l, v);
}

صحیح strv_extend_with_size(نویسه ***l, size_t *n, ثابت نویسه *مقدار) {
        نویسه *v;

        اگر (!مقدار)
                بازگشت 0;

        v = strdup(مقدار);
        اگر (!v)
                بازگشت -ENOMEM;

        بازگشت strv_consume_with_size(l, n, v);
}

صحیح strv_extend_many_internal(نویسه ***l, ثابت نویسه *مقدار, ...) {
        va_list ap;
        size_t n, m;
        صحیح r;

        أكد(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, مقدار);
        برای (ثابت نویسه *s = مقدار; s != POINTER_MAX; s = va_arg(ap, ثابت نویسه*)) {
                اگر (!s)
                        ادامه;

                اگر (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        بشکن;
                }
                m++;
        }
        va_end(ap);

        اگر (r < 0)
                بازگشت r;
        اگر (m > SIZE_MAX-1)
                بازگشت -ENOMEM;

        نویسه **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), اندازه(نویسه*));
        اگر (!c)
                بازگشت -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, مقدار);
        برای (ثابت نویسه *s = مقدار; s != POINTER_MAX; s = va_arg(ap, ثابت نویسه*)) {
                اگر (!s)
                        ادامه;

                c[i] = strdup(s);
                اگر (!c[i]) {
                        r = -ENOMEM;
                        بشکن;
                }
                i++;
        }
        va_end(ap);

        اگر (r < 0) {
                /* rollback on error */
                برای (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                بازگشت r;
        }

        c[i] = پوچ;
        بازگشت 0;
}

نویسه** strv_uniq(نویسه **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        بازگشت l;
}

bool strv_is_uniq(نویسه * ثابت *l) {
        STRV_FOREACH(i, l)
                اگر (strv_contains(i+1, *i))
                        بازگشت false;

        بازگشت true;
}

نویسه** strv_remove(نویسه **l, ثابت نویسه *s) {
        اگر (!l)
                بازگشت پوچ;

        أكد(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        نویسه **f, **t;
        برای (f = t = l; *f; f++)
                اگر (streq(*f, s))
                        آزاد(*f);
                وگرنه
                        *(t++) = *f;

        *t = پوچ;
        بازگشت l;
}

نویسه** strv_remove_strv(نویسه **l, نویسه *ثابت*ll) {

        اگر (strv_isempty(l))
                بازگشت l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        بازگشت l;
}

bool strv_overlap(نویسه * ثابت *a, نویسه * ثابت *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                اگر (strv_contains(b, *i))
                        بازگشت true;

        بازگشت false;
}

ایستا صحیح str_compare(نویسه * ثابت *a, نویسه * ثابت *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        بازگشت strcmp(*a, *b);
}

نویسه** strv_sort(نویسه **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        بازگشت l;
}

نویسه** strv_sort_uniq_full(نویسه **l, صحیح (*compare)(نویسه * ثابت *a, نویسه * ثابت *b)) {
        أكد(compare);

        اگر (strv_isempty(l))
                بازگشت l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        نویسه **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                اگر (compare(i, tail - 1) == 0)
                        آزاد(*i);
                وگرنه
                        *(tail++) = *i;

        *tail = پوچ;
        بازگشت l;
}

نویسه** strv_sort_uniq(نویسه **l) {
        بازگشت strv_sort_uniq_full(l, str_compare);
}

صحیح strv_compare(نویسه * ثابت *a, نویسه * ثابت *b) {
        صحیح r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        اگر (strv_isempty(a)) {
                اگر (strv_isempty(b))
                        بازگشت 0;
                وگرنه
                        بازگشت -1;
        }

        اگر (strv_isempty(b))
                بازگشت 1;

        برای ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                اگر (r != 0)
                        بازگشت r;
        }

        بازگشت 0;
}

bool strv_equal_ignore_order(نویسه * ثابت *a, نویسه * ثابت *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        اگر (a == b)
                بازگشت true;

        STRV_FOREACH(i, a)
                اگر (!strv_contains(b, *i))
                        بازگشت false;

        STRV_FOREACH(i, b)
                اگر (!strv_contains(a, *i))
                        بازگشت false;

        بازگشت true;
}

تهی strv_print_full(نویسه * ثابت *l, ثابت نویسه *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

صحیح strv_extendf_with_size(نویسه ***l, size_t *n, ثابت نویسه *format, ...) {
        va_list ap;
        نویسه *x;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت -ENOMEM;

        بازگشت strv_consume_with_size(l, n, x);
}

صحیح strv_extend_joined_with_size_sentinel(نویسه ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        نویسه *x = strextendv_with_separator(/* x= */ پوچ, /* separator= */ پوچ, ap);
        va_end(ap);
        اگر (!x)
                بازگشت -ENOMEM;

        بازگشت strv_consume_with_size(l, n, x);
}

نویسه* startswith_strv_internal(ثابت نویسه *s, نویسه * ثابت *l) {
        STRV_FOREACH(i, l) {
                نویسه *found = (نویسه*) startswith(s, *i);
                اگر (found)
                        بازگشت found;
        }

        بازگشت پوچ;
}

نویسه* endswith_strv_internal(ثابت نویسه *s, نویسه * ثابت *l) {
        STRV_FOREACH(i, l) {
                نویسه *found = (نویسه*) endswith(s, *i);
                اگر (found)
                        بازگشت found;
        }

        بازگشت پوچ;
}

نویسه** strv_reverse(نویسه **l) {
        size_t n;

        n = strv_length(l);
        اگر (n <= 1)
                بازگشت l;

        برای (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        بازگشت l;
}

نویسه** strv_shell_escape(نویسه **l, ثابت نویسه *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                نویسه *v;

                v = shell_escape(*s, bad);
                اگر (!v)
                        بازگشت پوچ;

                free_and_replace(*s, v);
        }

        بازگشت l;
}

bool strv_fnmatch_full(
                نویسه* ثابت* patterns,
                ثابت نویسه *s,
                صحیح flags,
                size_t *ret_matched_pos) {

        أكد(s);

        اگر (patterns)
                برای (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        اگر (fnmatch(patterns[i], s, flags) == 0) {
                                اگر (ret_matched_pos)
                                        *ret_matched_pos = i;
                                بازگشت true;
                        }

        اگر (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        بازگشت false;
}

نویسه** strv_skip(نویسه **l, size_t n) {
        وقتی (n > 0) {
                اگر (strv_isempty(l))
                        بازگشت پوچ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        اگر (strv_isempty(l))
                بازگشت پوچ;
        بازگشت l;
}

صحیح strv_extend_n(نویسه ***l, ثابت نویسه *مقدار, size_t n) {
        size_t i, k;
        نویسه **nl;

        أكد(l);

        اگر (!مقدار)
                بازگشت 0;
        اگر (n == 0)
                بازگشت 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        اگر (n >= SIZE_MAX - k)
                بازگشت -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), اندازه(نویسه *));
        اگر (!nl)
                بازگشت -ENOMEM;

        *l = nl;

        برای (i = k; i < k + n; i++) {
                nl[i] = strdup(مقدار);
                اگر (!nl[i])
                        برو rollback;
        }
        nl[i] = پوچ;

        بازگشت 0;

rollback:
        برای (size_t j = k; j < i; j++)
                آزاد(nl[j]);
        nl[k] = پوچ;

        بازگشت -ENOMEM;
}

صحیح strv_extend_assignment(نویسه ***l, ثابت نویسه *lhs, ثابت نویسه *rhs) {
        نویسه *j;

        أكد(l);
        أكد(lhs);

        اگر (!rhs) /* value is optional, in which case we suppress the field */
                بازگشت 0;

        j = strjoin(lhs, "=", rhs);
        اگر (!j)
                بازگشت -ENOMEM;

        بازگشت strv_consume(l, j);
}

صحیح fputstrv(FILE *f, نویسه * ثابت *l, ثابت نویسه *separator, bool *space) {
        bool b = false;
        صحیح r;

        أكد(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        اگر (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

تهی string_strv_hashmap_remove(Hashmap *h, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        أكد(کلید);

        اگر (مقدار) {
                نویسه **l = hashmap_get(h, کلید);
                اگر (!l)
                        بازگشت;

                strv_remove(l, مقدار);
                اگر (!strv_isempty(l))
                        بازگشت;
        }

        _unused_ _cleanup_free_ نویسه *کلید_free = پوچ;
        strv_free(hashmap_remove2(h, کلید, (تهی**) &کلید_free));
}

تهی string_strv_ordered_hashmap_remove(OrderedHashmap *h, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), کلید, مقدار);
}

ایستا صحیح string_strv_hashmap_put_internal(Hashmap *h, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        نویسه **l;
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        l = hashmap_get(h, کلید);
        اگر (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                اگر (strv_contains(l, مقدار))
                        بازگشت 0;

                r = strv_extend(&l, مقدار);
                اگر (r < 0)
                        بازگشت r;

                assert_se(hashmap_update(h, کلید, l) >= 0);
        } وگرنه {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ نویسه **l2 = پوچ;
                _cleanup_free_ نویسه *t = پوچ;

                t = strdup(کلید);
                اگر (!t)
                        بازگشت -ENOMEM;

                r = strv_extend(&l2, مقدار);
                اگر (r < 0)
                        بازگشت r;

                r = hashmap_put(h, t, l2);
                اگر (r < 0)
                        بازگشت r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        بازگشت 1;
}

صحیح string_strv_hashmap_put(Hashmap **h, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        اگر (r < 0)
                بازگشت r;

        بازگشت string_strv_hashmap_put_internal(*h, کلید, مقدار);
}

صحیح string_strv_ordered_hashmap_put(OrderedHashmap **h, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        صحیح r;

        أكد(h);
        أكد(کلید);
        أكد(مقدار);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        اگر (r < 0)
                بازگشت r;

        بازگشت string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), کلید, مقدار);
}

ایستا صحیح rebreak_flush_سطر(
                نویسه ***broken,
                ثابت نویسه *start,
                ثابت نویسه *end,
                bool in_prefix,
                ثابت نویسه *whitespace_begin,
                ثابت نویسه *whitespace_end) {

        أكد(broken);
        أكد(start);
        أكد(end >= start);

        اگر (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                بازگشت strv_extend(broken, "");

        اگر (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ نویسه *truncated = strndup(start, end - start);
        اگر (!truncated)
                بازگشت -ENOMEM;

        بازگشت strv_consume(broken, TAKE_PTR(truncated));
}

صحیح strv_rebreak_سطرها(نویسه **l, size_t width, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **broken = پوچ;
        صحیح r;

        أكد(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        اگر (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                اگر (!broken)
                        بازگشت -ENOMEM;

                *ret = TAKE_PTR(broken);
                بازگشت 0;
        }

        STRV_FOREACH(i, l) {
                ثابت نویسه *start = *i, *whitespace_begin = پوچ, *whitespace_end = پوچ;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                برای (ثابت نویسه *p = start; *p != 0; ) {
                        اگر (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_سطر(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                اگر (r < 0)
                                        بازگشت r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = پوچ;
                                w = 0;
                                ادامه;
                        } وگرنه اگر (strchr(WHITESPACE, *p)) {
                                اگر (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = پوچ;
                                }
                        } وگرنه {
                                اگر (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        صحیح cw = utf8_char_console_width(p);
                        اگر (cw < 0)
                                بازگشت log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        اگر (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ نویسه *truncated = پوچ;

                                truncated = strndup(start, whitespace_begin - start);
                                اگر (!truncated)
                                        بازگشت -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                اگر (r < 0)
                                        بازگشت r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = پوچ;
                                w = 0;
                                ادامه;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                أكد(start);
                اگر (after_newline && in_prefix)
                        ادامه;

                r = rebreak_flush_سطر(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(broken);
        بازگشت 0;
}

نویسه** strv_filter_prefix(نویسه * ثابت *l, ثابت نویسه *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        اگر (isempty(prefix))
                بازگشت strv_copy(l);

        _cleanup_strv_free_ نویسه **f = پوچ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                اگر (!startswith(*i, prefix))
                        ادامه;

                اگر (strv_extend_with_size(&f, &sz, *i) < 0)
                        بازگشت پوچ;
        }

        بازگشت TAKE_PTR(f);
}

ثابت نویسه* ثابت strv_empty[] = { پوچ };
