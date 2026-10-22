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
        คืน (size_t) l;
}

static bool env_name_is_valid_n(const var_na *e, size_t n) {

        ถ้า (n == SIZE_MAX)
                n = strlen_ptr(e);

        ถ้า (n == 0)
                คืน false;

        assert(e);

        ถ้า (ascii_isdigit(e[0]))
                คืน false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ถ้า (_unlikely_(n > sc_arg_max() - 2))
                คืน false;

        สำหรับ (const var_na *p = e; p < e + n; p++)
                ถ้า (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        คืน false;

        คืน true;
}

bool env_name_is_valid(const var_na *e) {
        คืน env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const var_na *e) {
        ถ้า (!e)
                คืน false;

        ถ้า (!utf8_is_valid(e))
                คืน false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ถ้า (_unlikely_(strlen(e) > sc_arg_max() - 3))
                คืน false;

        คืน true;
}

bool env_assignment_is_valid(const var_na *e) {
        const var_na *eq;

        assert(e);

        eq = strchr(e, '=');
        ถ้า (!eq)
                คืน false;

        ถ้า (!env_name_is_valid_n(e, eq - e))
                คืน false;

        ถ้า (!env_value_is_valid(eq + 1))
                คืน false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ถ้า (_unlikely_(strlen(e) > sc_arg_max() - 1))
                คืน false;

        คืน true;
}

bool strv_env_is_valid(var_na * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ถ้า (!env_assignment_is_valid(*p))
                        คืน false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ถ้า (strneq(*p, *q, k) && (*q)[k] == '=')
                                คืน false;
        }

        คืน true;
}

bool strv_env_name_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                ถ้า (!env_name_is_valid(*p))
                        คืน false;

                ถ้า (strv_contains(p + 1, *p))
                        คืน false;
        }

        คืน true;
}

