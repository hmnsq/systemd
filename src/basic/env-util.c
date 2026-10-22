/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "syslog-util.h"
#अन्तर्भाव "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(शून्य) {
        long l = sysconf(_SC_ARG_MAX);
        प्रतिज्ञा(l > 0);
        प्रत्यागम (size_t) l;
}

स्थिर bool env_name_is_valid_n(ध्रुव वर्ण *e, size_t n) {

        यदि (n == SIZE_MAX)
                n = strlen_ptr(e);

        यदि (n == 0)
                प्रत्यागम false;

        प्रतिज्ञा(e);

        यदि (ascii_isdigit(e[0]))
                प्रत्यागम false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        यदि (_unlikely_(n > sc_arg_max() - 2))
                प्रत्यागम false;

        प्रति (ध्रुव वर्ण *p = e; p < e + n; p++)
                यदि (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        प्रत्यागम false;

        प्रत्यागम true;
}

bool env_name_is_valid(ध्रुव वर्ण *e) {
        प्रत्यागम env_name_is_valid_n(e, SIZE_MAX);
}

bool env_मूल्य_is_valid(ध्रुव वर्ण *e) {
        यदि (!e)
                प्रत्यागम false;

        यदि (!utf8_is_valid(e))
                प्रत्यागम false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        यदि (_unlikely_(strlen(e) > sc_arg_max() - 3))
                प्रत्यागम false;

        प्रत्यागम true;
}

bool env_assignment_is_valid(ध्रुव वर्ण *e) {
        ध्रुव वर्ण *eq;

        प्रतिज्ञा(e);

        eq = strchr(e, '=');
        यदि (!eq)
                प्रत्यागम false;

        यदि (!env_name_is_valid_n(e, eq - e))
                प्रत्यागम false;

        यदि (!env_मूल्य_is_valid(eq + 1))
                प्रत्यागम false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        यदि (_unlikely_(strlen(e) > sc_arg_max() - 1))
                प्रत्यागम false;

        प्रत्यागम true;
}

bool strv_env_is_valid(वर्ण * ध्रुव *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                यदि (!env_assignment_is_valid(*p))
                        प्रत्यागम false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        यदि (strneq(*p, *q, k) && (*q)[k] == '=')
                                प्रत्यागम false;
        }

        प्रत्यागम true;
}

bool strv_env_name_is_valid(वर्ण * ध्रुव *l) {
        STRV_FOREACH(p, l) {
                यदि (!env_name_is_valid(*p))
                        प्रत्यागम false;

                यदि (strv_contains(p + 1, *p))
                        प्रत्यागम false;
        }

        प्रत्यागम true;
}

