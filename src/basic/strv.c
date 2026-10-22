/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fnmatch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gunicode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਅੱਖਰ* strv_ਲੱਭੋ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH(i, l)
                ਜੇ (streq(*i, name))
                        ਵਾਪਸ *i;

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* strv_ਲੱਭੋ_case(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH(i, l)
                ਜੇ (strcaseeq(*i, name))
                        ਵਾਪਸ *i;

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* strv_ਲੱਭੋ_ਅਗੇਤਰ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH(i, l)
                ਜੇ (startswith(*i, name))
                        ਵਾਪਸ *i;

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* strv_ਲੱਭੋ_startswith(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                ਅੱਖਰ *e;

                e = startswith(*i, name);
                ਜੇ (e)
                        ਵਾਪਸ e;
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅੱਖਰ* strv_ਲੱਭੋ_closest_ਅਗੇਤਰ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        size_t ਉੱਤਮ_ਦੂਰੀ = SIZE_MAX;
        ਅੱਖਰ *ਉੱਤਮ = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH(s, l) {
                ਅੱਖਰ *e = startswith(*s, name);
                ਜੇ (!e)
                        ਜਾਰੀ;

                size_t n = ਲੜੀ_ਲੰਬਾਈ(e);
                ਜੇ (n < ਉੱਤਮ_ਦੂਰੀ) {
                        ਉੱਤਮ_ਦੂਰੀ = n;
                        ਉੱਤਮ = *s;
                }
        }

        ਵਾਪਸ ਉੱਤਮ;
}

ਸਥਿਰ ਅੱਖਰ* strv_ਲੱਭੋ_closest_by_levenshtein(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ssize_t ਉੱਤਮ_ਦੂਰੀ = SSIZE_MAX;
        ਅੱਖਰ *ਉੱਤਮ = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH(i, l) {
                ssize_t ਦੂਰੀ;

                ਦੂਰੀ = strlevenshtein(*i, name);
                ਜੇ (ਦੂਰੀ < 0) {
                        log_debug_errno(ਦੂਰੀ, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ਵਾਪਸ ਨਿਰਾ;
                }

                ਜੇ (ਦੂਰੀ > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ਜਾਰੀ;

                ਜੇ (ਦੂਰੀ < ਉੱਤਮ_ਦੂਰੀ) {
                        ਉੱਤਮ_ਦੂਰੀ = ਦੂਰੀ;
                        ਉੱਤਮ = *i;
                }
        }

        ਵਾਪਸ ਉੱਤਮ;
}

ਅੱਖਰ* strv_ਲੱਭੋ_closest(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        ਅੱਖਰ *ਮਿਲਿਆ = strv_ਲੱਭੋ_closest_ਅਗੇਤਰ(l, name);
        ਜੇ (ਮਿਲਿਆ)
                ਵਾਪਸ ਮਿਲਿਆ;

        ਵਾਪਸ strv_ਲੱਭੋ_closest_by_levenshtein(l, name);
}

ਅੱਖਰ* strv_ਲੱਭੋ_ਪਹਿਲਾ_ਖੇਤਰ(ਅੱਖਰ * ਅਚਲ *needles, ਅੱਖਰ * ਅਚਲ *haystack) {
        STRV_FOREACH(k, needles) {
                ਅੱਖਰ *ਮੁੱਲ = strv_env_ਜੋੜੇ_get((ਅੱਖਰ **)haystack, *k);
                ਜੇ (ਮੁੱਲ)
                        ਵਾਪਸ ਮੁੱਲ;
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ** strv_free(ਅੱਖਰ **l) {
        STRV_FOREACH(k, l)
                ਮੁਕਤ(*k);

        ਵਾਪਸ mfree(l);
}

ਅੱਖਰ** strv_free_erase(ਅੱਖਰ **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ਵਾਪਸ mfree(l);
}

ਖਾਲੀ strv_free_many(ਅੱਖਰ ***strvs, size_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        ਮੁਕਤ(strvs);
}

ਅੱਖਰ** strv_copy_n(ਅੱਖਰ * ਅਚਲ *l, size_t n) {
        _cleanup_strv_free_ ਅੱਖਰ **ਨਤੀਜਾ = ਨਿਰਾ;
        ਅੱਖਰ **k;

        ਨਤੀਜਾ = new(ਅੱਖਰ*, MIN(strv_ਲੰਮਾਈ(l), n) + 1);
        ਜੇ (!ਨਤੀਜਾ)
                ਵਾਪਸ ਨਿਰਾ;

        k = ਨਤੀਜਾ;
        STRV_FOREACH(i, l) {
                ਜੇ (n == 0)
                        ਰੋਕੋ;

                *k = strdup(*i);
                ਜੇ (!*k)
                        ਵਾਪਸ ਨਿਰਾ;
                k++;

                ਜੇ (n != SIZE_MAX)
                        n--;
        }

        *k = ਨਿਰਾ;
        ਵਾਪਸ TAKE_PTR(ਨਤੀਜਾ);
}

ਪੂਰਨ strv_copy_unless_ਖਾਲੀ_ਥਾਵਾਂ(ਅੱਖਰ * ਅਚਲ *l, ਅੱਖਰ ***ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (strv_isempty(l)) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਅੱਖਰ **copy = strv_copy(l);
        ਜੇ (!copy)
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ਵਾਪਸ 1;
}

size_t strv_ਲੰਮਾਈ(ਅੱਖਰ * ਅਚਲ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ਵਾਪਸ n;
}

ਅੱਖਰ** strv_new_ap(ਅਚਲ ਅੱਖਰ *x, va_list ap) {
        _cleanup_strv_free_ ਅੱਖਰ **a = ਨਿਰਾ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        ਲਈ (ਅਚਲ ਅੱਖਰ *s = x; s; s = va_arg(aq, ਅਚਲ ਅੱਖਰ*)) {
                ਜੇ (s == STRV_IGNORE)
                        ਜਾਰੀ;

                n++;
        }
        va_end(aq);

        a = new(ਅੱਖਰ*, n+1);
        ਜੇ (!a)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅਚਲ ਅੱਖਰ *s = x; s; s = va_arg(ap, ਅਚਲ ਅੱਖਰ*)) {
                ਜੇ (s == STRV_IGNORE)
                        ਜਾਰੀ;

                a[i] = strdup(s);
                ਜੇ (!a[i])
                        ਵਾਪਸ ਨਿਰਾ;

                i++;
        }

        a[i] = ਨਿਰਾ;

        ਵਾਪਸ TAKE_PTR(a);
}

ਅੱਖਰ** strv_new_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *x, ...) {
        ਅੱਖਰ **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ strv_extend_strv(ਅੱਖਰ ***a, ਅੱਖਰ * ਅਚਲ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        q = strv_ਲੰਮਾਈ(b);
        ਜੇ (q == 0)
                ਵਾਪਸ 0;

        p = strv_ਲੰਮਾਈ(*a);
        ਜੇ (p >= SIZE_MAX - q)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ **t = reallocarray(*a, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(p + q + 1), ਆਕਾਰ(ਅੱਖਰ *));
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        t[p] = ਨਿਰਾ;
        *a = t;

        STRV_FOREACH(s, b) {
                ਜੇ (filter_duplicates && strv_ਸ਼ਾਮਲ_ਹੈ(t, *s))
                        ਜਾਰੀ;

                t[p+i] = strdup(*s);
                ਜੇ (!t[p+i])
                        ਜਾਓ rollback;

                i++;
                t[p+i] = ਨਿਰਾ;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(i <= q);

        ਵਾਪਸ (ਪੂਰਨ) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ਨਿਰਾ;
        ਵਾਪਸ -ENOMEM;
}

ਪੂਰਨ strv_extend_strv_consume(ਅੱਖਰ ***a, ਅੱਖਰ **b, bool filter_duplicates) {
        _cleanup_strv_free_ ਅੱਖਰ **b_consume = b;
        size_t p, q, i;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        q = strv_ਲੰਮਾਈ(b);
        ਜੇ (q == 0)
                ਵਾਪਸ 0;

        p = strv_ਲੰਮਾਈ(*a);
        ਜੇ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ਜੇ (filter_duplicates)
                        strv_uniq(*a);

                ਵਾਪਸ strv_ਲੰਮਾਈ(*a);
        }

        ਜੇ (p >= SIZE_MAX - q)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ **t = reallocarray(*a, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(p + q + 1), ਆਕਾਰ(ਅੱਖਰ *));
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        t[p] = ਨਿਰਾ;
        *a = t;

        ਜੇ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ਨਿਰਾ;
                i = q;
        } ਨਹੀਂਤਾਂ {
                i = 0;

                STRV_FOREACH(s, b) {
                        ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(t, *s)) {
                                ਮੁਕਤ(*s);
                                ਜਾਰੀ;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ਨਿਰਾ;
                }
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(i <= q);

        b_consume = mfree(b_consume);

        ਵਾਪਸ (ਪੂਰਨ) i;
}

ਪੂਰਨ strv_extend_strv_biconcat(ਅੱਖਰ ***a, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ* ਅਚਲ *b, ਅਚਲ ਅੱਖਰ *suffix) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);

        STRV_FOREACH(s, b) {
                ਅੱਖਰ *v;

                v = strjoin(strempty(ਅਗੇਤਰ), *s, suffix);
                ਜੇ (!v)
                        ਵਾਪਸ -ENOMEM;

                r = strv_consume(a, v);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ strv_ਵੰਡੋ_newlines_ਭਰਿਆ(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *s, ਕੱਢੋ_Flags flags) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_ਵੰਡੋ_ਭਰਿਆ(&l, s, NEWLINE, flags);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        n = strv_ਲੰਮਾਈ(l);
        ਜੇ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ਵਾਪਸ n;
}

ਅੱਖਰ** strv_ਵੰਡੋ_newlines(ਅਚਲ ਅੱਖਰ *s) {
        ਅੱਖਰ **ret;

        ਜੇ (strv_ਵੰਡੋ_newlines_ਭਰਿਆ(&ret, s, 0) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ret;
}

ਪੂਰਨ strv_ਵੰਡੋ_ਭਰਿਆ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        size_t n = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &ਸ਼ਬਦ, separators, flags);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (!GREEDY_REALLOC(l, n + 2))
                        ਵਾਪਸ -ENOMEM;

                l[n++] = TAKE_PTR(ਸ਼ਬਦ);
                l[n] = ਨਿਰਾ;
        }

        ਜੇ (!l) {
                l = new0(ਅੱਖਰ*, 1);
                ਜੇ (!l)
                        ਵਾਪਸ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ਵਾਪਸ (ਪੂਰਨ) n;
}

ਅੱਖਰ** strv_ਵੰਡੋ(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators) {
        ਅੱਖਰ **ret;

        ਜੇ (strv_ਵੰਡੋ_ਭਰਿਆ(&ret, s, separators, ਕੱਢੋ_RETAIN_ESCAPE) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ret;
}

ਪੂਰਨ strv_ਵੰਡੋ_and_extend_ਭਰਿਆ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, bool filter_duplicates, ਕੱਢੋ_Flags flags) {
        ਅੱਖਰ **l;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = strv_ਵੰਡੋ_ਭਰਿਆ(&l, s, separators, flags);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ (ਪੂਰਨ) strv_ਲੰਮਾਈ(*t);
}

ਪੂਰਨ strv_ਵੰਡੋ_and_extend(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separators, bool filter_duplicates) {
        ਵਾਪਸ strv_ਵੰਡੋ_and_extend_ਭਰਿਆ(t, s, separators, filter_duplicates, 0);
}

ਪੂਰਨ strv_ਵੰਡੋ_colon_ਜੋੜੇ(ਅੱਖਰ ***t, ਅਚਲ ਅੱਖਰ *s) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        size_t n = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਪਹਿਲਾ = ਨਿਰਾ, *ਦੂਜਾ = ਨਿਰਾ, *tuple = ਨਿਰਾ, *ਦੂਜਾ_or_ਖਾਲੀ_ਥਾਵਾਂ = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &tuple, ਨਿਰਾ, ਕੱਢੋ_UNQUOTE|ਕੱਢੋ_RETAIN_ESCAPE);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਅਚਲ ਅੱਖਰ *p = tuple;
                r = ਕੱਢੋ_many_ਸ਼ਬਦ(&p, ":", ਕੱਢੋ_CUNESCAPE|ਕੱਢੋ_UNESCAPE_SEPARATORS,
                                       &ਪਹਿਲਾ, &ਦੂਜਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਜਾਰੀ;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ਜੇ (!isempty(p))
                        ਵਾਪਸ -EINVAL;

                ਦੂਜਾ_or_ਖਾਲੀ_ਥਾਵਾਂ = strdup(strempty(ਦੂਜਾ));
                ਜੇ (!ਦੂਜਾ_or_ਖਾਲੀ_ਥਾਵਾਂ)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (!GREEDY_REALLOC(l, n + 3))
                        ਵਾਪਸ -ENOMEM;

                l[n++] = TAKE_PTR(ਪਹਿਲਾ);
                l[n++] = TAKE_PTR(ਦੂਜਾ_or_ਖਾਲੀ_ਥਾਵਾਂ);

                l[n] = ਨਿਰਾ;
        }

        ਜੇ (!l) {
                l = new0(ਅੱਖਰ*, 1);
                ਜੇ (!l)
                        ਵਾਪਸ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ਵਾਪਸ (ਪੂਰਨ) n;
}

ਅੱਖਰ* strv_join_ਭਰਿਆ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *separator, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        ਅੱਖਰ *r, *e;
        size_t n, k, m;

        ਜੇ (!separator)
                separator = " ";

        k = ਲੜੀ_ਲੰਬਾਈ(separator);
        m = strlen_ptr(ਅਗੇਤਰ);

        n = 0;
        STRV_FOREACH(s, l) {
                ਜੇ (s != l)
                        n += k;

                n += m + ਲੜੀ_ਲੰਬਾਈ(*s);
        }

        r = new(ਅੱਖਰ, n+1);
        ਜੇ (!r)
                ਵਾਪਸ ਨਿਰਾ;

        e = r;
        STRV_FOREACH(s, l) {
                ਜੇ (s != l)
                        e = stpcpy(e, separator);

                ਜੇ (ਅਗੇਤਰ)
                        e = stpcpy(e, ਅਗੇਤਰ);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ਵਾਪਸ r;
}

ਪੂਰਨ strv_ਧੱਕੋ_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅੱਖਰ *ਮੁੱਲ) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਸੂਚਕ_MAY_BE_NULL(n);

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ 0;

        size_t ਮਾਪ = n ? *n : SIZE_MAX;
        ਜੇ (ਮਾਪ == SIZE_MAX)
                ਮਾਪ = strv_ਲੰਮਾਈ(*l);

        /* Check for overflow */
        ਜੇ (ਮਾਪ > SIZE_MAX-2)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ **c = reallocarray(*l, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(ਮਾਪ + 2), ਆਕਾਰ(ਅੱਖਰ*));
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        c[ਮਾਪ] = ਮੁੱਲ;
        c[ਮਾਪ+1] = ਨਿਰਾ;

        *l = c;
        ਜੇ (n)
                *n = ਮਾਪ + 1;
        ਵਾਪਸ 0;
}

