/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "syslog-util.h"
#అన్తర్భావ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#నిర్వచించు VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(శూన్య) {
        దీర్ఘ l = sysconf(_SC_ARG_MAX);
        ప్రతిజ్ఞా(l > 0);
        తిరిగి (size_t) l;
}

స్థిర bool env_name_is_valid_n(స్థిరరాశి అక్షరం *e, size_t n) {

        ఒకవేళ (n == SIZE_MAX)
                n = strlen_ptr(e);

        ఒకవేళ (n == 0)
                తిరిగి false;

        ప్రతిజ్ఞా(e);

        ఒకవేళ (ascii_isdigit(e[0]))
                తిరిగి false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ఒకవేళ (_unlikely_(n > sc_arg_max() - 2))
                తిరిగి false;

        కొరకు (స్థిరరాశి అక్షరం *p = e; p < e + n; p++)
                ఒకవేళ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        తిరిగి false;

        తిరిగి true;
}

bool env_name_is_valid(స్థిరరాశి అక్షరం *e) {
        తిరిగి env_name_is_valid_n(e, SIZE_MAX);
}

bool env_విలువ_is_valid(స్థిరరాశి అక్షరం *e) {
        ఒకవేళ (!e)
                తిరిగి false;

        ఒకవేళ (!utf8_is_valid(e))
                తిరిగి false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ఒకవేళ (_unlikely_(అక్షరశ్రేణి_పొడవు(e) > sc_arg_max() - 3))
                తిరిగి false;

        తిరిగి true;
}

bool env_assignment_is_valid(స్థిరరాశి అక్షరం *e) {
        స్థిరరాశి అక్షరం *eq;

        ప్రతిజ్ఞా(e);

        eq = strchr(e, '=');
        ఒకవేళ (!eq)
                తిరిగి false;

        ఒకవేళ (!env_name_is_valid_n(e, eq - e))
                తిరిగి false;

        ఒకవేళ (!env_విలువ_is_valid(eq + 1))
                తిరిగి false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ఒకవేళ (_unlikely_(అక్షరశ్రేణి_పొడవు(e) > sc_arg_max() - 1))
                తిరిగి false;

        తిరిగి true;
}

bool strv_env_is_valid(అక్షరం * స్థిరరాశి *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ఒకవేళ (!env_assignment_is_valid(*p))
                        తిరిగి false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ఒకవేళ (strneq(*p, *q, k) && (*q)[k] == '=')
                                తిరిగి false;
        }

        తిరిగి true;
}

bool strv_env_name_is_valid(అక్షరం * స్థిరరాశి *l) {
        STRV_FOREACH(p, l) {
                ఒకవేళ (!env_name_is_valid(*p))
                        తిరిగి false;

                ఒకవేళ (strv_కలిగియుంది(p + 1, *p))
                        తిరిగి false;
        }

        తిరిగి true;
}

bool strv_env_name_or_assignment_is_valid(అక్షరం * స్థిరరాశి *l) {
        STRV_FOREACH(p, l) {
                ఒకవేళ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        తిరిగి false;

                ఒకవేళ (strv_కలిగియుంది(p + 1, *p))
                        తిరిగి false;
        }

        తిరిగి true;
}

స్థిర పూర్ణాంకం env_append(అక్షరం **e, అక్షరం ***k, అక్షరం **a) {
        ప్రతిజ్ఞా(e);
        ప్రతిజ్ఞా(k);
        ప్రతిజ్ఞా(*k >= e);

        ఒకవేళ (!a)
                తిరిగి 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        కొరకు (; *a; a++) {
                అక్షరం **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ఒకవేళ ((*a)[n] == '=')
                        n++;

                కొరకు (j = e; j < *k; j++)
                        ఒకవేళ (strneq(*j, *a, n))
                                ఆపు;

                c = strdup(*a);
                ఒకవేళ (!c)
                        తిరిగి -ENOMEM;

                ఒకవేళ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ఖాళీ;
                        (*k)++;
                } లేకపోతే
                        free_and_replace(*j, c); /* Override existing item */
        }

        తిరిగి 0;
}

