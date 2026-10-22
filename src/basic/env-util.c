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
        ပြန်ပေး (size_t) l;
}

static bool env_name_is_valid_n(const var_na *e, size_t n) {

        အကယ်၍ (n == SIZE_MAX)
                n = strlen_ptr(e);

        အကယ်၍ (n == 0)
                ပြန်ပေး false;

        assert(e);

        အကယ်၍ (ascii_isdigit(e[0]))
                ပြန်ပေး false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        အကယ်၍ (_unlikely_(n > sc_arg_max() - 2))
                ပြန်ပေး false;

        အတွက် (const var_na *p = e; p < e + n; p++)
                အကယ်၍ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

bool env_name_is_valid(const var_na *e) {
        ပြန်ပေး env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const var_na *e) {
        အကယ်၍ (!e)
                ပြန်ပေး false;

        အကယ်၍ (!utf8_is_valid(e))
                ပြန်ပေး false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        အကယ်၍ (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool env_assignment_is_valid(const var_na *e) {
        const var_na *eq;

        assert(e);

        eq = strchr(e, '=');
        အကယ်၍ (!eq)
                ပြန်ပေး false;

        အကယ်၍ (!env_name_is_valid_n(e, eq - e))
                ပြန်ပေး false;

        အကယ်၍ (!env_value_is_valid(eq + 1))
                ပြန်ပေး false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        အကယ်၍ (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool strv_env_is_valid(var_na * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                အကယ်၍ (!env_assignment_is_valid(*p))
                        ပြန်ပေး false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        အကယ်၍ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

bool strv_env_name_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                အကယ်၍ (!env_name_is_valid(*p))
                        ပြန်ပေး false;

                အကယ်၍ (strv_contains(p + 1, *p))
                        ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

bool strv_env_name_or_assignment_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                အကယ်၍ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ပြန်ပေး false;

                အကယ်၍ (strv_contains(p + 1, *p))
                        ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

static ကိန်းပြည့် env_append(var_na **e, var_na ***k, var_na **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        အကယ်၍ (!a)
                ပြန်ပေး 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        အတွက် (; *a; a++) {
                var_na **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                အကယ်၍ ((*a)[n] == '=')
                        n++;

                အတွက် (j = e; j < *k; j++)
                        အကယ်၍ (strneq(*j, *a, n))
                                bha_nga;

                c = strdup(*a);
                အကယ်၍ (!c)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } မဟုတ်ပါက
                        free_and_replace(*j, c); /* Override existing item */
        }

        ပြန်ပေး 0;
}

var_na** _strv_env_merge(var_na **first, ...) {
        _cleanup_strv_free_ var_na **merged = NULL;
        var_na **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        အတွက် (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                အကယ်၍ (l == POINTER_MAX)
                        bha_nga;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(var_na*, n + 1);
        အကယ်၍ (!merged)
                ပြန်ပေး NULL;
        merged[0] = NULL;

        အကယ်၍ (env_append(merged, &k, first) < 0)
                ပြန်ပေး NULL;

        va_start(ap, first);
        အတွက် (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                အကယ်၍ (l == POINTER_MAX)
                        bha_nga;

                အကယ်၍ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ပြန်ပေး NULL;
                }
        }
        va_end(ap);

        ပြန်ပေး TAKE_PTR(merged);
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

        အကယ်၍ (streq(t, pattern))
                ပြန်ပေး true;

        အကယ်၍ (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ပြန်ပေး t && *t == '=';
        }

        ပြန်ပေး false;
}

static bool env_entry_has_name(const var_na *entry, const var_na *name) {
        const var_na *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        အကယ်၍ (!t)
                ပြန်ပေး false;

        ပြန်ပေး *t == '=';
}

var_na** strv_env_delete(var_na **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ var_na **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(var_na*, n+1);
        အကယ်၍ (!t)
                ပြန်ပေး NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                အတွက် (size_t v = 0; v < n_lists; v++) {
                        var_na **l;

                        l = va_arg(ap, var_na**);
                        STRV_FOREACH(j, l)
                                အကယ်၍ (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                အကယ်၍ (!t[i])
                        ပြန်ပေး NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        ပြန်ပေး TAKE_PTR(t);
}

var_na** strv_env_unset(var_na **l, const var_na *p) {
        assert(p);

        အကယ်၍ (!l)
                ပြန်ပေး NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        var_na **f, **t;
        အတွက် (f = t = l; *f; f++) {
                အကယ်၍ (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ပြန်ပေး l;
}

var_na** strv_env_unset_many_internal(var_na **l, ...) {
        အကယ်၍ (!l)
                ပြန်ပေး NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        var_na **f, **t;
        အတွက် (f = t = l; *f; f++) {
                bool found = false;
                const var_na *p;
                va_list ap;

                va_start(ap, l);

                နေစဉ် ((p = va_arg(ap, const var_na*)))
                        အကယ်၍ (env_match(*f, p)) {
                                found = true;
                                bha_nga;
                        }

                va_end(ap);

                အကယ်၍ (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        ပြန်ပေး l;
}

ကိန်းပြည့် strv_env_replace_consume(var_na ***l, var_na *p) {
        const var_na *t, *name;
        ကိန်းပြည့် r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        အကယ်၍ (!t) {
                free(p);
                ပြန်ပေး -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                အကယ်၍ (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ပြန်ပေး 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1;
}

ကိန်းပြည့် strv_env_replace_strdup(var_na ***l, const var_na *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        var_na *p = strdup(assignment);
        အကယ်၍ (!p)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_env_replace_consume(l, p);
}

ကိန်းပြည့် strv_env_replace_strdup_passthrough(var_na ***l, const var_na *assignment) {
        var_na *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        အကယ်၍ (strchr(assignment, '=')) {
                အကယ်၍ (!env_assignment_is_valid(assignment))
                        ပြန်ပေး -EINVAL;

                p = strdup(assignment);
        } မဟုတ်ပါက {
                အကယ်၍ (!env_name_is_valid(assignment))
                        ပြန်ပေး -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        အကယ်၍ (!p)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_env_replace_consume(l, p);
}

ကိန်းပြည့် strv_env_assign(var_na ***l, const var_na *key, const var_na *value) {
        assert(l);

        အကယ်၍ (!env_name_is_valid(key))
                ပြန်ပေး -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        အကယ်၍ (!value) {
                strv_env_unset(*l, key);
                ပြန်ပေး 0;
        }

        var_na *p = strjoin(key, "=", value);
        အကယ်၍ (!p)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_env_replace_consume(l, p);
}

ကိန်းပြည့် strv_env_assignf(var_na ***l, const var_na *key, const var_na *valuef, ...) {
        ကိန်းပြည့် r;

        assert(l);
        assert(key);

        အကယ်၍ (!env_name_is_valid(key))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!valuef) {
                strv_env_unset(*l, key);
                ပြန်ပေး 0;
        }

        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        အကယ်၍ (r < 0)
                ပြန်ပေး -ENOMEM;

        var_na *p = strjoin(key, "=", value);
        အကယ်၍ (!p)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_env_replace_consume(l, p);
}

ကိန်းပြည့် _strv_env_assign_many(var_na ***l, ...) {
        va_list ap;
        ကိန်းပြည့် r;

        assert(l);

        va_start(ap, l);
        အတွက် (;;) {
                const var_na *key, *value;

                key = va_arg(ap, const var_na *);
                အကယ်၍ (!key)
                        bha_nga;

                အကယ်၍ (!env_name_is_valid(key)) {
                        va_end(ap);
                        ပြန်ပေး -EINVAL;
                }

                value = va_arg(ap, const var_na *);
                အကယ်၍ (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                var_na *p = strjoin(key, "=", value);
                အကယ်၍ (!p) {
                        va_end(ap);
                        ပြန်ပေး -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                အကယ်၍ (r < 0) {
                        va_end(ap);
                        ပြန်ပေး r;
                }
        }
        va_end(ap);

        ပြန်ပေး 0;
}

var_na* strv_env_get_n(var_na * const *l, const var_na *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        အကယ်၍ (k == SIZE_MAX)
                k = strlen(name);
        အကယ်၍ (k <= 0)
                ပြန်ပေး NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                အကယ်၍ (strneq(*i, name, k) && (*i)[k] == '=')
                        ပြန်ပေး (var_na*) *i + k + 1;

        အကယ်၍ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const var_na *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                အကယ်၍ (k > sc_arg_max() - 2)
                        ပြန်ပေး NULL;

                t = strndupa_safe(name, k);
                ပြန်ပေး secure_getenv(t);
        };

        ပြန်ပေး NULL;
}

var_na* strv_env_pairs_get(var_na **l, const var_na *name) {
        var_na *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                အကယ်၍ (streq(*key, name))
                        result = *value;

        ပြန်ပေး result;
}

ကိန်းပြည့် strv_env_get_merged(var_na **l, var_na ***ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        size_t n = 0;
        ကိန်းပြည့် r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                var_na *s;

                s = strjoin(*key, "=", *value);
                အကယ်၍ (!s)
                        ပြန်ပေး -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        *ret = TAKE_PTR(v);
        ပြန်ပေး 0;
}

var_na** strv_env_clean_with_callback(var_na **l, void (*invalid_callback)(const var_na *p, void *userdata), void *userdata) {
        ကိန်းပြည့် k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                အကယ်၍ (!env_assignment_is_valid(*p)) {
                        အကယ်၍ (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        အကယ်၍ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                bha_nga;
                        }

                အကယ်၍ (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        အကယ်၍ (l)
                l[k] = NULL;

        ပြန်ပေး l;
}

static ကိန်းပြည့် strv_extend_with_length(var_na ***l, const var_na *s, size_t n) {
        var_na *c;

        c = strndup(s, n);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး strv_consume(l, c);
}

static ကိန်းပြည့် strv_env_get_n_validated(
                var_na **env,
                const var_na *name,
                size_t l,
                ReplaceEnvFlags flags,
                var_na **ret,              /* points into the env block! do not free! */
                var_na ***unset_variables, /* updated in place */
                var_na ***bad_variables) { /* ditto */

        var_na *e;
        ကိန်းပြည့် r;

        assert(l == 0 || name);
        assert(ret);

        အကယ်၍ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                အကယ်၍ (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                }
        } မဟုတ်ပါက {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                အကယ်၍ (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                }
        }

        *ret = e;
        ပြန်ပေး !!e;
}

ကိန်းပြည့် replace_env_full(
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
        ကိန်းပြည့် nest = 0, r;

        assert(format);

        အကယ်၍ (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        အတွက် (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                pak_sa WORD:
                        အကယ်၍ (*e == '$')
                                state = CURLY;
                        bha_nga;

                pak_sa CURLY:
                        အကယ်၍ (*e == '{') {
                                အကယ်၍ (!strextendn(&s, word, e-word-1))
                                        ပြန်ပေး -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } မဟုတ်ပါက အကယ်၍ (*e == '$') {
                                အကယ်၍ (!strextendn(&s, word, e-word))
                                        ပြန်ပေး -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } မဟုတ်ပါက အကယ်၍ (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                အကယ်၍ (!strextendn(&s, word, e-word-1))
                                        ပြန်ပေး -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } မဟုတ်ပါက
                                state = WORD;
                        bha_nga;

                pak_sa VARIABLE:
                        အကယ်၍ (*e == '}') {
                                var_na *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                အကယ်၍ (!strextend(&s, t))
                                        ပြန်ပေး -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } မဟုတ်ပါက အကယ်၍ (*e == ':') {
                                အကယ်၍ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } မဟုတ်ပါက
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        bha_nga;

                pak_sa TEST:
                        အကယ်၍ (*e == '-')
                                state = DEFAULT_VALUE;
                        မဟုတ်ပါက အကယ်၍ (*e == '+')
                                state = ALTERNATE_VALUE;
                        မဟုတ်ပါက {
                                state = WORD;
                                bha_nga;
                        }

                        test_value = e+1;
                        bha_nga;

                pak_sa DEFAULT_VALUE: /* fall through */
                pak_sa ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        အကယ်၍ (*e == '{') {
                                nest++;
                                bha_nga;
                        }

                        အကယ်၍ (*e != '}')
                                bha_nga;

                        nest--;
                        အကယ်၍ (nest == 0) {
                                _cleanup_strv_free_ var_na **u = NULL, **b = NULL;
                                _cleanup_free_ var_na *v = NULL;
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                အကယ်၍ (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        အကယ်၍ (r < 0)
                                                ပြန်ပေး r;

                                        t = v;
                                } မဟုတ်ပါက အကယ်၍ (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        အကယ်၍ (r < 0)
                                                ပြန်ပေး r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                အကယ်၍ (!strextend(&s, t))
                                        ပြန်ပေး -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        bha_nga;

                pak_sa VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        အကယ်၍ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                အကယ်၍ (!strextend(&s, t))
                                        ပြန်ပေး -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        bha_nga;
                }

        အကယ်၍ (state == VARIABLE_RAW) {
                var_na *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (!strextend(&s, t))
                        ပြန်ပေး -ENOMEM;

        } မဟုတ်ပါက အကယ်၍ (!strextendn(&s, word, e-word))
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        အကယ်၍ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        အကယ်၍ (ret)
                *ret = TAKE_PTR(s);

        ပြန်ပေး 0;
}

ကိန်းပြည့် replace_env_argv(
                var_na **argv,
                var_na **env,
                var_na ***ret,
                var_na ***ret_unset_variables,
                var_na ***ret_bad_variables) {

        _cleanup_strv_free_ var_na **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        ကိန်းပြည့် r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(var_na*, l+1);
        အကယ်၍ (!n)
                ပြန်ပေး -ENOMEM;

        STRV_FOREACH(i, argv) {
                const var_na *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                အကယ်၍ (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ var_na **m = NULL;
                        const var_na *name = word + 1;
                        var_na *e, **w;
                        size_t q;

                        အကယ်၍ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                အကယ်၍ (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                မဟုတ်ပါက အကယ်၍ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                မဟုတ်ပါက
                                        r = 0;
                        } မဟုတ်ပါက အကယ်၍ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        မဟုတ်ပါက
                                r = 0;
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, parimaa_na(var_na*));
                        အကယ်၍ (!w)
                                ပြန်ပေး -ENOMEM;

                        n = w;
                        အကယ်၍ (m) {
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
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        အကယ်၍ (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ပြန်ပေး 0;
}

ကိန်းပြည့် getenv_bool(const var_na *p) {
        const var_na *e;

        e = getenv(p);
        အကယ်၍ (!e)
                ပြန်ပေး -ENXIO;

        ပြန်ပေး parse_boolean(e);
}

ကိန်းပြည့် secure_getenv_bool(const var_na *p) {
        const var_na *e;

        e = secure_getenv(p);
        အကယ်၍ (!e)
                ပြန်ပေး -ENXIO;

        ပြန်ပေး parse_boolean(e);
}

ကိန်းပြည့် secure_getenv_uint64(const var_na *p, uint64_t *ret) {
        const var_na *e;

        assert(p);

        e = secure_getenv(p);
        အကယ်၍ (!e)
                ပြန်ပေး -ENXIO;

        ပြန်ပေး safe_atou64(e, ret);
}

ကိန်းပြည့် set_unset_env(const var_na *name, const var_na *value, bool overwrite) {
        assert(name);

        အကယ်၍ (value)
                ပြန်ပေး RET_NERRNO(setenv(name, value, overwrite));

        ပြန်ပေး RET_NERRNO(unsetenv(name));
}

ကိန်းပြည့် putenv_dup(const var_na *assignment, bool override) {
        const var_na *e, *n;

        e = strchr(assignment, '=');
        အကယ်၍ (!e)
                ပြန်ပေး -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ပြန်ပေး RET_NERRNO(setenv(n, e + 1, override));
}

ကိန်းပြည့် setenv_systemd_exec_pid(bool update_only) {
        const var_na *e;
        ကိန်းပြည့် r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        အကယ်၍ (!e && update_only)
                ပြန်ပေး 0;

        အကယ်၍ (streq_ptr(e, "*"))
                ပြန်ပေး 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1;
}

ကိန်းပြည့် setenv_systemd_log_level(void) {
        _cleanup_free_ var_na *val = NULL;
        ကိန်းပြည့် r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ကိန်းပြည့် getenv_path_list(const var_na *name, var_na ***ret_paths) {
        _cleanup_strv_free_ var_na **l = NULL;
        const var_na *e;
        ကိန်းပြည့် r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        အကယ်၍ (!e)
                ပြန်ပေး -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                အကယ်၍ (!path_is_absolute(*p))
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                အကယ်၍ (!path_is_normalized(*p))
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                အကယ်၍ (path_equal(*p, "/"))
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        အကယ်၍ (strv_isempty(l))
                ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ပြန်ပေး 1;
}

ကိန်းပြည့် getenv_steal_erase(const var_na *name, var_na **ret) {
        _cleanup_(erase_and_freep) var_na *a = NULL;
        var_na *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        အကယ်၍ (!e) {
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး 0;
        }

        အကယ်၍ (ret) {
                a = strdup(e);
                အကယ်၍ (!a)
                        ပြန်ပေး -ENOMEM;
        }

        string_erase(e);

        အကယ်၍ (unsetenv(name) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ret)
                *ret = TAKE_PTR(a);

        ပြန်ပေး 1;
}

ကိန်းပြည့် setenvf(const var_na *name, bool overwrite, const var_na *valuef, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        ကိန်းပြည့် r;

        assert(name);

        အကယ်၍ (!valuef)
                ပြန်ပေး RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        အကယ်၍ (streq_ptr(getenv(name), value))
                ပြန်ပေး 0;

        ပြန်ပေး RET_NERRNO(setenv(name, value, overwrite));
}
