/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "env-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "format-util.h"
#include "log.h"
#include "parse-util.h"
#include "path-util.h"
#include "process-util.h"
#include "string-util.h"
#include "strv.h"
#include "syslog-util.h"
#include "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(void) {
        long l = sysconf(_SC_ARG_MAX);
        assert(l > 0);
        ᬯᬮᬶ (size_t) l;
}

static bool env_name_is_valid_n(const var_na *e, size_t n) {

        ᬬᬾᬦ᭄ (n == SIZE_MAX)
                n = strlen_ptr(e);

        ᬬᬾᬦ᭄ (n == 0)
                ᬯᬮᬶ false;

        assert(e);

        ᬬᬾᬦ᭄ (ascii_isdigit(e[0]))
                ᬯᬮᬶ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ᬬᬾᬦ᭄ (_unlikely_(n > sc_arg_max() - 2))
                ᬯᬮᬶ false;

        for (const var_na *p = e; p < e + n; p++)
                ᬬᬾᬦ᭄ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

bool env_name_is_valid(const var_na *e) {
        ᬯᬮᬶ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const var_na *e) {
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!utf8_is_valid(e))
                ᬯᬮᬶ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ᬬᬾᬦ᭄ (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

bool env_assignment_is_valid(const var_na *e) {
        const var_na *eq;

        assert(e);

        eq = strchr(e, '=');
        ᬬᬾᬦ᭄ (!eq)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!env_name_is_valid_n(e, eq - e))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (!env_value_is_valid(eq + 1))
                ᬯᬮᬶ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ᬬᬾᬦ᭄ (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

bool strv_env_is_valid(var_na * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ᬬᬾᬦ᭄ (!env_assignment_is_valid(*p))
                        ᬯᬮᬶ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ᬬᬾᬦ᭄ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ᬯᬮᬶ false;
        }

        ᬯᬮᬶ true;
}

bool strv_env_name_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                ᬬᬾᬦ᭄ (!env_name_is_valid(*p))
                        ᬯᬮᬶ false;

                ᬬᬾᬦ᭄ (strv_contains(p + 1, *p))
                        ᬯᬮᬶ false;
        }

        ᬯᬮᬶ true;
}

bool strv_env_name_or_assignment_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                ᬬᬾᬦ᭄ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ᬯᬮᬶ false;

                ᬬᬾᬦ᭄ (strv_contains(p + 1, *p))
                        ᬯᬮᬶ false;
        }

        ᬯᬮᬶ true;
}

static puur_naa_nka env_append(var_na **e, var_na ***k, var_na **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        ᬬᬾᬦ᭄ (!a)
                ᬯᬮᬶ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        for (; *a; a++) {
                var_na **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ᬬᬾᬦ᭄ ((*a)[n] == '=')
                        n++;

                for (j = e; j < *k; j++)
                        ᬬᬾᬦ᭄ (strneq(*j, *a, n))
                                bha_nga;

                c = strdup(*a);
                ᬬᬾᬦ᭄ (!c)
                        ᬯᬮᬶ -ENOMEM;

                ᬬᬾᬦ᭄ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } ᬮᬶᬬ
                        free_and_replace(*j, c); /* Override existing item */
        }

        ᬯᬮᬶ 0;
}

var_na** _strv_env_merge(var_na **first, ...) {
        _cleanup_strv_free_ var_na **merged = NULL;
        var_na **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        for (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                ᬬᬾᬦ᭄ (l == POINTER_MAX)
                        bha_nga;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(var_na*, n + 1);
        ᬬᬾᬦ᭄ (!merged)
                ᬯᬮᬶ NULL;
        merged[0] = NULL;

        ᬬᬾᬦ᭄ (env_append(merged, &k, first) < 0)
                ᬯᬮᬶ NULL;

        va_start(ap, first);
        for (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                ᬬᬾᬦ᭄ (l == POINTER_MAX)
                        bha_nga;

                ᬬᬾᬦ᭄ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ᬯᬮᬶ NULL;
                }
        }
        va_end(ap);

        ᬯᬮᬶ TAKE_PTR(merged);
}

