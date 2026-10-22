/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fnmatch.h>
#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "gunicode.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "sort-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "utf8.h"

അക്ഷരം* strv_find(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        പ്രതിജ്ഞാ(name);

        STRV_FOREACH(i, l)
                എങ്കിൽ (streq(*i, name))
                        തിരികെ *i;

        തിരികെ ശൂന്യ;
}

അക്ഷരം* strv_find_case(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        പ്രതിജ്ഞാ(name);

        STRV_FOREACH(i, l)
                എങ്കിൽ (strcaseeq(*i, name))
                        തിരികെ *i;

        തിരികെ ശൂന്യ;
}

അക്ഷരം* strv_find_prefix(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        പ്രതിജ്ഞാ(name);

        STRV_FOREACH(i, l)
                എങ്കിൽ (startswith(*i, name))
                        തിരികെ *i;

        തിരികെ ശൂന്യ;
}

അക്ഷരം* strv_find_startswith(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        പ്രതിജ്ഞാ(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                അക്ഷരം *e;

                e = startswith(*i, name);
                എങ്കിൽ (e)
                        തിരികെ e;
        }

        തിരികെ ശൂന്യ;
}

സ്ഥിതം അക്ഷരം* strv_find_closest_prefix(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        size_t best_distance = SIZE_MAX;
        അക്ഷരം *best = ശൂന്യ;

        പ്രതിജ്ഞാ(name);

        STRV_FOREACH(s, l) {
                അക്ഷരം *e = startswith(*s, name);
                എങ്കിൽ (!e)
                        തുടരുക;

                size_t n = strlen(e);
                എങ്കിൽ (n < best_distance) {
                        best_distance = n;
                        best = *s;
                }
        }

        തിരികെ best;
}

