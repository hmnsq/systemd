/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "env-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "format-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "syslog-util.h"
#समावेश "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#घोषणा VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

माप_प्रकार sc_arg_max(व्योम) {
        दीर्घ l = sysconf(_SC_ARG_MAX);
        निश्चित(l > 0);
        वापस (माप_प्रकार) l;
}

अटल bool env_name_is_valid_n(स्थिर अक्षर *e, माप_प्रकार n) {

        अगर (n == SIZE_MAX)
                n = strlen_ptr(e);

        अगर (n == 0)
                वापस false;

        निश्चित(e);

        अगर (ascii_isdigit(e[0]))
                वापस false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        अगर (_unlikely_(n > sc_arg_max() - 2))
                वापस false;

        क्रम (स्थिर अक्षर *p = e; p < e + n; p++)
                अगर (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *p))
                        वापस false;

        वापस true;
}

bool env_name_is_valid(स्थिर अक्षर *e) {
        वापस env_name_is_valid_n(e, SIZE_MAX);
}

bool env_मूल्य_is_valid(स्थिर अक्षर *e) {
        अगर (!e)
                वापस false;

        अगर (!utf8_is_valid(e))
                वापस false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        अगर (_unlikely_(म_माप(e) > sc_arg_max() - 3))
                वापस false;

        वापस true;
}

bool env_assignment_is_valid(स्थिर अक्षर *e) {
        स्थिर अक्षर *eq;

        निश्चित(e);

        eq = म_अक्षर(e, '=');
        अगर (!eq)
                वापस false;

        अगर (!env_name_is_valid_n(e, eq - e))
                वापस false;

        अगर (!env_मूल्य_is_valid(eq + 1))
                वापस false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        अगर (_unlikely_(म_माप(e) > sc_arg_max() - 1))
                वापस false;

        वापस true;
}

bool strv_env_is_valid(अक्षर * स्थिर *e) {
        STRV_FOREACH(p, e) {
                माप_प्रकार k;

                अगर (!env_assignment_is_valid(*p))
                        वापस false;

                /* Check if there are duplicate assignments */
                k = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        अगर (strneq(*p, *q, k) && (*q)[k] == '=')
                                वापस false;
        }

        वापस true;
}

bool strv_env_name_is_valid(अक्षर * स्थिर *l) {
        STRV_FOREACH(p, l) {
                अगर (!env_name_is_valid(*p))
                        वापस false;

                अगर (strv_contains(p + 1, *p))
                        वापस false;
        }

        वापस true;
}

