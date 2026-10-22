/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "format-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "syslog-util.h"
#অন্তর্ভাৱ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(শূন্য) {
        long l = sysconf(_SC_ARG_MAX);
        প্রতিজ্ঞা(l > 0);
        ঘূৰাই (size_t) l;
}

স্থির bool env_name_is_valid_n(ধ্রুৱ ৱর্ণ *e, size_t n) {

        যদি (n == SIZE_MAX)
                n = strlen_ptr(e);

        যদি (n == 0)
                ঘূৰাই false;

        প্রতিজ্ঞা(e);

        যদি (ascii_isdigit(e[0]))
                ঘূৰাই false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        যদি (_unlikely_(n > sc_arg_max() - 2))
                ঘূৰাই false;

        বাবে (ধ্রুৱ ৱর্ণ *p = e; p < e + n; p++)
                যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ঘূৰাই false;

        ঘূৰাই true;
}

bool env_name_is_valid(ধ্রুৱ ৱর্ণ *e) {
        ঘূৰাই env_name_is_valid_n(e, SIZE_MAX);
}

bool env_মান_is_valid(ধ্রুৱ ৱর্ণ *e) {
        যদি (!e)
                ঘূৰাই false;

        যদি (!utf8_is_valid(e))
                ঘূৰাই false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ঘূৰাই false;

        ঘূৰাই true;
}

bool env_assignment_is_valid(ধ্রুৱ ৱর্ণ *e) {
        ধ্রুৱ ৱর্ণ *eq;

        প্রতিজ্ঞা(e);

        eq = strchr(e, '=');
        যদি (!eq)
                ঘূৰাই false;

        যদি (!env_name_is_valid_n(e, eq - e))
                ঘূৰাই false;

        যদি (!env_মান_is_valid(eq + 1))
                ঘূৰাই false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        যদি (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ঘূৰাই false;

        ঘূৰাই true;
}

bool strv_env_is_valid(ৱর্ণ * ধ্রুৱ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                যদি (!env_assignment_is_valid(*p))
                        ঘূৰাই false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, k) && (*q)[k] == '=')
                                ঘূৰাই false;
        }

        ঘূৰাই true;
}

bool strv_env_name_is_valid(ৱর্ণ * ধ্রুৱ *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_name_is_valid(*p))
                        ঘূৰাই false;

                যদি (strv_contains(p + 1, *p))
                        ঘূৰাই false;
        }

        ঘূৰাই true;
}

