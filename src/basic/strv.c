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

वर्ण* strv_find(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जइ (streq(*i, name))
                        प्रत्यागम *i;

        प्रत्यागम रिक्त;
}

वर्ण* strv_find_case(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जइ (strcaseeq(*i, name))
                        प्रत्यागम *i;

        प्रत्यागम रिक्त;
}

वर्ण* strv_find_prefix(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जइ (startswith(*i, name))
                        प्रत्यागम *i;

        प्रत्यागम रिक्त;
}

वर्ण* strv_find_startswith(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        प्रतिज्ञा(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                वर्ण *e;

                e = startswith(*i, name);
                जइ (e)
                        प्रत्यागम e;
        }

        प्रत्यागम रिक्त;
}

स्थिर वर्ण* strv_find_closest_prefix(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        size_t best_distance = SIZE_MAX;
        वर्ण *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(s, l) {
                वर्ण *e = startswith(*s, name);
                जइ (!e)
                        अनुवर्त;

                size_t n = strlen(e);
                जइ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        प्रत्यागम best;
}

स्थिर वर्ण* strv_find_closest_by_levenshtein(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        ssize_t best_distance = SSIZE_MAX;
        वर्ण *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                जइ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        प्रत्यागम रिक्त;
                }

                जइ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        अनुवर्त;

                जइ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        प्रत्यागम best;
}

वर्ण* strv_find_closest(वर्ण * ध्रुव *l, ध्रुव वर्ण *name) {
        प्रतिज्ञा(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        वर्ण *found = strv_find_closest_prefix(l, name);
        जइ (found)
                प्रत्यागम found;

        प्रत्यागम strv_find_closest_by_levenshtein(l, name);
}

वर्ण* strv_find_first_field(वर्ण * ध्रुव *needles, वर्ण * ध्रुव *haystack) {
        STRV_FOREACH(k, needles) {
                वर्ण *मानम् = strv_env_pairs_get((वर्ण **)haystack, *k);
                जइ (मानम्)
                        प्रत्यागम मानम्;
        }

        प्रत्यागम रिक्त;
}

वर्ण** strv_free(वर्ण **l) {
        STRV_FOREACH(k, l)
                मुक्त(*k);

        प्रत्यागम mfree(l);
}

वर्ण** strv_free_erase(वर्ण **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        प्रत्यागम mfree(l);
}

शून्य strv_free_many(वर्ण ***strvs, size_t n) {
        प्रतिज्ञा(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        मुक्त(strvs);
}

वर्ण** strv_copy_n(वर्ण * ध्रुव *l, size_t n) {
        _cleanup_strv_free_ वर्ण **result = रिक्त;
        वर्ण **k;

        result = new(वर्ण*, MIN(strv_length(l), n) + 1);
        जइ (!result)
                प्रत्यागम रिक्त;

        k = result;
        STRV_FOREACH(i, l) {
                जइ (n == 0)
                        भङ्ग;

                *k = strdup(*i);
                जइ (!*k)
                        प्रत्यागम रिक्त;
                k++;

                जइ (n != SIZE_MAX)
                        n--;
        }

        *k = रिक्त;
        प्रत्यागम TAKE_PTR(result);
}

पूर्णाङ्क strv_copy_unless_empty(वर्ण * ध्रुव *l, वर्ण ***ret) {
        प्रतिज्ञा(ret);

        जइ (strv_isempty(l)) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        वर्ण **copy = strv_copy(l);
        जइ (!copy)
                प्रत्यागम -ENOMEM;

        *ret = TAKE_PTR(copy);
        प्रत्यागम 1;
}

size_t strv_length(वर्ण * ध्रुव *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        प्रत्यागम n;
}

वर्ण** strv_new_ap(ध्रुव वर्ण *x, va_list ap) {
        _cleanup_strv_free_ वर्ण **a = रिक्त;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        प्रति (ध्रुव वर्ण *s = x; s; s = va_arg(aq, ध्रुव वर्ण*)) {
                जइ (s == STRV_IGNORE)
                        अनुवर्त;

                n++;
        }
        va_end(aq);

        a = new(वर्ण*, n+1);
        जइ (!a)
                प्रत्यागम रिक्त;

        प्रति (ध्रुव वर्ण *s = x; s; s = va_arg(ap, ध्रुव वर्ण*)) {
                जइ (s == STRV_IGNORE)
                        अनुवर्त;

                a[i] = strdup(s);
                जइ (!a[i])
                        प्रत्यागम रिक्त;

                i++;
        }

        a[i] = रिक्त;

        प्रत्यागम TAKE_PTR(a);
}

वर्ण** strv_new_internal(ध्रुव वर्ण *x, ...) {
        वर्ण **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        प्रत्यागम r;
}

पूर्णाङ्क strv_extend_strv(वर्ण ***a, वर्ण * ध्रुव *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        प्रतिज्ञा(a);

        q = strv_length(b);
        जइ (q == 0)
                प्रत्यागम 0;

        p = strv_length(*a);
        जइ (p >= SIZE_MAX - q)
                प्रत्यागम -ENOMEM;

        वर्ण **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), परिमाण(वर्ण *));
        जइ (!t)
                प्रत्यागम -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        STRV_FOREACH(s, b) {
                जइ (filter_duplicates && strv_contains(t, *s))
                        अनुवर्त;

                t[p+i] = strdup(*s);
                जइ (!t[p+i])
                        गच्छ rollback;

                i++;
                t[p+i] = रिक्त;
        }

        प्रतिज्ञा(i <= q);

        प्रत्यागम (पूर्णाङ्क) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = रिक्त;
        प्रत्यागम -ENOMEM;
}

