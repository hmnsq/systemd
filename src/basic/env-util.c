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
        вернуть (size_t) l;
}

static bool env_name_is_valid_n(const char *e, size_t n) {

        если (n == SIZE_MAX)
                n = strlen_ptr(e);

        если (n == 0)
                вернуть false;

        assert(e);

        если (ascii_isdigit(e[0]))
                вернуть false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        если (_unlikely_(n > sc_arg_max() - 2))
                вернуть false;

        для (const char *p = e; p < e + n; p++)
                если (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        вернуть false;

        вернуть true;
}

bool env_name_is_valid(const char *e) {
        вернуть env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const char *e) {
        если (!e)
                вернуть false;

        если (!utf8_is_valid(e))
                вернуть false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        если (_unlikely_(strlen(e) > sc_arg_max() - 3))
                вернуть false;

        вернуть true;
}

bool env_assignment_is_valid(const char *e) {
        const char *eq;

        assert(e);

        eq = strchr(e, '=');
        если (!eq)
                вернуть false;

        если (!env_name_is_valid_n(e, eq - e))
                вернуть false;

        если (!env_value_is_valid(eq + 1))
                вернуть false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        если (_unlikely_(strlen(e) > sc_arg_max() - 1))
                вернуть false;

        вернуть true;
}

bool strv_env_is_valid(char * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                если (!env_assignment_is_valid(*p))
                        вернуть false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        если (strneq(*p, *q, k) && (*q)[k] == '=')
                                вернуть false;
        }

        вернуть true;
}

bool strv_env_name_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                если (!env_name_is_valid(*p))
                        вернуть false;

                если (strv_contains(p + 1, *p))
                        вернуть false;
        }

        вернуть true;
}

bool strv_env_name_or_assignment_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                если (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        вернуть false;

                если (strv_contains(p + 1, *p))
                        вернуть false;
        }

        вернуть true;
}

static целое env_append(char **e, char ***k, char **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        если (!a)
                вернуть 0;

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
                если ((*a)[n] == '=')
                        n++;

                для (j = e; j < *k; j++)
                        если (strneq(*j, *a, n))
                                break;

                c = strdup(*a);
                если (!c)
                        вернуть -ENOMEM;

                если (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } иначе
                        free_and_replace(*j, c); /* Override existing item */
        }

        вернуть 0;
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
                если (l == POINTER_MAX)
                        break;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(char*, n + 1);
        если (!merged)
                вернуть NULL;
        merged[0] = NULL;

        если (env_append(merged, &k, first) < 0)
                вернуть NULL;

        va_start(ap, first);
        для (;;) {
                char **l;

                l = va_arg(ap, char**);
                если (l == POINTER_MAX)
                        break;

                если (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        вернуть NULL;
                }
        }
        va_end(ap);

        вернуть TAKE_PTR(merged);
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

        если (streq(t, pattern))
                вернуть true;

        если (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                вернуть t && *t == '=';
        }

        вернуть false;
}

static bool env_entry_has_name(const char *entry, const char *name) {
        const char *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        если (!t)
                вернуть false;

        вернуть *t == '=';
}

