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
        फर्काऊ (माप_प्रकार) l;
}

स्थिर bool env_name_is_valid_n(अचर वर्ण *e, माप_प्रकार n) {

        यदि (n == SIZE_MAX)
                n = strlen_ptr(e);

        यदि (n == 0)
                फर्काऊ false;

        प्रतिज्ञा(e);

        यदि (ascii_isdigit(e[0]))
                फर्काऊ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        यदि (_unlikely_(n > sc_arg_max() - 2))
                फर्काऊ false;

        लागि (अचर वर्ण *p = e; p < e + n; p++)
                यदि (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *p))
                        फर्काऊ false;

        फर्काऊ true;
}

bool env_name_is_valid(अचर वर्ण *e) {
        फर्काऊ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_मान_is_valid(अचर वर्ण *e) {
        यदि (!e)
                फर्काऊ false;

        यदि (!utf8_is_valid(e))
                फर्काऊ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        यदि (_unlikely_(म_माप(e) > sc_arg_max() - 3))
                फर्काऊ false;

        फर्काऊ true;
}

bool env_assignment_is_valid(अचर वर्ण *e) {
        अचर वर्ण *eq;

        प्रतिज्ञा(e);

        eq = म_अक्षर(e, '=');
        यदि (!eq)
                फर्काऊ false;

        यदि (!env_name_is_valid_n(e, eq - e))
                फर्काऊ false;

        यदि (!env_मान_is_valid(eq + 1))
                फर्काऊ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        यदि (_unlikely_(म_माप(e) > sc_arg_max() - 1))
                फर्काऊ false;

        फर्काऊ true;
}

bool strv_env_is_valid(वर्ण * अचर *e) {
        STRV_FOREACH(p, e) {
                माप_प्रकार k;

                यदि (!env_assignment_is_valid(*p))
                        फर्काऊ false;

                /* Check if there are duplicate assignments */
                k = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        यदि (strneq(*p, *q, k) && (*q)[k] == '=')
                                फर्काऊ false;
        }

        फर्काऊ true;
}

bool strv_env_name_is_valid(वर्ण * अचर *l) {
        STRV_FOREACH(p, l) {
                यदि (!env_name_is_valid(*p))
                        फर्काऊ false;

                यदि (strv_contains(p + 1, *p))
                        फर्काऊ false;
        }

        फर्काऊ true;
}

bool strv_env_name_or_assignment_is_valid(वर्ण * अचर *l) {
        STRV_FOREACH(p, l) {
                यदि (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        फर्काऊ false;

                यदि (strv_contains(p + 1, *p))
                        फर्काऊ false;
        }

        फर्काऊ true;
}

स्थिर पूर्णाङ्क env_append(वर्ण **e, वर्ण ***k, वर्ण **a) {
        प्रतिज्ञा(e);
        प्रतिज्ञा(k);
        प्रतिज्ञा(*k >= e);

        यदि (!a)
                फर्काऊ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        लागि (; *a; a++) {
                वर्ण **j, *c;
                माप_प्रकार n;

                n = म_खोज(*a, "=");
                यदि ((*a)[n] == '=')
                        n++;

                लागि (j = e; j < *k; j++)
                        यदि (strneq(*j, *a, n))
                                रोक;

                c = strdup(*a);
                यदि (!c)
                        फर्काऊ -ENOMEM;

                यदि (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = रिक्त;
                        (*k)++;
                } अन्यथा
                        free_and_replace(*j, c); /* Override existing item */
        }

        फर्काऊ 0;
}

वर्ण** _strv_env_merge(वर्ण **first, ...) {
        _cleanup_strv_free_ वर्ण **merged = रिक्त;
        वर्ण **k;
        बहु_सूची ap;

        /* Merges an arbitrary number of environment sets */

        माप_प्रकार n = strv_length(first);

        बहु_शुरू(ap, first);
        लागि (;;) {
                वर्ण **l;

                l = बहु_तर्क(ap, वर्ण**);
                यदि (l == POINTER_MAX)
                        रोक;

                n += strv_length(l);
        }
        बहु_पूर्ण(ap);

        k = merged = new(वर्ण*, n + 1);
        यदि (!merged)
                फर्काऊ रिक्त;
        merged[0] = रिक्त;

        यदि (env_append(merged, &k, first) < 0)
                फर्काऊ रिक्त;

        बहु_शुरू(ap, first);
        लागि (;;) {
                वर्ण **l;

                l = बहु_तर्क(ap, वर्ण**);
                यदि (l == POINTER_MAX)
                        रोक;

                यदि (env_append(merged, &k, l) < 0) {
                        बहु_पूर्ण(ap);
                        फर्काऊ रिक्त;
                }
        }
        बहु_पूर्ण(ap);

        फर्काऊ TAKE_PTR(merged);
}

स्थिर bool env_match(अचर वर्ण *t, अचर वर्ण *pattern) {
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
                फर्काऊ true;

        यदि (!म_अक्षर(pattern, '=')) {
                t = startswith(t, pattern);

                फर्काऊ t && *t == '=';
        }

        फर्काऊ false;
}

स्थिर bool env_entry_has_name(अचर वर्ण *entry, अचर वर्ण *name) {
        अचर वर्ण *t;

        प्रतिज्ञा(entry);
        प्रतिज्ञा(name);

        t = startswith(entry, name);
        यदि (!t)
                फर्काऊ false;

        फर्काऊ *t == '=';
}

वर्ण** strv_env_delete(वर्ण **x, माप_प्रकार n_lists, ...) {
        माप_प्रकार n, i = 0;
        _cleanup_strv_free_ वर्ण **t = रिक्त;
        बहु_सूची ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(वर्ण*, n+1);
        यदि (!t)
                फर्काऊ रिक्त;

        STRV_FOREACH(k, x) {
                बहु_शुरू(ap, n_lists);
                लागि (माप_प्रकार v = 0; v < n_lists; v++) {
                        वर्ण **l;

                        l = बहु_तर्क(ap, वर्ण**);
                        STRV_FOREACH(j, l)
                                यदि (env_match(*k, *j))
                                        गच्छ skip;
                }
                बहु_पूर्ण(ap);

                t[i] = strdup(*k);
                यदि (!t[i])
                        फर्काऊ रिक्त;

                i++;
                जारी;

        skip:
                बहु_पूर्ण(ap);
        }

        t[i] = रिक्त;

        प्रतिज्ञा(i <= n);

        फर्काऊ TAKE_PTR(t);
}

वर्ण** strv_env_unset(वर्ण **l, अचर वर्ण *p) {
        प्रतिज्ञा(p);

        यदि (!l)
                फर्काऊ रिक्त;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        वर्ण **f, **t;
        लागि (f = t = l; *f; f++) {
                यदि (env_match(*f, p)) {
                        मुक्त(*f);
                        जारी;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        फर्काऊ l;
}

वर्ण** strv_env_unset_many_internal(वर्ण **l, ...) {
        यदि (!l)
                फर्काऊ रिक्त;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        वर्ण **f, **t;
        लागि (f = t = l; *f; f++) {
                bool found = false;
                अचर वर्ण *p;
                बहु_सूची ap;

                बहु_शुरू(ap, l);

                जबसम्म ((p = बहु_तर्क(ap, अचर वर्ण*)))
                        यदि (env_match(*f, p)) {
                                found = true;
                                रोक;
                        }

                बहु_पूर्ण(ap);

                यदि (found) {
                        मुक्त(*f);
                        जारी;
                }

                *(t++) = *f;
        }

        *t = रिक्त;
        फर्काऊ l;
}

पूर्णाङ्क strv_env_replace_consume(वर्ण ***l, वर्ण *p) {
        अचर वर्ण *t, *name;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = म_अक्षर(p, '=');
        यदि (!t) {
                मुक्त(p);
                फर्काऊ -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                यदि (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        फर्काऊ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ 1;
}

पूर्णाङ्क strv_env_replace_strdup(वर्ण ***l, अचर वर्ण *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        वर्ण *p = strdup(assignment);
        यदि (!p)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_replace_strdup_passthrough(वर्ण ***l, अचर वर्ण *assignment) {
        वर्ण *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        प्रतिज्ञा(l);
        प्रतिज्ञा(assignment);

        यदि (म_अक्षर(assignment, '=')) {
                यदि (!env_assignment_is_valid(assignment))
                        फर्काऊ -EINVAL;

                p = strdup(assignment);
        } अन्यथा {
                यदि (!env_name_is_valid(assignment))
                        फर्काऊ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        यदि (!p)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assign(वर्ण ***l, अचर वर्ण *साँचो, अचर वर्ण *मान) {
        प्रतिज्ञा(l);

        यदि (!env_name_is_valid(साँचो))
                फर्काऊ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        यदि (!मान) {
                strv_env_unset(*l, साँचो);
                फर्काऊ 0;
        }

        वर्ण *p = strjoin(साँचो, "=", मान);
        यदि (!p)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_env_replace_consume(l, p);
}

पूर्णाङ्क strv_env_assignf(वर्ण ***l, अचर वर्ण *साँचो, अचर वर्ण *valuef, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(l);
        प्रतिज्ञा(साँचो);

        यदि (!env_name_is_valid(साँचो))
                फर्काऊ -EINVAL;

        यदि (!valuef) {
                strv_env_unset(*l, साँचो);
                फर्काऊ 0;
        }

        _cleanup_free_ वर्ण *मान = रिक्त;
        बहु_सूची ap;
        बहु_शुरू(ap, valuef);
        r = vasprintf(&मान, valuef, ap);
        बहु_पूर्ण(ap);
        यदि (r < 0)
                फर्काऊ -ENOMEM;

        वर्ण *p = strjoin(साँचो, "=", मान);
        यदि (!p)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_env_replace_consume(l, p);
}

पूर्णाङ्क _strv_env_assign_many(वर्ण ***l, ...) {
        बहु_सूची ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(l);

        बहु_शुरू(ap, l);
        लागि (;;) {
                अचर वर्ण *साँचो, *मान;

                साँचो = बहु_तर्क(ap, अचर वर्ण *);
                यदि (!साँचो)
                        रोक;

                यदि (!env_name_is_valid(साँचो)) {
                        बहु_पूर्ण(ap);
                        फर्काऊ -EINVAL;
                }

                मान = बहु_तर्क(ap, अचर वर्ण *);
                यदि (!मान) {
                        strv_env_unset(*l, साँचो);
                        जारी;
                }

                वर्ण *p = strjoin(साँचो, "=", मान);
                यदि (!p) {
                        बहु_पूर्ण(ap);
                        फर्काऊ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                यदि (r < 0) {
                        बहु_पूर्ण(ap);
                        फर्काऊ r;
                }
        }
        बहु_पूर्ण(ap);

        फर्काऊ 0;
}

वर्ण* strv_env_get_n(वर्ण * अचर *l, अचर वर्ण *name, माप_प्रकार k, ReplaceEnvFlags flags) {
        प्रतिज्ञा(name);

        यदि (k == SIZE_MAX)
                k = म_माप(name);
        यदि (k <= 0)
                फर्काऊ रिक्त;

        STRV_FOREACH_BACKWARDS(i, l)
                यदि (strneq(*i, name, k) && (*i)[k] == '=')
                        फर्काऊ (वर्ण*) *i + k + 1;

        यदि (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                अचर वर्ण *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                यदि (k > sc_arg_max() - 2)
                        फर्काऊ रिक्त;

                t = strndupa_safe(name, k);
                फर्काऊ secure_getenv(t);
        };

        फर्काऊ रिक्त;
}

वर्ण* strv_env_pairs_get(वर्ण **l, अचर वर्ण *name) {
        वर्ण *result = रिक्त;

        प्रतिज्ञा(name);

        STRV_FOREACH_PAIR(साँचो, मान, l)
                यदि (streq(*साँचो, name))
                        result = *मान;

        फर्काऊ result;
}

पूर्णाङ्क strv_env_get_merged(वर्ण **l, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **v = रिक्त;
        माप_प्रकार n = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(साँचो, मान, l) {
                वर्ण *s;

                s = strjoin(*साँचो, "=", *मान);
                यदि (!s)
                        फर्काऊ -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(v);
        फर्काऊ 0;
}

वर्ण** strv_env_clean_with_callback(वर्ण **l, शून्य (*invalid_callback)(अचर वर्ण *p, शून्य *userdata), शून्य *userdata) {
        पूर्णाङ्क k = 0;

        STRV_FOREACH(p, l) {
                माप_प्रकार n;
                bool duplicate = false;

                यदि (!env_assignment_is_valid(*p)) {
                        यदि (invalid_callback)
                                invalid_callback(*p, userdata);
                        मुक्त(*p);
                        जारी;
                }

                n = म_खोज(*p, "=");
                STRV_FOREACH(q, p + 1)
                        यदि (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                रोक;
                        }

                यदि (duplicate) {
                        मुक्त(*p);
                        जारी;
                }

                l[k++] = *p;
        }

        यदि (l)
                l[k] = रिक्त;

        फर्काऊ l;
}

स्थिर पूर्णाङ्क strv_extend_with_length(वर्ण ***l, अचर वर्ण *s, माप_प्रकार n) {
        वर्ण *c;

        c = strndup(s, n);
        यदि (!c)
                फर्काऊ -ENOMEM;

        फर्काऊ strv_consume(l, c);
}

स्थिर पूर्णाङ्क strv_env_get_n_validated(
                वर्ण **env,
                अचर वर्ण *name,
                माप_प्रकार l,
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
                                फर्काऊ r;
                }
        } अन्यथा {
                e = रिक्त; /* Resolve invalid variable names the same way as unset ones */

                यदि (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        यदि (r < 0)
                                फर्काऊ r;
                }
        }

        *ret = e;
        फर्काऊ !!e;
}

पूर्णाङ्क replace_env_full(
                अचर वर्ण *format,
                माप_प्रकार length,
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
                DEFAULT_मान,
                ALTERNATE_मान,
        } state = WORD;

        _cleanup_strv_free_ वर्ण **unset_variables = रिक्त, **bad_variables = रिक्त;
        अचर वर्ण *e, *word = format, *test_मान = रिक्त; /* test_value is initialized to appease gcc */
        _cleanup_free_ वर्ण *s = रिक्त;
        वर्ण ***pu, ***pb;
        माप_प्रकार i, len = 0; /* len is initialized to appease gcc */
        पूर्णाङ्क nest = 0, r;

        प्रतिज्ञा(format);

        यदि (length == SIZE_MAX)
                length = म_माप(format);

        pu = ret_unset_variables ? &unset_variables : रिक्त;
        pb = ret_bad_variables ? &bad_variables : रिक्त;

        लागि (e = format, i = 0; *e && i < length; e++, i++)
                विकल्प (state) {

                पक्ष WORD:
                        यदि (*e == '$')
                                state = CURLY;
                        रोक;

                पक्ष CURLY:
                        यदि (*e == '{') {
                                यदि (!strextendn(&s, word, e-word-1))
                                        फर्काऊ -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } अन्यथा यदि (*e == '$') {
                                यदि (!strextendn(&s, word, e-word))
                                        फर्काऊ -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } अन्यथा यदि (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                यदि (!strextendn(&s, word, e-word-1))
                                        फर्काऊ -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } अन्यथा
                                state = WORD;
                        रोक;

                पक्ष VARIABLE:
                        यदि (*e == '}') {
                                वर्ण *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                यदि (r < 0)
                                        फर्काऊ r;

                                यदि (!strextend(&s, t))
                                        फर्काऊ -ENOMEM;

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
                        रोक;

                पक्ष TEST:
                        यदि (*e == '-')
                                state = DEFAULT_मान;
                        अन्यथा यदि (*e == '+')
                                state = ALTERNATE_मान;
                        अन्यथा {
                                state = WORD;
                                रोक;
                        }

                        test_मान = e+1;
                        रोक;

                पक्ष DEFAULT_मान: /* fall through */
                पक्ष ALTERNATE_मान:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        यदि (*e == '{') {
                                nest++;
                                रोक;
                        }

                        यदि (*e != '}')
                                रोक;

                        nest--;
                        यदि (nest == 0) {
                                _cleanup_strv_free_ वर्ण **u = रिक्त, **b = रिक्त;
                                _cleanup_free_ वर्ण *v = रिक्त;
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                यदि (r < 0)
                                        फर्काऊ r;

                                यदि (t && state == ALTERNATE_मान) {
                                        r = replace_env_full(test_मान, e-test_मान, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        यदि (r < 0)
                                                फर्काऊ r;

                                        t = v;
                                } अन्यथा यदि (!t && state == DEFAULT_मान) {
                                        r = replace_env_full(test_मान, e-test_मान, env, flags, &v, pu ? &u : रिक्त, pb ? &b : रिक्त);
                                        यदि (r < 0)
                                                फर्काऊ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                यदि (r < 0)
                                        फर्काऊ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                यदि (r < 0)
                                        फर्काऊ r;

                                यदि (!strextend(&s, t))
                                        फर्काऊ -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        रोक;

                पक्ष VARIABLE_RAW:
                        प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        यदि (!म_अक्षर(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                वर्ण *t = रिक्त;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                यदि (r < 0)
                                        फर्काऊ r;

                                यदि (!strextend(&s, t))
                                        फर्काऊ -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        रोक;
                }

        यदि (state == VARIABLE_RAW) {
                वर्ण *t;

                प्रतिज्ञा(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (!strextend(&s, t))
                        फर्काऊ -ENOMEM;

        } अन्यथा यदि (!strextendn(&s, word, e-word))
                फर्काऊ -ENOMEM;

        यदि (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        यदि (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        यदि (ret)
                *ret = TAKE_PTR(s);

        फर्काऊ 0;
}

पूर्णाङ्क replace_env_argv(
                वर्ण **argv,
                वर्ण **env,
                वर्ण ***ret,
                वर्ण ***ret_unset_variables,
                वर्ण ***ret_bad_variables) {

        _cleanup_strv_free_ वर्ण **n = रिक्त, **unset_variables = रिक्त, **bad_variables = रिक्त;
        माप_प्रकार k = 0, l;
        पूर्णाङ्क r;

        प्रतिज्ञा(!strv_isempty(argv));
        प्रतिज्ञा(ret);

        l = strv_length(argv);

        n = new(वर्ण*, l+1);
        यदि (!n)
                फर्काऊ -ENOMEM;

        STRV_FOREACH(i, argv) {
                अचर वर्ण *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                यदि (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ वर्ण **m = रिक्त;
                        अचर वर्ण *name = word + 1;
                        वर्ण *e, **w;
                        माप_प्रकार q;

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
                                फर्काऊ r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, परिमाण(वर्ण*));
                        यदि (!w)
                                फर्काऊ -ENOMEM;

                        n = w;
                        यदि (m) {
                                स_नकल(n + k, m, (q + 1) * परिमाण(वर्ण*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = रिक्त;
                        जारी;
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
                        फर्काऊ r;
                n[++k] = रिक्त;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                यदि (r < 0)
                        फर्काऊ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                यदि (r < 0)
                        फर्काऊ r;
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
        फर्काऊ 0;
}

पूर्णाङ्क getenv_bool(अचर वर्ण *p) {
        अचर वर्ण *e;

        e = दो_पर्या(p);
        यदि (!e)
                फर्काऊ -ENXIO;

        फर्काऊ parse_boolean(e);
}

पूर्णाङ्क secure_getenv_bool(अचर वर्ण *p) {
        अचर वर्ण *e;

        e = secure_getenv(p);
        यदि (!e)
                फर्काऊ -ENXIO;

        फर्काऊ parse_boolean(e);
}

पूर्णाङ्क secure_getenv_uint64(अचर वर्ण *p, uint64_t *ret) {
        अचर वर्ण *e;

        प्रतिज्ञा(p);

        e = secure_getenv(p);
        यदि (!e)
                फर्काऊ -ENXIO;

        फर्काऊ safe_atou64(e, ret);
}

पूर्णाङ्क set_unset_env(अचर वर्ण *name, अचर वर्ण *मान, bool overwrite) {
        प्रतिज्ञा(name);

        यदि (मान)
                फर्काऊ RET_NERRNO(setenv(name, मान, overwrite));

        फर्काऊ RET_NERRNO(unsetenv(name));
}

पूर्णाङ्क putenv_dup(अचर वर्ण *assignment, bool override) {
        अचर वर्ण *e, *n;

        e = म_अक्षर(assignment, '=');
        यदि (!e)
                फर्काऊ -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        फर्काऊ RET_NERRNO(setenv(n, e + 1, override));
}

पूर्णाङ्क setenv_systemd_exec_pid(bool update_only) {
        अचर वर्ण *e;
        पूर्णाङ्क r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        यदि (!e && update_only)
                फर्काऊ 0;

        यदि (streq_ptr(e, "*"))
                फर्काऊ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ 1;
}

पूर्णाङ्क setenv_systemd_log_level(शून्य) {
        _cleanup_free_ वर्ण *val = रिक्त;
        पूर्णाङ्क r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

पूर्णाङ्क getenv_path_list(अचर वर्ण *name, वर्ण ***ret_paths) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        अचर वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(ret_paths);

        e = secure_getenv(name);
        यदि (!e)
                फर्काऊ -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                फर्काऊ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                यदि (!path_is_absolute(*p))
                        फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                यदि (!path_is_normalized(*p))
                        फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                यदि (path_equal(*p, "/"))
                        फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        यदि (strv_isempty(l))
                फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        फर्काऊ 1;
}

पूर्णाङ्क getenv_steal_erase(अचर वर्ण *name, वर्ण **ret) {
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
                फर्काऊ 0;
        }

        यदि (ret) {
                a = strdup(e);
                यदि (!a)
                        फर्काऊ -ENOMEM;
        }

        string_erase(e);

        यदि (unsetenv(name) < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (ret)
                *ret = TAKE_PTR(a);

        फर्काऊ 1;
}

पूर्णाङ्क setenvf(अचर वर्ण *name, bool overwrite, अचर वर्ण *valuef, ...) {
        _cleanup_free_ वर्ण *मान = रिक्त;
        बहु_सूची ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        यदि (!valuef)
                फर्काऊ RET_NERRNO(unsetenv(name));

        बहु_शुरू(ap, valuef);
        r = vasprintf(&मान, valuef, ap);
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        यदि (streq_ptr(दो_पर्या(name), मान))
                फर्काऊ 0;

        फर्काऊ RET_NERRNO(setenv(name, मान, overwrite));
}