bool strv_env_name_or_assignment_is_valid(var_na * const *l) {
        STRV_FOREACH(p, l) {
                ถ้า (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        คืน false;

                ถ้า (strv_contains(p + 1, *p))
                        คืน false;
        }

        คืน true;
}

static จำนวนเต็ม env_append(var_na **e, var_na ***k, var_na **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        ถ้า (!a)
                คืน 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        สำหรับ (; *a; a++) {
                var_na **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ถ้า ((*a)[n] == '=')
                        n++;

                สำหรับ (j = e; j < *k; j++)
                        ถ้า (strneq(*j, *a, n))
                                bha_nga;

                c = strdup(*a);
                ถ้า (!c)
                        คืน -ENOMEM;

                ถ้า (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } มิฉะนั้น
                        free_and_replace(*j, c); /* Override existing item */
        }

        คืน 0;
}

var_na** _strv_env_merge(var_na **first, ...) {
        _cleanup_strv_free_ var_na **merged = NULL;
        var_na **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        สำหรับ (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                ถ้า (l == POINTER_MAX)
                        bha_nga;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(var_na*, n + 1);
        ถ้า (!merged)
                คืน NULL;
        merged[0] = NULL;

        ถ้า (env_append(merged, &k, first) < 0)
                คืน NULL;

        va_start(ap, first);
        สำหรับ (;;) {
                var_na **l;

                l = va_arg(ap, var_na**);
                ถ้า (l == POINTER_MAX)
                        bha_nga;

                ถ้า (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        คืน NULL;
                }
        }
        va_end(ap);

        คืน TAKE_PTR(merged);
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

        ถ้า (streq(t, pattern))
                คืน true;

        ถ้า (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                คืน t && *t == '=';
        }

        คืน false;
}

static bool env_entry_has_name(const var_na *entry, const var_na *name) {
        const var_na *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        ถ้า (!t)
                คืน false;

        คืน *t == '=';
}

var_na** strv_env_delete(var_na **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ var_na **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(var_na*, n+1);
        ถ้า (!t)
                คืน NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                สำหรับ (size_t v = 0; v < n_lists; v++) {
                        var_na **l;

                        l = va_arg(ap, var_na**);
                        STRV_FOREACH(j, l)
                                ถ้า (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ถ้า (!t[i])
                        คืน NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        คืน TAKE_PTR(t);
}

var_na** strv_env_unset(var_na **l, const var_na *p) {
        assert(p);

        ถ้า (!l)
                คืน NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        var_na **f, **t;
        สำหรับ (f = t = l; *f; f++) {
                ถ้า (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        คืน l;
}

var_na** strv_env_unset_many_internal(var_na **l, ...) {
        ถ้า (!l)
                คืน NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        var_na **f, **t;
        สำหรับ (f = t = l; *f; f++) {
                bool found = false;
                const var_na *p;
                va_list ap;

                va_start(ap, l);

                ขณะ ((p = va_arg(ap, const var_na*)))
                        ถ้า (env_match(*f, p)) {
                                found = true;
                                bha_nga;
                        }

                va_end(ap);

                ถ้า (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        คืน l;
}

จำนวนเต็ม strv_env_replace_consume(var_na ***l, var_na *p) {
        const var_na *t, *name;
        จำนวนเต็ม r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ถ้า (!t) {
                free(p);
                คืน -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                ถ้า (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        คืน 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ถ้า (r < 0)
                คืน r;

        คืน 1;
}

จำนวนเต็ม strv_env_replace_strdup(var_na ***l, const var_na *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        var_na *p = strdup(assignment);
        ถ้า (!p)
                คืน -ENOMEM;

        คืน strv_env_replace_consume(l, p);
}

จำนวนเต็ม strv_env_replace_strdup_passthrough(var_na ***l, const var_na *assignment) {
        var_na *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        ถ้า (strchr(assignment, '=')) {
                ถ้า (!env_assignment_is_valid(assignment))
                        คืน -EINVAL;

                p = strdup(assignment);
        } มิฉะนั้น {
                ถ้า (!env_name_is_valid(assignment))
                        คืน -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ถ้า (!p)
                คืน -ENOMEM;

        คืน strv_env_replace_consume(l, p);
}

จำนวนเต็ม strv_env_assign(var_na ***l, const var_na *key, const var_na *value) {
        assert(l);

        ถ้า (!env_name_is_valid(key))
                คืน -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ถ้า (!value) {
                strv_env_unset(*l, key);
                คืน 0;
        }

        var_na *p = strjoin(key, "=", value);
        ถ้า (!p)
                คืน -ENOMEM;

        คืน strv_env_replace_consume(l, p);
}

จำนวนเต็ม strv_env_assignf(var_na ***l, const var_na *key, const var_na *valuef, ...) {
        จำนวนเต็ม r;

        assert(l);
        assert(key);

        ถ้า (!env_name_is_valid(key))
                คืน -EINVAL;

        ถ้า (!valuef) {
                strv_env_unset(*l, key);
                คืน 0;
        }

        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        ถ้า (r < 0)
                คืน -ENOMEM;

        var_na *p = strjoin(key, "=", value);
        ถ้า (!p)
                คืน -ENOMEM;

        คืน strv_env_replace_consume(l, p);
}

จำนวนเต็ม _strv_env_assign_many(var_na ***l, ...) {
        va_list ap;
        จำนวนเต็ม r;

        assert(l);

        va_start(ap, l);
        สำหรับ (;;) {
                const var_na *key, *value;

                key = va_arg(ap, const var_na *);
                ถ้า (!key)
                        bha_nga;

                ถ้า (!env_name_is_valid(key)) {
                        va_end(ap);
                        คืน -EINVAL;
                }

                value = va_arg(ap, const var_na *);
                ถ้า (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                var_na *p = strjoin(key, "=", value);
                ถ้า (!p) {
                        va_end(ap);
                        คืน -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ถ้า (r < 0) {
                        va_end(ap);
                        คืน r;
                }
        }
        va_end(ap);

        คืน 0;
}

var_na* strv_env_get_n(var_na * const *l, const var_na *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        ถ้า (k == SIZE_MAX)
                k = strlen(name);
        ถ้า (k <= 0)
                คืน NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                ถ้า (strneq(*i, name, k) && (*i)[k] == '=')
                        คืน (var_na*) *i + k + 1;

        ถ้า (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const var_na *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ถ้า (k > sc_arg_max() - 2)
                        คืน NULL;

                t = strndupa_safe(name, k);
                คืน secure_getenv(t);
        };

        คืน NULL;
}

var_na* strv_env_pairs_get(var_na **l, const var_na *name) {
        var_na *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                ถ้า (streq(*key, name))
                        result = *value;

        คืน result;
}

จำนวนเต็ม strv_env_get_merged(var_na **l, var_na ***ret) {
        _cleanup_strv_free_ var_na **v = NULL;
        size_t n = 0;
        จำนวนเต็ม r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                var_na *s;

                s = strjoin(*key, "=", *value);
                ถ้า (!s)
                        คืน -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                ถ้า (r < 0)
                        คืน r;
        }

        *ret = TAKE_PTR(v);
        คืน 0;
}

var_na** strv_env_clean_with_callback(var_na **l, void (*invalid_callback)(const var_na *p, void *userdata), void *userdata) {
        จำนวนเต็ม k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ถ้า (!env_assignment_is_valid(*p)) {
                        ถ้า (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ถ้า (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                bha_nga;
                        }

                ถ้า (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        ถ้า (l)
                l[k] = NULL;

        คืน l;
}

static จำนวนเต็ม strv_extend_with_length(var_na ***l, const var_na *s, size_t n) {
        var_na *c;

        c = strndup(s, n);
        ถ้า (!c)
                คืน -ENOMEM;

        คืน strv_consume(l, c);
}

static จำนวนเต็ม strv_env_get_n_validated(
                var_na **env,
                const var_na *name,
                size_t l,
                ReplaceEnvFlags flags,
                var_na **ret,              /* points into the env block! do not free! */
                var_na ***unset_variables, /* updated in place */
                var_na ***bad_variables) { /* ditto */

        var_na *e;
        จำนวนเต็ม r;

        assert(l == 0 || name);
        assert(ret);

        ถ้า (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ถ้า (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        ถ้า (r < 0)
                                คืน r;
                }
        } มิฉะนั้น {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                ถ้า (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        ถ้า (r < 0)
                                คืน r;
                }
        }

        *ret = e;
        คืน !!e;
}

จำนวนเต็ม replace_env_full(
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
        จำนวนเต็ม nest = 0, r;

        assert(format);

        ถ้า (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        สำหรับ (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                pak_sa WORD:
                        ถ้า (*e == '$')
                                state = CURLY;
                        bha_nga;

                pak_sa CURLY:
                        ถ้า (*e == '{') {
                                ถ้า (!strextendn(&s, word, e-word-1))
                                        คืน -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } มิฉะนั้น ถ้า (*e == '$') {
                                ถ้า (!strextendn(&s, word, e-word))
                                        คืน -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } มิฉะนั้น ถ้า (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ถ้า (!strextendn(&s, word, e-word-1))
                                        คืน -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } มิฉะนั้น
                                state = WORD;
                        bha_nga;

                pak_sa VARIABLE:
                        ถ้า (*e == '}') {
                                var_na *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                ถ้า (r < 0)
                                        คืน r;

                                ถ้า (!strextend(&s, t))
                                        คืน -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } มิฉะนั้น ถ้า (*e == ':') {
                                ถ้า (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } มิฉะนั้น
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        bha_nga;

                pak_sa TEST:
                        ถ้า (*e == '-')
                                state = DEFAULT_VALUE;
                        มิฉะนั้น ถ้า (*e == '+')
                                state = ALTERNATE_VALUE;
                        มิฉะนั้น {
                                state = WORD;
                                bha_nga;
                        }

                        test_value = e+1;
                        bha_nga;

                pak_sa DEFAULT_VALUE: /* fall through */
                pak_sa ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ถ้า (*e == '{') {
                                nest++;
                                bha_nga;
                        }

                        ถ้า (*e != '}')
                                bha_nga;

                        nest--;
                        ถ้า (nest == 0) {
                                _cleanup_strv_free_ var_na **u = NULL, **b = NULL;
                                _cleanup_free_ var_na *v = NULL;
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                ถ้า (r < 0)
                                        คืน r;

                                ถ้า (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ถ้า (r < 0)
                                                คืน r;

                                        t = v;
                                } มิฉะนั้น ถ้า (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ถ้า (r < 0)
                                                คืน r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ถ้า (r < 0)
                                        คืน r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ถ้า (r < 0)
                                        คืน r;

                                ถ้า (!strextend(&s, t))
                                        คืน -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        bha_nga;

                pak_sa VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ถ้า (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                var_na *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                ถ้า (r < 0)
                                        คืน r;

                                ถ้า (!strextend(&s, t))
                                        คืน -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        bha_nga;
                }

        ถ้า (state == VARIABLE_RAW) {
                var_na *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (!strextend(&s, t))
                        คืน -ENOMEM;

        } มิฉะนั้น ถ้า (!strextendn(&s, word, e-word))
                คืน -ENOMEM;

        ถ้า (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ถ้า (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ถ้า (ret)
                *ret = TAKE_PTR(s);

        คืน 0;
}

จำนวนเต็ม replace_env_argv(
                var_na **argv,
                var_na **env,
                var_na ***ret,
                var_na ***ret_unset_variables,
                var_na ***ret_bad_variables) {

        _cleanup_strv_free_ var_na **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        จำนวนเต็ม r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(var_na*, l+1);
        ถ้า (!n)
                คืน -ENOMEM;

        STRV_FOREACH(i, argv) {
                const var_na *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ถ้า (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ var_na **m = NULL;
                        const var_na *name = word + 1;
                        var_na *e, **w;
                        size_t q;

                        ถ้า (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ถ้า (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                มิฉะนั้น ถ้า (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                มิฉะนั้น
                                        r = 0;
                        } มิฉะนั้น ถ้า (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        มิฉะนั้น
                                r = 0;
                        ถ้า (r < 0)
                                คืน r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, parimaa_na(var_na*));
                        ถ้า (!w)
                                คืน -ENOMEM;

                        n = w;
                        ถ้า (m) {
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
                ถ้า (r < 0)
                        คืน r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ถ้า (r < 0)
                        คืน r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ถ้า (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        คืน 0;
}

จำนวนเต็ม getenv_bool(const var_na *p) {
        const var_na *e;

        e = getenv(p);
        ถ้า (!e)
                คืน -ENXIO;

        คืน parse_boolean(e);
}

จำนวนเต็ม secure_getenv_bool(const var_na *p) {
        const var_na *e;

        e = secure_getenv(p);
        ถ้า (!e)
                คืน -ENXIO;

        คืน parse_boolean(e);
}

จำนวนเต็ม secure_getenv_uint64(const var_na *p, uint64_t *ret) {
        const var_na *e;

        assert(p);

        e = secure_getenv(p);
        ถ้า (!e)
                คืน -ENXIO;

        คืน safe_atou64(e, ret);
}

จำนวนเต็ม set_unset_env(const var_na *name, const var_na *value, bool overwrite) {
        assert(name);

        ถ้า (value)
                คืน RET_NERRNO(setenv(name, value, overwrite));

        คืน RET_NERRNO(unsetenv(name));
}

จำนวนเต็ม putenv_dup(const var_na *assignment, bool override) {
        const var_na *e, *n;

        e = strchr(assignment, '=');
        ถ้า (!e)
                คืน -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        คืน RET_NERRNO(setenv(n, e + 1, override));
}

จำนวนเต็ม setenv_systemd_exec_pid(bool update_only) {
        const var_na *e;
        จำนวนเต็ม r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ถ้า (!e && update_only)
                คืน 0;

        ถ้า (streq_ptr(e, "*"))
                คืน 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ถ้า (r < 0)
                คืน r;

        คืน 1;
}

จำนวนเต็ม setenv_systemd_log_level(void) {
        _cleanup_free_ var_na *val = NULL;
        จำนวนเต็ม r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        ถ้า (r < 0)
                คืน r;

        คืน RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

จำนวนเต็ม getenv_path_list(const var_na *name, var_na ***ret_paths) {
        _cleanup_strv_free_ var_na **l = NULL;
        const var_na *e;
        จำนวนเต็ม r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        ถ้า (!e)
                คืน -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ถ้า (r < 0)
                คืน log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ถ้า (!path_is_absolute(*p))
                        คืน log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ถ้า (!path_is_normalized(*p))
                        คืน log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ถ้า (path_equal(*p, "/"))
                        คืน log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ถ้า (strv_isempty(l))
                คืน log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        คืน 1;
}

จำนวนเต็ม getenv_steal_erase(const var_na *name, var_na **ret) {
        _cleanup_(erase_and_freep) var_na *a = NULL;
        var_na *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ถ้า (!e) {
                ถ้า (ret)
                        *ret = NULL;
                คืน 0;
        }

        ถ้า (ret) {
                a = strdup(e);
                ถ้า (!a)
                        คืน -ENOMEM;
        }

        string_erase(e);

        ถ้า (unsetenv(name) < 0)
                คืน -errno;

        ถ้า (ret)
                *ret = TAKE_PTR(a);

        คืน 1;
}

จำนวนเต็ม setenvf(const var_na *name, bool overwrite, const var_na *valuef, ...) {
        _cleanup_free_ var_na *value = NULL;
        va_list ap;
        จำนวนเต็ม r;

        assert(name);

        ถ้า (!valuef)
                คืน RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        ถ้า (r < 0)
                คืน -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ถ้า (streq_ptr(getenv(name), value))
                คืน 0;

        คืน RET_NERRNO(setenv(name, value, overwrite));
}
