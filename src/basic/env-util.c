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
        ফেরত (size_t) l;
}

স্থির bool env_name_is_valid_n(ধ্রুবক অক্ষর *e, size_t n) {

        যদি (n == SIZE_MAX)
                n = strlen_ptr(e);

        যদি (n == 0)
                ফেরত false;

        প্রতিজ্ঞা(e);

        যদি (ascii_isdigit(e[0]))
                ফেরত false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        যদি (_unlikely_(n > sc_arg_max() - 2))
                ফেরত false;

        জন্য (ধ্রুবক অক্ষর *p = e; p < e + n; p++)
                যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ফেরত false;

        ফেরত true;
}

bool env_name_is_valid(ধ্রুবক অক্ষর *e) {
        ফেরত env_name_is_valid_n(e, SIZE_MAX);
}

bool env_মান_is_valid(ধ্রুবক অক্ষর *e) {
        যদি (!e)
                ফেরত false;

        যদি (!utf8_is_valid(e))
                ফেরত false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ফেরত false;

        ফেরত true;
}

bool env_assignment_is_valid(ধ্রুবক অক্ষর *e) {
        ধ্রুবক অক্ষর *eq;

        প্রতিজ্ঞা(e);

        eq = strchr(e, '=');
        যদি (!eq)
                ফেরত false;

        যদি (!env_name_is_valid_n(e, eq - e))
                ফেরত false;

        যদি (!env_মান_is_valid(eq + 1))
                ফেরত false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ফেরত false;

        ফেরত true;
}

bool strv_env_is_valid(অক্ষর * ধ্রুবক *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                যদি (!env_assignment_is_valid(*p))
                        ফেরত false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, k) && (*q)[k] == '=')
                                ফেরত false;
        }

        ফেরত true;
}

bool strv_env_name_is_valid(অক্ষর * ধ্রুবক *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_name_is_valid(*p))
                        ফেরত false;

                যদি (strv_contains(p + 1, *p))
                        ফেরত false;
        }

        ফেরত true;
}

bool strv_env_name_or_assignment_is_valid(অক্ষর * ধ্রুবক *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ফেরত false;

                যদি (strv_contains(p + 1, *p))
                        ফেরত false;
        }

        ফেরত true;
}

স্থির পূর্ণসংখ্যা env_append(অক্ষর **e, অক্ষর ***k, অক্ষর **a) {
        প্রতিজ্ঞা(e);
        প্রতিজ্ঞা(k);
        প্রতিজ্ঞা(*k >= e);

        যদি (!a)
                ফেরত 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        জন্য (; *a; a++) {
                অক্ষর **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                যদি ((*a)[n] == '=')
                        n++;

                জন্য (j = e; j < *k; j++)
                        যদি (strneq(*j, *a, n))
                                বিরতি;

                c = strdup(*a);
                যদি (!c)
                        ফেরত -ENOMEM;

                যদি (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ফাঁকা;
                        (*k)++;
                } অন্যথা
                        free_and_replace(*j, c); /* Override existing item */
        }

        ফেরত 0;
}

অক্ষর** _strv_env_merge(অক্ষর **first, ...) {
        _cleanup_strv_free_ অক্ষর **merged = ফাঁকা;
        অক্ষর **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_দৈর্ঘ্য(first);

        va_start(ap, first);
        জন্য (;;) {
                অক্ষর **l;

                l = va_arg(ap, অক্ষর**);
                যদি (l == POINTER_MAX)
                        বিরতি;

                n += strv_দৈর্ঘ্য(l);
        }
        va_end(ap);

        k = merged = new(অক্ষর*, n + 1);
        যদি (!merged)
                ফেরত ফাঁকা;
        merged[0] = ফাঁকা;

        যদি (env_append(merged, &k, first) < 0)
                ফেরত ফাঁকা;

        va_start(ap, first);
        জন্য (;;) {
                অক্ষর **l;

                l = va_arg(ap, অক্ষর**);
                যদি (l == POINTER_MAX)
                        বিরতি;

                যদি (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ফেরত ফাঁকা;
                }
        }
        va_end(ap);

        ফেরত TAKE_PTR(merged);
}

স্থির bool env_match(ধ্রুবক অক্ষর *t, ধ্রুবক অক্ষর *pattern) {
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
                ফেরত true;

        যদি (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ফেরত t && *t == '=';
        }

        ফেরত false;
}

