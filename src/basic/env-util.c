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
        върни (size_t) l;
}

static bool env_name_is_valid_n(const char *e, size_t n) {

        ако (n == SIZE_MAX)
                n = strlen_ptr(e);

        ако (n == 0)
                върни false;

        assert(e);

        ако (ascii_isdigit(e[0]))
                върни false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ако (_unlikely_(n > sc_arg_max() - 2))
                върни false;

        за (const char *p = e; p < e + n; p++)
                ако (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        върни false;

        върни true;
}

bool env_name_is_valid(const char *e) {
        върни env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const char *e) {
        ако (!e)
                върни false;

        ако (!utf8_is_valid(e))
                върни false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ако (_unlikely_(strlen(e) > sc_arg_max() - 3))
                върни false;

        върни true;
}

bool env_assignment_is_valid(const char *e) {
        const char *eq;

        assert(e);

        eq = strchr(e, '=');
        ако (!eq)
                върни false;

        ако (!env_name_is_valid_n(e, eq - e))
                върни false;

        ако (!env_value_is_valid(eq + 1))
                върни false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ако (_unlikely_(strlen(e) > sc_arg_max() - 1))
                върни false;

        върни true;
}

bool strv_env_is_valid(char * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ако (!env_assignment_is_valid(*p))
                        върни false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ако (strneq(*p, *q, k) && (*q)[k] == '=')
                                върни false;
        }

        върни true;
}

bool strv_env_name_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                ако (!env_name_is_valid(*p))
                        върни false;

                ако (strv_contains(p + 1, *p))
                        върни false;
        }

        върни true;
}

bool strv_env_name_or_assignment_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                ако (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        върни false;

                ако (strv_contains(p + 1, *p))
                        върни false;
        }

        върни true;
}

static цяло env_append(char **e, char ***k, char **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        ако (!a)
                върни 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        за (; *a; a++) {
                char **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ако ((*a)[n] == '=')
                        n++;

                за (j = e; j < *k; j++)
                        ако (strneq(*j, *a, n))
                                break;

                c = strdup(*a);
                ако (!c)
                        върни -ENOMEM;

                ако (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } иначе
                        free_and_replace(*j, c); /* Override existing item */
        }

        върни 0;
}

