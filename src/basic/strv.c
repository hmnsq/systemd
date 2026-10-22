/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fnmatch.h>
#అన్తర్భావ <stdio.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "gunicode.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "sort-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "utf8.h"

అక్షరం* strv_కనుగొను(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ప్రతిజ్ఞా(name);

        STRV_FOREACH(i, l)
                ఒకవేళ (streq(*i, name))
                        తిరిగి *i;

        తిరిగి ఖాళీ;
}

అక్షరం* strv_కనుగొను_case(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ప్రతిజ్ఞా(name);

        STRV_FOREACH(i, l)
                ఒకవేళ (strcaseeq(*i, name))
                        తిరిగి *i;

        తిరిగి ఖాళీ;
}

అక్షరం* strv_కనుగొను_ఉపసర్గ(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ప్రతిజ్ఞా(name);

        STRV_FOREACH(i, l)
                ఒకవేళ (startswith(*i, name))
                        తిరిగి *i;

        తిరిగి ఖాళీ;
}

అక్షరం* strv_కనుగొను_startswith(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ప్రతిజ్ఞా(name);

        /* Like strv_find_prefix, but actually returns only the
         * suffix, not the whole item */

        STRV_FOREACH(i, l) {
                అక్షరం *e;

                e = startswith(*i, name);
                ఒకవేళ (e)
                        తిరిగి e;
        }

        తిరిగి ఖాళీ;
}

స్థిర అక్షరం* strv_కనుగొను_closest_ఉపసర్గ(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        size_t ఉత్తమ_దూరం = SIZE_MAX;
        అక్షరం *ఉత్తమ = ఖాళీ;

        ప్రతిజ్ఞా(name);

        STRV_FOREACH(s, l) {
                అక్షరం *e = startswith(*s, name);
                ఒకవేళ (!e)
                        కొనసాగు;

                size_t n = అక్షరశ్రేణి_పొడవు(e);
                ఒకవేళ (n < ఉత్తమ_దూరం) {
                        ఉత్తమ_దూరం = n;
                        ఉత్తమ = *s;
                }
        }

        తిరిగి ఉత్తమ;
}

స్థిర అక్షరం* strv_కనుగొను_closest_by_levenshtein(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ssize_t ఉత్తమ_దూరం = SSIZE_MAX;
        అక్షరం *ఉత్తమ = ఖాళీ;

        ప్రతిజ్ఞా(name);

        STRV_FOREACH(i, l) {
                ssize_t దూరం;

                దూరం = strlevenshtein(*i, name);
                ఒకవేళ (దూరం < 0) {
                        log_debug_errno(దూరం, "Failed to determine Levenshtein distance between %s and %s: %m", *i, name);
                        తిరిగి ఖాళీ;
                }

                ఒకవేళ (దూరం > 5) /* If the distance is just too far off, don't make a bad suggestion */
                        కొనసాగు;

                ఒకవేళ (దూరం < ఉత్తమ_దూరం) {
                        ఉత్తమ_దూరం = దూరం;
                        ఉత్తమ = *i;
                }
        }

        తిరిగి ఉత్తమ;
}

అక్షరం* strv_కనుగొను_closest(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name) {
        ప్రతిజ్ఞా(name);

        /* Be more helpful to the user, and give a hint what the user might have wanted to type. We search
         * with two mechanisms: a simple prefix match and – if that didn't yield results –, a Levenshtein
         * word distance based match. */

        అక్షరం *దొరికింది = strv_కనుగొను_closest_ఉపసర్గ(l, name);
        ఒకవేళ (దొరికింది)
                తిరిగి దొరికింది;

        తిరిగి strv_కనుగొను_closest_by_levenshtein(l, name);
}

అక్షరం* strv_కనుగొను_మొదటి_క్షేత్రం(అక్షరం * స్థిరరాశి *needles, అక్షరం * స్థిరరాశి *haystack) {
        STRV_FOREACH(k, needles) {
                అక్షరం *విలువ = strv_env_జతలు_get((అక్షరం **)haystack, *k);
                ఒకవేళ (విలువ)
                        తిరిగి విలువ;
        }

        తిరిగి ఖాళీ;
}

అక్షరం** strv_free(అక్షరం **l) {
        STRV_FOREACH(k, l)
                విడుదల(*k);

        తిరిగి mfree(l);
}

అక్షరం** strv_free_erase(అక్షరం **l) {
        STRV_FOREACH(i, l)
                erase_and_freep(i);

        తిరిగి mfree(l);
}

శూన్య strv_free_many(అక్షరం ***strvs, size_t n) {
        ప్రతిజ్ఞా(strvs || n == 0);

        FOREACH_ARRAY (i, strvs, n)
                strv_free(*i);

        విడుదల(strvs);
}

అక్షరం** strv_copy_n(అక్షరం * స్థిరరాశి *l, size_t n) {
        _cleanup_strv_free_ అక్షరం **ఫలితం = ఖాళీ;
        అక్షరం **k;

        ఫలితం = new(అక్షరం*, MIN(strv_నిడివి(l), n) + 1);
        ఒకవేళ (!ఫలితం)
                తిరిగి ఖాళీ;

        k = ఫలితం;
        STRV_FOREACH(i, l) {
                ఒకవేళ (n == 0)
                        ఆపు;

                *k = strdup(*i);
                ఒకవేళ (!*k)
                        తిరిగి ఖాళీ;
                k++;

                ఒకవేళ (n != SIZE_MAX)
                        n--;
        }

        *k = ఖాళీ;
        తిరిగి TAKE_PTR(ఫలితం);
}