স্থির bool env_entry_has_name(ধ্রুবক অক্ষর *entry, ধ্রুবক অক্ষর *name) {
        ধ্রুবক অক্ষর *t;

        প্রতিজ্ঞা(entry);
        প্রতিজ্ঞা(name);

        t = startswith(entry, name);
        যদি (!t)
                ফেরত false;

        ফেরত *t == '=';
}

অক্ষর** strv_env_delete(অক্ষর **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ অক্ষর **t = ফাঁকা;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_দৈর্ঘ্য(x);

        t = new(অক্ষর*, n+1);
        যদি (!t)
                ফেরত ফাঁকা;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                জন্য (size_t v = 0; v < n_lists; v++) {
                        অক্ষর **l;

                        l = va_arg(ap, অক্ষর**);
                        STRV_FOREACH(j, l)
                                যদি (env_match(*k, *j))
                                        যাও skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                যদি (!t[i])
                        ফেরত ফাঁকা;

                i++;
                চালু;

        skip:
                va_end(ap);
        }

        t[i] = ফাঁকা;

        প্রতিজ্ঞা(i <= n);

        ফেরত TAKE_PTR(t);
}

অক্ষর** strv_env_unset(অক্ষর **l, ধ্রুবক অক্ষর *p) {
        প্রতিজ্ঞা(p);

        যদি (!l)
                ফেরত ফাঁকা;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        অক্ষর **f, **t;
        জন্য (f = t = l; *f; f++) {
                যদি (env_match(*f, p)) {
                        মুক্ত(*f);
                        চালু;
                }

                *(t++) = *f;
        }

        *t = ফাঁকা;
        ফেরত l;
}

অক্ষর** strv_env_unset_many_internal(অক্ষর **l, ...) {
        যদি (!l)
                ফেরত ফাঁকা;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        অক্ষর **f, **t;
        জন্য (f = t = l; *f; f++) {
                bool found = false;
                ধ্রুবক অক্ষর *p;
                va_list ap;

                va_start(ap, l);

                যখন ((p = va_arg(ap, ধ্রুবক অক্ষর*)))
                        যদি (env_match(*f, p)) {
                                found = true;
                                বিরতি;
                        }

                va_end(ap);

                যদি (found) {
                        মুক্ত(*f);
                        চালু;
                }

                *(t++) = *f;
        }

        *t = ফাঁকা;
        ফেরত l;
}

