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

माप_प्रकार sc_arg_max(शून्य) {
        दीर्घ l = sysconf(_SC_ARG_MAX);
        प्रतिज्ञा(l > 0);
        परत (माप_प्रकार) l;
}

स्थिर bool env_name_is_valid_n(स्थिरांक अक्षर *e, माप_प्रकार n) {

        जर (n == SIZE_MAX)
                n = strlen_ptr(e);

        जर (n == 0)
                परत false;

        प्रतिज्ञा(e);

        जर (ascii_isdigit(e[0]))
                परत false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        जर (_unlikely_(n > sc_arg_max() - 2))
                परत false;

        साठी (स्थिरांक अक्षर *p = e; p < e + n; p++)
                जर (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *p))
                        परत false;

        परत true;
}

bool env_name_is_valid(स्थिरांक अक्षर *e) {
        परत env_name_is_valid_n(e, SIZE_MAX);
}

bool env_मूल्य_is_valid(स्थिरांक अक्षर *e) {
        जर (!e)
                परत false;

        जर (!utf8_is_valid(e))
                परत false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        जर (_unlikely_(म_माप(e) > sc_arg_max() - 3))
                परत false;

        परत true;
}

bool env_assignment_is_valid(स्थिरांक अक्षर *e) {
        स्थिरांक अक्षर *eq;

        प्रतिज्ञा(e);

        eq = म_अक्षर(e, '=');
        जर (!eq)
                परत false;

        जर (!env_name_is_valid_n(e, eq - e))
                परत false;

        जर (!env_मूल्य_is_valid(eq + 1))
                परत false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        जर (_unlikely_(म_माप(e) > sc_arg_max() - 1))
                परत false;

        परत true;
}

bool strv_env_is_valid(अक्षर * स्थिरांक *e) {
        STRV_FOREACH(p, e) {
                माप_प्रकार k;

                जर (!env_assignment_is_valid(*p))
                        परत false;

                /* Check if there are duplicate assignments */
                k = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        जर (strneq(*p, *q, k) && (*q)[k] == '=')
                                परत false;
        }

        परत true;
}

bool strv_env_name_is_valid(अक्षर * स्थिरांक *l) {
        STRV_FOREACH(p, l) {
                जर (!env_name_is_valid(*p))
                        परत false;

                जर (strv_contains(p + 1, *p))
                        परत false;
        }

        परत true;
}