पूर्णाङ्क strv_extend_strv_consume(वर्ण ***a, वर्ण **b, bool filter_duplicates) {
        _cleanup_strv_free_ वर्ण **b_consume = b;
        size_t p, q, i;

        प्रतिज्ञा(a);

        q = strv_length(b);
        जइ (q == 0)
                प्रत्यागम 0;

        p = strv_length(*a);
        जइ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                जइ (filter_duplicates)
                        strv_uniq(*a);

                प्रत्यागम strv_length(*a);
        }

        जइ (p >= SIZE_MAX - q)
                प्रत्यागम -ENOMEM;

        वर्ण **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), परिमाण(वर्ण *));
        जइ (!t)
                प्रत्यागम -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        जइ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = रिक्त;
                i = q;
        } अण्णहा {
                i = 0;

                STRV_FOREACH(s, b) {
                        जइ (strv_contains(t, *s)) {
                                मुक्त(*s);
                                अनुवर्त;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = रिक्त;
                }
        }

        प्रतिज्ञा(i <= q);

        b_consume = mfree(b_consume);

        प्रत्यागम (पूर्णाङ्क) i;
}

पूर्णाङ्क strv_extend_strv_biconcat(वर्ण ***a, ध्रुव वर्ण *prefix, ध्रुव वर्ण* ध्रुव *b, ध्रुव वर्ण *suffix) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);

        STRV_FOREACH(s, b) {
                वर्ण *v;

                v = strjoin(strempty(prefix), *s, suffix);
                जइ (!v)
                        प्रत्यागम -ENOMEM;

                r = strv_consume(a, v);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क strv_split_newlines_full(वर्ण ***ret, ध्रुव वर्ण *s, ExtractFlags flags) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        जइ (r < 0)
                प्रत्यागम r;

        n = strv_length(l);
        जइ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        प्रत्यागम n;
}