అక్షరం** strv_env_విలీనం(అక్షరం **మొదటి, ...) {
        _cleanup_strv_free_ అక్షరం **merged = ఖాళీ;
        అక్షరం **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_నిడివి(మొదటి);

        va_start(ap, మొదటి);
        కొరకు (;;) {
                అక్షరం **l;

                l = va_arg(ap, అక్షరం**);
                ఒకవేళ (l == సూచిక_MAX)
                        ఆపు;

                n += strv_నిడివి(l);
        }
        va_end(ap);

        k = merged = new(అక్షరం*, n + 1);
        ఒకవేళ (!merged)
                తిరిగి ఖాళీ;
        merged[0] = ఖాళీ;

        ఒకవేళ (env_append(merged, &k, మొదటి) < 0)
                తిరిగి ఖాళీ;

        va_start(ap, మొదటి);
        కొరకు (;;) {
                అక్షరం **l;

                l = va_arg(ap, అక్షరం**);
                ఒకవేళ (l == సూచిక_MAX)
                        ఆపు;

                ఒకవేళ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        తిరిగి ఖాళీ;
                }
        }
        va_end(ap);

        తిరిగి TAKE_PTR(merged);
}

స్థిర bool env_సరిపోలిక(స్థిరరాశి అక్షరం *t, స్థిరరాశి అక్షరం *నమూనా) {
        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(నమూనా);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ఒకవేళ (streq(t, నమూనా))
                తిరిగి true;

        ఒకవేళ (!strchr(నమూనా, '=')) {
                t = startswith(t, నమూనా);

                తిరిగి t && *t == '=';
        }

        తిరిగి false;
}

స్థిర bool env_నమోదు_has_name(స్థిరరాశి అక్షరం *నమోదు_2, స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి అక్షరం *t;

        ప్రతిజ్ఞా(నమోదు_2);
        ప్రతిజ్ఞా(name);

        t = startswith(నమోదు_2, name);
        ఒకవేళ (!t)
                తిరిగి false;

        తిరిగి *t == '=';
}

అక్షరం** strv_env_delete(అక్షరం **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ అక్షరం **t = ఖాళీ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_నిడివి(x);

        t = new(అక్షరం*, n+1);
        ఒకవేళ (!t)
                తిరిగి ఖాళీ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                కొరకు (size_t v = 0; v < n_lists; v++) {
                        అక్షరం **l;

                        l = va_arg(ap, అక్షరం**);
                        STRV_FOREACH(j, l)
                                ఒకవేళ (env_సరిపోలిక(*k, *j))
                                        వెళ్ళు skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ఒకవేళ (!t[i])
                        తిరిగి ఖాళీ;

                i++;
                కొనసాగు;

        skip:
                va_end(ap);
        }

        t[i] = ఖాళీ;

        ప్రతిజ్ఞా(i <= n);

        తిరిగి TAKE_PTR(t);
}

అక్షరం** strv_env_unset(అక్షరం **l, స్థిరరాశి అక్షరం *p) {
        ప్రతిజ్ఞా(p);

        ఒకవేళ (!l)
                తిరిగి ఖాళీ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        అక్షరం **f, **t;
        కొరకు (f = t = l; *f; f++) {
                ఒకవేళ (env_సరిపోలిక(*f, p)) {
                        విడుదల(*f);
                        కొనసాగు;
                }

                *(t++) = *f;
        }

        *t = ఖాళీ;
        తిరిగి l;
}

