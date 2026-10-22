/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fnmatch.h>
#समावेश <stdio.h>

#समावेश "alloc-util.h"
#समावेश "env-util.h"
#समावेश "escape.h"
#समावेश "extract-word.h"
#समावेश "fileio.h"
#समावेश "gunicode.h"
#समावेश "hashmap.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "sort-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "utf8.h"

अक्षर* strv_find(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        निश्चित(name);

        STRV_FOREACH(i, l)
                अगर (streq(*i, name))
                        वापस *i;

        वापस शून्य;
}

अक्षर* strv_find_case(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        निश्चित(name);

        STRV_FOREACH(i, l)
                अगर (strcaseeq(*i, name))
                        वापस *i;

        वापस शून्य;
}

अक्षर* strv_find_prefix(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        निश्चित(name);

        STRV_FOREACH(i, l)
                अगर (startswith(*i, name))
                        वापस *i;

        वापस शून्य;
}

अक्षर* strv_find_startswith(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        निश्चित(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                अक्षर *e;

                e = startswith(*i, name);
                अगर (e)
                        वापस e;
        }

        वापस शून्य;
}

अटल अक्षर* strv_find_closest_prefix(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        माप_प्रकार best_distance = SIZE_MAX;
        अक्षर *best = शून्य;

        निश्चित(name);

        STRV_FOREACH(s, l) {
                अक्षर *e = startswith(*s, name);
                अगर (!e)
                        जारी;

                माप_प्रकार n = म_माप(e);
                अगर (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        वापस best;
}

अटल अक्षर* strv_find_closest_by_levenshtein(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        ssize_t best_distance = SSIZE_MAX;
        अक्षर *best = शून्य;

        निश्चित(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                अगर (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        वापस शून्य;
                }

                अगर (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        जारी;

                अगर (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        वापस best;
}

अक्षर* strv_find_closest(अक्षर * स्थिर *l, स्थिर अक्षर *name) {
        निश्चित(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        अक्षर *found = strv_find_closest_prefix(l, name);
        अगर (found)
                वापस found;

        वापस strv_find_closest_by_levenshtein(l, name);
}

अक्षर* strv_find_first_field(अक्षर * स्थिर *needles, अक्षर * स्थिर *haystack) {
        STRV_FOREACH(k, needles) {
                अक्षर *मूल्य = strv_env_pairs_get((अक्षर **)haystack, *k);
                अगर (मूल्य)
                        वापस मूल्य;
        }

        वापस शून्य;
}

अक्षर** strv_free(अक्षर **l) {
        STRV_FOREACH(k, l)
                मुक्त(*k);

        वापस mfree(l);
}

अक्षर** strv_free_erase(अक्षर **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        वापस mfree(l);
}

व्योम strv_free_many(अक्षर ***strvs, माप_प्रकार n) {
        निश्चित(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        मुक्त(strvs);
}

अक्षर** strv_copy_n(अक्षर * स्थिर *l, माप_प्रकार n) {
        _cleanup_strv_free_ अक्षर **परिणाम = शून्य;
        अक्षर **k;

        परिणाम = new(अक्षर*, MIN(strv_लंबाई(l), n) + 1);
        अगर (!परिणाम)
                वापस शून्य;

        k = परिणाम;
        STRV_FOREACH(i, l) {
                अगर (n == 0)
                        अवरोध;

                *k = strdup(*i);
                अगर (!*k)
                        वापस शून्य;
                k++;

                अगर (n != SIZE_MAX)
                        n--;
        }

        *k = शून्य;
        वापस TAKE_PTR(परिणाम);
}

पूर्णांक strv_copy_unless_empty(अक्षर * स्थिर *l, अक्षर ***ret) {
        निश्चित(ret);

        अगर (strv_isempty(l)) {
                *ret = शून्य;
                वापस 0;
        }

        अक्षर **copy = strv_copy(l);
        अगर (!copy)
                वापस -ENOMEM;

        *ret = TAKE_PTR(copy);
        वापस 1;
}

माप_प्रकार strv_लंबाई(अक्षर * स्थिर *l) {
        माप_प्रकार n = 0;

        STRV_FOREACH(i, l)
                n++;

        वापस n;
}

अक्षर** strv_new_ap(स्थिर अक्षर *x, बहु_सूची ap) {
        _cleanup_strv_free_ अक्षर **a = शून्य;
        माप_प्रकार n = 0, i = 0;
        बहु_सूची aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        क्रम (स्थिर अक्षर *s = x; s; s = बहु_तर्क(aq, स्थिर अक्षर*)) {
                अगर (s == STRV_IGNORE)
                        जारी;

                n++;
        }
        बहु_पूर्ण(aq);

        a = new(अक्षर*, n+1);
        अगर (!a)
                वापस शून्य;

        क्रम (स्थिर अक्षर *s = x; s; s = बहु_तर्क(ap, स्थिर अक्षर*)) {
                अगर (s == STRV_IGNORE)
                        जारी;

                a[i] = strdup(s);
                अगर (!a[i])
                        वापस शून्य;

                i++;
        }

        a[i] = शून्य;

        वापस TAKE_PTR(a);
}

अक्षर** strv_new_internal(स्थिर अक्षर *x, ...) {
        अक्षर **r;
        बहु_सूची ap;

        बहु_शुरू(ap, x);
        r = strv_new_ap(x, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक strv_extend_strv(अक्षर ***a, अक्षर * स्थिर *b, bool filter_duplicates) {
        माप_प्रकार p, q, i = 0;

        निश्चित(a);

        q = strv_लंबाई(b);
        अगर (q == 0)
                वापस 0;

        p = strv_लंबाई(*a);
        अगर (p >= SIZE_MAX - q)
                वापस -ENOMEM;

        अक्षर **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), माप(अक्षर *));
        अगर (!t)
                वापस -ENOMEM;

        t[p] = शून्य;
        *a = t;

        STRV_FOREACH(s, b) {
                अगर (filter_duplicates && strv_contains(t, *s))
                        जारी;

                t[p+i] = strdup(*s);
                अगर (!t[p+i])
                        जाओ rollback;

                i++;
                t[p+i] = शून्य;
        }

        निश्चित(i <= q);

        वापस (पूर्णांक) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = शून्य;
        वापस -ENOMEM;
}

पूर्णांक strv_extend_strv_consume(अक्षर ***a, अक्षर **b, bool filter_duplicates) {
        _cleanup_strv_free_ अक्षर **b_consume = b;
        माप_प्रकार p, q, i;

        निश्चित(a);

        q = strv_लंबाई(b);
        अगर (q == 0)
                वापस 0;

        p = strv_लंबाई(*a);
        अगर (p == 0) {
                strv_free_and_replace(*a, b_consume);

                अगर (filter_duplicates)
                        strv_uniq(*a);

                वापस strv_लंबाई(*a);
        }

        अगर (p >= SIZE_MAX - q)
                वापस -ENOMEM;

        अक्षर **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), माप(अक्षर *));
        अगर (!t)
                वापस -ENOMEM;

        t[p] = शून्य;
        *a = t;

        अगर (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = शून्य;
                i = q;
        } अन्यथा {
                i = 0;

                STRV_FOREACH(s, b) {
                        अगर (strv_contains(t, *s)) {
                                मुक्त(*s);
                                जारी;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = शून्य;
                }
        }

        निश्चित(i <= q);

        b_consume = mfree(b_consume);

        वापस (पूर्णांक) i;
}

पूर्णांक strv_extend_strv_biconcat(अक्षर ***a, स्थिर अक्षर *prefix, स्थिर अक्षर* स्थिर *b, स्थिर अक्षर *suffix) {
        पूर्णांक r;

        निश्चित(a);

        STRV_FOREACH(s, b) {
                अक्षर *v;

                v = strjoin(strempty(prefix), *s, suffix);
                अगर (!v)
                        वापस -ENOMEM;

                r = strv_consume(a, v);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक strv_split_newlines_full(अक्षर ***ret, स्थिर अक्षर *s, ExtractFlags flags) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(ret);
        निश्चित(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        अगर (r < 0)
                वापस r;

        n = strv_लंबाई(l);
        अगर (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        वापस n;
}

अक्षर** strv_split_newlines(स्थिर अक्षर *s) {
        अक्षर **ret;

        अगर (strv_split_newlines_full(&ret, s, 0) < 0)
                वापस शून्य;

        वापस ret;
}

पूर्णांक strv_split_full(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, ExtractFlags flags) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        माप_प्रकार n = 0;
        पूर्णांक r;

        निश्चित(t);
        निश्चित(s);

        क्रम (;;) {
                _cleanup_free_ अक्षर *word = शून्य;

                r = extract_first_word(&s, &word, separators, flags);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अगर (!GREEDY_REALLOC(l, n + 2))
                        वापस -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = शून्य;
        }

        अगर (!l) {
                l = new0(अक्षर*, 1);
                अगर (!l)
                        वापस -ENOMEM;
        }

        *t = TAKE_PTR(l);

        वापस (पूर्णांक) n;
}

अक्षर** strv_split(स्थिर अक्षर *s, स्थिर अक्षर *separators) {
        अक्षर **ret;

        अगर (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                वापस शून्य;

        वापस ret;
}

पूर्णांक strv_split_and_extend_full(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, bool filter_duplicates, ExtractFlags flags) {
        अक्षर **l;
        पूर्णांक r;

        निश्चित(t);
        निश्चित(s);

        r = strv_split_full(&l, s, separators, flags);
        अगर (r < 0)
                वापस r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        अगर (r < 0)
                वापस r;

        वापस (पूर्णांक) strv_लंबाई(*t);
}

पूर्णांक strv_split_and_extend(अक्षर ***t, स्थिर अक्षर *s, स्थिर अक्षर *separators, bool filter_duplicates) {
        वापस strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

पूर्णांक strv_split_colon_pairs(अक्षर ***t, स्थिर अक्षर *s) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        माप_प्रकार n = 0;
        पूर्णांक r;

        निश्चित(t);
        निश्चित(s);

        क्रम (;;) {
                _cleanup_free_ अक्षर *first = शून्य, *second = शून्य, *tuple = शून्य, *second_or_empty = शून्य;

                r = extract_first_word(&s, &tuple, शून्य, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                स्थिर अक्षर *p = tuple;
                r = extract_many_शब्दगण(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        जारी;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                अगर (!isempty(p))
                        वापस -EINVAL;

                second_or_empty = strdup(strempty(second));
                अगर (!second_or_empty)
                        वापस -ENOMEM;

                अगर (!GREEDY_REALLOC(l, n + 3))
                        वापस -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = शून्य;
        }

        अगर (!l) {
                l = new0(अक्षर*, 1);
                अगर (!l)
                        वापस -ENOMEM;
        }

        *t = TAKE_PTR(l);

        वापस (पूर्णांक) n;
}

अक्षर* strv_join_full(अक्षर * स्थिर *l, स्थिर अक्षर *separator, स्थिर अक्षर *prefix) {
        अक्षर *r, *e;
        माप_प्रकार n, k, m;

        अगर (!separator)
                separator = " ";

        k = म_माप(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                अगर (s != l)
                        n += k;

                n += m + म_माप(*s);
        }

        r = new(अक्षर, n+1);
        अगर (!r)
                वापस शून्य;

        e = r;
        STRV_FOREACH(s, l) {
                अगर (s != l)
                        e = stpcpy(e, separator);

                अगर (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        वापस r;
}

पूर्णांक strv_धकेलो_with_आकार(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        निश्चित(l);
        POINTER_MAY_BE_NULL(n);

        अगर (!मूल्य)
                वापस 0;

        माप_प्रकार आकार = n ? *n : SIZE_MAX;
        अगर (आकार == SIZE_MAX)
                आकार = strv_लंबाई(*l);

        /* Check for overflow */
        अगर (आकार > SIZE_MAX-2)
                वापस -ENOMEM;

        अक्षर **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(आकार + 2), माप(अक्षर*));
        अगर (!c)
                वापस -ENOMEM;

        c[आकार] = मूल्य;
        c[आकार+1] = शून्य;

        *l = c;
        अगर (n)
                *n = आकार + 1;
        वापस 0;
}

पूर्णांक strv_धकेलो_pair(अक्षर ***l, अक्षर *a, अक्षर *b) {
        अक्षर **c;
        माप_प्रकार n;

        निश्चित(l);

        अगर (!a && !b)
                वापस 0;

        n = strv_लंबाई(*l);

        /* Check for overflow */
        अगर (n > SIZE_MAX-3)
                वापस -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), माप(अक्षर*));
        अगर (!c)
                वापस -ENOMEM;

        अगर (a)
                c[n++] = a;
        अगर (b)
                c[n++] = b;
        c[n] = शून्य;

        *l = c;
        वापस 0;
}

पूर्णांक strv_जोड़ो(अक्षर ***l, माप_प्रकार position, अक्षर *मूल्य) {
        अक्षर **c;
        माप_प्रकार n, m;

        निश्चित(l);

        अगर (!मूल्य)
                वापस 0;

        n = strv_लंबाई(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        अगर (n > SIZE_MAX - 2)
                वापस -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), माप(अक्षर*));
        अगर (!c)
                वापस -ENOMEM;

        अगर (n > position)
                स_हटाओ(c + position + 1, c + position, (n - position) * माप(अक्षर*));

        c[position] = मूल्य;
        c[n + 1] = शून्य;

        *l = c;
        वापस 0;
}

पूर्णांक strv_consume_with_आकार(अक्षर ***l, माप_प्रकार *n, अक्षर *मूल्य) {
        पूर्णांक r;

        r = strv_धकेलो_with_आकार(l, n, मूल्य);
        अगर (r < 0)
                मुक्त(मूल्य);

        वापस r;
}

पूर्णांक strv_consume_pair(अक्षर ***l, अक्षर *a, अक्षर *b) {
        पूर्णांक r;

        r = strv_धकेलो_pair(l, a, b);
        अगर (r < 0) {
                मुक्त(a);
                मुक्त(b);
        }

        वापस r;
}

पूर्णांक strv_consume_prepend(अक्षर ***l, अक्षर *मूल्य) {
        पूर्णांक r;

        r = strv_धकेलो_prepend(l, मूल्य);
        अगर (r < 0)
                मुक्त(मूल्य);

        वापस r;
}

पूर्णांक strv_prepend(अक्षर ***l, स्थिर अक्षर *मूल्य) {
        अक्षर *v;

        अगर (!मूल्य)
                वापस 0;

        v = strdup(मूल्य);
        अगर (!v)
                वापस -ENOMEM;

        वापस strv_consume_prepend(l, v);
}

पूर्णांक strv_extend_with_आकार(अक्षर ***l, माप_प्रकार *n, स्थिर अक्षर *मूल्य) {
        अक्षर *v;

        अगर (!मूल्य)
                वापस 0;

        v = strdup(मूल्य);
        अगर (!v)
                वापस -ENOMEM;

        वापस strv_consume_with_आकार(l, n, v);
}

पूर्णांक strv_extend_many_internal(अक्षर ***l, स्थिर अक्षर *मूल्य, ...) {
        बहु_सूची ap;
        माप_प्रकार n, m;
        पूर्णांक r;

        निश्चित(l);

        m = n = strv_लंबाई(*l);

        r = 0;
        बहु_शुरू(ap, मूल्य);
        क्रम (स्थिर अक्षर *s = मूल्य; s != POINTER_MAX; s = बहु_तर्क(ap, स्थिर अक्षर*)) {
                अगर (!s)
                        जारी;

                अगर (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        अवरोध;
                }
                m++;
        }
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस r;
        अगर (m > SIZE_MAX-1)
                वापस -ENOMEM;

        अक्षर **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), माप(अक्षर*));
        अगर (!c)
                वापस -ENOMEM;
        *l = c;

        r = 0;
        माप_प्रकार i = n;
        बहु_शुरू(ap, मूल्य);
        क्रम (स्थिर अक्षर *s = मूल्य; s != POINTER_MAX; s = बहु_तर्क(ap, स्थिर अक्षर*)) {
                अगर (!s)
                        जारी;

                c[i] = strdup(s);
                अगर (!c[i]) {
                        r = -ENOMEM;
                        अवरोध;
                }
                i++;
        }
        बहु_पूर्ण(ap);

        अगर (r < 0) {
                /* rollback on error */
                क्रम (माप_प्रकार j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                वापस r;
        }

        c[i] = शून्य;
        वापस 0;
}

अक्षर** strv_uniq(अक्षर **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        वापस l;
}

bool strv_is_uniq(अक्षर * स्थिर *l) {
        STRV_FOREACH(i, l)
                अगर (strv_contains(i+1, *i))
                        वापस false;

        वापस true;
}

अक्षर** strv_remove(अक्षर **l, स्थिर अक्षर *s) {
        अगर (!l)
                वापस शून्य;

        निश्चित(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        अक्षर **f, **t;
        क्रम (f = t = l; *f; f++)
                अगर (streq(*f, s))
                        मुक्त(*f);
                अन्यथा
                        *(t++) = *f;

        *t = शून्य;
        वापस l;
}

अक्षर** strv_remove_strv(अक्षर **l, अक्षर *स्थिर*ll) {

        अगर (strv_isempty(l))
                वापस l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        वापस l;
}

bool strv_overlap(अक्षर * स्थिर *a, अक्षर * स्थिर *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                अगर (strv_contains(b, *i))
                        वापस true;

        वापस false;
}

अटल पूर्णांक str_compare(अक्षर * स्थिर *a, अक्षर * स्थिर *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        वापस म_भेद(*a, *b);
}

अक्षर** strv_sort(अक्षर **l) {
        typesafe_qsort(l, strv_लंबाई(l), str_compare);
        वापस l;
}

अक्षर** strv_sort_uniq_full(अक्षर **l, पूर्णांक (*compare)(अक्षर * स्थिर *a, अक्षर * स्थिर *b)) {
        निश्चित(compare);

        अगर (strv_isempty(l))
                वापस l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_लंबाई(l), compare);

        अक्षर **पुच्छ_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                अगर (compare(i, पुच्छ_2 - 1) == 0)
                        मुक्त(*i);
                अन्यथा
                        *(पुच्छ_2++) = *i;

        *पुच्छ_2 = शून्य;
        वापस l;
}

अक्षर** strv_sort_uniq(अक्षर **l) {
        वापस strv_sort_uniq_full(l, str_compare);
}

पूर्णांक strv_compare(अक्षर * स्थिर *a, अक्षर * स्थिर *b) {
        पूर्णांक r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        अगर (strv_isempty(a)) {
                अगर (strv_isempty(b))
                        वापस 0;
                अन्यथा
                        वापस -1;
        }

        अगर (strv_isempty(b))
                वापस 1;

        क्रम ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                अगर (r != 0)
                        वापस r;
        }

        वापस 0;
}

bool strv_equal_ignore_order(अक्षर * स्थिर *a, अक्षर * स्थिर *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        अगर (a == b)
                वापस true;

        STRV_FOREACH(i, a)
                अगर (!strv_contains(b, *i))
                        वापस false;

        STRV_FOREACH(i, b)
                अगर (!strv_contains(a, *i))
                        वापस false;

        वापस true;
}

व्योम strv_print_full(अक्षर * स्थिर *l, स्थिर अक्षर *prefix) {
        STRV_FOREACH(s, l)
                म_लिखो("%s%s\n", strempty(prefix), *s);
}

पूर्णांक strv_extendf_with_आकार(अक्षर ***l, माप_प्रकार *n, स्थिर अक्षर *format, ...) {
        बहु_सूची ap;
        अक्षर *x;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&x, format, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस -ENOMEM;

        वापस strv_consume_with_आकार(l, n, x);
}

पूर्णांक strv_extend_joined_with_आकार_sentinel(अक्षर ***l, माप_प्रकार *n, ...) {
        बहु_सूची ap;

        बहु_शुरू(ap, n);
        अक्षर *x = strextendv_with_separator(/* x= */ शून्य, /* separator= */ शून्य, ap);
        बहु_पूर्ण(ap);
        अगर (!x)
                वापस -ENOMEM;

        वापस strv_consume_with_आकार(l, n, x);
}

अक्षर* startswith_strv_internal(स्थिर अक्षर *s, अक्षर * स्थिर *l) {
        STRV_FOREACH(i, l) {
                अक्षर *found = (अक्षर*) startswith(s, *i);
                अगर (found)
                        वापस found;
        }

        वापस शून्य;
}

अक्षर* endswith_strv_internal(स्थिर अक्षर *s, अक्षर * स्थिर *l) {
        STRV_FOREACH(i, l) {
                अक्षर *found = (अक्षर*) endswith(s, *i);
                अगर (found)
                        वापस found;
        }

        वापस शून्य;
}

अक्षर** strv_reverse(अक्षर **l) {
        माप_प्रकार n;

        n = strv_लंबाई(l);
        अगर (n <= 1)
                वापस l;

        क्रम (माप_प्रकार i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        वापस l;
}

अक्षर** strv_shell_escape(अक्षर **l, स्थिर अक्षर *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                अक्षर *v;

                v = shell_escape(*s, bad);
                अगर (!v)
                        वापस शून्य;

                free_and_replace(*s, v);
        }

        वापस l;
}

bool strv_fnmatch_full(
                अक्षर* स्थिर* patterns,
                स्थिर अक्षर *s,
                पूर्णांक flags,
                माप_प्रकार *ret_matched_pos) {

        निश्चित(s);

        अगर (patterns)
                क्रम (माप_प्रकार i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        अगर (fnmatch(patterns[i], s, flags) == 0) {
                                अगर (ret_matched_pos)
                                        *ret_matched_pos = i;
                                वापस true;
                        }

        अगर (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        वापस false;
}

अक्षर** strv_skip(अक्षर **l, माप_प्रकार n) {
        जबतक (n > 0) {
                अगर (strv_isempty(l))
                        वापस शून्य;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        अगर (strv_isempty(l))
                वापस शून्य;
        वापस l;
}

पूर्णांक strv_extend_n(अक्षर ***l, स्थिर अक्षर *मूल्य, माप_प्रकार n) {
        माप_प्रकार i, k;
        अक्षर **nl;

        निश्चित(l);

        अगर (!मूल्य)
                वापस 0;
        अगर (n == 0)
                वापस 0;

        /* Adds the value n times to l */

        k = strv_लंबाई(*l);
        अगर (n >= SIZE_MAX - k)
                वापस -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), माप(अक्षर *));
        अगर (!nl)
                वापस -ENOMEM;

        *l = nl;

        क्रम (i = k; i < k + n; i++) {
                nl[i] = strdup(मूल्य);
                अगर (!nl[i])
                        जाओ rollback;
        }
        nl[i] = शून्य;

        वापस 0;

rollback:
        क्रम (माप_प्रकार j = k; j < i; j++)
                मुक्त(nl[j]);
        nl[k] = शून्य;

        वापस -ENOMEM;
}

पूर्णांक strv_extend_assignment(अक्षर ***l, स्थिर अक्षर *lhs, स्थिर अक्षर *rhs) {
        अक्षर *j;

        निश्चित(l);
        निश्चित(lhs);

        अगर (!rhs) /* value is optional, in which case we suppress the field */
                वापस 0;

        j = strjoin(lhs, "=", rhs);
        अगर (!j)
                वापस -ENOMEM;

        वापस strv_consume(l, j);
}

पूर्णांक fputstrv(खाता *f, अक्षर * स्थिर *l, स्थिर अक्षर *separator, bool *space) {
        bool b = false;
        पूर्णांक r;

        निश्चित(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        अगर (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

व्योम string_strv_hashmap_remove(Hashmap *h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        निश्चित(कुंजी);

        अगर (मूल्य) {
                अक्षर **l = hashmap_get(h, कुंजी);
                अगर (!l)
                        वापस;

                strv_remove(l, मूल्य);
                अगर (!strv_isempty(l))
                        वापस;
        }

        _unused_ _cleanup_free_ अक्षर *कुंजी_free = शून्य;
        strv_free(hashmap_remove2(h, कुंजी, (व्योम**) &कुंजी_free));
}

व्योम string_strv_ordered_hashmap_remove(OrderedHashmap *h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), कुंजी, मूल्य);
}

अटल पूर्णांक string_strv_hashmap_put_internal(Hashmap *h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        अक्षर **l;
        पूर्णांक r;

        निश्चित(h);
        निश्चित(कुंजी);
        निश्चित(मूल्य);

        l = hashmap_get(h, कुंजी);
        अगर (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                अगर (strv_contains(l, मूल्य))
                        वापस 0;

                r = strv_extend(&l, मूल्य);
                अगर (r < 0)
                        वापस r;

                assert_se(hashmap_update(h, कुंजी, l) >= 0);
        } अन्यथा {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ अक्षर **l2 = शून्य;
                _cleanup_free_ अक्षर *t = शून्य;

                t = strdup(कुंजी);
                अगर (!t)
                        वापस -ENOMEM;

                r = strv_extend(&l2, मूल्य);
                अगर (r < 0)
                        वापस r;

                r = hashmap_put(h, t, l2);
                अगर (r < 0)
                        वापस r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        वापस 1;
}

पूर्णांक string_strv_hashmap_put(Hashmap **h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        पूर्णांक r;

        निश्चित(h);
        निश्चित(कुंजी);
        निश्चित(मूल्य);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        अगर (r < 0)
                वापस r;

        वापस string_strv_hashmap_put_internal(*h, कुंजी, मूल्य);
}

पूर्णांक string_strv_ordered_hashmap_put(OrderedHashmap **h, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        पूर्णांक r;

        निश्चित(h);
        निश्चित(कुंजी);
        निश्चित(मूल्य);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        अगर (r < 0)
                वापस r;

        वापस string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), कुंजी, मूल्य);
}

अटल पूर्णांक rebreak_flush_पंक्ति(
                अक्षर ***broken,
                स्थिर अक्षर *start,
                स्थिर अक्षर *end,
                bool in_prefix,
                स्थिर अक्षर *whitespace_begin,
                स्थिर अक्षर *whitespace_end) {

        निश्चित(broken);
        निश्चित(start);
        निश्चित(end >= start);

        अगर (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                वापस strv_extend(broken, "");

        अगर (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ अक्षर *truncated = strndup(start, end - start);
        अगर (!truncated)
                वापस -ENOMEM;

        वापस strv_consume(broken, TAKE_PTR(truncated));
}

पूर्णांक strv_rebreak_पंक्तियाँ(अक्षर **l, माप_प्रकार width, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **broken = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        अगर (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                अगर (!broken)
                        वापस -ENOMEM;

                *ret = TAKE_PTR(broken);
                वापस 0;
        }

        STRV_FOREACH(i, l) {
                स्थिर अक्षर *start = *i, *whitespace_begin = शून्य, *whitespace_end = शून्य;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                माप_प्रकार w = 0;

                क्रम (स्थिर अक्षर *p = start; *p != 0; ) {
                        अगर (म_अक्षर(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_पंक्ति(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                अगर (r < 0)
                                        वापस r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = शून्य;
                                w = 0;
                                जारी;
                        } अन्यथा अगर (म_अक्षर(WHITESPACE, *p)) {
                                अगर (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = शून्य;
                                }
                        } अन्यथा {
                                अगर (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        पूर्णांक cw = utf8_char_console_width(p);
                        अगर (cw < 0)
                                वापस log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        अगर (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ अक्षर *truncated = शून्य;

                                truncated = strndup(start, whitespace_begin - start);
                                अगर (!truncated)
                                        वापस -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                अगर (r < 0)
                                        वापस r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = शून्य;
                                w = 0;
                                जारी;
                        }

                        p = utf8_अग्र_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                निश्चित(start);
                अगर (after_newline && in_prefix)
                        जारी;

                r = rebreak_flush_पंक्ति(&broken, start, start + म_माप(start), in_prefix, whitespace_begin, whitespace_end);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(broken);
        वापस 0;
}

अक्षर** strv_filter_prefix(अक्षर * स्थिर *l, स्थिर अक्षर *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        अगर (isempty(prefix))
                वापस strv_copy(l);

        _cleanup_strv_free_ अक्षर **f = शून्य;
        माप_प्रकार sz = 0;

        STRV_FOREACH(i, l) {
                अगर (!startswith(*i, prefix))
                        जारी;

                अगर (strv_extend_with_आकार(&f, &sz, *i) < 0)
                        वापस शून्य;
        }

        वापस TAKE_PTR(f);
}

स्थिर अक्षर* स्थिर strv_empty[] = { शून्य };