ਪੂਰਨ strv_ਧੱਕੋ_pair(ਅੱਖਰ ***l, ਅੱਖਰ *a, ਅੱਖਰ *b) {
        ਅੱਖਰ **c;
        size_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        ਜੇ (!a && !b)
                ਵਾਪਸ 0;

        n = strv_ਲੰਮਾਈ(*l);

        /* Check for overflow */
        ਜੇ (n > SIZE_MAX-3)
                ਵਾਪਸ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(n + !!a + !!b + 1), ਆਕਾਰ(ਅੱਖਰ*));
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (a)
                c[n++] = a;
        ਜੇ (b)
                c[n++] = b;
        c[n] = ਨਿਰਾ;

        *l = c;
        ਵਾਪਸ 0;
}

ਪੂਰਨ strv_ਪਾਓ(ਅੱਖਰ ***l, size_t position, ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ **c;
        size_t n, m;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ 0;

        n = strv_ਲੰਮਾਈ(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ਜੇ (n > SIZE_MAX - 2)
                ਵਾਪਸ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(m), ਆਕਾਰ(ਅੱਖਰ*));
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (n > position)
                memmove(c + position + 1, c + position, (n - position) * ਆਕਾਰ(ਅੱਖਰ*));

        c[position] = ਮੁੱਲ;
        c[n + 1] = ਨਿਰਾ;

        *l = c;
        ਵਾਪਸ 0;
}

