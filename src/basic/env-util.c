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
        पटिनिवत्त (size_t) l;
}

स्थिर bool env_name_is_valid_n(ध्रुव वर्ण *e, size_t n) {

        सचे (n == SIZE_MAX)
                n = strlen_ptr(e);

        सचे (n == 0)
                पटिनिवत्त false;

        प्रतिज्ञा(e);

        सचे (ascii_isdigit(e[0]))
                पटिनिवत्त false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        सचे (_unlikely_(n > sc_arg_max() - 2))
                पटिनिवत्त false;

        अत्थाय (ध्रुव वर्ण *p = e; p < e + n; p++)
                सचे (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        पटिनिवत्त false;

        पटिनिवत्त true;
}

bool env_name_is_valid(ध्रुव वर्ण *e) {
        पटिनिवत्त env_name_is_valid_n(e, SIZE_MAX);
}

bool env_मानम्_is_valid(ध्रुव वर्ण *e) {
        सचे (!e)
                पटिनिवत्त false;

        सचे (!utf8_is_valid(e))
                पटिनिवत्त false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        सचे (_unlikely_(strlen(e) > sc_arg_max() - 3))
                पटिनिवत्त false;

        पटिनिवत्त true;
}

bool env_assignment_is_valid(ध्रुव वर्ण *e) {
        ध्रुव वर्ण *eq;

        प्रतिज्ञा(e);

        eq = strchr(e, '=');
        सचे (!eq)
                पटिनिवत्त false;

        सचे (!env_name_is_valid_n(e, eq - e))
                पटिनिवत्त false;

        सचे (!env_मानम्_is_valid(eq + 1))
                पटिनिवत्त false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        सचे (_unlikely_(strlen(e) > sc_arg_max() - 1))
                पटिनिवत्त false;

        पटिनिवत्त true;
}

bool strv_env_is_valid(वर्ण * ध्रुव *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                सचे (!env_assignment_is_valid(*p))
                        पटिनिवत्त false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        सचे (strneq(*p, *q, k) && (*q)[k] == '=')
                                पटिनिवत्त false;
        }

        पटिनिवत्त true;
}

bool strv_env_name_is_valid(वर्ण * ध्रुव *l) {
        STRV_FOREACH(p, l) {
                सचे (!env_name_is_valid(*p))
                        पटिनिवत्त false;

                सचे (strv_contains(p + 1, *p))
                        पटिनिवत्त false;
        }

        पटिनिवत्त true;
}