वर्ण** strv_split_newlines(ध्रुव वर्ण *s) {
        वर्ण **ret;

        जइ (strv_split_newlines_full(&ret, s, 0) < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम ret;
}

पूर्णाङ्क strv_split_full(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, ExtractFlags flags) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        size_t n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        प्रति (;;) {
                _cleanup_free_ वर्ण *word = रिक्त;

                r = extract_first_word(&s, &word, separators, flags);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;

                जइ (!GREEDY_REALLOC(l, n + 2))
                        प्रत्यागम -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = रिक्त;
        }

        जइ (!l) {
                l = new0(वर्ण*, 1);
                जइ (!l)
                        प्रत्यागम -ENOMEM;
        }

        *t = TAKE_PTR(l);

        प्रत्यागम (पूर्णाङ्क) n;
}

वर्ण** strv_split(ध्रुव वर्ण *s, ध्रुव वर्ण *separators) {
        वर्ण **ret;

        जइ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम ret;
}

पूर्णाङ्क strv_split_and_extend_full(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates, ExtractFlags flags) {
        वर्ण **l;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        r = strv_split_full(&l, s, separators, flags);
        जइ (r < 0)
                प्रत्यागम r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम (पूर्णाङ्क) strv_length(*t);
}

पूर्णाङ्क strv_split_and_extend(वर्ण ***t, ध्रुव वर्ण *s, ध्रुव वर्ण *separators, bool filter_duplicates) {
        प्रत्यागम strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

पूर्णाङ्क strv_split_colon_pairs(वर्ण ***t, ध्रुव वर्ण *s) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        size_t n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        प्रति (;;) {
                _cleanup_free_ वर्ण *first = रिक्त, *second = रिक्त, *tuple = रिक्त, *second_or_empty = रिक्त;

                r = extract_first_word(&s, &tuple, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;

                ध्रुव वर्ण *p = tuple;
                r = extract_many_शब्दाः(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        अनुवर्त;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                जइ (!isempty(p))
                        प्रत्यागम -EINVAL;

                second_or_empty = strdup(strempty(second));
                जइ (!second_or_empty)
                        प्रत्यागम -ENOMEM;

                जइ (!GREEDY_REALLOC(l, n + 3))
                        प्रत्यागम -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = रिक्त;
        }

        जइ (!l) {
                l = new0(वर्ण*, 1);
                जइ (!l)
                        प्रत्यागम -ENOMEM;
        }

        *t = TAKE_PTR(l);

        प्रत्यागम (पूर्णाङ्क) n;
}

वर्ण* strv_join_full(वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, ध्रुव वर्ण *prefix) {
        वर्ण *r, *e;
        size_t n, k, m;

        जइ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                जइ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(वर्ण, n+1);
        जइ (!r)
                प्रत्यागम रिक्त;

        e = r;
        STRV_FOREACH(s, l) {
                जइ (s != l)
                        e = stpcpy(e, separator);

                जइ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        प्रत्यागम r;
}

पूर्णाङ्क strv_प्रक्षिप_with_size(वर्ण ***l, size_t *n, वर्ण *मानम्) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        प्रतिज्ञा(l);
        POINTER_MAY_BE_NULL(n);

        जइ (!मानम्)
                प्रत्यागम 0;

        size_t size = n ? *n : SIZE_MAX;
        जइ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        जइ (size > SIZE_MAX-2)
                प्रत्यागम -ENOMEM;

        वर्ण **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), परिमाण(वर्ण*));
        जइ (!c)
                प्रत्यागम -ENOMEM;

        c[size] = मानम्;
        c[size+1] = रिक्त;

        *l = c;
        जइ (n)
                *n = size + 1;
        प्रत्यागम 0;
}

पूर्णाङ्क strv_प्रक्षिप_pair(वर्ण ***l, वर्ण *a, वर्ण *b) {
        वर्ण **c;
        size_t n;

        प्रतिज्ञा(l);

        जइ (!a && !b)
                प्रत्यागम 0;

        n = strv_length(*l);

        /* Check for overflow */
        जइ (n > SIZE_MAX-3)
                प्रत्यागम -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), परिमाण(वर्ण*));
        जइ (!c)
                प्रत्यागम -ENOMEM;

        जइ (a)
                c[n++] = a;
        जइ (b)
                c[n++] = b;
        c[n] = रिक्त;

        *l = c;
        प्रत्यागम 0;
}

