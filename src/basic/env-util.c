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
        ཕྱིར (size_t) l;
}

static bool env_name_is_valid_n(const var_na *e, size_t n) {

        གལ་ཏེ (n == SIZE_MAX)
                n = strlen_ptr(e);

        གལ་ཏེ (n == 0)
                ཕྱིར false;

        assert(e);

        གལ་ཏེ (ascii_isdigit(e[0]))
                ཕྱིར false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        གལ་ཏེ (_unlikely_(n > sc_arg_max() - 2))
                ཕྱིར false;

        ཆེད་དུ (const var_na *p = e; p < e + n; p++)
                གལ་ཏེ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ཕྱིར false;

        ཕྱིར true;
}

bool env_name_is_valid(const var_na *e) {
        ཕྱིར env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const var_na *e) {
        གལ་ཏེ (!e)
                ཕྱིར false;

        གལ་ཏེ (!utf8_is_valid(e))
                ཕྱིར false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        གལ་ཏེ (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ཕྱིར false;

        ཕྱིར true;
}

bool env_assignment_is_valid(const var_na *e) {
        const var_na *eq;

        assert(e);

        eq = strchr(e, '=');
        གལ་ཏེ (!eq)
                ཕྱིར false;

        གལ་ཏེ (!env_name_is_valid_n(e, eq - e))
                ཕྱིར false;

        གལ་ཏེ (!env_value_is_valid(eq + 1))
                ཕྱིར false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        གལ་ཏེ (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ཕྱིར false;

        ཕྱིར true;
}

bool strv_env_is_valid(var_na * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                གལ་ཏེ (!env_assignment_is_valid(*p))
                        ཕྱིར false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        གལ་ཏེ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ཕྱིར false;
        }

        ཕྱིར true;
}

bool strv_env_name_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                གལ་ཏེ (!env_name_is_valid(*p))
                        ཕྱིར false;

                གལ་ཏེ (strv_contains(p + 1, *p))
                        ཕྱིར false;
        }

        ཕྱིར true;
}

bool strv_env_name_or_assignment_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                གལ་ཏེ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ཕྱིར false;

                གལ་ཏེ (strv_contains(p + 1, *p))
                        ཕྱིར false;
        }

        ཕྱིར true;
}

static ཧྲིལ env_append(var_na **e, var_na ***k, var_na **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        གལ་ཏེ (!a)
                ཕྱིར 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        ཆེད་དུ (; *a; a++) {
                var_na **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                གལ་ཏེ ((*a)[n] == '=')
                        n++;

                ཆེད་དུ (j = e; j < *k; j++)
                        གལ་ཏེ (strneq(*j, *a, n))
                                bha_nga;

                c = strdup(*a);
                གལ་ཏེ (!c)
                        ཕྱིར -ENOMEM;

                གལ་ཏེ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } དེ་མིན
                        free_and_replace(*j, c); /* Override existing item */
        }

        ཕྱིར 0;
}

var_na** _strv_env_merge(var_na **first, ...) {
        _cleanup_strv_free_ var_na **merged = NULL;
        var_na **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        ཆེད་དུ (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                གལ་ཏེ (l == POINTER_MAX)
                        bha_nga;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(var_na*, n + 1);
        གལ་ཏེ (!merged)
                ཕྱིར NULL;
        merged[0] = NULL;

        གལ་ཏེ (env_append(merged, &k, first) < 0)
                ཕྱིར NULL;

        va_start(ap, first);
        ཆེད་དུ (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                གལ་ཏེ (l == POINTER_MAX)
                        bha_nga;

                གལ་ཏེ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ཕྱིར NULL;
                }
        }
        va_end(ap);

        ཕྱིར TAKE_PTR(merged);
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

        གལ་ཏེ (streq(t, pattern))
                ཕྱིར true;

        གལ་ཏེ (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ཕྱིར t && *t == '=';
        }

        ཕྱིར false;
}

static bool env_entry_has_name(const var_na *entry, const var_na *name) {
        const var_na *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        གལ་ཏེ (!t)
                ཕྱིར false;

        ཕྱིར *t == '=';
}