char** _strv_env_merge(char **first, ...) {
        _cleanup_strv_free_ char **merged = NULL;
        char **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        за (;;) {
                char **l;

                l = va_arg(ap, char**);
                ако (l == POINTER_MAX)
                        break;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(char*, n + 1);
        ако (!merged)
                върни NULL;
        merged[0] = NULL;

        ако (env_append(merged, &k, first) < 0)
                върни NULL;

        va_start(ap, first);
        за (;;) {
                char **l;

                l = va_arg(ap, char**);
                ако (l == POINTER_MAX)
                        break;

                ако (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        върни NULL;
                }
        }
        va_end(ap);

        върни TAKE_PTR(merged);
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

        ако (streq(t, pattern))
                върни true;

        ако (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                върни t && *t == '=';
        }

        върни false;
}

static bool env_entry_has_name(const char *entry, const char *name) {
        const char *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        ако (!t)
                върни false;

        върни *t == '=';
}

char** strv_env_delete(char **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ char **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(char*, n+1);
        ако (!t)
                върни NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                за (size_t v = 0; v < n_lists; v++) {
                        char **l;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                ако (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ако (!t[i])
                        върни NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        върни TAKE_PTR(t);
}

char** strv_env_unset(char **l, const char *p) {
        assert(p);

        ако (!l)
                върни NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        char **f, **t;
        за (f = t = l; *f; f++) {
                ако (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        върни l;
}

char** strv_env_unset_many_internal(char **l, ...) {
        ако (!l)
                върни NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        char **f, **t;
        за (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                докато ((p = va_arg(ap, const char*)))
                        ако (env_match(*f, p)) {
                                found = true;
                                break;
                        }

                va_end(ap);

                ако (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        върни l;
}

цяло strv_env_replace_consume(char ***l, char *p) {
        const char *t, *name;
        цяло r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ако (!t) {
                free(p);
                върни -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                ако (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        върни 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ако (r < 0)
                върни r;

        върни 1;
}

цяло strv_env_replace_strdup(char ***l, const char *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        char *p = strdup(assignment);
        ако (!p)
                върни -ENOMEM;

        върни strv_env_replace_consume(l, p);
}

цяло strv_env_replace_strdup_passthrough(char ***l, const char *assignment) {
        char *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        ако (strchr(assignment, '=')) {
                ако (!env_assignment_is_valid(assignment))
                        върни -EINVAL;

                p = strdup(assignment);
        } иначе {
                ако (!env_name_is_valid(assignment))
                        върни -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ако (!p)
                върни -ENOMEM;

        върни strv_env_replace_consume(l, p);
}

цяло strv_env_assign(char ***l, const char *key, const char *value) {
        assert(l);

        ако (!env_name_is_valid(key))
                върни -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ако (!value) {
                strv_env_unset(*l, key);
                върни 0;
        }

        char *p = strjoin(key, "=", value);
        ако (!p)
                върни -ENOMEM;

        върни strv_env_replace_consume(l, p);
}

цяло strv_env_assignf(char ***l, const char *key, const char *valuef, ...) {
        цяло r;

        assert(l);
        assert(key);

        ако (!env_name_is_valid(key))
                върни -EINVAL;

        ако (!valuef) {
                strv_env_unset(*l, key);
                върни 0;
        }

        _cleanup_free_ char *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        ако (r < 0)
                върни -ENOMEM;

        char *p = strjoin(key, "=", value);
        ако (!p)
                върни -ENOMEM;

        върни strv_env_replace_consume(l, p);
}

цяло _strv_env_assign_many(char ***l, ...) {
        va_list ap;
        цяло r;

        assert(l);

        va_start(ap, l);
        за (;;) {
                const char *key, *value;

                key = va_arg(ap, const char *);
                ако (!key)
                        break;

                ако (!env_name_is_valid(key)) {
                        va_end(ap);
                        върни -EINVAL;
                }

                value = va_arg(ap, const char *);
                ако (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                char *p = strjoin(key, "=", value);
                ако (!p) {
                        va_end(ap);
                        върни -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ако (r < 0) {
                        va_end(ap);
                        върни r;
                }
        }
        va_end(ap);

        върни 0;
}

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        ако (k == SIZE_MAX)
                k = strlen(name);
        ако (k <= 0)
                върни NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                ако (strneq(*i, name, k) && (*i)[k] == '=')
                        върни (char*) *i + k + 1;

        ако (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const char *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ако (k > sc_arg_max() - 2)
                        върни NULL;

                t = strndupa_safe(name, k);
                върни secure_getenv(t);
        };

        върни NULL;
}

char* strv_env_pairs_get(char **l, const char *name) {
        char *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                ако (streq(*key, name))
                        result = *value;

        върни result;
}

цяло strv_env_get_merged(char **l, char ***ret) {
        _cleanup_strv_free_ char **v = NULL;
        size_t n = 0;
        цяло r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                char *s;

                s = strjoin(*key, "=", *value);
                ако (!s)
                        върни -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                ако (r < 0)
                        върни r;
        }

        *ret = TAKE_PTR(v);
        върни 0;
}

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata) {
        цяло k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ако (!env_assignment_is_valid(*p)) {
                        ако (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ако (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                break;
                        }

                ако (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        ако (l)
                l[k] = NULL;

        върни l;
}

static цяло strv_extend_with_length(char ***l, const char *s, size_t n) {
        char *c;

        c = strndup(s, n);
        ако (!c)
                върни -ENOMEM;

        върни strv_consume(l, c);
}

static цяло strv_env_get_n_validated(
                char **env,
                const char *name,
                size_t l,
                ReplaceEnvFlags flags,
                char **ret,              /* points into the env block! do not free! */
                char ***unset_variables, /* updated in place */
                char ***bad_variables) { /* ditto */

        char *e;
        цяло r;

        assert(l == 0 || name);
        assert(ret);

        ако (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ако (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        ако (r < 0)
                                върни r;
                }
        } иначе {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                ако (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        ако (r < 0)
                                върни r;
                }
        }

        *ret = e;
        върни !!e;
}

цяло replace_env_full(
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
        цяло nest = 0, r;

        assert(format);

        ако (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        за (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                case WORD:
                        ако (*e == '$')
                                state = CURLY;
                        break;

                case CURLY:
                        ако (*e == '{') {
                                ако (!strextendn(&s, word, e-word-1))
                                        върни -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } иначе ако (*e == '$') {
                                ако (!strextendn(&s, word, e-word))
                                        върни -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } иначе ако (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ако (!strextendn(&s, word, e-word-1))
                                        върни -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } иначе
                                state = WORD;
                        break;

                case VARIABLE:
                        ако (*e == '}') {
                                char *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                ако (r < 0)
                                        върни r;

                                ако (!strextend(&s, t))
                                        върни -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } иначе ако (*e == ':') {
                                ако (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } иначе
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        break;

                case TEST:
                        ако (*e == '-')
                                state = DEFAULT_VALUE;
                        иначе ако (*e == '+')
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

                        ако (*e == '{') {
                                nest++;
                                break;
                        }

                        ако (*e != '}')
                                break;

                        nest--;
                        ако (nest == 0) {
                                _cleanup_strv_free_ char **u = NULL, **b = NULL;
                                _cleanup_free_ char *v = NULL;
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                ако (r < 0)
                                        върни r;

                                ако (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ако (r < 0)
                                                върни r;

                                        t = v;
                                } иначе ако (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        ако (r < 0)
                                                върни r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ако (r < 0)
                                        върни r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ако (r < 0)
                                        върни r;

                                ако (!strextend(&s, t))
                                        върни -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        break;

                case VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ако (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                ако (r < 0)
                                        върни r;

                                ако (!strextend(&s, t))
                                        върни -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        break;
                }

        ако (state == VARIABLE_RAW) {
                char *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                ако (r < 0)
                        върни r;

                ако (!strextend(&s, t))
                        върни -ENOMEM;

        } иначе ако (!strextendn(&s, word, e-word))
                върни -ENOMEM;

        ако (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ако (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ако (ret)
                *ret = TAKE_PTR(s);

        върни 0;
}

цяло replace_env_argv(
                char **argv,
                char **env,
                char ***ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        _cleanup_strv_free_ char **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        цяло r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(char*, l+1);
        ако (!n)
                върни -ENOMEM;

        STRV_FOREACH(i, argv) {
                const char *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ако (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ char **m = NULL;
                        const char *name = word + 1;
                        char *e, **w;
                        size_t q;

                        ако (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ако (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                иначе ако (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                иначе
                                        r = 0;
                        } иначе ако (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        иначе
                                r = 0;
                        ако (r < 0)
                                върни r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, sizeof(char*));
                        ако (!w)
                                върни -ENOMEM;

                        n = w;
                        ако (m) {
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
                ако (r < 0)
                        върни r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ако (r < 0)
                        върни r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ако (r < 0)
                        върни r;
        }

        ако (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ако (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        върни 0;
}

цяло getenv_bool(const char *p) {
        const char *e;

        e = getenv(p);
        ако (!e)
                върни -ENXIO;

        върни parse_boolean(e);
}

цяло secure_getenv_bool(const char *p) {
        const char *e;

        e = secure_getenv(p);
        ако (!e)
                върни -ENXIO;

        върни parse_boolean(e);
}

цяло secure_getenv_uint64(const char *p, uint64_t *ret) {
        const char *e;

        assert(p);

        e = secure_getenv(p);
        ако (!e)
                върни -ENXIO;

        върни safe_atou64(e, ret);
}

цяло set_unset_env(const char *name, const char *value, bool overwrite) {
        assert(name);

        ако (value)
                върни RET_NERRNO(setenv(name, value, overwrite));

        върни RET_NERRNO(unsetenv(name));
}

цяло putenv_dup(const char *assignment, bool override) {
        const char *e, *n;

        e = strchr(assignment, '=');
        ако (!e)
                върни -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        върни RET_NERRNO(setenv(n, e + 1, override));
}

цяло setenv_systemd_exec_pid(bool update_only) {
        const char *e;
        цяло r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ако (!e && update_only)
                върни 0;

        ако (streq_ptr(e, "*"))
                върни 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ако (r < 0)
                върни r;

        върни 1;
}

цяло setenv_systemd_log_level(void) {
        _cleanup_free_ char *val = NULL;
        цяло r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        ако (r < 0)
                върни r;

        върни RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

цяло getenv_path_list(const char *name, char ***ret_paths) {
        _cleanup_strv_free_ char **l = NULL;
        const char *e;
        цяло r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        ако (!e)
                върни -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ако (r < 0)
                върни log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ако (!path_is_absolute(*p))
                        върни log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ако (!path_is_normalized(*p))
                        върни log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ако (path_equal(*p, "/"))
                        върни log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ако (strv_isempty(l))
                върни log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        върни 1;
}

цяло getenv_steal_erase(const char *name, char **ret) {
        _cleanup_(erase_and_freep) char *a = NULL;
        char *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ако (!e) {
                ако (ret)
                        *ret = NULL;
                върни 0;
        }

        ако (ret) {
                a = strdup(e);
                ако (!a)
                        върни -ENOMEM;
        }

        string_erase(e);

        ако (unsetenv(name) < 0)
                върни -errno;

        ако (ret)
                *ret = TAKE_PTR(a);

        върни 1;
}

цяло setenvf(const char *name, bool overwrite, const char *valuef, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        цяло r;

        assert(name);

        ако (!valuef)
                върни RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        ако (r < 0)
                върни -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ако (streq_ptr(getenv(name), value))
                върни 0;

        върни RET_NERRNO(setenv(name, value, overwrite));
}
