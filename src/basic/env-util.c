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
        trả_về (size_t) l;
}

static bool env_name_is_valid_n(const char *e, size_t n) {

        nếu (n == SIZE_MAX)
                n = strlen_ptr(e);

        nếu (n == 0)
                trả_về false;

        assert(e);

        nếu (ascii_isdigit(e[0]))
                trả_về false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        nếu (_unlikely_(n > sc_arg_max() - 2))
                trả_về false;

        cho (const char *p = e; p < e + n; p++)
                nếu (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        trả_về false;

        trả_về true;
}

bool env_name_is_valid(const char *e) {
        trả_về env_name_is_valid_n(e, SIZE_MAX);
}

bool env_value_is_valid(const char *e) {
        nếu (!e)
                trả_về false;

        nếu (!utf8_is_valid(e))
                trả_về false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        nếu (_unlikely_(strlen(e) > sc_arg_max() - 3))
                trả_về false;

        trả_về true;
}

bool env_assignment_is_valid(const char *e) {
        const char *eq;

        assert(e);

        eq = strchr(e, '=');
        nếu (!eq)
                trả_về false;

        nếu (!env_name_is_valid_n(e, eq - e))
                trả_về false;

        nếu (!env_value_is_valid(eq + 1))
                trả_về false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        nếu (_unlikely_(strlen(e) > sc_arg_max() - 1))
                trả_về false;

        trả_về true;
}

bool strv_env_is_valid(char * const *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                nếu (!env_assignment_is_valid(*p))
                        trả_về false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        nếu (strneq(*p, *q, k) && (*q)[k] == '=')
                                trả_về false;
        }

        trả_về true;
}

bool strv_env_name_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                nếu (!env_name_is_valid(*p))
                        trả_về false;

                nếu (strv_contains(p + 1, *p))
                        trả_về false;
        }

        trả_về true;
}