పూర్ణాంకం strv_copy_unless_ఖాళీలు(అక్షరం * స్థిరరాశి *l, అక్షరం ***ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (strv_isempty(l)) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        అక్షరం **copy = strv_copy(l);
        ఒకవేళ (!copy)
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(copy);
        తిరిగి 1;
}

size_t strv_నిడివి(అక్షరం * స్థిరరాశి *l) {
        size_t n = 0;

        STRV_FOREACH(i, l)
                n++;

        తిరిగి n;
}

అక్షరం** strv_new_ap(స్థిరరాశి అక్షరం *x, va_list ap) {
        _cleanup_strv_free_ అక్షరం **a = ఖాళీ;
        size_t n = 0, i = 0;
        va_list aq;

        /* As a special trick we ignore all listed strings that equal
         * STRV_IGNORE. This is supposed to be used with the
         * STRV_IFNOTNULL() macro to include possibly NULL strings in
         * the string list. */

        va_copy(aq, ap);
        కొరకు (స్థిరరాశి అక్షరం *s = x; s; s = va_arg(aq, స్థిరరాశి అక్షరం*)) {
                ఒకవేళ (s == STRV_IGNORE)
                        కొనసాగు;

                n++;
        }
        va_end(aq);

        a = new(అక్షరం*, n+1);
        ఒకవేళ (!a)
                తిరిగి ఖాళీ;

        కొరకు (స్థిరరాశి అక్షరం *s = x; s; s = va_arg(ap, స్థిరరాశి అక్షరం*)) {
                ఒకవేళ (s == STRV_IGNORE)
                        కొనసాగు;

                a[i] = strdup(s);
                ఒకవేళ (!a[i])
                        తిరిగి ఖాళీ;

                i++;
        }

        a[i] = ఖాళీ;

        తిరిగి TAKE_PTR(a);
}

అక్షరం** strv_new_అంతర్గత(స్థిరరాశి అక్షరం *x, ...) {
        అక్షరం **r;
        va_list ap;

        va_start(ap, x);
        r = strv_new_ap(x, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం strv_extend_strv(అక్షరం ***a, అక్షరం * స్థిరరాశి *b, bool filter_duplicates) {
        size_t p, q, i = 0;

        ప్రతిజ్ఞా(a);

        q = strv_నిడివి(b);
        ఒకవేళ (q == 0)
                తిరిగి 0;

        p = strv_నిడివి(*a);
        ఒకవేళ (p >= SIZE_MAX - q)
                తిరిగి -ENOMEM;

        అక్షరం **t = reallocarray(*a, GREEDY_ALLOC_విడత_పైకి(p + q + 1), పరిమాణం(అక్షరం *));
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        t[p] = ఖాళీ;
        *a = t;

        STRV_FOREACH(s, b) {
                ఒకవేళ (filter_duplicates && strv_కలిగియుంది(t, *s))
                        కొనసాగు;

                t[p+i] = strdup(*s);
                ఒకవేళ (!t[p+i])
                        వెళ్ళు rollback;

                i++;
                t[p+i] = ఖాళీ;
        }

        ప్రతిజ్ఞా(i <= q);

        తిరిగి (పూర్ణాంకం) i;

rollback:
        free_many_charp(t + p, i);
        t[p] = ఖాళీ;
        తిరిగి -ENOMEM;
}

పూర్ణాంకం strv_extend_strv_consume(అక్షరం ***a, అక్షరం **b, bool filter_duplicates) {
        _cleanup_strv_free_ అక్షరం **b_consume = b;
        size_t p, q, i;

        ప్రతిజ్ఞా(a);

        q = strv_నిడివి(b);
        ఒకవేళ (q == 0)
                తిరిగి 0;

        p = strv_నిడివి(*a);
        ఒకవేళ (p == 0) {
                strv_free_and_replace(*a, b_consume);

                ఒకవేళ (filter_duplicates)
                        strv_uniq(*a);

                తిరిగి strv_నిడివి(*a);
        }

        ఒకవేళ (p >= SIZE_MAX - q)
                తిరిగి -ENOMEM;

        అక్షరం **t = reallocarray(*a, GREEDY_ALLOC_విడత_పైకి(p + q + 1), పరిమాణం(అక్షరం *));
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        t[p] = ఖాళీ;
        *a = t;

        ఒకవేళ (!filter_duplicates) {
                *mempcpy_typesafe(t + p, b, q) = ఖాళీ;
                i = q;
        } లేకపోతే {
                i = 0;

                STRV_FOREACH(s, b) {
                        ఒకవేళ (strv_కలిగియుంది(t, *s)) {
                                విడుదల(*s);
                                కొనసాగు;
                        }

                        t[p+i] = *s;

                        i++;
                        t[p+i] = ఖాళీ;
                }
        }

        ప్రతిజ్ఞా(i <= q);

        b_consume = mfree(b_consume);

        తిరిగి (పూర్ణాంకం) i;
}

పూర్ణాంకం strv_extend_strv_biconcat(అక్షరం ***a, స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం* స్థిరరాశి *b, స్థిరరాశి అక్షరం *suffix) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);

        STRV_FOREACH(s, b) {
                అక్షరం *v;

                v = strjoin(strempty(ఉపసర్గ), *s, suffix);
                ఒకవేళ (!v)
                        తిరిగి -ENOMEM;

                r = strv_consume(a, v);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం strv_విభజించు_newlines_నిండిన(అక్షరం ***ret, స్థిరరాశి అక్షరం *s, వెలికితీయి_Flags flags) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(s);

        /* Special version of strv_split_full() that splits on newlines and
         * suppresses an empty string at the end. */

        r = strv_విభజించు_నిండిన(&l, s, NEWLINE, flags);
        ఒకవేళ (r < 0)
                తిరిగి r;

        n = strv_నిడివి(l);
        ఒకవేళ (n > 0 && isempty(l[n - 1])) {
                l[n - 1] = mfree(l[n - 1]);
                n--;
        }

        *ret = TAKE_PTR(l);
        తిరిగి n;
}

అక్షరం** strv_విభజించు_newlines(స్థిరరాశి అక్షరం *s) {
        అక్షరం **ret;

        ఒకవేళ (strv_విభజించు_newlines_నిండిన(&ret, s, 0) < 0)
                తిరిగి ఖాళీ;

        తిరిగి ret;
}

పూర్ణాంకం strv_విభజించు_నిండిన(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        size_t n = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(s);

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *మాట = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&s, &మాట, separators, flags);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (!GREEDY_REALLOC(l, n + 2))
                        తిరిగి -ENOMEM;

                l[n++] = TAKE_PTR(మాట);
                l[n] = ఖాళీ;
        }

        ఒకవేళ (!l) {
                l = new0(అక్షరం*, 1);
                ఒకవేళ (!l)
                        తిరిగి -ENOMEM;
        }

        *t = TAKE_PTR(l);

        తిరిగి (పూర్ణాంకం) n;
}

