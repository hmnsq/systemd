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
        повернути (size_t) l;
}

static bool env_name_is_valid_n(const char *e, size_t n) {

        якщо (n == SIZE_MAX)
                n = strlen_ptr(e);

        якщо (n == 0)
                повернути false;

        assert(e);

        якщо (ascii_isdigit(e[0]))
                повернути false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        якщо (_unlikely_(n > sc_arg_max() - 2))
                повернути false;

        для (const char *p = e; p < e + n; p++)
                якщо (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        повернути false;

        повернути true;
}

bool env_name_is_valid(const char *e) {
        повернути env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const char *e) {
        якщо (!e)
                повернути false;

        якщо (!utf8_is_valid(e))
                повернути false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        якщо (_unlikely_(strlen(e) > sc_arg_max() - 3))
                повернути false;

        повернути true;
}

bool env_assignment_is_valid(const char *e) {
        const char *eq;

        assert(e);

        eq = strchr(e, '=');
        якщо (!eq)
                повернути false;

        якщо (!env_name_is_valid_n(e, eq - e))
                повернути false;

        якщо (!env_value_is_valid(eq + 1))
                повернути false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        якщо (_unlikely_(strlen(e) > sc_arg_max() - 1))
                повернути false;

        повернути true;
}

bool strv_env_is_valid(char * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                якщо (!env_assignment_is_valid(*p))
                        повернути false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        якщо (strneq(*p, *q, k) && (*q)[k] == '=')
                                повернути false;
        }

        повернути true;
}

bool strv_env_name_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                якщо (!env_name_is_valid(*p))
                        повернути false;

                якщо (strv_contains(p + 1, *p))
                        повернути false;
        }

        повернути true;
}

bool strv_env_name_or_assignment_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                якщо (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        повернути false;

                якщо (strv_contains(p + 1, *p))
                        повернути false;
        }

        повернути true;
}

static ціле env_append(char **e, char ***k, char **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        якщо (!a)
                повернути 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        для (; *a; a++) {
                char **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                якщо ((*a)[n] == '=')
                        n++;

                для (j = e; j < *k; j++)
                        якщо (strneq(*j, *a, n))
                                break;

                c = strdup(*a);
                якщо (!c)
                        повернути -ENOMEM;

                якщо (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } інакше
                        free_and_replace(*j, c); /* Override existing item */
        }

        повернути 0;
}

