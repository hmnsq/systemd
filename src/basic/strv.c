/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <stdio.h>

#include "alloc-util.h"
#include "env-util.h"
#include "escape.h"
#include "extract-word.h"
#include "fileio.h"
#include "gunicode.h"
#include "hashmap.h"
#include "log.h"
#include "memory-util.h"
#include "sort-util.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

char* strv_find(char * const *l, const char *name) {
        assert(name);

        STRV_FOREACH(i, l)
                nếu (streq(*i, name))
                        trả_về *i;

        trả_về NULL;
}

char* strv_find_case(char * const *l, const char *name) {
        assert(name);

        STRV_FOREACH(i, l)
                nếu (strcaseeq(*i, name))
                        trả_về *i;

        trả_về NULL;
}

char* strv_find_prefix(char * const *l, const char *name) {
        assert(name);

        STRV_FOREACH(i, l)
                nếu (startswith(*i, name))
                        trả_về *i;

        trả_về NULL;
}

char* strv_find_startswith(char * const *l, const char *name) {
        assert(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                char *e;

                e = startswith(*i, name);
                nếu (e)
                        trả_về e;
        }

        trả_về NULL;
}

static char* strv_find_closest_prefix(char * const *l, const char *name) {
        size_t best_distance = SIZE_MAX;
        char *best = NULL;

        assert(name);

        STRV_FOREACH(s, l) {
                char *e = startswith(*s, name);
                nếu (!e)
                        continue;

                size_t n = strlen(e);
                nếu (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        trả_về best;
}

static char* strv_find_closest_by_levenshtein(char * const *l, const char *name) {
        ssize_t best_distance = SSIZE_MAX;
        char *best = NULL;

        assert(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                nếu (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        trả_về NULL;
                }

                nếu (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        continue;

                nếu (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        trả_về best;
}

char* strv_find_closest(char * const *l, const char *name) {
        assert(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        char *found = strv_find_closest_prefix(l, name);
        nếu (found)
                trả_về found;

        trả_về strv_find_closest_by_levenshtein(l, name);
}

char* strv_find_first_field(char * const *needles, char * const *haystack) {
        STRV_FOREACH(k, needles) {
                char *value = strv_env_pairs_get((char **)haystack, *k);
                nếu (value)
                        trả_về value;
        }

        trả_về NULL;
}

char** strv_free(char **l) {
        STRV_FOREACH(k, l)
                free(*k);

        trả_về mfree(l);
}

char** strv_free_erase(char **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        trả_về mfree(l);
}

void strv_free_many(char ***strvs, size_t n) {
        assert(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        free(strvs);
}

char** strv_copy_n(char * const *l, size_t n) {
        _cleanup_strv_free_ char **result = NULL;
        char **k;

        result = new(char*, MIN(strv_length(l), n) + 1);
        nếu (!result)
                trả_về NULL;

        k = result;
        STRV_FOREACH(i, l) {
                nếu (n == 0)
                        break;

                *k = strdup(*i);
                nếu (!*k)
                        trả_về NULL;
                k++;

                nếu (n != SIZE_MAX)
                        n--;
        }

        *k = NULL;
        trả_về TAKE_PTR(result);
}

số_nguyên strv_copy_unless_empty(char * const *l, char ***ret) {
        assert(ret);

        nếu (strv_isempty(l)) {
                *ret = NULL;
                trả_về 0;
        }

        char **copy = strv_copy(l);
        nếu (!copy)
                trả_về -ENOMEM;

        *ret = TAKE_PTR(copy);
        trả_về 1;
}

size_t strv_length(char * const *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        trả_về n;
}

char** strv_new_ap(const char *x, va_list ap) {
        _cleanup_strv_free_ char **a = NULL;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        cho (const char *s = x; s; s = va_arg(aq, const char*)) {
                nếu (s == STRV_IGNORE)
                        continue;

                n++;
        }
        va_end(aq);

        a = new(char*, n+1);
        nếu (!a)
                trả_về NULL;

        cho (const char *s = x; s; s = va_arg(ap, const char*)) {
                nếu (s == STRV_IGNORE)
                        continue;

                a[i] = strdup(s);
                nếu (!a[i])
                        trả_về NULL;

                i++;
        }

        a[i] = NULL;

        trả_về TAKE_PTR(a);
}

char** strv_new_internal(const char *x, ...) {
        char **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        trả_về r;
}

số_nguyên strv_extend_strv(char ***a, char * const *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        assert(a);

        q = strv_length(b);
        nếu (q == 0)
                trả_về 0;

        p = strv_length(*a);
        nếu (p >= SIZE_MAX - q)
                trả_về -ENOMEM;

        char **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), sizeof(char *));
        nếu (!t)
                trả_về -ENOMEM;

        t[p] = NULL;
        *a = t;

        STRV_FOREACH(s, b) {
                nếu (filter_duplicates && strv_contains(t, *s))
                        continue;

                t[p+i] = strdup(*s);
                nếu (!t[p+i])
                        goto rollback;

                i++;
                t[p+i] = NULL;
        }

        assert(i <= q);

        trả_về (số_nguyên) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = NULL;
        trả_về -ENOMEM;
}

số_nguyên strv_extend_strv_consume(char ***a, char **b, bool filter_duplicates) {
        _cleanup_strv_free_ char **b_consume = b;
        size_t p, q, i;

        assert(a);

        q = strv_length(b);
        nếu (q == 0)
                trả_về 0;

        p = strv_length(*a);
        nếu (p == 0) {
                strv_free_and_replace(*a, b_consume);

                nếu (filter_duplicates)
                        strv_uniq(*a);

                trả_về strv_length(*a);
        }

        nếu (p >= SIZE_MAX - q)
                trả_về -ENOMEM;

        char **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), sizeof(char *));
        nếu (!t)
                trả_về -ENOMEM;

        t[p] = NULL;
        *a = t;

        nếu (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = NULL;
                i = q;
        } không_thì {
                i = 0;

                STRV_FOREACH(s, b) {
                        nếu (strv_contains(t, *s)) {
                                free(*s);
                                continue;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = NULL;
                }
        }

        assert(i <= q);

        b_consume = mfree(b_consume);

        trả_về (số_nguyên) i;
}

số_nguyên strv_extend_strv_biconcat(char ***a, const char *prefix, const char* const *b, const char *suffix) {
        số_nguyên r;

        assert(a);

        STRV_FOREACH(s, b) {
                char *v;

                v = strjoin(strempty(prefix), *s, suffix);
                nếu (!v)
                        trả_về -ENOMEM;

                r = strv_consume(a, v);
                nếu (r < 0)
                        trả_về r;
        }

        trả_về 0;
}

số_nguyên strv_split_newlines_full(char ***ret, const char *s, ExtractFlags flags) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n;
        số_nguyên r;

        assert(ret);
        assert(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        nếu (r < 0)
                trả_về r;

        n = strv_length(l);
        nếu (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        trả_về n;
}

char** strv_split_newlines(const char *s) {
        char **ret;

        nếu (strv_split_newlines_full(&ret, s, 0) < 0)
                trả_về NULL;

        trả_về ret;
}

số_nguyên strv_split_full(char ***t, const char *s, const char *separators, ExtractFlags flags) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n = 0;
        số_nguyên r;

        assert(t);
        assert(s);

        cho (;;) {
                _cleanup_free_ char *word = NULL;

                r = extract_first_word(&s, &word, separators, flags);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                nếu (!GREEDY_REALLOC(l, n + 2))
                        trả_về -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = NULL;
        }

        nếu (!l) {
                l = new0(char*, 1);
                nếu (!l)
                        trả_về -ENOMEM;
        }

        *t = TAKE_PTR(l);

        trả_về (số_nguyên) n;
}

char** strv_split(const char *s, const char *separators) {
        char **ret;

        nếu (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                trả_về NULL;

        trả_về ret;
}

số_nguyên strv_split_and_extend_full(char ***t, const char *s, const char *separators, bool filter_duplicates, ExtractFlags flags) {
        char **l;
        số_nguyên r;

        assert(t);
        assert(s);

        r = strv_split_full(&l, s, separators, flags);
        nếu (r < 0)
                trả_về r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        nếu (r < 0)
                trả_về r;

        trả_về (số_nguyên) strv_length(*t);
}

số_nguyên strv_split_and_extend(char ***t, const char *s, const char *separators, bool filter_duplicates) {
        trả_về strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

số_nguyên strv_split_colon_pairs(char ***t, const char *s) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n = 0;
        số_nguyên r;

        assert(t);
        assert(s);

        cho (;;) {
                _cleanup_free_ char *first = NULL, *second = NULL, *tuple = NULL, *second_or_empty = NULL;

                r = extract_first_word(&s, &tuple, NULL, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                const char *p = tuple;
                r = extract_many_words(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        continue;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                nếu (!isempty(p))
                        trả_về -EINVAL;

                second_or_empty = strdup(strempty(second));
                nếu (!second_or_empty)
                        trả_về -ENOMEM;

                nếu (!GREEDY_REALLOC(l, n + 3))
                        trả_về -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = NULL;
        }

        nếu (!l) {
                l = new0(char*, 1);
                nếu (!l)
                        trả_về -ENOMEM;
        }

        *t = TAKE_PTR(l);

        trả_về (số_nguyên) n;
}

char* strv_join_full(char * const *l, const char *separator, const char *prefix) {
        char *r, *e;
        size_t n, k, m;

        nếu (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                nếu (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(char, n+1);
        nếu (!r)
                trả_về NULL;

        e = r;
        STRV_FOREACH(s, l) {
                nếu (s != l)
                        e = stpcpy(e, separator);

                nếu (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        trả_về r;
}

số_nguyên strv_push_with_size(char ***l, size_t *n, char *value) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        assert(l);
        POINTER_MAY_BE_NULL(n);

        nếu (!value)
                trả_về 0;

        size_t size = n ? *n : SIZE_MAX;
        nếu (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        nếu (size > SIZE_MAX-2)
                trả_về -ENOMEM;

        char **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), sizeof(char*));
        nếu (!c)
                trả_về -ENOMEM;

        c[size] = value;
        c[size+1] = NULL;

        *l = c;
        nếu (n)
                *n = size + 1;
        trả_về 0;
}

số_nguyên strv_push_pair(char ***l, char *a, char *b) {
        char **c;
        size_t n;

        assert(l);

        nếu (!a && !b)
                trả_về 0;

        n = strv_length(*l);

        /* Check for overflow */
        nếu (n > SIZE_MAX-3)
                trả_về -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), sizeof(char*));
        nếu (!c)
                trả_về -ENOMEM;

        nếu (a)
                c[n++] = a;
        nếu (b)
                c[n++] = b;
        c[n] = NULL;

        *l = c;
        trả_về 0;
}

số_nguyên strv_insert(char ***l, size_t position, char *value) {
        char **c;
        size_t n, m;

        assert(l);

        nếu (!value)
                trả_về 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        nếu (n > SIZE_MAX - 2)
                trả_về -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), sizeof(char*));
        nếu (!c)
                trả_về -ENOMEM;

        nếu (n > position)
                memmove(c + position + 1, c + position, (n - position) * sizeof(char*));

        c[position] = value;
        c[n + 1] = NULL;

        *l = c;
        trả_về 0;
}

số_nguyên strv_consume_with_size(char ***l, size_t *n, char *value) {
        số_nguyên r;

        r = strv_push_with_size(l, n, value);
        nếu (r < 0)
                free(value);

        trả_về r;
}

số_nguyên strv_consume_pair(char ***l, char *a, char *b) {
        số_nguyên r;

        r = strv_push_pair(l, a, b);
        nếu (r < 0) {
                free(a);
                free(b);
        }

        trả_về r;
}

số_nguyên strv_consume_prepend(char ***l, char *value) {
        số_nguyên r;

        r = strv_push_prepend(l, value);
        nếu (r < 0)
                free(value);

        trả_về r;
}

số_nguyên strv_prepend(char ***l, const char *value) {
        char *v;

        nếu (!value)
                trả_về 0;

        v = strdup(value);
        nếu (!v)
                trả_về -ENOMEM;

        trả_về strv_consume_prepend(l, v);
}

số_nguyên strv_extend_with_size(char ***l, size_t *n, const char *value) {
        char *v;

        nếu (!value)
                trả_về 0;

        v = strdup(value);
        nếu (!v)
                trả_về -ENOMEM;

        trả_về strv_consume_with_size(l, n, v);
}

số_nguyên strv_extend_many_internal(char ***l, const char *value, ...) {
        va_list ap;
        size_t n, m;
        số_nguyên r;

        assert(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, value);
        cho (const char *s = value; s != POINTER_MAX; s = va_arg(ap, const char*)) {
                nếu (!s)
                        continue;

                nếu (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        break;
                }
                m++;
        }
        va_end(ap);

        nếu (r < 0)
                trả_về r;
        nếu (m > SIZE_MAX-1)
                trả_về -ENOMEM;

        char **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), sizeof(char*));
        nếu (!c)
                trả_về -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, value);
        cho (const char *s = value; s != POINTER_MAX; s = va_arg(ap, const char*)) {
                nếu (!s)
                        continue;

                c[i] = strdup(s);
                nếu (!c[i]) {
                        r = -ENOMEM;
                        break;
                }
                i++;
        }
        va_end(ap);

        nếu (r < 0) {
                /* rollback on error */
                cho (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                trả_về r;
        }

        c[i] = NULL;
        trả_về 0;
}

char** strv_uniq(char **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        trả_về l;
}

bool strv_is_uniq(char * const *l) {
        STRV_FOREACH(i, l)
                nếu (strv_contains(i+1, *i))
                        trả_về false;

        trả_về true;
}

char** strv_remove(char **l, const char *s) {
        nếu (!l)
                trả_về NULL;

        assert(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        char **f, **t;
        cho (f = t = l; *f; f++)
                nếu (streq(*f, s))
                        free(*f);
                không_thì
                        *(t++) = *f;

        *t = NULL;
        trả_về l;
}

char** strv_remove_strv(char **l, char *const*ll) {

        nếu (strv_isempty(l))
                trả_về l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        trả_về l;
}

bool strv_overlap(char * const *a, char * const *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                nếu (strv_contains(b, *i))
                        trả_về true;

        trả_về false;
}

static số_nguyên str_compare(char * const *a, char * const *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        trả_về strcmp(*a, *b);
}

char** strv_sort(char **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        trả_về l;
}

char** strv_sort_uniq_full(char **l, số_nguyên (*compare)(char * const *a, char * const *b)) {
        assert(compare);

        nếu (strv_isempty(l))
                trả_về l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        char **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                nếu (compare(i, tail - 1) == 0)
                        free(*i);
                không_thì
                        *(tail++) = *i;

        *tail = NULL;
        trả_về l;
}

char** strv_sort_uniq(char **l) {
        trả_về strv_sort_uniq_full(l, str_compare);
}

số_nguyên strv_compare(char * const *a, char * const *b) {
        số_nguyên r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        nếu (strv_isempty(a)) {
                nếu (strv_isempty(b))
                        trả_về 0;
                không_thì
                        trả_về -1;
        }

        nếu (strv_isempty(b))
                trả_về 1;

        cho ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                nếu (r != 0)
                        trả_về r;
        }

        trả_về 0;
}

bool strv_equal_ignore_order(char * const *a, char * const *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        nếu (a == b)
                trả_về true;

        STRV_FOREACH(i, a)
                nếu (!strv_contains(b, *i))
                        trả_về false;

        STRV_FOREACH(i, b)
                nếu (!strv_contains(a, *i))
                        trả_về false;

        trả_về true;
}

void strv_print_full(char * const *l, const char *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

số_nguyên strv_extendf_with_size(char ***l, size_t *n, const char *format, ...) {
        va_list ap;
        char *x;
        số_nguyên r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về -ENOMEM;

        trả_về strv_consume_with_size(l, n, x);
}

số_nguyên strv_extend_joined_with_size_sentinel(char ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        char *x = strextendv_with_separator(/* x= */ NULL, /* separator= */ NULL, ap);
        va_end(ap);
        nếu (!x)
                trả_về -ENOMEM;

        trả_về strv_consume_with_size(l, n, x);
}

char* startswith_strv_internal(const char *s, char * const *l) {
        STRV_FOREACH(i, l) {
                char *found = (char*) startswith(s, *i);
                nếu (found)
                        trả_về found;
        }

        trả_về NULL;
}

char* endswith_strv_internal(const char *s, char * const *l) {
        STRV_FOREACH(i, l) {
                char *found = (char*) endswith(s, *i);
                nếu (found)
                        trả_về found;
        }

        trả_về NULL;
}

char** strv_reverse(char **l) {
        size_t n;

        n = strv_length(l);
        nếu (n <= 1)
                trả_về l;

        cho (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        trả_về l;
}

char** strv_shell_escape(char **l, const char *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                char *v;

                v = shell_escape(*s, bad);
                nếu (!v)
                        trả_về NULL;

                free_and_replace(*s, v);
        }

        trả_về l;
}

bool strv_fnmatch_full(
                char* const* patterns,
                const char *s,
                số_nguyên flags,
                size_t *ret_matched_pos) {

        assert(s);

        nếu (patterns)
                cho (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        nếu (fnmatch(patterns[i], s, flags) == 0) {
                                nếu (ret_matched_pos)
                                        *ret_matched_pos = i;
                                trả_về true;
                        }

        nếu (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        trả_về false;
}

char** strv_skip(char **l, size_t n) {
        trong_khi (n > 0) {
                nếu (strv_isempty(l))
                        trả_về NULL;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        nếu (strv_isempty(l))
                trả_về NULL;
        trả_về l;
}

số_nguyên strv_extend_n(char ***l, const char *value, size_t n) {
        size_t i, k;
        char **nl;

        assert(l);

        nếu (!value)
                trả_về 0;
        nếu (n == 0)
                trả_về 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        nếu (n >= SIZE_MAX - k)
                trả_về -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), sizeof(char *));
        nếu (!nl)
                trả_về -ENOMEM;

        *l = nl;

        cho (i = k; i < k + n; i++) {
                nl[i] = strdup(value);
                nếu (!nl[i])
                        goto rollback;
        }
        nl[i] = NULL;

        trả_về 0;

rollback:
        cho (size_t j = k; j < i; j++)
                free(nl[j]);
        nl[k] = NULL;

        trả_về -ENOMEM;
}

số_nguyên strv_extend_assignment(char ***l, const char *lhs, const char *rhs) {
        char *j;

        assert(l);
        assert(lhs);

        nếu (!rhs) /* value is optional, in which case we suppress the field */
                trả_về 0;

        j = strjoin(lhs, "=", rhs);
        nếu (!j)
                trả_về -ENOMEM;

        trả_về strv_consume(l, j);
}

số_nguyên fputstrv(FILE *f, char * const *l, const char *separator, bool *space) {
        bool b = false;
        số_nguyên r;

        assert(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        nếu (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                nếu (r < 0)
                        trả_về r;
        }

        trả_về 0;
}

void string_strv_hashmap_remove(Hashmap *h, const char *key, const char *value) {
        assert(key);

        nếu (value) {
                char **l = hashmap_get(h, key);
                nếu (!l)
                        trả_về;

                strv_remove(l, value);
                nếu (!strv_isempty(l))
                        trả_về;
        }

        _unused_ _cleanup_free_ char *key_free = NULL;
        strv_free(hashmap_remove2(h, key, (void**) &key_free));
}

void string_strv_ordered_hashmap_remove(OrderedHashmap *h, const char *key, const char *value) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), key, value);
}

static số_nguyên string_strv_hashmap_put_internal(Hashmap *h, const char *key, const char *value) {
        char **l;
        số_nguyên r;

        assert(h);
        assert(key);
        assert(value);

        l = hashmap_get(h, key);
        nếu (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                nếu (strv_contains(l, value))
                        trả_về 0;

                r = strv_extend(&l, value);
                nếu (r < 0)
                        trả_về r;

                assert_se(hashmap_update(h, key, l) >= 0);
        } không_thì {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ char **l2 = NULL;
                _cleanup_free_ char *t = NULL;

                t = strdup(key);
                nếu (!t)
                        trả_về -ENOMEM;

                r = strv_extend(&l2, value);
                nếu (r < 0)
                        trả_về r;

                r = hashmap_put(h, t, l2);
                nếu (r < 0)
                        trả_về r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        trả_về 1;
}

số_nguyên string_strv_hashmap_put(Hashmap **h, const char *key, const char *value) {
        số_nguyên r;

        assert(h);
        assert(key);
        assert(value);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        nếu (r < 0)
                trả_về r;

        trả_về string_strv_hashmap_put_internal(*h, key, value);
}

số_nguyên string_strv_ordered_hashmap_put(OrderedHashmap **h, const char *key, const char *value) {
        số_nguyên r;

        assert(h);
        assert(key);
        assert(value);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        nếu (r < 0)
                trả_về r;

        trả_về string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), key, value);
}

static số_nguyên rebreak_flush_line(
                char ***broken,
                const char *start,
                const char *end,
                bool in_prefix,
                const char *whitespace_begin,
                const char *whitespace_end) {

        assert(broken);
        assert(start);
        assert(end >= start);

        nếu (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                trả_về strv_extend(broken, "");

        nếu (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ char *truncated = strndup(start, end - start);
        nếu (!truncated)
                trả_về -ENOMEM;

        trả_về strv_consume(broken, TAKE_PTR(truncated));
}

số_nguyên strv_rebreak_lines(char **l, size_t width, char ***ret) {
        _cleanup_strv_free_ char **broken = NULL;
        số_nguyên r;

        assert(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        nếu (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                nếu (!broken)
                        trả_về -ENOMEM;

                *ret = TAKE_PTR(broken);
                trả_về 0;
        }

        STRV_FOREACH(i, l) {
                const char *start = *i, *whitespace_begin = NULL, *whitespace_end = NULL;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                cho (const char *p = start; *p != 0; ) {
                        nếu (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_line(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                nếu (r < 0)
                                        trả_về r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        } không_thì nếu (strchr(WHITESPACE, *p)) {
                                nếu (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = NULL;
                                }
                        } không_thì {
                                nếu (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        số_nguyên cw = utf8_char_console_width(p);
                        nếu (cw < 0)
                                trả_về log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        nếu (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ char *truncated = NULL;

                                truncated = strndup(start, whitespace_begin - start);
                                nếu (!truncated)
                                        trả_về -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                nếu (r < 0)
                                        trả_về r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                assert(start);
                nếu (after_newline && in_prefix)
                        continue;

                r = rebreak_flush_line(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                nếu (r < 0)
                        trả_về r;
        }

        *ret = TAKE_PTR(broken);
        trả_về 0;
}

char** strv_filter_prefix(char * const *l, const char *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        nếu (isempty(prefix))
                trả_về strv_copy(l);

        _cleanup_strv_free_ char **f = NULL;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                nếu (!startswith(*i, prefix))
                        continue;

                nếu (strv_extend_with_size(&f, &sz, *i) < 0)
                        trả_về NULL;
        }

        trả_về TAKE_PTR(f);
}

const char* const strv_empty[] = { NULL };