char** strv_env_delete(char **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ char **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(char*, n+1);
        если (!t)
                вернуть NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                для (size_t v = 0; v < n_lists; v++) {
                        char **l;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                если (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                если (!t[i])
                        вернуть NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        вернуть TAKE_PTR(t);
}

char** strv_env_unset(char **l, const char *p) {
        assert(p);

        если (!l)
                вернуть NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        char **f, **t;
        для (f = t = l; *f; f++) {
                если (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        вернуть l;
}

char** strv_env_unset_many_internal(char **l, ...) {
        если (!l)
                вернуть NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        char **f, **t;
        для (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                пока ((p = va_arg(ap, const char*)))
                        если (env_match(*f, p)) {
                                found = true;
                                break;
                        }

                va_end(ap);

                если (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        вернуть l;
}

целое strv_env_replace_consume(char ***l, char *p) {
        const char *t, *name;
        целое r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        если (!t) {
                free(p);
                вернуть -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                если (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        вернуть 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        если (r < 0)
                вернуть r;

        вернуть 1;
}

целое strv_env_replace_strdup(char ***l, const char *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        char *p = strdup(assignment);
        если (!p)
                вернуть -ENOMEM;

        вернуть strv_env_replace_consume(l, p);
}

целое strv_env_replace_strdup_passthrough(char ***l, const char *assignment) {
        char *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        если (strchr(assignment, '=')) {
                если (!env_assignment_is_valid(assignment))
                        вернуть -EINVAL;

                p = strdup(assignment);
        } иначе {
                если (!env_name_is_valid(assignment))
                        вернуть -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        если (!p)
                вернуть -ENOMEM;

        вернуть strv_env_replace_consume(l, p);
}

целое strv_env_assign(char ***l, const char *key, const char *value) {
        assert(l);

        если (!env_name_is_valid(key))
                вернуть -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        если (!value) {
                strv_env_unset(*l, key);
                вернуть 0;
        }

        char *p = strjoin(key, "=", value);
        если (!p)
                вернуть -ENOMEM;

        вернуть strv_env_replace_consume(l, p);
}

целое strv_env_assignf(char ***l, const char *key, const char *valuef, ...) {
        целое r;

        assert(l);
        assert(key);

        если (!env_name_is_valid(key))
                вернуть -EINVAL;

        если (!valuef) {
                strv_env_unset(*l, key);
                вернуть 0;
        }

        _cleanup_free_ char *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        если (r < 0)
                вернуть -ENOMEM;

        char *p = strjoin(key, "=", value);
        если (!p)
                вернуть -ENOMEM;

        вернуть strv_env_replace_consume(l, p);
}

целое _strv_env_assign_many(char ***l, ...) {
        va_list ap;
        целое r;

        assert(l);

        va_start(ap, l);
        для (;;) {
                const char *key, *value;

                key = va_arg(ap, const char *);
                если (!key)
                        break;

                если (!env_name_is_valid(key)) {
                        va_end(ap);
                        вернуть -EINVAL;
                }

                value = va_arg(ap, const char *);
                если (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                char *p = strjoin(key, "=", value);
                если (!p) {
                        va_end(ap);
                        вернуть -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                если (r < 0) {
                        va_end(ap);
                        вернуть r;
                }
        }
        va_end(ap);

        вернуть 0;
}

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        если (k == SIZE_MAX)
                k = strlen(name);
        если (k <= 0)
                вернуть NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                если (strneq(*i, name, k) && (*i)[k] == '=')
                        вернуть (char*) *i + k + 1;

        если (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const char *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                если (k > sc_arg_max() - 2)
                        вернуть NULL;

                t = strndupa_safe(name, k);
                вернуть secure_getenv(t);
        };

        вернуть NULL;
}

char* strv_env_pairs_get(char **l, const char *name) {
        char *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                если (streq(*key, name))
                        result = *value;

        вернуть result;
}

целое strv_env_get_merged(char **l, char ***ret) {
        _cleanup_strv_free_ char **v = NULL;
        size_t n = 0;
        целое r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                char *s;

                s = strjoin(*key, "=", *value);
                если (!s)
                        вернуть -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                если (r < 0)
                        вернуть r;
        }

        *ret = TAKE_PTR(v);
        вернуть 0;
}

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata) {
        целое k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                если (!env_assignment_is_valid(*p)) {
                        если (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        если (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                break;
                        }

                если (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        если (l)
                l[k] = NULL;

        вернуть l;
}

static целое strv_extend_with_length(char ***l, const char *s, size_t n) {
        char *c;

        c = strndup(s, n);
        если (!c)
                вернуть -ENOMEM;

        вернуть strv_consume(l, c);
}

static целое strv_env_get_n_validated(
                char **env,
                const char *name,
                size_t l,
                ReplaceEnvFlags flags,
                char **ret,              /* points into the env block! do not free! */
                char ***unset_variables, /* updated in place */
                char ***bad_variables) { /* ditto */

        char *e;
        целое r;

        assert(l == 0 || name);
        assert(ret);

        если (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                если (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        если (r < 0)
                                вернуть r;
                }
        } иначе {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                если (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        если (r < 0)
                                вернуть r;
                }
        }

        *ret = e;
        вернуть !!e;
}

целое replace_env_full(
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
        целое nest = 0, r;

        assert(format);

        если (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        для (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                case WORD:
                        если (*e == '$')
                                state = CURLY;
                        break;

                case CURLY:
                        если (*e == '{') {
                                если (!strextendn(&s, word, e-word-1))
                                        вернуть -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } иначе если (*e == '$') {
                                если (!strextendn(&s, word, e-word))
                                        вернуть -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } иначе если (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                если (!strextendn(&s, word, e-word-1))
                                        вернуть -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } иначе
                                state = WORD;
                        break;

                case VARIABLE:
                        если (*e == '}') {
                                char *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                если (r < 0)
                                        вернуть r;

                                если (!strextend(&s, t))
                                        вернуть -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } иначе если (*e == ':') {
                                если (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } иначе
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        break;

                case TEST:
                        если (*e == '-')
                                state = DEFAULT_VALUE;
                        иначе если (*e == '+')
                                state = ALTERNATE_VALUE;
                        иначе {
                                state = WORD;
                                break;
                        }

                        test_value = e+1;
                        break;

                case DEFAULT_VALUE: /* fall through */
                case ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        если (*e == '{') {
                                nest++;
                                break;
                        }

                        если (*e != '}')
                                break;

                        nest--;
                        если (nest == 0) {
                                _cleanup_strv_free_ char **u = NULL, **b = NULL;
                                _cleanup_free_ char *v = NULL;
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                если (r < 0)
                                        вернуть r;

                                если (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        если (r < 0)
                                                вернуть r;

                                        t = v;
                                } иначе если (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        если (r < 0)
                                                вернуть r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                если (r < 0)
                                        вернуть r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                если (r < 0)
                                        вернуть r;

                                если (!strextend(&s, t))
                                        вернуть -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        break;

                case VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        если (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                если (r < 0)
                                        вернуть r;

                                если (!strextend(&s, t))
                                        вернуть -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        break;
                }

        если (state == VARIABLE_RAW) {
                char *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                если (r < 0)
                        вернуть r;

                если (!strextend(&s, t))
                        вернуть -ENOMEM;

        } иначе если (!strextendn(&s, word, e-word))
                вернуть -ENOMEM;

        если (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        если (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        если (ret)
                *ret = TAKE_PTR(s);

        вернуть 0;
}

целое replace_env_argv(
                char **argv,
                char **env,
                char ***ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        _cleanup_strv_free_ char **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        целое r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(char*, l+1);
        если (!n)
                вернуть -ENOMEM;

        STRV_FOREACH(i, argv) {
                const char *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                если (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ char **m = NULL;
                        const char *name = word + 1;
                        char *e, **w;
                        size_t q;

                        если (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                если (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                иначе если (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                иначе
                                        r = 0;
                        } иначе если (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        иначе
                                r = 0;
                        если (r < 0)
                                вернуть r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, sizeof(char*));
                        если (!w)
                                вернуть -ENOMEM;

                        n = w;
                        если (m) {
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
                если (r < 0)
                        вернуть r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                если (r < 0)
                        вернуть r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                если (r < 0)
                        вернуть r;
        }

        если (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        если (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        вернуть 0;
}

целое getenv_bool(const char *p) {
        const char *e;

        e = getenv(p);
        если (!e)
                вернуть -ENXIO;

        вернуть parse_boolean(e);
}

целое secure_getenv_bool(const char *p) {
        const char *e;

        e = secure_getenv(p);
        если (!e)
                вернуть -ENXIO;

        вернуть parse_boolean(e);
}

целое secure_getenv_uint64(const char *p, uint64_t *ret) {
        const char *e;

        assert(p);

        e = secure_getenv(p);
        если (!e)
                вернуть -ENXIO;

        вернуть safe_atou64(e, ret);
}

целое set_unset_env(const char *name, const char *value, bool overwrite) {
        assert(name);

        если (value)
                вернуть RET_NERRNO(setenv(name, value, overwrite));

        вернуть RET_NERRNO(unsetenv(name));
}

целое putenv_dup(const char *assignment, bool override) {
        const char *e, *n;

        e = strchr(assignment, '=');
        если (!e)
                вернуть -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        вернуть RET_NERRNO(setenv(n, e + 1, override));
}

целое setenv_systemd_exec_pid(bool update_only) {
        const char *e;
        целое r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        если (!e && update_only)
                вернуть 0;

        если (streq_ptr(e, "*"))
                вернуть 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        если (r < 0)
                вернуть r;

        вернуть 1;
}

целое setenv_systemd_log_level(void) {
        _cleanup_free_ char *val = NULL;
        целое r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        если (r < 0)
                вернуть r;

        вернуть RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

целое getenv_path_list(const char *name, char ***ret_paths) {
        _cleanup_strv_free_ char **l = NULL;
        const char *e;
        целое r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        если (!e)
                вернуть -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                если (!path_is_absolute(*p))
                        вернуть log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                если (!path_is_normalized(*p))
                        вернуть log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                если (path_equal(*p, "/"))
                        вернуть log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        если (strv_isempty(l))
                вернуть log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        вернуть 1;
}

целое getenv_steal_erase(const char *name, char **ret) {
        _cleanup_(erase_and_freep) char *a = NULL;
        char *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        если (!e) {
                если (ret)
                        *ret = NULL;
                вернуть 0;
        }

        если (ret) {
                a = strdup(e);
                если (!a)
                        вернуть -ENOMEM;
        }

        string_erase(e);

        если (unsetenv(name) < 0)
                вернуть -errno;

        если (ret)
                *ret = TAKE_PTR(a);

        вернуть 1;
}

целое setenvf(const char *name, bool overwrite, const char *valuef, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        целое r;

        assert(name);

        если (!valuef)
                вернуть RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        если (r < 0)
                вернуть -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        если (streq_ptr(getenv(name), value))
                вернуть 0;

        вернуть RET_NERRNO(setenv(name, value, overwrite));
}
