/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "syslog-util.h"
#અન્તર્ભાવ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(શૂન્ય) {
        long l = sysconf(_SC_ARG_MAX);
        પ્રતિજ્ઞા(l > 0);
        પાછા (size_t) l;
}

સ્થિર bool env_name_is_valid_n(અચલ અક્ષર *e, size_t n) {

        જો (n == SIZE_MAX)
                n = strlen_ptr(e);

        જો (n == 0)
                પાછા false;

        પ્રતિજ્ઞા(e);

        જો (ascii_isdigit(e[0]))
                પાછા false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        જો (_unlikely_(n > sc_arg_max() - 2))
                પાછા false;

        માટે (અચલ અક્ષર *p = e; p < e + n; p++)
                જો (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        પાછા false;

        પાછા true;
}

bool env_name_is_valid(અચલ અક્ષર *e) {
        પાછા env_name_is_valid_n(e, SIZE_MAX);
}

bool env_કિંમત_is_valid(અચલ અક્ષર *e) {
        જો (!e)
                પાછા false;

        જો (!utf8_is_valid(e))
                પાછા false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        જો (_unlikely_(strlen(e) > sc_arg_max() - 3))
                પાછા false;

        પાછા true;
}

bool env_assignment_is_valid(અચલ અક્ષર *e) {
        અચલ અક્ષર *eq;

        પ્રતિજ્ઞા(e);

        eq = strchr(e, '=');
        જો (!eq)
                પાછા false;

        જો (!env_name_is_valid_n(e, eq - e))
                પાછા false;

        જો (!env_કિંમત_is_valid(eq + 1))
                પાછા false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        જો (_unlikely_(strlen(e) > sc_arg_max() - 1))
                પાછા false;

        પાછા true;
}

bool strv_env_is_valid(અક્ષર * અચલ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                જો (!env_assignment_is_valid(*p))
                        પાછા false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        જો (strneq(*p, *q, k) && (*q)[k] == '=')
                                પાછા false;
        }

        પાછા true;
}

bool strv_env_name_is_valid(અક્ષર * અચલ *l) {
        STRV_FOREACH(p, l) {
                જો (!env_name_is_valid(*p))
                        પાછા false;

                જો (strv_contains(p + 1, *p))
                        પાછા false;
        }

        પાછા true;
}

