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
        반환 (size_t) l;
}

static bool env_name_is_valid_n(const char *e, size_t n) {

        만약 (n == SIZE_MAX)
                n = strlen_ptr(e);

        만약 (n == 0)
                반환 false;

        assert(e);

        만약 (ascii_isdigit(e[0]))
                반환 false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        만약 (_unlikely_(n > sc_arg_max() - 2))
                반환 false;

        위해 (const char *p = e; p < e + n; p++)
                만약 (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        반환 false;

        반환 true;
}

bool env_name_is_valid(const char *e) {
        반환 env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const char *e) {
        만약 (!e)
                반환 false;

        만약 (!utf8_is_valid(e))
                반환 false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        만약 (_unlikely_(strlen(e) > sc_arg_max() - 3))
                반환 false;

        반환 true;
}

bool env_assignment_is_valid(const char *e) {
        const char *eq;

        assert(e);

        eq = strchr(e, '=');
        만약 (!eq)
                반환 false;

        만약 (!env_name_is_valid_n(e, eq - e))
                반환 false;

        만약 (!env_value_is_valid(eq + 1))
                반환 false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        만약 (_unlikely_(strlen(e) > sc_arg_max() - 1))
                반환 false;

        반환 true;
}

bool strv_env_is_valid(char * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                만약 (!env_assignment_is_valid(*p))
                        반환 false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        만약 (strneq(*p, *q, k) && (*q)[k] == '=')
                                반환 false;
        }

        반환 true;
}

bool strv_env_name_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                만약 (!env_name_is_valid(*p))
                        반환 false;

                만약 (strv_contains(p + 1, *p))
                        반환 false;
        }

        반환 true;
}

bool strv_env_name_or_assignment_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                만약 (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        반환 false;

                만약 (strv_contains(p + 1, *p))
                        반환 false;
        }

        반환 true;
}

static 정수 env_append(char **e, char ***k, char **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        만약 (!a)
                반환 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        위해 (; *a; a++) {
                char **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                만약 ((*a)[n] == '=')
                        n++;

                위해 (j = e; j < *k; j++)
                        만약 (strneq(*j, *a, n))
                                break;

                c = strdup(*a);
                만약 (!c)
                        반환 -ENOMEM;

                만약 (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } 아니면
                        free_and_replace(*j, c); /* Override existing item */
        }

        반환 0;
}

