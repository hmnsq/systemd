/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "syslog-util.h"
#অন্তর্ভাব "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(শূন্য) {
        long l = sysconf(_SC_ARG_MAX);
        প্রতিজ্ঞা(l > 0);
        প্রত্যাগম (size_t) l;
}

স্থির bool env_name_is_valid_n(ধ্রুব বর্ণ *e, size_t n) {

        যদি (n == SIZE_MAX)
                n = strlen_ptr(e);

        যদি (n == 0)
                প্রত্যাগম false;

        প্রতিজ্ঞা(e);

        যদি (ascii_isdigit(e[0]))
                প্রত্যাগম false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        যদি (_unlikely_(n > sc_arg_max() - 2))
                প্রত্যাগম false;

        প্রতি (ধ্রুব বর্ণ *p = e; p < e + n; p++)
                যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        প্রত্যাগম false;

        প্রত্যাগম true;
}

bool env_name_is_valid(ধ্রুব বর্ণ *e) {
        প্রত্যাগম env_name_is_valid_n(e, SIZE_MAX);
}

bool env_মান_is_valid(ধ্রুব বর্ণ *e) {
        যদি (!e)
                প্রত্যাগম false;

        যদি (!utf8_is_valid(e))
                প্রত্যাগম false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 3))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool env_assignment_is_valid(ধ্রুব বর্ণ *e) {
        ধ্রুব বর্ণ *eq;

        প্রতিজ্ঞা(e);

        eq = strchr(e, '=');
        যদি (!eq)
                প্রত্যাগম false;

        যদি (!env_name_is_valid_n(e, eq - e))
                প্রত্যাগম false;

        যদি (!env_মান_is_valid(eq + 1))
                প্রত্যাগম false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 1))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool strv_env_is_valid(বর্ণ * ধ্রুব *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                যদি (!env_assignment_is_valid(*p))
                        প্রত্যাগম false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, k) && (*q)[k] == '=')
                                প্রত্যাগম false;
        }

        প্রত্যাগম true;
}

bool strv_env_name_is_valid(বর্ণ * ধ্রুব *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_name_is_valid(*p))
                        প্রত্যাগম false;

                যদি (strv_contains(p + 1, *p))
                        প্রত্যাগম false;
        }

        প্রত্যাগম true;
}

bool strv_env_name_or_assignment_is_valid(বর্ণ * ধ্রুব *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        প্রত্যাগম false;

                যদি (strv_contains(p + 1, *p))
                        প্রত্যাগম false;
        }

        প্রত্যাগম true;
}

স্থির পূর্ণাঙ্ক env_append(বর্ণ **e, বর্ণ ***k, বর্ণ **a) {
        প্রতিজ্ঞা(e);
        প্রতিজ্ঞা(k);
        প্রতিজ্ঞা(*k >= e);

        যদি (!a)
                প্রত্যাগম 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        প্রতি (; *a; a++) {
                বর্ণ **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                যদি ((*a)[n] == '=')
                        n++;

                প্রতি (j = e; j < *k; j++)
                        যদি (strneq(*j, *a, n))
                                ভঙ্গ;

                c = strdup(*a);
                যদি (!c)
                        প্রত্যাগম -ENOMEM;

                যদি (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = রিক্ত;
                        (*k)++;
                } অন্যথা
                        free_and_replace(*j, c); /* Override existing item */
        }

        প্রত্যাগম 0;
}