സ്ഥിതം അക്ഷരം* strv_find_closest_by_levenshtein(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        ssize_t best_distance = SSIZE_MAX;
        അക്ഷരം *best = ശൂന്യ;

        പ്രതിജ്ഞാ(name);

        STRV_FOREACH(i, l) {
                ssize_t distance;

                distance = strlevenshtein(*i, name);
                എങ്കിൽ (distance < 0) {
                        log_debug_errno(distance, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        തിരികെ ശൂന്യ;
                }

                എങ്കിൽ (distance > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        തുടരുക;

                എങ്കിൽ (distance < best_distance) {
                        best_distance = distance;
                        best = *i;
                }
        }

        തിരികെ best;
}

അക്ഷരം* strv_find_closest(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name) {
        പ്രതിജ്ഞാ(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        അക്ഷരം *found = strv_find_closest_prefix(l, name);
        എങ്കിൽ (found)
                തിരികെ found;

        തിരികെ strv_find_closest_by_levenshtein(l, name);
}

അക്ഷരം* strv_find_first_field(അക്ഷരം * സ്ഥിരം *needles, അക്ഷരം * സ്ഥിരം *haystack) {
        STRV_FOREACH(k, needles) {
                അക്ഷരം *മൂല്യം = strv_env_pairs_get((അക്ഷരം **)haystack, *k);
                എങ്കിൽ (മൂല്യം)
                        തിരികെ മൂല്യം;
        }

        തിരികെ ശൂന്യ;
}

അക്ഷരം** strv_free(അക്ഷരം **l) {
        STRV_FOREACH(k, l)
                സ്വതന്ത്രം(*k);

        തിരികെ mfree(l);
}

അക്ഷരം** strv_free_erase(അക്ഷരം **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        തിരികെ mfree(l);
}

ശൂന്യം strv_free_many(അക്ഷരം ***strvs, size_t n) {
        പ്രതിജ്ഞാ(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        സ്വതന്ത്രം(strvs);
}

അക്ഷരം** strv_copy_n(അക്ഷരം * സ്ഥിരം *l, size_t n) {
        _cleanup_strv_free_ അക്ഷരം **result = ശൂന്യ;
        അക്ഷരം **k;

        result = new(അക്ഷരം*, MIN(strv_length(l), n) + 1);
        എങ്കിൽ (!result)
                തിരികെ ശൂന്യ;

        k = result;
        STRV_FOREACH(i, l) {
                എങ്കിൽ (n == 0)
                        നിർത്തുക;

                *k = strdup(*i);
                എങ്കിൽ (!*k)
                        തിരികെ ശൂന്യ;
                k++;

                എങ്കിൽ (n != SIZE_MAX)
                        n--;
        }

        *k = ശൂന്യ;
        തിരികെ TAKE_PTR(result);
}

പൂർണ്ണസംഖ്യ strv_copy_unless_empty(അക്ഷരം * സ്ഥിരം *l, അക്ഷരം ***ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (strv_isempty(l)) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        അക്ഷരം **copy = strv_copy(l);
        എങ്കിൽ (!copy)
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(copy);
        തിരികെ 1;
}

size_t strv_length(അക്ഷരം * സ്ഥിരം *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        തിരികെ n;
}

അക്ഷരം** strv_new_ap(സ്ഥിരം അക്ഷരം *x, va_list ap) {
        _cleanup_strv_free_ അക്ഷരം **a = ശൂന്യ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        വേണ്ടി (സ്ഥിരം അക്ഷരം *s = x; s; s = va_arg(aq, സ്ഥിരം അക്ഷരം*)) {
                എങ്കിൽ (s == STRV_IGNORE)
                        തുടരുക;

                n++;
        }
        va_end(aq);

        a = new(അക്ഷരം*, n+1);
        എങ്കിൽ (!a)
                തിരികെ ശൂന്യ;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *s = x; s; s = va_arg(ap, സ്ഥിരം അക്ഷരം*)) {
                എങ്കിൽ (s == STRV_IGNORE)
                        തുടരുക;

                a[i] = strdup(s);
                എങ്കിൽ (!a[i])
                        തിരികെ ശൂന്യ;

                i++;
        }

        a[i] = ശൂന്യ;

        തിരികെ TAKE_PTR(a);
}

അക്ഷരം** strv_new_internal(സ്ഥിരം അക്ഷരം *x, ...) {
        അക്ഷരം **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ strv_extend_strv(അക്ഷരം ***a, അക്ഷരം * സ്ഥിരം *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        പ്രതിജ്ഞാ(a);

        q = strv_length(b);
        എങ്കിൽ (q == 0)
                തിരികെ 0;

        p = strv_length(*a);
        എങ്കിൽ (p >= SIZE_MAX - q)
                തിരികെ -ENOMEM;

        അക്ഷരം **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), വലുപ്പം(അക്ഷരം *));
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        t[p] = ശൂന്യ;
        *a = t;

        STRV_FOREACH(s, b) {
                എങ്കിൽ (filter_duplicates && strv_contains(t, *s))
                        തുടരുക;

                t[p+i] = strdup(*s);
                എങ്കിൽ (!t[p+i])
                        പോകുക rollback;

                i++;
                t[p+i] = ശൂന്യ;
        }

        പ്രതിജ്ഞാ(i <= q);

        തിരികെ (പൂർണ്ണസംഖ്യ) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ശൂന്യ;
        തിരികെ -ENOMEM;
}

പൂർണ്ണസംഖ്യ strv_extend_strv_consume(അക്ഷരം ***a, അക്ഷരം **b, bool filter_duplicates) {
        _cleanup_strv_free_ അക്ഷരം **b_consume = b;
        size_t p, q, i;

        പ്രതിജ്ഞാ(a);

        q = strv_length(b);
        എങ്കിൽ (q == 0)
                തിരികെ 0;

        p = strv_length(*a);
        എങ്കിൽ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                എങ്കിൽ (filter_duplicates)
                        strv_uniq(*a);

                തിരികെ strv_length(*a);
        }

        എങ്കിൽ (p >= SIZE_MAX - q)
                തിരികെ -ENOMEM;

        അക്ഷരം **t = reallocarray(*a, GREEDY_ALLOC_ROUND_UP(p + q + 1), വലുപ്പം(അക്ഷരം *));
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        t[p] = ശൂന്യ;
        *a = t;

        എങ്കിൽ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ശൂന്യ;
                i = q;
        } അല്ലെങ്കിൽ {
                i = 0;

                STRV_FOREACH(s, b) {
                        എങ്കിൽ (strv_contains(t, *s)) {
                                സ്വതന്ത്രം(*s);
                                തുടരുക;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ശൂന്യ;
                }
        }

        പ്രതിജ്ഞാ(i <= q);

        b_consume = mfree(b_consume);

        തിരികെ (പൂർണ്ണസംഖ്യ) i;
}

പൂർണ്ണസംഖ്യ strv_extend_strv_biconcat(അക്ഷരം ***a, സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം* സ്ഥിരം *b, സ്ഥിരം അക്ഷരം *suffix) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);

        STRV_FOREACH(s, b) {
                അക്ഷരം *v;

                v = strjoin(strempty(prefix), *s, suffix);
                എങ്കിൽ (!v)
                        തിരികെ -ENOMEM;

                r = strv_consume(a, v);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ strv_split_newlines_full(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *s, ExtractFlags flags) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_split_full(&l, s, NEWLINE, flags);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        n = strv_length(l);
        എങ്കിൽ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        തിരികെ n;
}