అక్షరం** strv_విభజించు(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators) {
        అక్షరం **ret;

        ఒకవేళ (strv_విభజించు_నిండిన(&ret, s, separators, వెలికితీయి_RETAIN_ESCAPE) < 0)
                తిరిగి ఖాళీ;

        తిరిగి ret;
}

పూర్ణాంకం strv_విభజించు_and_extend_నిండిన(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, bool filter_duplicates, వెలికితీయి_Flags flags) {
        అక్షరం **l;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(s);

        r = strv_విభజించు_నిండిన(&l, s, separators, flags);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = strv_extend_strv_consume(t, l, filter_duplicates);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి (పూర్ణాంకం) strv_నిడివి(*t);
}

పూర్ణాంకం strv_విభజించు_and_extend(అక్షరం ***t, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separators, bool filter_duplicates) {
        తిరిగి strv_విభజించు_and_extend_నిండిన(t, s, separators, filter_duplicates, 0);
}

పూర్ణాంకం strv_విభజించు_colon_జతలు(అక్షరం ***t, స్థిరరాశి అక్షరం *s) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        size_t n = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(s);

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *మొదటి = ఖాళీ, *రెండవ = ఖాళీ, *tuple = ఖాళీ, *రెండవ_or_ఖాళీలు = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&s, &tuple, ఖాళీ, వెలికితీయి_UNQUOTE|వెలికితీయి_RETAIN_ESCAPE);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                స్థిరరాశి అక్షరం *p = tuple;
                r = వెలికితీయి_many_పదాలు(&p, ":", వెలికితీయి_CUNESCAPE|వెలికితీయి_UNESCAPE_SEPARATORS,
                                       &మొదటి, &రెండవ);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        కొనసాగు;
                /* Enforce that at most 2 colon-separated words are contained in each group */
                ఒకవేళ (!isempty(p))
                        తిరిగి -EINVAL;

                రెండవ_or_ఖాళీలు = strdup(strempty(రెండవ));
                ఒకవేళ (!రెండవ_or_ఖాళీలు)
                        తిరిగి -ENOMEM;

                ఒకవేళ (!GREEDY_REALLOC(l, n + 3))
                        తిరిగి -ENOMEM;

                l[n++] = TAKE_PTR(మొదటి);
                l[n++] = TAKE_PTR(రెండవ_or_ఖాళీలు);

                l[n] = ఖాళీ;
        }

        ఒకవేళ (!l) {
                l = new0(అక్షరం*, 1);
                ఒకవేళ (!l)
                        తిరిగి -ENOMEM;
        }

        *t = TAKE_PTR(l);

        తిరిగి (పూర్ణాంకం) n;
}

అక్షరం* strv_join_నిండిన(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *separator, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        అక్షరం *r, *e;
        size_t n, k, m;

        ఒకవేళ (!separator)
                separator = " ";

        k = అక్షరశ్రేణి_పొడవు(separator);
        m = strlen_ptr(ఉపసర్గ);

        n = 0;
        STRV_FOREACH(s, l) {
                ఒకవేళ (s != l)
                        n += k;

                n += m + అక్షరశ్రేణి_పొడవు(*s);
        }

        r = new(అక్షరం, n+1);
        ఒకవేళ (!r)
                తిరిగి ఖాళీ;

        e = r;
        STRV_FOREACH(s, l) {
                ఒకవేళ (s != l)
                        e = stpcpy(e, separator);

                ఒకవేళ (ఉపసర్గ)
                        e = stpcpy(e, ఉపసర్గ);

                e = stpcpy(e, *s);
        }

        *e = 0;

        తిరిగి r;
}