bool strv_env_name_or_assignment_is_valid(वर्ण * ध्रुव *l) {
        STRV_FOREACH(p, l) {
                यदि (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        प्रत्यागम false;

                यदि (strv_contains(p + 1, *p))
                        प्रत्यागम false;
        }

        प्रत्यागम true;
}

स्थिर पूर्णाङ्क env_append(वर्ण **e, वर्ण ***k, वर्ण **a) {
        प्रतिज्ञा(e);
        प्रतिज्ञा(k);
        प्रतिज्ञा(*k >= e);

        यदि (!a)
                प्रत्यागम 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        प्रति (; *a; a++) {
                वर्ण **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                यदि ((*a)[n] == '=')
                        n++;

                प्रति (j = e; j < *k; j++)
                        यदि (strneq(*j, *a, n))
                                भङ्ग;

                c = strdup(*a);
                यदि (!c)
                        प्रत्यागम -ENOMEM;

                यदि (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = रिक्त;
                        (*k)++;
                } अन्यथा
                        free_and_replace(*j, c); /* Override existing item */
        }

        प्रत्यागम 0;
}

वर्ण** _strv_env_merge(वर्ण **first, ...) {
        _cleanup_strv_free_ वर्ण **merged = रिक्त;
        वर्ण **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        प्रति (;;) {
                वर्ण **l;

                l = va_arg(ap, वर्ण**);
                यदि (l == POINTER_MAX)
                        भङ्ग;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(वर्ण*, n + 1);
        यदि (!merged)
                प्रत्यागम रिक्त;
        merged[0] = रिक्त;

        यदि (env_append(merged, &k, first) < 0)
                प्रत्यागम रिक्त;

        va_start(ap, first);
        प्रति (;;) {
                वर्ण **l;

                l = va_arg(ap, वर्ण**);
                यदि (l == POINTER_MAX)
                        भङ्ग;

                यदि (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        प्रत्यागम रिक्त;
                }
        }
        va_end(ap);

        प्रत्यागम TAKE_PTR(merged);
}

स्थिर bool env_match(ध्रुव वर्ण *t, ध्रुव वर्ण *pattern) {
        प्रतिज्ञा(t);
        प्रतिज्ञा(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        यदि (streq(t, pattern))
                प्रत्यागम true;

        यदि (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                प्रत्यागम t && *t == '=';
        }

        प्रत्यागम false;
}

स्थिर bool env_entry_has_name(ध्रुव वर्ण *entry, ध्रुव वर्ण *name) {
        ध्रुव वर्ण *t;

        प्रतिज्ञा(entry);
        प्रतिज्ञा(name);

        t = startswith(entry, name);
        यदि (!t)
                प्रत्यागम false;

        प्रत्यागम *t == '=';
}

वर्ण** strv_env_delete(वर्ण **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ वर्ण **t = रिक्त;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(वर्ण*, n+1);
        यदि (!t)
                प्रत्यागम रिक्त;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                प्रति (size_t v = 0; v < n_lists; v++) {
                        वर्ण **l;

                        l = va_arg(ap, वर्ण**);
                        STRV_FOREACH(j, l)
                                यदि (env_match(*k, *j))
                                        गच्छ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                यदि (!t[i])
                        प्रत्यागम रिक्त;

                i++;
                अनुवर्त;

        skip:
                va_end(ap);
        }

        t[i] = रिक्त;

        प्रतिज्ञा(i <= n);

        प्रत्यागम TAKE_PTR(t);
}

वर्ण** strv_env_unset(वर्ण **l, ध्रुव वर्ण *p) {
        प्रतिज्ञा(p);

        यदि (!l)
                प्रत्यागम रिक्त;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        वर्ण **f, **t;
        प्रति (f = t = l; *f; f++) {
                यदि (env_match(*f, p)) {
                        मुक्त(*f);
                        अनुवर्त;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        प्रत्यागम l;
}

वर्ण** strv_env_unset_many_internal(वर्ण **l, ...) {
        यदि (!l)
                प्रत्यागम रिक्त;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        वर्ण **f, **t;
        प्रति (f = t = l; *f; f++) {
                bool found = false;
                ध्रुव वर्ण *p;
                va_list ap;

                va_start(ap, l);

                यावत् ((p = va_arg(ap, ध्रुव वर्ण*)))
                        यदि (env_match(*f, p)) {
                                found = true;
                                भङ्ग;
                        }

                va_end(ap);

                यदि (found) {
                        मुक्त(*f);
                        अनुवर्त;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        प्रत्यागम l;
}

पूर्णाङ्क strv_env_replace_consume(वर्ण ***l, वर्ण *p) {
        ध्रुव वर्ण *t, *name;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        यदि (!t) {
                मुक्त(p);
                प्रत्यागम -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                यदि (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        प्रत्यागम 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

पूर्णाङ्क strv_env_replace_strdup(वर्ण ***l, ध्रुव वर्ण *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        वर्ण *p = strdup(assignment);
        यदि (!p)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_replace_strdup_passthrough(वर्ण ***l, ध्रुव वर्ण *assignment) {
        वर्ण *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        यदि (strchr(assignment, '=')) {
                यदि (!env_assignment_is_valid(assignment))
                        प्रत्यागम -EINVAL;

                p = strdup(assignment);
        } अन्यथा {
                यदि (!env_name_is_valid(assignment))
                        प्रत्यागम -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        यदि (!p)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assign(वर्ण ***l, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य) {
        प्रतिज्ञा(l);

        यदि (!env_name_is_valid(कुंजी))
                प्रत्यागम -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        यदि (!मूल्य) {
                strv_env_unset(*l, कुंजी);
                प्रत्यागम 0;
        }

        वर्ण *p = strjoin(कुंजी, "=", मूल्य);
        यदि (!p)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assignf(वर्ण ***l, ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *valuef, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(कुंजी);

        यदि (!env_name_is_valid(कुंजी))
                प्रत्यागम -EINVAL;

        यदि (!valuef) {
                strv_env_unset(*l, कुंजी);
                प्रत्यागम 0;
        }

        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        va_end(ap);
        यदि (r < 0)
                प्रत्यागम -ENOMEM;

        वर्ण *p = strjoin(कुंजी, "=", मूल्य);
        यदि (!p)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_env_replace_consume(l, p);
}

पूर्णाङ्क _strv_env_assign_many(वर्ण ***l, ...) {
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);

        va_start(ap, l);
        प्रति (;;) {
                ध्रुव वर्ण *कुंजी, *मूल्य;

                कुंजी = va_arg(ap, ध्रुव वर्ण *);
                यदि (!कुंजी)
                        भङ्ग;

                यदि (!env_name_is_valid(कुंजी)) {
                        va_end(ap);
                        प्रत्यागम -EINVAL;
                }

                मूल्य = va_arg(ap, ध्रुव वर्ण *);
                यदि (!मूल्य) {
                        strv_env_unset(*l, कुंजी);
                        अनुवर्त;
                }

                वर्ण *p = strjoin(कुंजी, "=", मूल्य);
                यदि (!p) {
                        va_end(ap);
                        प्रत्यागम -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                यदि (r < 0) {
                        va_end(ap);
                        प्रत्यागम r;
                }
        }
        va_end(ap);

        प्रत्यागम 0;
}

वर्ण* strv_env_get_n(वर्ण * ध्रुव *l, ध्रुव वर्ण *name, size_t k, ReplaceEnvFlags flags) {
        प्रतिज्ञा(name);

        यदि (k == SIZE_MAX)
                k = strlen(name);
        यदि (k <= 0)
                प्रत्यागम रिक्त;

        STRV_FOREACH_BACKWARDS(i, l)
                यदि (strneq(*i, name, k) && (*i)[k] == '=')
                        प्रत्यागम (वर्ण*) *i + k + 1;

        यदि (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ध्रुव वर्ण *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                यदि (k > sc_arg_max() - 2)
                        प्रत्यागम रिक्त;

                t = strndupa_safe(name, k);
                प्रत्यागम secure_getenv(t);
        };

        प्रत्यागम रिक्त;
}

वर्ण* strv_env_pairs_get(वर्ण **l, ध्रुव वर्ण *name) {
        वर्ण *result = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH_PAIR(कुंजी, मूल्य, l)
                यदि (streq(*कुंजी, name))
                        result = *मूल्य;

        प्रत्यागम result;
}

पूर्णाङ्क strv_env_get_merged(वर्ण **l, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        size_t n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(कुंजी, मूल्य, l) {
                वर्ण *s;

                s = strjoin(*कुंजी, "=", *मूल्य);
                यदि (!s)
                        प्रत्यागम -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(v);
        प्रत्यागम 0;
}

वर्ण** strv_env_clean_with_callback(वर्ण **l, शून्य (*invalid_callback)(ध्रुव वर्ण *p, शून्य *userdata), शून्य *userdata) {
        पूर्णाङ्क k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                यदि (!env_assignment_is_valid(*p)) {
                        यदि (invalid_callback)
                                invalid_callback(*p, userdata);
                        मुक्त(*p);
                        अनुवर्त;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        यदि (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                भङ्ग;
                        }

                यदि (duplicate) {
                        मुक्त(*p);
                        अनुवर्त;
                }

                l[k++] = *p;
        }

        यदि (l)
                l[k] = रिक्त;

        प्रत्यागम l;
}

स्थिर पूर्णाङ्क strv_extend_with_length(वर्ण ***l, ध्रुव वर्ण *s, size_t n) {
        वर्ण *c;

        c = strndup(s, n);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        प्रत्यागम strv_consume(l, c);
}

स्थिर पूर्णाङ्क strv_env_get_n_validated(
                वर्ण **env,
                ध्रुव वर्ण *name,
                size_t l,
                ReplaceEnvFlags flags,
                वर्ण **ret,              /* points into the env block! do not free! */
                वर्ण ***unset_variables, /* updated in place */
                वर्ण ***bad_variables) { /* ditto */

        वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(l == 0 || name);
        प्रतिज्ञा(ret);

        यदि (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                यदि (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        } अन्यथा {
                e = रिक्त; /* Resolve invalid variable names the same way as unset ones */

                यदि (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        }

        *ret = e;
        प्रत्यागम !!e;
}

पूर्णाङ्क replace_env_full(
                ध्रुव वर्ण *format,
                size_t length,
                वर्ण **env,
                ReplaceEnvFlags flags,
                वर्ण **ret,
                वर्ण ***ret_unset_variables,
                वर्ण ***ret_bad_variables) {

        गणना {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_मूल्य,
                ALTERNATE_मूल्य,
        } state = WORD;

        _cleanup_strv_free_ वर्ण **unset_variables = रिक्त, **bad_variables = रिक्त;
        ध्रुव वर्ण *e, *word = format, *test_मूल्य = रिक्त; /* test_value is initialized to appease gcc */
        _cleanup_free_ वर्ण *s = रिक्त;
        वर्ण ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        पूर्णाङ्क nest = 0, r;

        प्रतिज्ञा(format);

        यदि (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : रिक्त;
        pb = ret_bad_variables ? &bad_variables : रिक्त;

        प्रति (e = format, i = 0; *e && i < length; e++, i++)
                विकल्प (state) {

                पक्ष WORD:
                        यदि (*e == '$')
                                state = CURLY;
                        भङ्ग;

                पक्ष CURLY:
                        यदि (*e == '{') {
                                यदि (!strextendn(&s, word, e-word-1))
                                        प्रत्यागम -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } अन्यथा यदि (*e == '$') {
                                यदि (!strextendn(&s, word, e-word))
                                        प्रत्यागम -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } अन्यथा यदि (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                यदि (!strextendn(&s, word, e-word-1))
                                        प्रत्यागम -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } अन्यथा
                                state = WORD;
                        भङ्ग;

                पक्ष VARIABLE:
                        यदि (*e == '}') {
                                वर्ण *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (!strextend(&s, t))
                                        प्रत्यागम -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } अन्यथा यदि (*e == ':') {
                                यदि (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } अन्यथा
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        भङ्ग;

                पक्ष TEST:
                        यदि (*e == '-')
                                state = DEFAULT_मूल्य;
                        अन्यथा यदि (*e == '+')
                                state = ALTERNATE_मूल्य;
                        अन्यथा {
                                state = WORD;
                                भङ्ग;
                        }

                        test_मूल्य = e+1;
                        भङ्ग;

                पक्ष DEFAULT_मूल्य: /* fall through */
                पक्ष ALTERNATE_मूल्य:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        यदि (*e == '{') {
                                nest++;
                                भङ्ग;
                        }

                        यदि (*e != '}')
                                भङ्ग;

                        nest--;
                        यदि (nest == 0) {
                                _cleanup_strv_free_ वर्ण **u = रिक्त, **b = रिक्त;
                                _cleanup_free_ वर्ण *v = रिक्त;
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (t && state == ALTERNATE_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        यदि (r < 0)
                                                प्रत्यागम r;

                                        t = v;
                                } अन्यथा यदि (!t && state == DEFAULT_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        यदि (r < 0)
                                                प्रत्यागम r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                यदि (r < 0)
                                        प्रत्यागम r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (!strextend(&s, t))
                                        प्रत्यागम -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        भङ्ग;

                पक्ष VARIABLE_RAW:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        यदि (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (!strextend(&s, t))
                                        प्रत्यागम -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        भङ्ग;
                }

        यदि (state == VARIABLE_RAW) {
                वर्ण *t;

                प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (!strextend(&s, t))
                        प्रत्यागम -ENOMEM;

        } अन्यथा यदि (!strextendn(&s, word, e-word))
                प्रत्यागम -ENOMEM;

        यदि (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        यदि (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        यदि (ret)
                *ret = TAKE_PTR(s);

        प्रत्यागम 0;
}

पूर्णाङ्क replace_env_argv(
                वर्ण **argv,
                वर्ण **env,
                वर्ण ***ret,
                वर्ण ***ret_unset_variables,
                वर्ण ***ret_bad_variables) {

        _cleanup_strv_free_ वर्ण **n = रिक्त, **unset_variables = रिक्त, **bad_variables = रिक्त;
        size_t k = 0, l;
        पूर्णाङ्क r;

        प्रतिज्ञा(!strv_isempty(argv));
        प्रतिज्ञा(ret);

        l = strv_length(argv);

        n = new(वर्ण*, l+1);
        यदि (!n)
                प्रत्यागम -ENOMEM;

        STRV_FOREACH(i, argv) {
                ध्रुव वर्ण *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                यदि (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ वर्ण **m = रिक्त;
                        ध्रुव वर्ण *name = word + 1;
                        वर्ण *e, **w;
                        size_t q;

                        यदि (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                यदि (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                अन्यथा यदि (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                अन्यथा
                                        r = 0;
                        } अन्यथा यदि (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        अन्यथा
                                r = 0;
                        यदि (r < 0)
                                प्रत्यागम r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, परिमाण(वर्ण*));
                        यदि (!w)
                                प्रत्यागम -ENOMEM;

                        n = w;
                        यदि (m) {
                                memcpy(n + k, m, (q + 1) * परिमाण(वर्ण*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = रिक्त;
                        अनुवर्त;
                }

                _cleanup_strv_free_ वर्ण **u = रिक्त, **b = रिक्त;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : रिक्त,
                                ret_bad_variables ? &b : रिक्त);
                यदि (r < 0)
                        प्रत्यागम r;
                n[++k] = रिक्त;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                यदि (r < 0)
                        प्रत्यागम r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        यदि (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        प्रत्यागम 0;
}

पूर्णाङ्क getenv_bool(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        e = getenv(p);
        यदि (!e)
                प्रत्यागम -ENXIO;

        प्रत्यागम parse_boolean(e);
}

पूर्णाङ्क secure_getenv_bool(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        e = secure_getenv(p);
        यदि (!e)
                प्रत्यागम -ENXIO;

        प्रत्यागम parse_boolean(e);
}

पूर्णाङ्क secure_getenv_uint64(ध्रुव वर्ण *p, uint64_t *ret) {
        ध्रुव वर्ण *e;

        प्रतिज्ञा(p);

        e = secure_getenv(p);
        यदि (!e)
                प्रत्यागम -ENXIO;

        प्रत्यागम safe_atou64(e, ret);
}

पूर्णाङ्क set_unset_env(ध्रुव वर्ण *name, ध्रुव वर्ण *मूल्य, bool overwrite) {
        प्रतिज्ञा(name);

        यदि (मूल्य)
                प्रत्यागम RET_NERRNO(setenv(name, मूल्य, overwrite));

        प्रत्यागम RET_NERRNO(unsetenv(name));
}

पूर्णाङ्क putenv_dup(ध्रुव वर्ण *assignment, bool override) {
        ध्रुव वर्ण *e, *n;

        e = strchr(assignment, '=');
        यदि (!e)
                प्रत्यागम -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        प्रत्यागम RET_NERRNO(setenv(n, e + 1, override));
}

पूर्णाङ्क setenv_systemd_exec_pid(bool update_only) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        यदि (!e && update_only)
                प्रत्यागम 0;

        यदि (streq_ptr(e, "*"))
                प्रत्यागम 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

पूर्णाङ्क setenv_systemd_log_level(शून्य) {
        _cleanup_free_ वर्ण *val = रिक्त;
        पूर्णाङ्क r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

पूर्णाङ्क getenv_path_list(ध्रुव वर्ण *name, वर्ण ***ret_paths) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret_paths);

        e = secure_getenv(name);
        यदि (!e)
                प्रत्यागम -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                यदि (!path_is_absolute(*p))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                यदि (!path_is_normalized(*p))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                यदि (path_equal(*p, "/"))
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        यदि (strv_isempty(l))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        प्रत्यागम 1;
}

पूर्णाङ्क getenv_steal_erase(ध्रुव वर्ण *name, वर्ण **ret) {
        _cleanup_(erase_and_freep) वर्ण *a = रिक्त;
        वर्ण *e;

        प्रतिज्ञा(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        यदि (!e) {
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        यदि (ret) {
                a = strdup(e);
                यदि (!a)
                        प्रत्यागम -ENOMEM;
        }

        string_erase(e);

        यदि (unsetenv(name) < 0)
                प्रत्यागम -errno;

        यदि (ret)
                *ret = TAKE_PTR(a);

        प्रत्यागम 1;
}

पूर्णाङ्क setenvf(ध्रुव वर्ण *name, bool overwrite, ध्रुव वर्ण *valuef, ...) {
        _cleanup_free_ वर्ण *मूल्य = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        यदि (!valuef)
                प्रत्यागम RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        va_end(ap);

        यदि (r < 0)
                प्रत्यागम -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        यदि (streq_ptr(getenv(name), मूल्य))
                प्रत्यागम 0;

        प्रत्यागम RET_NERRNO(setenv(name, मूल्य, overwrite));
}