അക്ഷരം** strv_split_newlines(സ്ഥിരം അക്ഷരം *s) {
        അക്ഷരം **ret;

        എങ്കിൽ (strv_split_newlines_full(&ret, s, 0) < 0)
                തിരികെ ശൂന്യ;

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ strv_split_full(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        size_t n = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *word = ശൂന്യ;

                r = extract_first_word(&s, &word, separators, flags);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (!GREEDY_REALLOC(l, n + 2))
                        തിരികെ -ENOMEM;

                l[n++] = TAKE_PTR(word);
                l[n] = ശൂന്യ;
        }

        എങ്കിൽ (!l) {
                l = new0(അക്ഷരം*, 1);
                എങ്കിൽ (!l)
                        തിരികെ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        തിരികെ (പൂർണ്ണസംഖ്യ) n;
}

അക്ഷരം** strv_split(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators) {
        അക്ഷരം **ret;

        എങ്കിൽ (strv_split_full(&ret, s, separators, EXTRACT_RETAIN_ESCAPE) < 0)
                തിരികെ ശൂന്യ;

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ strv_split_and_extend_full(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, bool filter_duplicates, ExtractFlags flags) {
        അക്ഷരം **l;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(s);

        r = strv_split_full(&l, s, separators, flags);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ (പൂർണ്ണസംഖ്യ) strv_length(*t);
}

പൂർണ്ണസംഖ്യ strv_split_and_extend(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separators, bool filter_duplicates) {
        തിരികെ strv_split_and_extend_full(t, s, separators, filter_duplicates, 0);
}

പൂർണ്ണസംഖ്യ strv_split_colon_pairs(അക്ഷരം ***t, സ്ഥിരം അക്ഷരം *s) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        size_t n = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(s);

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *first = ശൂന്യ, *second = ശൂന്യ, *tuple = ശൂന്യ, *second_or_empty = ശൂന്യ;

                r = extract_first_word(&s, &tuple, ശൂന്യ, EXTRACT_UNQUOTE|EXTRACT_RETAIN_ESCAPE);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                സ്ഥിരം അക്ഷരം *p = tuple;
                r = extract_many_വാക്കുകൾ(&p, ":", EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS,
                                       &first, &second);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തുടരുക;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                എങ്കിൽ (!isempty(p))
                        തിരികെ -EINVAL;

                second_or_empty = strdup(strempty(second));
                എങ്കിൽ (!second_or_empty)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (!GREEDY_REALLOC(l, n + 3))
                        തിരികെ -ENOMEM;

                l[n++] = TAKE_PTR(first);
                l[n++] = TAKE_PTR(second_or_empty);

                l[n] = ശൂന്യ;
        }

        എങ്കിൽ (!l) {
                l = new0(അക്ഷരം*, 1);
                എങ്കിൽ (!l)
                        തിരികെ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        തിരികെ (പൂർണ്ണസംഖ്യ) n;
}

അക്ഷരം* strv_join_full(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *separator, സ്ഥിരം അക്ഷരം *prefix) {
        അക്ഷരം *r, *e;
        size_t n, k, m;

        എങ്കിൽ (!separator)
                separator = " ";

        k = strlen(separator);
        m = strlen_ptr(prefix);

        n = 0;
        STRV_FOREACH(s, l) {
                എങ്കിൽ (s != l)
                        n += k;

                n += m + strlen(*s);
        }

        r = new(അക്ഷരം, n+1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        e = r;
        STRV_FOREACH(s, l) {
                എങ്കിൽ (s != l)
                        e = stpcpy(e, separator);

                എങ്കിൽ (prefix)
                        e = stpcpy(e, prefix);

                e = stpcpy(e, *s);
        }

        *e = 0;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ strv_തള്ളുക_with_size(അക്ഷരം ***l, size_t *n, അക്ഷരം *മൂല്യം) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        പ്രതിജ്ഞാ(l);
        POINTER_MAY_BE_NULL(n);

        എങ്കിൽ (!മൂല്യം)
                തിരികെ 0;

        size_t size = n ? *n : SIZE_MAX;
        എങ്കിൽ (size == SIZE_MAX)
                size = strv_length(*l);

        /* Check for overflow */
        എങ്കിൽ (size > SIZE_MAX-2)
                തിരികെ -ENOMEM;

        അക്ഷരം **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(size + 2), വലുപ്പം(അക്ഷരം*));
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        c[size] = മൂല്യം;
        c[size+1] = ശൂന്യ;

        *l = c;
        എങ്കിൽ (n)
                *n = size + 1;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ strv_തള്ളുക_pair(അക്ഷരം ***l, അക്ഷരം *a, അക്ഷരം *b) {
        അക്ഷരം **c;
        size_t n;

        പ്രതിജ്ഞാ(l);

        എങ്കിൽ (!a && !b)
                തിരികെ 0;

        n = strv_length(*l);

        /* Check for overflow */
        എങ്കിൽ (n > SIZE_MAX-3)
                തിരികെ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(n + !!a + !!b + 1), വലുപ്പം(അക്ഷരം*));
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        എങ്കിൽ (a)
                c[n++] = a;
        എങ്കിൽ (b)
                c[n++] = b;
        c[n] = ശൂന്യ;

        *l = c;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ strv_തിരുകുക(അക്ഷരം ***l, size_t position, അക്ഷരം *മൂല്യം) {
        അക്ഷരം **c;
        size_t n, m;

        പ്രതിജ്ഞാ(l);

        എങ്കിൽ (!മൂല്യം)
                തിരികെ 0;

        n = strv_length(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        എങ്കിൽ (n > SIZE_MAX - 2)
                തിരികെ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m), വലുപ്പം(അക്ഷരം*));
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        എങ്കിൽ (n > position)
                memmove(c + position + 1, c + position, (n - position) * വലുപ്പം(അക്ഷരം*));

        c[position] = മൂല്യം;
        c[n + 1] = ശൂന്യ;

        *l = c;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ strv_consume_with_size(അക്ഷരം ***l, size_t *n, അക്ഷരം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        r = strv_തള്ളുക_with_size(l, n, മൂല്യം);
        എങ്കിൽ (r < 0)
                സ്വതന്ത്രം(മൂല്യം);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ strv_consume_pair(അക്ഷരം ***l, അക്ഷരം *a, അക്ഷരം *b) {
        പൂർണ്ണസംഖ്യ r;

        r = strv_തള്ളുക_pair(l, a, b);
        എങ്കിൽ (r < 0) {
                സ്വതന്ത്രം(a);
                സ്വതന്ത്രം(b);
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ strv_consume_prepend(അക്ഷരം ***l, അക്ഷരം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        r = strv_തള്ളുക_prepend(l, മൂല്യം);
        എങ്കിൽ (r < 0)
                സ്വതന്ത്രം(മൂല്യം);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ strv_prepend(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        അക്ഷരം *v;

        എങ്കിൽ (!മൂല്യം)
                തിരികെ 0;

        v = strdup(മൂല്യം);
        എങ്കിൽ (!v)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume_prepend(l, v);
}

പൂർണ്ണസംഖ്യ strv_extend_with_size(അക്ഷരം ***l, size_t *n, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        അക്ഷരം *v;

        എങ്കിൽ (!മൂല്യം)
                തിരികെ 0;

        v = strdup(മൂല്യം);
        എങ്കിൽ (!v)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume_with_size(l, n, v);
}

പൂർണ്ണസംഖ്യ strv_extend_many_internal(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം, ...) {
        va_list ap;
        size_t n, m;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(l);

        m = n = strv_length(*l);

        r = 0;
        va_start(ap, മൂല്യം);
        വേണ്ടി (സ്ഥിരം അക്ഷരം *s = മൂല്യം; s != POINTER_MAX; s = va_arg(ap, സ്ഥിരം അക്ഷരം*)) {
                എങ്കിൽ (!s)
                        തുടരുക;

                എങ്കിൽ (m > SIZE_MAX-1) { /* overflow */
                        r = -ENOMEM;
                        നിർത്തുക;
                }
                m++;
        }
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (m > SIZE_MAX-1)
                തിരികെ -ENOMEM;

        അക്ഷരം **c = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(m+1), വലുപ്പം(അക്ഷരം*));
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, മൂല്യം);
        വേണ്ടി (സ്ഥിരം അക്ഷരം *s = മൂല്യം; s != POINTER_MAX; s = va_arg(ap, സ്ഥിരം അക്ഷരം*)) {
                എങ്കിൽ (!s)
                        തുടരുക;

                c[i] = strdup(s);
                എങ്കിൽ (!c[i]) {
                        r = -ENOMEM;
                        നിർത്തുക;
                }
                i++;
        }
        va_end(ap);

        എങ്കിൽ (r < 0) {
                /* rollback on error */
                വേണ്ടി (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                തിരികെ r;
        }

        c[i] = ശൂന്യ;
        തിരികെ 0;
}

അക്ഷരം** strv_uniq(അക്ഷരം **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        തിരികെ l;
}

bool strv_is_uniq(അക്ഷരം * സ്ഥിരം *l) {
        STRV_FOREACH(i, l)
                എങ്കിൽ (strv_contains(i+1, *i))
                        തിരികെ false;

        തിരികെ true;
}

അക്ഷരം** strv_remove(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *s) {
        എങ്കിൽ (!l)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        അക്ഷരം **f, **t;
        വേണ്ടി (f = t = l; *f; f++)
                എങ്കിൽ (streq(*f, s))
                        സ്വതന്ത്രം(*f);
                അല്ലെങ്കിൽ
                        *(t++) = *f;

        *t = ശൂന്യ;
        തിരികെ l;
}

അക്ഷരം** strv_remove_strv(അക്ഷരം **l, അക്ഷരം *സ്ഥിരം*ll) {

        എങ്കിൽ (strv_isempty(l))
                തിരികെ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        തിരികെ l;
}

bool strv_overlap(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) {
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                എങ്കിൽ (strv_contains(b, *i))
                        തിരികെ true;

        തിരികെ false;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ str_compare(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        തിരികെ strcmp(*a, *b);
}

അക്ഷരം** strv_sort(അക്ഷരം **l) {
        typesafe_qsort(l, strv_length(l), str_compare);
        തിരികെ l;
}

അക്ഷരം** strv_sort_uniq_full(അക്ഷരം **l, പൂർണ്ണസംഖ്യ (*compare)(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b)) {
        പ്രതിജ്ഞാ(compare);

        എങ്കിൽ (strv_isempty(l))
                തിരികെ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_length(l), compare);

        അക്ഷരം **tail = l + 1;
        STRV_FOREACH(i, l + 1)
                എങ്കിൽ (compare(i, tail - 1) == 0)
                        സ്വതന്ത്രം(*i);
                അല്ലെങ്കിൽ
                        *(tail++) = *i;

        *tail = ശൂന്യ;
        തിരികെ l;
}

അക്ഷരം** strv_sort_uniq(അക്ഷരം **l) {
        തിരികെ strv_sort_uniq_full(l, str_compare);
}

പൂർണ്ണസംഖ്യ strv_compare(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) {
        പൂർണ്ണസംഖ്യ r;

        POINTER_MAY_BE_NULL(a);
        POINTER_MAY_BE_NULL(b);

        എങ്കിൽ (strv_isempty(a)) {
                എങ്കിൽ (strv_isempty(b))
                        തിരികെ 0;
                അല്ലെങ്കിൽ
                        തിരികെ -1;
        }

        എങ്കിൽ (strv_isempty(b))
                തിരികെ 1;

        വേണ്ടി ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                എങ്കിൽ (r != 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

bool strv_equal_ignore_order(അക്ഷരം * സ്ഥിരം *a, അക്ഷരം * സ്ഥിരം *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        എങ്കിൽ (a == b)
                തിരികെ true;

        STRV_FOREACH(i, a)
                എങ്കിൽ (!strv_contains(b, *i))
                        തിരികെ false;

        STRV_FOREACH(i, b)
                എങ്കിൽ (!strv_contains(a, *i))
                        തിരികെ false;

        തിരികെ true;
}

ശൂന്യം strv_print_full(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *prefix) {
        STRV_FOREACH(s, l)
                printf("%s%s\n", strempty(prefix), *s);
}

പൂർണ്ണസംഖ്യ strv_extendf_with_size(അക്ഷരം ***l, size_t *n, സ്ഥിരം അക്ഷരം *format, ...) {
        va_list ap;
        അക്ഷരം *x;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume_with_size(l, n, x);
}

പൂർണ്ണസംഖ്യ strv_extend_joined_with_size_sentinel(അക്ഷരം ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        അക്ഷരം *x = strextendv_with_separator(/* x= */ ശൂന്യ, /* separator= */ ശൂന്യ, ap);
        va_end(ap);
        എങ്കിൽ (!x)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume_with_size(l, n, x);
}

അക്ഷരം* startswith_strv_internal(സ്ഥിരം അക്ഷരം *s, അക്ഷരം * സ്ഥിരം *l) {
        STRV_FOREACH(i, l) {
                അക്ഷരം *found = (അക്ഷരം*) startswith(s, *i);
                എങ്കിൽ (found)
                        തിരികെ found;
        }

        തിരികെ ശൂന്യ;
}

അക്ഷരം* endswith_strv_internal(സ്ഥിരം അക്ഷരം *s, അക്ഷരം * സ്ഥിരം *l) {
        STRV_FOREACH(i, l) {
                അക്ഷരം *found = (അക്ഷരം*) endswith(s, *i);
                എങ്കിൽ (found)
                        തിരികെ found;
        }

        തിരികെ ശൂന്യ;
}

അക്ഷരം** strv_reverse(അക്ഷരം **l) {
        size_t n;

        n = strv_length(l);
        എങ്കിൽ (n <= 1)
                തിരികെ l;

        വേണ്ടി (size_t i = 0; i < n / 2; i++)
                SWAP_TWO(l[i], l[n-1-i]);

        തിരികെ l;
}

അക്ഷരം** strv_shell_escape(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                അക്ഷരം *v;

                v = shell_escape(*s, bad);
                എങ്കിൽ (!v)
                        തിരികെ ശൂന്യ;

                free_and_replace(*s, v);
        }

        തിരികെ l;
}

bool strv_fnmatch_full(
                അക്ഷരം* സ്ഥിരം* patterns,
                സ്ഥിരം അക്ഷരം *s,
                പൂർണ്ണസംഖ്യ flags,
                size_t *ret_matched_pos) {

        പ്രതിജ്ഞാ(s);

        എങ്കിൽ (patterns)
                വേണ്ടി (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        എങ്കിൽ (fnmatch(patterns[i], s, flags) == 0) {
                                എങ്കിൽ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                തിരികെ true;
                        }

        എങ്കിൽ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        തിരികെ false;
}

അക്ഷരം** strv_skip(അക്ഷരം **l, size_t n) {
        ആകുമ്പോൾ (n > 0) {
                എങ്കിൽ (strv_isempty(l))
                        തിരികെ ശൂന്യ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        എങ്കിൽ (strv_isempty(l))
                തിരികെ ശൂന്യ;
        തിരികെ l;
}

പൂർണ്ണസംഖ്യ strv_extend_n(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *മൂല്യം, size_t n) {
        size_t i, k;
        അക്ഷരം **nl;

        പ്രതിജ്ഞാ(l);

        എങ്കിൽ (!മൂല്യം)
                തിരികെ 0;
        എങ്കിൽ (n == 0)
                തിരികെ 0;

        /* Adds the value n times to l */

        k = strv_length(*l);
        എങ്കിൽ (n >= SIZE_MAX - k)
                തിരികെ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ROUND_UP(k + n + 1), വലുപ്പം(അക്ഷരം *));
        എങ്കിൽ (!nl)
                തിരികെ -ENOMEM;

        *l = nl;

        വേണ്ടി (i = k; i < k + n; i++) {
                nl[i] = strdup(മൂല്യം);
                എങ്കിൽ (!nl[i])
                        പോകുക rollback;
        }
        nl[i] = ശൂന്യ;

        തിരികെ 0;

rollback:
        വേണ്ടി (size_t j = k; j < i; j++)
                സ്വതന്ത്രം(nl[j]);
        nl[k] = ശൂന്യ;

        തിരികെ -ENOMEM;
}

പൂർണ്ണസംഖ്യ strv_extend_assignment(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *lhs, സ്ഥിരം അക്ഷരം *rhs) {
        അക്ഷരം *j;

        പ്രതിജ്ഞാ(l);
        പ്രതിജ്ഞാ(lhs);

        എങ്കിൽ (!rhs) /* value is optional, in which case we suppress the field */
                തിരികെ 0;

        j = strjoin(lhs, "=", rhs);
        എങ്കിൽ (!j)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume(l, j);
}

പൂർണ്ണസംഖ്യ fputstrv(FILE *f, അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *separator, bool *space) {
        bool b = false;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        എങ്കിൽ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

ശൂന്യം string_strv_hashmap_remove(Hashmap *h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പ്രതിജ്ഞാ(താക്കോൽ);

        എങ്കിൽ (മൂല്യം) {
                അക്ഷരം **l = hashmap_get(h, താക്കോൽ);
                എങ്കിൽ (!l)
                        തിരികെ;

                strv_remove(l, മൂല്യം);
                എങ്കിൽ (!strv_isempty(l))
                        തിരികെ;
        }

        _unused_ _cleanup_free_ അക്ഷരം *താക്കോൽ_free = ശൂന്യ;
        strv_free(hashmap_remove2(h, താക്കോൽ, (ശൂന്യം**) &താക്കോൽ_free));
}

ശൂന്യം string_strv_ordered_hashmap_remove(OrderedHashmap *h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), താക്കോൽ, മൂല്യം);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ string_strv_hashmap_put_internal(Hashmap *h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        അക്ഷരം **l;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(മൂല്യം);

        l = hashmap_get(h, താക്കോൽ);
        എങ്കിൽ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                എങ്കിൽ (strv_contains(l, മൂല്യം))
                        തിരികെ 0;

                r = strv_extend(&l, മൂല്യം);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                assert_se(hashmap_update(h, താക്കോൽ, l) >= 0);
        } അല്ലെങ്കിൽ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ അക്ഷരം **l2 = ശൂന്യ;
                _cleanup_free_ അക്ഷരം *t = ശൂന്യ;

                t = strdup(താക്കോൽ);
                എങ്കിൽ (!t)
                        തിരികെ -ENOMEM;

                r = strv_extend(&l2, മൂല്യം);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = hashmap_put(h, t, l2);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ string_strv_hashmap_put(Hashmap **h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(മൂല്യം);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ string_strv_hashmap_put_internal(*h, താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ string_strv_ordered_hashmap_put(OrderedHashmap **h, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(മൂല്യം);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ string_strv_hashmap_put_internal(PLAIN_HASHMAP(*h), താക്കോൽ, മൂല്യം);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rebreak_flush_വരി(
                അക്ഷരം ***broken,
                സ്ഥിരം അക്ഷരം *start,
                സ്ഥിരം അക്ഷരം *end,
                bool in_prefix,
                സ്ഥിരം അക്ഷരം *whitespace_begin,
                സ്ഥിരം അക്ഷരം *whitespace_end) {

        പ്രതിജ്ഞാ(broken);
        പ്രതിജ്ഞാ(start);
        പ്രതിജ്ഞാ(end >= start);

        എങ്കിൽ (in_prefix) /* Never seen anything non-whitespace? Generate empty line! */
                തിരികെ strv_extend(broken, "");

        എങ്കിൽ (whitespace_begin && !whitespace_end) /* Ends in whitespace? Chop it off! */
                end = whitespace_begin;

        _cleanup_free_ അക്ഷരം *truncated = strndup(start, end - start);
        എങ്കിൽ (!truncated)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume(broken, TAKE_PTR(truncated));
}

പൂർണ്ണസംഖ്യ strv_rebreak_വരികൾ(അക്ഷരം **l, size_t width, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **broken = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        എങ്കിൽ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                എങ്കിൽ (!broken)
                        തിരികെ -ENOMEM;

                *ret = TAKE_PTR(broken);
                തിരികെ 0;
        }

        STRV_FOREACH(i, l) {
                സ്ഥിരം അക്ഷരം *start = *i, *whitespace_begin = ശൂന്യ, *whitespace_end = ശൂന്യ;
                bool in_prefix = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                വേണ്ടി (സ്ഥിരം അക്ഷരം *p = start; *p != 0; ) {
                        എങ്കിൽ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_വരി(&broken, start, p, in_prefix, whitespace_begin, whitespace_end);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                start = p;
                                in_prefix = true;
                                after_newline = true;
                                whitespace_begin = whitespace_end = ശൂന്യ;
                                w = 0;
                                തുടരുക;
                        } അല്ലെങ്കിൽ എങ്കിൽ (strchr(WHITESPACE, *p)) {
                                എങ്കിൽ (!in_prefix && (!whitespace_begin || whitespace_end)) {
                                        whitespace_begin = p;
                                        whitespace_end = ശൂന്യ;
                                }
                        } അല്ലെങ്കിൽ {
                                എങ്കിൽ (whitespace_begin && !whitespace_end)
                                        whitespace_end = p;

                                in_prefix = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        പൂർണ്ണസംഖ്യ cw = utf8_char_console_width(p);
                        എങ്കിൽ (cw < 0)
                                തിരികെ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        എങ്കിൽ (w > width && whitespace_begin && whitespace_end) {
                                _cleanup_free_ അക്ഷരം *truncated = ശൂന്യ;

                                truncated = strndup(start, whitespace_begin - start);
                                എങ്കിൽ (!truncated)
                                        തിരികെ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = start = whitespace_end;
                                after_newline = false;
                                whitespace_begin = whitespace_end = ശൂന്യ;
                                w = 0;
                                തുടരുക;
                        }

                        p = utf8_next_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                പ്രതിജ്ഞാ(start);
                എങ്കിൽ (after_newline && in_prefix)
                        തുടരുക;

                r = rebreak_flush_വരി(&broken, start, start + strlen(start), in_prefix, whitespace_begin, whitespace_end);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(broken);
        തിരികെ 0;
}

അക്ഷരം** strv_filter_prefix(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *prefix) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        എങ്കിൽ (isempty(prefix))
                തിരികെ strv_copy(l);

        _cleanup_strv_free_ അക്ഷരം **f = ശൂന്യ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                എങ്കിൽ (!startswith(*i, prefix))
                        തുടരുക;

                എങ്കിൽ (strv_extend_with_size(&f, &sz, *i) < 0)
                        തിരികെ ശൂന്യ;
        }

        തിരികെ TAKE_PTR(f);
}

സ്ഥിരം അക്ഷരം* സ്ഥിരം strv_empty[] = { ശൂന്യ };
