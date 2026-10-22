/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <stdio.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "gunicode.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "sort-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"

वर्ण* strv_find(वर्ण * अचर *l, अचर वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                यदि (streq(*i, name))
                        फर्काऊ *i;

        फर्काऊ रिक्त;
}

वर्ण* strv_find_case(वर्ण * अचर *l, अचर वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                यदि (strcaseeq(*i, name))
                        फर्काऊ *i;

        फर्काऊ रिक्त;
}

वर्ण* strv_find_prefix(वर्ण * अचर *l, अचर वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                यदि (startswith(*i, name))
                        फर्काऊ *i;

        फर्काऊ रिक्त;
}

वर्ण* strv_find_startswith(वर्ण * अचर *l, अचर वर्ण *name) {
        प्रतिज्ञा(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                वर्ण *e;

                e = startswith(*i, name);
                यदि (e)
                        फर्काऊ e;
        }

        फर्काऊ रिक्त;
}

स्थिर वर्ण* strv_find_closest_prefix(वर्ण * अचर *l, अचर वर्ण *name) {
        माप_प्रकार best_distance = SIZE_MAX;
        वर्ण *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(s, l) {
                वर्ण *e = startswith(*s, name);
                यदि (!e)
                        जारी;

                माप_प्रकार n = म_माप(e);
                यदि (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        फर्काऊ best;
}

स्थिर वर्ण* strv_find_closest_by_levenshtein(वर्ण * अचर *l, अचर वर्ण *name) {
        ssize_t best_distance = SSIZE_MAX;
        वर्ण *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                यदि (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        फर्काऊ रिक्त;
                }

                यदि (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        जारी;

                यदि (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        फर्काऊ best;
}

वर्ण* strv_find_closest(वर्ण * अचर *l, अचर वर्ण *name) {
        प्रतिज्ञा(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        वर्ण *found = strv_find_closest_prefix(l, name);
        यदि (found)
                फर्काऊ found;

        फर्काऊ strv_find_closest_by_levenshtein(l, name);
}

वर्ण* strv_find_first_field(वर्ण * अचर *needles, वर्ण * अचर *haystack) {
        STRV_FOREACH(k, needles) {
                वर्ण *मान = strv_env_pairs_get((वर्ण **)haystack, *k);
                यदि (मान)
                        फर्काऊ मान;
        }

        फर्काऊ रिक्त;
}

वर्ण** strv_free(वर्ण **l) {
        STRV_FOREACH(k, l)
                मुक्त(*k);

        फर्काऊ mfree(l);
}

वर्ण** strv_free_erase(वर्ण **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        फर्काऊ mfree(l);
}

शून्य strv_free_many(वर्ण ***strvs, माप_प्रकार n) {
        प्रतिज्ञा(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        मुक्त(strvs);
}

वर्ण** strv_copy_n(वर्ण * अचर *l, माप_प्रकार n) {
        _cleanup_strv_free_ वर्ण **result = रिक्त;
        वर्ण **k;

        result = new(वर्ण*, MIN(strv_length(l), n) + 1);
        यदि (!result)
                फर्काऊ रिक्त;

        k = result;
        STRV_FOREACH(i, l) {
                यदि (n == 0)
                        रोक;

                *k = strdup(*i);
                यदि (!*k)
                        फर्काऊ रिक्त;
                k++;

                यदि (n != SIZE_MAX)
                        n--;
        }

        *k = रिक्त;
        फर्काऊ TAKE_PTR(result);
}

पूर्णाङ्क strv_copy_unless_empty(वर्ण * अचर *l, वर्ण ***ret) {
        प्रतिज्ञा(ret);

        यदि (strv_isempty(l)) {
                *ret = रिक्त;
                फर्काऊ 0;
        }

        वर्ण **copy = strv_copy(l);
        यदि (!copy)
                फर्काऊ -ENOMEM;

        *ret = TAKE_PTR(copy);
        फर्काऊ 1;
}

माप_प्रकार strv_length(वर्ण * अचर *l) {
        माप_प्रकार n = 0;

        STRV_FOREACH(i, l)
                n++;

        फर्काऊ n;
}

वर्ण** strv_new_ap(अचर वर्ण *x, बहु_सूची ap) {
        _cleanup_strv_free_ वर्ण **a = रिक्त;
        माप_प्रकार n = 0, i = 0;
        बहु_सूची aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        लागि (अचर वर्ण *s = x; s; s = बहु_तर्क(aq, अचर वर्ण*)) {
                यदि (s == STRV_IGNORE)
                        जारी;

                n++;
        }
        बहु_पूर्ण(aq);

        a = new(वर्ण*, n+1);
        यदि (!a)
                फर्काऊ रिक्त;

        लागि (अचर वर्ण *s = x; s; s = बहु_तर्क(ap, अचर वर्ण*)) {
                यदि (s == STRV_IGNORE)
                        जारी;

                a[i] = strdup(s);
                यदि (!a[i])
                        फर्काऊ रिक्त;

                i++;
        }

        a[i] = रिक्त;

        फर्काऊ TAKE_PTR(a);
}

वर्ण** strv_new_internal(अचर वर्ण *x, ...) {
        वर्ण **r;
        बहु_सूची ap;

        बहु_शुरू(ap, x);
        r = strv_new_ap(x, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क strv_extend_strv(वर्ण ***a, वर्ण * अचर *b, bool filter_duplicates) {
        माप_प्रकार p, q, i = 0;

        प्रतिज्ञा(a);

        q = strv_length(b);
        यदि (q == 0)
                फर्काऊ 0;

        p = strv_length(*a);
        यदि (p >= SIZE_MAX - q)
                फर्काऊ -ENOMEM;

        वर्ण **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), परिमाण(वर्ण *));
        यदि (!t)
                फर्काऊ -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        STRV_FOREACH(s, b) {
                यदि (filter_duplicates && strv_contains(t, *s))
                        जारी;

                t[p+i] = strdup(*s);
                यदि (!t[p+i])
                        गच्छ rollback;

                i++;
                t[p+i] = रिक्त;
        }

        प्रतिज्ञा(i <= q);

        फर्काऊ (पूर्णाङ्क) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = रिक्त;
        फर्काऊ -ENOMEM;
}

पूर्णाङ्क strv_extend_strv_consume(वर्ण ***a, वर्ण **b, bool filter_duplicates) {
        _cleanup_strv_free_ वर्ण **b_consume = b;
        माप_प्रकार p, q, i;

        प्रतिज्ञा(a);

        q = strv_length(b);
        यदि (q == 0)
                फर्काऊ 0;

        p = strv_length(*a);
        यदि (p == 0) {
                strv_free_and_replace(*a, b_consume);

                यदि (filter_duplicates)
                        strv_uniq(*a);

                फर्काऊ strv_length(*a);
        }

        यदि (p >= SIZE_MAX - q)
                फर्काऊ -ENOMEM;

        वर्ण **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), परिमाण(वर्ण *));
        यदि (!t)
                फर्काऊ -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        यदि (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = रिक्त;
                i = q;
        } अन्यथा {
                i = 0;

                STRV_FOREACH(s, b) {
                        यदि (strv_contains(t, *s)) {
                                मुक्त(*s);
                                जारी;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = रिक्त;
                }
        }

        प्रतिज्ञा(i <= q);

        b_consume = mfree(b_consume);

        फर्काऊ (पूर्णाङ्क) i;
}

पूर्णाङ्क strv_extend_strv_biconcat(वर्ण ***a, अचर वर्ण *prefix, अचर वर्ण* अचर *b, अचर वर्ण *suffix) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);

        STRV_FOREACH(s, b) {
                वर्ण *v;

                v = strjoin(strempty(prefix), *s, suffix);
                यदि (!v)
                        फर्काऊ -ENOMEM;

                r = strv_consume(a, v);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क strv_split_newlines_full(वर्ण ***ret, अचर वर्ण *s, ExtractFlags flags) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        माप_प्रकार n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        यदि (r < 0)
                फर्काऊ r;

        n = strv_length(l);
        यदि (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        फर्काऊ n;
}

वर्ण** strv_split_newlines(अचर वर्ण *s) {
        वर्ण **ret;

        यदि (strv_split_newlines_full(&ret, s, 0) < 0)
                फर्काऊ रिक्त;

        फर्काऊ ret;
}

पूर्णाङ्क strv_split_full(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, ExtractFlags flags) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        माप_प्रकार n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        लागि (;;) {
                _cleanup_free_ वर्ण *word = रिक्त;

                r = extract_first_word(&s, &word, separators, flags);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                यदि (!GREEDY_REALLOC(l, n + 2))
                        फर्काऊ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = रिक्त;
        }

        यदि (!l) {
                l = new0(वर्ण*, 1);
                यदि (!l)
                        फर्काऊ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        फर्काऊ (पूर्णाङ्क) n;
}

वर्ण** strv_split(अचर वर्ण *s, अचर वर्ण *separators) {
        वर्ण **ret;

        यदि (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                फर्काऊ रिक्त;

        फर्काऊ ret;
}

पूर्णाङ्क strv_split_and_extend_full(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, bool filter_duplicates, ExtractFlags flags) {
        वर्ण **l;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        r = strv_split_full(&l, s, separators, flags);
        यदि (r < 0)
                फर्काऊ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ (पूर्णाङ्क) strv_length(*t);
}

पूर्णाङ्क strv_split_and_extend(वर्ण ***t, अचर वर्ण *s, अचर वर्ण *separators, bool filter_duplicates) {
        फर्काऊ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

पूर्णाङ्क strv_split_colon_pairs(वर्ण ***t, अचर वर्ण *s) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        माप_प्रकार n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        लागि (;;) {
                _cleanup_free_ वर्ण *first = रिक्त, *second = रिक्त, *tuple = रिक्त, *second_or_empty = रिक्त;

                r = extract_first_word(&s, &tuple, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                अचर वर्ण *p = tuple;
                r = extract_many_शब्दहरू(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        जारी;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                यदि (!isempty(p))
                        फर्काऊ -EINVAL;

                second_or_empty = strdup(strempty(second));
                यदि (!second_or_empty)
                        फर्काऊ -ENOMEM;

                यदि (!GREEDY_REALLOC(l, n + 3))
                        फर्काऊ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = रिक्त;
        }

        यदि (!l) {
                l = new0(वर्ण*, 1);
                यदि (!l)
                        फर्काऊ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        फर्काऊ (पूर्णाङ्क) n;
}

वर्ण* strv_join_full(वर्ण * अचर *l, अचर वर्ण *separator, अचर वर्ण *prefix) {
        वर्ण *r, *e;
        माप_प्रकार n, k, m;

        यदि (!separator)
                separator = " ";

        k = म_माप(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                यदि (s != l)
                        n += k;

                n += m + म_माप(*s);
        }

        r = new(वर्ण, n+1);
        यदि (!r)
                फर्काऊ रिक्त;

        e = r;
        STRV_FOREACH(s, l) {
                यदि (s != l)
                        e = stpcpy(e, separator);

                यदि (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        फर्काऊ r;
}

पूर्णाङ्क strv_थप_with_size(वर्ण ***l, माप_प्रकार *n, वर्ण *मान) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        प्रतिज्ञा(l);
        POINTER_MAY_BE_NULL(n);

        यदि (!मान)
                फर्काऊ 0;

        माप_प्रकार size = n ? *n : SIZE_MAX;
        यदि (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        यदि (size > SIZE_MAX-2)
                फर्काऊ -ENOMEM;

        वर्ण **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), परिमाण(वर्ण*));
        यदि (!c)
                फर्काऊ -ENOMEM;

        c[size] = मान;
        c[size+1] = रिक्त;

        *l = c;
        यदि (n)
                *n = size + 1;
        फर्काऊ 0;
}

पूर्णाङ्क strv_थप_pair(वर्ण ***l, वर्ण *a, वर्ण *b) {
        वर्ण **c;
        माप_प्रकार n;

        प्रतिज्ञा(l);

        यदि (!a && !b)
                फर्काऊ 0;

        n = strv_length(*l);

        /* Check for overflow */
        यदि (n > SIZE_MAX-3)
                फर्काऊ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), परिमाण(वर्ण*));
        यदि (!c)
                फर्काऊ -ENOMEM;

        यदि (a)
                c[n++] = a;
        यदि (b)
                c[n++] = b;
        c[n] = रिक्त;

        *l = c;
        फर्काऊ 0;
}

पूर्णाङ्क strv_घुसाउ(वर्ण ***l, माप_प्रकार position, वर्ण *मान) {
        वर्ण **c;
        माप_प्रकार n, m;

        प्रतिज्ञा(l);

        यदि (!मान)
                फर्काऊ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        यदि (n > SIZE_MAX - 2)
                फर्काऊ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), परिमाण(वर्ण*));
        यदि (!c)
                फर्काऊ -ENOMEM;

        यदि (n > position)
                स_हटाओ(c + position + 1, c + position, (n - position) * परिमाण(वर्ण*));

        c[position] = मान;
        c[n + 1] = रिक्त;

        *l = c;
        फर्काऊ 0;
}

पूर्णाङ्क strv_consume_with_size(वर्ण ***l, माप_प्रकार *n, वर्ण *मान) {
        पूर्णाङ्क r;

        r = strv_थप_with_size(l, n, मान);
        यदि (r < 0)
                मुक्त(मान);

        फर्काऊ r;
}

पूर्णाङ्क strv_consume_pair(वर्ण ***l, वर्ण *a, वर्ण *b) {
        पूर्णाङ्क r;

        r = strv_थप_pair(l, a, b);
        यदि (r < 0) {
                मुक्त(a);
                मुक्त(b);
        }

        फर्काऊ r;
}

पूर्णाङ्क strv_consume_prepend(वर्ण ***l, वर्ण *मान) {
        पूर्णाङ्क r;

        r = strv_थप_prepend(l, मान);
        यदि (r < 0)
                मुक्त(मान);

        फर्काऊ r;
}

पूर्णाङ्क strv_prepend(वर्ण ***l, अचर वर्ण *मान) {
        वर्ण *v;

        यदि (!मान)
                फर्काऊ 0;

        v = strdup(मान);
        यदि (!v)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume_prepend(l, v);
}

पूर्णाङ्क strv_extend_with_size(वर्ण ***l, माप_प्रकार *n, अचर वर्ण *मान) {
        वर्ण *v;

        यदि (!मान)
                फर्काऊ 0;

        v = strdup(मान);
        यदि (!v)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume_with_size(l, n, v);
}

पूर्णाङ्क strv_extend_many_internal(वर्ण ***l, अचर वर्ण *मान, ...) {
        बहु_सूची ap;
        माप_प्रकार n, m;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);

        m = n = strv_length(*l);

        r = 0;
        बहु_शुरू(ap, मान);
        लागि (अचर वर्ण *s = मान; s != POINTER_MAX; s = बहु_तर्क(ap, अचर वर्ण*)) {
                यदि (!s)
                        जारी;

                यदि (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        रोक;
                }
                m++;
        }
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ r;
        यदि (m > SIZE_MAX-1)
                फर्काऊ -ENOMEM;

        वर्ण **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), परिमाण(वर्ण*));
        यदि (!c)
                फर्काऊ -ENOMEM;
        *l = c;

        r = 0;
        माप_प्रकार i = n;
        बहु_शुरू(ap, मान);
        लागि (अचर वर्ण *s = मान; s != POINTER_MAX; s = बहु_तर्क(ap, अचर वर्ण*)) {
                यदि (!s)
                        जारी;

                c[i] = strdup(s);
                यदि (!c[i]) {
                        r = -ENOMEM;
                        रोक;
                }
                i++;
        }
        बहु_पूर्ण(ap);

        यदि (r < 0) {
                /* rollback on error */
                लागि (माप_प्रकार j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                फर्काऊ r;
        }

        c[i] = रिक्त;
        फर्काऊ 0;
}

वर्ण** strv_uniq(वर्ण **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        फर्काऊ l;
}

bool strv_is_uniq(वर्ण * अचर *l) {
        STRV_FOREACH(i, l)
                यदि (strv_contains(i+1, *i))
                        फर्काऊ false;

        फर्काऊ true;
}

वर्ण** strv_remove(वर्ण **l, अचर वर्ण *s) {
        यदि (!l)
                फर्काऊ रिक्त;

        प्रतिज्ञा(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        वर्ण **f, **t;
        लागि (f = t = l; *f; f++)
                यदि (streq(*f, s))
                        मुक्त(*f);
                अन्यथा
                        *(t++) = *f;

        *t = रिक्त;
        फर्काऊ l;
}

वर्ण** strv_remove_strv(वर्ण **l, वर्ण *अचर*ll) {

        यदि (strv_isempty(l))
                फर्काऊ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        फर्काऊ l;
}

bool strv_overlap(वर्ण * अचर *a, वर्ण * अचर *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                यदि (strv_contains(b, *i))
                        फर्काऊ true;

        फर्काऊ false;
}

स्थिर पूर्णाङ्क str_compare(वर्ण * अचर *a, वर्ण * अचर *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        फर्काऊ म_भेद(*a, *b);
}

वर्ण** strv_sort(वर्ण **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        फर्काऊ l;
}

वर्ण** strv_sort_uniq_full(वर्ण **l, पूर्णाङ्क (*compare)(वर्ण * अचर *a, वर्ण * अचर *b)) {
        प्रतिज्ञा(compare);

        यदि (strv_isempty(l))
                फर्काऊ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        वर्ण **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                यदि (compare(i, tail - 1) == 0)
                        मुक्त(*i);
                अन्यथा
                        *(tail++) = *i;

        *tail = रिक्त;
        फर्काऊ l;
}

वर्ण** strv_sort_uniq(वर्ण **l) {
        फर्काऊ strv_sort_uniq_full(l, str_compare);
}

पूर्णाङ्क strv_compare(वर्ण * अचर *a, वर्ण * अचर *b) {
        पूर्णाङ्क r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        यदि (strv_isempty(a)) {
                यदि (strv_isempty(b))
                        फर्काऊ 0;
                अन्यथा
                        फर्काऊ -1;
        }

        यदि (strv_isempty(b))
                फर्काऊ 1;

        लागि ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                यदि (r != 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

bool strv_equal_ignore_order(वर्ण * अचर *a, वर्ण * अचर *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        यदि (a == b)
                फर्काऊ true;

        STRV_FOREACH(i, a)
                यदि (!strv_contains(b, *i))
                        फर्काऊ false;

        STRV_FOREACH(i, b)
                यदि (!strv_contains(a, *i))
                        फर्काऊ false;

        फर्काऊ true;
}

शून्य strv_print_full(वर्ण * अचर *l, अचर वर्ण *prefix) {
        STRV_FOREACH(s, l)
                म_लिखो("%s%s\n", strempty(prefix), *s);
}

पूर्णाङ्क strv_extendf_with_size(वर्ण ***l, माप_प्रकार *n, अचर वर्ण *format, ...) {
        बहु_सूची ap;
        वर्ण *x;
        पूर्णाङ्क r;

        बहु_शुरू(ap, format);
        r = vasprintf(&x, format, ap);
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume_with_size(l, n, x);
}

पूर्णाङ्क strv_extend_joined_with_size_sentinel(वर्ण ***l, माप_प्रकार *n, ...) {
        बहु_सूची ap;

        बहु_शुरू(ap, n);
        वर्ण *x = strextendv_with_separator(/* x= */ रिक्त, /* separator= */ रिक्त, ap);
        बहु_पूर्ण(ap);
        यदि (!x)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume_with_size(l, n, x);
}

वर्ण* startswith_strv_internal(अचर वर्ण *s, वर्ण * अचर *l) {
        STRV_FOREACH(i, l) {
                वर्ण *found = (वर्ण*) startswith(s, *i);
                यदि (found)
                        फर्काऊ found;
        }

        फर्काऊ रिक्त;
}

वर्ण* endswith_strv_internal(अचर वर्ण *s, वर्ण * अचर *l) {
        STRV_FOREACH(i, l) {
                वर्ण *found = (वर्ण*) endswith(s, *i);
                यदि (found)
                        फर्काऊ found;
        }

        फर्काऊ रिक्त;
}

वर्ण** strv_reverse(वर्ण **l) {
        माप_प्रकार n;

        n = strv_length(l);
        यदि (n <= 1)
                फर्काऊ l;

        लागि (माप_प्रकार i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        फर्काऊ l;
}

वर्ण** strv_shell_escape(वर्ण **l, अचर वर्ण *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                वर्ण *v;

                v = shell_escape(*s, bad);
                यदि (!v)
                        फर्काऊ रिक्त;

                free_and_replace(*s, v);
        }

        फर्काऊ l;
}

bool strv_fnmatch_full(
                वर्ण* अचर* patterns,
                अचर वर्ण *s,
                पूर्णाङ्क flags,
                माप_प्रकार *ret_matched_pos) {

        प्रतिज्ञा(s);

        यदि (patterns)
                लागि (माप_प्रकार i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        यदि (fnmatch(patterns[i], s, flags) == 0) {
                                यदि (ret_matched_pos)
                                        *ret_matched_pos = i;
                                फर्काऊ true;
                        }

        यदि (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        फर्काऊ false;
}

वर्ण** strv_skip(वर्ण **l, माप_प्रकार n) {
        जबसम्म (n > 0) {
                यदि (strv_isempty(l))
                        फर्काऊ रिक्त;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        यदि (strv_isempty(l))
                फर्काऊ रिक्त;
        फर्काऊ l;
}

पूर्णाङ्क strv_extend_n(वर्ण ***l, अचर वर्ण *मान, माप_प्रकार n) {
        माप_प्रकार i, k;
        वर्ण **nl;

        प्रतिज्ञा(l);

        यदि (!मान)
                फर्काऊ 0;
        यदि (n == 0)
                फर्काऊ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        यदि (n >= SIZE_MAX - k)
                फर्काऊ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), परिमाण(वर्ण *));
        यदि (!nl)
                फर्काऊ -ENOMEM;

        *l = nl;

        लागि (i = k; i < k + n; i++) {
                nl[i] = strdup(मान);
                यदि (!nl[i])
                        गच्छ rollback;
        }
        nl[i] = रिक्त;

        फर्काऊ 0;

rollback:
        लागि (माप_प्रकार j = k; j < i; j++)
                मुक्त(nl[j]);
        nl[k] = रिक्त;

        फर्काऊ -ENOMEM;
}

पूर्णाङ्क strv_extend_assignment(वर्ण ***l, अचर वर्ण *lhs, अचर वर्ण *rhs) {
        वर्ण *j;

        प्रतिज्ञा(l);
        प्रतिज्ञा(lhs);

        यदि (!rhs) /* value is optional, in which case we suppress the field */
                फर्काऊ 0;

        j = strjoin(lhs, "=", rhs);
        यदि (!j)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume(l, j);
}

पूर्णाङ्क fputstrv(खाता *f, वर्ण * अचर *l, अचर वर्ण *separator, bool *space) {
        bool b = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        यदि (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

शून्य string_strv_hashmap_remove(Hashmap *h, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        प्रतिज्ञा(साँचो);

        यदि (मान) {
                वर्ण **l = hashmap_get(h, साँचो);
                यदि (!l)
                        फर्काऊ;

                strv_remove(l, मान);
                यदि (!strv_isempty(l))
                        फर्काऊ;
        }

        _unused_ _cleanup_free_ वर्ण *साँचो_free = रिक्त;
        strv_free(hashmap_remove2(h, साँचो, (शून्य**) &साँचो_free));
}

शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), साँचो, मान);
}

स्थिर पूर्णाङ्क string_strv_hashmap_put_internal(Hashmap *h, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        वर्ण **l;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(साँचो);
        प्रतिज्ञा(मान);

        l = hashmap_get(h, साँचो);
        यदि (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                यदि (strv_contains(l, मान))
                        फर्काऊ 0;

                r = strv_extend(&l, मान);
                यदि (r < 0)
                        फर्काऊ r;

                assert_se(hashmap_update(h, साँचो, l) >= 0);
        } अन्यथा {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ वर्ण **l2 = रिक्त;
                _cleanup_free_ वर्ण *t = रिक्त;

                t = strdup(साँचो);
                यदि (!t)
                        फर्काऊ -ENOMEM;

                r = strv_extend(&l2, मान);
                यदि (r < 0)
                        फर्काऊ r;

                r = hashmap_put(h, t, l2);
                यदि (r < 0)
                        फर्काऊ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        फर्काऊ 1;
}

पूर्णाङ्क string_strv_hashmap_put(Hashmap **h, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(साँचो);
        प्रतिज्ञा(मान);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ string_strv_hashmap_put_internal(*h, साँचो, मान);
}

पूर्णाङ्क string_strv_ordered_hashmap_put(OrderedHashmap **h, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(साँचो);
        प्रतिज्ञा(मान);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), साँचो, मान);
}

स्थिर पूर्णाङ्क rebreak_flush_पङ्क्ति(
                वर्ण ***broken,
                अचर वर्ण *start,
                अचर वर्ण *end,
                bool in_prefix,
                अचर वर्ण *whitespace_begin,
                अचर वर्ण *whitespace_end) {

        प्रतिज्ञा(broken);
        प्रतिज्ञा(start);
        प्रतिज्ञा(end >= start);

        यदि (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                फर्काऊ strv_extend(broken, "");

        यदि (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ वर्ण *truncated = strndup(start, end - start);
        यदि (!truncated)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume(broken, TAKE_PTR(truncated));
}

पूर्णाङ्क strv_rebreak_पङ्क्तिहरू(वर्ण **l, माप_प्रकार width, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **broken = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        यदि (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                यदि (!broken)
                        फर्काऊ -ENOMEM;

                *ret = TAKE_PTR(broken);
                फर्काऊ 0;
        }

        STRV_FOREACH(i, l) {
                अचर वर्ण *start = *i, *whitespace_begin = रिक्त, *whitespace_end = रिक्त;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                माप_प्रकार w = 0;

                लागि (अचर वर्ण *p = start; *p != 0; ) {
                        यदि (म_अक्षर(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_पङ्क्ति(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                यदि (r < 0)
                                        फर्काऊ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                जारी;
                        } अन्यथा यदि (म_अक्षर(WHITESPACE, *p)) {
                                यदि (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = रिक्त;
                                }
                        } अन्यथा {
                                यदि (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        पूर्णाङ्क cw = utf8_char_console_width(p);
                        यदि (cw < 0)
                                फर्काऊ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        यदि (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ वर्ण *truncated = रिक्त;

                                truncated = strndup(start, whitespace_begin - start);
                                यदि (!truncated)
                                        फर्काऊ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                यदि (r < 0)
                                        फर्काऊ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                जारी;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                प्रतिज्ञा(start);
                यदि (after_newline && in_prefix)
                        जारी;

                r = rebreak_flush_पङ्क्ति(&broken, start, start + म_माप(start), in_prefix, whitespace_begin, whitespace_end);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(broken);
        फर्काऊ 0;
}

वर्ण** strv_filter_prefix(वर्ण * अचर *l, अचर वर्ण *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        यदि (isempty(prefix))
                फर्काऊ strv_copy(l);

        _cleanup_strv_free_ वर्ण **f = रिक्त;
        माप_प्रकार sz = 0;

        STRV_FOREACH(i, l) {
                यदि (!startswith(*i, prefix))
                        जारी;

                यदि (strv_extend_with_size(&f, &sz, *i) < 0)
                        फर्काऊ रिक्त;
        }

        फर्काऊ TAKE_PTR(f);
}

अचर वर्ण* अचर strv_empty[] = { रिक्त };