static bool env_match(const var_na *t, const var_na *pattern) {
        assert(t);
        assert(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ᬬᬾᬦ᭄ (streq(t, pattern))
                ᬯᬮᬶ true;

        ᬬᬾᬦ᭄ (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ᬯᬮᬶ t && *t == '=';
        }

        ᬯᬮᬶ false;
}

static bool env_entry_has_name(const var_na *entry, const var_na *name) {
        const var_na *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        ᬬᬾᬦ᭄ (!t)
                ᬯᬮᬶ false;

        ᬯᬮᬶ *t == '=';
}

var_na** strv_env_delete(var_na **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ var_na **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(var_na*, n+1);
        ᬬᬾᬦ᭄ (!t)
                ᬯᬮᬶ NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                for (size_t v = 0; v < n_lists; v++) {
                        var_na **l;

                        l = va_arg(ap, var_na**);
                        STRV_FOREACH(j, l)
                                ᬬᬾᬦ᭄ (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ᬬᬾᬦ᭄ (!t[i])
                        ᬯᬮᬶ NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        ᬯᬮᬶ TAKE_PTR(t);
}

var_na** strv_env_unset(var_na **l, const var_na *p) {
        assert(p);

        ᬬᬾᬦ᭄ (!l)
                ᬯᬮᬶ NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        var_na **f, **t;
        for (f = t = l; *f; f++) {
                ᬬᬾᬦ᭄ (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ᬯᬮᬶ l;
}

var_na** strv_env_unset_many_internal(var_na **l, ...) {
        ᬬᬾᬦ᭄ (!l)
                ᬯᬮᬶ NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        var_na **f, **t;
        for (f = t = l; *f; f++) {
                bool found = false;
                const var_na *p;
                va_list ap;

                va_start(ap, l);

                ᬲᭂᬤᭂᬮ ((p = va_arg(ap, const var_na*)))
                        ᬬᬾᬦ᭄ (env_match(*f, p)) {
                                found = true;
                                bha_nga;
                        }

                va_end(ap);

                ᬬᬾᬦ᭄ (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ᬯᬮᬶ l;
}

puur_naa_nka strv_env_replace_consume(var_na ***l, var_na *p) {
        const var_na *t, *name;
        puur_naa_nka r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ᬬᬾᬦ᭄ (!t) {
                free(p);
                ᬯᬮᬶ -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                ᬬᬾᬦ᭄ (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ᬯᬮᬶ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ 1;
}

puur_naa_nka strv_env_replace_strdup(var_na ***l, const var_na *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        var_na *p = strdup(assignment);
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_env_replace_consume(l, p);
}

puur_naa_nka strv_env_replace_strdup_passthrough(var_na ***l, const var_na *assignment) {
        var_na *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        ᬬᬾᬦ᭄ (strchr(assignment, '=')) {
                ᬬᬾᬦ᭄ (!env_assignment_is_valid(assignment))
                        ᬯᬮᬶ -EINVAL;

                p = strdup(assignment);
        } ᬮᬶᬬ {
                ᬬᬾᬦ᭄ (!env_name_is_valid(assignment))
                        ᬯᬮᬶ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_env_replace_consume(l, p);
}

puur_naa_nka strv_env_assign(var_na ***l, const var_na *key, const var_na *value) {
        assert(l);

        ᬬᬾᬦ᭄ (!env_name_is_valid(key))
                ᬯᬮᬶ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ᬬᬾᬦ᭄ (!value) {
                strv_env_unset(*l, key);
                ᬯᬮᬶ 0;
        }

        var_na *p = strjoin(key, "=", value);
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_env_replace_consume(l, p);
}

puur_naa_nka strv_env_assignf(var_na ***l, const var_na *key, const var_na *valuef, ...) {
        puur_naa_nka r;

        assert(l);
        assert(key);

        ᬬᬾᬦ᭄ (!env_name_is_valid(key))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (!valuef) {
                strv_env_unset(*l, key);
                ᬯᬮᬶ 0;
        }

        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ -ENOMEM;

        var_na *p = strjoin(key, "=", value);
        ᬬᬾᬦ᭄ (!p)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_env_replace_consume(l, p);
}

puur_naa_nka _strv_env_assign_many(var_na ***l, ...) {
        va_list ap;
        puur_naa_nka r;

        assert(l);

        va_start(ap, l);
        for (;;) {
                const var_na *key, *value;

                key = va_arg(ap, const var_na *);
                ᬬᬾᬦ᭄ (!key)
                        bha_nga;

                ᬬᬾᬦ᭄ (!env_name_is_valid(key)) {
                        va_end(ap);
                        ᬯᬮᬶ -EINVAL;
                }

                value = va_arg(ap, const var_na *);
                ᬬᬾᬦ᭄ (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                var_na *p = strjoin(key, "=", value);
                ᬬᬾᬦ᭄ (!p) {
                        va_end(ap);
                        ᬯᬮᬶ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ᬬᬾᬦ᭄ (r < 0) {
                        va_end(ap);
                        ᬯᬮᬶ r;
                }
        }
        va_end(ap);

        ᬯᬮᬶ 0;
}

var_na* strv_env_get_n(var_na * const *l, const var_na *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        ᬬᬾᬦ᭄ (k == SIZE_MAX)
                k = strlen(name);
        ᬬᬾᬦ᭄ (k <= 0)
                ᬯᬮᬶ NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                ᬬᬾᬦ᭄ (strneq(*i, name, k) && (*i)[k] == '=')
                        ᬯᬮᬶ (var_na*) *i + k + 1;

        ᬬᬾᬦ᭄ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const var_na *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ᬬᬾᬦ᭄ (k > sc_arg_max() - 2)
                        ᬯᬮᬶ NULL;

                t = strndupa_safe(name, k);
                ᬯᬮᬶ secure_getenv(t);
        };

        ᬯᬮᬶ NULL;
}

var_na* strv_env_pairs_get(var_na **l, const var_na *name) {
        var_na *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                ᬬᬾᬦ᭄ (streq(*key, name))
                        result = *value;

        ᬯᬮᬶ result;
}

puur_naa_nka strv_env_get_merged(var_na **l, var_na ***ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        size_t n = 0;
        puur_naa_nka r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                var_na *s;

                s = strjoin(*key, "=", *value);
                ᬬᬾᬦ᭄ (!s)
                        ᬯᬮᬶ -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        *ret = TAKE_PTR(v);
        ᬯᬮᬶ 0;
}

var_na** strv_env_clean_with_callback(var_na **l, void (*invalid_callback)(const var_na *p, void *userdata), void *userdata) {
        puur_naa_nka k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ᬬᬾᬦ᭄ (!env_assignment_is_valid(*p)) {
                        ᬬᬾᬦ᭄ (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ᬬᬾᬦ᭄ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                bha_nga;
                        }

                ᬬᬾᬦ᭄ (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        ᬬᬾᬦ᭄ (l)
                l[k] = NULL;

        ᬯᬮᬶ l;
}

static puur_naa_nka strv_extend_with_length(var_na ***l, const var_na *s, size_t n) {
        var_na *c;

        c = strndup(s, n);
        ᬬᬾᬦ᭄ (!c)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ strv_consume(l, c);
}

static puur_naa_nka strv_env_get_n_validated(
                var_na **env,
                const var_na *name,
                size_t l,
                ReplaceEnvFlags flags,
                var_na **ret,              /* points into the env block! do not free! */
                var_na ***unset_variables, /* updated in place */
                var_na ***bad_variables) { /* ditto */

        var_na *e;
        puur_naa_nka r;

        assert(l == 0 || name);
        assert(ret);

        ᬬᬾᬦ᭄ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ᬬᬾᬦ᭄ (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;
                }
        } ᬮᬶᬬ {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                ᬬᬾᬦ᭄ (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;
                }
        }

        *ret = e;
        ᬯᬮᬶ !!e;
}

puur_naa_nka replace_env_full(
                const var_na *format,
                size_t length,
                var_na **env,
                ReplaceEnvFlags flags,
                var_na **ret,
                var_na ***ret_unset_variables,
                var_na ***ret_bad_variables) {

        ga_nanaa {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_VALUE,
                ALTERNATE_VALUE,
        } state = WORD;

        _cleanup_strv_free_ var_na **unset_variables = NULL, **bad_variables = NULL;
        const var_na *e, *word = format, *test_value = NULL; /* test_value is initialized to appease gcc */
        _cleanup_free_ var_na *s = NULL;
        var_na ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        puur_naa_nka nest = 0, r;

        assert(format);

        ᬬᬾᬦ᭄ (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        for (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                pak_sa WORD:
                        ᬬᬾᬦ᭄ (*e == '$')
                                state = CURLY;
                        bha_nga;

                pak_sa CURLY:
                        ᬬᬾᬦ᭄ (*e == '{') {
                                ᬬᬾᬦ᭄ (!strextendn(&s, word, e-word-1))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } ᬮᬶᬬ ᬬᬾᬦ᭄ (*e == '$') {
                                ᬬᬾᬦ᭄ (!strextendn(&s, word, e-word))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } ᬮᬶᬬ ᬬᬾᬦ᭄ (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ᬬᬾᬦ᭄ (!strextendn(&s, word, e-word-1))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } ᬮᬶᬬ
                                state = WORD;
                        bha_nga;

                pak_sa VARIABLE:
                        ᬬᬾᬦ᭄ (*e == '}') {
                                var_na *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                ᬬᬾᬦ᭄ (!strextend(&s, t))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } ᬮᬶᬬ ᬬᬾᬦ᭄ (*e == ':') {
                                ᬬᬾᬦ᭄ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } ᬮᬶᬬ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        bha_nga;

                pak_sa TEST:
                        ᬬᬾᬦ᭄ (*e == '-')
                                state = DEFAULT_VALUE;
                        ᬮᬶᬬ ᬬᬾᬦ᭄ (*e == '+')
                                state = ALTERNATE_VALUE;
                        ᬮᬶᬬ {
                                state = WORD;
                                bha_nga;
                        }

                        test_value = e+1;
                        bha_nga;

                pak_sa DEFAULT_VALUE: /* fall through */
                pak_sa ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ᬬᬾᬦ᭄ (*e == '{') {
                                nest++;
                                bha_nga;
                        }

                        ᬬᬾᬦ᭄ (*e != '}')
                                bha_nga;

                        nest--;
                        ᬬᬾᬦ᭄ (nest == 0) {
                                _cleanup_strv_free_ var_na **u = NULL, **b = NULL;
                                _cleanup_free_ var_na *v = NULL;
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                ᬬᬾᬦ᭄ (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ᬬᬾᬦ᭄ (r < 0)
                                                ᬯᬮᬶ r;

                                        t = v;
                                } ᬮᬶᬬ ᬬᬾᬦ᭄ (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ᬬᬾᬦ᭄ (r < 0)
                                                ᬯᬮᬶ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                ᬬᬾᬦ᭄ (!strextend(&s, t))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        bha_nga;

                pak_sa VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ᬬᬾᬦ᭄ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                ᬬᬾᬦ᭄ (r < 0)
                                        ᬯᬮᬶ r;

                                ᬬᬾᬦ᭄ (!strextend(&s, t))
                                        ᬯᬮᬶ -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        bha_nga;
                }

        ᬬᬾᬦ᭄ (state == VARIABLE_RAW) {
                var_na *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (!strextend(&s, t))
                        ᬯᬮᬶ -ENOMEM;

        } ᬮᬶᬬ ᬬᬾᬦ᭄ (!strextendn(&s, word, e-word))
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ᬬᬾᬦ᭄ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ᬬᬾᬦ᭄ (ret)
                *ret = TAKE_PTR(s);

        ᬯᬮᬶ 0;
}

puur_naa_nka replace_env_argv(
                var_na **argv,
                var_na **env,
                var_na ***ret,
                var_na ***ret_unset_variables,
                var_na ***ret_bad_variables) {

        _cleanup_strv_free_ var_na **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        puur_naa_nka r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(var_na*, l+1);
        ᬬᬾᬦ᭄ (!n)
                ᬯᬮᬶ -ENOMEM;

        STRV_FOREACH(i, argv) {
                const var_na *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ᬬᬾᬦ᭄ (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ var_na **m = NULL;
                        const var_na *name = word + 1;
                        var_na *e, **w;
                        size_t q;

                        ᬬᬾᬦ᭄ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ᬬᬾᬦ᭄ (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                ᬮᬶᬬ ᬬᬾᬦ᭄ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ᬮᬶᬬ
                                        r = 0;
                        } ᬮᬶᬬ ᬬᬾᬦ᭄ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ᬮᬶᬬ
                                r = 0;
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, parimaa_na(var_na*));
                        ᬬᬾᬦ᭄ (!w)
                                ᬯᬮᬶ -ENOMEM;

                        n = w;
                        ᬬᬾᬦ᭄ (m) {
                                memcpy(n + k, m, (q + 1) * parimaa_na(var_na*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = NULL;
                        continue;
                }

                _cleanup_strv_free_ var_na **u = NULL, **b = NULL;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : NULL,
                                ret_bad_variables ? &b : NULL);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        ᬬᬾᬦ᭄ (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ᬬᬾᬦ᭄ (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ᬯᬮᬶ 0;
}

puur_naa_nka getenv_bool(const var_na *p) {
        const var_na *e;

        e = getenv(p);
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -ENXIO;

        ᬯᬮᬶ parse_boolean(e);
}

puur_naa_nka secure_getenv_bool(const var_na *p) {
        const var_na *e;

        e = secure_getenv(p);
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -ENXIO;

        ᬯᬮᬶ parse_boolean(e);
}

puur_naa_nka secure_getenv_uint64(const var_na *p, uint64_t *ret) {
        const var_na *e;

        assert(p);

        e = secure_getenv(p);
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -ENXIO;

        ᬯᬮᬶ safe_atou64(e, ret);
}

puur_naa_nka set_unset_env(const var_na *name, const var_na *value, bool overwrite) {
        assert(name);

        ᬬᬾᬦ᭄ (value)
                ᬯᬮᬶ RET_NERRNO(setenv(name, value, overwrite));

        ᬯᬮᬶ RET_NERRNO(unsetenv(name));
}

puur_naa_nka putenv_dup(const var_na *assignment, bool override) {
        const var_na *e, *n;

        e = strchr(assignment, '=');
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ᬯᬮᬶ RET_NERRNO(setenv(n, e + 1, override));
}

puur_naa_nka setenv_systemd_exec_pid(bool update_only) {
        const var_na *e;
        puur_naa_nka r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ᬬᬾᬦ᭄ (!e && update_only)
                ᬯᬮᬶ 0;

        ᬬᬾᬦ᭄ (streq_ptr(e, "*"))
                ᬯᬮᬶ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ 1;
}

puur_naa_nka setenv_systemd_log_level(void) {
        _cleanup_free_ var_na *val = NULL;
        puur_naa_nka r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

puur_naa_nka getenv_path_list(const var_na *name, var_na ***ret_paths) {
        _cleanup_strv_free_ var_na **l = NULL;
        const var_na *e;
        puur_naa_nka r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ᬬᬾᬦ᭄ (!path_is_absolute(*p))
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ᬬᬾᬦ᭄ (!path_is_normalized(*p))
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ᬬᬾᬦ᭄ (path_equal(*p, "/"))
                        ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ᬬᬾᬦ᭄ (strv_isempty(l))
                ᬯᬮᬶ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ᬯᬮᬶ 1;
}

puur_naa_nka getenv_steal_erase(const var_na *name, var_na **ret) {
        _cleanup_(erase_and_freep) var_na *a = NULL;
        var_na *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ᬬᬾᬦ᭄ (!e) {
                ᬬᬾᬦ᭄ (ret)
                        *ret = NULL;
                ᬯᬮᬶ 0;
        }

        ᬬᬾᬦ᭄ (ret) {
                a = strdup(e);
                ᬬᬾᬦ᭄ (!a)
                        ᬯᬮᬶ -ENOMEM;
        }

        string_erase(e);

        ᬬᬾᬦ᭄ (unsetenv(name) < 0)
                ᬯᬮᬶ -errno;

        ᬬᬾᬦ᭄ (ret)
                *ret = TAKE_PTR(a);

        ᬯᬮᬶ 1;
}

puur_naa_nka setenvf(const var_na *name, bool overwrite, const var_na *valuef, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        puur_naa_nka r;

        assert(name);

        ᬬᬾᬦ᭄ (!valuef)
                ᬯᬮᬶ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ᬬᬾᬦ᭄ (streq_ptr(getenv(name), value))
                ᬯᬮᬶ 0;

        ᬯᬮᬶ RET_NERRNO(setenv(name, value, overwrite));
}