bool strv_env_name_or_assignment_is_valid(ৱর্ণ * ধ্রুৱ *l) {
        STRV_FOREACH(p, l) {
                যদি (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ঘূৰাই false;

                যদি (strv_contains(p + 1, *p))
                        ঘূৰাই false;
        }

        ঘূৰাই true;
}

স্থির পূর্ণাঙ্ক env_append(ৱর্ণ **e, ৱর্ণ ***k, ৱর্ণ **a) {
        প্রতিজ্ঞা(e);
        প্রতিজ্ঞা(k);
        প্রতিজ্ঞা(*k >= e);

        যদি (!a)
                ঘূৰাই 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        বাবে (; *a; a++) {
                ৱর্ণ **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                যদি ((*a)[n] == '=')
                        n++;

                বাবে (j = e; j < *k; j++)
                        যদি (strneq(*j, *a, n))
                                ভঙ্গ;

                c = strdup(*a);
                যদি (!c)
                        ঘূৰাই -ENOMEM;

                যদি (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = রিক্ত;
                        (*k)++;
                } অন্যথা
                        free_and_replace(*j, c); /* Override existing item */
        }

        ঘূৰাই 0;
}

ৱর্ণ** _strv_env_merge(ৱর্ণ **first, ...) {
        _cleanup_strv_free_ ৱর্ণ **merged = রিক্ত;
        ৱর্ণ **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        বাবে (;;) {
                ৱর্ণ **l;

                l = va_arg(ap, ৱর্ণ**);
                যদি (l == POINTER_MAX)
                        ভঙ্গ;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(ৱর্ণ*, n + 1);
        যদি (!merged)
                ঘূৰাই রিক্ত;
        merged[0] = রিক্ত;

        যদি (env_append(merged, &k, first) < 0)
                ঘূৰাই রিক্ত;

        va_start(ap, first);
        বাবে (;;) {
                ৱর্ণ **l;

                l = va_arg(ap, ৱর্ণ**);
                যদি (l == POINTER_MAX)
                        ভঙ্গ;

                যদি (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ঘূৰাই রিক্ত;
                }
        }
        va_end(ap);

        ঘূৰাই TAKE_PTR(merged);
}

স্থির bool env_match(ধ্রুৱ ৱর্ণ *t, ধ্রুৱ ৱর্ণ *pattern) {
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
                ঘূৰাই true;

        যদি (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ঘূৰাই t && *t == '=';
        }

        ঘূৰাই false;
}

স্থির bool env_entry_has_name(ধ্রুৱ ৱর্ণ *entry, ধ্রুৱ ৱর্ণ *name) {
        ধ্রুৱ ৱর্ণ *t;

        প্রতিজ্ঞা(entry);
        প্রতিজ্ঞা(name);

        t = startswith(entry, name);
        যদি (!t)
                ঘূৰাই false;

        ঘূৰাই *t == '=';
}

ৱর্ণ** strv_env_delete(ৱর্ণ **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ ৱর্ণ **t = রিক্ত;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(ৱর্ণ*, n+1);
        যদি (!t)
                ঘূৰাই রিক্ত;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                বাবে (size_t v = 0; v < n_lists; v++) {
                        ৱর্ণ **l;

                        l = va_arg(ap, ৱর্ণ**);
                        STRV_FOREACH(j, l)
                                যদি (env_match(*k, *j))
                                        গচ্ছ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                যদি (!t[i])
                        ঘূৰাই রিক্ত;

                i++;
                অনুৱর্ত;

        skip:
                va_end(ap);
        }

        t[i] = রিক্ত;

        প্রতিজ্ঞা(i <= n);

        ঘূৰাই TAKE_PTR(t);
}

ৱর্ণ** strv_env_unset(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *p) {
        প্রতিজ্ঞা(p);

        যদি (!l)
                ঘূৰাই রিক্ত;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        ৱর্ণ **f, **t;
        বাবে (f = t = l; *f; f++) {
                যদি (env_match(*f, p)) {
                        মুক্ত(*f);
                        অনুৱর্ত;
                }

                *(t++) = *f;
        }

        *t = রিক্ত;
        ঘূৰাই l;
}

ৱর্ণ** strv_env_unset_many_internal(ৱর্ণ **l, ...) {
        যদি (!l)
                ঘূৰাই রিক্ত;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        ৱর্ণ **f, **t;
        বাবে (f = t = l; *f; f++) {
                bool found = false;
                ধ্রুৱ ৱর্ণ *p;
                va_list ap;

                va_start(ap, l);

                যেতিয়ালৈ ((p = va_arg(ap, ধ্রুৱ ৱর্ণ*)))
                        যদি (env_match(*f, p)) {
                                found = true;
                                ভঙ্গ;
                        }

                va_end(ap);

                যদি (found) {
                        মুক্ত(*f);
                        অনুৱর্ত;
                }

                *(t++) = *f;
        }

        *t = রিক্ত;
        ঘূৰাই l;
}

পূর্ণাঙ্ক strv_env_replace_consume(ৱর্ণ ***l, ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *t, *name;
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
                ঘূৰাই -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                যদি (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ঘূৰাই 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক strv_env_replace_strdup(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        ৱর্ণ *p = strdup(assignment);
        যদি (!p)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_replace_strdup_passthrough(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *assignment) {
        ৱর্ণ *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(assignment);

        যদি (strchr(assignment, '=')) {
                যদি (!env_assignment_is_valid(assignment))
                        ঘূৰাই -EINVAL;

                p = strdup(assignment);
        } অন্যথা {
                যদি (!env_name_is_valid(assignment))
                        ঘূৰাই -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        যদি (!p)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_assign(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        প্রতিজ্ঞা(l);

        যদি (!env_name_is_valid(চাবি))
                ঘূৰাই -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        যদি (!মান) {
                strv_env_unset(*l, চাবি);
                ঘূৰাই 0;
        }

        ৱর্ণ *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক strv_env_assignf(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *valuef, ...) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);
        প্রতিজ্ঞা(চাবি);

        যদি (!env_name_is_valid(চাবি))
                ঘূৰাই -EINVAL;

        যদি (!valuef) {
                strv_env_unset(*l, চাবি);
                ঘূৰাই 0;
        }

        _cleanup_free_ ৱর্ণ *মান = রিক্ত;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);
        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        ৱর্ণ *p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_env_replace_consume(l, p);
}

পূর্ণাঙ্ক _strv_env_assign_many(ৱর্ণ ***l, ...) {
        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l);

        va_start(ap, l);
        বাবে (;;) {
                ধ্রুৱ ৱর্ণ *চাবি, *মান;

                চাবি = va_arg(ap, ধ্রুৱ ৱর্ণ *);
                যদি (!চাবি)
                        ভঙ্গ;

                যদি (!env_name_is_valid(চাবি)) {
                        va_end(ap);
                        ঘূৰাই -EINVAL;
                }

                মান = va_arg(ap, ধ্রুৱ ৱর্ণ *);
                যদি (!মান) {
                        strv_env_unset(*l, চাবি);
                        অনুৱর্ত;
                }

                ৱর্ণ *p = strjoin(চাবি, "=", মান);
                যদি (!p) {
                        va_end(ap);
                        ঘূৰাই -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                যদি (r < 0) {
                        va_end(ap);
                        ঘূৰাই r;
                }
        }
        va_end(ap);

        ঘূৰাই 0;
}

ৱর্ণ* strv_env_get_n(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *name, size_t k, ReplaceEnvFlags flags) {
        প্রতিজ্ঞা(name);

        যদি (k == SIZE_MAX)
                k = strlen(name);
        যদি (k <= 0)
                ঘূৰাই রিক্ত;

        STRV_FOREACH_BACKWARDS(i, l)
                যদি (strneq(*i, name, k) && (*i)[k] == '=')
                        ঘূৰাই (ৱর্ণ*) *i + k + 1;

        যদি (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ধ্রুৱ ৱর্ণ *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                যদি (k > sc_arg_max() - 2)
                        ঘূৰাই রিক্ত;

                t = strndupa_safe(name, k);
                ঘূৰাই secure_getenv(t);
        };

        ঘূৰাই রিক্ত;
}

ৱর্ণ* strv_env_pairs_get(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *name) {
        ৱর্ণ *result = রিক্ত;

        প্রতিজ্ঞা(name);

        STRV_FOREACH_PAIR(চাবি, মান, l)
                যদি (streq(*চাবি, name))
                        result = *মান;

        ঘূৰাই result;
}

পূর্ণাঙ্ক strv_env_get_merged(ৱর্ণ **l, ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **v = রিক্ত;
        size_t n = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(চাবি, মান, l) {
                ৱর্ণ *s;

                s = strjoin(*চাবি, "=", *মান);
                যদি (!s)
                        ঘূৰাই -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(v);
        ঘূৰাই 0;
}

ৱর্ণ** strv_env_clean_with_callback(ৱর্ণ **l, শূন্য (*invalid_callback)(ধ্রুৱ ৱর্ণ *p, শূন্য *userdata), শূন্য *userdata) {
        পূর্ণাঙ্ক k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                যদি (!env_assignment_is_valid(*p)) {
                        যদি (invalid_callback)
                                invalid_callback(*p, userdata);
                        মুক্ত(*p);
                        অনুৱর্ত;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        যদি (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ভঙ্গ;
                        }

                যদি (duplicate) {
                        মুক্ত(*p);
                        অনুৱর্ত;
                }

                l[k++] = *p;
        }

        যদি (l)
                l[k] = রিক্ত;

        ঘূৰাই l;
}

স্থির পূর্ণাঙ্ক strv_extend_with_length(ৱর্ণ ***l, ধ্রুৱ ৱর্ণ *s, size_t n) {
        ৱর্ণ *c;

        c = strndup(s, n);
        যদি (!c)
                ঘূৰাই -ENOMEM;

        ঘূৰাই strv_consume(l, c);
}

স্থির পূর্ণাঙ্ক strv_env_get_n_validated(
                ৱর্ণ **env,
                ধ্রুৱ ৱর্ণ *name,
                size_t l,
                ReplaceEnvFlags flags,
                ৱর্ণ **ret,              /* points into the env block! do not free! */
                ৱর্ণ ***unset_variables, /* updated in place */
                ৱর্ণ ***bad_variables) { /* ditto */

        ৱর্ণ *e;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(l == 0 || name);
        প্রতিজ্ঞা(ret);

        যদি (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                যদি (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        যদি (r < 0)
                                ঘূৰাই r;
                }
        } অন্যথা {
                e = রিক্ত; /* Resolve invalid variable names the same way as unset ones */

                যদি (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        যদি (r < 0)
                                ঘূৰাই r;
                }
        }

        *ret = e;
        ঘূৰাই !!e;
}

পূর্ণাঙ্ক replace_env_full(
                ধ্রুৱ ৱর্ণ *format,
                size_t length,
                ৱর্ণ **env,
                ReplaceEnvFlags flags,
                ৱর্ণ **ret,
                ৱর্ণ ***ret_unset_variables,
                ৱর্ণ ***ret_bad_variables) {

        গণনা {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_মান,
                ALTERNATE_মান,
        } state = WORD;

        _cleanup_strv_free_ ৱর্ণ **unset_variables = রিক্ত, **bad_variables = রিক্ত;
        ধ্রুৱ ৱর্ণ *e, *word = format, *test_মান = রিক্ত; /* test_value is initialized to appease gcc */
        _cleanup_free_ ৱর্ণ *s = রিক্ত;
        ৱর্ণ ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        পূর্ণাঙ্ক nest = 0, r;

        প্রতিজ্ঞা(format);

        যদি (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : রিক্ত;
        pb = ret_bad_variables ? &bad_variables : রিক্ত;

        বাবে (e = format, i = 0; *e && i < length; e++, i++)
                ৱিকল্প (state) {

                পক্ষ WORD:
                        যদি (*e == '$')
                                state = CURLY;
                        ভঙ্গ;

                পক্ষ CURLY:
                        যদি (*e == '{') {
                                যদি (!strextendn(&s, word, e-word-1))
                                        ঘূৰাই -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } অন্যথা যদি (*e == '$') {
                                যদি (!strextendn(&s, word, e-word))
                                        ঘূৰাই -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } অন্যথা যদি (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                যদি (!strextendn(&s, word, e-word-1))
                                        ঘূৰাই -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } অন্যথা
                                state = WORD;
                        ভঙ্গ;

                পক্ষ VARIABLE:
                        যদি (*e == '}') {
                                ৱর্ণ *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                যদি (!strextend(&s, t))
                                        ঘূৰাই -ENOMEM;

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
                                _cleanup_strv_free_ ৱর্ণ **u = রিক্ত, **b = রিক্ত;
                                _cleanup_free_ ৱর্ণ *v = রিক্ত;
                                ৱর্ণ *t = রিক্ত;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                যদি (t && state == ALTERNATE_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : রিক্ত, pb ? &b : রিক্ত);
                                        যদি (r < 0)
                                                ঘূৰাই r;

                                        t = v;
                                } অন্যথা যদি (!t && state == DEFAULT_মান) {
                                        r = replace_env_full(test_মান, e-test_মান, env, flags, &v, pu ? &u : রিক্ত, pb ? &b : রিক্ত);
                                        যদি (r < 0)
                                                ঘূৰাই r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        ঘূৰাই r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                যদি (!strextend(&s, t))
                                        ঘূৰাই -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        ভঙ্গ;

                পক্ষ VARIABLE_RAW:
                        প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        যদি (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ৱর্ণ *t = রিক্ত;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                যদি (!strextend(&s, t))
                                        ঘূৰাই -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        ভঙ্গ;
                }

        যদি (state == VARIABLE_RAW) {
                ৱর্ণ *t;

                প্রতিজ্ঞা(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (!strextend(&s, t))
                        ঘূৰাই -ENOMEM;

        } অন্যথা যদি (!strextendn(&s, word, e-word))
                ঘূৰাই -ENOMEM;

        যদি (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        যদি (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        যদি (ret)
                *ret = TAKE_PTR(s);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক replace_env_argv(
                ৱর্ণ **argv,
                ৱর্ণ **env,
                ৱর্ণ ***ret,
                ৱর্ণ ***ret_unset_variables,
                ৱর্ণ ***ret_bad_variables) {

        _cleanup_strv_free_ ৱর্ণ **n = রিক্ত, **unset_variables = রিক্ত, **bad_variables = রিক্ত;
        size_t k = 0, l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(!strv_isempty(argv));
        প্রতিজ্ঞা(ret);

        l = strv_length(argv);

        n = new(ৱর্ণ*, l+1);
        যদি (!n)
                ঘূৰাই -ENOMEM;

        STRV_FOREACH(i, argv) {
                ধ্রুৱ ৱর্ণ *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                যদি (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ ৱর্ণ **m = রিক্ত;
                        ধ্রুৱ ৱর্ণ *name = word + 1;
                        ৱর্ণ *e, **w;
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
                                ঘূৰাই r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, পরিমাণ(ৱর্ণ*));
                        যদি (!w)
                                ঘূৰাই -ENOMEM;

                        n = w;
                        যদি (m) {
                                memcpy(n + k, m, (q + 1) * পরিমাণ(ৱর্ণ*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = রিক্ত;
                        অনুৱর্ত;
                }

                _cleanup_strv_free_ ৱর্ণ **u = রিক্ত, **b = রিক্ত;

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
                        ঘূৰাই r;
                n[++k] = রিক্ত;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                যদি (r < 0)
                        ঘূৰাই r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                যদি (r < 0)
                        ঘূৰাই r;
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
        ঘূৰাই 0;
}

পূর্ণাঙ্ক getenv_bool(ধ্রুৱ ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *e;

        e = getenv(p);
        যদি (!e)
                ঘূৰাই -ENXIO;

        ঘূৰাই parse_boolean(e);
}

পূর্ণাঙ্ক secure_getenv_bool(ধ্রুৱ ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *e;

        e = secure_getenv(p);
        যদি (!e)
                ঘূৰাই -ENXIO;

        ঘূৰাই parse_boolean(e);
}

পূর্ণাঙ্ক secure_getenv_uint64(ধ্রুৱ ৱর্ণ *p, uint64_t *ret) {
        ধ্রুৱ ৱর্ণ *e;

        প্রতিজ্ঞা(p);

        e = secure_getenv(p);
        যদি (!e)
                ঘূৰাই -ENXIO;

        ঘূৰাই safe_atou64(e, ret);
}

পূর্ণাঙ্ক set_unset_env(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *মান, bool overwrite) {
        প্রতিজ্ঞা(name);

        যদি (মান)
                ঘূৰাই RET_NERRNO(setenv(name, মান, overwrite));

        ঘূৰাই RET_NERRNO(unsetenv(name));
}

পূর্ণাঙ্ক putenv_dup(ধ্রুৱ ৱর্ণ *assignment, bool override) {
        ধ্রুৱ ৱর্ণ *e, *n;

        e = strchr(assignment, '=');
        যদি (!e)
                ঘূৰাই -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ঘূৰাই RET_NERRNO(setenv(n, e + 1, override));
}

পূর্ণাঙ্ক setenv_systemd_exec_pid(bool update_only) {
        ধ্রুৱ ৱর্ণ *e;
        পূর্ণাঙ্ক r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        যদি (!e && update_only)
                ঘূৰাই 0;

        যদি (streq_ptr(e, "*"))
                ঘূৰাই 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক setenv_systemd_log_level(শূন্য) {
        _cleanup_free_ ৱর্ণ *val = রিক্ত;
        পূর্ণাঙ্ক r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

পূর্ণাঙ্ক getenv_path_list(ধ্রুৱ ৱর্ণ *name, ৱর্ণ ***ret_paths) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        ধ্রুৱ ৱর্ণ *e;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(ret_paths);

        e = secure_getenv(name);
        যদি (!e)
                ঘূৰাই -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ঘূৰাই log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                যদি (!path_is_absolute(*p))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                যদি (!path_is_normalized(*p))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                যদি (path_equal(*p, "/"))
                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        যদি (strv_isempty(l))
                ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ঘূৰাই 1;
}

পূর্ণাঙ্ক getenv_steal_erase(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret) {
        _cleanup_(erase_and_freep) ৱর্ণ *a = রিক্ত;
        ৱর্ণ *e;

        প্রতিজ্ঞা(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        যদি (!e) {
                যদি (ret)
                        *ret = রিক্ত;
                ঘূৰাই 0;
        }

        যদি (ret) {
                a = strdup(e);
                যদি (!a)
                        ঘূৰাই -ENOMEM;
        }

        string_erase(e);

        যদি (unsetenv(name) < 0)
                ঘূৰাই -errno;

        যদি (ret)
                *ret = TAKE_PTR(a);

        ঘূৰাই 1;
}

পূর্ণাঙ্ক setenvf(ধ্রুৱ ৱর্ণ *name, bool overwrite, ধ্রুৱ ৱর্ণ *valuef, ...) {
        _cleanup_free_ ৱর্ণ *মান = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        যদি (!valuef)
                ঘূৰাই RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&মান, valuef, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        যদি (streq_ptr(getenv(name), মান))
                ঘূৰাই 0;

        ঘূৰাই RET_NERRNO(setenv(name, মান, overwrite));
}