पूर्णाङ्क strv_निवेशय(वर्ण ***l, size_t position, वर्ण *मानम्) {
        वर्ण **c;
        size_t n, m;

        प्रतिज्ञा(l);

        जइ (!मानम्)
                प्रत्यागम 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        जइ (n > SIZE_MAX - 2)
                प्रत्यागम -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), परिमाण(वर्ण*));
        जइ (!c)
                प्रत्यागम -ENOMEM;

        जइ (n > position)
                memmove(c + position + 1, c + position, (n - position) * परिमाण(वर्ण*));

        c[position] = मानम्;
        c[n + 1] = रिक्त;

        *l = c;
        प्रत्यागम 0;
}

पूर्णाङ्क strv_consume_with_size(वर्ण ***l, size_t *n, वर्ण *मानम्) {
        पूर्णाङ्क r;

        r = strv_प्रक्षिप_with_size(l, n, मानम्);
        जइ (r < 0)
                मुक्त(मानम्);

        प्रत्यागम r;
}

पूर्णाङ्क strv_consume_pair(वर्ण ***l, वर्ण *a, वर्ण *b) {
        पूर्णाङ्क r;

        r = strv_प्रक्षिप_pair(l, a, b);
        जइ (r < 0) {
                मुक्त(a);
                मुक्त(b);
        }

        प्रत्यागम r;
}

पूर्णाङ्क strv_consume_prepend(वर्ण ***l, वर्ण *मानम्) {
        पूर्णाङ्क r;

        r = strv_प्रक्षिप_prepend(l, मानम्);
        जइ (r < 0)
                मुक्त(मानम्);

        प्रत्यागम r;
}

पूर्णाङ्क strv_prepend(वर्ण ***l, ध्रुव वर्ण *मानम्) {
        वर्ण *v;

        जइ (!मानम्)
                प्रत्यागम 0;

        v = strdup(मानम्);
        जइ (!v)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume_prepend(l, v);
}

पूर्णाङ्क strv_extend_with_size(वर्ण ***l, size_t *n, ध्रुव वर्ण *मानम्) {
        वर्ण *v;

        जइ (!मानम्)
                प्रत्यागम 0;

        v = strdup(मानम्);
        जइ (!v)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume_with_size(l, n, v);
}

पूर्णाङ्क strv_extend_many_internal(वर्ण ***l, ध्रुव वर्ण *मानम्, ...) {
        va_list ap;
        size_t n, m;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, मानम्);
        प्रति (ध्रुव वर्ण *s = मानम्; s != POINTER_MAX; s = va_arg(ap, ध्रुव वर्ण*)) {
                जइ (!s)
                        अनुवर्त;

                जइ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        भङ्ग;
                }
                m++;
        }
        va_end(ap);

        जइ (r < 0)
                प्रत्यागम r;
        जइ (m > SIZE_MAX-1)
                प्रत्यागम -ENOMEM;

        वर्ण **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), परिमाण(वर्ण*));
        जइ (!c)
                प्रत्यागम -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, मानम्);
        प्रति (ध्रुव वर्ण *s = मानम्; s != POINTER_MAX; s = va_arg(ap, ध्रुव वर्ण*)) {
                जइ (!s)
                        अनुवर्त;

                c[i] = strdup(s);
                जइ (!c[i]) {
                        r = -ENOMEM;
                        भङ्ग;
                }
                i++;
        }
        va_end(ap);

        जइ (r < 0) {
                /* rollback on error */
                प्रति (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                प्रत्यागम r;
        }

        c[i] = रिक्त;
        प्रत्यागम 0;
}