পূর্ণসংখ্যা strv_env_replace_consume(অক্ষর ***l, অক্ষর *p) {
        ধ্রুবক অক্ষর *t, *name;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        যদি (!t) {
                মুক্ত(p);
                ফেরত -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                যদি (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ফেরত 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

পূর্ণসংখ্যা strv_env_replace_strdup(অক্ষর ***l, ধ্রুবক অক্ষর *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        অক্ষর *p = strdup(assignment);
        যদি (!p)
                ফেরত -ENOMEM;

        ফেরত strv_env_replace_consume(l, p);
}

পূর্ণসংখ্যা strv_env_replace_strdup_passthrough(অক্ষর ***l, ধ্রুবক অক্ষর *assignment) {
        অক্ষর *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        যদি (strchr(assignment, '=')) {
                যদি (!env_assignment_is_valid(assignment))
                        ফেরত -EINVAL;

                p = strdup(assignment);
        } অন্যথা {
                যদি (!env_name_is_valid(assignment))
                        ফেরত -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        যদি (!p)
                ফেরত -ENOMEM;

        ফেরত strv_env_replace_consume(l, p);
}

পূর্ণসংখ্যা strv_env_assign(অক্ষর ***l, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        প্রতিজ্ঞা(l);

        যদি (!env_name_is_valid(চাবি))
                ফেরত -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        যদি (!মান) {
                strv_env_unset(*l, চাবি);
                ফেরত 0;
        }

        অক্ষর *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ফেরত -ENOMEM;

        ফেরত strv_env_replace_consume(l, p);
}

পূর্ণসংখ্যা strv_env_assignf(অক্ষর ***l, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *valuef, ...) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(চাবি);

        যদি (!env_name_is_valid(চাবি))
                ফেরত -EINVAL;

        যদি (!valuef) {
                strv_env_unset(*l, চাবি);
                ফেরত 0;
        }

        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);
        যদি (r < 0)
                ফেরত -ENOMEM;

        অক্ষর *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ফেরত -ENOMEM;

        ফেরত strv_env_replace_consume(l, p);
}

পূর্ণসংখ্যা _strv_env_assign_many(অক্ষর ***l, ...) {
        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(l);

        va_start(ap, l);
        জন্য (;;) {
                ধ্রুবক অক্ষর *চাবি, *মান;

                চাবি = va_arg(ap, ধ্রুবক অক্ষর *);
                যদি (!চাবি)
                        বিরতি;

                যদি (!env_name_is_valid(চাবি)) {
                        va_end(ap);
                        ফেরত -EINVAL;
                }

                মান = va_arg(ap, ধ্রুবক অক্ষর *);
                যদি (!মান) {
                        strv_env_unset(*l, চাবি);
                        চালু;
                }

                অক্ষর *p = strjoin(চাবি, "=", মান);
                যদি (!p) {
                        va_end(ap);
                        ফেরত -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                যদি (r < 0) {
                        va_end(ap);
                        ফেরত r;
                }
        }
        va_end(ap);

        ফেরত 0;
}

অক্ষর* strv_env_get_n(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *name, size_t k, ReplaceEnvFlags flags) {
        প্রতিজ্ঞা(name);

        যদি (k == SIZE_MAX)
                k = strlen(name);
        যদি (k <= 0)
                ফেরত ফাঁকা;

        STRV_FOREACH_BACKWARDS(i, l)
                যদি (strneq(*i, name, k) && (*i)[k] == '=')
                        ফেরত (অক্ষর*) *i + k + 1;

        যদি (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ধ্রুবক অক্ষর *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                যদি (k > sc_arg_max() - 2)
                        ফেরত ফাঁকা;

                t = strndupa_safe(name, k);
                ফেরত secure_getenv(t);
        };

        ফেরত ফাঁকা;
}

অক্ষর* strv_env_pairs_get(অক্ষর **l, ধ্রুবক অক্ষর *name) {
        অক্ষর *ফলাফল = ফাঁকা;

        প্রতিজ্ঞা(name);

        STRV_FOREACH_PAIR(চাবি, মান, l)
                যদি (streq(*চাবি, name))
                        ফলাফল = *মান;

        ফেরত ফলাফল;
}

পূর্ণসংখ্যা strv_env_get_merged(অক্ষর **l, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **v = ফাঁকা;
        size_t n = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(চাবি, মান, l) {
                অক্ষর *s;

                s = strjoin(*চাবি, "=", *মান);
                যদি (!s)
                        ফেরত -ENOMEM;

                r = strv_consume_with_আকার(&v, &n, s);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(v);
        ফেরত 0;
}

অক্ষর** strv_env_clean_with_callback(অক্ষর **l, শূন্য (*invalid_callback)(ধ্রুবক অক্ষর *p, শূন্য *userdata), শূন্য *userdata) {
        পূর্ণসংখ্যা k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                যদি (!env_assignment_is_valid(*p)) {
                        যদি (invalid_callback)
                                invalid_callback(*p, userdata);
                        মুক্ত(*p);
                        চালু;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                বিরতি;
                        }

                যদি (duplicate) {
                        মুক্ত(*p);
                        চালু;
                }

                l[k++] = *p;
        }

        যদি (l)
                l[k] = ফাঁকা;

        ফেরত l;
}

স্থির পূর্ণসংখ্যা strv_extend_with_দৈর্ঘ্য(অক্ষর ***l, ধ্রুবক অক্ষর *s, size_t n) {
        অক্ষর *c;

        c = strndup(s, n);
        যদি (!c)
                ফেরত -ENOMEM;

        ফেরত strv_consume(l, c);
}

স্থির পূর্ণসংখ্যা strv_env_get_n_validated(
                অক্ষর **env,
                ধ্রুবক অক্ষর *name,
                size_t l,
                ReplaceEnvFlags flags,
                অক্ষর **ret,              /* points into the env block! do not free! */
                অক্ষর ***unset_variables, /* updated in place */
                অক্ষর ***bad_variables) { /* ditto */

        অক্ষর *e;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(l == 0 || name);
        প্রতিজ্ঞা(ret);

        যদি (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                যদি (!e && unset_variables) {
                        r = strv_extend_with_দৈর্ঘ্য(unset_variables, name, l);
                        যদি (r < 0)
                                ফেরত r;
                }
        } অন্যথা {
                e = ফাঁকা; /* Resolve invalid variable names the same way as unset ones */

                যদি (bad_variables) {
                        r = strv_extend_with_দৈর্ঘ্য(bad_variables, name, l);
                        যদি (r < 0)
                                ফেরত r;
                }
        }

        *ret = e;
        ফেরত !!e;
}

পূর্ণসংখ্যা replace_env_full(
                ধ্রুবক অক্ষর *format,
                size_t length,
                অক্ষর **env,
                ReplaceEnvFlags flags,
                অক্ষর **ret,
                অক্ষর ***ret_unset_variables,
                অক্ষর ***ret_bad_variables) {

        গণনা {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_মান,
                ALTERNATE_মান,
        } state = WORD;

        _cleanup_strv_free_ অক্ষর **unset_variables = ফাঁকা, **bad_variables = ফাঁকা;
        ধ্রুবক অক্ষর *e, *word = format, *test_মান = ফাঁকা; /* test_value is initialized to appease gcc */
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        অক্ষর ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        পূর্ণসংখ্যা nest = 0, r;

        প্রতিজ্ঞা(format);

        যদি (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : ফাঁকা;
        pb = ret_bad_variables ? &bad_variables : ফাঁকা;

        জন্য (e = format, i = 0; *e && i < length; e++, i++)
                নির্বাচন (state) {

                ক্ষেত্র WORD:
                        যদি (*e == '$')
                                state = CURLY;
                        বিরতি;

                ক্ষেত্র CURLY:
                        যদি (*e == '{') {
                                যদি (!strextendn(&s, word, e-word-1))
                                        ফেরত -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } অন্যথা যদি (*e == '$') {
                                যদি (!strextendn(&s, word, e-word))
                                        ফেরত -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } অন্যথা যদি (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                যদি (!strextendn(&s, word, e-word-1))
                                        ফেরত -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } অন্যথা
                                state = WORD;
                        বিরতি;

                ক্ষেত্র VARIABLE:
                        যদি (*e == '}') {
                                অক্ষর *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (!strextend(&s, t))
                                        ফেরত -ENOMEM;

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
                        বিরতি;

                ক্ষেত্র TEST:
                        যদি (*e == '-')
                                state = DEFAULT_মান;
                        অন্যথা যদি (*e == '+')
                                state = ALTERNATE_মান;
                        অন্যথা {
                                state = WORD;
                                বিরতি;
                        }

                        test_মান = e+1;
                        বিরতি;

                ক্ষেত্র DEFAULT_মান: /* fall through */
                ক্ষেত্র ALTERNATE_মান:
                        প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        যদি (*e == '{') {
                                nest++;
                                বিরতি;
                        }

                        যদি (*e != '}')
                                বিরতি;

                        nest--;
                        যদি (nest == 0) {
                                _cleanup_strv_free_ অক্ষর **u = ফাঁকা, **b = ফাঁকা;
                                _cleanup_free_ অক্ষর *v = ফাঁকা;
                                অক্ষর *t = ফাঁকা;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (t && state == ALTERNATE_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : ফাঁকা, pb ? &b : ফাঁকা);
                                        যদি (r < 0)
                                                ফেরত r;

                                        t = v;
                                } অন্যথা যদি (!t && state == DEFAULT_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : ফাঁকা, pb ? &b : ফাঁকা);
                                        যদি (r < 0)
                                                ফেরত r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        ফেরত r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (!strextend(&s, t))
                                        ফেরত -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        বিরতি;

                ক্ষেত্র VARIABLE_RAW:
                        প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                অক্ষর *t = ফাঁকা;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                যদি (r < 0)
                                        ফেরত r;

                                যদি (!strextend(&s, t))
                                        ফেরত -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        বিরতি;
                }

        যদি (state == VARIABLE_RAW) {
                অক্ষর *t;

                প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                যদি (r < 0)
                        ফেরত r;

                যদি (!strextend(&s, t))
                        ফেরত -ENOMEM;

        } অন্যথা যদি (!strextendn(&s, word, e-word))
                ফেরত -ENOMEM;

        যদি (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        যদি (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        যদি (ret)
                *ret = TAKE_PTR(s);

        ফেরত 0;
}

পূর্ণসংখ্যা replace_env_argv(
                অক্ষর **argv,
                অক্ষর **env,
                অক্ষর ***ret,
                অক্ষর ***ret_unset_variables,
                অক্ষর ***ret_bad_variables) {

        _cleanup_strv_free_ অক্ষর **n = ফাঁকা, **unset_variables = ফাঁকা, **bad_variables = ফাঁকা;
        size_t k = 0, l;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(!strv_isempty(argv));
        প্রতিজ্ঞা(ret);

        l = strv_দৈর্ঘ্য(argv);

        n = new(অক্ষর*, l+1);
        যদি (!n)
                ফেরত -ENOMEM;

        STRV_FOREACH(i, argv) {
                ধ্রুবক অক্ষর *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                যদি (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ অক্ষর **m = ফাঁকা;
                        ধ্রুবক অক্ষর *name = word + 1;
                        অক্ষর *e, **w;
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
                                ফেরত r;

                        q = strv_দৈর্ঘ্য(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, আকার(অক্ষর*));
                        যদি (!w)
                                ফেরত -ENOMEM;

                        n = w;
                        যদি (m) {
                                memcpy(n + k, m, (q + 1) * আকার(অক্ষর*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ফাঁকা;
                        চালু;
                }

                _cleanup_strv_free_ অক্ষর **u = ফাঁকা, **b = ফাঁকা;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ফাঁকা,
                                ret_bad_variables ? &b : ফাঁকা);
                যদি (r < 0)
                        ফেরত r;
                n[++k] = ফাঁকা;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                যদি (r < 0)
                        ফেরত r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                যদি (r < 0)
                        ফেরত r;
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
        ফেরত 0;
}

পূর্ণসংখ্যা getenv_bool(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *e;

        e = getenv(p);
        যদি (!e)
                ফেরত -ENXIO;

        ফেরত parse_boolean(e);
}

পূর্ণসংখ্যা secure_getenv_bool(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *e;

        e = secure_getenv(p);
        যদি (!e)
                ফেরত -ENXIO;

        ফেরত parse_boolean(e);
}

পূর্ণসংখ্যা secure_getenv_uint64(ধ্রুবক অক্ষর *p, uint64_t *ret) {
        ধ্রুবক অক্ষর *e;

        প্রতিজ্ঞা(p);

        e = secure_getenv(p);
        যদি (!e)
                ফেরত -ENXIO;

        ফেরত safe_atou64(e, ret);
}

পূর্ণসংখ্যা set_unset_env(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *মান, bool overwrite) {
        প্রতিজ্ঞা(name);

        যদি (মান)
                ফেরত RET_NERRNO(setenv(name, মান, overwrite));

        ফেরত RET_NERRNO(unsetenv(name));
}

পূর্ণসংখ্যা putenv_dup(ধ্রুবক অক্ষর *assignment, bool override) {
        ধ্রুবক অক্ষর *e, *n;

        e = strchr(assignment, '=');
        যদি (!e)
                ফেরত -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ফেরত RET_NERRNO(setenv(n, e + 1, override));
}

পূর্ণসংখ্যা setenv_systemd_exec_pid(bool update_only) {
        ধ্রুবক অক্ষর *e;
        পূর্ণসংখ্যা r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        যদি (!e && update_only)
                ফেরত 0;

        যদি (streq_ptr(e, "*"))
                ফেরত 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

পূর্ণসংখ্যা setenv_systemd_log_level(শূন্য) {
        _cleanup_free_ অক্ষর *val = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        যদি (r < 0)
                ফেরত r;

        ফেরত RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

পূর্ণসংখ্যা getenv_path_list(ধ্রুবক অক্ষর *name, অক্ষর ***ret_paths) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        ধ্রুবক অক্ষর *e;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret_paths);

        e = secure_getenv(name);
        যদি (!e)
                ফেরত -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                যদি (!path_is_absolute(*p))
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                যদি (!path_is_normalized(*p))
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                যদি (path_equal(*p, "/"))
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        যদি (strv_isempty(l))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ফেরত 1;
}

পূর্ণসংখ্যা getenv_steal_erase(ধ্রুবক অক্ষর *name, অক্ষর **ret) {
        _cleanup_(erase_and_freep) অক্ষর *a = ফাঁকা;
        অক্ষর *e;

        প্রতিজ্ঞা(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        যদি (!e) {
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত 0;
        }

        যদি (ret) {
                a = strdup(e);
                যদি (!a)
                        ফেরত -ENOMEM;
        }

        string_erase(e);

        যদি (unsetenv(name) < 0)
                ফেরত -errno;

        যদি (ret)
                *ret = TAKE_PTR(a);

        ফেরত 1;
}

পূর্ণসংখ্যা setenvf(ধ্রুবক অক্ষর *name, bool overwrite, ধ্রুবক অক্ষর *valuef, ...) {
        _cleanup_free_ অক্ষর *মান = ফাঁকা;
        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);

        যদি (!valuef)
                ফেরত RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        যদি (streq_ptr(getenv(name), মান))
                ফেরত 0;

        ফেরত RET_NERRNO(setenv(name, মান, overwrite));
}