char** _strv_env_merge(char **first, ...) {
        _cleanup_strv_free_ char **merged = NULL;
        char **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        위해 (;;) {
                char **l;

                l = va_arg(ap, char**);
                만약 (l == POINTER_MAX)
                        break;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(char*, n + 1);
        만약 (!merged)
                반환 NULL;
        merged[0] = NULL;

        만약 (env_append(merged, &k, first) < 0)
                반환 NULL;

        va_start(ap, first);
        위해 (;;) {
                char **l;

                l = va_arg(ap, char**);
                만약 (l == POINTER_MAX)
                        break;

                만약 (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        반환 NULL;
                }
        }
        va_end(ap);

        반환 TAKE_PTR(merged);
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

        만약 (streq(t, pattern))
                반환 true;

        만약 (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                반환 t && *t == '=';
        }

        반환 false;
}

static bool env_entry_has_name(const char *entry, const char *name) {
        const char *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        만약 (!t)
                반환 false;

        반환 *t == '=';
}

char** strv_env_delete(char **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ char **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(char*, n+1);
        만약 (!t)
                반환 NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                위해 (size_t v = 0; v < n_lists; v++) {
                        char **l;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                만약 (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                만약 (!t[i])
                        반환 NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        반환 TAKE_PTR(t);
}

char** strv_env_unset(char **l, const char *p) {
        assert(p);

        만약 (!l)
                반환 NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        char **f, **t;
        위해 (f = t = l; *f; f++) {
                만약 (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        반환 l;
}

char** strv_env_unset_many_internal(char **l, ...) {
        만약 (!l)
                반환 NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        char **f, **t;
        위해 (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                동안 ((p = va_arg(ap, const char*)))
                        만약 (env_match(*f, p)) {
                                found = true;
                                break;
                        }

                va_end(ap);

                만약 (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        반환 l;
}

정수 strv_env_replace_consume(char ***l, char *p) {
        const char *t, *name;
        정수 r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        만약 (!t) {
                free(p);
                반환 -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                만약 (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        반환 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        만약 (r < 0)
                반환 r;

        반환 1;
}

정수 strv_env_replace_strdup(char ***l, const char *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        char *p = strdup(assignment);
        만약 (!p)
                반환 -ENOMEM;

        반환 strv_env_replace_consume(l, p);
}

정수 strv_env_replace_strdup_passthrough(char ***l, const char *assignment) {
        char *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        만약 (strchr(assignment, '=')) {
                만약 (!env_assignment_is_valid(assignment))
                        반환 -EINVAL;

                p = strdup(assignment);
        } 아니면 {
                만약 (!env_name_is_valid(assignment))
                        반환 -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        만약 (!p)
                반환 -ENOMEM;

        반환 strv_env_replace_consume(l, p);
}

정수 strv_env_assign(char ***l, const char *key, const char *value) {
        assert(l);

        만약 (!env_name_is_valid(key))
                반환 -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        만약 (!value) {
                strv_env_unset(*l, key);
                반환 0;
        }

        char *p = strjoin(key, "=", value);
        만약 (!p)
                반환 -ENOMEM;

        반환 strv_env_replace_consume(l, p);
}

정수 strv_env_assignf(char ***l, const char *key, const char *valuef, ...) {
        정수 r;

        assert(l);
        assert(key);

        만약 (!env_name_is_valid(key))
                반환 -EINVAL;

        만약 (!valuef) {
                strv_env_unset(*l, key);
                반환 0;
        }

        _cleanup_free_ char *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        만약 (r < 0)
                반환 -ENOMEM;

        char *p = strjoin(key, "=", value);
        만약 (!p)
                반환 -ENOMEM;

        반환 strv_env_replace_consume(l, p);
}

정수 _strv_env_assign_many(char ***l, ...) {
        va_list ap;
        정수 r;

        assert(l);

        va_start(ap, l);
        위해 (;;) {
                const char *key, *value;

                key = va_arg(ap, const char *);
                만약 (!key)
                        break;

                만약 (!env_name_is_valid(key)) {
                        va_end(ap);
                        반환 -EINVAL;
                }

                value = va_arg(ap, const char *);
                만약 (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                char *p = strjoin(key, "=", value);
                만약 (!p) {
                        va_end(ap);
                        반환 -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                만약 (r < 0) {
                        va_end(ap);
                        반환 r;
                }
        }
        va_end(ap);

        반환 0;
}

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        만약 (k == SIZE_MAX)
                k = strlen(name);
        만약 (k <= 0)
                반환 NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                만약 (strneq(*i, name, k) && (*i)[k] == '=')
                        반환 (char*) *i + k + 1;

        만약 (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const char *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                만약 (k > sc_arg_max() - 2)
                        반환 NULL;

                t = strndupa_safe(name, k);
                반환 secure_getenv(t);
        };

        반환 NULL;
}

char* strv_env_pairs_get(char **l, const char *name) {
        char *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                만약 (streq(*key, name))
                        result = *value;

        반환 result;
}

정수 strv_env_get_merged(char **l, char ***ret) {
        _cleanup_strv_free_ char **v = NULL;
        size_t n = 0;
        정수 r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                char *s;

                s = strjoin(*key, "=", *value);
                만약 (!s)
                        반환 -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                만약 (r < 0)
                        반환 r;
        }

        *ret = TAKE_PTR(v);
        반환 0;
}

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata) {
        정수 k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                만약 (!env_assignment_is_valid(*p)) {
                        만약 (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        만약 (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                break;
                        }

                만약 (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        만약 (l)
                l[k] = NULL;

        반환 l;
}

static 정수 strv_extend_with_length(char ***l, const char *s, size_t n) {
        char *c;

        c = strndup(s, n);
        만약 (!c)
                반환 -ENOMEM;

        반환 strv_consume(l, c);
}

static 정수 strv_env_get_n_validated(
                char **env,
                const char *name,
                size_t l,
                ReplaceEnvFlags flags,
                char **ret,              /* points into the env block! do not free! */
                char ***unset_variables, /* updated in place */
                char ***bad_variables) { /* ditto */

        char *e;
        정수 r;

        assert(l == 0 || name);
        assert(ret);

        만약 (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                만약 (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        만약 (r < 0)
                                반환 r;
                }
        } 아니면 {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                만약 (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        만약 (r < 0)
                                반환 r;
                }
        }

        *ret = e;
        반환 !!e;
}

정수 replace_env_full(
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
        정수 nest = 0, r;

        assert(format);

        만약 (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        위해 (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                case WORD:
                        만약 (*e == '$')
                                state = CURLY;
                        break;

                case CURLY:
                        만약 (*e == '{') {
                                만약 (!strextendn(&s, word, e-word-1))
                                        반환 -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } 아니면 만약 (*e == '$') {
                                만약 (!strextendn(&s, word, e-word))
                                        반환 -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } 아니면 만약 (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                만약 (!strextendn(&s, word, e-word-1))
                                        반환 -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } 아니면
                                state = WORD;
                        break;

                case VARIABLE:
                        만약 (*e == '}') {
                                char *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                만약 (r < 0)
                                        반환 r;

                                만약 (!strextend(&s, t))
                                        반환 -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } 아니면 만약 (*e == ':') {
                                만약 (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } 아니면
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        break;

                case TEST:
                        만약 (*e == '-')
                                state = DEFAULT_VALUE;
                        아니면 만약 (*e == '+')
                                state = ALTERNATE_VALUE;
                        아니면 {
                                state = WORD;
                                break;
                        }

                        test_value = e+1;
                        break;

                case DEFAULT_VALUE: /* fall through */
                case ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        만약 (*e == '{') {
                                nest++;
                                break;
                        }

                        만약 (*e != '}')
                                break;

                        nest--;
                        만약 (nest == 0) {
                                _cleanup_strv_free_ char **u = NULL, **b = NULL;
                                _cleanup_free_ char *v = NULL;
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                만약 (r < 0)
                                        반환 r;

                                만약 (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        만약 (r < 0)
                                                반환 r;

                                        t = v;
                                } 아니면 만약 (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        만약 (r < 0)
                                                반환 r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                만약 (r < 0)
                                        반환 r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                만약 (r < 0)
                                        반환 r;

                                만약 (!strextend(&s, t))
                                        반환 -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        break;

                case VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        만약 (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                만약 (r < 0)
                                        반환 r;

                                만약 (!strextend(&s, t))
                                        반환 -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        break;
                }

        만약 (state == VARIABLE_RAW) {
                char *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                만약 (r < 0)
                        반환 r;

                만약 (!strextend(&s, t))
                        반환 -ENOMEM;

        } 아니면 만약 (!strextendn(&s, word, e-word))
                반환 -ENOMEM;

        만약 (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        만약 (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        만약 (ret)
                *ret = TAKE_PTR(s);

        반환 0;
}

정수 replace_env_argv(
                char **argv,
                char **env,
                char ***ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        _cleanup_strv_free_ char **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        정수 r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(char*, l+1);
        만약 (!n)
                반환 -ENOMEM;

        STRV_FOREACH(i, argv) {
                const char *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                만약 (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ char **m = NULL;
                        const char *name = word + 1;
                        char *e, **w;
                        size_t q;

                        만약 (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                만약 (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                아니면 만약 (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                아니면
                                        r = 0;
                        } 아니면 만약 (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        아니면
                                r = 0;
                        만약 (r < 0)
                                반환 r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, sizeof(char*));
                        만약 (!w)
                                반환 -ENOMEM;

                        n = w;
                        만약 (m) {
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
                만약 (r < 0)
                        반환 r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                만약 (r < 0)
                        반환 r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                만약 (r < 0)
                        반환 r;
        }

        만약 (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        만약 (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        반환 0;
}

정수 getenv_bool(const char *p) {
        const char *e;

        e = getenv(p);
        만약 (!e)
                반환 -ENXIO;

        반환 parse_boolean(e);
}

정수 secure_getenv_bool(const char *p) {
        const char *e;

        e = secure_getenv(p);
        만약 (!e)
                반환 -ENXIO;

        반환 parse_boolean(e);
}

정수 secure_getenv_uint64(const char *p, uint64_t *ret) {
        const char *e;

        assert(p);

        e = secure_getenv(p);
        만약 (!e)
                반환 -ENXIO;

        반환 safe_atou64(e, ret);
}

정수 set_unset_env(const char *name, const char *value, bool overwrite) {
        assert(name);

        만약 (value)
                반환 RET_NERRNO(setenv(name, value, overwrite));

        반환 RET_NERRNO(unsetenv(name));
}

정수 putenv_dup(const char *assignment, bool override) {
        const char *e, *n;

        e = strchr(assignment, '=');
        만약 (!e)
                반환 -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        반환 RET_NERRNO(setenv(n, e + 1, override));
}

정수 setenv_systemd_exec_pid(bool update_only) {
        const char *e;
        정수 r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        만약 (!e && update_only)
                반환 0;

        만약 (streq_ptr(e, "*"))
                반환 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        만약 (r < 0)
                반환 r;

        반환 1;
}

정수 setenv_systemd_log_level(void) {
        _cleanup_free_ char *val = NULL;
        정수 r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        만약 (r < 0)
                반환 r;

        반환 RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

정수 getenv_path_list(const char *name, char ***ret_paths) {
        _cleanup_strv_free_ char **l = NULL;
        const char *e;
        정수 r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        만약 (!e)
                반환 -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        만약 (r < 0)
                반환 log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                만약 (!path_is_absolute(*p))
                        반환 log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                만약 (!path_is_normalized(*p))
                        반환 log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                만약 (path_equal(*p, "/"))
                        반환 log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        만약 (strv_isempty(l))
                반환 log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        반환 1;
}

정수 getenv_steal_erase(const char *name, char **ret) {
        _cleanup_(erase_and_freep) char *a = NULL;
        char *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        만약 (!e) {
                만약 (ret)
                        *ret = NULL;
                반환 0;
        }

        만약 (ret) {
                a = strdup(e);
                만약 (!a)
                        반환 -ENOMEM;
        }

        string_erase(e);

        만약 (unsetenv(name) < 0)
                반환 -errno;

        만약 (ret)
                *ret = TAKE_PTR(a);

        반환 1;
}

정수 setenvf(const char *name, bool overwrite, const char *valuef, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        정수 r;

        assert(name);

        만약 (!valuef)
                반환 RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        만약 (r < 0)
                반환 -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        만약 (streq_ptr(getenv(name), value))
                반환 0;

        반환 RET_NERRNO(setenv(name, value, overwrite));
}
