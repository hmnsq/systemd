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
                如果 (streq(*i, name))
                        返回 *i;

        返回 NULL;
}

char* strv_find_case(char * const *l, const char *name) {
        assert(name);

        STRV_FOREACH(i, l)
                如果 (strcaseeq(*i, name))
                        返回 *i;

        返回 NULL;
}

char* strv_find_prefix(char * const *l, const char *name) {
        assert(name);

        STRV_FOREACH(i, l)
                如果 (startswith(*i, name))
                        返回 *i;

        返回 NULL;
}

char* strv_find_startswith(char * const *l, const char *name) {
        assert(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                char *e;

                e = startswith(*i, name);
                如果 (e)
                        返回 e;
        }

        返回 NULL;
}

static char* strv_find_closest_prefix(char * const *l, const char *name) {
        size_t best_distance = SIZE_MAX;
        char *best = NULL;

        assert(name);

        STRV_FOREACH(s, l) {
                char *e = startswith(*s, name);
                如果 (!e)
                        continue;

                size_t n = strlen(e);
                如果 (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        返回 best;
}

static char* strv_find_closest_by_levenshtein(char * const *l, const char *name) {
        ssize_t best_distance = SSIZE_MAX;
        char *best = NULL;

        assert(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                如果 (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        返回 NULL;
                }

                如果 (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        continue;

                如果 (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        返回 best;
}

char* strv_find_closest(char * const *l, const char *name) {
        assert(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        char *found = strv_find_closest_prefix(l, name);
        如果 (found)
                返回 found;

        返回 strv_find_closest_by_levenshtein(l, name);
}

char* strv_find_first_field(char * const *needles, char * const *haystack) {
        STRV_FOREACH(k, needles) {
                char *value = strv_env_pairs_get((char **)haystack, *k);
                如果 (value)
                        返回 value;
        }

        返回 NULL;
}

char** strv_free(char **l) {
        STRV_FOREACH(k, l)
                free(*k);

        返回 mfree(l);
}

char** strv_free_erase(char **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        返回 mfree(l);
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
        如果 (!result)
                返回 NULL;

        k = result;
        STRV_FOREACH(i, l) {
                如果 (n == 0)
                        break;

                *k = strdup(*i);
                如果 (!*k)
                        返回 NULL;
                k++;

                如果 (n != SIZE_MAX)
                        n--;
        }

        *k = NULL;
        返回 TAKE_PTR(result);
}

整数 strv_copy_unless_empty(char * const *l, char ***ret) {
        assert(ret);

        如果 (strv_isempty(l)) {
                *ret = NULL;
                返回 0;
        }

        char **copy = strv_copy(l);
        如果 (!copy)
                返回 -ENOMEM;

        *ret = TAKE_PTR(copy);
        返回 1;
}

size_t strv_length(char * const *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        返回 n;
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
        为 (const char *s = x; s; s = va_arg(aq, const char*)) {
                如果 (s == STRV_IGNORE)
                        continue;

                n++;
        }
        va_end(aq);

        a = new(char*, n+1);
        如果 (!a)
                返回 NULL;

        为 (const char *s = x; s; s = va_arg(ap, const char*)) {
                如果 (s == STRV_IGNORE)
                        continue;

                a[i] = strdup(s);
                如果 (!a[i])
                        返回 NULL;

                i++;
        }

        a[i] = NULL;

        返回 TAKE_PTR(a);
}

char** strv_new_internal(const char *x, ...) {
        char **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        返回 r;
}

整数 strv_extend_strv(char ***a, char * const *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        assert(a);

        q = strv_length(b);
        如果 (q == 0)
                返回 0;

        p = strv_length(*a);
        如果 (p >= SIZE_MAX - q)
                返回 -ENOMEM;

        char **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), sizeof(char *));
        如果 (!t)
                返回 -ENOMEM;

        t[p] = NULL;
        *a = t;

        STRV_FOREACH(s, b) {
                如果 (filter_duplicates && strv_contains(t, *s))
                        continue;

                t[p+i] = strdup(*s);
                如果 (!t[p+i])
                        goto rollback;

                i++;
                t[p+i] = NULL;
        }

        assert(i <= q);

        返回 (整数) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = NULL;
        返回 -ENOMEM;
}

整数 strv_extend_strv_consume(char ***a, char **b, bool filter_duplicates) {
        _cleanup_strv_free_ char **b_consume = b;
        size_t p, q, i;

        assert(a);

        q = strv_length(b);
        如果 (q == 0)
                返回 0;

        p = strv_length(*a);
        如果 (p == 0) {
                strv_free_and_replace(*a, b_consume);

                如果 (filter_duplicates)
                        strv_uniq(*a);

                返回 strv_length(*a);
        }

        如果 (p >= SIZE_MAX - q)
                返回 -ENOMEM;

        char **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), sizeof(char *));
        如果 (!t)
                返回 -ENOMEM;

        t[p] = NULL;
        *a = t;

        如果 (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = NULL;
                i = q;
        } 否则 {
                i = 0;

                STRV_FOREACH(s, b) {
                        如果 (strv_contains(t, *s)) {
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

        返回 (整数) i;
}

整数 strv_extend_strv_biconcat(char ***a, const char *prefix, const char* const *b, const char *suffix) {
        整数 r;

        assert(a);

        STRV_FOREACH(s, b) {
                char *v;

                v = strjoin(strempty(prefix), *s, suffix);
                如果 (!v)
                        返回 -ENOMEM;

                r = strv_consume(a, v);
                如果 (r < 0)
                        返回 r;
        }

        返回 0;
}

整数 strv_split_newlines_full(char ***ret, const char *s, ExtractFlags flags) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n;
        整数 r;

        assert(ret);
        assert(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        如果 (r < 0)
                返回 r;

        n = strv_length(l);
        如果 (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        返回 n;
}

char** strv_split_newlines(const char *s) {
        char **ret;

        如果 (strv_split_newlines_full(&ret, s, 0) < 0)
                返回 NULL;

        返回 ret;
}

整数 strv_split_full(char ***t, const char *s, const char *separators, ExtractFlags flags) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n = 0;
        整数 r;

        assert(t);
        assert(s);

        为 (;;) {
                _cleanup_free_ char *word = NULL;

                r = extract_first_word(&s, &word, separators, flags);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                如果 (!GREEDY_REALLOC(l, n + 2))
                        返回 -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = NULL;
        }

        如果 (!l) {
                l = new0(char*, 1);
                如果 (!l)
                        返回 -ENOMEM;
        }

        *t = TAKE_PTR(l);

        返回 (整数) n;
}

char** strv_split(const char *s, const char *separators) {
        char **ret;

        如果 (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                返回 NULL;

        返回 ret;
}

整数 strv_split_and_extend_full(char ***t, const char *s, const char *separators, bool filter_duplicates, ExtractFlags flags) {
        char **l;
        整数 r;

        assert(t);
        assert(s);

        r = strv_split_full(&l, s, separators, flags);
        如果 (r < 0)
                返回 r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        如果 (r < 0)
                返回 r;

        返回 (整数) strv_length(*t);
}

整数 strv_split_and_extend(char ***t, const char *s, const char *separators, bool filter_duplicates) {
        返回 strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

整数 strv_split_colon_pairs(char ***t, const char *s) {
        _cleanup_strv_free_ char **l = NULL;
        size_t n = 0;
        整数 r;

        assert(t);
        assert(s);

        为 (;;) {
                _cleanup_free_ char *first = NULL, *second = NULL, *tuple = NULL, *second_or_empty = NULL;

                r = extract_first_word(&s, &tuple, NULL, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                const char *p = tuple;
                r = extract_many_words(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        continue;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                如果 (!isempty(p))
                        返回 -EINVAL;

                second_or_empty = strdup(strempty(second));
                如果 (!second_or_empty)
                        返回 -ENOMEM;

                如果 (!GREEDY_REALLOC(l, n + 3))
                        返回 -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = NULL;
        }

        如果 (!l) {
                l = new0(char*, 1);
                如果 (!l)
                        返回 -ENOMEM;
        }

        *t = TAKE_PTR(l);

        返回 (整数) n;
}

char* strv_join_full(char * const *l, const char *separator, const char *prefix) {
        char *r, *e;
        size_t n, k, m;

        如果 (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                如果 (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(char, n+1);
        如果 (!r)
                返回 NULL;

        e = r;
        STRV_FOREACH(s, l) {
                如果 (s != l)
                        e = stpcpy(e, separator);

                如果 (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        返回 r;
}

整数 strv_push_with_size(char ***l, size_t *n, char *value) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        assert(l);
        POINTER_MAY_BE_NULL(n);

        如果 (!value)
                返回 0;

        size_t size = n ? *n : SIZE_MAX;
        如果 (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        如果 (size > SIZE_MAX-2)
                返回 -ENOMEM;

        char **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), sizeof(char*));
        如果 (!c)
                返回 -ENOMEM;

        c[size] = value;
        c[size+1] = NULL;

        *l = c;
        如果 (n)
                *n = size + 1;
        返回 0;
}

整数 strv_push_pair(char ***l, char *a, char *b) {
        char **c;
        size_t n;

        assert(l);

        如果 (!a && !b)
                返回 0;

        n = strv_length(*l);

        /* Check for overflow */
        如果 (n > SIZE_MAX-3)
                返回 -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), sizeof(char*));
        如果 (!c)
                返回 -ENOMEM;

        如果 (a)
                c[n++] = a;
        如果 (b)
                c[n++] = b;
        c[n] = NULL;

        *l = c;
        返回 0;
}

整数 strv_insert(char ***l, size_t position, char *value) {
        char **c;
        size_t n, m;

        assert(l);

        如果 (!value)
                返回 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        如果 (n > SIZE_MAX - 2)
                返回 -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), sizeof(char*));
        如果 (!c)
                返回 -ENOMEM;

        如果 (n > position)
                memmove(c + position + 1, c + position, (n - position) * sizeof(char*));

        c[position] = value;
        c[n + 1] = NULL;

        *l = c;
        返回 0;
}

整数 strv_consume_with_size(char ***l, size_t *n, char *value) {
        整数 r;

        r = strv_push_with_size(l, n, value);
        如果 (r < 0)
                free(value);

        返回 r;
}

整数 strv_consume_pair(char ***l, char *a, char *b) {
        整数 r;

        r = strv_push_pair(l, a, b);
        如果 (r < 0) {
                free(a);
                free(b);
        }

        返回 r;
}

整数 strv_consume_prepend(char ***l, char *value) {
        整数 r;

        r = strv_push_prepend(l, value);
        如果 (r < 0)
                free(value);

        返回 r;
}

整数 strv_prepend(char ***l, const char *value) {
        char *v;

        如果 (!value)
                返回 0;

        v = strdup(value);
        如果 (!v)
                返回 -ENOMEM;

        返回 strv_consume_prepend(l, v);
}

整数 strv_extend_with_size(char ***l, size_t *n, const char *value) {
        char *v;

        如果 (!value)
                返回 0;

        v = strdup(value);
        如果 (!v)
                返回 -ENOMEM;

        返回 strv_consume_with_size(l, n, v);
}

整数 strv_extend_many_internal(char ***l, const char *value, ...) {
        va_list ap;
        size_t n, m;
        整数 r;

        assert(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, value);
        为 (const char *s = value; s != POINTER_MAX; s = va_arg(ap, const char*)) {
                如果 (!s)
                        continue;

                如果 (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        break;
                }
                m++;
        }
        va_end(ap);

        如果 (r < 0)
                返回 r;
        如果 (m > SIZE_MAX-1)
                返回 -ENOMEM;

        char **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), sizeof(char*));
        如果 (!c)
                返回 -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, value);
        为 (const char *s = value; s != POINTER_MAX; s = va_arg(ap, const char*)) {
                如果 (!s)
                        continue;

                c[i] = strdup(s);
                如果 (!c[i]) {
                        r = -ENOMEM;
                        break;
                }
                i++;
        }
        va_end(ap);

        如果 (r < 0) {
                /* rollback on error */
                为 (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                返回 r;
        }

        c[i] = NULL;
        返回 0;
}

char** strv_uniq(char **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        返回 l;
}

bool strv_is_uniq(char * const *l) {
        STRV_FOREACH(i, l)
                如果 (strv_contains(i+1, *i))
                        返回 false;

        返回 true;
}

char** strv_remove(char **l, const char *s) {
        如果 (!l)
                返回 NULL;

        assert(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        char **f, **t;
        为 (f = t = l; *f; f++)
                如果 (streq(*f, s))
                        free(*f);
                否则
                        *(t++) = *f;

        *t = NULL;
        返回 l;
}

char** strv_remove_strv(char **l, char *const*ll) {

        如果 (strv_isempty(l))
                返回 l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        返回 l;
}

bool strv_overlap(char * const *a, char * const *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                如果 (strv_contains(b, *i))
                        返回 true;

        返回 false;
}

static 整数 str_compare(char * const *a, char * const *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        返回 strcmp(*a, *b);
}

char** strv_sort(char **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        返回 l;
}

char** strv_sort_uniq_full(char **l, 整数 (*compare)(char * const *a, char * const *b)) {
        assert(compare);

        如果 (strv_isempty(l))
                返回 l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        char **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                如果 (compare(i, tail - 1) == 0)
                        free(*i);
                否则
                        *(tail++) = *i;

        *tail = NULL;
        返回 l;
}

char** strv_sort_uniq(char **l) {
        返回 strv_sort_uniq_full(l, str_compare);
}

整数 strv_compare(char * const *a, char * const *b) {
        整数 r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        如果 (strv_isempty(a)) {
                如果 (strv_isempty(b))
                        返回 0;
                否则
                        返回 -1;
        }

        如果 (strv_isempty(b))
                返回 1;

        为 ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                如果 (r != 0)
                        返回 r;
        }

        返回 0;
}

bool strv_equal_ignore_order(char * const *a, char * const *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        如果 (a == b)
                返回 true;

        STRV_FOREACH(i, a)
                如果 (!strv_contains(b, *i))
                        返回 false;

        STRV_FOREACH(i, b)
                如果 (!strv_contains(a, *i))
                        返回 false;

        返回 true;
}

void strv_print_full(char * const *l, const char *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

整数 strv_extendf_with_size(char ***l, size_t *n, const char *format, ...) {
        va_list ap;
        char *x;
        整数 r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        如果 (r < 0)
                返回 -ENOMEM;

        返回 strv_consume_with_size(l, n, x);
}

整数 strv_extend_joined_with_size_sentinel(char ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        char *x = strextendv_with_separator(/* x= */ NULL, /* separator= */ NULL, ap);
        va_end(ap);
        如果 (!x)
                返回 -ENOMEM;

        返回 strv_consume_with_size(l, n, x);
}

char* startswith_strv_internal(const char *s, char * const *l) {
        STRV_FOREACH(i, l) {
                char *found = (char*) startswith(s, *i);
                如果 (found)
                        返回 found;
        }

        返回 NULL;
}

char* endswith_strv_internal(const char *s, char * const *l) {
        STRV_FOREACH(i, l) {
                char *found = (char*) endswith(s, *i);
                如果 (found)
                        返回 found;
        }

        返回 NULL;
}

char** strv_reverse(char **l) {
        size_t n;

        n = strv_length(l);
        如果 (n <= 1)
                返回 l;

        为 (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        返回 l;
}

char** strv_shell_escape(char **l, const char *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                char *v;

                v = shell_escape(*s, bad);
                如果 (!v)
                        返回 NULL;

                free_and_replace(*s, v);
        }

        返回 l;
}

bool strv_fnmatch_full(
                char* const* patterns,
                const char *s,
                整数 flags,
                size_t *ret_matched_pos) {

        assert(s);

        如果 (patterns)
                为 (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        如果 (fnmatch(patterns[i], s, flags) == 0) {
                                如果 (ret_matched_pos)
                                        *ret_matched_pos = i;
                                返回 true;
                        }

        如果 (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        返回 false;
}

char** strv_skip(char **l, size_t n) {
        当 (n > 0) {
                如果 (strv_isempty(l))
                        返回 NULL;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        如果 (strv_isempty(l))
                返回 NULL;
        返回 l;
}

整数 strv_extend_n(char ***l, const char *value, size_t n) {
        size_t i, k;
        char **nl;

        assert(l);

        如果 (!value)
                返回 0;
        如果 (n == 0)
                返回 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        如果 (n >= SIZE_MAX - k)
                返回 -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), sizeof(char *));
        如果 (!nl)
                返回 -ENOMEM;

        *l = nl;

        为 (i = k; i < k + n; i++) {
                nl[i] = strdup(value);
                如果 (!nl[i])
                        goto rollback;
        }
        nl[i] = NULL;

        返回 0;

rollback:
        为 (size_t j = k; j < i; j++)
                free(nl[j]);
        nl[k] = NULL;

        返回 -ENOMEM;
}

整数 strv_extend_assignment(char ***l, const char *lhs, const char *rhs) {
        char *j;

        assert(l);
        assert(lhs);

        如果 (!rhs) /* value is optional, in which case we suppress the field */
                返回 0;

        j = strjoin(lhs, "=", rhs);
        如果 (!j)
                返回 -ENOMEM;

        返回 strv_consume(l, j);
}

整数 fputstrv(FILE *f, char * const *l, const char *separator, bool *space) {
        bool b = false;
        整数 r;

        assert(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        如果 (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                如果 (r < 0)
                        返回 r;
        }

        返回 0;
}

void string_strv_hashmap_remove(Hashmap *h, const char *key, const char *value) {
        assert(key);

        如果 (value) {
                char **l = hashmap_get(h, key);
                如果 (!l)
                        返回;

                strv_remove(l, value);
                如果 (!strv_isempty(l))
                        返回;
        }

        _unused_ _cleanup_free_ char *key_free = NULL;
        strv_free(hashmap_remove2(h, key, (void**) &key_free));
}

void string_strv_ordered_hashmap_remove(OrderedHashmap *h, const char *key, const char *value) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), key, value);
}

static 整数 string_strv_hashmap_put_internal(Hashmap *h, const char *key, const char *value) {
        char **l;
        整数 r;

        assert(h);
        assert(key);
        assert(value);

        l = hashmap_get(h, key);
        如果 (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                如果 (strv_contains(l, value))
                        返回 0;

                r = strv_extend(&l, value);
                如果 (r < 0)
                        返回 r;

                assert_se(hashmap_update(h, key, l) >= 0);
        } 否则 {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ char **l2 = NULL;
                _cleanup_free_ char *t = NULL;

                t = strdup(key);
                如果 (!t)
                        返回 -ENOMEM;

                r = strv_extend(&l2, value);
                如果 (r < 0)
                        返回 r;

                r = hashmap_put(h, t, l2);
                如果 (r < 0)
                        返回 r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        返回 1;
}

整数 string_strv_hashmap_put(Hashmap **h, const char *key, const char *value) {
        整数 r;

        assert(h);
        assert(key);
        assert(value);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        如果 (r < 0)
                返回 r;

        返回 string_strv_hashmap_put_internal(*h, key, value);
}

整数 string_strv_ordered_hashmap_put(OrderedHashmap **h, const char *key, const char *value) {
        整数 r;

        assert(h);
        assert(key);
        assert(value);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        如果 (r < 0)
                返回 r;

        返回 string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), key, value);
}

static 整数 rebreak_flush_line(
                char ***broken,
                const char *start,
                const char *end,
                bool in_prefix,
                const char *whitespace_begin,
                const char *whitespace_end) {

        assert(broken);
        assert(start);
        assert(end >= start);

        如果 (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                返回 strv_extend(broken, "");

        如果 (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ char *truncated = strndup(start, end - start);
        如果 (!truncated)
                返回 -ENOMEM;

        返回 strv_consume(broken, TAKE_PTR(truncated));
}

整数 strv_rebreak_lines(char **l, size_t width, char ***ret) {
        _cleanup_strv_free_ char **broken = NULL;
        整数 r;

        assert(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        如果 (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                如果 (!broken)
                        返回 -ENOMEM;

                *ret = TAKE_PTR(broken);
                返回 0;
        }

        STRV_FOREACH(i, l) {
                const char *start = *i, *whitespace_begin = NULL, *whitespace_end = NULL;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                为 (const char *p = start; *p != 0; ) {
                        如果 (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_line(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                如果 (r < 0)
                                        返回 r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = NULL;
                                w = 0;
                                continue;
                        } 否则 如果 (strchr(WHITESPACE, *p)) {
                                如果 (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = NULL;
                                }
                        } 否则 {
                                如果 (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        整数 cw = utf8_char_console_width(p);
                        如果 (cw < 0)
                                返回 log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        如果 (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ char *truncated = NULL;

                                truncated = strndup(start, whitespace_begin - start);
                                如果 (!truncated)
                                        返回 -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                如果 (r < 0)
                                        返回 r;

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
                如果 (after_newline && in_prefix)
                        continue;

                r = rebreak_flush_line(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                如果 (r < 0)
                        返回 r;
        }

        *ret = TAKE_PTR(broken);
        返回 0;
}

char** strv_filter_prefix(char * const *l, const char *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        如果 (isempty(prefix))
                返回 strv_copy(l);

        _cleanup_strv_free_ char **f = NULL;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                如果 (!startswith(*i, prefix))
                        continue;

                如果 (strv_extend_with_size(&f, &sz, *i) < 0)
                        返回 NULL;
        }

        返回 TAKE_PTR(f);
}

const char* const strv_empty[] = { NULL };