పూర్ణాంకం strv_నెట్టు_with_కొలత(అక్షరం ***l, size_t *n, అక్షరం *విలువ) {
        /* n is a pointer to a variable to store the size of l.
         * If not given (i.e. n is NULL or *n is SIZE_MAX), size will be calculated using strv_length().
         * If n is not NULL, the size after the push will be returned.
         * If value is empty, no action is taken and *n is not set. */

        ప్రతిజ్ఞా(l);
        సూచిక_MAY_BE_NULL(n);

        ఒకవేళ (!విలువ)
                తిరిగి 0;

        size_t కొలత = n ? *n : SIZE_MAX;
        ఒకవేళ (కొలత == SIZE_MAX)
                కొలత = strv_నిడివి(*l);

        /* Check for overflow */
        ఒకవేళ (కొలత > SIZE_MAX-2)
                తిరిగి -ENOMEM;

        అక్షరం **c = reallocarray(*l, GREEDY_ALLOC_విడత_పైకి(కొలత + 2), పరిమాణం(అక్షరం*));
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        c[కొలత] = విలువ;
        c[కొలత+1] = ఖాళీ;

        *l = c;
        ఒకవేళ (n)
                *n = కొలత + 1;
        తిరిగి 0;
}

పూర్ణాంకం strv_నెట్టు_pair(అక్షరం ***l, అక్షరం *a, అక్షరం *b) {
        అక్షరం **c;
        size_t n;

        ప్రతిజ్ఞా(l);

        ఒకవేళ (!a && !b)
                తిరిగి 0;

        n = strv_నిడివి(*l);

        /* Check for overflow */
        ఒకవేళ (n > SIZE_MAX-3)
                తిరిగి -ENOMEM;

        /* increase and check for overflow */
        c = reallocarray(*l, GREEDY_ALLOC_విడత_పైకి(n + !!a + !!b + 1), పరిమాణం(అక్షరం*));
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        ఒకవేళ (a)
                c[n++] = a;
        ఒకవేళ (b)
                c[n++] = b;
        c[n] = ఖాళీ;

        *l = c;
        తిరిగి 0;
}

పూర్ణాంకం strv_చొప్పించు(అక్షరం ***l, size_t position, అక్షరం *విలువ) {
        అక్షరం **c;
        size_t n, m;

        ప్రతిజ్ఞా(l);

        ఒకవేళ (!విలువ)
                తిరిగి 0;

        n = strv_నిడివి(*l);
        position = MIN(position, n);

        /* check for overflow and increase */
        ఒకవేళ (n > SIZE_MAX - 2)
                తిరిగి -ENOMEM;
        m = n + 2;

        c = reallocarray(*l, GREEDY_ALLOC_విడత_పైకి(m), పరిమాణం(అక్షరం*));
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        ఒకవేళ (n > position)
                memmove(c + position + 1, c + position, (n - position) * పరిమాణం(అక్షరం*));

        c[position] = విలువ;
        c[n + 1] = ఖాళీ;

        *l = c;
        తిరిగి 0;
}

పూర్ణాంకం strv_consume_with_కొలత(అక్షరం ***l, size_t *n, అక్షరం *విలువ) {
        పూర్ణాంకం r;

        r = strv_నెట్టు_with_కొలత(l, n, విలువ);
        ఒకవేళ (r < 0)
                విడుదల(విలువ);

        తిరిగి r;
}

పూర్ణాంకం strv_consume_pair(అక్షరం ***l, అక్షరం *a, అక్షరం *b) {
        పూర్ణాంకం r;

        r = strv_నెట్టు_pair(l, a, b);
        ఒకవేళ (r < 0) {
                విడుదల(a);
                విడుదల(b);
        }

        తిరిగి r;
}

పూర్ణాంకం strv_consume_prepend(అక్షరం ***l, అక్షరం *విలువ) {
        పూర్ణాంకం r;

        r = strv_నెట్టు_prepend(l, విలువ);
        ఒకవేళ (r < 0)
                విడుదల(విలువ);

        తిరిగి r;
}

పూర్ణాంకం strv_prepend(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ) {
        అక్షరం *v;

        ఒకవేళ (!విలువ)
                తిరిగి 0;

        v = strdup(విలువ);
        ఒకవేళ (!v)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume_prepend(l, v);
}

పూర్ణాంకం strv_extend_with_కొలత(అక్షరం ***l, size_t *n, స్థిరరాశి అక్షరం *విలువ) {
        అక్షరం *v;

        ఒకవేళ (!విలువ)
                తిరిగి 0;

        v = strdup(విలువ);
        ఒకవేళ (!v)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume_with_కొలత(l, n, v);
}