char** _strv_env_merge(char **first, ...) {
        _cleanup_strv_free_ char **merged = NULL;
        char **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        для (;;) {
                char **l;

                l = va_arg(ap, char**);
                якщо (l == POINTER_MAX)
                        break;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(char*, n + 1);
        якщо (!merged)
                повернути NULL;
        merged[0] = NULL;

        якщо (env_append(merged, &k, first) < 0)
                повернути NULL;

        va_start(ap, first);
        для (;;) {
                char **l;

                l = va_arg(ap, char**);
                якщо (l == POINTER_MAX)
                        break;

                якщо (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        повернути NULL;
                }
        }
        va_end(ap);

        повернути TAKE_PTR(merged);
}

static bool env_match(const char *t, const char *pattern) {
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

        якщо (streq(t, pattern))
                повернути true;

        якщо (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                повернути t && *t == '=';
        }

        повернути false;
}

static bool env_entry_has_name(const char *entry, const char *name) {
        const char *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        якщо (!t)
                повернути false;

        повернути *t == '=';
}

char** strv_env_delete(char **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ char **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(char*, n+1);
        якщо (!t)
                повернути NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                для (size_t v = 0; v < n_lists; v++) {
                        char **l;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                якщо (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                якщо (!t[i])
                        повернути NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        повернути TAKE_PTR(t);
}

char** strv_env_unset(char **l, const char *p) {
        assert(p);

        якщо (!l)
                повернути NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        char **f, **t;
        для (f = t = l; *f; f++) {
                якщо (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        повернути l;
}

char** strv_env_unset_many_internal(char **l, ...) {
        якщо (!l)
                повернути NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        char **f, **t;
        для (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                поки ((p = va_arg(ap, const char*)))
                        якщо (env_match(*f, p)) {
                                found = true;
                                break;
                        }

                va_end(ap);

                якщо (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        повернути l;
}

ціле strv_env_replace_consume(char ***l, char *p) {
        const char *t, *name;
        ціле r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        якщо (!t) {
                free(p);
                повернути -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                якщо (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        повернути 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        якщо (r < 0)
                повернути r;

        повернути 1;
}

ціле strv_env_replace_strdup(char ***l, const char *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        char *p = strdup(assignment);
        якщо (!p)
                повернути -ENOMEM;

        повернути strv_env_replace_consume(l, p);
}

ціле strv_env_replace_strdup_passthrough(char ***l, const char *assignment) {
        char *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        якщо (strchr(assignment, '=')) {
                якщо (!env_assignment_is_valid(assignment))
                        повернути -EINVAL;

                p = strdup(assignment);
        } інакше {
                якщо (!env_name_is_valid(assignment))
                        повернути -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        якщо (!p)
                повернути -ENOMEM;

        повернути strv_env_replace_consume(l, p);
}

ціле strv_env_assign(char ***l, const char *key, const char *value) {
        assert(l);

        якщо (!env_name_is_valid(key))
                повернути -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        якщо (!value) {
                strv_env_unset(*l, key);
                повернути 0;
        }

        char *p = strjoin(key, "=", value);
        якщо (!p)
                повернути -ENOMEM;

        повернути strv_env_replace_consume(l, p);
}

ціле strv_env_assignf(char ***l, const char *key, const char *valuef, ...) {
        ціле r;

        assert(l);
        assert(key);

        якщо (!env_name_is_valid(key))
                повернути -EINVAL;

        якщо (!valuef) {
                strv_env_unset(*l, key);
                повернути 0;
        }

        _cleanup_free_ char *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        якщо (r < 0)
                повернути -ENOMEM;

        char *p = strjoin(key, "=", value);
        якщо (!p)
                повернути -ENOMEM;

        повернути strv_env_replace_consume(l, p);
}

ціле _strv_env_assign_many(char ***l, ...) {
        va_list ap;
        ціле r;

        assert(l);

        va_start(ap, l);
        для (;;) {
                const char *key, *value;

                key = va_arg(ap, const char *);
                якщо (!key)
                        break;

                якщо (!env_name_is_valid(key)) {
                        va_end(ap);
                        повернути -EINVAL;
                }

                value = va_arg(ap, const char *);
                якщо (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                char *p = strjoin(key, "=", value);
                якщо (!p) {
                        va_end(ap);
                        повернути -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                якщо (r < 0) {
                        va_end(ap);
                        повернути r;
                }
        }
        va_end(ap);

        повернути 0;
}

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        якщо (k == SIZE_MAX)
                k = strlen(name);
        якщо (k <= 0)
                повернути NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                якщо (strneq(*i, name, k) && (*i)[k] == '=')
                        повернути (char*) *i + k + 1;

        якщо (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const char *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                якщо (k > sc_arg_max() - 2)
                        повернути NULL;

                t = strndupa_safe(name, k);
                повернути secure_getenv(t);
        };

        повернути NULL;
}

char* strv_env_pairs_get(char **l, const char *name) {
        char *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                якщо (streq(*key, name))
                        result = *value;

        повернути result;
}

ціле strv_env_get_merged(char **l, char ***ret) {
        _cleanup_strv_free_ char **v = NULL;
        size_t n = 0;
        ціле r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                char *s;

                s = strjoin(*key, "=", *value);
                якщо (!s)
                        повернути -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                якщо (r < 0)
                        повернути r;
        }

        *ret = TAKE_PTR(v);
        повернути 0;
}

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata) {
        ціле k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                якщо (!env_assignment_is_valid(*p)) {
                        якщо (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        якщо (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                break;
                        }

                якщо (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        якщо (l)
                l[k] = NULL;

        повернути l;
}

static ціле strv_extend_with_length(char ***l, const char *s, size_t n) {
        char *c;

        c = strndup(s, n);
        якщо (!c)
                повернути -ENOMEM;

        повернути strv_consume(l, c);
}

static ціле strv_env_get_n_validated(
                char **env,
                const char *name,
                size_t l,
                ReplaceEnvFlags flags,
                char **ret,              /* points into the env block! do not free! */
                char ***unset_variables, /* updated in place */
                char ***bad_variables) { /* ditto */

        char *e;
        ціле r;

        assert(l == 0 || name);
        assert(ret);

        якщо (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                якщо (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        якщо (r < 0)
                                повернути r;
                }
        } інакше {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                якщо (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        якщо (r < 0)
                                повернути r;
                }
        }

        *ret = e;
        повернути !!e;
}

ціле replace_env_full(
                const char *format,
                size_t length,
                char **env,
                ReplaceEnvFlags flags,
                char **ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        enum {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_VALUE,
                ALTERNATE_VALUE,
        } state = WORD;

        _cleanup_strv_free_ char **unset_variables = NULL, **bad_variables = NULL;
        const char *e, *word = format, *test_value = NULL; /* test_value is initialized to appease gcc */
        _cleanup_free_ char *s = NULL;
        char ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        ціле nest = 0, r;

        assert(format);

        якщо (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        для (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                case WORD:
                        якщо (*e == '$')
                                state = CURLY;
                        break;

                case CURLY:
                        якщо (*e == '{') {
                                якщо (!strextendn(&s, word, e-word-1))
                                        повернути -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } інакше якщо (*e == '$') {
                                якщо (!strextendn(&s, word, e-word))
                                        повернути -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } інакше якщо (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                якщо (!strextendn(&s, word, e-word-1))
                                        повернути -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } інакше
                                state = WORD;
                        break;

                case VARIABLE:
                        якщо (*e == '}') {
                                char *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                якщо (r < 0)
                                        повернути r;

                                якщо (!strextend(&s, t))
                                        повернути -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } інакше якщо (*e == ':') {
                                якщо (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } інакше
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        break;

                case TEST:
                        якщо (*e == '-')
                                state = DEFAULT_VALUE;
                        інакше якщо (*e == '+')
                                state = ALTERNATE_VALUE;
                        інакше {
                                state = WORD;
                                break;
                        }

                        test_value = e+1;
                        break;

                case DEFAULT_VALUE: /* fall through */
                case ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        якщо (*e == '{') {
                                nest++;
                                break;
                        }

                        якщо (*e != '}')
                                break;

                        nest--;
                        якщо (nest == 0) {
                                _cleanup_strv_free_ char **u = NULL, **b = NULL;
                                _cleanup_free_ char *v = NULL;
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                якщо (r < 0)
                                        повернути r;

                                якщо (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        якщо (r < 0)
                                                повернути r;

                                        t = v;
                                } інакше якщо (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        якщо (r < 0)
                                                повернути r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                якщо (r < 0)
                                        повернути r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                якщо (r < 0)
                                        повернути r;

                                якщо (!strextend(&s, t))
                                        повернути -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        break;

                case VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        якщо (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                якщо (r < 0)
                                        повернути r;

                                якщо (!strextend(&s, t))
                                        повернути -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        break;
                }

        якщо (state == VARIABLE_RAW) {
                char *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                якщо (r < 0)
                        повернути r;

                якщо (!strextend(&s, t))
                        повернути -ENOMEM;

        } інакше якщо (!strextendn(&s, word, e-word))
                повернути -ENOMEM;

        якщо (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        якщо (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        якщо (ret)
                *ret = TAKE_PTR(s);

        повернути 0;
}

ціле replace_env_argv(
                char **argv,
                char **env,
                char ***ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        _cleanup_strv_free_ char **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        ціле r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(char*, l+1);
        якщо (!n)
                повернути -ENOMEM;

        STRV_FOREACH(i, argv) {
                const char *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                якщо (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ char **m = NULL;
                        const char *name = word + 1;
                        char *e, **w;
                        size_t q;

                        якщо (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                якщо (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                інакше якщо (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                інакше
                                        r = 0;
                        } інакше якщо (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        інакше
                                r = 0;
                        якщо (r < 0)
                                повернути r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, sizeof(char*));
                        якщо (!w)
                                повернути -ENOMEM;

                        n = w;
                        якщо (m) {
                                memcpy(n + k, m, (q + 1) * sizeof(char*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = NULL;
                        continue;
                }

                _cleanup_strv_free_ char **u = NULL, **b = NULL;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : NULL,
                                ret_bad_variables ? &b : NULL);
                якщо (r < 0)
                        повернути r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                якщо (r < 0)
                        повернути r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        якщо (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        повернути 0;
}

ціле getenv_bool(const char *p) {
        const char *e;

        e = getenv(p);
        якщо (!e)
                повернути -ENXIO;

        повернути parse_boolean(e);
}

ціле secure_getenv_bool(const char *p) {
        const char *e;

        e = secure_getenv(p);
        якщо (!e)
                повернути -ENXIO;

        повернути parse_boolean(e);
}

ціле secure_getenv_uint64(const char *p, uint64_t *ret) {
        const char *e;

        assert(p);

        e = secure_getenv(p);
        якщо (!e)
                повернути -ENXIO;

        повернути safe_atou64(e, ret);
}

ціле set_unset_env(const char *name, const char *value, bool overwrite) {
        assert(name);

        якщо (value)
                повернути RET_NERRNO(setenv(name, value, overwrite));

        повернути RET_NERRNO(unsetenv(name));
}

ціле putenv_dup(const char *assignment, bool override) {
        const char *e, *n;

        e = strchr(assignment, '=');
        якщо (!e)
                повернути -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        повернути RET_NERRNO(setenv(n, e + 1, override));
}

ціле setenv_systemd_exec_pid(bool update_only) {
        const char *e;
        ціле r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        якщо (!e && update_only)
                повернути 0;

        якщо (streq_ptr(e, "*"))
                повернути 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        якщо (r < 0)
                повернути r;

        повернути 1;
}

ціле setenv_systemd_log_level(void) {
        _cleanup_free_ char *val = NULL;
        ціле r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        якщо (r < 0)
                повернути r;

        повернути RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ціле getenv_path_list(const char *name, char ***ret_paths) {
        _cleanup_strv_free_ char **l = NULL;
        const char *e;
        ціле r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        якщо (!e)
                повернути -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                якщо (!path_is_absolute(*p))
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                якщо (!path_is_normalized(*p))
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                якщо (path_equal(*p, "/"))
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        якщо (strv_isempty(l))
                повернути log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        повернути 1;
}

ціле getenv_steal_erase(const char *name, char **ret) {
        _cleanup_(erase_and_freep) char *a = NULL;
        char *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        якщо (!e) {
                якщо (ret)
                        *ret = NULL;
                повернути 0;
        }

        якщо (ret) {
                a = strdup(e);
                якщо (!a)
                        повернути -ENOMEM;
        }

        string_erase(e);

        якщо (unsetenv(name) < 0)
                повернути -errno;

        якщо (ret)
                *ret = TAKE_PTR(a);

        повернути 1;
}

ціле setenvf(const char *name, bool overwrite, const char *valuef, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        ціле r;

        assert(name);

        якщо (!valuef)
                повернути RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        якщо (r < 0)
                повернути -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        якщо (streq_ptr(getenv(name), value))
                повернути 0;

        повернути RET_NERRNO(setenv(name, value, overwrite));
}