bool strv_env_name_or_assignment_is_valid(वर्ण * ध्रुव *l) {
        STRV_FOREACH(p, l) {
                सचे (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        पटिनिवत्त false;

                सचे (strv_contains(p + 1, *p))
                        पटिनिवत्त false;
        }

        पटिनिवत्त true;
}

स्थिर पूर्णाङ्क env_append(वर्ण **e, वर्ण ***k, वर्ण **a) {
        प्रतिज्ञा(e);
        प्रतिज्ञा(k);
        प्रतिज्ञा(*k >= e);

        सचे (!a)
                पटिनिवत्त 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        अत्थाय (; *a; a++) {
                वर्ण **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                सचे ((*a)[n] == '=')
                        n++;

                अत्थाय (j = e; j < *k; j++)
                        सचे (strneq(*j, *a, n))
                                भङ्ग;

                c = strdup(*a);
                सचे (!c)
                        पटिनिवत्त -ENOMEM;

                सचे (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = रिक्त;
                        (*k)++;
                } अञ्ञथा
                        free_and_replace(*j, c); /* Override existing item */
        }

        पटिनिवत्त 0;
}

वर्ण** _strv_env_merge(वर्ण **first, ...) {
        _cleanup_strv_free_ वर्ण **merged = रिक्त;
        वर्ण **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        अत्थाय (;;) {
                वर्ण **l;

                l = va_arg(ap, वर्ण**);
                सचे (l == POINTER_MAX)
                        भङ्ग;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(वर्ण*, n + 1);
        सचे (!merged)
                पटिनिवत्त रिक्त;
        merged[0] = रिक्त;

        सचे (env_append(merged, &k, first) < 0)
                पटिनिवत्त रिक्त;

        va_start(ap, first);
        अत्थाय (;;) {
                वर्ण **l;

                l = va_arg(ap, वर्ण**);
                सचे (l == POINTER_MAX)
                        भङ्ग;

                सचे (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        पटिनिवत्त रिक्त;
                }
        }
        va_end(ap);

        पटिनिवत्त TAKE_PTR(merged);
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

        सचे (streq(t, pattern))
                पटिनिवत्त true;

        सचे (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                पटिनिवत्त t && *t == '=';
        }

        पटिनिवत्त false;
}

स्थिर bool env_entry_has_name(ध्रुव वर्ण *entry, ध्रुव वर्ण *name) {
        ध्रुव वर्ण *t;

        प्रतिज्ञा(entry);
        प्रतिज्ञा(name);

        t = startswith(entry, name);
        सचे (!t)
                पटिनिवत्त false;

        पटिनिवत्त *t == '=';
}

वर्ण** strv_env_delete(वर्ण **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ वर्ण **t = रिक्त;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(वर्ण*, n+1);
        सचे (!t)
                पटिनिवत्त रिक्त;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                अत्थाय (size_t v = 0; v < n_lists; v++) {
                        वर्ण **l;

                        l = va_arg(ap, वर्ण**);
                        STRV_FOREACH(j, l)
                                सचे (env_match(*k, *j))
                                        गच्छ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                सचे (!t[i])
                        पटिनिवत्त रिक्त;

                i++;
                अनुवर्त;

        skip:
                va_end(ap);
        }

        t[i] = रिक्त;

        प्रतिज्ञा(i <= n);

        पटिनिवत्त TAKE_PTR(t);
}

वर्ण** strv_env_unset(वर्ण **l, ध्रुव वर्ण *p) {
        प्रतिज्ञा(p);

        सचे (!l)
                पटिनिवत्त रिक्त;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        वर्ण **f, **t;
        अत्थाय (f = t = l; *f; f++) {
                सचे (env_match(*f, p)) {
                        मुक्त(*f);
                        अनुवर्त;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        पटिनिवत्त l;
}

वर्ण** strv_env_unset_many_internal(वर्ण **l, ...) {
        सचे (!l)
                पटिनिवत्त रिक्त;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        वर्ण **f, **t;
        अत्थाय (f = t = l; *f; f++) {
                bool found = false;
                ध्रुव वर्ण *p;
                va_list ap;

                va_start(ap, l);

                याव ((p = va_arg(ap, ध्रुव वर्ण*)))
                        सचे (env_match(*f, p)) {
                                found = true;
                                भङ्ग;
                        }

                va_end(ap);

                सचे (found) {
                        मुक्त(*f);
                        अनुवर्त;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        पटिनिवत्त l;
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
        सचे (!t) {
                मुक्त(p);
                पटिनिवत्त -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                सचे (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        पटिनिवत्त 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 1;
}

पूर्णाङ्क strv_env_replace_strdup(वर्ण ***l, ध्रुव वर्ण *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        वर्ण *p = strdup(assignment);
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_replace_strdup_passthrough(वर्ण ***l, ध्रुव वर्ण *assignment) {
        वर्ण *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        सचे (strchr(assignment, '=')) {
                सचे (!env_assignment_is_valid(assignment))
                        पटिनिवत्त -EINVAL;

                p = strdup(assignment);
        } अञ्ञथा {
                सचे (!env_name_is_valid(assignment))
                        पटिनिवत्त -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assign(वर्ण ***l, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        प्रतिज्ञा(l);

        सचे (!env_name_is_valid(कुञ्चिका))
                पटिनिवत्त -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        सचे (!मानम्) {
                strv_env_unset(*l, कुञ्चिका);
                पटिनिवत्त 0;
        }

        वर्ण *p = strjoin(कुञ्चिका, "=", मानम्);
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assignf(वर्ण ***l, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *valuef, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(कुञ्चिका);

        सचे (!env_name_is_valid(कुञ्चिका))
                पटिनिवत्त -EINVAL;

        सचे (!valuef) {
                strv_env_unset(*l, कुञ्चिका);
                पटिनिवत्त 0;
        }

        _cleanup_free_ वर्ण *मानम् = रिक्त;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&मानम्, valuef, ap);
        va_end(ap);
        सचे (r < 0)
                पटिनिवत्त -ENOMEM;

        वर्ण *p = strjoin(कुञ्चिका, "=", मानम्);
        सचे (!p)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त strv_env_replace_consume(l, p);
}

पूर्णाङ्क _strv_env_assign_many(वर्ण ***l, ...) {
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);

        va_start(ap, l);
        अत्थाय (;;) {
                ध्रुव वर्ण *कुञ्चिका, *मानम्;

                कुञ्चिका = va_arg(ap, ध्रुव वर्ण *);
                सचे (!कुञ्चिका)
                        भङ्ग;

                सचे (!env_name_is_valid(कुञ्चिका)) {
                        va_end(ap);
                        पटिनिवत्त -EINVAL;
                }

                मानम् = va_arg(ap, ध्रुव वर्ण *);
                सचे (!मानम्) {
                        strv_env_unset(*l, कुञ्चिका);
                        अनुवर्त;
                }

                वर्ण *p = strjoin(कुञ्चिका, "=", मानम्);
                सचे (!p) {
                        va_end(ap);
                        पटिनिवत्त -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                सचे (r < 0) {
                        va_end(ap);
                        पटिनिवत्त r;
                }
        }
        va_end(ap);

        पटिनिवत्त 0;
}

वर्ण* strv_env_get_n(वर्ण * ध्रुव *l, ध्रुव वर्ण *name, size_t k, ReplaceEnvFlags flags) {
        प्रतिज्ञा(name);

        सचे (k == SIZE_MAX)
                k = strlen(name);
        सचे (k <= 0)
                पटिनिवत्त रिक्त;

        STRV_FOREACH_BACKWARDS(i, l)
                सचे (strneq(*i, name, k) && (*i)[k] == '=')
                        पटिनिवत्त (वर्ण*) *i + k + 1;

        सचे (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ध्रुव वर्ण *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                सचे (k > sc_arg_max() - 2)
                        पटिनिवत्त रिक्त;

                t = strndupa_safe(name, k);
                पटिनिवत्त secure_getenv(t);
        };

        पटिनिवत्त रिक्त;
}

वर्ण* strv_env_pairs_get(वर्ण **l, ध्रुव वर्ण *name) {
        वर्ण *result = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH_PAIR(कुञ्चिका, मानम्, l)
                सचे (streq(*कुञ्चिका, name))
                        result = *मानम्;

        पटिनिवत्त result;
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

        STRV_FOREACH_PAIR(कुञ्चिका, मानम्, l) {
                वर्ण *s;

                s = strjoin(*कुञ्चिका, "=", *मानम्);
                सचे (!s)
                        पटिनिवत्त -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        *ret = TAKE_PTR(v);
        पटिनिवत्त 0;
}

वर्ण** strv_env_clean_with_callback(वर्ण **l, शून्य (*invalid_callback)(ध्रुव वर्ण *p, शून्य *userdata), शून्य *userdata) {
        पूर्णाङ्क k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                सचे (!env_assignment_is_valid(*p)) {
                        सचे (invalid_callback)
                                invalid_callback(*p, userdata);
                        मुक्त(*p);
                        अनुवर्त;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        सचे (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                भङ्ग;
                        }

                सचे (duplicate) {
                        मुक्त(*p);
                        अनुवर्त;
                }

                l[k++] = *p;
        }

        सचे (l)
                l[k] = रिक्त;

        पटिनिवत्त l;
}

स्थिर पूर्णाङ्क strv_extend_with_length(वर्ण ***l, ध्रुव वर्ण *s, size_t n) {
        वर्ण *c;

        c = strndup(s, n);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त strv_consume(l, c);
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

        सचे (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                सचे (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        सचे (r < 0)
                                पटिनिवत्त r;
                }
        } अञ्ञथा {
                e = रिक्त; /* Resolve invalid variable names the same way as unset ones */

                सचे (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        सचे (r < 0)
                                पटिनिवत्त r;
                }
        }

        *ret = e;
        पटिनिवत्त !!e;
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
                DEFAULT_मानम्,
                ALTERNATE_मानम्,
        } state = WORD;

        _cleanup_strv_free_ वर्ण **unset_variables = रिक्त, **bad_variables = रिक्त;
        ध्रुव वर्ण *e, *word = format, *test_मानम् = रिक्त; /* test_value is initialized to appease gcc */
        _cleanup_free_ वर्ण *s = रिक्त;
        वर्ण ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        पूर्णाङ्क nest = 0, r;

        प्रतिज्ञा(format);

        सचे (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : रिक्त;
        pb = ret_bad_variables ? &bad_variables : रिक्त;

        अत्थाय (e = format, i = 0; *e && i < length; e++, i++)
                विकल्प (state) {

                पक्ष WORD:
                        सचे (*e == '$')
                                state = CURLY;
                        भङ्ग;

                पक्ष CURLY:
                        सचे (*e == '{') {
                                सचे (!strextendn(&s, word, e-word-1))
                                        पटिनिवत्त -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } अञ्ञथा सचे (*e == '$') {
                                सचे (!strextendn(&s, word, e-word))
                                        पटिनिवत्त -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } अञ्ञथा सचे (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                सचे (!strextendn(&s, word, e-word-1))
                                        पटिनिवत्त -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } अञ्ञथा
                                state = WORD;
                        भङ्ग;

                पक्ष VARIABLE:
                        सचे (*e == '}') {
                                वर्ण *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                सचे (!strextend(&s, t))
                                        पटिनिवत्त -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } अञ्ञथा सचे (*e == ':') {
                                सचे (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } अञ्ञथा
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        भङ्ग;

                पक्ष TEST:
                        सचे (*e == '-')
                                state = DEFAULT_मानम्;
                        अञ्ञथा सचे (*e == '+')
                                state = ALTERNATE_मानम्;
                        अञ्ञथा {
                                state = WORD;
                                भङ्ग;
                        }

                        test_मानम् = e+1;
                        भङ्ग;

                पक्ष DEFAULT_मानम्: /* fall through */
                पक्ष ALTERNATE_मानम्:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        सचे (*e == '{') {
                                nest++;
                                भङ्ग;
                        }

                        सचे (*e != '}')
                                भङ्ग;

                        nest--;
                        सचे (nest == 0) {
                                _cleanup_strv_free_ वर्ण **u = रिक्त, **b = रिक्त;
                                _cleanup_free_ वर्ण *v = रिक्त;
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                सचे (t && state == ALTERNATE_मानम्) {
                                        r = replace_env_full(test_मानम्, e-test_मानम्, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        सचे (r < 0)
                                                पटिनिवत्त r;

                                        t = v;
                                } अञ्ञथा सचे (!t && state == DEFAULT_मानम्) {
                                        r = replace_env_full(test_मानम्, e-test_मानम्, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        सचे (r < 0)
                                                पटिनिवत्त r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                सचे (!strextend(&s, t))
                                        पटिनिवत्त -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        भङ्ग;

                पक्ष VARIABLE_RAW:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        सचे (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                सचे (!strextend(&s, t))
                                        पटिनिवत्त -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        भङ्ग;
                }

        सचे (state == VARIABLE_RAW) {
                वर्ण *t;

                प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (!strextend(&s, t))
                        पटिनिवत्त -ENOMEM;

        } अञ्ञथा सचे (!strextendn(&s, word, e-word))
                पटिनिवत्त -ENOMEM;

        सचे (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        सचे (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        सचे (ret)
                *ret = TAKE_PTR(s);

        पटिनिवत्त 0;
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
        सचे (!n)
                पटिनिवत्त -ENOMEM;

        STRV_FOREACH(i, argv) {
                ध्रुव वर्ण *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                सचे (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ वर्ण **m = रिक्त;
                        ध्रुव वर्ण *name = word + 1;
                        वर्ण *e, **w;
                        size_t q;

                        सचे (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                सचे (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                अञ्ञथा सचे (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                अञ्ञथा
                                        r = 0;
                        } अञ्ञथा सचे (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        अञ्ञथा
                                r = 0;
                        सचे (r < 0)
                                पटिनिवत्त r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, परिमाण(वर्ण*));
                        सचे (!w)
                                पटिनिवत्त -ENOMEM;

                        n = w;
                        सचे (m) {
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
                सचे (r < 0)
                        पटिनिवत्त r;
                n[++k] = रिक्त;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                सचे (r < 0)
                        पटिनिवत्त r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        सचे (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        पटिनिवत्त 0;
}

पूर्णाङ्क getenv_bool(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        e = getenv(p);
        सचे (!e)
                पटिनिवत्त -ENXIO;

        पटिनिवत्त parse_boolean(e);
}

पूर्णाङ्क secure_getenv_bool(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        e = secure_getenv(p);
        सचे (!e)
                पटिनिवत्त -ENXIO;

        पटिनिवत्त parse_boolean(e);
}

पूर्णाङ्क secure_getenv_uint64(ध्रुव वर्ण *p, uint64_t *ret) {
        ध्रुव वर्ण *e;

        प्रतिज्ञा(p);

        e = secure_getenv(p);
        सचे (!e)
                पटिनिवत्त -ENXIO;

        पटिनिवत्त safe_atou64(e, ret);
}

पूर्णाङ्क set_unset_env(ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्, bool overwrite) {
        प्रतिज्ञा(name);

        सचे (मानम्)
                पटिनिवत्त RET_NERRNO(setenv(name, मानम्, overwrite));

        पटिनिवत्त RET_NERRNO(unsetenv(name));
}

पूर्णाङ्क putenv_dup(ध्रुव वर्ण *assignment, bool override) {
        ध्रुव वर्ण *e, *n;

        e = strchr(assignment, '=');
        सचे (!e)
                पटिनिवत्त -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        पटिनिवत्त RET_NERRNO(setenv(n, e + 1, override));
}

पूर्णाङ्क setenv_systemd_exec_pid(bool update_only) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        सचे (!e && update_only)
                पटिनिवत्त 0;

        सचे (streq_ptr(e, "*"))
                पटिनिवत्त 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 1;
}

पूर्णाङ्क setenv_systemd_log_level(शून्य) {
        _cleanup_free_ वर्ण *val = रिक्त;
        पूर्णाङ्क r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

पूर्णाङ्क getenv_path_list(ध्रुव वर्ण *name, वर्ण ***ret_paths) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret_paths);

        e = secure_getenv(name);
        सचे (!e)
                पटिनिवत्त -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                सचे (!path_is_absolute(*p))
                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                सचे (!path_is_normalized(*p))
                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                सचे (path_equal(*p, "/"))
                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        सचे (strv_isempty(l))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        पटिनिवत्त 1;
}

पूर्णाङ्क getenv_steal_erase(ध्रुव वर्ण *name, वर्ण **ret) {
        _cleanup_(erase_and_freep) वर्ण *a = रिक्त;
        वर्ण *e;

        प्रतिज्ञा(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        सचे (!e) {
                सचे (ret)
                        *ret = रिक्त;
                पटिनिवत्त 0;
        }

        सचे (ret) {
                a = strdup(e);
                सचे (!a)
                        पटिनिवत्त -ENOMEM;
        }

        string_erase(e);

        सचे (unsetenv(name) < 0)
                पटिनिवत्त -errno;

        सचे (ret)
                *ret = TAKE_PTR(a);

        पटिनिवत्त 1;
}

पूर्णाङ्क setenvf(ध्रुव वर्ण *name, bool overwrite, ध्रुव वर्ण *valuef, ...) {
        _cleanup_free_ वर्ण *मानम् = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        सचे (!valuef)
                पटिनिवत्त RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&मानम्, valuef, ap);
        va_end(ap);

        सचे (r < 0)
                पटिनिवत्त -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        सचे (streq_ptr(getenv(name), मानम्))
                पटिनिवत्त 0;

        पटिनिवत्त RET_NERRNO(setenv(name, मानम्, overwrite));
}