పూర్ణాంకం strv_extend_many_అంతర్గత(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ, ...) {
        va_list ap;
        size_t n, m;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(l);

        m = n = strv_నిడివి(*l);

        r = 0;
        va_start(ap, విలువ);
        కొరకు (స్థిరరాశి అక్షరం *s = విలువ; s != సూచిక_MAX; s = va_arg(ap, స్థిరరాశి అక్షరం*)) {
                ఒకవేళ (!s)
                        కొనసాగు;

                ఒకవేళ (m > SIZE_MAX-1) { /*అధికప్రవాహం*/
                        r = -ENOMEM;
                        ఆపు;
                }
                m++;
        }
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (m > SIZE_MAX-1)
                తిరిగి -ENOMEM;

        అక్షరం **c = reallocarray(*l, GREEDY_ALLOC_విడత_పైకి(m+1), పరిమాణం(అక్షరం*));
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;
        *l = c;

        r = 0;
        size_t i = n;
        va_start(ap, విలువ);
        కొరకు (స్థిరరాశి అక్షరం *s = విలువ; s != సూచిక_MAX; s = va_arg(ap, స్థిరరాశి అక్షరం*)) {
                ఒకవేళ (!s)
                        కొనసాగు;

                c[i] = strdup(s);
                ఒకవేళ (!c[i]) {
                        r = -ENOMEM;
                        ఆపు;
                }
                i++;
        }
        va_end(ap);

        ఒకవేళ (r < 0) {
                /* rollback on error */
                కొరకు (size_t j = n; j < i; j++)
                        c[j] = mfree(c[j]);
                తిరిగి r;
        }

        c[i] = ఖాళీ;
        తిరిగి 0;
}

అక్షరం** strv_uniq(అక్షరం **l) {
        /* Drops duplicate entries. The first identical string will be
         * kept, the others dropped */

        STRV_FOREACH(i, l)
                strv_remove(i+1, *i);

        తిరిగి l;
}

bool strv_is_uniq(అక్షరం * స్థిరరాశి *l) {
        STRV_FOREACH(i, l)
                ఒకవేళ (strv_కలిగియుంది(i+1, *i))
                        తిరిగి false;

        తిరిగి true;
}

అక్షరం** strv_remove(అక్షరం **l, స్థిరరాశి అక్షరం *s) {
        ఒకవేళ (!l)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(s);

        /* Drops every occurrence of s in the string list, edits in-place. */

        అక్షరం **f, **t;
        కొరకు (f = t = l; *f; f++)
                ఒకవేళ (streq(*f, s))
                        విడుదల(*f);
                లేకపోతే
                        *(t++) = *f;

        *t = ఖాళీ;
        తిరిగి l;
}

అక్షరం** strv_remove_strv(అక్షరం **l, అక్షరం *స్థిరరాశి*ll) {

        ఒకవేళ (strv_isempty(l))
                తిరిగి l;

        STRV_FOREACH(i, ll)
                strv_remove(l, *i);

        తిరిగి l;
}

bool strv_overlap(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) {
        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        STRV_FOREACH(i, a)
                ఒకవేళ (strv_కలిగియుంది(b, *i))
                        తిరిగి true;

        తిరిగి false;
}

స్థిర పూర్ణాంకం str_compare(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) {
        /* This is called from qsort()s inner loops. Correctly implemented qsort will never pass NULL so we
           just suppress the check via POINTER_MAY_BE_NULL instead of assert() to avoid the runtime cost. */
        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        తిరిగి strcmp(*a, *b);
}

అక్షరం** strv_క్రమీకరించు(అక్షరం **l) {
        typesafe_qsort(l, strv_నిడివి(l), str_compare);
        తిరిగి l;
}

అక్షరం** strv_క్రమీకరించు_uniq_నిండిన(అక్షరం **l, పూర్ణాంకం (*compare)(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b)) {
        ప్రతిజ్ఞా(compare);

        ఒకవేళ (strv_isempty(l))
                తిరిగి l;

        /* Sorts with the given comparison function, then drops entries that compare equal to the previous
         * one. The comparison function hence must only return 0 for entries that are to be merged. */

        typesafe_qsort(l, strv_నిడివి(l), compare);

        అక్షరం **తోక_2 = l + 1;
        STRV_FOREACH(i, l + 1)
                ఒకవేళ (compare(i, తోక_2 - 1) == 0)
                        విడుదల(*i);
                లేకపోతే
                        *(తోక_2++) = *i;

        *తోక_2 = ఖాళీ;
        తిరిగి l;
}

అక్షరం** strv_క్రమీకరించు_uniq(అక్షరం **l) {
        తిరిగి strv_క్రమీకరించు_uniq_నిండిన(l, str_compare);
}