bool strv_env_name_or_assignment_is_valid(અક્ષર * અચલ *l) {
        STRV_FOREACH(p, l) {
                જો (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        પાછા false;

                જો (strv_contains(p + 1, *p))
                        પાછા false;
        }

        પાછા true;
}

સ્થિર પૂર્ણાંક env_append(અક્ષર **e, અક્ષર ***k, અક્ષર **a) {
        પ્રતિજ્ઞા(e);
        પ્રતિજ્ઞા(k);
        પ્રતિજ્ઞા(*k >= e);

        જો (!a)
                પાછા 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        માટે (; *a; a++) {
                અક્ષર **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                જો ((*a)[n] == '=')
                        n++;

                માટે (j = e; j < *k; j++)
                        જો (strneq(*j, *a, n))
                                તોડો;

                c = strdup(*a);
                જો (!c)
                        પાછા -ENOMEM;

                જો (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ખાલી;
                        (*k)++;
                } નહીંતર
                        free_and_replace(*j, c); /* Override existing item */
        }

        પાછા 0;
}

અક્ષર** _strv_env_merge(અક્ષર **first, ...) {
        _cleanup_strv_free_ અક્ષર **merged = ખાલી;
        અક્ષર **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        માટે (;;) {
                અક્ષર **l;

                l = va_arg(ap, અક્ષર**);
                જો (l == POINTER_MAX)
                        તોડો;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(અક્ષર*, n + 1);
        જો (!merged)
                પાછા ખાલી;
        merged[0] = ખાલી;

        જો (env_append(merged, &k, first) < 0)
                પાછા ખાલી;

        va_start(ap, first);
        માટે (;;) {
                અક્ષર **l;

                l = va_arg(ap, અક્ષર**);
                જો (l == POINTER_MAX)
                        તોડો;

                જો (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        પાછા ખાલી;
                }
        }
        va_end(ap);

        પાછા TAKE_PTR(merged);
}

સ્થિર bool env_match(અચલ અક્ષર *t, અચલ અક્ષર *pattern) {
        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        જો (streq(t, pattern))
                પાછા true;

        જો (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                પાછા t && *t == '=';
        }

        પાછા false;
}

સ્થિર bool env_entry_has_name(અચલ અક્ષર *entry, અચલ અક્ષર *name) {
        અચલ અક્ષર *t;

        પ્રતિજ્ઞા(entry);
        પ્રતિજ્ઞા(name);

        t = startswith(entry, name);
        જો (!t)
                પાછા false;

        પાછા *t == '=';
}

અક્ષર** strv_env_delete(અક્ષર **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ અક્ષર **t = ખાલી;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(અક્ષર*, n+1);
        જો (!t)
                પાછા ખાલી;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                માટે (size_t v = 0; v < n_lists; v++) {
                        અક્ષર **l;

                        l = va_arg(ap, અક્ષર**);
                        STRV_FOREACH(j, l)
                                જો (env_match(*k, *j))
                                        જાઓ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                જો (!t[i])
                        પાછા ખાલી;

                i++;
                ચાલુ;

        skip:
                va_end(ap);
        }

        t[i] = ખાલી;

        પ્રતિજ્ઞા(i <= n);

        પાછા TAKE_PTR(t);
}

અક્ષર** strv_env_unset(અક્ષર **l, અચલ અક્ષર *p) {
        પ્રતિજ્ઞા(p);

        જો (!l)
                પાછા ખાલી;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        અક્ષર **f, **t;
        માટે (f = t = l; *f; f++) {
                જો (env_match(*f, p)) {
                        મુક્ત(*f);
                        ચાલુ;
                }

                *(t++) = *f;
        }

        *t = ખાલી;
        પાછા l;
}

અક્ષર** strv_env_unset_many_internal(અક્ષર **l, ...) {
        જો (!l)
                પાછા ખાલી;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        અક્ષર **f, **t;
        માટે (f = t = l; *f; f++) {
                bool found = false;
                અચલ અક્ષર *p;
                va_list ap;

                va_start(ap, l);

                જ્યારે ((p = va_arg(ap, અચલ અક્ષર*)))
                        જો (env_match(*f, p)) {
                                found = true;
                                તોડો;
                        }

                va_end(ap);

                જો (found) {
                        મુક્ત(*f);
                        ચાલુ;
                }

                *(t++) = *f;
        }

        *t = ખાલી;
        પાછા l;
}

પૂર્ણાંક strv_env_replace_consume(અક્ષર ***l, અક્ષર *p) {
        અચલ અક્ષર *t, *name;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(l);
        પ્રતિજ્ઞા(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        જો (!t) {
                મુક્ત(p);
                પાછા -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                જો (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        પાછા 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

પૂર્ણાંક strv_env_replace_strdup(અક્ષર ***l, અચલ અક્ષર *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        પ્રતિજ્ઞા(l);
        પ્રતિજ્ઞા(assignment);

        અક્ષર *p = strdup(assignment);
        જો (!p)
                પાછા -ENOMEM;

        પાછા strv_env_replace_consume(l, p);
}

પૂર્ણાંક strv_env_replace_strdup_passthrough(અક્ષર ***l, અચલ અક્ષર *assignment) {
        અક્ષર *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        પ્રતિજ્ઞા(l);
        પ્રતિજ્ઞા(assignment);

        જો (strchr(assignment, '=')) {
                જો (!env_assignment_is_valid(assignment))
                        પાછા -EINVAL;

                p = strdup(assignment);
        } નહીંતર {
                જો (!env_name_is_valid(assignment))
                        પાછા -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        જો (!p)
                પાછા -ENOMEM;

        પાછા strv_env_replace_consume(l, p);
}

પૂર્ણાંક strv_env_assign(અક્ષર ***l, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પ્રતિજ્ઞા(l);

        જો (!env_name_is_valid(ચાવી))
                પાછા -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        જો (!કિંમત) {
                strv_env_unset(*l, ચાવી);
                પાછા 0;
        }

        અક્ષર *p = strjoin(ચાવી, "=", કિંમત);
        જો (!p)
                પાછા -ENOMEM;

        પાછા strv_env_replace_consume(l, p);
}

પૂર્ણાંક strv_env_assignf(અક્ષર ***l, અચલ અક્ષર *ચાવી, અચલ અક્ષર *valuef, ...) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(l);
        પ્રતિજ્ઞા(ચાવી);

        જો (!env_name_is_valid(ચાવી))
                પાછા -EINVAL;

        જો (!valuef) {
                strv_env_unset(*l, ચાવી);
                પાછા 0;
        }

        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&કિંમત, valuef, ap);
        va_end(ap);
        જો (r < 0)
                પાછા -ENOMEM;

        અક્ષર *p = strjoin(ચાવી, "=", કિંમત);
        જો (!p)
                પાછા -ENOMEM;

        પાછા strv_env_replace_consume(l, p);
}

પૂર્ણાંક _strv_env_assign_many(અક્ષર ***l, ...) {
        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(l);

        va_start(ap, l);
        માટે (;;) {
                અચલ અક્ષર *ચાવી, *કિંમત;

                ચાવી = va_arg(ap, અચલ અક્ષર *);
                જો (!ચાવી)
                        તોડો;

                જો (!env_name_is_valid(ચાવી)) {
                        va_end(ap);
                        પાછા -EINVAL;
                }

                કિંમત = va_arg(ap, અચલ અક્ષર *);
                જો (!કિંમત) {
                        strv_env_unset(*l, ચાવી);
                        ચાલુ;
                }

                અક્ષર *p = strjoin(ચાવી, "=", કિંમત);
                જો (!p) {
                        va_end(ap);
                        પાછા -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                જો (r < 0) {
                        va_end(ap);
                        પાછા r;
                }
        }
        va_end(ap);

        પાછા 0;
}

અક્ષર* strv_env_get_n(અક્ષર * અચલ *l, અચલ અક્ષર *name, size_t k, ReplaceEnvFlags flags) {
        પ્રતિજ્ઞા(name);

        જો (k == SIZE_MAX)
                k = strlen(name);
        જો (k <= 0)
                પાછા ખાલી;

        STRV_FOREACH_BACKWARDS(i, l)
                જો (strneq(*i, name, k) && (*i)[k] == '=')
                        પાછા (અક્ષર*) *i + k + 1;

        જો (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                અચલ અક્ષર *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                જો (k > sc_arg_max() - 2)
                        પાછા ખાલી;

                t = strndupa_safe(name, k);
                પાછા secure_getenv(t);
        };

        પાછા ખાલી;
}

અક્ષર* strv_env_pairs_get(અક્ષર **l, અચલ અક્ષર *name) {
        અક્ષર *result = ખાલી;

        પ્રતિજ્ઞા(name);

        STRV_FOREACH_PAIR(ચાવી, કિંમત, l)
                જો (streq(*ચાવી, name))
                        result = *કિંમત;

        પાછા result;
}

પૂર્ણાંક strv_env_get_merged(અક્ષર **l, અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **v = ખાલી;
        size_t n = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(ચાવી, કિંમત, l) {
                અક્ષર *s;

                s = strjoin(*ચાવી, "=", *કિંમત);
                જો (!s)
                        પાછા -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(v);
        પાછા 0;
}

અક્ષર** strv_env_clean_with_callback(અક્ષર **l, શૂન્ય (*invalid_callback)(અચલ અક્ષર *p, શૂન્ય *userdata), શૂન્ય *userdata) {
        પૂર્ણાંક k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                જો (!env_assignment_is_valid(*p)) {
                        જો (invalid_callback)
                                invalid_callback(*p, userdata);
                        મુક્ત(*p);
                        ચાલુ;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        જો (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                તોડો;
                        }

                જો (duplicate) {
                        મુક્ત(*p);
                        ચાલુ;
                }

                l[k++] = *p;
        }

        જો (l)
                l[k] = ખાલી;

        પાછા l;
}

સ્થિર પૂર્ણાંક strv_extend_with_length(અક્ષર ***l, અચલ અક્ષર *s, size_t n) {
        અક્ષર *c;

        c = strndup(s, n);
        જો (!c)
                પાછા -ENOMEM;

        પાછા strv_consume(l, c);
}

સ્થિર પૂર્ણાંક strv_env_get_n_validated(
                અક્ષર **env,
                અચલ અક્ષર *name,
                size_t l,
                ReplaceEnvFlags flags,
                અક્ષર **ret,              /* points into the env block! do not free! */
                અક્ષર ***unset_variables, /* updated in place */
                અક્ષર ***bad_variables) { /* ditto */

        અક્ષર *e;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(l == 0 || name);
        પ્રતિજ્ઞા(ret);

        જો (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                જો (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        જો (r < 0)
                                પાછા r;
                }
        } નહીંતર {
                e = ખાલી; /* Resolve invalid variable names the same way as unset ones */

                જો (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        જો (r < 0)
                                પાછા r;
                }
        }

        *ret = e;
        પાછા !!e;
}

પૂર્ણાંક replace_env_full(
                અચલ અક્ષર *format,
                size_t length,
                અક્ષર **env,
                ReplaceEnvFlags flags,
                અક્ષર **ret,
                અક્ષર ***ret_unset_variables,
                અક્ષર ***ret_bad_variables) {

        ગણના {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_કિંમત,
                ALTERNATE_કિંમત,
        } state = WORD;

        _cleanup_strv_free_ અક્ષર **unset_variables = ખાલી, **bad_variables = ખાલી;
        અચલ અક્ષર *e, *word = format, *test_કિંમત = ખાલી; /* test_value is initialized to appease gcc */
        _cleanup_free_ અક્ષર *s = ખાલી;
        અક્ષર ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        પૂર્ણાંક nest = 0, r;

        પ્રતિજ્ઞા(format);

        જો (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : ખાલી;
        pb = ret_bad_variables ? &bad_variables : ખાલી;

        માટે (e = format, i = 0; *e && i < length; e++, i++)
                પસંદગી (state) {

                સ્થિતિ WORD:
                        જો (*e == '$')
                                state = CURLY;
                        તોડો;

                સ્થિતિ CURLY:
                        જો (*e == '{') {
                                જો (!strextendn(&s, word, e-word-1))
                                        પાછા -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } નહીંતર જો (*e == '$') {
                                જો (!strextendn(&s, word, e-word))
                                        પાછા -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } નહીંતર જો (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                જો (!strextendn(&s, word, e-word-1))
                                        પાછા -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } નહીંતર
                                state = WORD;
                        તોડો;

                સ્થિતિ VARIABLE:
                        જો (*e == '}') {
                                અક્ષર *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                જો (r < 0)
                                        પાછા r;

                                જો (!strextend(&s, t))
                                        પાછા -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } નહીંતર જો (*e == ':') {
                                જો (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } નહીંતર
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        તોડો;

                સ્થિતિ TEST:
                        જો (*e == '-')
                                state = DEFAULT_કિંમત;
                        નહીંતર જો (*e == '+')
                                state = ALTERNATE_કિંમત;
                        નહીંતર {
                                state = WORD;
                                તોડો;
                        }

                        test_કિંમત = e+1;
                        તોડો;

                સ્થિતિ DEFAULT_કિંમત: /* fall through */
                સ્થિતિ ALTERNATE_કિંમત:
                        પ્રતિજ્ઞા(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        જો (*e == '{') {
                                nest++;
                                તોડો;
                        }

                        જો (*e != '}')
                                તોડો;

                        nest--;
                        જો (nest == 0) {
                                _cleanup_strv_free_ અક્ષર **u = ખાલી, **b = ખાલી;
                                _cleanup_free_ અક્ષર *v = ખાલી;
                                અક્ષર *t = ખાલી;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                જો (r < 0)
                                        પાછા r;

                                જો (t && state == ALTERNATE_કિંમત) {
                                        r = replace_env_full(test_કિંમત, e-test_કિંમત, env, flags, &v, pu ? &u : ખાલી, pb ? &b : ખાલી);
                                        જો (r < 0)
                                                પાછા r;

                                        t = v;
                                } નહીંતર જો (!t && state == DEFAULT_કિંમત) {
                                        r = replace_env_full(test_કિંમત, e-test_કિંમત, env, flags, &v, pu ? &u : ખાલી, pb ? &b : ખાલી);
                                        જો (r < 0)
                                                પાછા r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                જો (r < 0)
                                        પાછા r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                જો (r < 0)
                                        પાછા r;

                                જો (!strextend(&s, t))
                                        પાછા -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        તોડો;

                સ્થિતિ VARIABLE_RAW:
                        પ્રતિજ્ઞા(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        જો (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                અક્ષર *t = ખાલી;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                જો (r < 0)
                                        પાછા r;

                                જો (!strextend(&s, t))
                                        પાછા -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        તોડો;
                }

        જો (state == VARIABLE_RAW) {
                અક્ષર *t;

                પ્રતિજ્ઞા(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                જો (r < 0)
                        પાછા r;

                જો (!strextend(&s, t))
                        પાછા -ENOMEM;

        } નહીંતર જો (!strextendn(&s, word, e-word))
                પાછા -ENOMEM;

        જો (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        જો (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        જો (ret)
                *ret = TAKE_PTR(s);

        પાછા 0;
}

પૂર્ણાંક replace_env_argv(
                અક્ષર **argv,
                અક્ષર **env,
                અક્ષર ***ret,
                અક્ષર ***ret_unset_variables,
                અક્ષર ***ret_bad_variables) {

        _cleanup_strv_free_ અક્ષર **n = ખાલી, **unset_variables = ખાલી, **bad_variables = ખાલી;
        size_t k = 0, l;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(!strv_isempty(argv));
        પ્રતિજ્ઞા(ret);

        l = strv_length(argv);

        n = new(અક્ષર*, l+1);
        જો (!n)
                પાછા -ENOMEM;

        STRV_FOREACH(i, argv) {
                અચલ અક્ષર *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                જો (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ અક્ષર **m = ખાલી;
                        અચલ અક્ષર *name = word + 1;
                        અક્ષર *e, **w;
                        size_t q;

                        જો (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                જો (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                નહીંતર જો (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                નહીંતર
                                        r = 0;
                        } નહીંતર જો (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        નહીંતર
                                r = 0;
                        જો (r < 0)
                                પાછા r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, કદ(અક્ષર*));
                        જો (!w)
                                પાછા -ENOMEM;

                        n = w;
                        જો (m) {
                                memcpy(n + k, m, (q + 1) * કદ(અક્ષર*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ખાલી;
                        ચાલુ;
                }

                _cleanup_strv_free_ અક્ષર **u = ખાલી, **b = ખાલી;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ખાલી,
                                ret_bad_variables ? &b : ખાલી);
                જો (r < 0)
                        પાછા r;
                n[++k] = ખાલી;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                જો (r < 0)
                        પાછા r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        જો (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        પાછા 0;
}

પૂર્ણાંક getenv_bool(અચલ અક્ષર *p) {
        અચલ અક્ષર *e;

        e = getenv(p);
        જો (!e)
                પાછા -ENXIO;

        પાછા parse_boolean(e);
}

પૂર્ણાંક secure_getenv_bool(અચલ અક્ષર *p) {
        અચલ અક્ષર *e;

        e = secure_getenv(p);
        જો (!e)
                પાછા -ENXIO;

        પાછા parse_boolean(e);
}

પૂર્ણાંક secure_getenv_uint64(અચલ અક્ષર *p, uint64_t *ret) {
        અચલ અક્ષર *e;

        પ્રતિજ્ઞા(p);

        e = secure_getenv(p);
        જો (!e)
                પાછા -ENXIO;

        પાછા safe_atou64(e, ret);
}

પૂર્ણાંક set_unset_env(અચલ અક્ષર *name, અચલ અક્ષર *કિંમત, bool overwrite) {
        પ્રતિજ્ઞા(name);

        જો (કિંમત)
                પાછા RET_NERRNO(setenv(name, કિંમત, overwrite));

        પાછા RET_NERRNO(unsetenv(name));
}

પૂર્ણાંક putenv_dup(અચલ અક્ષર *assignment, bool override) {
        અચલ અક્ષર *e, *n;

        e = strchr(assignment, '=');
        જો (!e)
                પાછા -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        પાછા RET_NERRNO(setenv(n, e + 1, override));
}

પૂર્ણાંક setenv_systemd_exec_pid(bool update_only) {
        અચલ અક્ષર *e;
        પૂર્ણાંક r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        જો (!e && update_only)
                પાછા 0;

        જો (streq_ptr(e, "*"))
                પાછા 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

પૂર્ણાંક setenv_systemd_log_level(શૂન્ય) {
        _cleanup_free_ અક્ષર *val = ખાલી;
        પૂર્ણાંક r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        જો (r < 0)
                પાછા r;

        પાછા RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

પૂર્ણાંક getenv_path_list(અચલ અક્ષર *name, અક્ષર ***ret_paths) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        અચલ અક્ષર *e;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(ret_paths);

        e = secure_getenv(name);
        જો (!e)
                પાછા -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                જો (!path_is_absolute(*p))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                જો (!path_is_normalized(*p))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                જો (path_equal(*p, "/"))
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        જો (strv_isempty(l))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        પાછા 1;
}

પૂર્ણાંક getenv_steal_erase(અચલ અક્ષર *name, અક્ષર **ret) {
        _cleanup_(erase_and_freep) અક્ષર *a = ખાલી;
        અક્ષર *e;

        પ્રતિજ્ઞા(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        જો (!e) {
                જો (ret)
                        *ret = ખાલી;
                પાછા 0;
        }

        જો (ret) {
                a = strdup(e);
                જો (!a)
                        પાછા -ENOMEM;
        }

        string_erase(e);

        જો (unsetenv(name) < 0)
                પાછા -errno;

        જો (ret)
                *ret = TAKE_PTR(a);

        પાછા 1;
}

પૂર્ણાંક setenvf(અચલ અક્ષર *name, bool overwrite, અચલ અક્ષર *valuef, ...) {
        _cleanup_free_ અક્ષર *કિંમત = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);

        જો (!valuef)
                પાછા RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&કિંમત, valuef, ap);
        va_end(ap);

        જો (r < 0)
                પાછા -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        જો (streq_ptr(getenv(name), કિંમત))
                પાછા 0;

        પાછા RET_NERRNO(setenv(name, કિંમત, overwrite));
}