bool strv_env_name_or_assignment_is_valid(अक्षर * स्थिरांक *l) {
        STRV_FOREACH(p, l) {
                जर (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        परत false;

                जर (strv_contains(p + 1, *p))
                        परत false;
        }

        परत true;
}

स्थिर पूर्णांक env_append(अक्षर **e, अक्षर ***k, अक्षर **a) {
        प्रतिज्ञा(e);
        प्रतिज्ञा(k);
        प्रतिज्ञा(*k >= e);

        जर (!a)
                परत 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        साठी (; *a; a++) {
                अक्षर **j, *c;
                माप_प्रकार n;

                n = म_खोज(*a, "=");
                जर ((*a)[n] == '=')
                        n++;

                साठी (j = e; j < *k; j++)
                        जर (strneq(*j, *a, n))
                                थांबा;

                c = strdup(*a);
                जर (!c)
                        परत -ENOMEM;

                जर (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = रिक्त;
                        (*k)++;
                } अन्यथा
                        free_and_replace(*j, c); /* Override existing item */
        }

        परत 0;
}

अक्षर** _strv_env_merge(अक्षर **first, ...) {
        _cleanup_strv_free_ अक्षर **merged = रिक्त;
        अक्षर **k;
        बहु_सूची ap;

        /* Merges an arbitrary number of environment sets */

        माप_प्रकार n = strv_length(first);

        बहु_शुरू(ap, first);
        साठी (;;) {
                अक्षर **l;

                l = बहु_तर्क(ap, अक्षर**);
                जर (l == POINTER_MAX)
                        थांबा;

                n += strv_length(l);
        }
        बहु_पूर्ण(ap);

        k = merged = new(अक्षर*, n + 1);
        जर (!merged)
                परत रिक्त;
        merged[0] = रिक्त;

        जर (env_append(merged, &k, first) < 0)
                परत रिक्त;

        बहु_शुरू(ap, first);
        साठी (;;) {
                अक्षर **l;

                l = बहु_तर्क(ap, अक्षर**);
                जर (l == POINTER_MAX)
                        थांबा;

                जर (env_append(merged, &k, l) < 0) {
                        बहु_पूर्ण(ap);
                        परत रिक्त;
                }
        }
        बहु_पूर्ण(ap);

        परत TAKE_PTR(merged);
}

स्थिर bool env_match(स्थिरांक अक्षर *t, स्थिरांक अक्षर *pattern) {
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

        जर (streq(t, pattern))
                परत true;

        जर (!म_अक्षर(pattern, '=')) {
                t = startswith(t, pattern);

                परत t && *t == '=';
        }

        परत false;
}

स्थिर bool env_entry_has_name(स्थिरांक अक्षर *entry, स्थिरांक अक्षर *name) {
        स्थिरांक अक्षर *t;

        प्रतिज्ञा(entry);
        प्रतिज्ञा(name);

        t = startswith(entry, name);
        जर (!t)
                परत false;

        परत *t == '=';
}

अक्षर** strv_env_delete(अक्षर **x, माप_प्रकार n_lists, ...) {
        माप_प्रकार n, i = 0;
        _cleanup_strv_free_ अक्षर **t = रिक्त;
        बहु_सूची ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(अक्षर*, n+1);
        जर (!t)
                परत रिक्त;

        STRV_FOREACH(k, x) {
                बहु_शुरू(ap, n_lists);
                साठी (माप_प्रकार v = 0; v < n_lists; v++) {
                        अक्षर **l;

                        l = बहु_तर्क(ap, अक्षर**);
                        STRV_FOREACH(j, l)
                                जर (env_match(*k, *j))
                                        जा skip;
                }
                बहु_पूर्ण(ap);

                t[i] = strdup(*k);
                जर (!t[i])
                        परत रिक्त;

                i++;
                सुरूठेवा;

        skip:
                बहु_पूर्ण(ap);
        }

        t[i] = रिक्त;

        प्रतिज्ञा(i <= n);

        परत TAKE_PTR(t);
}

अक्षर** strv_env_unset(अक्षर **l, स्थिरांक अक्षर *p) {
        प्रतिज्ञा(p);

        जर (!l)
                परत रिक्त;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        अक्षर **f, **t;
        साठी (f = t = l; *f; f++) {
                जर (env_match(*f, p)) {
                        मुक्त(*f);
                        सुरूठेवा;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        परत l;
}

अक्षर** strv_env_unset_many_internal(अक्षर **l, ...) {
        जर (!l)
                परत रिक्त;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        अक्षर **f, **t;
        साठी (f = t = l; *f; f++) {
                bool found = false;
                स्थिरांक अक्षर *p;
                बहु_सूची ap;

                बहु_शुरू(ap, l);

                पर्यंत ((p = बहु_तर्क(ap, स्थिरांक अक्षर*)))
                        जर (env_match(*f, p)) {
                                found = true;
                                थांबा;
                        }

                बहु_पूर्ण(ap);

                जर (found) {
                        मुक्त(*f);
                        सुरूठेवा;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        परत l;
}

पूर्णांक strv_env_replace_consume(अक्षर ***l, अक्षर *p) {
        स्थिरांक अक्षर *t, *name;
        पूर्णांक r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = म_अक्षर(p, '=');
        जर (!t) {
                मुक्त(p);
                परत -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                जर (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        परत 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        जर (r < 0)
                परत r;

        परत 1;
}

पूर्णांक strv_env_replace_strdup(अक्षर ***l, स्थिरांक अक्षर *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        अक्षर *p = strdup(assignment);
        जर (!p)
                परत -ENOMEM;

        परत strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_replace_strdup_passthrough(अक्षर ***l, स्थिरांक अक्षर *assignment) {
        अक्षर *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        जर (म_अक्षर(assignment, '=')) {
                जर (!env_assignment_is_valid(assignment))
                        परत -EINVAL;

                p = strdup(assignment);
        } अन्यथा {
                जर (!env_name_is_valid(assignment))
                        परत -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        जर (!p)
                परत -ENOMEM;

        परत strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_assign(अक्षर ***l, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        प्रतिज्ञा(l);

        जर (!env_name_is_valid(किल्ली))
                परत -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        जर (!मूल्य) {
                strv_env_unset(*l, किल्ली);
                परत 0;
        }

        अक्षर *p = strjoin(किल्ली, "=", मूल्य);
        जर (!p)
                परत -ENOMEM;

        परत strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_assignf(अक्षर ***l, स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *valuef, ...) {
        पूर्णांक r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(किल्ली);

        जर (!env_name_is_valid(किल्ली))
                परत -EINVAL;

        जर (!valuef) {
                strv_env_unset(*l, किल्ली);
                परत 0;
        }

        _cleanup_free_ अक्षर *मूल्य = रिक्त;
        बहु_सूची ap;
        बहु_शुरू(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        बहु_पूर्ण(ap);
        जर (r < 0)
                परत -ENOMEM;

        अक्षर *p = strjoin(किल्ली, "=", मूल्य);
        जर (!p)
                परत -ENOMEM;

        परत strv_env_replace_consume(l, p);
}

पूर्णांक _strv_env_assign_many(अक्षर ***l, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        प्रतिज्ञा(l);

        बहु_शुरू(ap, l);
        साठी (;;) {
                स्थिरांक अक्षर *किल्ली, *मूल्य;

                किल्ली = बहु_तर्क(ap, स्थिरांक अक्षर *);
                जर (!किल्ली)
                        थांबा;

                जर (!env_name_is_valid(किल्ली)) {
                        बहु_पूर्ण(ap);
                        परत -EINVAL;
                }

                मूल्य = बहु_तर्क(ap, स्थिरांक अक्षर *);
                जर (!मूल्य) {
                        strv_env_unset(*l, किल्ली);
                        सुरूठेवा;
                }

                अक्षर *p = strjoin(किल्ली, "=", मूल्य);
                जर (!p) {
                        बहु_पूर्ण(ap);
                        परत -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                जर (r < 0) {
                        बहु_पूर्ण(ap);
                        परत r;
                }
        }
        बहु_पूर्ण(ap);

        परत 0;
}

अक्षर* strv_env_get_n(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *name, माप_प्रकार k, ReplaceEnvFlags flags) {
        प्रतिज्ञा(name);

        जर (k == SIZE_MAX)
                k = म_माप(name);
        जर (k <= 0)
                परत रिक्त;

        STRV_FOREACH_BACKWARDS(i, l)
                जर (strneq(*i, name, k) && (*i)[k] == '=')
                        परत (अक्षर*) *i + k + 1;

        जर (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                स्थिरांक अक्षर *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                जर (k > sc_arg_max() - 2)
                        परत रिक्त;

                t = strndupa_safe(name, k);
                परत secure_getenv(t);
        };

        परत रिक्त;
}

अक्षर* strv_env_pairs_get(अक्षर **l, स्थिरांक अक्षर *name) {
        अक्षर *result = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH_PAIR(किल्ली, मूल्य, l)
                जर (streq(*किल्ली, name))
                        result = *मूल्य;

        परत result;
}

पूर्णांक strv_env_get_merged(अक्षर **l, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **v = रिक्त;
        माप_प्रकार n = 0;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(किल्ली, मूल्य, l) {
                अक्षर *s;

                s = strjoin(*किल्ली, "=", *मूल्य);
                जर (!s)
                        परत -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                जर (r < 0)
                        परत r;
        }

        *ret = TAKE_PTR(v);
        परत 0;
}

अक्षर** strv_env_clean_with_callback(अक्षर **l, शून्य (*invalid_callback)(स्थिरांक अक्षर *p, शून्य *userdata), शून्य *userdata) {
        पूर्णांक k = 0;

        STRV_FOREACH(p, l) {
                माप_प्रकार n;
                bool duplicate = false;

                जर (!env_assignment_is_valid(*p)) {
                        जर (invalid_callback)
                                invalid_callback(*p, userdata);
                        मुक्त(*p);
                        सुरूठेवा;
                }

                n = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        जर (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                थांबा;
                        }

                जर (duplicate) {
                        मुक्त(*p);
                        सुरूठेवा;
                }

                l[k++] = *p;
        }

        जर (l)
                l[k] = रिक्त;

        परत l;
}

स्थिर पूर्णांक strv_extend_with_length(अक्षर ***l, स्थिरांक अक्षर *s, माप_प्रकार n) {
        अक्षर *c;

        c = strndup(s, n);
        जर (!c)
                परत -ENOMEM;

        परत strv_consume(l, c);
}

स्थिर पूर्णांक strv_env_get_n_validated(
                अक्षर **env,
                स्थिरांक अक्षर *name,
                माप_प्रकार l,
                ReplaceEnvFlags flags,
                अक्षर **ret,              /* points into the env block! do not free! */
                अक्षर ***unset_variables, /* updated in place */
                अक्षर ***bad_variables) { /* ditto */

        अक्षर *e;
        पूर्णांक r;

        प्रतिज्ञा(l == 0 || name);
        प्रतिज्ञा(ret);

        जर (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                जर (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        जर (r < 0)
                                परत r;
                }
        } अन्यथा {
                e = रिक्त; /* Resolve invalid variable names the same way as unset ones */

                जर (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        जर (r < 0)
                                परत r;
                }
        }

        *ret = e;
        परत !!e;
}

पूर्णांक replace_env_full(
                स्थिरांक अक्षर *format,
                माप_प्रकार length,
                अक्षर **env,
                ReplaceEnvFlags flags,
                अक्षर **ret,
                अक्षर ***ret_unset_variables,
                अक्षर ***ret_bad_variables) {

        गणना {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_मूल्य,
                ALTERNATE_मूल्य,
        } state = WORD;

        _cleanup_strv_free_ अक्षर **unset_variables = रिक्त, **bad_variables = रिक्त;
        स्थिरांक अक्षर *e, *word = format, *test_मूल्य = रिक्त; /* test_value is initialized to appease gcc */
        _cleanup_free_ अक्षर *s = रिक्त;
        अक्षर ***pu, ***pb;
        माप_प्रकार i, len = 0; /* len is initialized to appease gcc */
        पूर्णांक nest = 0, r;

        प्रतिज्ञा(format);

        जर (length == SIZE_MAX)
                length = म_माप(format);

        pu = ret_unset_variables ? &unset_variables : रिक्त;
        pb = ret_bad_variables ? &bad_variables : रिक्त;

        साठी (e = format, i = 0; *e && i < length; e++, i++)
                निवड (state) {

                स्थिती WORD:
                        जर (*e == '$')
                                state = CURLY;
                        थांबा;

                स्थिती CURLY:
                        जर (*e == '{') {
                                जर (!strextendn(&s, word, e-word-1))
                                        परत -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } अन्यथा जर (*e == '$') {
                                जर (!strextendn(&s, word, e-word))
                                        परत -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } अन्यथा जर (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                जर (!strextendn(&s, word, e-word-1))
                                        परत -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } अन्यथा
                                state = WORD;
                        थांबा;

                स्थिती VARIABLE:
                        जर (*e == '}') {
                                अक्षर *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                जर (r < 0)
                                        परत r;

                                जर (!strextend(&s, t))
                                        परत -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } अन्यथा जर (*e == ':') {
                                जर (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } अन्यथा
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        थांबा;

                स्थिती TEST:
                        जर (*e == '-')
                                state = DEFAULT_मूल्य;
                        अन्यथा जर (*e == '+')
                                state = ALTERNATE_मूल्य;
                        अन्यथा {
                                state = WORD;
                                थांबा;
                        }

                        test_मूल्य = e+1;
                        थांबा;

                स्थिती DEFAULT_मूल्य: /* fall through */
                स्थिती ALTERNATE_मूल्य:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        जर (*e == '{') {
                                nest++;
                                थांबा;
                        }

                        जर (*e != '}')
                                थांबा;

                        nest--;
                        जर (nest == 0) {
                                _cleanup_strv_free_ अक्षर **u = रिक्त, **b = रिक्त;
                                _cleanup_free_ अक्षर *v = रिक्त;
                                अक्षर *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                जर (r < 0)
                                        परत r;

                                जर (t && state == ALTERNATE_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        जर (r < 0)
                                                परत r;

                                        t = v;
                                } अन्यथा जर (!t && state == DEFAULT_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        जर (r < 0)
                                                परत r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                जर (r < 0)
                                        परत r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                जर (r < 0)
                                        परत r;

                                जर (!strextend(&s, t))
                                        परत -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        थांबा;

                स्थिती VARIABLE_RAW:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        जर (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                अक्षर *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                जर (r < 0)
                                        परत r;

                                जर (!strextend(&s, t))
                                        परत -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        थांबा;
                }

        जर (state == VARIABLE_RAW) {
                अक्षर *t;

                प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                जर (r < 0)
                        परत r;

                जर (!strextend(&s, t))
                        परत -ENOMEM;

        } अन्यथा जर (!strextendn(&s, word, e-word))
                परत -ENOMEM;

        जर (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        जर (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        जर (ret)
                *ret = TAKE_PTR(s);

        परत 0;
}

पूर्णांक replace_env_argv(
                अक्षर **argv,
                अक्षर **env,
                अक्षर ***ret,
                अक्षर ***ret_unset_variables,
                अक्षर ***ret_bad_variables) {

        _cleanup_strv_free_ अक्षर **n = रिक्त, **unset_variables = रिक्त, **bad_variables = रिक्त;
        माप_प्रकार k = 0, l;
        पूर्णांक r;

        प्रतिज्ञा(!strv_isempty(argv));
        प्रतिज्ञा(ret);

        l = strv_length(argv);

        n = new(अक्षर*, l+1);
        जर (!n)
                परत -ENOMEM;

        STRV_FOREACH(i, argv) {
                स्थिरांक अक्षर *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                जर (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ अक्षर **m = रिक्त;
                        स्थिरांक अक्षर *name = word + 1;
                        अक्षर *e, **w;
                        माप_प्रकार q;

                        जर (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                जर (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                अन्यथा जर (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                अन्यथा
                                        r = 0;
                        } अन्यथा जर (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        अन्यथा
                                r = 0;
                        जर (r < 0)
                                परत r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, आकार(अक्षर*));
                        जर (!w)
                                परत -ENOMEM;

                        n = w;
                        जर (m) {
                                स_नकल(n + k, m, (q + 1) * आकार(अक्षर*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = रिक्त;
                        सुरूठेवा;
                }

                _cleanup_strv_free_ अक्षर **u = रिक्त, **b = रिक्त;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : रिक्त,
                                ret_bad_variables ? &b : रिक्त);
                जर (r < 0)
                        परत r;
                n[++k] = रिक्त;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                जर (r < 0)
                        परत r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                जर (r < 0)
                        परत r;
        }

        जर (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        जर (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        परत 0;
}

पूर्णांक getenv_bool(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *e;

        e = दो_पर्या(p);
        जर (!e)
                परत -ENXIO;

        परत parse_boolean(e);
}

पूर्णांक secure_getenv_bool(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *e;

        e = secure_getenv(p);
        जर (!e)
                परत -ENXIO;

        परत parse_boolean(e);
}

पूर्णांक secure_getenv_uint64(स्थिरांक अक्षर *p, uint64_t *ret) {
        स्थिरांक अक्षर *e;

        प्रतिज्ञा(p);

        e = secure_getenv(p);
        जर (!e)
                परत -ENXIO;

        परत safe_atou64(e, ret);
}

पूर्णांक set_unset_env(स्थिरांक अक्षर *name, स्थिरांक अक्षर *मूल्य, bool overwrite) {
        प्रतिज्ञा(name);

        जर (मूल्य)
                परत RET_NERRNO(setenv(name, मूल्य, overwrite));

        परत RET_NERRNO(unsetenv(name));
}

पूर्णांक putenv_dup(स्थिरांक अक्षर *assignment, bool override) {
        स्थिरांक अक्षर *e, *n;

        e = म_अक्षर(assignment, '=');
        जर (!e)
                परत -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        परत RET_NERRNO(setenv(n, e + 1, override));
}

पूर्णांक setenv_systemd_exec_pid(bool update_only) {
        स्थिरांक अक्षर *e;
        पूर्णांक r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        जर (!e && update_only)
                परत 0;

        जर (streq_ptr(e, "*"))
                परत 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        जर (r < 0)
                परत r;

        परत 1;
}

पूर्णांक setenv_systemd_log_level(शून्य) {
        _cleanup_free_ अक्षर *val = रिक्त;
        पूर्णांक r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        जर (r < 0)
                परत r;

        परत RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

पूर्णांक getenv_path_list(स्थिरांक अक्षर *name, अक्षर ***ret_paths) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        स्थिरांक अक्षर *e;
        पूर्णांक r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret_paths);

        e = secure_getenv(name);
        जर (!e)
                परत -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                जर (!path_is_absolute(*p))
                        परत log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                जर (!path_is_normalized(*p))
                        परत log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                जर (path_equal(*p, "/"))
                        परत log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        जर (strv_isempty(l))
                परत log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        परत 1;
}

पूर्णांक getenv_steal_erase(स्थिरांक अक्षर *name, अक्षर **ret) {
        _cleanup_(erase_and_freep) अक्षर *a = रिक्त;
        अक्षर *e;

        प्रतिज्ञा(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        जर (!e) {
                जर (ret)
                        *ret = रिक्त;
                परत 0;
        }

        जर (ret) {
                a = strdup(e);
                जर (!a)
                        परत -ENOMEM;
        }

        string_erase(e);

        जर (unsetenv(name) < 0)
                परत -त्रुटि_सं;

        जर (ret)
                *ret = TAKE_PTR(a);

        परत 1;
}

पूर्णांक setenvf(स्थिरांक अक्षर *name, bool overwrite, स्थिरांक अक्षर *valuef, ...) {
        _cleanup_free_ अक्षर *मूल्य = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        प्रतिज्ञा(name);

        जर (!valuef)
                परत RET_NERRNO(unsetenv(name));

        बहु_शुरू(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        जर (streq_ptr(दो_पर्या(name), मूल्य))
                परत 0;

        परत RET_NERRNO(setenv(name, मूल्य, overwrite));
}