বর্ণ** _strv_env_merge(বর্ণ **first, ...) {
        _cleanup_strv_free_ বর্ণ **merged = রিক্ত;
        বর্ণ **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        প্রতি (;;) {
                বর্ণ **l;

                l = va_arg(ap, বর্ণ**);
                যদি (l == POINTER_MAX)
                        ভঙ্গ;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(বর্ণ*, n + 1);
        যদি (!merged)
                প্রত্যাগম রিক্ত;
        merged[0] = রিক্ত;

        যদি (env_append(merged, &k, first) < 0)
                প্রত্যাগম রিক্ত;

        va_start(ap, first);
        প্রতি (;;) {
                বর্ণ **l;

                l = va_arg(ap, বর্ণ**);
                যদি (l == POINTER_MAX)
                        ভঙ্গ;

                যদি (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        প্রত্যাগম রিক্ত;
                }
        }
        va_end(ap);

        প্রত্যাগম TAKE_PTR(merged);
}

স্থির bool env_match(ধ্রুব বর্ণ *t, ধ্রুব বর্ণ *pattern) {
        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        যদি (streq(t, pattern))
                প্রত্যাগম true;

        যদি (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                প্রত্যাগম t && *t == '=';
        }

        প্রত্যাগম false;
}

স্থির bool env_entry_has_name(ধ্রুব বর্ণ *entry, ধ্রুব বর্ণ *name) {
        ধ্রুব বর্ণ *t;

        প্রতিজ্ঞা(entry);
        প্রতিজ্ঞা(name);

        t = startswith(entry, name);
        যদি (!t)
                প্রত্যাগম false;

        প্রত্যাগম *t == '=';
}

বর্ণ** strv_env_delete(বর্ণ **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ বর্ণ **t = রিক্ত;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(বর্ণ*, n+1);
        যদি (!t)
                প্রত্যাগম রিক্ত;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                প্রতি (size_t v = 0; v < n_lists; v++) {
                        বর্ণ **l;

                        l = va_arg(ap, বর্ণ**);
                        STRV_FOREACH(j, l)
                                যদি (env_match(*k, *j))
                                        গচ্ছ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                যদি (!t[i])
                        প্রত্যাগম রিক্ত;

                i++;
                অনুবর্ত;

        skip:
                va_end(ap);
        }

        t[i] = রিক্ত;

        প্রতিজ্ঞা(i <= n);

        প্রত্যাগম TAKE_PTR(t);
}

বর্ণ** strv_env_unset(বর্ণ **l, ধ্রুব বর্ণ *p) {
        প্রতিজ্ঞা(p);

        যদি (!l)
                প্রত্যাগম রিক্ত;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        বর্ণ **f, **t;
        প্রতি (f = t = l; *f; f++) {
                যদি (env_match(*f, p)) {
                        মুক্ত(*f);
                        অনুবর্ত;
                }

                *(t++) = *f;
        }

        *t = রিক্ত;
        প্রত্যাগম l;
}

বর্ণ** strv_env_unset_many_internal(বর্ণ **l, ...) {
        যদি (!l)
                প্রত্যাগম রিক্ত;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        বর্ণ **f, **t;
        প্রতি (f = t = l; *f; f++) {
                bool found = false;
                ধ্রুব বর্ণ *p;
                va_list ap;

                va_start(ap, l);

                যাবত্ ((p = va_arg(ap, ধ্রুব বর্ণ*)))
                        যদি (env_match(*f, p)) {
                                found = true;
                                ভঙ্গ;
                        }

                va_end(ap);

                যদি (found) {
                        মুক্ত(*f);
                        অনুবর্ত;
                }

                *(t++) = *f;
        }

        *t = রিক্ত;
        প্রত্যাগম l;
}

পূর্ণাঙ্ক strv_env_replace_consume(বর্ণ ***l, বর্ণ *p) {
        ধ্রুব বর্ণ *t, *name;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        যদি (!t) {
                মুক্ত(p);
                প্রত্যাগম -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                যদি (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        প্রত্যাগম 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক strv_env_replace_strdup(বর্ণ ***l, ধ্রুব বর্ণ *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        বর্ণ *p = strdup(assignment);
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_replace_strdup_passthrough(বর্ণ ***l, ধ্রুব বর্ণ *assignment) {
        বর্ণ *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        যদি (strchr(assignment, '=')) {
                যদি (!env_assignment_is_valid(assignment))
                        প্রত্যাগম -EINVAL;

                p = strdup(assignment);
        } অন্যথা {
                যদি (!env_name_is_valid(assignment))
                        প্রত্যাগম -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_assign(বর্ণ ***l, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        প্রতিজ্ঞা(l);

        যদি (!env_name_is_valid(চাবি))
                প্রত্যাগম -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        যদি (!মান) {
                strv_env_unset(*l, চাবি);
                প্রত্যাগম 0;
        }

        বর্ণ *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_assignf(বর্ণ ***l, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *valuef, ...) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(চাবি);

        যদি (!env_name_is_valid(চাবি))
                প্রত্যাগম -EINVAL;

        যদি (!valuef) {
                strv_env_unset(*l, চাবি);
                প্রত্যাগম 0;
        }

        _cleanup_free_ বর্ণ *মান = রিক্ত;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);
        যদি (r < 0)
                প্রত্যাগম -ENOMEM;

        বর্ণ *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক _strv_env_assign_many(বর্ণ ***l, ...) {
        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);

        va_start(ap, l);
        প্রতি (;;) {
                ধ্রুব বর্ণ *চাবি, *মান;

                চাবি = va_arg(ap, ধ্রুব বর্ণ *);
                যদি (!চাবি)
                        ভঙ্গ;

                যদি (!env_name_is_valid(চাবি)) {
                        va_end(ap);
                        প্রত্যাগম -EINVAL;
                }

                মান = va_arg(ap, ধ্রুব বর্ণ *);
                যদি (!মান) {
                        strv_env_unset(*l, চাবি);
                        অনুবর্ত;
                }

                বর্ণ *p = strjoin(চাবি, "=", মান);
                যদি (!p) {
                        va_end(ap);
                        প্রত্যাগম -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                যদি (r < 0) {
                        va_end(ap);
                        প্রত্যাগম r;
                }
        }
        va_end(ap);

        প্রত্যাগম 0;
}

বর্ণ* strv_env_get_n(বর্ণ * ধ্রুব *l, ধ্রুব বর্ণ *name, size_t k, ReplaceEnvFlags flags) {
        প্রতিজ্ঞা(name);

        যদি (k == SIZE_MAX)
                k = strlen(name);
        যদি (k <= 0)
                প্রত্যাগম রিক্ত;

        STRV_FOREACH_BACKWARDS(i, l)
                যদি (strneq(*i, name, k) && (*i)[k] == '=')
                        প্রত্যাগম (বর্ণ*) *i + k + 1;

        যদি (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ধ্রুব বর্ণ *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                যদি (k > sc_arg_max() - 2)
                        প্রত্যাগম রিক্ত;

                t = strndupa_safe(name, k);
                প্রত্যাগম secure_getenv(t);
        };

        প্রত্যাগম রিক্ত;
}

বর্ণ* strv_env_pairs_get(বর্ণ **l, ধ্রুব বর্ণ *name) {
        বর্ণ *result = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH_PAIR(চাবি, মান, l)
                যদি (streq(*চাবি, name))
                        result = *মান;

        প্রত্যাগম result;
}

পূর্ণাঙ্ক strv_env_get_merged(বর্ণ **l, বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **v = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(চাবি, মান, l) {
                বর্ণ *s;

                s = strjoin(*চাবি, "=", *মান);
                যদি (!s)
                        প্রত্যাগম -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        *ret = TAKE_PTR(v);
        প্রত্যাগম 0;
}

বর্ণ** strv_env_clean_with_callback(বর্ণ **l, শূন্য (*invalid_callback)(ধ্রুব বর্ণ *p, শূন্য *userdata), শূন্য *userdata) {
        পূর্ণাঙ্ক k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                যদি (!env_assignment_is_valid(*p)) {
                        যদি (invalid_callback)
                                invalid_callback(*p, userdata);
                        মুক্ত(*p);
                        অনুবর্ত;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ভঙ্গ;
                        }

                যদি (duplicate) {
                        মুক্ত(*p);
                        অনুবর্ত;
                }

                l[k++] = *p;
        }

        যদি (l)
                l[k] = রিক্ত;

        প্রত্যাগম l;
}

স্থির পূর্ণাঙ্ক strv_extend_with_length(বর্ণ ***l, ধ্রুব বর্ণ *s, size_t n) {
        বর্ণ *c;

        c = strndup(s, n);
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম strv_consume(l, c);
}

স্থির পূর্ণাঙ্ক strv_env_get_n_validated(
                বর্ণ **env,
                ধ্রুব বর্ণ *name,
                size_t l,
                ReplaceEnvFlags flags,
                বর্ণ **ret,              /* points into the env block! do not free! */
                বর্ণ ***unset_variables, /* updated in place */
                বর্ণ ***bad_variables) { /* ditto */

        বর্ণ *e;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l == 0 || name);
        প্রতিজ্ঞা(ret);

        যদি (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                যদি (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }
        } অন্যথা {
                e = রিক্ত; /* Resolve invalid variable names the same way as unset ones */

                যদি (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }
        }

        *ret = e;
        প্রত্যাগম !!e;
}

পূর্ণাঙ্ক replace_env_full(
                ধ্রুব বর্ণ *format,
                size_t length,
                বর্ণ **env,
                ReplaceEnvFlags flags,
                বর্ণ **ret,
                বর্ণ ***ret_unset_variables,
                বর্ণ ***ret_bad_variables) {

        গণনা {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_মান,
                ALTERNATE_মান,
        } state = WORD;

        _cleanup_strv_free_ বর্ণ **unset_variables = রিক্ত, **bad_variables = রিক্ত;
        ধ্রুব বর্ণ *e, *word = format, *test_মান = রিক্ত; /* test_value is initialized to appease gcc */
        _cleanup_free_ বর্ণ *s = রিক্ত;
        বর্ণ ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        পূর্ণাঙ্ক nest = 0, r;

        প্রতিজ্ঞা(format);

        যদি (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : রিক্ত;
        pb = ret_bad_variables ? &bad_variables : রিক্ত;

        প্রতি (e = format, i = 0; *e && i < length; e++, i++)
                বিকল্প (state) {

                পক্ষ WORD:
                        যদি (*e == '$')
                                state = CURLY;
                        ভঙ্গ;

                পক্ষ CURLY:
                        যদি (*e == '{') {
                                যদি (!strextendn(&s, word, e-word-1))
                                        প্রত্যাগম -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } অন্যথা যদি (*e == '$') {
                                যদি (!strextendn(&s, word, e-word))
                                        প্রত্যাগম -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } অন্যথা যদি (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                যদি (!strextendn(&s, word, e-word-1))
                                        প্রত্যাগম -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } অন্যথা
                                state = WORD;
                        ভঙ্গ;

                পক্ষ VARIABLE:
                        যদি (*e == '}') {
                                বর্ণ *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                যদি (!strextend(&s, t))
                                        প্রত্যাগম -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } অন্যথা যদি (*e == ':') {
                                যদি (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } অন্যথা
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        ভঙ্গ;

                পক্ষ TEST:
                        যদি (*e == '-')
                                state = DEFAULT_মান;
                        অন্যথা যদি (*e == '+')
                                state = ALTERNATE_মান;
                        অন্যথা {
                                state = WORD;
                                ভঙ্গ;
                        }

                        test_মান = e+1;
                        ভঙ্গ;

                পক্ষ DEFAULT_মান: /* fall through */
                পক্ষ ALTERNATE_মান:
                        প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        যদি (*e == '{') {
                                nest++;
                                ভঙ্গ;
                        }

                        যদি (*e != '}')
                                ভঙ্গ;

                        nest--;
                        যদি (nest == 0) {
                                _cleanup_strv_free_ বর্ণ **u = রিক্ত, **b = রিক্ত;
                                _cleanup_free_ বর্ণ *v = রিক্ত;
                                বর্ণ *t = রিক্ত;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                যদি (t && state == ALTERNATE_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : রিক্ত, pb ? &b : রিক্ত);
                                        যদি (r < 0)
                                                প্রত্যাগম r;

                                        t = v;
                                } অন্যথা যদি (!t && state == DEFAULT_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : রিক্ত, pb ? &b : রিক্ত);
                                        যদি (r < 0)
                                                প্রত্যাগম r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                যদি (!strextend(&s, t))
                                        প্রত্যাগম -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        ভঙ্গ;

                পক্ষ VARIABLE_RAW:
                        প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                বর্ণ *t = রিক্ত;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                যদি (!strextend(&s, t))
                                        প্রত্যাগম -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        ভঙ্গ;
                }

        যদি (state == VARIABLE_RAW) {
                বর্ণ *t;

                প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (!strextend(&s, t))
                        প্রত্যাগম -ENOMEM;

        } অন্যথা যদি (!strextendn(&s, word, e-word))
                প্রত্যাগম -ENOMEM;

        যদি (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        যদি (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        যদি (ret)
                *ret = TAKE_PTR(s);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক replace_env_argv(
                বর্ণ **argv,
                বর্ণ **env,
                বর্ণ ***ret,
                বর্ণ ***ret_unset_variables,
                বর্ণ ***ret_bad_variables) {

        _cleanup_strv_free_ বর্ণ **n = রিক্ত, **unset_variables = রিক্ত, **bad_variables = রিক্ত;
        size_t k = 0, l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(!strv_isempty(argv));
        প্রতিজ্ঞা(ret);

        l = strv_length(argv);

        n = new(বর্ণ*, l+1);
        যদি (!n)
                প্রত্যাগম -ENOMEM;

        STRV_FOREACH(i, argv) {
                ধ্রুব বর্ণ *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                যদি (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ বর্ণ **m = রিক্ত;
                        ধ্রুব বর্ণ *name = word + 1;
                        বর্ণ *e, **w;
                        size_t q;

                        যদি (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                যদি (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                অন্যথা যদি (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                অন্যথা
                                        r = 0;
                        } অন্যথা যদি (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        অন্যথা
                                r = 0;
                        যদি (r < 0)
                                প্রত্যাগম r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, পরিমাণ(বর্ণ*));
                        যদি (!w)
                                প্রত্যাগম -ENOMEM;

                        n = w;
                        যদি (m) {
                                memcpy(n + k, m, (q + 1) * পরিমাণ(বর্ণ*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = রিক্ত;
                        অনুবর্ত;
                }

                _cleanup_strv_free_ বর্ণ **u = রিক্ত, **b = রিক্ত;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : রিক্ত,
                                ret_bad_variables ? &b : রিক্ত);
                যদি (r < 0)
                        প্রত্যাগম r;
                n[++k] = রিক্ত;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                যদি (r < 0)
                        প্রত্যাগম r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        যদি (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক getenv_bool(ধ্রুব বর্ণ *p) {
        ধ্রুব বর্ণ *e;

        e = getenv(p);
        যদি (!e)
                প্রত্যাগম -ENXIO;

        প্রত্যাগম parse_boolean(e);
}

পূর্ণাঙ্ক secure_getenv_bool(ধ্রুব বর্ণ *p) {
        ধ্রুব বর্ণ *e;

        e = secure_getenv(p);
        যদি (!e)
                প্রত্যাগম -ENXIO;

        প্রত্যাগম parse_boolean(e);
}

পূর্ণাঙ্ক secure_getenv_uint64(ধ্রুব বর্ণ *p, uint64_t *ret) {
        ধ্রুব বর্ণ *e;

        প্রতিজ্ঞা(p);

        e = secure_getenv(p);
        যদি (!e)
                প্রত্যাগম -ENXIO;

        প্রত্যাগম safe_atou64(e, ret);
}

পূর্ণাঙ্ক set_unset_env(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *মান, bool overwrite) {
        প্রতিজ্ঞা(name);

        যদি (মান)
                প্রত্যাগম RET_NERRNO(setenv(name, মান, overwrite));

        প্রত্যাগম RET_NERRNO(unsetenv(name));
}

পূর্ণাঙ্ক putenv_dup(ধ্রুব বর্ণ *assignment, bool override) {
        ধ্রুব বর্ণ *e, *n;

        e = strchr(assignment, '=');
        যদি (!e)
                প্রত্যাগম -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        প্রত্যাগম RET_NERRNO(setenv(n, e + 1, override));
}

পূর্ণাঙ্ক setenv_systemd_exec_pid(bool update_only) {
        ধ্রুব বর্ণ *e;
        পূর্ণাঙ্ক r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        যদি (!e && update_only)
                প্রত্যাগম 0;

        যদি (streq_ptr(e, "*"))
                প্রত্যাগম 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক setenv_systemd_log_level(শূন্য) {
        _cleanup_free_ বর্ণ *val = রিক্ত;
        পূর্ণাঙ্ক r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

পূর্ণাঙ্ক getenv_path_list(ধ্রুব বর্ণ *name, বর্ণ ***ret_paths) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        ধ্রুব বর্ণ *e;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret_paths);

        e = secure_getenv(name);
        যদি (!e)
                প্রত্যাগম -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                যদি (!path_is_absolute(*p))
                        প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                যদি (!path_is_normalized(*p))
                        প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                যদি (path_equal(*p, "/"))
                        প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        যদি (strv_isempty(l))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        প্রত্যাগম 1;
}

পূর্ণাঙ্ক getenv_steal_erase(ধ্রুব বর্ণ *name, বর্ণ **ret) {
        _cleanup_(erase_and_freep) বর্ণ *a = রিক্ত;
        বর্ণ *e;

        প্রতিজ্ঞা(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        যদি (!e) {
                যদি (ret)
                        *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        যদি (ret) {
                a = strdup(e);
                যদি (!a)
                        প্রত্যাগম -ENOMEM;
        }

        string_erase(e);

        যদি (unsetenv(name) < 0)
                প্রত্যাগম -errno;

        যদি (ret)
                *ret = TAKE_PTR(a);

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক setenvf(ধ্রুব বর্ণ *name, bool overwrite, ধ্রুব বর্ণ *valuef, ...) {
        _cleanup_free_ বর্ণ *মান = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        যদি (!valuef)
                প্রত্যাগম RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        যদি (streq_ptr(getenv(name), মান))
                প্রত্যাগম 0;

        প্রত্যাগম RET_NERRNO(setenv(name, মান, overwrite));
}