అక్షరం** strv_env_unset_many_అంతర్గత(అక్షరం **l, ...) {
        ఒకవేళ (!l)
                తిరిగి ఖాళీ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        అక్షరం **f, **t;
        కొరకు (f = t = l; *f; f++) {
                bool దొరికింది = false;
                స్థిరరాశి అక్షరం *p;
                va_list ap;

                va_start(ap, l);

                అయితే ((p = va_arg(ap, స్థిరరాశి అక్షరం*)))
                        ఒకవేళ (env_సరిపోలిక(*f, p)) {
                                దొరికింది = true;
                                ఆపు;
                        }

                va_end(ap);

                ఒకవేళ (దొరికింది) {
                        విడుదల(*f);
                        కొనసాగు;
                }

                *(t++) = *f;
        }

        *t = ఖాళీ;
        తిరిగి l;
}

పూర్ణాంకం strv_env_replace_consume(అక్షరం ***l, అక్షరం *p) {
        స్థిరరాశి అక్షరం *t, *name;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(l);
        ప్రతిజ్ఞా(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ఒకవేళ (!t) {
                విడుదల(p);
                తిరిగి -EINVAL;
        }

        name = strndupa_సురక్షిత(p, t - p);

        STRV_FOREACH(f, *l)
                ఒకవేళ (env_నమోదు_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        తిరిగి 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

పూర్ణాంకం strv_env_replace_strdup(అక్షరం ***l, స్థిరరాశి అక్షరం *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ప్రతిజ్ఞా(l);
        ప్రతిజ్ఞా(assignment);

        అక్షరం *p = strdup(assignment);
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        తిరిగి strv_env_replace_consume(l, p);
}

పూర్ణాంకం strv_env_replace_strdup_passthrough(అక్షరం ***l, స్థిరరాశి అక్షరం *assignment) {
        అక్షరం *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ప్రతిజ్ఞా(l);
        ప్రతిజ్ఞా(assignment);

        ఒకవేళ (strchr(assignment, '=')) {
                ఒకవేళ (!env_assignment_is_valid(assignment))
                        తిరిగి -EINVAL;

                p = strdup(assignment);
        } లేకపోతే {
                ఒకవేళ (!env_name_is_valid(assignment))
                        తిరిగి -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        తిరిగి strv_env_replace_consume(l, p);
}

పూర్ణాంకం strv_env_కేటాయించు(అక్షరం ***l, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        ప్రతిజ్ఞా(l);

        ఒకవేళ (!env_name_is_valid(కీలకం))
                తిరిగి -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ఒకవేళ (!విలువ) {
                strv_env_unset(*l, కీలకం);
                తిరిగి 0;
        }

        అక్షరం *p = strjoin(కీలకం, "=", విలువ);
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        తిరిగి strv_env_replace_consume(l, p);
}

పూర్ణాంకం strv_env_assignf(అక్షరం ***l, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *valuef, ...) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(l);
        ప్రతిజ్ఞా(కీలకం);

        ఒకవేళ (!env_name_is_valid(కీలకం))
                తిరిగి -EINVAL;

        ఒకవేళ (!valuef) {
                strv_env_unset(*l, కీలకం);
                తిరిగి 0;
        }

        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&విలువ, valuef, ap);
        va_end(ap);
        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        అక్షరం *p = strjoin(కీలకం, "=", విలువ);
        ఒకవేళ (!p)
                తిరిగి -ENOMEM;

        తిరిగి strv_env_replace_consume(l, p);
}

పూర్ణాంకం strv_env_కేటాయించు_many(అక్షరం ***l, ...) {
        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(l);

        va_start(ap, l);
        కొరకు (;;) {
                స్థిరరాశి అక్షరం *కీలకం, *విలువ;

                కీలకం = va_arg(ap, స్థిరరాశి అక్షరం *);
                ఒకవేళ (!కీలకం)
                        ఆపు;

                ఒకవేళ (!env_name_is_valid(కీలకం)) {
                        va_end(ap);
                        తిరిగి -EINVAL;
                }

                విలువ = va_arg(ap, స్థిరరాశి అక్షరం *);
                ఒకవేళ (!విలువ) {
                        strv_env_unset(*l, కీలకం);
                        కొనసాగు;
                }

                అక్షరం *p = strjoin(కీలకం, "=", విలువ);
                ఒకవేళ (!p) {
                        va_end(ap);
                        తిరిగి -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ఒకవేళ (r < 0) {
                        va_end(ap);
                        తిరిగి r;
                }
        }
        va_end(ap);

        తిరిగి 0;
}

అక్షరం* strv_env_get_n(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *name, size_t k, ReplaceEnvFlags flags) {
        ప్రతిజ్ఞా(name);

        ఒకవేళ (k == SIZE_MAX)
                k = అక్షరశ్రేణి_పొడవు(name);
        ఒకవేళ (k <= 0)
                తిరిగి ఖాళీ;

        STRV_FOREACH_BACKWARDS(i, l)
                ఒకవేళ (strneq(*i, name, k) && (*i)[k] == '=')
                        తిరిగి (అక్షరం*) *i + k + 1;

        ఒకవేళ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                స్థిరరాశి అక్షరం *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ఒకవేళ (k > sc_arg_max() - 2)
                        తిరిగి ఖాళీ;

                t = strndupa_సురక్షిత(name, k);
                తిరిగి secure_getenv(t);
        };

        తిరిగి ఖాళీ;
}

అక్షరం* strv_env_జతలు_get(అక్షరం **l, స్థిరరాశి అక్షరం *name) {
        అక్షరం *ఫలితం = ఖాళీ;

        ప్రతిజ్ఞా(name);

        STRV_FOREACH_PAIR(కీలకం, విలువ, l)
                ఒకవేళ (streq(*కీలకం, name))
                        ఫలితం = *విలువ;

        తిరిగి ఫలితం;
}

పూర్ణాంకం strv_env_get_merged(అక్షరం **l, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **v = ఖాళీ;
        size_t n = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(కీలకం, విలువ, l) {
                అక్షరం *s;

                s = strjoin(*కీలకం, "=", *విలువ);
                ఒకవేళ (!s)
                        తిరిగి -ENOMEM;

                r = strv_consume_with_కొలత(&v, &n, s);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(v);
        తిరిగి 0;
}

అక్షరం** strv_env_clean_with_callback(అక్షరం **l, శూన్య (*invalid_callback)(స్థిరరాశి అక్షరం *p, శూన్య *userdata), శూన్య *userdata) {
        పూర్ణాంకం k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ఒకవేళ (!env_assignment_is_valid(*p)) {
                        ఒకవేళ (invalid_callback)
                                invalid_callback(*p, userdata);
                        విడుదల(*p);
                        కొనసాగు;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ఒకవేళ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ఆపు;
                        }

                ఒకవేళ (duplicate) {
                        విడుదల(*p);
                        కొనసాగు;
                }

                l[k++] = *p;
        }

        ఒకవేళ (l)
                l[k] = ఖాళీ;

        తిరిగి l;
}

స్థిర పూర్ణాంకం strv_extend_with_నిడివి(అక్షరం ***l, స్థిరరాశి అక్షరం *s, size_t n) {
        అక్షరం *c;

        c = strndup(s, n);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        తిరిగి strv_consume(l, c);
}

స్థిర పూర్ణాంకం strv_env_get_n_validated(
                అక్షరం **env,
                స్థిరరాశి అక్షరం *name,
                size_t l,
                ReplaceEnvFlags flags,
                అక్షరం **ret,              /* points into the env block! do not free! */
                అక్షరం ***unset_variables, /* updated in place */
                అక్షరం ***bad_variables) { /* ditto */

        అక్షరం *e;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(l == 0 || name);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ఒకవేళ (!e && unset_variables) {
                        r = strv_extend_with_నిడివి(unset_variables, name, l);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        } లేకపోతే {
                e = ఖాళీ; /* Resolve invalid variable names the same way as unset ones */

                ఒకవేళ (bad_variables) {
                        r = strv_extend_with_నిడివి(bad_variables, name, l);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        }

        *ret = e;
        తిరిగి !!e;
}

పూర్ణాంకం replace_env_నిండిన(
                స్థిరరాశి అక్షరం *format,
                size_t length,
                అక్షరం **env,
                ReplaceEnvFlags flags,
                అక్షరం **ret,
                అక్షరం ***ret_unset_variables,
                అక్షరం ***ret_bad_variables) {

        గణన {
                మాట_2,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_విలువ,
                ALTERNATE_విలువ,
        } state = మాట_2;

        _cleanup_strv_free_ అక్షరం **unset_variables = ఖాళీ, **bad_variables = ఖాళీ;
        స్థిరరాశి అక్షరం *e, *మాట = format, *test_విలువ = ఖాళీ; /* test_value is initialized to appease gcc */
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        అక్షరం ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        పూర్ణాంకం nest = 0, r;

        ప్రతిజ్ఞా(format);

        ఒకవేళ (length == SIZE_MAX)
                length = అక్షరశ్రేణి_పొడవు(format);

        pu = ret_unset_variables ? &unset_variables : ఖాళీ;
        pb = ret_bad_variables ? &bad_variables : ఖాళీ;

        కొరకు (e = format, i = 0; *e && i < length; e++, i++)
                ఎంపిక (state) {

                స్థితి మాట_2:
                        ఒకవేళ (*e == '$')
                                state = CURLY;
                        ఆపు;

                స్థితి CURLY:
                        ఒకవేళ (*e == '{') {
                                ఒకవేళ (!strextendn(&s, మాట, e-మాట-1))
                                        తిరిగి -ENOMEM;

                                మాట = e-1;
                                state = VARIABLE;
                                nest++;

                        } లేకపోతే ఒకవేళ (*e == '$') {
                                ఒకవేళ (!strextendn(&s, మాట, e-మాట))
                                        తిరిగి -ENOMEM;

                                మాట = e+1;
                                state = మాట_2;

                        } లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ఒకవేళ (!strextendn(&s, మాట, e-మాట-1))
                                        తిరిగి -ENOMEM;

                                మాట = e-1;
                                state = VARIABLE_RAW;

                        } లేకపోతే
                                state = మాట_2;
                        ఆపు;

                స్థితి VARIABLE:
                        ఒకవేళ (*e == '}') {
                                అక్షరం *t;

                                r = strv_env_get_n_validated(env, మాట+2, e-మాట-2, flags, &t, pu, pb);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (!strextend(&s, t))
                                        తిరిగి -ENOMEM;

                                మాట = e+1;
                                state = మాట_2;
                                nest--;
                        } లేకపోతే ఒకవేళ (*e == ':') {
                                ఒకవేళ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - మాట - 2;
                                        state = TEST;
                                } లేకపోతే
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = మాట_2;
                        }
                        ఆపు;

                స్థితి TEST:
                        ఒకవేళ (*e == '-')
                                state = DEFAULT_విలువ;
                        లేకపోతే ఒకవేళ (*e == '+')
                                state = ALTERNATE_విలువ;
                        లేకపోతే {
                                state = మాట_2;
                                ఆపు;
                        }

                        test_విలువ = e+1;
                        ఆపు;

                స్థితి DEFAULT_విలువ: /* fall through */
                స్థితి ALTERNATE_విలువ:
                        ప్రతిజ్ఞా(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ఒకవేళ (*e == '{') {
                                nest++;
                                ఆపు;
                        }

                        ఒకవేళ (*e != '}')
                                ఆపు;

                        nest--;
                        ఒకవేళ (nest == 0) {
                                _cleanup_strv_free_ అక్షరం **u = ఖాళీ, **b = ఖాళీ;
                                _cleanup_free_ అక్షరం *v = ఖాళీ;
                                అక్షరం *t = ఖాళీ;

                                r = strv_env_get_n_validated(env, మాట+2, len, flags, &t, pu, pb);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (t && state == ALTERNATE_విలువ) {
                                        r = replace_env_నిండిన(test_విలువ, e-test_విలువ, env, flags, &v, pu ? &u : ఖాళీ, pb ? &b : ఖాళీ);
                                        ఒకవేళ (r < 0)
                                                తిరిగి r;

                                        t = v;
                                } లేకపోతే ఒకవేళ (!t && state == DEFAULT_విలువ) {
                                        r = replace_env_నిండిన(test_విలువ, e-test_విలువ, env, flags, &v, pu ? &u : ఖాళీ, pb ? &b : ఖాళీ);
                                        ఒకవేళ (r < 0)
                                                తిరిగి r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (!strextend(&s, t))
                                        తిరిగి -ENOMEM;

                                మాట = e+1;
                                state = మాట_2;
                        }
                        ఆపు;

                స్థితి VARIABLE_RAW:
                        ప్రతిజ్ఞా(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ఒకవేళ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                అక్షరం *t = ఖాళీ;

                                r = strv_env_get_n_validated(env, మాట+1, e-మాట-1, flags, &t, &unset_variables, &bad_variables);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (!strextend(&s, t))
                                        తిరిగి -ENOMEM;

                                మాట = e--;
                                i--;
                                state = మాట_2;
                        }
                        ఆపు;
                }

        ఒకవేళ (state == VARIABLE_RAW) {
                అక్షరం *t;

                ప్రతిజ్ఞా(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, మాట+1, e-మాట-1, flags, &t, &unset_variables, &bad_variables);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (!strextend(&s, t))
                        తిరిగి -ENOMEM;

        } లేకపోతే ఒకవేళ (!strextendn(&s, మాట, e-మాట))
                తిరిగి -ENOMEM;

        ఒకవేళ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ఒకవేళ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ఒకవేళ (ret)
                *ret = TAKE_PTR(s);

        తిరిగి 0;
}

పూర్ణాంకం replace_env_argv(
                అక్షరం **argv,
                అక్షరం **env,
                అక్షరం ***ret,
                అక్షరం ***ret_unset_variables,
                అక్షరం ***ret_bad_variables) {

        _cleanup_strv_free_ అక్షరం **n = ఖాళీ, **unset_variables = ఖాళీ, **bad_variables = ఖాళీ;
        size_t k = 0, l;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(!strv_isempty(argv));
        ప్రతిజ్ఞా(ret);

        l = strv_నిడివి(argv);

        n = new(అక్షరం*, l+1);
        ఒకవేళ (!n)
                తిరిగి -ENOMEM;

        STRV_FOREACH(i, argv) {
                స్థిరరాశి అక్షరం *మాట = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ఒకవేళ (మాట[0] == '$' && !లోపల_సమితి(మాట[1], '{', '$')) {
                        _cleanup_strv_free_ అక్షరం **m = ఖాళీ;
                        స్థిరరాశి అక్షరం *name = మాట + 1;
                        అక్షరం *e, **w;
                        size_t q;

                        ఒకవేళ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ఒకవేళ (e)
                                        r = strv_విభజించు_నిండిన(&m, e, WHITESPACE, వెలికితీయి_RELAX|వెలికితీయి_UNQUOTE);
                                లేకపోతే ఒకవేళ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                లేకపోతే
                                        r = 0;
                        } లేకపోతే ఒకవేళ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        లేకపోతే
                                r = 0;
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        q = strv_నిడివి(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, పరిమాణం(అక్షరం*));
                        ఒకవేళ (!w)
                                తిరిగి -ENOMEM;

                        n = w;
                        ఒకవేళ (m) {
                                జ్ఞాపకం_నకలు(n + k, m, (q + 1) * పరిమాణం(అక్షరం*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ఖాళీ;
                        కొనసాగు;
                }

                _cleanup_strv_free_ అక్షరం **u = ఖాళీ, **b = ఖాళీ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_నిండిన(
                                మాట,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ఖాళీ,
                                ret_bad_variables ? &b : ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                n[++k] = ఖాళీ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_unset_variables) {
                strv_క్రమీకరించు_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ఒకవేళ (ret_bad_variables) {
                strv_క్రమీకరించు_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        తిరిగి 0;
}

పూర్ణాంకం getenv_bool(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *e;

        e = getenv(p);
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        తిరిగి parse_boolean(e);
}

పూర్ణాంకం secure_getenv_bool(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *e;

        e = secure_getenv(p);
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        తిరిగి parse_boolean(e);
}

పూర్ణాంకం secure_getenv_uint64(స్థిరరాశి అక్షరం *p, uint64_t *ret) {
        స్థిరరాశి అక్షరం *e;

        ప్రతిజ్ఞా(p);

        e = secure_getenv(p);
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        తిరిగి సురక్షిత_atou64(e, ret);
}

పూర్ణాంకం సమితి_unset_env(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *విలువ, bool overwrite) {
        ప్రతిజ్ఞా(name);

        ఒకవేళ (విలువ)
                తిరిగి RET_NERRNO(setenv(name, విలువ, overwrite));

        తిరిగి RET_NERRNO(unsetenv(name));
}

పూర్ణాంకం putenv_dup(స్థిరరాశి అక్షరం *assignment, bool override) {
        స్థిరరాశి అక్షరం *e, *n;

        e = strchr(assignment, '=');
        ఒకవేళ (!e)
                తిరిగి -EINVAL;

        n = strndupa_సురక్షిత(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        తిరిగి RET_NERRNO(setenv(n, e + 1, override));
}

పూర్ణాంకం setenv_systemd_exec_pid(bool update_only) {
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ఒకవేళ (!e && update_only)
                తిరిగి 0;

        ఒకవేళ (streq_ptr(e, "*"))
                తిరిగి 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

పూర్ణాంకం setenv_systemd_log_level(శూన్య) {
        _cleanup_free_ అక్షరం *val = ఖాళీ;
        పూర్ణాంకం r;

        r = log_level_గమ్యం_string_alloc(log_get_max_level(), &val);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

పూర్ణాంకం getenv_మార్గం_list(స్థిరరాశి అక్షరం *name, అక్షరం ***ret_paths) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(ret_paths);

        e = secure_getenv(name);
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        r = strv_విభజించు_నిండిన(&l, e, ":", వెలికితీయి_DONT_COALESCE_SEPARATORS);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ఒకవేళ (!మార్గం_is_absolute(*p))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ఒకవేళ (!మార్గం_is_normalized(*p))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ఒకవేళ (మార్గం_equal(*p, "/"))
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ఒకవేళ (strv_isempty(l))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        తిరిగి 1;
}

పూర్ణాంకం getenv_steal_erase(స్థిరరాశి అక్షరం *name, అక్షరం **ret) {
        _cleanup_(erase_and_freep) అక్షరం *a = ఖాళీ;
        అక్షరం *e;

        ప్రతిజ్ఞా(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ఒకవేళ (!e) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి 0;
        }

        ఒకవేళ (ret) {
                a = strdup(e);
                ఒకవేళ (!a)
                        తిరిగి -ENOMEM;
        }

        string_erase(e);

        ఒకవేళ (unsetenv(name) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(a);

        తిరిగి 1;
}

పూర్ణాంకం setenvf(స్థిరరాశి అక్షరం *name, bool overwrite, స్థిరరాశి అక్షరం *valuef, ...) {
        _cleanup_free_ అక్షరం *విలువ = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);

        ఒకవేళ (!valuef)
                తిరిగి RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&విలువ, valuef, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ఒకవేళ (streq_ptr(getenv(name), విలువ))
                తిరిగి 0;

        తిరిగి RET_NERRNO(setenv(name, విలువ, overwrite));
}
