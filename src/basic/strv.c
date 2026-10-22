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

अक्षर* strv_find(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जर (streq(*i, name))
                        परत *i;

        परत रिक्त;
}

अक्षर* strv_find_case(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जर (strcaseeq(*i, name))
                        परत *i;

        परत रिक्त;
}

अक्षर* strv_find_prefix(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        प्रतिज्ञा(name);

        STRV_FOREACH(i, l)
                जर (startswith(*i, name))
                        परत *i;

        परत रिक्त;
}

अक्षर* strv_find_startswith(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        प्रतिज्ञा(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                अक्षर *e;

                e = startswith(*i, name);
                जर (e)
                        परत e;
        }

        परत रिक्त;
}

स्थिर अक्षर* strv_find_closest_prefix(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        माप_प्रकार best_distance = SIZE_MAX;
        अक्षर *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(s, l) {
                अक्षर *e = startswith(*s, name);
                जर (!e)
                        सुरूठेवा;

                माप_प्रकार n = म_माप(e);
                जर (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        परत best;
}

स्थिर अक्षर* strv_find_closest_by_levenshtein(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        ssize_t best_distance = SSIZE_MAX;
        अक्षर *best = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                जर (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        परत रिक्त;
                }

                जर (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        सुरूठेवा;

                जर (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        परत best;
}

अक्षर* strv_find_closest(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name) {
        प्रतिज्ञा(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        अक्षर *found = strv_find_closest_prefix(l, name);
        जर (found)
                परत found;

        परत strv_find_closest_by_levenshtein(l, name);
}

अक्षर* strv_find_first_field(अक्षर * स्थिरांक *needles, अक्षर * स्थिरांक *haystack) {
        STRV_FOREACH(k, needles) {
                अक्षर *मूल्य = strv_env_pairs_get((अक्षर **)haystack, *k);
                जर (मूल्य)
                        परत मूल्य;
        }

        परत रिक्त;
}

अक्षर** strv_free(अक्षर **l) {
        STRV_FOREACH(k, l)
                मुक्त(*k);

        परत mfree(l);
}

अक्षर** strv_free_erase(अक्षर **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        परत mfree(l);
}

शून्य strv_free_many(अक्षर ***strvs, माप_प्रकार n) {
        प्रतिज्ञा(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        मुक्त(strvs);
}

अक्षर** strv_copy_n(अक्षर * स्थिरांक *l, माप_प्रकार n) {
        _cleanup_strv_free_ अक्षर **result = रिक्त;
        अक्षर **k;

        result = new(अक्षर*, MIN(strv_length(l), n) + 1);
        जर (!result)
                परत रिक्त;

        k = result;
        STRV_FOREACH(i, l) {
                जर (n == 0)
                        थांबा;

                *k = strdup(*i);
                जर (!*k)
                        परत रिक्त;
                k++;

                जर (n != SIZE_MAX)
                        n--;
        }

        *k = रिक्त;
        परत TAKE_PTR(result);
}

पूर्णांक strv_copy_unless_empty(अक्षर * स्थिरांक *l, अक्षर ***ret) {
        प्रतिज्ञा(ret);

        जर (strv_isempty(l)) {
                *ret = रिक्त;
                परत 0;
        }

        अक्षर **copy = strv_copy(l);
        जर (!copy)
                परत -ENOMEM;

        *ret = TAKE_PTR(copy);
        परत 1;
}

माप_प्रकार strv_length(अक्षर * स्थिरांक *l) {
        माप_प्रकार n = 0;

        STRV_FOREACH(i, l)
                n++;

        परत n;
}

अक्षर** strv_new_ap(स्थिरांक अक्षर *x, बहु_सूची ap) {
        _cleanup_strv_free_ अक्षर **a = रिक्त;
        माप_प्रकार n = 0, i = 0;
        बहु_सूची aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        साठी (स्थिरांक अक्षर *s = x; s; s = बहु_तर्क(aq, स्थिरांक अक्षर*)) {
                जर (s == STRV_IGNORE)
                        सुरूठेवा;

                n++;
        }
        बहु_पूर्ण(aq);

        a = new(अक्षर*, n+1);
        जर (!a)
                परत रिक्त;

        साठी (स्थिरांक अक्षर *s = x; s; s = बहु_तर्क(ap, स्थिरांक अक्षर*)) {
                जर (s == STRV_IGNORE)
                        सुरूठेवा;

                a[i] = strdup(s);
                जर (!a[i])
                        परत रिक्त;

                i++;
        }

        a[i] = रिक्त;

        परत TAKE_PTR(a);
}

अक्षर** strv_new_internal(स्थिरांक अक्षर *x, ...) {
        अक्षर **r;
        बहु_सूची ap;

        बहु_शुरू(ap, x);
        r = strv_new_ap(x, ap);
        बहु_पूर्ण(ap);

        परत r;
}

पूर्णांक strv_extend_strv(अक्षर ***a, अक्षर * स्थिरांक *b, bool filter_duplicates) {
        माप_प्रकार p, q, i = 0;

        प्रतिज्ञा(a);

        q = strv_length(b);
        जर (q == 0)
                परत 0;

        p = strv_length(*a);
        जर (p >= SIZE_MAX - q)
                परत -ENOMEM;

        अक्षर **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), आकार(अक्षर *));
        जर (!t)
                परत -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        STRV_FOREACH(s, b) {
                जर (filter_duplicates && strv_contains(t, *s))
                        सुरूठेवा;

                t[p+i] = strdup(*s);
                जर (!t[p+i])
                        जा rollback;

                i++;
                t[p+i] = रिक्त;
        }

        प्रतिज्ञा(i <= q);

        परत (पूर्णांक) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = रिक्त;
        परत -ENOMEM;
}

पूर्णांक strv_extend_strv_consume(अक्षर ***a, अक्षर **b, bool filter_duplicates) {
        _cleanup_strv_free_ अक्षर **b_consume = b;
        माप_प्रकार p, q, i;

        प्रतिज्ञा(a);

        q = strv_length(b);
        जर (q == 0)
                परत 0;

        p = strv_length(*a);
        जर (p == 0) {
                strv_free_and_replace(*a, b_consume);

                जर (filter_duplicates)
                        strv_uniq(*a);

                परत strv_length(*a);
        }

        जर (p >= SIZE_MAX - q)
                परत -ENOMEM;

        अक्षर **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), आकार(अक्षर *));
        जर (!t)
                परत -ENOMEM;

        t[p] = रिक्त;
        *a = t;

        जर (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = रिक्त;
                i = q;
        } अन्यथा {
                i = 0;

                STRV_FOREACH(s, b) {
                        जर (strv_contains(t, *s)) {
                                मुक्त(*s);
                                सुरूठेवा;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = रिक्त;
                }
        }

        प्रतिज्ञा(i <= q);

        b_consume = mfree(b_consume);

        परत (पूर्णांक) i;
}

पूर्णांक strv_extend_strv_biconcat(अक्षर ***a, स्थिरांक अक्षर *prefix, स्थिरांक अक्षर* स्थिरांक *b, स्थिरांक अक्षर *suffix) {
        पूर्णांक r;

        प्रतिज्ञा(a);

        STRV_FOREACH(s, b) {
                अक्षर *v;

                v = strjoin(strempty(prefix), *s, suffix);
                जर (!v)
                        परत -ENOMEM;

                r = strv_consume(a, v);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

पूर्णांक strv_split_newlines_full(अक्षर ***ret, स्थिरांक अक्षर *s, ExtractFlags flags) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        माप_प्रकार n;
        पूर्णांक r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        जर (r < 0)
                परत r;

        n = strv_length(l);
        जर (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        परत n;
}

अक्षर** strv_split_newlines(स्थिरांक अक्षर *s) {
        अक्षर **ret;

        जर (strv_split_newlines_full(&ret, s, 0) < 0)
                परत रिक्त;

        परत ret;
}

पूर्णांक strv_split_full(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, ExtractFlags flags) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        माप_प्रकार n = 0;
        पूर्णांक r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        साठी (;;) {
                _cleanup_free_ अक्षर *word = रिक्त;

                r = extract_first_word(&s, &word, separators, flags);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                जर (!GREEDY_REALLOC(l, n + 2))
                        परत -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = रिक्त;
        }

        जर (!l) {
                l = new0(अक्षर*, 1);
                जर (!l)
                        परत -ENOMEM;
        }

        *t = TAKE_PTR(l);

        परत (पूर्णांक) n;
}

अक्षर** strv_split(स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators) {
        अक्षर **ret;

        जर (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                परत रिक्त;

        परत ret;
}

पूर्णांक strv_split_and_extend_full(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, bool filter_duplicates, ExtractFlags flags) {
        अक्षर **l;
        पूर्णांक r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        r = strv_split_full(&l, s, separators, flags);
        जर (r < 0)
                परत r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        जर (r < 0)
                परत r;

        परत (पूर्णांक) strv_length(*t);
}

पूर्णांक strv_split_and_extend(अक्षर ***t, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separators, bool filter_duplicates) {
        परत strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

पूर्णांक strv_split_colon_pairs(अक्षर ***t, स्थिरांक अक्षर *s) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        माप_प्रकार n = 0;
        पूर्णांक r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(s);

        साठी (;;) {
                _cleanup_free_ अक्षर *first = रिक्त, *second = रिक्त, *tuple = रिक्त, *second_or_empty = रिक्त;

                r = extract_first_word(&s, &tuple, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        थांबा;

                स्थिरांक अक्षर *p = tuple;
                r = extract_many_शब्द(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        सुरूठेवा;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                जर (!isempty(p))
                        परत -EINVAL;

                second_or_empty = strdup(strempty(second));
                जर (!second_or_empty)
                        परत -ENOMEM;

                जर (!GREEDY_REALLOC(l, n + 3))
                        परत -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = रिक्त;
        }

        जर (!l) {
                l = new0(अक्षर*, 1);
                जर (!l)
                        परत -ENOMEM;
        }

        *t = TAKE_PTR(l);

        परत (पूर्णांक) n;
}

अक्षर* strv_join_full(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *separator, स्थिरांक अक्षर *prefix) {
        अक्षर *r, *e;
        माप_प्रकार n, k, m;

        जर (!separator)
                separator = " ";

        k = म_माप(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                जर (s != l)
                        n += k;

                n += m + म_माप(*s);
        }

        r = new(अक्षर, n+1);
        जर (!r)
                परत रिक्त;

        e = r;
        STRV_FOREACH(s, l) {
                जर (s != l)
                        e = stpcpy(e, separator);

                जर (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        परत r;
}

पूर्णांक strv_ढकला_with_size(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        प्रतिज्ञा(l);
        POINTER_MAY_BE_NULL(n);

        जर (!मूल्य)
                परत 0;

        माप_प्रकार size = n ? *n : SIZE_MAX;
        जर (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        जर (size > SIZE_MAX-2)
                परत -ENOMEM;

        अक्षर **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), आकार(अक्षर*));
        जर (!c)
                परत -ENOMEM;

        c[size] = मूल्य;
        c[size+1] = रिक्त;

        *l = c;
        जर (n)
                *n = size + 1;
        परत 0;
}

पूर्णांक strv_ढकला_pair(अक्षर ***l, अक्षर *a, अक्षर *b) {
        अक्षर **c;
        माप_प्रकार n;

        प्रतिज्ञा(l);

        जर (!a && !b)
                परत 0;

        n = strv_length(*l);

        /* Check for overflow */
        जर (n > SIZE_MAX-3)
                परत -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), आकार(अक्षर*));
        जर (!c)
                परत -ENOMEM;

        जर (a)
                c[n++] = a;
        जर (b)
                c[n++] = b;
        c[n] = रिक्त;

        *l = c;
        परत 0;
}

पूर्णांक strv_घाला(अक्षर ***l, माप_प्रकार position, अक्षर *मूल्य) {
        अक्षर **c;
        माप_प्रकार n, m;

        प्रतिज्ञा(l);

        जर (!मूल्य)
                परत 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        जर (n > SIZE_MAX - 2)
                परत -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), आकार(अक्षर*));
        जर (!c)
                परत -ENOMEM;

        जर (n > position)
                स_हटाओ(c + position + 1, c + position, (n - position) * आकार(अक्षर*));

        c[position] = मूल्य;
        c[n + 1] = रिक्त;

        *l = c;
        परत 0;
}

पूर्णांक strv_consume_with_size(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य) {
        पूर्णांक r;

        r = strv_ढकला_with_size(l, n, मूल्य);
        जर (r < 0)
                मुक्त(मूल्य);

        परत r;
}

पूर्णांक strv_consume_pair(अक्षर ***l, अक्षर *a, अक्षर *b) {
        पूर्णांक r;

        r = strv_ढकला_pair(l, a, b);
        जर (r < 0) {
                मुक्त(a);
                मुक्त(b);
        }

        परत r;
}

पूर्णांक strv_consume_prepend(अक्षर ***l, अक्षर *मूल्य) {
        पूर्णांक r;

        r = strv_ढकला_prepend(l, मूल्य);
        जर (r < 0)
                मुक्त(मूल्य);

        परत r;
}

पूर्णांक strv_prepend(अक्षर ***l, स्थिरांक अक्षर *मूल्य) {
        अक्षर *v;

        जर (!मूल्य)
                परत 0;

        v = strdup(मूल्य);
        जर (!v)
                परत -ENOMEM;

        परत strv_consume_prepend(l, v);
}

पूर्णांक strv_extend_with_size(अक्षर ***l, माप_प्रकार *n, स्थिरांक अक्षर *मूल्य) {
        अक्षर *v;

        जर (!मूल्य)
                परत 0;

        v = strdup(मूल्य);
        जर (!v)
                परत -ENOMEM;

        परत strv_consume_with_size(l, n, v);
}

पूर्णांक strv_extend_many_internal(अक्षर ***l, स्थिरांक अक्षर *मूल्य, ...) {
        बहु_सूची ap;
        माप_प्रकार n, m;
        पूर्णांक r;

        प्रतिज्ञा(l);

        m = n = strv_length(*l);

        r = 0;
        बहु_शुरू(ap, मूल्य);
        साठी (स्थिरांक अक्षर *s = मूल्य; s != POINTER_MAX; s = बहु_तर्क(ap, स्थिरांक अक्षर*)) {
                जर (!s)
                        सुरूठेवा;

                जर (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        थांबा;
                }
                m++;
        }
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत r;
        जर (m > SIZE_MAX-1)
                परत -ENOMEM;

        अक्षर **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), आकार(अक्षर*));
        जर (!c)
                परत -ENOMEM;
        *l = c;

        r = 0;
        माप_प्रकार i = n;
        बहु_शुरू(ap, मूल्य);
        साठी (स्थिरांक अक्षर *s = मूल्य; s != POINTER_MAX; s = बहु_तर्क(ap, स्थिरांक अक्षर*)) {
                जर (!s)
                        सुरूठेवा;

                c[i] = strdup(s);
                जर (!c[i]) {
                        r = -ENOMEM;
                        थांबा;
                }
                i++;
        }
        बहु_पूर्ण(ap);

        जर (r < 0) {
                /* rollback on error */
                साठी (माप_प्रकार j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                परत r;
        }

        c[i] = रिक्त;
        परत 0;
}

अक्षर** strv_uniq(अक्षर **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        परत l;
}

bool strv_is_uniq(अक्षर * स्थिरांक *l) {
        STRV_FOREACH(i, l)
                जर (strv_contains(i+1, *i))
                        परत false;

        परत true;
}

अक्षर** strv_remove(अक्षर **l, स्थिरांक अक्षर *s) {
        जर (!l)
                परत रिक्त;

        प्रतिज्ञा(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        अक्षर **f, **t;
        साठी (f = t = l; *f; f++)
                जर (streq(*f, s))
                        मुक्त(*f);
                अन्यथा
                        *(t++) = *f;

        *t = रिक्त;
        परत l;
}

अक्षर** strv_remove_strv(अक्षर **l, अक्षर *स्थिरांक*ll) {

        जर (strv_isempty(l))
                परत l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        परत l;
}

bool strv_overlap(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                जर (strv_contains(b, *i))
                        परत true;

        परत false;
}

स्थिर पूर्णांक str_compare(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        परत म_भेद(*a, *b);
}

अक्षर** strv_sort(अक्षर **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        परत l;
}

अक्षर** strv_sort_uniq_full(अक्षर **l, पूर्णांक (*compare)(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b)) {
        प्रतिज्ञा(compare);

        जर (strv_isempty(l))
                परत l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        अक्षर **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                जर (compare(i, tail - 1) == 0)
                        मुक्त(*i);
                अन्यथा
                        *(tail++) = *i;

        *tail = रिक्त;
        परत l;
}

अक्षर** strv_sort_uniq(अक्षर **l) {
        परत strv_sort_uniq_full(l, str_compare);
}

पूर्णांक strv_compare(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) {
        पूर्णांक r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        जर (strv_isempty(a)) {
                जर (strv_isempty(b))
                        परत 0;
                अन्यथा
                        परत -1;
        }

        जर (strv_isempty(b))
                परत 1;

        साठी ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                जर (r != 0)
                        परत r;
        }

        परत 0;
}

bool strv_equal_ignore_order(अक्षर * स्थिरांक *a, अक्षर * स्थिरांक *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        जर (a == b)
                परत true;

        STRV_FOREACH(i, a)
                जर (!strv_contains(b, *i))
                        परत false;

        STRV_FOREACH(i, b)
                जर (!strv_contains(a, *i))
                        परत false;

        परत true;
}

शून्य strv_print_full(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *prefix) {
        STRV_FOREACH(s, l)
                म_लिखो("%s%s\n", strempty(prefix), *s);
}

पूर्णांक strv_extendf_with_size(अक्षर ***l, माप_प्रकार *n, स्थिरांक अक्षर *format, ...) {
        बहु_सूची ap;
        अक्षर *x;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&x, format, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत -ENOMEM;

        परत strv_consume_with_size(l, n, x);
}

पूर्णांक strv_extend_joined_with_size_sentinel(अक्षर ***l, माप_प्रकार *n, ...) {
        बहु_सूची ap;

        बहु_शुरू(ap, n);
        अक्षर *x = strextendv_with_separator(/* x= */ रिक्त, /* separator= */ रिक्त, ap);
        बहु_पूर्ण(ap);
        जर (!x)
                परत -ENOMEM;

        परत strv_consume_with_size(l, n, x);
}

अक्षर* startswith_strv_internal(स्थिरांक अक्षर *s, अक्षर * स्थिरांक *l) {
        STRV_FOREACH(i, l) {
                अक्षर *found = (अक्षर*) startswith(s, *i);
                जर (found)
                        परत found;
        }

        परत रिक्त;
}

अक्षर* endswith_strv_internal(स्थिरांक अक्षर *s, अक्षर * स्थिरांक *l) {
        STRV_FOREACH(i, l) {
                अक्षर *found = (अक्षर*) endswith(s, *i);
                जर (found)
                        परत found;
        }

        परत रिक्त;
}

अक्षर** strv_reverse(अक्षर **l) {
        माप_प्रकार n;

        n = strv_length(l);
        जर (n <= 1)
                परत l;

        साठी (माप_प्रकार i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        परत l;
}

अक्षर** strv_shell_escape(अक्षर **l, स्थिरांक अक्षर *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                अक्षर *v;

                v = shell_escape(*s, bad);
                जर (!v)
                        परत रिक्त;

                free_and_replace(*s, v);
        }

        परत l;
}

bool strv_fnmatch_full(
                अक्षर* स्थिरांक* patterns,
                स्थिरांक अक्षर *s,
                पूर्णांक flags,
                माप_प्रकार *ret_matched_pos) {

        प्रतिज्ञा(s);

        जर (patterns)
                साठी (माप_प्रकार i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        जर (fnmatch(patterns[i], s, flags) == 0) {
                                जर (ret_matched_pos)
                                        *ret_matched_pos = i;
                                परत true;
                        }

        जर (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        परत false;
}

अक्षर** strv_skip(अक्षर **l, माप_प्रकार n) {
        पर्यंत (n > 0) {
                जर (strv_isempty(l))
                        परत रिक्त;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        जर (strv_isempty(l))
                परत रिक्त;
        परत l;
}

पूर्णांक strv_extend_n(अक्षर ***l, स्थिरांक अक्षर *मूल्य, माप_प्रकार n) {
        माप_प्रकार i, k;
        अक्षर **nl;

        प्रतिज्ञा(l);

        जर (!मूल्य)
                परत 0;
        जर (n == 0)
                परत 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        जर (n >= SIZE_MAX - k)
                परत -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), आकार(अक्षर *));
        जर (!nl)
                परत -ENOMEM;

        *l = nl;

        साठी (i = k; i < k + n; i++) {
                nl[i] = strdup(मूल्य);
                जर (!nl[i])
                        जा rollback;
        }
        nl[i] = रिक्त;

        परत 0;

rollback:
        साठी (माप_प्रकार j = k; j < i; j++)
                मुक्त(nl[j]);
        nl[k] = रिक्त;

        परत -ENOMEM;
}

पूर्णांक strv_extend_assignment(अक्षर ***l, स्थिरांक अक्षर *lhs, स्थिरांक अक्षर *rhs) {
        अक्षर *j;

        प्रतिज्ञा(l);
        प्रतिज्ञा(lhs);

        जर (!rhs) /* value is optional, in which case we suppress the field */
                परत 0;

        j = strjoin(lhs, "=", rhs);
        जर (!j)
                परत -ENOMEM;

        परत strv_consume(l, j);
}

पूर्णांक fputstrv(खाता *f, अक्षर * स्थिरांक *l, स्थिरांक अक्षर *separator, bool *space) {
        bool b = false;
        पूर्णांक r;

        प्रतिज्ञा(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        जर (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

शून्य string_strv_hashmap_remove(Hashmap *h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        प्रतिज्ञा(किल्ली);

        जर (मूल्य) {
                अक्षर **l = hashmap_get(h, किल्ली);
                जर (!l)
                        परत;

                strv_remove(l, मूल्य);
                जर (!strv_isempty(l))
                        परत;
        }

        _unused_ _cleanup_free_ अक्षर *किल्ली_free = रिक्त;
        strv_free(hashmap_remove2(h, किल्ली, (शून्य**) &किल्ली_free));
}

शून्य string_strv_ordered_hashmap_remove(OrderedHashmap *h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), किल्ली, मूल्य);
}

स्थिर पूर्णांक string_strv_hashmap_put_internal(Hashmap *h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        अक्षर **l;
        पूर्णांक r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(किल्ली);
        प्रतिज्ञा(मूल्य);

        l = hashmap_get(h, किल्ली);
        जर (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                जर (strv_contains(l, मूल्य))
                        परत 0;

                r = strv_extend(&l, मूल्य);
                जर (r < 0)
                        परत r;

                assert_se(hashmap_update(h, किल्ली, l) >= 0);
        } अन्यथा {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ अक्षर **l2 = रिक्त;
                _cleanup_free_ अक्षर *t = रिक्त;

                t = strdup(किल्ली);
                जर (!t)
                        परत -ENOMEM;

                r = strv_extend(&l2, मूल्य);
                जर (r < 0)
                        परत r;

                r = hashmap_put(h, t, l2);
                जर (r < 0)
                        परत r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        परत 1;
}

पूर्णांक string_strv_hashmap_put(Hashmap **h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(किल्ली);
        प्रतिज्ञा(मूल्य);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        जर (r < 0)
                परत r;

        परत string_strv_hashmap_put_internal(*h, किल्ली, मूल्य);
}

पूर्णांक string_strv_ordered_hashmap_put(OrderedHashmap **h, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);
        प्रतिज्ञा(किल्ली);
        प्रतिज्ञा(मूल्य);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        जर (r < 0)
                परत r;

        परत string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), किल्ली, मूल्य);
}

स्थिर पूर्णांक rebreak_flush_ओळ(
                अक्षर ***broken,
                स्थिरांक अक्षर *start,
                स्थिरांक अक्षर *end,
                bool in_prefix,
                स्थिरांक अक्षर *whitespace_begin,
                स्थिरांक अक्षर *whitespace_end) {

        प्रतिज्ञा(broken);
        प्रतिज्ञा(start);
        प्रतिज्ञा(end >= start);

        जर (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                परत strv_extend(broken, "");

        जर (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ अक्षर *truncated = strndup(start, end - start);
        जर (!truncated)
                परत -ENOMEM;

        परत strv_consume(broken, TAKE_PTR(truncated));
}

पूर्णांक strv_rebreak_ओळी(अक्षर **l, माप_प्रकार width, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **broken = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        जर (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                जर (!broken)
                        परत -ENOMEM;

                *ret = TAKE_PTR(broken);
                परत 0;
        }

        STRV_FOREACH(i, l) {
                स्थिरांक अक्षर *start = *i, *whitespace_begin = रिक्त, *whitespace_end = रिक्त;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                माप_प्रकार w = 0;

                साठी (स्थिरांक अक्षर *p = start; *p != 0; ) {
                        जर (म_अक्षर(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_ओळ(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                जर (r < 0)
                                        परत r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                सुरूठेवा;
                        } अन्यथा जर (म_अक्षर(WHITESPACE, *p)) {
                                जर (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = रिक्त;
                                }
                        } अन्यथा {
                                जर (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        पूर्णांक cw = utf8_char_console_width(p);
                        जर (cw < 0)
                                परत log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        जर (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ अक्षर *truncated = रिक्त;

                                truncated = strndup(start, whitespace_begin - start);
                                जर (!truncated)
                                        परत -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                जर (r < 0)
                                        परत r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = रिक्त;
                                w = 0;
                                सुरूठेवा;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                प्रतिज्ञा(start);
                जर (after_newline && in_prefix)
                        सुरूठेवा;

                r = rebreak_flush_ओळ(&broken, start, start + म_माप(start), in_prefix, whitespace_begin, whitespace_end);
                जर (r < 0)
                        परत r;
        }

        *ret = TAKE_PTR(broken);
        परत 0;
}

अक्षर** strv_filter_prefix(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        जर (isempty(prefix))
                परत strv_copy(l);

        _cleanup_strv_free_ अक्षर **f = रिक्त;
        माप_प्रकार sz = 0;

        STRV_FOREACH(i, l) {
                जर (!startswith(*i, prefix))
                        सुरूठेवा;

                जर (strv_extend_with_size(&f, &sz, *i) < 0)
                        परत रिक्त;
        }

        परत TAKE_PTR(f);
}

स्थिरांक अक्षर* स्थिरांक strv_empty[] = { रिक्त };