var_na** strv_env_delete(var_na **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ var_na **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(var_na*, n+1);
        གལ་ཏེ (!t)
                ཕྱིར NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                ཆེད་དུ (size_t v = 0; v < n_lists; v++) {
                        var_na **l;

                        l = va_arg(ap, var_na**);
                        STRV_FOREACH(j, l)
                                གལ་ཏེ (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                གལ་ཏེ (!t[i])
                        ཕྱིར NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        ཕྱིར TAKE_PTR(t);
}

var_na** strv_env_unset(var_na **l, const var_na *p) {
        assert(p);

        གལ་ཏེ (!l)
                ཕྱིར NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        var_na **f, **t;
        ཆེད་དུ (f = t = l; *f; f++) {
                གལ་ཏེ (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ཕྱིར l;
}

var_na** strv_env_unset_many_internal(var_na **l, ...) {
        གལ་ཏེ (!l)
                ཕྱིར NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        var_na **f, **t;
        ཆེད་དུ (f = t = l; *f; f++) {
                bool found = false;
                const var_na *p;
                va_list ap;

                va_start(ap, l);

                སྐབས ((p = va_arg(ap, const var_na*)))
                        གལ་ཏེ (env_match(*f, p)) {
                                found = true;
                                bha_nga;
                        }

                va_end(ap);

                གལ་ཏེ (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ཕྱིར l;
}

ཧྲིལ strv_env_replace_consume(var_na ***l, var_na *p) {
        const var_na *t, *name;
        ཧྲིལ r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        གལ་ཏེ (!t) {
                free(p);
                ཕྱིར -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                གལ་ཏེ (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ཕྱིར 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར 1;
}

ཧྲིལ strv_env_replace_strdup(var_na ***l, const var_na *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        var_na *p = strdup(assignment);
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        ཕྱིར strv_env_replace_consume(l, p);
}

ཧྲིལ strv_env_replace_strdup_passthrough(var_na ***l, const var_na *assignment) {
        var_na *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        གལ་ཏེ (strchr(assignment, '=')) {
                གལ་ཏེ (!env_assignment_is_valid(assignment))
                        ཕྱིར -EINVAL;

                p = strdup(assignment);
        } དེ་མིན {
                གལ་ཏེ (!env_name_is_valid(assignment))
                        ཕྱིར -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        ཕྱིར strv_env_replace_consume(l, p);
}

ཧྲིལ strv_env_assign(var_na ***l, const var_na *key, const var_na *value) {
        assert(l);

        གལ་ཏེ (!env_name_is_valid(key))
                ཕྱིར -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        གལ་ཏེ (!value) {
                strv_env_unset(*l, key);
                ཕྱིར 0;
        }

        var_na *p = strjoin(key, "=", value);
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        ཕྱིར strv_env_replace_consume(l, p);
}

ཧྲིལ strv_env_assignf(var_na ***l, const var_na *key, const var_na *valuef, ...) {
        ཧྲིལ r;

        assert(l);
        assert(key);

        གལ་ཏེ (!env_name_is_valid(key))
                ཕྱིར -EINVAL;

        གལ་ཏེ (!valuef) {
                strv_env_unset(*l, key);
                ཕྱིར 0;
        }

        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        གལ་ཏེ (r < 0)
                ཕྱིར -ENOMEM;

        var_na *p = strjoin(key, "=", value);
        གལ་ཏེ (!p)
                ཕྱིར -ENOMEM;

        ཕྱིར strv_env_replace_consume(l, p);
}

ཧྲིལ _strv_env_assign_many(var_na ***l, ...) {
        va_list ap;
        ཧྲིལ r;

        assert(l);

        va_start(ap, l);
        ཆེད་དུ (;;) {
                const var_na *key, *value;

                key = va_arg(ap, const var_na *);
                གལ་ཏེ (!key)
                        bha_nga;

                གལ་ཏེ (!env_name_is_valid(key)) {
                        va_end(ap);
                        ཕྱིར -EINVAL;
                }

                value = va_arg(ap, const var_na *);
                གལ་ཏེ (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                var_na *p = strjoin(key, "=", value);
                གལ་ཏེ (!p) {
                        va_end(ap);
                        ཕྱིར -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                གལ་ཏེ (r < 0) {
                        va_end(ap);
                        ཕྱིར r;
                }
        }
        va_end(ap);

        ཕྱིར 0;
}

var_na* strv_env_get_n(var_na * const *l, const var_na *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        གལ་ཏེ (k == SIZE_MAX)
                k = strlen(name);
        གལ་ཏེ (k <= 0)
                ཕྱིར NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                གལ་ཏེ (strneq(*i, name, k) && (*i)[k] == '=')
                        ཕྱིར (var_na*) *i + k + 1;

        གལ་ཏེ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const var_na *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                གལ་ཏེ (k > sc_arg_max() - 2)
                        ཕྱིར NULL;

                t = strndupa_safe(name, k);
                ཕྱིར secure_getenv(t);
        };

        ཕྱིར NULL;
}

var_na* strv_env_pairs_get(var_na **l, const var_na *name) {
        var_na *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                གལ་ཏེ (streq(*key, name))
                        result = *value;

        ཕྱིར result;
}

ཧྲིལ strv_env_get_merged(var_na **l, var_na ***ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        size_t n = 0;
        ཧྲིལ r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                var_na *s;

                s = strjoin(*key, "=", *value);
                གལ་ཏེ (!s)
                        ཕྱིར -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        *ret = TAKE_PTR(v);
        ཕྱིར 0;
}

var_na** strv_env_clean_with_callback(var_na **l, void (*invalid_callback)(const var_na *p, void *userdata), void *userdata) {
        ཧྲིལ k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                གལ་ཏེ (!env_assignment_is_valid(*p)) {
                        གལ་ཏེ (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        གལ་ཏེ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                bha_nga;
                        }

                གལ་ཏེ (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        གལ་ཏེ (l)
                l[k] = NULL;

        ཕྱིར l;
}

static ཧྲིལ strv_extend_with_length(var_na ***l, const var_na *s, size_t n) {
        var_na *c;

        c = strndup(s, n);
        གལ་ཏེ (!c)
                ཕྱིར -ENOMEM;

        ཕྱིར strv_consume(l, c);
}

static ཧྲིལ strv_env_get_n_validated(
                var_na **env,
                const var_na *name,
                size_t l,
                ReplaceEnvFlags flags,
                var_na **ret,              /* points into the env block! do not free! */
                var_na ***unset_variables, /* updated in place */
                var_na ***bad_variables) { /* ditto */

        var_na *e;
        ཧྲིལ r;

        assert(l == 0 || name);
        assert(ret);

        གལ་ཏེ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                གལ་ཏེ (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;
                }
        } དེ་མིན {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                གལ་ཏེ (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;
                }
        }

        *ret = e;
        ཕྱིར !!e;
}

ཧྲིལ replace_env_full(
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
        ཧྲིལ nest = 0, r;

        assert(format);

        གལ་ཏེ (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        ཆེད་དུ (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                pak_sa WORD:
                        གལ་ཏེ (*e == '$')
                                state = CURLY;
                        bha_nga;

                pak_sa CURLY:
                        གལ་ཏེ (*e == '{') {
                                གལ་ཏེ (!strextendn(&s, word, e-word-1))
                                        ཕྱིར -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } དེ་མིན གལ་ཏེ (*e == '$') {
                                གལ་ཏེ (!strextendn(&s, word, e-word))
                                        ཕྱིར -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } དེ་མིན གལ་ཏེ (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                གལ་ཏེ (!strextendn(&s, word, e-word-1))
                                        ཕྱིར -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } དེ་མིན
                                state = WORD;
                        bha_nga;

                pak_sa VARIABLE:
                        གལ་ཏེ (*e == '}') {
                                var_na *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;

                                གལ་ཏེ (!strextend(&s, t))
                                        ཕྱིར -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } དེ་མིན གལ་ཏེ (*e == ':') {
                                གལ་ཏེ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } དེ་མིན
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        bha_nga;

                pak_sa TEST:
                        གལ་ཏེ (*e == '-')
                                state = DEFAULT_VALUE;
                        དེ་མིན གལ་ཏེ (*e == '+')
                                state = ALTERNATE_VALUE;
                        དེ་མིན {
                                state = WORD;
                                bha_nga;
                        }

                        test_value = e+1;
                        bha_nga;

                pak_sa DEFAULT_VALUE: /* fall through */
                pak_sa ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        གལ་ཏེ (*e == '{') {
                                nest++;
                                bha_nga;
                        }

                        གལ་ཏེ (*e != '}')
                                bha_nga;

                        nest--;
                        གལ་ཏེ (nest == 0) {
                                _cleanup_strv_free_ var_na **u = NULL, **b = NULL;
                                _cleanup_free_ var_na *v = NULL;
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;

                                གལ་ཏེ (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        གལ་ཏེ (r < 0)
                                                ཕྱིར r;

                                        t = v;
                                } དེ་མིན གལ་ཏེ (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        གལ་ཏེ (r < 0)
                                                ཕྱིར r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;

                                གལ་ཏེ (!strextend(&s, t))
                                        ཕྱིར -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        bha_nga;

                pak_sa VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        གལ་ཏེ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                གལ་ཏེ (r < 0)
                                        ཕྱིར r;

                                གལ་ཏེ (!strextend(&s, t))
                                        ཕྱིར -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        bha_nga;
                }

        གལ་ཏེ (state == VARIABLE_RAW) {
                var_na *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                གལ་ཏེ (!strextend(&s, t))
                        ཕྱིར -ENOMEM;

        } དེ་མིན གལ་ཏེ (!strextendn(&s, word, e-word))
                ཕྱིར -ENOMEM;

        གལ་ཏེ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        གལ་ཏེ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(s);

        ཕྱིར 0;
}

ཧྲིལ replace_env_argv(
                var_na **argv,
                var_na **env,
                var_na ***ret,
                var_na ***ret_unset_variables,
                var_na ***ret_bad_variables) {

        _cleanup_strv_free_ var_na **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        ཧྲིལ r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(var_na*, l+1);
        གལ་ཏེ (!n)
                ཕྱིར -ENOMEM;

        STRV_FOREACH(i, argv) {
                const var_na *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                གལ་ཏེ (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ var_na **m = NULL;
                        const var_na *name = word + 1;
                        var_na *e, **w;
                        size_t q;

                        གལ་ཏེ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                གལ་ཏེ (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                དེ་མིན གལ་ཏེ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                དེ་མིན
                                        r = 0;
                        } དེ་མིན གལ་ཏེ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        དེ་མིན
                                r = 0;
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, parimaa_na(var_na*));
                        གལ་ཏེ (!w)
                                ཕྱིར -ENOMEM;

                        n = w;
                        གལ་ཏེ (m) {
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
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        གལ་ཏེ (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        གལ་ཏེ (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ཕྱིར 0;
}

ཧྲིལ getenv_bool(const var_na *p) {
        const var_na *e;

        e = getenv(p);
        གལ་ཏེ (!e)
                ཕྱིར -ENXIO;

        ཕྱིར parse_boolean(e);
}

ཧྲིལ secure_getenv_bool(const var_na *p) {
        const var_na *e;

        e = secure_getenv(p);
        གལ་ཏེ (!e)
                ཕྱིར -ENXIO;

        ཕྱིར parse_boolean(e);
}

ཧྲིལ secure_getenv_uint64(const var_na *p, uint64_t *ret) {
        const var_na *e;

        assert(p);

        e = secure_getenv(p);
        གལ་ཏེ (!e)
                ཕྱིར -ENXIO;

        ཕྱིར safe_atou64(e, ret);
}

ཧྲིལ set_unset_env(const var_na *name, const var_na *value, bool overwrite) {
        assert(name);

        གལ་ཏེ (value)
                ཕྱིར RET_NERRNO(setenv(name, value, overwrite));

        ཕྱིར RET_NERRNO(unsetenv(name));
}

ཧྲིལ putenv_dup(const var_na *assignment, bool override) {
        const var_na *e, *n;

        e = strchr(assignment, '=');
        གལ་ཏེ (!e)
                ཕྱིར -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ཕྱིར RET_NERRNO(setenv(n, e + 1, override));
}

ཧྲིལ setenv_systemd_exec_pid(bool update_only) {
        const var_na *e;
        ཧྲིལ r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        གལ་ཏེ (!e && update_only)
                ཕྱིར 0;

        གལ་ཏེ (streq_ptr(e, "*"))
                ཕྱིར 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར 1;
}

ཧྲིལ setenv_systemd_log_level(void) {
        _cleanup_free_ var_na *val = NULL;
        ཧྲིལ r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ཧྲིལ getenv_path_list(const var_na *name, var_na ***ret_paths) {
        _cleanup_strv_free_ var_na **l = NULL;
        const var_na *e;
        ཧྲིལ r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        གལ་ཏེ (!e)
                ཕྱིར -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        གལ་ཏེ (r < 0)
                ཕྱིར log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                གལ་ཏེ (!path_is_absolute(*p))
                        ཕྱིར log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                གལ་ཏེ (!path_is_normalized(*p))
                        ཕྱིར log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                གལ་ཏེ (path_equal(*p, "/"))
                        ཕྱིར log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        གལ་ཏེ (strv_isempty(l))
                ཕྱིར log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ཕྱིར 1;
}

ཧྲིལ getenv_steal_erase(const var_na *name, var_na **ret) {
        _cleanup_(erase_and_freep) var_na *a = NULL;
        var_na *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        གལ་ཏེ (!e) {
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར 0;
        }

        གལ་ཏེ (ret) {
                a = strdup(e);
                གལ་ཏེ (!a)
                        ཕྱིར -ENOMEM;
        }

        string_erase(e);

        གལ་ཏེ (unsetenv(name) < 0)
                ཕྱིར -errno;

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(a);

        ཕྱིར 1;
}

ཧྲིལ setenvf(const var_na *name, bool overwrite, const var_na *valuef, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        ཧྲིལ r;

        assert(name);

        གལ་ཏེ (!valuef)
                ཕྱིར RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        གལ་ཏེ (r < 0)
                ཕྱིར -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        གལ་ཏེ (streq_ptr(getenv(name), value))
                ཕྱིར 0;

        ཕྱིར RET_NERRNO(setenv(name, value, overwrite));
}