bool strv_env_name_or_assignment_is_valid(अक्षर * स्थिर *l) {
        STRV_FOREACH(p, l) {
                अगर (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        वापस false;

                अगर (strv_contains(p + 1, *p))
                        वापस false;
        }

        वापस true;
}

अटल पूर्णांक env_append(अक्षर **e, अक्षर ***k, अक्षर **a) {
        निश्चित(e);
        निश्चित(k);
        निश्चित(*k >= e);

        अगर (!a)
                वापस 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        क्रम (; *a; a++) {
                अक्षर **j, *c;
                माप_प्रकार n;

                n = म_खोज(*a, "=");
                अगर ((*a)[n] == '=')
                        n++;

                क्रम (j = e; j < *k; j++)
                        अगर (strneq(*j, *a, n))
                                अवरोध;

                c = strdup(*a);
                अगर (!c)
                        वापस -ENOMEM;

                अगर (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = शून्य;
                        (*k)++;
                } अन्यथा
                        free_and_replace(*j, c); /* Override existing item */
        }

        वापस 0;
}

अक्षर** _strv_env_merge(अक्षर **first, ...) {
        _cleanup_strv_free_ अक्षर **merged = शून्य;
        अक्षर **k;
        बहु_सूची ap;

        /* Merges an arbitrary number of environment sets */

        माप_प्रकार n = strv_लंबाई(first);

        बहु_शुरू(ap, first);
        क्रम (;;) {
                अक्षर **l;

                l = बहु_तर्क(ap, अक्षर**);
                अगर (l == POINTER_MAX)
                        अवरोध;

                n += strv_लंबाई(l);
        }
        बहु_पूर्ण(ap);

        k = merged = new(अक्षर*, n + 1);
        अगर (!merged)
                वापस शून्य;
        merged[0] = शून्य;

        अगर (env_append(merged, &k, first) < 0)
                वापस शून्य;

        बहु_शुरू(ap, first);
        क्रम (;;) {
                अक्षर **l;

                l = बहु_तर्क(ap, अक्षर**);
                अगर (l == POINTER_MAX)
                        अवरोध;

                अगर (env_append(merged, &k, l) < 0) {
                        बहु_पूर्ण(ap);
                        वापस शून्य;
                }
        }
        बहु_पूर्ण(ap);

        वापस TAKE_PTR(merged);
}

अटल bool env_match(स्थिर अक्षर *t, स्थिर अक्षर *pattern) {
        निश्चित(t);
        निश्चित(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        अगर (streq(t, pattern))
                वापस true;

        अगर (!म_अक्षर(pattern, '=')) {
                t = startswith(t, pattern);

                वापस t && *t == '=';
        }

        वापस false;
}

अटल bool env_entry_has_name(स्थिर अक्षर *entry, स्थिर अक्षर *name) {
        स्थिर अक्षर *t;

        निश्चित(entry);
        निश्चित(name);

        t = startswith(entry, name);
        अगर (!t)
                वापस false;

        वापस *t == '=';
}

अक्षर** strv_env_delete(अक्षर **x, माप_प्रकार n_lists, ...) {
        माप_प्रकार n, i = 0;
        _cleanup_strv_free_ अक्षर **t = शून्य;
        बहु_सूची ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_लंबाई(x);

        t = new(अक्षर*, n+1);
        अगर (!t)
                वापस शून्य;

        STRV_FOREACH(k, x) {
                बहु_शुरू(ap, n_lists);
                क्रम (माप_प्रकार v = 0; v < n_lists; v++) {
                        अक्षर **l;

                        l = बहु_तर्क(ap, अक्षर**);
                        STRV_FOREACH(j, l)
                                अगर (env_match(*k, *j))
                                        जाओ skip;
                }
                बहु_पूर्ण(ap);

                t[i] = strdup(*k);
                अगर (!t[i])
                        वापस शून्य;

                i++;
                जारी;

        skip:
                बहु_पूर्ण(ap);
        }

        t[i] = शून्य;

        निश्चित(i <= n);

        वापस TAKE_PTR(t);
}

अक्षर** strv_env_unset(अक्षर **l, स्थिर अक्षर *p) {
        निश्चित(p);

        अगर (!l)
                वापस शून्य;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        अक्षर **f, **t;
        क्रम (f = t = l; *f; f++) {
                अगर (env_match(*f, p)) {
                        मुक्त(*f);
                        जारी;
                }

                *(t++) = *f;
        }

        *t = शून्य;
        वापस l;
}

अक्षर** strv_env_unset_many_internal(अक्षर **l, ...) {
        अगर (!l)
                वापस शून्य;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        अक्षर **f, **t;
        क्रम (f = t = l; *f; f++) {
                bool found = false;
                स्थिर अक्षर *p;
                बहु_सूची ap;

                बहु_शुरू(ap, l);

                जबतक ((p = बहु_तर्क(ap, स्थिर अक्षर*)))
                        अगर (env_match(*f, p)) {
                                found = true;
                                अवरोध;
                        }

                बहु_पूर्ण(ap);

                अगर (found) {
                        मुक्त(*f);
                        जारी;
                }

                *(t++) = *f;
        }

        *t = शून्य;
        वापस l;
}

पूर्णांक strv_env_replace_consume(अक्षर ***l, अक्षर *p) {
        स्थिर अक्षर *t, *name;
        पूर्णांक r;

        निश्चित(l);
        निश्चित(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = म_अक्षर(p, '=');
        अगर (!t) {
                मुक्त(p);
                वापस -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                अगर (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        वापस 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        अगर (r < 0)
                वापस r;

        वापस 1;
}

पूर्णांक strv_env_replace_strdup(अक्षर ***l, स्थिर अक्षर *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        निश्चित(l);
        निश्चित(assignment);

        अक्षर *p = strdup(assignment);
        अगर (!p)
                वापस -ENOMEM;

        वापस strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_replace_strdup_passthrough(अक्षर ***l, स्थिर अक्षर *assignment) {
        अक्षर *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        निश्चित(l);
        निश्चित(assignment);

        अगर (म_अक्षर(assignment, '=')) {
                अगर (!env_assignment_is_valid(assignment))
                        वापस -EINVAL;

                p = strdup(assignment);
        } अन्यथा {
                अगर (!env_name_is_valid(assignment))
                        वापस -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        अगर (!p)
                वापस -ENOMEM;

        वापस strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_assign(अक्षर ***l, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        निश्चित(l);

        अगर (!env_name_is_valid(कुंजी))
                वापस -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        अगर (!मूल्य) {
                strv_env_unset(*l, कुंजी);
                वापस 0;
        }

        अक्षर *p = strjoin(कुंजी, "=", मूल्य);
        अगर (!p)
                वापस -ENOMEM;

        वापस strv_env_replace_consume(l, p);
}

पूर्णांक strv_env_assignf(अक्षर ***l, स्थिर अक्षर *कुंजी, स्थिर अक्षर *valuef, ...) {
        पूर्णांक r;

        निश्चित(l);
        निश्चित(कुंजी);

        अगर (!env_name_is_valid(कुंजी))
                वापस -EINVAL;

        अगर (!valuef) {
                strv_env_unset(*l, कुंजी);
                वापस 0;
        }

        _cleanup_free_ अक्षर *मूल्य = शून्य;
        बहु_सूची ap;
        बहु_शुरू(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        बहु_पूर्ण(ap);
        अगर (r < 0)
                वापस -ENOMEM;

        अक्षर *p = strjoin(कुंजी, "=", मूल्य);
        अगर (!p)
                वापस -ENOMEM;

        वापस strv_env_replace_consume(l, p);
}

पूर्णांक _strv_env_assign_many(अक्षर ***l, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(l);

        बहु_शुरू(ap, l);
        क्रम (;;) {
                स्थिर अक्षर *कुंजी, *मूल्य;

                कुंजी = बहु_तर्क(ap, स्थिर अक्षर *);
                अगर (!कुंजी)
                        अवरोध;

                अगर (!env_name_is_valid(कुंजी)) {
                        बहु_पूर्ण(ap);
                        वापस -EINVAL;
                }

                मूल्य = बहु_तर्क(ap, स्थिर अक्षर *);
                अगर (!मूल्य) {
                        strv_env_unset(*l, कुंजी);
                        जारी;
                }

                अक्षर *p = strjoin(कुंजी, "=", मूल्य);
                अगर (!p) {
                        बहु_पूर्ण(ap);
                        वापस -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                अगर (r < 0) {
                        बहु_पूर्ण(ap);
                        वापस r;
                }
        }
        बहु_पूर्ण(ap);

        वापस 0;
}

अक्षर* strv_env_get_n(अक्षर * स्थिर *l, स्थिर अक्षर *name, माप_प्रकार k, ReplaceEnvFlags flags) {
        निश्चित(name);

        अगर (k == SIZE_MAX)
                k = म_माप(name);
        अगर (k <= 0)
                वापस शून्य;

        STRV_FOREACH_BACKWARDS(i, l)
                अगर (strneq(*i, name, k) && (*i)[k] == '=')
                        वापस (अक्षर*) *i + k + 1;

        अगर (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                स्थिर अक्षर *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                अगर (k > sc_arg_max() - 2)
                        वापस शून्य;

                t = strndupa_safe(name, k);
                वापस secure_getenv(t);
        };

        वापस शून्य;
}

अक्षर* strv_env_pairs_get(अक्षर **l, स्थिर अक्षर *name) {
        अक्षर *परिणाम = शून्य;

        निश्चित(name);

        STRV_FOREACH_PAIR(कुंजी, मूल्य, l)
                अगर (streq(*कुंजी, name))
                        परिणाम = *मूल्य;

        वापस परिणाम;
}

पूर्णांक strv_env_get_merged(अक्षर **l, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **v = शून्य;
        माप_प्रकार n = 0;
        पूर्णांक r;

        निश्चित(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(कुंजी, मूल्य, l) {
                अक्षर *s;

                s = strjoin(*कुंजी, "=", *मूल्य);
                अगर (!s)
                        वापस -ENOMEM;

                r = strv_consume_with_आकार(&v, &n, s);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(v);
        वापस 0;
}

अक्षर** strv_env_clean_with_callback(अक्षर **l, व्योम (*invalid_callback)(स्थिर अक्षर *p, व्योम *userdata), व्योम *userdata) {
        पूर्णांक k = 0;

        STRV_FOREACH(p, l) {
                माप_प्रकार n;
                bool duplicate = false;

                अगर (!env_assignment_is_valid(*p)) {
                        अगर (invalid_callback)
                                invalid_callback(*p, userdata);
                        मुक्त(*p);
                        जारी;
                }

                n = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        अगर (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                अवरोध;
                        }

                अगर (duplicate) {
                        मुक्त(*p);
                        जारी;
                }

                l[k++] = *p;
        }

        अगर (l)
                l[k] = शून्य;

        वापस l;
}

अटल पूर्णांक strv_extend_with_लंबाई(अक्षर ***l, स्थिर अक्षर *s, माप_प्रकार n) {
        अक्षर *c;

        c = strndup(s, n);
        अगर (!c)
                वापस -ENOMEM;

        वापस strv_consume(l, c);
}

अटल पूर्णांक strv_env_get_n_validated(
                अक्षर **env,
                स्थिर अक्षर *name,
                माप_प्रकार l,
                ReplaceEnvFlags flags,
                अक्षर **ret,              /* points into the env block! do not free! */
                अक्षर ***unset_variables, /* updated in place */
                अक्षर ***bad_variables) { /* ditto */

        अक्षर *e;
        पूर्णांक r;

        निश्चित(l == 0 || name);
        निश्चित(ret);

        अगर (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                अगर (!e && unset_variables) {
                        r = strv_extend_with_लंबाई(unset_variables, name, l);
                        अगर (r < 0)
                                वापस r;
                }
        } अन्यथा {
                e = शून्य; /* Resolve invalid variable names the same way as unset ones */

                अगर (bad_variables) {
                        r = strv_extend_with_लंबाई(bad_variables, name, l);
                        अगर (r < 0)
                                वापस r;
                }
        }

        *ret = e;
        वापस !!e;
}

पूर्णांक replace_env_full(
                स्थिर अक्षर *format,
                माप_प्रकार length,
                अक्षर **env,
                ReplaceEnvFlags flags,
                अक्षर **ret,
                अक्षर ***ret_unset_variables,
                अक्षर ***ret_bad_variables) {

        क्रमागत {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_मूल्य,
                ALTERNATE_मूल्य,
        } state = WORD;

        _cleanup_strv_free_ अक्षर **unset_variables = शून्य, **bad_variables = शून्य;
        स्थिर अक्षर *e, *word = format, *test_मूल्य = शून्य; /* test_value is initialized to appease gcc */
        _cleanup_free_ अक्षर *s = शून्य;
        अक्षर ***pu, ***pb;
        माप_प्रकार i, len = 0; /* len is initialized to appease gcc */
        पूर्णांक nest = 0, r;

        निश्चित(format);

        अगर (length == SIZE_MAX)
                length = म_माप(format);

        pu = ret_unset_variables ? &unset_variables : शून्य;
        pb = ret_bad_variables ? &bad_variables : शून्य;

        क्रम (e = format, i = 0; *e && i < length; e++, i++)
                चयन (state) {

                हाल WORD:
                        अगर (*e == '$')
                                state = CURLY;
                        अवरोध;

                हाल CURLY:
                        अगर (*e == '{') {
                                अगर (!strextendn(&s, word, e-word-1))
                                        वापस -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } अन्यथा अगर (*e == '$') {
                                अगर (!strextendn(&s, word, e-word))
                                        वापस -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } अन्यथा अगर (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                अगर (!strextendn(&s, word, e-word-1))
                                        वापस -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } अन्यथा
                                state = WORD;
                        अवरोध;

                हाल VARIABLE:
                        अगर (*e == '}') {
                                अक्षर *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                अगर (r < 0)
                                        वापस r;

                                अगर (!strextend(&s, t))
                                        वापस -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } अन्यथा अगर (*e == ':') {
                                अगर (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } अन्यथा
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        अवरोध;

                हाल TEST:
                        अगर (*e == '-')
                                state = DEFAULT_मूल्य;
                        अन्यथा अगर (*e == '+')
                                state = ALTERNATE_मूल्य;
                        अन्यथा {
                                state = WORD;
                                अवरोध;
                        }

                        test_मूल्य = e+1;
                        अवरोध;

                हाल DEFAULT_मूल्य: /* fall through */
                हाल ALTERNATE_मूल्य:
                        निश्चित(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        अगर (*e == '{') {
                                nest++;
                                अवरोध;
                        }

                        अगर (*e != '}')
                                अवरोध;

                        nest--;
                        अगर (nest == 0) {
                                _cleanup_strv_free_ अक्षर **u = शून्य, **b = शून्य;
                                _cleanup_free_ अक्षर *v = शून्य;
                                अक्षर *t = शून्य;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                अगर (r < 0)
                                        वापस r;

                                अगर (t && state == ALTERNATE_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : शून्य, pb ? &b : शून्य);
                                        अगर (r < 0)
                                                वापस r;

                                        t = v;
                                } अन्यथा अगर (!t && state == DEFAULT_मूल्य) {
                                        r = replace_env_full(test_मूल्य, e-test_मूल्य, env, flags, &v, pu ? &u : शून्य, pb ? &b : शून्य);
                                        अगर (r < 0)
                                                वापस r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                अगर (r < 0)
                                        वापस r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                अगर (r < 0)
                                        वापस r;

                                अगर (!strextend(&s, t))
                                        वापस -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        अवरोध;

                हाल VARIABLE_RAW:
                        निश्चित(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        अगर (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                अक्षर *t = शून्य;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                अगर (r < 0)
                                        वापस r;

                                अगर (!strextend(&s, t))
                                        वापस -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        अवरोध;
                }

        अगर (state == VARIABLE_RAW) {
                अक्षर *t;

                निश्चित(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                अगर (r < 0)
                        वापस r;

                अगर (!strextend(&s, t))
                        वापस -ENOMEM;

        } अन्यथा अगर (!strextendn(&s, word, e-word))
                वापस -ENOMEM;

        अगर (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        अगर (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        अगर (ret)
                *ret = TAKE_PTR(s);

        वापस 0;
}

पूर्णांक replace_env_argv(
                अक्षर **argv,
                अक्षर **env,
                अक्षर ***ret,
                अक्षर ***ret_unset_variables,
                अक्षर ***ret_bad_variables) {

        _cleanup_strv_free_ अक्षर **n = शून्य, **unset_variables = शून्य, **bad_variables = शून्य;
        माप_प्रकार k = 0, l;
        पूर्णांक r;

        निश्चित(!strv_isempty(argv));
        निश्चित(ret);

        l = strv_लंबाई(argv);

        n = new(अक्षर*, l+1);
        अगर (!n)
                वापस -ENOMEM;

        STRV_FOREACH(i, argv) {
                स्थिर अक्षर *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                अगर (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ अक्षर **m = शून्य;
                        स्थिर अक्षर *name = word + 1;
                        अक्षर *e, **w;
                        माप_प्रकार q;

                        अगर (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                अगर (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                अन्यथा अगर (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                अन्यथा
                                        r = 0;
                        } अन्यथा अगर (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        अन्यथा
                                r = 0;
                        अगर (r < 0)
                                वापस r;

                        q = strv_लंबाई(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, माप(अक्षर*));
                        अगर (!w)
                                वापस -ENOMEM;

                        n = w;
                        अगर (m) {
                                स_नकल(n + k, m, (q + 1) * माप(अक्षर*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = शून्य;
                        जारी;
                }

                _cleanup_strv_free_ अक्षर **u = शून्य, **b = शून्य;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : शून्य,
                                ret_bad_variables ? &b : शून्य);
                अगर (r < 0)
                        वापस r;
                n[++k] = शून्य;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                अगर (r < 0)
                        वापस r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        अगर (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        वापस 0;
}

पूर्णांक getenv_bool(स्थिर अक्षर *p) {
        स्थिर अक्षर *e;

        e = दो_पर्या(p);
        अगर (!e)
                वापस -ENXIO;

        वापस parse_boolean(e);
}

पूर्णांक secure_getenv_bool(स्थिर अक्षर *p) {
        स्थिर अक्षर *e;

        e = secure_getenv(p);
        अगर (!e)
                वापस -ENXIO;

        वापस parse_boolean(e);
}

पूर्णांक secure_getenv_uint64(स्थिर अक्षर *p, uint64_t *ret) {
        स्थिर अक्षर *e;

        निश्चित(p);

        e = secure_getenv(p);
        अगर (!e)
                वापस -ENXIO;

        वापस safe_atou64(e, ret);
}

पूर्णांक set_unset_env(स्थिर अक्षर *name, स्थिर अक्षर *मूल्य, bool overwrite) {
        निश्चित(name);

        अगर (मूल्य)
                वापस RET_NERRNO(setenv(name, मूल्य, overwrite));

        वापस RET_NERRNO(unsetenv(name));
}

पूर्णांक putenv_dup(स्थिर अक्षर *assignment, bool override) {
        स्थिर अक्षर *e, *n;

        e = म_अक्षर(assignment, '=');
        अगर (!e)
                वापस -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        वापस RET_NERRNO(setenv(n, e + 1, override));
}

पूर्णांक setenv_systemd_exec_pid(bool update_only) {
        स्थिर अक्षर *e;
        पूर्णांक r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        अगर (!e && update_only)
                वापस 0;

        अगर (streq_ptr(e, "*"))
                वापस 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        अगर (r < 0)
                वापस r;

        वापस 1;
}

पूर्णांक setenv_systemd_log_level(व्योम) {
        _cleanup_free_ अक्षर *val = शून्य;
        पूर्णांक r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        अगर (r < 0)
                वापस r;

        वापस RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

पूर्णांक getenv_path_list(स्थिर अक्षर *name, अक्षर ***ret_paths) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        स्थिर अक्षर *e;
        पूर्णांक r;

        निश्चित(name);
        निश्चित(ret_paths);

        e = secure_getenv(name);
        अगर (!e)
                वापस -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                अगर (!path_is_absolute(*p))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                अगर (!path_is_normalized(*p))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                अगर (path_equal(*p, "/"))
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        अगर (strv_isempty(l))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        वापस 1;
}

पूर्णांक getenv_steal_erase(स्थिर अक्षर *name, अक्षर **ret) {
        _cleanup_(erase_and_freep) अक्षर *a = शून्य;
        अक्षर *e;

        निश्चित(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        अगर (!e) {
                अगर (ret)
                        *ret = शून्य;
                वापस 0;
        }

        अगर (ret) {
                a = strdup(e);
                अगर (!a)
                        वापस -ENOMEM;
        }

        string_erase(e);

        अगर (unsetenv(name) < 0)
                वापस -त्रुटि_सं;

        अगर (ret)
                *ret = TAKE_PTR(a);

        वापस 1;
}

पूर्णांक setenvf(स्थिर अक्षर *name, bool overwrite, स्थिर अक्षर *valuef, ...) {
        _cleanup_free_ अक्षर *मूल्य = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(name);

        अगर (!valuef)
                वापस RET_NERRNO(unsetenv(name));

        बहु_शुरू(ap, valuef);
        r = vasprintf(&मूल्य, valuef, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        अगर (streq_ptr(दो_पर्या(name), मूल्य))
                वापस 0;

        वापस RET_NERRNO(setenv(name, मूल्य, overwrite));
}