పూర్ణాంకం strv_compare(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) {
        పూర్ణాంకం r;

        సూచిక_MAY_BE_NULL(a);
        సూచిక_MAY_BE_NULL(b);

        ఒకవేళ (strv_isempty(a)) {
                ఒకవేళ (strv_isempty(b))
                        తిరిగి 0;
                లేకపోతే
                        తిరిగి -1;
        }

        ఒకవేళ (strv_isempty(b))
                తిరిగి 1;

        కొరకు ( ; *a || *b; ++a, ++b) {
                r = strcmp_ptr(*a, *b);
                ఒకవేళ (r != 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

bool strv_equal_ignore_క్రమం(అక్షరం * స్థిరరాశి *a, అక్షరం * స్థిరరాశి *b) {

        /* Just like strv_equal(), but doesn't care about the order of elements or about redundant entries
         * (i.e. it's even ok if the number of entries in the array differ, as long as the difference just
         * consists of repetitions). */

        ఒకవేళ (a == b)
                తిరిగి true;

        STRV_FOREACH(i, a)
                ఒకవేళ (!strv_కలిగియుంది(b, *i))
                        తిరిగి false;

        STRV_FOREACH(i, b)
                ఒకవేళ (!strv_కలిగియుంది(a, *i))
                        తిరిగి false;

        తిరిగి true;
}

శూన్య strv_print_నిండిన(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        STRV_FOREACH(s, l)
                రాయి("%s%s\n", strempty(ఉపసర్గ), *s);
}

పూర్ణాంకం strv_extendf_with_కొలత(అక్షరం ***l, size_t *n, స్థిరరాశి అక్షరం *format, ...) {
        va_list ap;
        అక్షరం *x;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = vasprintf(&x, format, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume_with_కొలత(l, n, x);
}

పూర్ణాంకం strv_extend_joined_with_కొలత_sentinel(అక్షరం ***l, size_t *n, ...) {
        va_list ap;

        va_start(ap, n);
        అక్షరం *x = strextendv_with_separator(/* x= */ ఖాళీ, /* separator= */ ఖాళీ, ap);
        va_end(ap);
        ఒకవేళ (!x)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume_with_కొలత(l, n, x);
}

అక్షరం* startswith_strv_అంతర్గత(స్థిరరాశి అక్షరం *s, అక్షరం * స్థిరరాశి *l) {
        STRV_FOREACH(i, l) {
                అక్షరం *దొరికింది = (అక్షరం*) startswith(s, *i);
                ఒకవేళ (దొరికింది)
                        తిరిగి దొరికింది;
        }

        తిరిగి ఖాళీ;
}

అక్షరం* endswith_strv_అంతర్గత(స్థిరరాశి అక్షరం *s, అక్షరం * స్థిరరాశి *l) {
        STRV_FOREACH(i, l) {
                అక్షరం *దొరికింది = (అక్షరం*) endswith(s, *i);
                ఒకవేళ (దొరికింది)
                        తిరిగి దొరికింది;
        }

        తిరిగి ఖాళీ;
}

అక్షరం** strv_తిరగేయి(అక్షరం **l) {
        size_t n;

        n = strv_నిడివి(l);
        ఒకవేళ (n <= 1)
                తిరిగి l;

        కొరకు (size_t i = 0; i < n / 2; i++)
                మార్పిడి_TWO(l[i], l[n-1-i]);

        తిరిగి l;
}

అక్షరం** strv_shell_escape(అక్షరం **l, స్థిరరాశి అక్షరం *bad) {
        /* Escapes every character in every string in l that is in bad,
         * edits in-place, does not roll-back on error. */

        STRV_FOREACH(s, l) {
                అక్షరం *v;

                v = shell_escape(*s, bad);
                ఒకవేళ (!v)
                        తిరిగి ఖాళీ;

                free_and_replace(*s, v);
        }

        తిరిగి l;
}

bool strv_fnmatch_నిండిన(
                అక్షరం* స్థిరరాశి* patterns,
                స్థిరరాశి అక్షరం *s,
                పూర్ణాంకం flags,
                size_t *ret_matched_pos) {

        ప్రతిజ్ఞా(s);

        ఒకవేళ (patterns)
                కొరకు (size_t i = 0; patterns[i]; i++)
                        /* NB: We treat all fnmatch() errors as equivalent to FNM_NOMATCH, i.e. if fnmatch() fails to
                         * process the pattern for some reason we'll consider this equivalent to non-matching. */
                        ఒకవేళ (fnmatch(patterns[i], s, flags) == 0) {
                                ఒకవేళ (ret_matched_pos)
                                        *ret_matched_pos = i;
                                తిరిగి true;
                        }

        ఒకవేళ (ret_matched_pos)
                *ret_matched_pos = SIZE_MAX;

        తిరిగి false;
}

అక్షరం** strv_skip(అక్షరం **l, size_t n) {
        అయితే (n > 0) {
                ఒకవేళ (strv_isempty(l))
                        తిరిగి ఖాళీ;

                l++, n--;
        }

        /* To simplify callers, always return NULL instead of a zero-item array. */
        ఒకవేళ (strv_isempty(l))
                తిరిగి ఖాళీ;
        తిరిగి l;
}

పూర్ణాంకం strv_extend_n(అక్షరం ***l, స్థిరరాశి అక్షరం *విలువ, size_t n) {
        size_t i, k;
        అక్షరం **nl;

        ప్రతిజ్ఞా(l);

        ఒకవేళ (!విలువ)
                తిరిగి 0;
        ఒకవేళ (n == 0)
                తిరిగి 0;

        /* Adds the value n times to l */

        k = strv_నిడివి(*l);
        ఒకవేళ (n >= SIZE_MAX - k)
                తిరిగి -ENOMEM;

        nl = reallocarray(*l, GREEDY_ALLOC_విడత_పైకి(k + n + 1), పరిమాణం(అక్షరం *));
        ఒకవేళ (!nl)
                తిరిగి -ENOMEM;

        *l = nl;

        కొరకు (i = k; i < k + n; i++) {
                nl[i] = strdup(విలువ);
                ఒకవేళ (!nl[i])
                        వెళ్ళు rollback;
        }
        nl[i] = ఖాళీ;

        తిరిగి 0;

rollback:
        కొరకు (size_t j = k; j < i; j++)
                విడుదల(nl[j]);
        nl[k] = ఖాళీ;

        తిరిగి -ENOMEM;
}

పూర్ణాంకం strv_extend_assignment(అక్షరం ***l, స్థిరరాశి అక్షరం *lhs, స్థిరరాశి అక్షరం *rhs) {
        అక్షరం *j;

        ప్రతిజ్ఞా(l);
        ప్రతిజ్ఞా(lhs);

        ఒకవేళ (!rhs) /* value is optional, in which case we suppress the field */
                తిరిగి 0;

        j = strjoin(lhs, "=", rhs);
        ఒకవేళ (!j)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume(l, j);
}

పూర్ణాంకం fputstrv(FILE *f, అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *separator, bool *space) {
        bool b = false;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);

        /* Like fputs(), but for strv, and with a less stupid argument order */

        ఒకవేళ (!space)
                space = &b;

        STRV_FOREACH(s, l) {
                r = fputs_with_separator(f, *s, separator, space);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

శూన్య string_strv_hashmap_remove(Hashmap *h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        ప్రతిజ్ఞా(కీలకం);

        ఒకవేళ (విలువ) {
                అక్షరం **l = hashmap_get(h, కీలకం);
                ఒకవేళ (!l)
                        తిరిగి;

                strv_remove(l, విలువ);
                ఒకవేళ (!strv_isempty(l))
                        తిరిగి;
        }

        వాడని _cleanup_free_ అక్షరం *కీలకం_free = ఖాళీ;
        strv_free(hashmap_remove2(h, కీలకం, (శూన్య**) &కీలకం_free));
}

శూన్య string_strv_ordered_hashmap_remove(OrderedHashmap *h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        string_strv_hashmap_remove(PLAIN_HASHMAP(h), కీలకం, విలువ);
}

స్థిర పూర్ణాంకం string_strv_hashmap_put_అంతర్గత(Hashmap *h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        అక్షరం **l;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(విలువ);

        l = hashmap_get(h, కీలకం);
        ఒకవేళ (l) {
                /* A list for this key already exists, let's append to it if it is not listed yet */
                ఒకవేళ (strv_కలిగియుంది(l, విలువ))
                        తిరిగి 0;

                r = strv_extend(&l, విలువ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                assert_se(hashmap_update(h, కీలకం, l) >= 0);
        } లేకపోతే {
                /* No list for this key exists yet, create one */
                _cleanup_strv_free_ అక్షరం **l2 = ఖాళీ;
                _cleanup_free_ అక్షరం *t = ఖాళీ;

                t = strdup(కీలకం);
                ఒకవేళ (!t)
                        తిరిగి -ENOMEM;

                r = strv_extend(&l2, విలువ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = hashmap_put(h, t, l2);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                TAKE_PTR(t);
                TAKE_PTR(l2);
        }

        తిరిగి 1;
}

పూర్ణాంకం string_strv_hashmap_put(Hashmap **h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(విలువ);

        r = hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి string_strv_hashmap_put_అంతర్గత(*h, కీలకం, విలువ);
}

పూర్ణాంకం string_strv_ordered_hashmap_put(OrderedHashmap **h, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(విలువ);

        r = ordered_hashmap_ensure_allocated(h, &string_hash_ops_free_strv_free);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి string_strv_hashmap_put_అంతర్గత(PLAIN_HASHMAP(*h), కీలకం, విలువ);
}

స్థిర పూర్ణాంకం rebreak_flush_పంక్తి(
                అక్షరం ***broken,
                స్థిరరాశి అక్షరం *ప్రారంభం,
                స్థిరరాశి అక్షరం *చివర,
                bool లోపల_ఉపసర్గ,
                స్థిరరాశి అక్షరం *whitespace_begin,
                స్థిరరాశి అక్షరం *whitespace_చివర) {

        ప్రతిజ్ఞా(broken);
        ప్రతిజ్ఞా(ప్రారంభం);
        ప్రతిజ్ఞా(చివర >= ప్రారంభం);

        ఒకవేళ (లోపల_ఉపసర్గ) /* Never seen anything non-whitespace? Generate empty line! */
                తిరిగి strv_extend(broken, "");

        ఒకవేళ (whitespace_begin && !whitespace_చివర) /* Ends in whitespace? Chop it off! */
                చివర = whitespace_begin;

        _cleanup_free_ అక్షరం *truncated = strndup(ప్రారంభం, చివర - ప్రారంభం);
        ఒకవేళ (!truncated)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume(broken, TAKE_PTR(truncated));
}

పూర్ణాంకం strv_rebreak_పంక్తులు(అక్షరం **l, size_t width, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **broken = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Implements a simple UTF-8 line breaking algorithm
         *
         * Goes through all entries in *l, and line-breaks each line that is longer than the specified
         * character width. Breaks at the end of words/beginning of whitespace. Lines that do not contain whitespace are not
         * broken. Retains whitespace at beginning of lines, removes it at end of lines. Newlines embedded
         * in an entry are hard line breaks, i.e. result in a separate output entry each. */

        ఒకవేళ (width == SIZE_MAX) { /* NOP? */
                broken = strv_copy(l);
                ఒకవేళ (!broken)
                        తిరిగి -ENOMEM;

                *ret = TAKE_PTR(broken);
                తిరిగి 0;
        }

        STRV_FOREACH(i, l) {
                స్థిరరాశి అక్షరం *ప్రారంభం = *i, *whitespace_begin = ఖాళీ, *whitespace_చివర = ఖాళీ;
                bool లోపల_ఉపసర్గ = true; /* still in the whitespace in the beginning of the line? */
                bool after_newline = false; /* did we just break at a newline embedded in the input? */
                size_t w = 0;

                కొరకు (స్థిరరాశి అక్షరం *p = ప్రారంభం; *p != 0; ) {
                        ఒకవేళ (strchr(NEWLINE, *p)) {
                                /* A newline embedded in the input is a hard line break: flush out what we
                                 * collected so far as its own line, and continue with a new one right after
                                 * it. */
                                r = rebreak_flush_పంక్తి(&broken, ప్రారంభం, p, లోపల_ఉపసర్గ, whitespace_begin, whitespace_చివర);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                p += p[0] == '\r' && p[1] == '\n' ? 2 : 1; /* Treat CRLF as a single break */
                                ప్రారంభం = p;
                                లోపల_ఉపసర్గ = true;
                                after_newline = true;
                                whitespace_begin = whitespace_చివర = ఖాళీ;
                                w = 0;
                                కొనసాగు;
                        } లేకపోతే ఒకవేళ (strchr(WHITESPACE, *p)) {
                                ఒకవేళ (!లోపల_ఉపసర్గ && (!whitespace_begin || whitespace_చివర)) {
                                        whitespace_begin = p;
                                        whitespace_చివర = ఖాళీ;
                                }
                        } లేకపోతే {
                                ఒకవేళ (whitespace_begin && !whitespace_చివర)
                                        whitespace_చివర = p;

                                లోపల_ఉపసర్గ = false;
                        }

                        /* Measure with the same helper utf8_console_width() uses. Our callers mix the
                         * two: sd-varlink-idl.c and verbs.c size their indent with utf8_console_width()
                         * and then break the body here, so if we accepted and measured what that one
                         * refuses we'd be computing a single width from two ideas of what valid UTF-8
                         * is. */
                        పూర్ణాంకం cw = utf8_char_console_వెడల్పు(p);
                        ఒకవేళ (cw < 0)
                                తిరిగి log_debug_errno(cw, "Line to break contains invalid UTF-8, refusing: %m");

                        w += cw;

                        ఒకవేళ (w > width && whitespace_begin && whitespace_చివర) {
                                _cleanup_free_ అక్షరం *truncated = ఖాళీ;

                                truncated = strndup(ప్రారంభం, whitespace_begin - ప్రారంభం);
                                ఒకవేళ (!truncated)
                                        తిరిగి -ENOMEM;

                                r = strv_consume(&broken, TAKE_PTR(truncated));
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                /* Continue with the next line, starting at the first character after the
                                 * whitespace we broke at. Note, that character has not been measured for
                                 * the new line yet, hence reset the width and reprocess it. */
                                p = ప్రారంభం = whitespace_చివర;
                                after_newline = false;
                                whitespace_begin = whitespace_చివర = ఖాళీ;
                                w = 0;
                                కొనసాగు;
                        }

                        p = utf8_తదుపరి_char(p);
                }

                /* Process rest of the line, except if all that's left after the last newline we already
                 * flushed out above is whitespace (possibly nothing): a trailing newline terminates the
                 * last line, it doesn't add a new, empty one. */
                ప్రతిజ్ఞా(ప్రారంభం);
                ఒకవేళ (after_newline && లోపల_ఉపసర్గ)
                        కొనసాగు;

                r = rebreak_flush_పంక్తి(&broken, ప్రారంభం, ప్రారంభం + అక్షరశ్రేణి_పొడవు(ప్రారంభం), లోపల_ఉపసర్గ, whitespace_begin, whitespace_చివర);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(broken);
        తిరిగి 0;
}

అక్షరం** strv_filter_ఉపసర్గ(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *ఉపసర్గ) {

        /* Allocates a copy of 'l', but only copies over entries starting with 'prefix' */

        ఒకవేళ (isempty(ఉపసర్గ))
                తిరిగి strv_copy(l);

        _cleanup_strv_free_ అక్షరం **f = ఖాళీ;
        size_t sz = 0;

        STRV_FOREACH(i, l) {
                ఒకవేళ (!startswith(*i, ఉపసర్గ))
                        కొనసాగు;

                ఒకవేళ (strv_extend_with_కొలత(&f, &sz, *i) < 0)
                        తిరిగి ఖాళీ;
        }

        తిరిగి TAKE_PTR(f);
}

స్థిరరాశి అక్షరం* స్థిరరాశి strv_ఖాళీలు[] = { ఖాళీ };