bool strv_env_name_or_assignment_is_valid(char * const *l) {
        STRV_FOREACH(p, l) {
                nếu (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        trả_về false;

                nếu (strv_contains(p + 1, *p))
                        trả_về false;
        }

        trả_về true;
}

static số_nguyên env_append(char **e, char ***k, char **a) {
        assert(e);
        assert(k);
        assert(*k >= e);

        nếu (!a)
                trả_về 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        cho (; *a; a++) {
                char **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                nếu ((*a)[n] == '=')
                        n++;

                cho (j = e; j < *k; j++)
                        nếu (strneq(*j, *a, n))
                                break;

                c = strdup(*a);
                nếu (!c)
                        trả_về -ENOMEM;

                nếu (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = NULL;
                        (*k)++;
                } không_thì
                        free_and_replace(*j, c); /* Override existing item */
        }

        trả_về 0;
}

char** _strv_env_merge(char **first, ...) {
        _cleanup_strv_free_ char **merged = NULL;
        char **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        cho (;;) {
                char **l;

                l = va_arg(ap, char**);
                nếu (l == POINTER_MAX)
                        break;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(char*, n + 1);
        nếu (!merged)
                trả_về NULL;
        merged[0] = NULL;

        nếu (env_append(merged, &k, first) < 0)
                trả_về NULL;

        va_start(ap, first);
        cho (;;) {
                char **l;

                l = va_arg(ap, char**);
                nếu (l == POINTER_MAX)
                        break;

                nếu (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        trả_về NULL;
                }
        }
        va_end(ap);

        trả_về TAKE_PTR(merged);
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

        nếu (streq(t, pattern))
                trả_về true;

        nếu (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                trả_về t && *t == '=';
        }

        trả_về false;
}

static bool env_entry_has_name(const char *entry, const char *name) {
        const char *t;

        assert(entry);
        assert(name);

        t = startswith(entry, name);
        nếu (!t)
                trả_về false;

        trả_về *t == '=';
}

char** strv_env_delete(char **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ char **t = NULL;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(char*, n+1);
        nếu (!t)
                trả_về NULL;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                cho (size_t v = 0; v < n_lists; v++) {
                        char **l;

                        l = va_arg(ap, char**);
                        STRV_FOREACH(j, l)
                                nếu (env_match(*k, *j))
                                        goto skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                nếu (!t[i])
                        trả_về NULL;

                i++;
                continue;

        skip:
                va_end(ap);
        }

        t[i] = NULL;

        assert(i <= n);

        trả_về TAKE_PTR(t);
}

char** strv_env_unset(char **l, const char *p) {
        assert(p);

        nếu (!l)
                trả_về NULL;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        char **f, **t;
        cho (f = t = l; *f; f++) {
                nếu (env_match(*f, p)) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        trả_về l;
}

char** strv_env_unset_many_internal(char **l, ...) {
        nếu (!l)
                trả_về NULL;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        char **f, **t;
        cho (f = t = l; *f; f++) {
                bool found = false;
                const char *p;
                va_list ap;

                va_start(ap, l);

                trong_khi ((p = va_arg(ap, const char*)))
                        nếu (env_match(*f, p)) {
                                found = true;
                                break;
                        }

                va_end(ap);

                nếu (found) {
                        free(*f);
                        continue;
                }

                *(t++) = *f;
        }

        *t = NULL;
        trả_về l;
}

số_nguyên strv_env_replace_consume(char ***l, char *p) {
        const char *t, *name;
        số_nguyên r;

        assert(l);
        assert(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        nếu (!t) {
                free(p);
                trả_về -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                nếu (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        trả_về 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        nếu (r < 0)
                trả_về r;

        trả_về 1;
}

số_nguyên strv_env_replace_strdup(char ***l, const char *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        assert(l);
        assert(assignment);

        char *p = strdup(assignment);
        nếu (!p)
                trả_về -ENOMEM;

        trả_về strv_env_replace_consume(l, p);
}

số_nguyên strv_env_replace_strdup_passthrough(char ***l, const char *assignment) {
        char *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        assert(l);
        assert(assignment);

        nếu (strchr(assignment, '=')) {
                nếu (!env_assignment_is_valid(assignment))
                        trả_về -EINVAL;

                p = strdup(assignment);
        } không_thì {
                nếu (!env_name_is_valid(assignment))
                        trả_về -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        nếu (!p)
                trả_về -ENOMEM;

        trả_về strv_env_replace_consume(l, p);
}

số_nguyên strv_env_assign(char ***l, const char *key, const char *value) {
        assert(l);

        nếu (!env_name_is_valid(key))
                trả_về -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        nếu (!value) {
                strv_env_unset(*l, key);
                trả_về 0;
        }

        char *p = strjoin(key, "=", value);
        nếu (!p)
                trả_về -ENOMEM;

        trả_về strv_env_replace_consume(l, p);
}

số_nguyên strv_env_assignf(char ***l, const char *key, const char *valuef, ...) {
        số_nguyên r;

        assert(l);
        assert(key);

        nếu (!env_name_is_valid(key))
                trả_về -EINVAL;

        nếu (!valuef) {
                strv_env_unset(*l, key);
                trả_về 0;
        }

        _cleanup_free_ char *value = NULL;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);
        nếu (r < 0)
                trả_về -ENOMEM;

        char *p = strjoin(key, "=", value);
        nếu (!p)
                trả_về -ENOMEM;

        trả_về strv_env_replace_consume(l, p);
}

số_nguyên _strv_env_assign_many(char ***l, ...) {
        va_list ap;
        số_nguyên r;

        assert(l);

        va_start(ap, l);
        cho (;;) {
                const char *key, *value;

                key = va_arg(ap, const char *);
                nếu (!key)
                        break;

                nếu (!env_name_is_valid(key)) {
                        va_end(ap);
                        trả_về -EINVAL;
                }

                value = va_arg(ap, const char *);
                nếu (!value) {
                        strv_env_unset(*l, key);
                        continue;
                }

                char *p = strjoin(key, "=", value);
                nếu (!p) {
                        va_end(ap);
                        trả_về -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                nếu (r < 0) {
                        va_end(ap);
                        trả_về r;
                }
        }
        va_end(ap);

        trả_về 0;
}

char* strv_env_get_n(char * const *l, const char *name, size_t k, ReplaceEnvFlags flags) {
        assert(name);

        nếu (k == SIZE_MAX)
                k = strlen(name);
        nếu (k <= 0)
                trả_về NULL;

        STRV_FOREACH_BACKWARDS(i, l)
                nếu (strneq(*i, name, k) && (*i)[k] == '=')
                        trả_về (char*) *i + k + 1;

        nếu (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                const char *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                nếu (k > sc_arg_max() - 2)
                        trả_về NULL;

                t = strndupa_safe(name, k);
                trả_về secure_getenv(t);
        };

        trả_về NULL;
}

char* strv_env_pairs_get(char **l, const char *name) {
        char *result = NULL;

        assert(name);

        STRV_FOREACH_PAIR(key, value, l)
                nếu (streq(*key, name))
                        result = *value;

        trả_về result;
}

số_nguyên strv_env_get_merged(char **l, char ***ret) {
        _cleanup_strv_free_ char **v = NULL;
        size_t n = 0;
        số_nguyên r;

        assert(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(key, value, l) {
                char *s;

                s = strjoin(*key, "=", *value);
                nếu (!s)
                        trả_về -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                nếu (r < 0)
                        trả_về r;
        }

        *ret = TAKE_PTR(v);
        trả_về 0;
}

char** strv_env_clean_with_callback(char **l, void (*invalid_callback)(const char *p, void *userdata), void *userdata) {
        số_nguyên k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                nếu (!env_assignment_is_valid(*p)) {
                        nếu (invalid_callback)
                                invalid_callback(*p, userdata);
                        free(*p);
                        continue;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        nếu (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                break;
                        }

                nếu (duplicate) {
                        free(*p);
                        continue;
                }

                l[k++] = *p;
        }

        nếu (l)
                l[k] = NULL;

        trả_về l;
}

static số_nguyên strv_extend_with_length(char ***l, const char *s, size_t n) {
        char *c;

        c = strndup(s, n);
        nếu (!c)
                trả_về -ENOMEM;

        trả_về strv_consume(l, c);
}

static số_nguyên strv_env_get_n_validated(
                char **env,
                const char *name,
                size_t l,
                ReplaceEnvFlags flags,
                char **ret,              /* points into the env block! do not free! */
                char ***unset_variables, /* updated in place */
                char ***bad_variables) { /* ditto */

        char *e;
        số_nguyên r;

        assert(l == 0 || name);
        assert(ret);

        nếu (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                nếu (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        nếu (r < 0)
                                trả_về r;
                }
        } không_thì {
                e = NULL; /* Resolve invalid variable names the same way as unset ones */

                nếu (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        nếu (r < 0)
                                trả_về r;
                }
        }

        *ret = e;
        trả_về !!e;
}

số_nguyên replace_env_full(
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
        số_nguyên nest = 0, r;

        assert(format);

        nếu (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : NULL;
        pb = ret_bad_variables ? &bad_variables : NULL;

        cho (e = format, i = 0; *e && i < length; e++, i++)
                switch (state) {

                case WORD:
                        nếu (*e == '$')
                                state = CURLY;
                        break;

                case CURLY:
                        nếu (*e == '{') {
                                nếu (!strextendn(&s, word, e-word-1))
                                        trả_về -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } không_thì nếu (*e == '$') {
                                nếu (!strextendn(&s, word, e-word))
                                        trả_về -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } không_thì nếu (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                nếu (!strextendn(&s, word, e-word-1))
                                        trả_về -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } không_thì
                                state = WORD;
                        break;

                case VARIABLE:
                        nếu (*e == '}') {
                                char *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                nếu (r < 0)
                                        trả_về r;

                                nếu (!strextend(&s, t))
                                        trả_về -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } không_thì nếu (*e == ':') {
                                nếu (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } không_thì
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        break;

                case TEST:
                        nếu (*e == '-')
                                state = DEFAULT_VALUE;
                        không_thì nếu (*e == '+')
                                state = ALTERNATE_VALUE;
                        không_thì {
                                state = WORD;
                                break;
                        }

                        test_value = e+1;
                        break;

                case DEFAULT_VALUE: /* fall through */
                case ALTERNATE_VALUE:
                        assert(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        nếu (*e == '{') {
                                nest++;
                                break;
                        }

                        nếu (*e != '}')
                                break;

                        nest--;
                        nếu (nest == 0) {
                                _cleanup_strv_free_ char **u = NULL, **b = NULL;
                                _cleanup_free_ char *v = NULL;
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                nếu (r < 0)
                                        trả_về r;

                                nếu (t && state == ALTERNATE_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        nếu (r < 0)
                                                trả_về r;

                                        t = v;
                                } không_thì nếu (!t && state == DEFAULT_VALUE) {
                                        r = replace_env_full(test_value, e-test_value, env, flags, &v, pu ? &u : NULL, pb ? &b : NULL);
                                        nếu (r < 0)
                                                trả_về r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                nếu (r < 0)
                                        trả_về r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                nếu (r < 0)
                                        trả_về r;

                                nếu (!strextend(&s, t))
                                        trả_về -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        break;

                case VARIABLE_RAW:
                        assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        nếu (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                char *t = NULL;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                nếu (r < 0)
                                        trả_về r;

                                nếu (!strextend(&s, t))
                                        trả_về -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        break;
                }

        nếu (state == VARIABLE_RAW) {
                char *t;

                assert(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                nếu (r < 0)
                        trả_về r;

                nếu (!strextend(&s, t))
                        trả_về -ENOMEM;

        } không_thì nếu (!strextendn(&s, word, e-word))
                trả_về -ENOMEM;

        nếu (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        nếu (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        nếu (ret)
                *ret = TAKE_PTR(s);

        trả_về 0;
}

số_nguyên replace_env_argv(
                char **argv,
                char **env,
                char ***ret,
                char ***ret_unset_variables,
                char ***ret_bad_variables) {

        _cleanup_strv_free_ char **n = NULL, **unset_variables = NULL, **bad_variables = NULL;
        size_t k = 0, l;
        số_nguyên r;

        assert(!strv_isempty(argv));
        assert(ret);

        l = strv_length(argv);

        n = new(char*, l+1);
        nếu (!n)
                trả_về -ENOMEM;

        STRV_FOREACH(i, argv) {
                const char *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                nếu (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ char **m = NULL;
                        const char *name = word + 1;
                        char *e, **w;
                        size_t q;

                        nếu (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                nếu (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                không_thì nếu (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                không_thì
                                        r = 0;
                        } không_thì nếu (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        không_thì
                                r = 0;
                        nếu (r < 0)
                                trả_về r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, sizeof(char*));
                        nếu (!w)
                                trả_về -ENOMEM;

                        n = w;
                        nếu (m) {
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
                nếu (r < 0)
                        trả_về r;
                n[++k] = NULL;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                nếu (r < 0)
                        trả_về r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        nếu (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        trả_về 0;
}

số_nguyên getenv_bool(const char *p) {
        const char *e;

        e = getenv(p);
        nếu (!e)
                trả_về -ENXIO;

        trả_về parse_boolean(e);
}

số_nguyên secure_getenv_bool(const char *p) {
        const char *e;

        e = secure_getenv(p);
        nếu (!e)
                trả_về -ENXIO;

        trả_về parse_boolean(e);
}

số_nguyên secure_getenv_uint64(const char *p, uint64_t *ret) {
        const char *e;

        assert(p);

        e = secure_getenv(p);
        nếu (!e)
                trả_về -ENXIO;

        trả_về safe_atou64(e, ret);
}

số_nguyên set_unset_env(const char *name, const char *value, bool overwrite) {
        assert(name);

        nếu (value)
                trả_về RET_NERRNO(setenv(name, value, overwrite));

        trả_về RET_NERRNO(unsetenv(name));
}

số_nguyên putenv_dup(const char *assignment, bool override) {
        const char *e, *n;

        e = strchr(assignment, '=');
        nếu (!e)
                trả_về -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        trả_về RET_NERRNO(setenv(n, e + 1, override));
}

số_nguyên setenv_systemd_exec_pid(bool update_only) {
        const char *e;
        số_nguyên r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        nếu (!e && update_only)
                trả_về 0;

        nếu (streq_ptr(e, "*"))
                trả_về 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        nếu (r < 0)
                trả_về r;

        trả_về 1;
}

số_nguyên setenv_systemd_log_level(void) {
        _cleanup_free_ char *val = NULL;
        số_nguyên r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        nếu (r < 0)
                trả_về r;

        trả_về RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

số_nguyên getenv_path_list(const char *name, char ***ret_paths) {
        _cleanup_strv_free_ char **l = NULL;
        const char *e;
        số_nguyên r;

        assert(name);
        assert(ret_paths);

        e = secure_getenv(name);
        nếu (!e)
                trả_về -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                nếu (!path_is_absolute(*p))
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                nếu (!path_is_normalized(*p))
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                nếu (path_equal(*p, "/"))
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        nếu (strv_isempty(l))
                trả_về log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        trả_về 1;
}

số_nguyên getenv_steal_erase(const char *name, char **ret) {
        _cleanup_(erase_and_freep) char *a = NULL;
        char *e;

        assert(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        nếu (!e) {
                nếu (ret)
                        *ret = NULL;
                trả_về 0;
        }

        nếu (ret) {
                a = strdup(e);
                nếu (!a)
                        trả_về -ENOMEM;
        }

        string_erase(e);

        nếu (unsetenv(name) < 0)
                trả_về -errno;

        nếu (ret)
                *ret = TAKE_PTR(a);

        trả_về 1;
}

số_nguyên setenvf(const char *name, bool overwrite, const char *valuef, ...) {
        _cleanup_free_ char *value = NULL;
        va_list ap;
        số_nguyên r;

        assert(name);

        nếu (!valuef)
                trả_về RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&value, valuef, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        nếu (streq_ptr(getenv(name), value))
                trả_về 0;

        trả_về RET_NERRNO(setenv(name, value, overwrite));
}