ਪੂਰਨ strv_consume_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅੱਖਰ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        r = strv_ਧੱਕੋ_with_ਮਾਪ(l, n, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਮੁਕਤ(ਮੁੱਲ);

        ਵਾਪਸ r;
}

ਪੂਰਨ strv_consume_pair(ਅੱਖਰ ***l, ਅੱਖਰ *a, ਅੱਖਰ *b) {
        ਪੂਰਨ r;

        r = strv_ਧੱਕੋ_pair(l, a, b);
        ਜੇ (r < 0) {
                ਮੁਕਤ(a);
                ਮੁਕਤ(b);
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ strv_consume_prepend(ਅੱਖਰ ***l, ਅੱਖਰ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        r = strv_ਧੱਕੋ_prepend(l, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਮੁਕਤ(ਮੁੱਲ);

        ਵਾਪਸ r;
}

ਪੂਰਨ strv_prepend(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ *v;

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ 0;

        v = strdup(ਮੁੱਲ);
        ਜੇ (!v)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume_prepend(l, v);
}

ਪੂਰਨ strv_extend_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ *v;

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ 0;

        v = strdup(ਮੁੱਲ);
        ਜੇ (!v)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume_with_ਮਾਪ(l, n, v);
}

ਪੂਰਨ strv_extend_many_ਅੰਦਰੂਨੀ(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, ...) {
        va_list ap;
        size_t n, m;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        m = n = strv_ਲੰਮਾਈ(*l);

        r = 0;
        va_start(ap, ਮੁੱਲ);
        ਲਈ (ਅਚਲ ਅੱਖਰ *s = ਮੁੱਲ; s != ਸੂਚਕ_MAX; s = va_arg(ap, ਅਚਲ ਅੱਖਰ*)) {
                ਜੇ (!s)
                        ਜਾਰੀ;

                ਜੇ (m > SIZE_MAX-1) { /*ਉਛਾਲ*/
                        r = -ENOMEM;
                        ਰੋਕੋ;
                }
                m++;
        }
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (m > SIZE_MAX-1)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ **c = reallocarray(*l, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(m+1), ਆਕਾਰ(ਅੱਖਰ*));
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, ਮੁੱਲ);
        ਲਈ (ਅਚਲ ਅੱਖਰ *s = ਮੁੱਲ; s != ਸੂਚਕ_MAX; s = va_arg(ap, ਅਚਲ ਅੱਖਰ*)) {
                ਜੇ (!s)
                        ਜਾਰੀ;

                c[i] = strdup(s);
                ਜੇ (!c[i]) {
                        r = -ENOMEM;
                        ਰੋਕੋ;
                }
                i++;
        }
        va_end(ap);

        ਜੇ (r < 0) {
                /* rollback on error */
                ਲਈ (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ਵਾਪਸ r;
        }

        c[i] = ਨਿਰਾ;
        ਵਾਪਸ 0;
}

ਅੱਖਰ** strv_uniq(ਅੱਖਰ **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ਵਾਪਸ l;
}

bool strv_is_uniq(ਅੱਖਰ * ਅਚਲ *l) {
        STRV_FOREACH(i, l)
                ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(i+1, *i))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਅੱਖਰ** strv_remove(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *s) {
        ਜੇ (!l)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        ਅੱਖਰ **f, **t;
        ਲਈ (f = t = l; *f; f++)
                ਜੇ (streq(*f, s))
                        ਮੁਕਤ(*f);
                ਨਹੀਂਤਾਂ
                        *(t++) = *f;

        *t = ਨਿਰਾ;
        ਵਾਪਸ l;
}

ਅੱਖਰ** strv_remove_strv(ਅੱਖਰ **l, ਅੱਖਰ *ਅਚਲ*ll) {

        ਜੇ (strv_isempty(l))
                ਵਾਪਸ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ਵਾਪਸ l;
}

bool strv_overlap(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) {
        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(b, *i))
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ str_compare(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        ਵਾਪਸ strcmp(*a, *b);
}

ਅੱਖਰ** strv_ਛਾਂਟੋ(ਅੱਖਰ **l) {
        typesafe_qsort(l, strv_ਲੰਮਾਈ(l), str_compare);
        ਵਾਪਸ l;
}

ਅੱਖਰ** strv_ਛਾਂਟੋ_uniq_ਭਰਿਆ(ਅੱਖਰ **l, ਪੂਰਨ (*compare)(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b)) {
        ਪ੍ਰਤਿਜ੍ਞਾ(compare);

        ਜੇ (strv_isempty(l))
                ਵਾਪਸ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_ਲੰਮਾਈ(l), compare);

        ਅੱਖਰ **ਪੂਛ_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                ਜੇ (compare(i, ਪੂਛ_2 - 1) == 0)
                        ਮੁਕਤ(*i);
                ਨਹੀਂਤਾਂ
                        *(ਪੂਛ_2++) = *i;

        *ਪੂਛ_2 = ਨਿਰਾ;
        ਵਾਪਸ l;
}

ਅੱਖਰ** strv_ਛਾਂਟੋ_uniq(ਅੱਖਰ **l) {
        ਵਾਪਸ strv_ਛਾਂਟੋ_uniq_ਭਰਿਆ(l, str_compare);
}

ਪੂਰਨ strv_compare(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) {
        ਪੂਰਨ r;

        ਸੂਚਕ_MAY_BE_NULL(a);
        ਸੂਚਕ_MAY_BE_NULL(b);

        ਜੇ (strv_isempty(a)) {
                ਜੇ (strv_isempty(b))
                        ਵਾਪਸ 0;
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ -1;
        }

        ਜੇ (strv_isempty(b))
                ਵਾਪਸ 1;

        ਲਈ ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ਜੇ (r != 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

bool strv_equal_ignore_ਕ੍ਰਮ(ਅੱਖਰ * ਅਚਲ *a, ਅੱਖਰ * ਅਚਲ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ਜੇ (a == b)
                ਵਾਪਸ true;

        STRV_FOREACH(i, a)
                ਜੇ (!strv_ਸ਼ਾਮਲ_ਹੈ(b, *i))
                        ਵਾਪਸ false;

        STRV_FOREACH(i, b)
                ਜੇ (!strv_ਸ਼ਾਮਲ_ਹੈ(a, *i))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਖਾਲੀ strv_print_ਭਰਿਆ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        STRV_FOREACH(s, l)
                ਲਿਖੋ("%s%s\n", strempty(ਅਗੇਤਰ), *s);
}

ਪੂਰਨ strv_extendf_with_ਮਾਪ(ਅੱਖਰ ***l, size_t *n, ਅਚਲ ਅੱਖਰ *format, ...) {
        va_list ap;
        ਅੱਖਰ *x;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume_with_ਮਾਪ(l, n, x);
}

ਪੂਰਨ strv_extend_joined_with_ਮਾਪ_sentinel(ਅੱਖਰ ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        ਅੱਖਰ *x = strextendv_with_separator(/* x= */ ਨਿਰਾ, /* separator= */ ਨਿਰਾ, ap);
        va_end(ap);
        ਜੇ (!x)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume_with_ਮਾਪ(l, n, x);
}

ਅੱਖਰ* startswith_strv_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ * ਅਚਲ *l) {
        STRV_FOREACH(i, l) {
                ਅੱਖਰ *ਮਿਲਿਆ = (ਅੱਖਰ*) startswith(s, *i);
                ਜੇ (ਮਿਲਿਆ)
                        ਵਾਪਸ ਮਿਲਿਆ;
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* endswith_strv_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ * ਅਚਲ *l) {
        STRV_FOREACH(i, l) {
                ਅੱਖਰ *ਮਿਲਿਆ = (ਅੱਖਰ*) endswith(s, *i);
                ਜੇ (ਮਿਲਿਆ)
                        ਵਾਪਸ ਮਿਲਿਆ;
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ** strv_ਉਲਟ(ਅੱਖਰ **l) {
        size_t n;

        n = strv_ਲੰਮਾਈ(l);
        ਜੇ (n <= 1)
                ਵਾਪਸ l;

        ਲਈ (size_t i = 0; i < n / 2; i++)
                ਅਦਲਾ_ਬਦਲੀ_TWO(l[i], l[n-1-i]);

        ਵਾਪਸ l;
}

ਅੱਖਰ** strv_shell_escape(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                ਅੱਖਰ *v;

                v = shell_escape(*s, bad);
                ਜੇ (!v)
                        ਵਾਪਸ ਨਿਰਾ;

                free_and_replace(*s, v);
        }

        ਵਾਪਸ l;
}

bool strv_fnmatch_ਭਰਿਆ(
                ਅੱਖਰ* ਅਚਲ* patterns,
                ਅਚਲ ਅੱਖਰ *s,
                ਪੂਰਨ flags,
                size_t *ret_matched_pos) {

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਜੇ (patterns)
                ਲਈ (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ਜੇ (fnmatch(patterns[i], s, flags) == 0) {
                                ਜੇ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ਵਾਪਸ true;
                        }

        ਜੇ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ਵਾਪਸ false;
}

ਅੱਖਰ** strv_skip(ਅੱਖਰ **l, size_t n) {
        ਜਦੋਂ (n > 0) {
                ਜੇ (strv_isempty(l))
                        ਵਾਪਸ ਨਿਰਾ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ਜੇ (strv_isempty(l))
                ਵਾਪਸ ਨਿਰਾ;
        ਵਾਪਸ l;
}

ਪੂਰਨ strv_extend_n(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, size_t n) {
        size_t i, k;
        ਅੱਖਰ **nl;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ 0;
        ਜੇ (n == 0)
                ਵਾਪਸ 0;

        /* Adds the value n times to l */

        k = strv_ਲੰਮਾਈ(*l);
        ਜੇ (n >= SIZE_MAX - k)
                ਵਾਪਸ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(k + n + 1), ਆਕਾਰ(ਅੱਖਰ *));
        ਜੇ (!nl)
                ਵਾਪਸ -ENOMEM;

        *l = nl;

        ਲਈ (i = k; i < k + n; i++) {
                nl[i] = strdup(ਮੁੱਲ);
                ਜੇ (!nl[i])
                        ਜਾਓ rollback;
        }
        nl[i] = ਨਿਰਾ;

        ਵਾਪਸ 0;

rollback:
        ਲਈ (size_t j = k; j < i; j++)
                ਮੁਕਤ(nl[j]);
        nl[k] = ਨਿਰਾ;

        ਵਾਪਸ -ENOMEM;
}

ਪੂਰਨ strv_extend_assignment(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *lhs, ਅਚਲ ਅੱਖਰ *rhs) {
        ਅੱਖਰ *j;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਪ੍ਰਤਿਜ੍ਞਾ(lhs);

        ਜੇ (!rhs) /* value is optional, in which case we suppress the field */
                ਵਾਪਸ 0;

        j = strjoin(lhs, "=", rhs);
        ਜੇ (!j)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume(l, j);
}

ਪੂਰਨ fputstrv(FILE *f, ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *separator, bool *space) {
        bool b = false;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ਜੇ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਖਾਲੀ string_strv_hashmap_remove(Hashmap *h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        ਜੇ (ਮੁੱਲ) {
                ਅੱਖਰ **l = hashmap_get(h, ਕੁੰਜੀ);
                ਜੇ (!l)
                        ਵਾਪਸ;

                strv_remove(l, ਮੁੱਲ);
                ਜੇ (!strv_isempty(l))
                        ਵਾਪਸ;
        }

        ਅਣਵਰਤਿਆ _cleanup_free_ ਅੱਖਰ *ਕੁੰਜੀ_free = ਨਿਰਾ;
        strv_free(hashmap_remove2(h, ਕੁੰਜੀ, (ਖਾਲੀ**) &ਕੁੰਜੀ_free));
}

ਖਾਲੀ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਸਥਿਰ ਪੂਰਨ string_strv_hashmap_put_ਅੰਦਰੂਨੀ(Hashmap *h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਅੱਖਰ **l;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        l = hashmap_get(h, ਕੁੰਜੀ);
        ਜੇ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(l, ਮੁੱਲ))
                        ਵਾਪਸ 0;

                r = strv_extend(&l, ਮੁੱਲ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                assert_se(hashmap_update(h, ਕੁੰਜੀ, l) >= 0);
        } ਨਹੀਂਤਾਂ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ ਅੱਖਰ **l2 = ਨਿਰਾ;
                _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;

                t = strdup(ਕੁੰਜੀ);
                ਜੇ (!t)
                        ਵਾਪਸ -ENOMEM;

                r = strv_extend(&l2, ਮੁੱਲ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = hashmap_put(h, t, l2);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ਵਾਪਸ 1;
}

ਪੂਰਨ string_strv_hashmap_put(Hashmap **h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ string_strv_hashmap_put_ਅੰਦਰੂਨੀ(*h, ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ string_strv_ordered_hashmap_put(OrderedHashmap **h, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ string_strv_hashmap_put_ਅੰਦਰੂਨੀ(PLAIN_HASHMAP(*h), ਕੁੰਜੀ, ਮੁੱਲ);
}

ਸਥਿਰ ਪੂਰਨ rebreak_flush_ਸਤਰ(
                ਅੱਖਰ ***broken,
                ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ,
                ਅਚਲ ਅੱਖਰ *ਸਿਰਾ,
                bool ਵਿੱਚ_ਅਗੇਤਰ,
                ਅਚਲ ਅੱਖਰ *whitespace_begin,
                ਅਚਲ ਅੱਖਰ *whitespace_ਸਿਰਾ) {

        ਪ੍ਰਤਿਜ੍ਞਾ(broken);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ੁਰੂ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਿਰਾ >= ਸ਼ੁਰੂ);

        ਜੇ (ਵਿੱਚ_ਅਗੇਤਰ) /* Never seen anything non-whitespace? Generate empty line! */
                ਵਾਪਸ strv_extend(broken, "");

        ਜੇ (whitespace_begin && !whitespace_ਸਿਰਾ) /* Ends in whitespace? Chop it off! */
                ਸਿਰਾ = whitespace_begin;

        _cleanup_free_ ਅੱਖਰ *truncated = strndup(ਸ਼ੁਰੂ, ਸਿਰਾ - ਸ਼ੁਰੂ);
        ਜੇ (!truncated)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume(broken, TAKE_PTR(truncated));
}

ਪੂਰਨ strv_rebreak_ਸਤਰਾਂ(ਅੱਖਰ **l, size_t width, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **broken = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ਜੇ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ਜੇ (!broken)
                        ਵਾਪਸ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ਵਾਪਸ 0;
        }

        STRV_FOREACH(i, l) {
                ਅਚਲ ਅੱਖਰ *ਸ਼ੁਰੂ = *i, *whitespace_begin = ਨਿਰਾ, *whitespace_ਸਿਰਾ = ਨਿਰਾ;
                bool ਵਿੱਚ_ਅਗੇਤਰ = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                ਲਈ (ਅਚਲ ਅੱਖਰ *p = ਸ਼ੁਰੂ; *p != 0; ) {
                        ਜੇ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_ਸਤਰ(&broken, ਸ਼ੁਰੂ, p, ਵਿੱਚ_ਅਗੇਤਰ, whitespace_begin, whitespace_ਸਿਰਾ);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                ਸ਼ੁਰੂ = p;
                                ਵਿੱਚ_ਅਗੇਤਰ = true;
                                after_newline = true;
                                whitespace_begin = whitespace_ਸਿਰਾ = ਨਿਰਾ;
                                w = 0;
                                ਜਾਰੀ;
                        } ਨਹੀਂਤਾਂ ਜੇ (strchr(WHITESPACE, *p)) {
                                ਜੇ (!ਵਿੱਚ_ਅਗੇਤਰ && (!whitespace_begin || whitespace_ਸਿਰਾ)) {
                                        whitespace_begin = p;
                                        whitespace_ਸਿਰਾ = ਨਿਰਾ;
                                }
                        } ਨਹੀਂਤਾਂ {
                                ਜੇ (whitespace_begin && !whitespace_ਸਿਰਾ)
                                        whitespace_ਸਿਰਾ = p;

                                ਵਿੱਚ_ਅਗੇਤਰ = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        ਪੂਰਨ cw = utf8_char_console_ਚੌੜਾਈ(p);
                        ਜੇ (cw < 0)
                                ਵਾਪਸ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ਜੇ (w > width && whitespace_begin && whitespace_ਸਿਰਾ) {
                                _cleanup_free_ ਅੱਖਰ *truncated = ਨਿਰਾ;

                                truncated = strndup(ਸ਼ੁਰੂ, whitespace_begin - ਸ਼ੁਰੂ);
                                ਜੇ (!truncated)
                                        ਵਾਪਸ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = ਸ਼ੁਰੂ = whitespace_ਸਿਰਾ;
                                after_newline = false;
                                whitespace_begin = whitespace_ਸਿਰਾ = ਨਿਰਾ;
                                w = 0;
                                ਜਾਰੀ;
                        }

                        p = utf8_ਅਗਲਾ_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ੁਰੂ);
                ਜੇ (after_newline && ਵਿੱਚ_ਅਗੇਤਰ)
                        ਜਾਰੀ;

                r = rebreak_flush_ਸਤਰ(&broken, ਸ਼ੁਰੂ, ਸ਼ੁਰੂ + ਲੜੀ_ਲੰਬਾਈ(ਸ਼ੁਰੂ), ਵਿੱਚ_ਅਗੇਤਰ, whitespace_begin, whitespace_ਸਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(broken);
        ਵਾਪਸ 0;
}

ਅੱਖਰ** strv_filter_ਅਗੇਤਰ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ਜੇ (isempty(ਅਗੇਤਰ))
                ਵਾਪਸ strv_copy(l);

        _cleanup_strv_free_ ਅੱਖਰ **f = ਨਿਰਾ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ਜੇ (!startswith(*i, ਅਗੇਤਰ))
                        ਜਾਰੀ;

                ਜੇ (strv_extend_with_ਮਾਪ(&f, &sz, *i) < 0)
                        ਵਾਪਸ ਨਿਰਾ;
        }

        ਵਾਪਸ TAKE_PTR(f);
}

ਅਚਲ ਅੱਖਰ* ਅਚਲ strv_ਖਾਲੀ_ਥਾਵਾਂ[] = { ਨਿਰਾ };