वर्ण** strv_uniq(वर्ण **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        प्रत्यागम l;
}

bool strv_is_uniq(वर्ण * ध्रुव *l) {
        STRV_FOREACH(i, l)
                जइ (strv_contains(i+1, *i))
                        प्रत्यागम false;

        प्रत्यागम true;
}

वर्ण** strv_remove(वर्ण **l, ध्रुव वर्ण *s) {
        जइ (!l)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        वर्ण **f, **t;
        प्रति (f = t = l; *f; f++)
                जइ (streq(*f, s))
                        मुक्त(*f);
                अण्णहा
                        *(t++) = *f;

        *t = रिक्त;
        प्रत्यागम l;
}

वर्ण** strv_remove_strv(वर्ण **l, वर्ण *ध्रुव*ll) {

        जइ (strv_isempty(l))
                प्रत्यागम l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        प्रत्यागम l;
}

bool strv_overlap(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                जइ (strv_contains(b, *i))
                        प्रत्यागम true;

        प्रत्यागम false;
}

स्थिर पूर्णाङ्क str_compare(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        प्रत्यागम strcmp(*a, *b);
}

वर्ण** strv_sort(वर्ण **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        प्रत्यागम l;
}

वर्ण** strv_sort_uniq_full(वर्ण **l, पूर्णाङ्क (*compare)(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b)) {
        प्रतिज्ञा(compare);

        जइ (strv_isempty(l))
                प्रत्यागम l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        वर्ण **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                जइ (compare(i, tail - 1) == 0)
                        मुक्त(*i);
                अण्णहा
                        *(tail++) = *i;

        *tail = रिक्त;
        प्रत्यागम l;
}

वर्ण** strv_sort_uniq(वर्ण **l) {
        प्रत्यागम strv_sort_uniq_full(l, str_compare);
}

