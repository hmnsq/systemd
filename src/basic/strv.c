/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fnmatch.h>
#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "gunicode.h"
#அந்தர்பாவ "hashmap.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "sort-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "utf8.h"

எழுத்து* strv_find(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH(i, l)
                எனில் (streq(*i, name))
                        திரும்பு *i;

        திரும்பு வெற்று;
}

எழுத்து* strv_find_case(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH(i, l)
                எனில் (strcaseeq(*i, name))
                        திரும்பு *i;

        திரும்பு வெற்று;
}

எழுத்து* strv_find_prefix(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH(i, l)
                எனில் (startswith(*i, name))
                        திரும்பு *i;

        திரும்பு வெற்று;
}

எழுத்து* strv_find_startswith(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ப்ரதிஜ்ஞா(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                எழுத்து *e;

                e = startswith(*i, name);
                எனில் (e)
                        திரும்பு e;
        }

        திரும்பு வெற்று;
}

நிலையான எழுத்து* strv_find_closest_prefix(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        size_t best_distance = SIZE_MAX;
        எழுத்து *best = வெற்று;

        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH(s, l) {
                எழுத்து *e = startswith(*s, name);
                எனில் (!e)
                        தொடர்;

                size_t n = strlen(e);
                எனில் (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        திரும்பு best;
}

நிலையான எழுத்து* strv_find_closest_by_levenshtein(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ssize_t best_distance = SSIZE_MAX;
        எழுத்து *best = வெற்று;

        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                எனில் (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        திரும்பு வெற்று;
                }

                எனில் (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        தொடர்;

                எனில் (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        திரும்பு best;
}

எழுத்து* strv_find_closest(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name) {
        ப்ரதிஜ்ஞா(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        எழுத்து *found = strv_find_closest_prefix(l, name);
        எனில் (found)
                திரும்பு found;

        திரும்பு strv_find_closest_by_levenshtein(l, name);
}

எழுத்து* strv_find_first_field(எழுத்து * மாறிலி *needles, எழுத்து * மாறிலி *haystack) {
        STRV_FOREACH(k, needles) {
                எழுத்து *மதிப்பு = strv_env_pairs_get((எழுத்து **)haystack, *k);
                எனில் (மதிப்பு)
                        திரும்பு மதிப்பு;
        }

        திரும்பு வெற்று;
}

எழுத்து** strv_free(எழுத்து **l) {
        STRV_FOREACH(k, l)
                விடுவி(*k);

        திரும்பு mfree(l);
}

எழுத்து** strv_free_erase(எழுத்து **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        திரும்பு mfree(l);
}

வெறுமை strv_free_many(எழுத்து ***strvs, size_t n) {
        ப்ரதிஜ்ஞா(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        விடுவி(strvs);
}

எழுத்து** strv_copy_n(எழுத்து * மாறிலி *l, size_t n) {
        _cleanup_strv_free_ எழுத்து **result = வெற்று;
        எழுத்து **k;

        result = new(எழுத்து*, MIN(strv_length(l), n) + 1);
        எனில் (!result)
                திரும்பு வெற்று;

        k = result;
        STRV_FOREACH(i, l) {
                எனில் (n == 0)
                        நிறுத்து;

                *k = strdup(*i);
                எனில் (!*k)
                        திரும்பு வெற்று;
                k++;

                எனில் (n != SIZE_MAX)
                        n--;
        }

        *k = வெற்று;
        திரும்பு TAKE_PTR(result);
}

முழு strv_copy_unless_empty(எழுத்து * மாறிலி *l, எழுத்து ***ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (strv_isempty(l)) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        எழுத்து **copy = strv_copy(l);
        எனில் (!copy)
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(copy);
        திரும்பு 1;
}

size_t strv_length(எழுத்து * மாறிலி *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        திரும்பு n;
}

எழுத்து** strv_new_ap(மாறிலி எழுத்து *x, va_list ap) {
        _cleanup_strv_free_ எழுத்து **a = வெற்று;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        க்கு (மாறிலி எழுத்து *s = x; s; s = va_arg(aq, மாறிலி எழுத்து*)) {
                எனில் (s == STRV_IGNORE)
                        தொடர்;

                n++;
        }
        va_end(aq);

        a = new(எழுத்து*, n+1);
        எனில் (!a)
                திரும்பு வெற்று;

        க்கு (மாறிலி எழுத்து *s = x; s; s = va_arg(ap, மாறிலி எழுத்து*)) {
                எனில் (s == STRV_IGNORE)
                        தொடர்;

                a[i] = strdup(s);
                எனில் (!a[i])
                        திரும்பு வெற்று;

                i++;
        }

        a[i] = வெற்று;

        திரும்பு TAKE_PTR(a);
}

எழுத்து** strv_new_internal(மாறிலி எழுத்து *x, ...) {
        எழுத்து **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        திரும்பு r;
}

முழு strv_extend_strv(எழுத்து ***a, எழுத்து * மாறிலி *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ப்ரதிஜ்ஞா(a);

        q = strv_length(b);
        எனில் (q == 0)
                திரும்பு 0;

        p = strv_length(*a);
        எனில் (p >= SIZE_MAX - q)
                திரும்பு -ENOMEM;

        எழுத்து **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), அளவு(எழுத்து *));
        எனில் (!t)
                திரும்பு -ENOMEM;

        t[p] = வெற்று;
        *a = t;

        STRV_FOREACH(s, b) {
                எனில் (filter_duplicates && strv_contains(t, *s))
                        தொடர்;

                t[p+i] = strdup(*s);
                எனில் (!t[p+i])
                        செல் rollback;

                i++;
                t[p+i] = வெற்று;
        }

        ப்ரதிஜ்ஞா(i <= q);

        திரும்பு (முழு) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = வெற்று;
        திரும்பு -ENOMEM;
}

முழு strv_extend_strv_consume(எழுத்து ***a, எழுத்து **b, bool filter_duplicates) {
        _cleanup_strv_free_ எழுத்து **b_consume = b;
        size_t p, q, i;

        ப்ரதிஜ்ஞா(a);

        q = strv_length(b);
        எனில் (q == 0)
                திரும்பு 0;

        p = strv_length(*a);
        எனில் (p == 0) {
                strv_free_and_replace(*a, b_consume);

                எனில் (filter_duplicates)
                        strv_uniq(*a);

                திரும்பு strv_length(*a);
        }

        எனில் (p >= SIZE_MAX - q)
                திரும்பு -ENOMEM;

        எழுத்து **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), அளவு(எழுத்து *));
        எனில் (!t)
                திரும்பு -ENOMEM;

        t[p] = வெற்று;
        *a = t;

        எனில் (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = வெற்று;
                i = q;
        } இல்லையெனில் {
                i = 0;

                STRV_FOREACH(s, b) {
                        எனில் (strv_contains(t, *s)) {
                                விடுவி(*s);
                                தொடர்;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = வெற்று;
                }
        }

        ப்ரதிஜ்ஞா(i <= q);

        b_consume = mfree(b_consume);

        திரும்பு (முழு) i;
}

முழு strv_extend_strv_biconcat(எழுத்து ***a, மாறிலி எழுத்து *prefix, மாறிலி எழுத்து* மாறிலி *b, மாறிலி எழுத்து *suffix) {
        முழு r;

        ப்ரதிஜ்ஞா(a);

        STRV_FOREACH(s, b) {
                எழுத்து *v;

                v = strjoin(strempty(prefix), *s, suffix);
                எனில் (!v)
                        திரும்பு -ENOMEM;

                r = strv_consume(a, v);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு strv_split_newlines_full(எழுத்து ***ret, மாறிலி எழுத்து *s, ExtractFlags flags) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        size_t n;
        முழு r;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        எனில் (r < 0)
                திரும்பு r;

        n = strv_length(l);
        எனில் (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        திரும்பு n;
}

எழுத்து** strv_split_newlines(மாறிலி எழுத்து *s) {
        எழுத்து **ret;

        எனில் (strv_split_newlines_full(&ret, s, 0) < 0)
                திரும்பு வெற்று;

        திரும்பு ret;
}

முழு strv_split_full(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, ExtractFlags flags) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        size_t n = 0;
        முழு r;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(s);

        க்கு (;;) {
                _cleanup_free_ எழுத்து *word = வெற்று;

                r = extract_first_word(&s, &word, separators, flags);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (!GREEDY_REALLOC(l, n + 2))
                        திரும்பு -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = வெற்று;
        }

        எனில் (!l) {
                l = new0(எழுத்து*, 1);
                எனில் (!l)
                        திரும்பு -ENOMEM;
        }

        *t = TAKE_PTR(l);

        திரும்பு (முழு) n;
}

எழுத்து** strv_split(மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators) {
        எழுத்து **ret;

        எனில் (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                திரும்பு வெற்று;

        திரும்பு ret;
}

முழு strv_split_and_extend_full(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, bool filter_duplicates, ExtractFlags flags) {
        எழுத்து **l;
        முழு r;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(s);

        r = strv_split_full(&l, s, separators, flags);
        எனில் (r < 0)
                திரும்பு r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு (முழு) strv_length(*t);
}

முழு strv_split_and_extend(எழுத்து ***t, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separators, bool filter_duplicates) {
        திரும்பு strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

முழு strv_split_colon_pairs(எழுத்து ***t, மாறிலி எழுத்து *s) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        size_t n = 0;
        முழு r;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(s);

        க்கு (;;) {
                _cleanup_free_ எழுத்து *first = வெற்று, *second = வெற்று, *tuple = வெற்று, *second_or_empty = வெற்று;

                r = extract_first_word(&s, &tuple, வெற்று, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                மாறிலி எழுத்து *p = tuple;
                r = extract_many_சொற்கள்(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        தொடர்;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                எனில் (!isempty(p))
                        திரும்பு -EINVAL;

                second_or_empty = strdup(strempty(second));
                எனில் (!second_or_empty)
                        திரும்பு -ENOMEM;

                எனில் (!GREEDY_REALLOC(l, n + 3))
                        திரும்பு -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = வெற்று;
        }

        எனில் (!l) {
                l = new0(எழுத்து*, 1);
                எனில் (!l)
                        திரும்பு -ENOMEM;
        }

        *t = TAKE_PTR(l);

        திரும்பு (முழு) n;
}

எழுத்து* strv_join_full(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *separator, மாறிலி எழுத்து *prefix) {
        எழுத்து *r, *e;
        size_t n, k, m;

        எனில் (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                எனில் (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(எழுத்து, n+1);
        எனில் (!r)
                திரும்பு வெற்று;

        e = r;
        STRV_FOREACH(s, l) {
                எனில் (s != l)
                        e = stpcpy(e, separator);

                எனில் (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        திரும்பு r;
}

முழு strv_தள்ளு_with_size(எழுத்து ***l, size_t *n, எழுத்து *மதிப்பு) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ப்ரதிஜ்ஞா(l);
        POINTER_MAY_BE_NULL(n);

        எனில் (!மதிப்பு)
                திரும்பு 0;

        size_t size = n ? *n : SIZE_MAX;
        எனில் (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        எனில் (size > SIZE_MAX-2)
                திரும்பு -ENOMEM;

        எழுத்து **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), அளவு(எழுத்து*));
        எனில் (!c)
                திரும்பு -ENOMEM;

        c[size] = மதிப்பு;
        c[size+1] = வெற்று;

        *l = c;
        எனில் (n)
                *n = size + 1;
        திரும்பு 0;
}

முழு strv_தள்ளு_pair(எழுத்து ***l, எழுத்து *a, எழுத்து *b) {
        எழுத்து **c;
        size_t n;

        ப்ரதிஜ்ஞா(l);

        எனில் (!a && !b)
                திரும்பு 0;

        n = strv_length(*l);

        /* Check for overflow */
        எனில் (n > SIZE_MAX-3)
                திரும்பு -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), அளவு(எழுத்து*));
        எனில் (!c)
                திரும்பு -ENOMEM;

        எனில் (a)
                c[n++] = a;
        எனில் (b)
                c[n++] = b;
        c[n] = வெற்று;

        *l = c;
        திரும்பு 0;
}

முழு strv_செருகு(எழுத்து ***l, size_t position, எழுத்து *மதிப்பு) {
        எழுத்து **c;
        size_t n, m;

        ப்ரதிஜ்ஞா(l);

        எனில் (!மதிப்பு)
                திரும்பு 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        எனில் (n > SIZE_MAX - 2)
                திரும்பு -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), அளவு(எழுத்து*));
        எனில் (!c)
                திரும்பு -ENOMEM;

        எனில் (n > position)
                memmove(c + position + 1, c + position, (n - position) * அளவு(எழுத்து*));

        c[position] = மதிப்பு;
        c[n + 1] = வெற்று;

        *l = c;
        திரும்பு 0;
}

முழு strv_consume_with_size(எழுத்து ***l, size_t *n, எழுத்து *மதிப்பு) {
        முழு r;

        r = strv_தள்ளு_with_size(l, n, மதிப்பு);
        எனில் (r < 0)
                விடுவி(மதிப்பு);

        திரும்பு r;
}

முழு strv_consume_pair(எழுத்து ***l, எழுத்து *a, எழுத்து *b) {
        முழு r;

        r = strv_தள்ளு_pair(l, a, b);
        எனில் (r < 0) {
                விடுவி(a);
                விடுவி(b);
        }

        திரும்பு r;
}

முழு strv_consume_prepend(எழுத்து ***l, எழுத்து *மதிப்பு) {
        முழு r;

        r = strv_தள்ளு_prepend(l, மதிப்பு);
        எனில் (r < 0)
                விடுவி(மதிப்பு);

        திரும்பு r;
}

முழு strv_prepend(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு) {
        எழுத்து *v;

        எனில் (!மதிப்பு)
                திரும்பு 0;

        v = strdup(மதிப்பு);
        எனில் (!v)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume_prepend(l, v);
}

முழு strv_extend_with_size(எழுத்து ***l, size_t *n, மாறிலி எழுத்து *மதிப்பு) {
        எழுத்து *v;

        எனில் (!மதிப்பு)
                திரும்பு 0;

        v = strdup(மதிப்பு);
        எனில் (!v)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume_with_size(l, n, v);
}

முழு strv_extend_many_internal(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு, ...) {
        va_list ap;
        size_t n, m;
        முழு r;

        ப்ரதிஜ்ஞா(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, மதிப்பு);
        க்கு (மாறிலி எழுத்து *s = மதிப்பு; s != POINTER_MAX; s = va_arg(ap, மாறிலி எழுத்து*)) {
                எனில் (!s)
                        தொடர்;

                எனில் (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        நிறுத்து;
                }
                m++;
        }
        va_end(ap);

        எனில் (r < 0)
                திரும்பு r;
        எனில் (m > SIZE_MAX-1)
                திரும்பு -ENOMEM;

        எழுத்து **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), அளவு(எழுத்து*));
        எனில் (!c)
                திரும்பு -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, மதிப்பு);
        க்கு (மாறிலி எழுத்து *s = மதிப்பு; s != POINTER_MAX; s = va_arg(ap, மாறிலி எழுத்து*)) {
                எனில் (!s)
                        தொடர்;

                c[i] = strdup(s);
                எனில் (!c[i]) {
                        r = -ENOMEM;
                        நிறுத்து;
                }
                i++;
        }
        va_end(ap);

        எனில் (r < 0) {
                /* rollback on error */
                க்கு (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                திரும்பு r;
        }

        c[i] = வெற்று;
        திரும்பு 0;
}

எழுத்து** strv_uniq(எழுத்து **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        திரும்பு l;
}

bool strv_is_uniq(எழுத்து * மாறிலி *l) {
        STRV_FOREACH(i, l)
                எனில் (strv_contains(i+1, *i))
                        திரும்பு false;

        திரும்பு true;
}

எழுத்து** strv_remove(எழுத்து **l, மாறிலி எழுத்து *s) {
        எனில் (!l)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        எழுத்து **f, **t;
        க்கு (f = t = l; *f; f++)
                எனில் (streq(*f, s))
                        விடுவி(*f);
                இல்லையெனில்
                        *(t++) = *f;

        *t = வெற்று;
        திரும்பு l;
}

எழுத்து** strv_remove_strv(எழுத்து **l, எழுத்து *மாறிலி*ll) {

        எனில் (strv_isempty(l))
                திரும்பு l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        திரும்பு l;
}

bool strv_overlap(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                எனில் (strv_contains(b, *i))
                        திரும்பு true;

        திரும்பு false;
}

நிலையான முழு str_compare(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        திரும்பு strcmp(*a, *b);
}

எழுத்து** strv_sort(எழுத்து **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        திரும்பு l;
}

எழுத்து** strv_sort_uniq_full(எழுத்து **l, முழு (*compare)(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b)) {
        ப்ரதிஜ்ஞா(compare);

        எனில் (strv_isempty(l))
                திரும்பு l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        எழுத்து **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                எனில் (compare(i, tail - 1) == 0)
                        விடுவி(*i);
                இல்லையெனில்
                        *(tail++) = *i;

        *tail = வெற்று;
        திரும்பு l;
}

எழுத்து** strv_sort_uniq(எழுத்து **l) {
        திரும்பு strv_sort_uniq_full(l, str_compare);
}

முழு strv_compare(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) {
        முழு r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        எனில் (strv_isempty(a)) {
                எனில் (strv_isempty(b))
                        திரும்பு 0;
                இல்லையெனில்
                        திரும்பு -1;
        }

        எனில் (strv_isempty(b))
                திரும்பு 1;

        க்கு ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                எனில் (r != 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

bool strv_equal_ignore_order(எழுத்து * மாறிலி *a, எழுத்து * மாறிலி *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        எனில் (a == b)
                திரும்பு true;

        STRV_FOREACH(i, a)
                எனில் (!strv_contains(b, *i))
                        திரும்பு false;

        STRV_FOREACH(i, b)
                எனில் (!strv_contains(a, *i))
                        திரும்பு false;

        திரும்பு true;
}

வெறுமை strv_print_full(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

முழு strv_extendf_with_size(எழுத்து ***l, size_t *n, மாறிலி எழுத்து *format, ...) {
        va_list ap;
        எழுத்து *x;
        முழு r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume_with_size(l, n, x);
}

முழு strv_extend_joined_with_size_sentinel(எழுத்து ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        எழுத்து *x = strextendv_with_separator(/* x= */ வெற்று, /* separator= */ வெற்று, ap);
        va_end(ap);
        எனில் (!x)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume_with_size(l, n, x);
}

எழுத்து* startswith_strv_internal(மாறிலி எழுத்து *s, எழுத்து * மாறிலி *l) {
        STRV_FOREACH(i, l) {
                எழுத்து *found = (எழுத்து*) startswith(s, *i);
                எனில் (found)
                        திரும்பு found;
        }

        திரும்பு வெற்று;
}

எழுத்து* endswith_strv_internal(மாறிலி எழுத்து *s, எழுத்து * மாறிலி *l) {
        STRV_FOREACH(i, l) {
                எழுத்து *found = (எழுத்து*) endswith(s, *i);
                எனில் (found)
                        திரும்பு found;
        }

        திரும்பு வெற்று;
}

எழுத்து** strv_reverse(எழுத்து **l) {
        size_t n;

        n = strv_length(l);
        எனில் (n <= 1)
                திரும்பு l;

        க்கு (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        திரும்பு l;
}

எழுத்து** strv_shell_escape(எழுத்து **l, மாறிலி எழுத்து *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                எழுத்து *v;

                v = shell_escape(*s, bad);
                எனில் (!v)
                        திரும்பு வெற்று;

                free_and_replace(*s, v);
        }

        திரும்பு l;
}

bool strv_fnmatch_full(
                எழுத்து* மாறிலி* patterns,
                மாறிலி எழுத்து *s,
                முழு flags,
                size_t *ret_matched_pos) {

        ப்ரதிஜ்ஞா(s);

        எனில் (patterns)
                க்கு (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        எனில் (fnmatch(patterns[i], s, flags) == 0) {
                                எனில் (ret_matched_pos)
                                        *ret_matched_pos = i;
                                திரும்பு true;
                        }

        எனில் (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        திரும்பு false;
}

எழுத்து** strv_skip(எழுத்து **l, size_t n) {
        போது (n > 0) {
                எனில் (strv_isempty(l))
                        திரும்பு வெற்று;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        எனில் (strv_isempty(l))
                திரும்பு வெற்று;
        திரும்பு l;
}

முழு strv_extend_n(எழுத்து ***l, மாறிலி எழுத்து *மதிப்பு, size_t n) {
        size_t i, k;
        எழுத்து **nl;

        ப்ரதிஜ்ஞா(l);

        எனில் (!மதிப்பு)
                திரும்பு 0;
        எனில் (n == 0)
                திரும்பு 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        எனில் (n >= SIZE_MAX - k)
                திரும்பு -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), அளவு(எழுத்து *));
        எனில் (!nl)
                திரும்பு -ENOMEM;

        *l = nl;

        க்கு (i = k; i < k + n; i++) {
                nl[i] = strdup(மதிப்பு);
                எனில் (!nl[i])
                        செல் rollback;
        }
        nl[i] = வெற்று;

        திரும்பு 0;

rollback:
        க்கு (size_t j = k; j < i; j++)
                விடுவி(nl[j]);
        nl[k] = வெற்று;

        திரும்பு -ENOMEM;
}

முழு strv_extend_assignment(எழுத்து ***l, மாறிலி எழுத்து *lhs, மாறிலி எழுத்து *rhs) {
        எழுத்து *j;

        ப்ரதிஜ்ஞா(l);
        ப்ரதிஜ்ஞா(lhs);

        எனில் (!rhs) /* value is optional, in which case we suppress the field */
                திரும்பு 0;

        j = strjoin(lhs, "=", rhs);
        எனில் (!j)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume(l, j);
}

முழு fputstrv(FILE *f, எழுத்து * மாறிலி *l, மாறிலி எழுத்து *separator, bool *space) {
        bool b = false;
        முழு r;

        ப்ரதிஜ்ஞா(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        எனில் (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

வெறுமை string_strv_hashmap_remove(Hashmap *h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        ப்ரதிஜ்ஞா(சாவி);

        எனில் (மதிப்பு) {
                எழுத்து **l = hashmap_get(h, சாவி);
                எனில் (!l)
                        திரும்பு;

                strv_remove(l, மதிப்பு);
                எனில் (!strv_isempty(l))
                        திரும்பு;
        }

        _unused_ _cleanup_free_ எழுத்து *சாவி_free = வெற்று;
        strv_free(hashmap_remove2(h, சாவி, (வெறுமை**) &சாவி_free));
}

வெறுமை string_strv_ordered_hashmap_remove(OrderedHashmap *h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), சாவி, மதிப்பு);
}

நிலையான முழு string_strv_hashmap_put_internal(Hashmap *h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        எழுத்து **l;
        முழு r;

        ப்ரதிஜ்ஞா(h);
        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(மதிப்பு);

        l = hashmap_get(h, சாவி);
        எனில் (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                எனில் (strv_contains(l, மதிப்பு))
                        திரும்பு 0;

                r = strv_extend(&l, மதிப்பு);
                எனில் (r < 0)
                        திரும்பு r;

                assert_se(hashmap_update(h, சாவி, l) >= 0);
        } இல்லையெனில் {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ எழுத்து **l2 = வெற்று;
                _cleanup_free_ எழுத்து *t = வெற்று;

                t = strdup(சாவி);
                எனில் (!t)
                        திரும்பு -ENOMEM;

                r = strv_extend(&l2, மதிப்பு);
                எனில் (r < 0)
                        திரும்பு r;

                r = hashmap_put(h, t, l2);
                எனில் (r < 0)
                        திரும்பு r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        திரும்பு 1;
}

முழு string_strv_hashmap_put(Hashmap **h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        முழு r;

        ப்ரதிஜ்ஞா(h);
        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(மதிப்பு);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு string_strv_hashmap_put_internal(*h, சாவி, மதிப்பு);
}

முழு string_strv_ordered_hashmap_put(OrderedHashmap **h, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        முழு r;

        ப்ரதிஜ்ஞா(h);
        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(மதிப்பு);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), சாவி, மதிப்பு);
}

நிலையான முழு rebreak_flush_வரி(
                எழுத்து ***broken,
                மாறிலி எழுத்து *start,
                மாறிலி எழுத்து *end,
                bool in_prefix,
                மாறிலி எழுத்து *whitespace_begin,
                மாறிலி எழுத்து *whitespace_end) {

        ப்ரதிஜ்ஞா(broken);
        ப்ரதிஜ்ஞா(start);
        ப்ரதிஜ்ஞா(end >= start);

        எனில் (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                திரும்பு strv_extend(broken, "");

        எனில் (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ எழுத்து *truncated = strndup(start, end - start);
        எனில் (!truncated)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume(broken, TAKE_PTR(truncated));
}

முழு strv_rebreak_வரிகள்(எழுத்து **l, size_t width, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **broken = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        எனில் (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                எனில் (!broken)
                        திரும்பு -ENOMEM;

                *ret = TAKE_PTR(broken);
                திரும்பு 0;
        }

        STRV_FOREACH(i, l) {
                மாறிலி எழுத்து *start = *i, *whitespace_begin = வெற்று, *whitespace_end = வெற்று;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                க்கு (மாறிலி எழுத்து *p = start; *p != 0; ) {
                        எனில் (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_வரி(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                எனில் (r < 0)
                                        திரும்பு r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = வெற்று;
                                w = 0;
                                தொடர்;
                        } இல்லையெனில் எனில் (strchr(WHITESPACE, *p)) {
                                எனில் (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = வெற்று;
                                }
                        } இல்லையெனில் {
                                எனில் (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        முழு cw = utf8_char_console_width(p);
                        எனில் (cw < 0)
                                திரும்பு log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        எனில் (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ எழுத்து *truncated = வெற்று;

                                truncated = strndup(start, whitespace_begin - start);
                                எனில் (!truncated)
                                        திரும்பு -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                எனில் (r < 0)
                                        திரும்பு r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = வெற்று;
                                w = 0;
                                தொடர்;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ப்ரதிஜ்ஞா(start);
                எனில் (after_newline && in_prefix)
                        தொடர்;

                r = rebreak_flush_வரி(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(broken);
        திரும்பு 0;
}

எழுத்து** strv_filter_prefix(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        எனில் (isempty(prefix))
                திரும்பு strv_copy(l);

        _cleanup_strv_free_ எழுத்து **f = வெற்று;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                எனில் (!startswith(*i, prefix))
                        தொடர்;

                எனில் (strv_extend_with_size(&f, &sz, *i) < 0)
                        திரும்பு வெற்று;
        }

        திரும்பு TAKE_PTR(f);
}

மாறிலி எழுத்து* மாறிலி strv_empty[] = { வெற்று };
