/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "gunicode.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಆದರೆ (streq(*i, name))
                        ಹಿಂತಿರುಗಿ *i;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_case(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಆದರೆ (strcaseeq(*i, name))
                        ಹಿಂತಿರುಗಿ *i;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l)
                ಆದರೆ (startswith(*i, name))
                        ಹಿಂತಿರುಗಿ *i;

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_startswith(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                ಅಕ್ಷರ *e;

                e = startswith(*i, name);
                ಆದರೆ (e)
                        ಹಿಂತಿರುಗಿ e;
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_closest_ಪೂರ್ವಪ್ರತ್ಯಯ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        size_t ಉತ್ತಮ_ದೂರ = SIZE_MAX;
        ಅಕ್ಷರ *ಉತ್ತಮ = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(s, l) {
                ಅಕ್ಷರ *e = startswith(*s, name);
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;

                size_t n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e);
                ಆದರೆ (n < ಉತ್ತಮ_ದೂರ) {
                        ಉತ್ತಮ_ದೂರ = n;
                        ಉತ್ತಮ = *s;
                }
        }

        ಹಿಂತಿರುಗಿ ಉತ್ತಮ;
}

ಸ್ಥಿರ ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_closest_by_levenshtein(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ssize_t ಉತ್ತಮ_ದೂರ = SSIZE_MAX;
        ಅಕ್ಷರ *ಉತ್ತಮ = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH(i, l) {
                ssize_t ದೂರ;

                ದೂರ = strlevenshtein(*i, name);
                ಆದರೆ (ದೂರ < 0) {
                        log_debug_errno(ದೂರ, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }

                ಆದರೆ (ದೂರ > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        ಮುಂದುವರಿ;

                ಆದರೆ (ದೂರ < ಉತ್ತಮ_ದೂರ) {
                        ಉತ್ತಮ_ದೂರ = ದೂರ;
                        ಉತ್ತಮ = *i;
                }
        }

        ಹಿಂತಿರುಗಿ ಉತ್ತಮ;
}

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_closest(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪ್ರತಿಜ್ಞಾ(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = strv_ಕಂಡುಹಿಡಿ_closest_ಪೂರ್ವಪ್ರತ್ಯಯ(l, name);
        ಆದರೆ (ಸಿಕ್ಕಿತು)
                ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;

        ಹಿಂತಿರುಗಿ strv_ಕಂಡುಹಿಡಿ_closest_by_levenshtein(l, name);
}

ಅಕ್ಷರ* strv_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಕ್ಷೇತ್ರ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *needles, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *haystack) {
        STRV_FOREACH(k, needles) {
                ಅಕ್ಷರ *ಮೌಲ್ಯ = strv_env_ಜೋಡಿಗಳು_get((ಅಕ್ಷರ **)haystack, *k);
                ಆದರೆ (ಮೌಲ್ಯ)
                        ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ** strv_free(ಅಕ್ಷರ **l) {
        STRV_FOREACH(k, l)
                ಬಿಡುಗಡೆ(*k);

        ಹಿಂತಿರುಗಿ mfree(l);
}

ಅಕ್ಷರ** strv_free_erase(ಅಕ್ಷರ **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        ಹಿಂತಿರುಗಿ mfree(l);
}

ಶೂನ್ಯ strv_free_many(ಅಕ್ಷರ ***strvs, size_t n) {
        ಪ್ರತಿಜ್ಞಾ(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        ಬಿಡುಗಡೆ(strvs);
}

ಅಕ್ಷರ** strv_copy_n(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, size_t n) {
        _cleanup_strv_free_ ಅಕ್ಷರ **ಫಲಿತಾಂಶ = ಖಾಲಿ;
        ಅಕ್ಷರ **k;

        ಫಲಿತಾಂಶ = new(ಅಕ್ಷರ*, MIN(strv_ಉದ್ದಳತೆ(l), n) + 1);
        ಆದರೆ (!ಫಲಿತಾಂಶ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        k = ಫಲಿತಾಂಶ;
        STRV_FOREACH(i, l) {
                ಆದರೆ (n == 0)
                        ನಿಲ್ಲಿಸು;

                *k = strdup(*i);
                ಆದರೆ (!*k)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                k++;

                ಆದರೆ (n != SIZE_MAX)
                        n--;
        }

        *k = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ TAKE_PTR(ಫಲಿತಾಂಶ);
}

ಪೂರ್ಣಾಂಕ strv_copy_unless_ಖಾಲಿಗಳು(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಅಕ್ಷರ ***ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (strv_isempty(l)) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಅಕ್ಷರ **copy = strv_copy(l);
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(copy);
        ಹಿಂತಿರುಗಿ 1;
}

size_t strv_ಉದ್ದಳತೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        ಹಿಂತಿರುಗಿ n;
}

ಅಕ್ಷರ** strv_new_ap(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, va_list ap) {
        _cleanup_strv_free_ ಅಕ್ಷರ **a = ಖಾಲಿ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = x; s; s = va_arg(aq, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)) {
                ಆದರೆ (s == STRV_IGNORE)
                        ಮುಂದುವರಿ;

                n++;
        }
        va_end(aq);

        a = new(ಅಕ್ಷರ*, n+1);
        ಆದರೆ (!a)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = x; s; s = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)) {
                ಆದರೆ (s == STRV_IGNORE)
                        ಮುಂದುವರಿ;

                a[i] = strdup(s);
                ಆದರೆ (!a[i])
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                i++;
        }

        a[i] = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ TAKE_PTR(a);
}

ಅಕ್ಷರ** strv_new_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ...) {
        ಅಕ್ಷರ **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ strv_extend_strv(ಅಕ್ಷರ ***a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ಪ್ರತಿಜ್ಞಾ(a);

        q = strv_ಉದ್ದಳತೆ(b);
        ಆದರೆ (q == 0)
                ಹಿಂತಿರುಗಿ 0;

        p = strv_ಉದ್ದಳತೆ(*a);
        ಆದರೆ (p >= SIZE_MAX - q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ **t = reallocarray(*a, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(p + q + 1), ಗಾತ್ರ(ಅಕ್ಷರ *));
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        t[p] = ಖಾಲಿ;
        *a = t;

        STRV_FOREACH(s, b) {
                ಆದರೆ (filter_duplicates && strv_ಒಳಗೊಂಡಿದೆ(t, *s))
                        ಮುಂದುವರಿ;

                t[p+i] = strdup(*s);
                ಆದರೆ (!t[p+i])
                        ಹೋಗು rollback;

                i++;
                t[p+i] = ಖಾಲಿ;
        }

        ಪ್ರತಿಜ್ಞಾ(i <= q);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ -ENOMEM;
}

ಪೂರ್ಣಾಂಕ strv_extend_strv_consume(ಅಕ್ಷರ ***a, ಅಕ್ಷರ **b, bool filter_duplicates) {
        _cleanup_strv_free_ ಅಕ್ಷರ **b_consume = b;
        size_t p, q, i;

        ಪ್ರತಿಜ್ಞಾ(a);

        q = strv_ಉದ್ದಳತೆ(b);
        ಆದರೆ (q == 0)
                ಹಿಂತಿರುಗಿ 0;

        p = strv_ಉದ್ದಳತೆ(*a);
        ಆದರೆ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ಆದರೆ (filter_duplicates)
                        strv_uniq(*a);

                ಹಿಂತಿರುಗಿ strv_ಉದ್ದಳತೆ(*a);
        }

        ಆದರೆ (p >= SIZE_MAX - q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ **t = reallocarray(*a, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(p + q + 1), ಗಾತ್ರ(ಅಕ್ಷರ *));
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        t[p] = ಖಾಲಿ;
        *a = t;

        ಆದರೆ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ಖಾಲಿ;
                i = q;
        } ಇಲ್ಲದಿದ್ದರೆ {
                i = 0;

                STRV_FOREACH(s, b) {
                        ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(t, *s)) {
                                ಬಿಡುಗಡೆ(*s);
                                ಮುಂದುವರಿ;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ಖಾಲಿ;
                }
        }

        ಪ್ರತಿಜ್ಞಾ(i <= q);

        b_consume = mfree(b_consume);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) i;
}

ಪೂರ್ಣಾಂಕ strv_extend_strv_biconcat(ಅಕ್ಷರ ***a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ *b, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);

        STRV_FOREACH(s, b) {
                ಅಕ್ಷರ *v;

                v = strjoin(strempty(ಪೂರ್ವಪ್ರತ್ಯಯ), *s, suffix);
                ಆದರೆ (!v)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = strv_consume(a, v);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_newlines_ತುಂಬಿದ(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಹೊರತೆಗೆ_Flags flags) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_ವಿಭಜಿಸು_ತುಂಬಿದ(&l, s, NEWLINE, flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        n = strv_ಉದ್ದಳತೆ(l);
        ಆದರೆ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ n;
}

ಅಕ್ಷರ** strv_ವಿಭಜಿಸು_newlines(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಅಕ್ಷರ **ret;

        ಆದರೆ (strv_ವಿಭಜಿಸು_newlines_ತುಂಬಿದ(&ret, s, 0) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_ತುಂಬಿದ(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        size_t n = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &ಶಬ್ದ, separators, flags);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (!GREEDY_REALLOC(l, n + 2))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l[n++] = TAKE_PTR(ಶಬ್ದ);
                l[n] = ಖಾಲಿ;
        }

        ಆದರೆ (!l) {
                l = new0(ಅಕ್ಷರ*, 1);
                ಆದರೆ (!l)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;
}

ಅಕ್ಷರ** strv_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators) {
        ಅಕ್ಷರ **ret;

        ಆದರೆ (strv_ವಿಭಜಿಸು_ತುಂಬಿದ(&ret, s, separators, ಹೊರತೆಗೆ_RETAIN_ESCAPE) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_and_extend_ತುಂಬಿದ(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, bool filter_duplicates, ಹೊರತೆಗೆ_Flags flags) {
        ಅಕ್ಷರ **l;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        r = strv_ವಿಭಜಿಸು_ತುಂಬಿದ(&l, s, separators, flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) strv_ಉದ್ದಳತೆ(*t);
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_and_extend(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, bool filter_duplicates) {
        ಹಿಂತಿರುಗಿ strv_ವಿಭಜಿಸು_and_extend_ತುಂಬಿದ(t, s, separators, filter_duplicates, 0);
}

ಪೂರ್ಣಾಂಕ strv_ವಿಭಜಿಸು_colon_ಜೋಡಿಗಳು(ಅಕ್ಷರ ***t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        size_t n = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಮೊದಲ = ಖಾಲಿ, *ಎರಡನೇ = ಖಾಲಿ, *tuple = ಖಾಲಿ, *ಎರಡನೇ_or_ಖಾಲಿಗಳು = ಖಾಲಿ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &tuple, ಖಾಲಿ, ಹೊರತೆಗೆ_UNQUOTE|ಹೊರತೆಗೆ_RETAIN_ESCAPE);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = tuple;
                r = ಹೊರತೆಗೆ_many_ಪದಗಳು(&p, ":", ಹೊರತೆಗೆ_CUNESCAPE|ಹೊರತೆಗೆ_UNESCAPE_SEPARATORS,
                                       &ಮೊದಲ, &ಎರಡನೇ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಮುಂದುವರಿ;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ಆದರೆ (!isempty(p))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಎರಡನೇ_or_ಖಾಲಿಗಳು = strdup(strempty(ಎರಡನೇ));
                ಆದರೆ (!ಎರಡನೇ_or_ಖಾಲಿಗಳು)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (!GREEDY_REALLOC(l, n + 3))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l[n++] = TAKE_PTR(ಮೊದಲ);
                l[n++] = TAKE_PTR(ಎರಡನೇ_or_ಖಾಲಿಗಳು);

                l[n] = ಖಾಲಿ;
        }

        ಆದರೆ (!l) {
                l = new0(ಅಕ್ಷರ*, 1);
                ಆದರೆ (!l)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        *t = TAKE_PTR(l);

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;
}

ಅಕ್ಷರ* strv_join_ತುಂಬಿದ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಅಕ್ಷರ *r, *e;
        size_t n, k, m;

        ಆದರೆ (!separator)
                separator = " ";

        k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(separator);
        m = strlen_ptr(ಪೂರ್ವಪ್ರತ್ಯಯ);

        n = 0;
        STRV_FOREACH(s, l) {
                ಆದರೆ (s != l)
                        n += k;

                n += m + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*s);
        }

        r = new(ಅಕ್ಷರ, n+1);
        ಆದರೆ (!r)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = r;
        STRV_FOREACH(s, l) {
                ಆದರೆ (s != l)
                        e = stpcpy(e, separator);

                ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ)
                        e = stpcpy(e, ಪೂರ್ವಪ್ರತ್ಯಯ);

                e = stpcpy(e, *s);
        }

        *e = 0;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ಪ್ರತಿಜ್ಞಾ(l);
        ಸೂಚಕ_MAY_BE_NULL(n);

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ 0;

        size_t ಗಾತ್ರ_2 = n ? *n : SIZE_MAX;
        ಆದರೆ (ಗಾತ್ರ_2 == SIZE_MAX)
                ಗಾತ್ರ_2 = strv_ಉದ್ದಳತೆ(*l);

        /* Check for overflow */
        ಆದರೆ (ಗಾತ್ರ_2 > SIZE_MAX-2)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ **c = reallocarray(*l, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(ಗಾತ್ರ_2 + 2), ಗಾತ್ರ(ಅಕ್ಷರ*));
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        c[ಗಾತ್ರ_2] = ಮೌಲ್ಯ;
        c[ಗಾತ್ರ_2+1] = ಖಾಲಿ;

        *l = c;
        ಆದರೆ (n)
                *n = ಗಾತ್ರ_2 + 1;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ strv_ತಳ್ಳು_pair(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *a, ಅಕ್ಷರ *b) {
        ಅಕ್ಷರ **c;
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಆದರೆ (!a && !b)
                ಹಿಂತಿರುಗಿ 0;

        n = strv_ಉದ್ದಳತೆ(*l);

        /* Check for overflow */
        ಆದರೆ (n > SIZE_MAX-3)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(n + !!a + !!b + 1), ಗಾತ್ರ(ಅಕ್ಷರ*));
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (a)
                c[n++] = a;
        ಆದರೆ (b)
                c[n++] = b;
        c[n] = ಖಾಲಿ;

        *l = c;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ strv_ಸೇರಿಸು(ಅಕ್ಷರ ***l, size_t position, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ **c;
        size_t n, m;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ 0;

        n = strv_ಉದ್ದಳತೆ(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ಆದರೆ (n > SIZE_MAX - 2)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(m), ಗಾತ್ರ(ಅಕ್ಷರ*));
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (n > position)
                memmove(c + position + 1, c + position, (n - position) * ಗಾತ್ರ(ಅಕ್ಷರ*));

        c[position] = ಮೌಲ್ಯ;
        c[n + 1] = ಖಾಲಿ;

        *l = c;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ strv_consume_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        r = strv_ತಳ್ಳು_with_ಗಾತ್ರ(l, n, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ strv_consume_pair(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *a, ಅಕ್ಷರ *b) {
        ಪೂರ್ಣಾಂಕ r;

        r = strv_ತಳ್ಳು_pair(l, a, b);
        ಆದರೆ (r < 0) {
                ಬಿಡುಗಡೆ(a);
                ಬಿಡುಗಡೆ(b);
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ strv_consume_prepend(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        r = strv_ತಳ್ಳು_prepend(l, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ strv_prepend(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ *v;

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ 0;

        v = strdup(ಮೌಲ್ಯ);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume_prepend(l, v);
}

ಪೂರ್ಣಾಂಕ strv_extend_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ *v;

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ 0;

        v = strdup(ಮೌಲ್ಯ);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume_with_ಗಾತ್ರ(l, n, v);
}

ಪೂರ್ಣಾಂಕ strv_extend_many_ಆಂತರಿಕ(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, ...) {
        va_list ap;
        size_t n, m;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);

        m = n = strv_ಉದ್ದಳತೆ(*l);

        r = 0;
        va_start(ap, ಮೌಲ್ಯ);
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = ಮೌಲ್ಯ; s != ಸೂಚಕ_MAX; s = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)) {
                ಆದರೆ (!s)
                        ಮುಂದುವರಿ;

                ಆದರೆ (m > SIZE_MAX-1) { /*ಅತಿಪ್ರವಾಹ*/
                        r = -ENOMEM;
                        ನಿಲ್ಲಿಸು;
                }
                m++;
        }
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (m > SIZE_MAX-1)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ **c = reallocarray(*l, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(m+1), ಗಾತ್ರ(ಅಕ್ಷರ*));
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, ಮೌಲ್ಯ);
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = ಮೌಲ್ಯ; s != ಸೂಚಕ_MAX; s = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)) {
                ಆದರೆ (!s)
                        ಮುಂದುವರಿ;

                c[i] = strdup(s);
                ಆದರೆ (!c[i]) {
                        r = -ENOMEM;
                        ನಿಲ್ಲಿಸು;
                }
                i++;
        }
        va_end(ap);

        ಆದರೆ (r < 0) {
                /* rollback on error */
                ಗಾಗಿ (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                ಹಿಂತಿರುಗಿ r;
        }

        c[i] = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ** strv_uniq(ಅಕ್ಷರ **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        ಹಿಂತಿರುಗಿ l;
}

bool strv_is_uniq(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        STRV_FOREACH(i, l)
                ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(i+1, *i))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಅಕ್ಷರ** strv_remove(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        ಅಕ್ಷರ **f, **t;
        ಗಾಗಿ (f = t = l; *f; f++)
                ಆದರೆ (streq(*f, s))
                        ಬಿಡುಗಡೆ(*f);
                ಇಲ್ಲದಿದ್ದರೆ
                        *(t++) = *f;

        *t = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** strv_remove_strv(ಅಕ್ಷರ **l, ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ*ll) {

        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        ಹಿಂತಿರುಗಿ l;
}

bool strv_overlap(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) {
        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(b, *i))
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ str_compare(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        ಹಿಂತಿರುಗಿ strcmp(*a, *b);
}

ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು(ಅಕ್ಷರ **l) {
        typesafe_qsort(l, strv_ಉದ್ದಳತೆ(l), str_compare);
        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು_uniq_ತುಂಬಿದ(ಅಕ್ಷರ **l, ಪೂರ್ಣಾಂಕ (*compare)(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b)) {
        ಪ್ರತಿಜ್ಞಾ(compare);

        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_ಉದ್ದಳತೆ(l), compare);

        ಅಕ್ಷರ **ಬಾಲ_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                ಆದರೆ (compare(i, ಬಾಲ_2 - 1) == 0)
                        ಬಿಡುಗಡೆ(*i);
                ಇಲ್ಲದಿದ್ದರೆ
                        *(ಬಾಲ_2++) = *i;

        *ಬಾಲ_2 = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** strv_ವಿಂಗಡಿಸು_uniq(ಅಕ್ಷರ **l) {
        ಹಿಂತಿರುಗಿ strv_ವಿಂಗಡಿಸು_uniq_ತುಂಬಿದ(l, str_compare);
}

ಪೂರ್ಣಾಂಕ strv_compare(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಸೂಚಕ_MAY_BE_NULL(a);
        ಸೂಚಕ_MAY_BE_NULL(b);

        ಆದರೆ (strv_isempty(a)) {
                ಆದರೆ (strv_isempty(b))
                        ಹಿಂತಿರುಗಿ 0;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ -1;
        }

        ಆದರೆ (strv_isempty(b))
                ಹಿಂತಿರುಗಿ 1;

        ಗಾಗಿ ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ಆದರೆ (r != 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

bool strv_equal_ignore_ಕ್ರಮ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *a, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ true;

        STRV_FOREACH(i, a)
                ಆದರೆ (!strv_ಒಳಗೊಂಡಿದೆ(b, *i))
                        ಹಿಂತಿರುಗಿ false;

        STRV_FOREACH(i, b)
                ಆದರೆ (!strv_ಒಳಗೊಂಡಿದೆ(a, *i))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಶೂನ್ಯ strv_print_ತುಂಬಿದ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        STRV_FOREACH(s, l)
                ಬರೆ("%s%s\n", strempty(ಪೂರ್ವಪ್ರತ್ಯಯ), *s);
}

ಪೂರ್ಣಾಂಕ strv_extendf_with_ಗಾತ್ರ(ಅಕ್ಷರ ***l, size_t *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {
        va_list ap;
        ಅಕ್ಷರ *x;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume_with_ಗಾತ್ರ(l, n, x);
}

ಪೂರ್ಣಾಂಕ strv_extend_joined_with_ಗಾತ್ರ_sentinel(ಅಕ್ಷರ ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        ಅಕ್ಷರ *x = strextendv_with_separator(/* x= */ ಖಾಲಿ, /* separator= */ ಖಾಲಿ, ap);
        va_end(ap);
        ಆದರೆ (!x)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume_with_ಗಾತ್ರ(l, n, x);
}

ಅಕ್ಷರ* startswith_strv_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        STRV_FOREACH(i, l) {
                ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = (ಅಕ್ಷರ*) startswith(s, *i);
                ಆದರೆ (ಸಿಕ್ಕಿತು)
                        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* endswith_strv_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        STRV_FOREACH(i, l) {
                ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = (ಅಕ್ಷರ*) endswith(s, *i);
                ಆದರೆ (ಸಿಕ್ಕಿತು)
                        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ** strv_ಹಿಮ್ಮುಖ(ಅಕ್ಷರ **l) {
        size_t n;

        n = strv_ಉದ್ದಳತೆ(l);
        ಆದರೆ (n <= 1)
                ಹಿಂತಿರುಗಿ l;

        ಗಾಗಿ (size_t i = 0; i < n / 2; i++)
                ಅದಲುಬದಲು_TWO(l[i], l[n-1-i]);

        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** strv_shell_escape(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                ಅಕ್ಷರ *v;

                v = shell_escape(*s, bad);
                ಆದರೆ (!v)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                free_and_replace(*s, v);
        }

        ಹಿಂತಿರುಗಿ l;
}

bool strv_fnmatch_ತುಂಬಿದ(
                ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* patterns,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s,
                ಪೂರ್ಣಾಂಕ flags,
                size_t *ret_matched_pos) {

        ಪ್ರತಿಜ್ಞಾ(s);

        ಆದರೆ (patterns)
                ಗಾಗಿ (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ಆದರೆ (fnmatch(patterns[i], s, flags) == 0) {
                                ಆದರೆ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                ಹಿಂತಿರುಗಿ true;
                        }

        ಆದರೆ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        ಹಿಂತಿರುಗಿ false;
}

ಅಕ್ಷರ** strv_skip(ಅಕ್ಷರ **l, size_t n) {
        ಆಗ (n > 0) {
                ಆದರೆ (strv_isempty(l))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ l;
}

ಪೂರ್ಣಾಂಕ strv_extend_n(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, size_t n) {
        size_t i, k;
        ಅಕ್ಷರ **nl;

        ಪ್ರತಿಜ್ಞಾ(l);

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ 0;

        /* Adds the value n times to l */

        k = strv_ಉದ್ದಳತೆ(*l);
        ಆದರೆ (n >= SIZE_MAX - k)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_ಸುತ್ತು_ಮೇಲೆ(k + n + 1), ಗಾತ್ರ(ಅಕ್ಷರ *));
        ಆದರೆ (!nl)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *l = nl;

        ಗಾಗಿ (i = k; i < k + n; i++) {
                nl[i] = strdup(ಮೌಲ್ಯ);
                ಆದರೆ (!nl[i])
                        ಹೋಗು rollback;
        }
        nl[i] = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ 0;

rollback:
        ಗಾಗಿ (size_t j = k; j < i; j++)
                ಬಿಡುಗಡೆ(nl[j]);
        nl[k] = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ -ENOMEM;
}

ಪೂರ್ಣಾಂಕ strv_extend_assignment(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lhs, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rhs) {
        ಅಕ್ಷರ *j;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(lhs);

        ಆದರೆ (!rhs) /* value is optional, in which case we suppress the field */
                ಹಿಂತಿರುಗಿ 0;

        j = strjoin(lhs, "=", rhs);
        ಆದರೆ (!j)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume(l, j);
}

ಪೂರ್ಣಾಂಕ fputstrv(FILE *f, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool *space) {
        bool b = false;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ಆದರೆ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ string_strv_hashmap_remove(Hashmap *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಆದರೆ (ಮೌಲ್ಯ) {
                ಅಕ್ಷರ **l = hashmap_get(h, ಕೀಲಿ);
                ಆದರೆ (!l)
                        ಹಿಂತಿರುಗಿ;

                strv_remove(l, ಮೌಲ್ಯ);
                ಆದರೆ (!strv_isempty(l))
                        ಹಿಂತಿರುಗಿ;
        }

        ಬಳಸದ _cleanup_free_ ಅಕ್ಷರ *ಕೀಲಿ_free = ಖಾಲಿ;
        strv_free(hashmap_remove2(h, ಕೀಲಿ, (ಶೂನ್ಯ**) &ಕೀಲಿ_free));
}

ಶೂನ್ಯ string_strv_ordered_hashmap_remove(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ string_strv_hashmap_put_ಆಂತರಿಕ(Hashmap *h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಅಕ್ಷರ **l;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        l = hashmap_get(h, ಕೀಲಿ);
        ಆದರೆ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(l, ಮೌಲ್ಯ))
                        ಹಿಂತಿರುಗಿ 0;

                r = strv_extend(&l, ಮೌಲ್ಯ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                assert_se(hashmap_update(h, ಕೀಲಿ, l) >= 0);
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ ಅಕ್ಷರ **l2 = ಖಾಲಿ;
                _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;

                t = strdup(ಕೀಲಿ);
                ಆದರೆ (!t)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = strv_extend(&l2, ಮೌಲ್ಯ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = hashmap_put(h, t, l2);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ string_strv_hashmap_put(Hashmap **h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ string_strv_hashmap_put_ಆಂತರಿಕ(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ string_strv_ordered_hashmap_put(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ string_strv_hashmap_put_ಆಂತರಿಕ(PLAIN_HASHMAP(*h), ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rebreak_flush_ಸಾಲು(
                ಅಕ್ಷರ ***broken,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ,
                bool ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *whitespace_begin,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *whitespace_ಕೊನೆ) {

        ಪ್ರತಿಜ್ಞಾ(broken);
        ಪ್ರತಿಜ್ಞಾ(ಆರಂಭ);
        ಪ್ರತಿಜ್ಞಾ(ಕೊನೆ >= ಆರಂಭ);

        ಆದರೆ (ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ) /* Never seen anything non-whitespace? Generate empty line! */
                ಹಿಂತಿರುಗಿ strv_extend(broken, "");

        ಆದರೆ (whitespace_begin && !whitespace_ಕೊನೆ) /* Ends in whitespace? Chop it off! */
                ಕೊನೆ = whitespace_begin;

        _cleanup_free_ ಅಕ್ಷರ *truncated = strndup(ಆರಂಭ, ಕೊನೆ - ಆರಂಭ);
        ಆದರೆ (!truncated)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume(broken, TAKE_PTR(truncated));
}

ಪೂರ್ಣಾಂಕ strv_rebreak_ಸಾಲುಗಳು(ಅಕ್ಷರ **l, size_t width, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **broken = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ಆದರೆ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ಆದರೆ (!broken)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret = TAKE_PTR(broken);
                ಹಿಂತಿರುಗಿ 0;
        }

        STRV_FOREACH(i, l) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆರಂಭ = *i, *whitespace_begin = ಖಾಲಿ, *whitespace_ಕೊನೆ = ಖಾಲಿ;
                bool ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಆರಂಭ; *p != 0; ) {
                        ಆದರೆ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_ಸಾಲು(&broken, ಆರಂಭ, p, ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ, whitespace_begin, whitespace_ಕೊನೆ);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                ಆರಂಭ = p;
                                ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ = true;
                                after_newline = true;
                                whitespace_begin = whitespace_ಕೊನೆ = ಖಾಲಿ;
                                w = 0;
                                ಮುಂದುವರಿ;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strchr(WHITESPACE, *p)) {
                                ಆದರೆ (!ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ && (!whitespace_begin || whitespace_ಕೊನೆ)) {
                                        whitespace_begin = p;
                                        whitespace_ಕೊನೆ = ಖಾಲಿ;
                                }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (whitespace_begin && !whitespace_ಕೊನೆ)
                                        whitespace_ಕೊನೆ = p;

                                ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        ಪೂರ್ಣಾಂಕ cw = utf8_char_console_ಅಗಲ(p);
                        ಆದರೆ (cw < 0)
                                ಹಿಂತಿರುಗಿ log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ಆದರೆ (w > width && whitespace_begin && whitespace_ಕೊನೆ) {
                                _cleanup_free_ ಅಕ್ಷರ *truncated = ಖಾಲಿ;

                                truncated = strndup(ಆರಂಭ, whitespace_begin - ಆರಂಭ);
                                ಆದರೆ (!truncated)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = ಆರಂಭ = whitespace_ಕೊನೆ;
                                after_newline = false;
                                whitespace_begin = whitespace_ಕೊನೆ = ಖಾಲಿ;
                                w = 0;
                                ಮುಂದುವರಿ;
                        }

                        p = utf8_ಮುಂದಿನ_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ಪ್ರತಿಜ್ಞಾ(ಆರಂಭ);
                ಆದರೆ (after_newline && ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ)
                        ಮುಂದುವರಿ;

                r = rebreak_flush_ಸಾಲು(&broken, ಆರಂಭ, ಆರಂಭ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಆರಂಭ), ಒಳ_ಪೂರ್ವಪ್ರತ್ಯಯ, whitespace_begin, whitespace_ಕೊನೆ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(broken);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ** strv_filter_ಪೂರ್ವಪ್ರತ್ಯಯ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ಆದರೆ (isempty(ಪೂರ್ವಪ್ರತ್ಯಯ))
                ಹಿಂತಿರುಗಿ strv_copy(l);

        _cleanup_strv_free_ ಅಕ್ಷರ **f = ಖಾಲಿ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ಆದರೆ (!startswith(*i, ಪೂರ್ವಪ್ರತ್ಯಯ))
                        ಮುಂದುವರಿ;

                ಆದರೆ (strv_extend_with_ಗಾತ್ರ(&f, &sz, *i) < 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }

        ಹಿಂತಿರುಗಿ TAKE_PTR(f);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ strv_ಖಾಲಿಗಳು[] = { ಖಾಲಿ };