पूर्णाङ्क strv_compare(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {
        पूर्णाङ्क r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        जइ (strv_isempty(a)) {
                जइ (strv_isempty(b))
                        प्रत्यागम 0;
                अण्णहा
                        प्रत्यागम -1;
        }

        जइ (strv_isempty(b))
                प्रत्यागम 1;

        प्रति ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                जइ (r != 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

bool strv_equal_ignore_order(वर्ण * ध्रुव *a, वर्ण * ध्रुव *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        जइ (a == b)
                प्रत्यागम true;

        STRV_FOREACH(i, a)
                जइ (!strv_contains(b, *i))
                        प्रत्यागम false;

        STRV_FOREACH(i, b)
                जइ (!strv_contains(a, *i))
                        प्रत्यागम false;

        प्रत्यागम true;
}

शून्य strv_print_full(वर्ण * ध्रुव *l, ध्रुव वर्ण *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

पूर्णाङ्क strv_extendf_with_size(वर्ण ***l, size_t *n, ध्रुव वर्ण *format, ...) {
        va_list ap;
        वर्ण *x;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        जइ (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume_with_size(l, n, x);
}

पूर्णाङ्क strv_extend_joined_with_size_sentinel(वर्ण ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        वर्ण *x = strextendv_with_separator(/* x= */ रिक्त, /* separator= */ रिक्त, ap);
        va_end(ap);
        जइ (!x)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume_with_size(l, n, x);
}

वर्ण* startswith_strv_internal(ध्रुव वर्ण *s, वर्ण * ध्रुव *l) {
        STRV_FOREACH(i, l) {
                वर्ण *found = (वर्ण*) startswith(s, *i);
                जइ (found)
                        प्रत्यागम found;
        }

        प्रत्यागम रिक्त;
}

वर्ण* endswith_strv_internal(ध्रुव वर्ण *s, वर्ण * ध्रुव *l) {
        STRV_FOREACH(i, l) {
                वर्ण *found = (वर्ण*) endswith(s, *i);
                जइ (found)
                        प्रत्यागम found;
        }

        प्रत्यागम रिक्त;
}

वर्ण** strv_reverse(वर्ण **l) {
        size_t n;

        n = strv_length(l);
        जइ (n <= 1)
                प्रत्यागम l;

        प्रति (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        प्रत्यागम l;
}

वर्ण** strv_shell_escape(वर्ण **l, ध्रुव वर्ण *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                वर्ण *v;

                v = shell_escape(*s, bad);
                जइ (!v)
                        प्रत्यागम रिक्त;

                free_and_replace(*s, v);
        }

        प्रत्यागम l;
}

bool strv_fnmatch_full(
                वर्ण* ध्रुव* patterns,
                ध्रुव वर्ण *s,
                पूर्णाङ्क flags,
                size_t *ret_matched_pos) {

        प्रतिज्ञा(s);

        जइ (patterns)
                प्रति (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        जइ (fnmatch(patterns[i], s, flags) == 0) {
                                जइ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                प्रत्यागम true;
                        }

        जइ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        प्रत्यागम false;
}

वर्ण** strv_skip(वर्ण **l, size_t n) {
        जाव (n > 0) {
                जइ (strv_isempty(l))
                        प्रत्यागम रिक्त;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        जइ (strv_isempty(l))
                प्रत्यागम रिक्त;
        प्रत्यागम l;
}

पूर्णाङ्क strv_extend_n(वर्ण ***l, ध्रुव वर्ण *मानम्, size_t n) {
        size_t i, k;
        वर्ण **nl;

        प्रतिज्ञा(l);

        जइ (!मानम्)
                प्रत्यागम 0;
        जइ (n == 0)
                प्रत्यागम 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        जइ (n >= SIZE_MAX - k)
                प्रत्यागम -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), परिमाण(वर्ण *));
        जइ (!nl)
                प्रत्यागम -ENOMEM;

        *l = nl;

        प्रति (i = k; i < k + n; i++) {
                nl[i] = strdup(मानम्);
                जइ (!nl[i])
                        गच्छ rollback;
        }
        nl[i] = रिक्त;

        प्रत्यागम 0;

rollback:
        प्रति (size_t j = k; j < i; j++)
                मुक्त(nl[j]);
        nl[k] = रिक्त;

        प्रत्यागम -ENOMEM;
}

पूर्णाङ्क strv_extend_assignment(वर्ण ***l, ध्रुव वर्ण *lhs, ध्रुव वर्ण *rhs) {
        वर्ण *j;

        प्रतिज्ञा(l);
        प्रतिज्ञा(lhs);

        जइ (!rhs) /* value is optional, in which case we suppress the field */
                प्रत्यागम 0;

        j = strjoin(lhs, "=", rhs);
        जइ (!j)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume(l, j);
}

पूर्णाङ्क fputstrv(FILE *f, वर्ण * ध्रुव *l, ध्रुव वर्ण *separator, bool *space) {
        bool b = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        जइ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

शून्य string_strv_hashmap_remove(Hashmap *h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        प्रतिज्ञा(कुञ्चिका);

        जइ (मानम्) {
                वर्ण **l = hashmap_get(h, कुञ्चिका);
                जइ (!l)
                        प्रत्यागम;

                strv_remove(l, मानम्);
                जइ (!strv_isempty(l))
                        प्रत्यागम;
        }

        _unused_ _cleanup_free_ वर्ण *कुञ्चिका_free = रिक्त;
        strv_free(hashmap_remove2(h, कुञ्चिका, (शून्य**) &कुञ्चिका_free));
}

शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), कुञ्चिका, मानम्);
}

स्थिर पूर्णाङ्क string_strv_hashmap_put_internal(Hashmap *h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        वर्ण **l;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(कुञ्चिका);
        प्रतिज्ञा(मानम्);

        l = hashmap_get(h, कुञ्चिका);
        जइ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                जइ (strv_contains(l, मानम्))
                        प्रत्यागम 0;

                r = strv_extend(&l, मानम्);
                जइ (r < 0)
                        प्रत्यागम r;

                assert_se(hashmap_update(h, कुञ्चिका, l) >= 0);
        } अण्णहा {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ वर्ण **l2 = रिक्त;
                _cleanup_free_ वर्ण *t = रिक्त;

                t = strdup(कुञ्चिका);
                जइ (!t)
                        प्रत्यागम -ENOMEM;

                r = strv_extend(&l2, मानम्);
                जइ (r < 0)
                        प्रत्यागम r;

                r = hashmap_put(h, t, l2);
                जइ (r < 0)
                        प्रत्यागम r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        प्रत्यागम 1;
}

पूर्णाङ्क string_strv_hashmap_put(Hashmap **h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(कुञ्चिका);
        प्रतिज्ञा(मानम्);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम string_strv_hashmap_put_internal(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क string_strv_ordered_hashmap_put(OrderedHashmap **h, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(कुञ्चिका);
        प्रतिज्ञा(मानम्);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        जइ (r < 0)
                प्रत्यागम r;

        प्रत्यागम string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), कुञ्चिका, मानम्);
}

स्थिर पूर्णाङ्क rebreak_flush_पङ्क्तिः(
                वर्ण ***broken,
                ध्रुव वर्ण *start,
                ध्रुव वर्ण *end,
                bool in_prefix,
                ध्रुव वर्ण *whitespace_begin,
                ध्रुव वर्ण *whitespace_end) {

        प्रतिज्ञा(broken);
        प्रतिज्ञा(start);
        प्रतिज्ञा(end >= start);

        जइ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                प्रत्यागम strv_extend(broken, "");

        जइ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ वर्ण *truncated = strndup(start, end - start);
        जइ (!truncated)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume(broken, TAKE_PTR(truncated));
}

पूर्णाङ्क strv_rebreak_पङ्क्तयः(वर्ण **l, size_t width, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **broken = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        जइ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                जइ (!broken)
                        प्रत्यागम -ENOMEM;

                *ret = TAKE_PTR(broken);
                प्रत्यागम 0;
        }

        STRV_FOREACH(i, l) {
                ध्रुव वर्ण *start = *i, *whitespace_begin = रिक्त, *whitespace_end = रिक्त;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                प्रति (ध्रुव वर्ण *p = start; *p != 0; ) {
                        जइ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_पङ्क्तिः(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                जइ (r < 0)
                                        प्रत्यागम r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                अनुवर्त;
                        } अण्णहा जइ (strchr(WHITESPACE, *p)) {
                                जइ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = रिक्त;
                                }
                        } अण्णहा {
                                जइ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        पूर्णाङ्क cw = utf8_char_console_width(p);
                        जइ (cw < 0)
                                प्रत्यागम log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        जइ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ वर्ण *truncated = रिक्त;

                                truncated = strndup(start, whitespace_begin - start);
                                जइ (!truncated)
                                        प्रत्यागम -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                जइ (r < 0)
                                        प्रत्यागम r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                अनुवर्त;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                प्रतिज्ञा(start);
                जइ (after_newline && in_prefix)
                        अनुवर्त;

                r = rebreak_flush_पङ्क्तिः(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(broken);
        प्रत्यागम 0;
}

वर्ण** strv_filter_prefix(वर्ण * ध्रुव *l, ध्रुव वर्ण *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        जइ (isempty(prefix))
                प्रत्यागम strv_copy(l);

        _cleanup_strv_free_ वर्ण **f = रिक्त;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                जइ (!startswith(*i, prefix))
                        अनुवर्त;

                जइ (strv_extend_with_size(&f, &sz, *i) < 0)
                        प्रत्यागम रिक्त;
        }

        प्रत्यागम TAKE_PTR(f);
}

ध्रुव वर्ण* ध्रुव strv_empty[] = { रिक्त };
