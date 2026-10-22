/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "env-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "syslog-util.h"
#ضمن "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(فراغ) {
        long l = sysconf(_SC_ARG_MAX);
        أكد(l > 0);
        أرجع (size_t) l;
}

ساكن bool env_name_is_valid_n(ثابت حرف *e, size_t n) {

        إذا (n == SIZE_MAX)
                n = strlen_ptr(e);

        إذا (n == 0)
                أرجع false;

        أكد(e);

        إذا (ascii_isdigit(e[0]))
                أرجع false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        إذا (_unlikely_(n > sc_arg_max() - 2))
                أرجع false;

        لكل (ثابت حرف *p = e; p < e + n; p++)
                إذا (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        أرجع false;

        أرجع true;
}

bool env_name_is_valid(ثابت حرف *e) {
        أرجع env_name_is_valid_n(e, SIZE_MAX);
}

bool env_قدر_is_valid(ثابت حرف *e) {
        إذا (!e)
                أرجع false;

        إذا (!utf8_is_valid(e))
                أرجع false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        إذا (_unlikely_(strlen(e) > sc_arg_max() - 3))
                أرجع false;

        أرجع true;
}

bool env_assignment_is_valid(ثابت حرف *e) {
        ثابت حرف *eq;

        أكد(e);

        eq = strchr(e, '=');
        إذا (!eq)
                أرجع false;

        إذا (!env_name_is_valid_n(e, eq - e))
                أرجع false;

        إذا (!env_قدر_is_valid(eq + 1))
                أرجع false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        إذا (_unlikely_(strlen(e) > sc_arg_max() - 1))
                أرجع false;

        أرجع true;
}

bool strv_env_is_valid(حرف * ثابت *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                إذا (!env_assignment_is_valid(*p))
                        أرجع false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        إذا (strneq(*p, *q, k) && (*q)[k] == '=')
                                أرجع false;
        }

        أرجع true;
}

bool strv_env_name_is_valid(حرف * ثابت *l) {
        STRV_FOREACH(p, l) {
                إذا (!env_name_is_valid(*p))
                        أرجع false;

                إذا (strv_contains(p + 1, *p))
                        أرجع false;
        }

        أرجع true;
}

bool strv_env_name_or_assignment_is_valid(حرف * ثابت *l) {
        STRV_FOREACH(p, l) {
                إذا (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        أرجع false;

                إذا (strv_contains(p + 1, *p))
                        أرجع false;
        }

        أرجع true;
}

ساكن صحيح env_append(حرف **e, حرف ***k, حرف **a) {
        أكد(e);
        أكد(k);
        أكد(*k >= e);

        إذا (!a)
                أرجع 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        لكل (; *a; a++) {
                حرف **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                إذا ((*a)[n] == '=')
                        n++;

                لكل (j = e; j < *k; j++)
                        إذا (strneq(*j, *a, n))
                                اكسر;

                c = strdup(*a);
                إذا (!c)
                        أرجع -ENOMEM;

                إذا (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = عدم;
                        (*k)++;
                } وإلا
                        free_and_replace(*j, c); /* Override existing item */
        }

        أرجع 0;
}

حرف** _strv_env_merge(حرف **first, ...) {
        _cleanup_strv_free_ حرف **merged = عدم;
        حرف **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        لكل (;;) {
                حرف **l;

                l = va_arg(ap, حرف**);
                إذا (l == POINTER_MAX)
                        اكسر;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(حرف*, n + 1);
        إذا (!merged)
                أرجع عدم;
        merged[0] = عدم;

        إذا (env_append(merged, &k, first) < 0)
                أرجع عدم;

        va_start(ap, first);
        لكل (;;) {
                حرف **l;

                l = va_arg(ap, حرف**);
                إذا (l == POINTER_MAX)
                        اكسر;

                إذا (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        أرجع عدم;
                }
        }
        va_end(ap);

        أرجع TAKE_PTR(merged);
}

ساكن bool env_match(ثابت حرف *t, ثابت حرف *pattern) {
        أكد(t);
        أكد(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        إذا (streq(t, pattern))
                أرجع true;

        إذا (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                أرجع t && *t == '=';
        }

        أرجع false;
}

ساكن bool env_entry_has_name(ثابت حرف *entry, ثابت حرف *name) {
        ثابت حرف *t;

        أكد(entry);
        أكد(name);

        t = startswith(entry, name);
        إذا (!t)
                أرجع false;

        أرجع *t == '=';
}

حرف** strv_env_delete(حرف **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ حرف **t = عدم;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(حرف*, n+1);
        إذا (!t)
                أرجع عدم;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                لكل (size_t v = 0; v < n_lists; v++) {
                        حرف **l;

                        l = va_arg(ap, حرف**);
                        STRV_FOREACH(j, l)
                                إذا (env_match(*k, *j))
                                        اذهب skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                إذا (!t[i])
                        أرجع عدم;

                i++;
                استمر;

        skip:
                va_end(ap);
        }

        t[i] = عدم;

        أكد(i <= n);

        أرجع TAKE_PTR(t);
}

حرف** strv_env_unset(حرف **l, ثابت حرف *p) {
        أكد(p);

        إذا (!l)
                أرجع عدم;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        حرف **f, **t;
        لكل (f = t = l; *f; f++) {
                إذا (env_match(*f, p)) {
                        حرر(*f);
                        استمر;
                }

                *(t++) = *f;
        }

        *t = عدم;
        أرجع l;
}

حرف** strv_env_unset_many_internal(حرف **l, ...) {
        إذا (!l)
                أرجع عدم;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        حرف **f, **t;
        لكل (f = t = l; *f; f++) {
                bool found = false;
                ثابت حرف *p;
                va_list ap;

                va_start(ap, l);

                طالما ((p = va_arg(ap, ثابت حرف*)))
                        إذا (env_match(*f, p)) {
                                found = true;
                                اكسر;
                        }

                va_end(ap);

                إذا (found) {
                        حرر(*f);
                        استمر;
                }

                *(t++) = *f;
        }

        *t = عدم;
        أرجع l;
}

صحيح strv_env_replace_consume(حرف ***l, حرف *p) {
        ثابت حرف *t, *name;
        صحيح r;

        أكد(l);
        أكد(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        إذا (!t) {
                حرر(p);
                أرجع -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                إذا (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        أرجع 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

صحيح strv_env_replace_strdup(حرف ***l, ثابت حرف *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        أكد(l);
        أكد(assignment);

        حرف *p = strdup(assignment);
        إذا (!p)
                أرجع -ENOMEM;

        أرجع strv_env_replace_consume(l, p);
}

صحيح strv_env_replace_strdup_passthrough(حرف ***l, ثابت حرف *assignment) {
        حرف *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        أكد(l);
        أكد(assignment);

        إذا (strchr(assignment, '=')) {
                إذا (!env_assignment_is_valid(assignment))
                        أرجع -EINVAL;

                p = strdup(assignment);
        } وإلا {
                إذا (!env_name_is_valid(assignment))
                        أرجع -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        إذا (!p)
                أرجع -ENOMEM;

        أرجع strv_env_replace_consume(l, p);
}

صحيح strv_env_assign(حرف ***l, ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(l);

        إذا (!env_name_is_valid(کلید))
                أرجع -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        إذا (!قدر) {
                strv_env_unset(*l, کلید);
                أرجع 0;
        }

        حرف *p = strjoin(کلید, "=", قدر);
        إذا (!p)
                أرجع -ENOMEM;

        أرجع strv_env_replace_consume(l, p);
}

صحيح strv_env_assignf(حرف ***l, ثابت حرف *کلید, ثابت حرف *valuef, ...) {
        صحيح r;

        أكد(l);
        أكد(کلید);

        إذا (!env_name_is_valid(کلید))
                أرجع -EINVAL;

        إذا (!valuef) {
                strv_env_unset(*l, کلید);
                أرجع 0;
        }

        _cleanup_free_ حرف *قدر = عدم;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&قدر, valuef, ap);
        va_end(ap);
        إذا (r < 0)
                أرجع -ENOMEM;

        حرف *p = strjoin(کلید, "=", قدر);
        إذا (!p)
                أرجع -ENOMEM;

        أرجع strv_env_replace_consume(l, p);
}

صحيح _strv_env_assign_many(حرف ***l, ...) {
        va_list ap;
        صحيح r;

        أكد(l);

        va_start(ap, l);
        لكل (;;) {
                ثابت حرف *کلید, *قدر;

                کلید = va_arg(ap, ثابت حرف *);
                إذا (!کلید)
                        اكسر;

                إذا (!env_name_is_valid(کلید)) {
                        va_end(ap);
                        أرجع -EINVAL;
                }

                قدر = va_arg(ap, ثابت حرف *);
                إذا (!قدر) {
                        strv_env_unset(*l, کلید);
                        استمر;
                }

                حرف *p = strjoin(کلید, "=", قدر);
                إذا (!p) {
                        va_end(ap);
                        أرجع -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                إذا (r < 0) {
                        va_end(ap);
                        أرجع r;
                }
        }
        va_end(ap);

        أرجع 0;
}

حرف* strv_env_get_n(حرف * ثابت *l, ثابت حرف *name, size_t k, ReplaceEnvFlags flags) {
        أكد(name);

        إذا (k == SIZE_MAX)
                k = strlen(name);
        إذا (k <= 0)
                أرجع عدم;

        STRV_FOREACH_BACKWARDS(i, l)
                إذا (strneq(*i, name, k) && (*i)[k] == '=')
                        أرجع (حرف*) *i + k + 1;

        إذا (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ثابت حرف *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                إذا (k > sc_arg_max() - 2)
                        أرجع عدم;

                t = strndupa_safe(name, k);
                أرجع secure_getenv(t);
        };

        أرجع عدم;
}

حرف* strv_env_pairs_get(حرف **l, ثابت حرف *name) {
        حرف *result = عدم;

        أكد(name);

        STRV_FOREACH_PAIR(کلید, قدر, l)
                إذا (streq(*کلید, name))
                        result = *قدر;

        أرجع result;
}

صحيح strv_env_get_merged(حرف **l, حرف ***ret) {
        _cleanup_strv_free_ حرف **v = عدم;
        size_t n = 0;
        صحيح r;

        أكد(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(کلید, قدر, l) {
                حرف *s;

                s = strjoin(*کلید, "=", *قدر);
                إذا (!s)
                        أرجع -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(v);
        أرجع 0;
}

حرف** strv_env_clean_with_callback(حرف **l, فراغ (*invalid_callback)(ثابت حرف *p, فراغ *userdata), فراغ *userdata) {
        صحيح k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                إذا (!env_assignment_is_valid(*p)) {
                        إذا (invalid_callback)
                                invalid_callback(*p, userdata);
                        حرر(*p);
                        استمر;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        إذا (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                اكسر;
                        }

                إذا (duplicate) {
                        حرر(*p);
                        استمر;
                }

                l[k++] = *p;
        }

        إذا (l)
                l[k] = عدم;

        أرجع l;
}

ساكن صحيح strv_extend_with_length(حرف ***l, ثابت حرف *s, size_t n) {
        حرف *c;

        c = strndup(s, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع strv_consume(l, c);
}

ساكن صحيح strv_env_get_n_validated(
                حرف **env,
                ثابت حرف *name,
                size_t l,
                ReplaceEnvFlags flags,
                حرف **ret,              /* points into the env block! do not free! */
                حرف ***unset_variables, /* updated in place */
                حرف ***bad_variables) { /* ditto */

        حرف *e;
        صحيح r;

        أكد(l == 0 || name);
        أكد(ret);

        إذا (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                إذا (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        إذا (r < 0)
                                أرجع r;
                }
        } وإلا {
                e = عدم; /* Resolve invalid variable names the same way as unset ones */

                إذا (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        *ret = e;
        أرجع !!e;
}

صحيح replace_env_full(
                ثابت حرف *format,
                size_t length,
                حرف **env,
                ReplaceEnvFlags flags,
                حرف **ret,
                حرف ***ret_unset_variables,
                حرف ***ret_bad_variables) {

        تعداد {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_قدر,
                ALTERNATE_قدر,
        } state = WORD;

        _cleanup_strv_free_ حرف **unset_variables = عدم, **bad_variables = عدم;
        ثابت حرف *e, *word = format, *test_قدر = عدم; /* test_value is initialized to appease gcc */
        _cleanup_free_ حرف *s = عدم;
        حرف ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        صحيح nest = 0, r;

        أكد(format);

        إذا (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : عدم;
        pb = ret_bad_variables ? &bad_variables : عدم;

        لكل (e = format, i = 0; *e && i < length; e++, i++)
                بدل (state) {

                حالة WORD:
                        إذا (*e == '$')
                                state = CURLY;
                        اكسر;

                حالة CURLY:
                        إذا (*e == '{') {
                                إذا (!strextendn(&s, word, e-word-1))
                                        أرجع -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } وإلا إذا (*e == '$') {
                                إذا (!strextendn(&s, word, e-word))
                                        أرجع -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } وإلا إذا (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                إذا (!strextendn(&s, word, e-word-1))
                                        أرجع -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } وإلا
                                state = WORD;
                        اكسر;

                حالة VARIABLE:
                        إذا (*e == '}') {
                                حرف *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!strextend(&s, t))
                                        أرجع -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } وإلا إذا (*e == ':') {
                                إذا (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } وإلا
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        اكسر;

                حالة TEST:
                        إذا (*e == '-')
                                state = DEFAULT_قدر;
                        وإلا إذا (*e == '+')
                                state = ALTERNATE_قدر;
                        وإلا {
                                state = WORD;
                                اكسر;
                        }

                        test_قدر = e+1;
                        اكسر;

                حالة DEFAULT_قدر: /* fall through */
                حالة ALTERNATE_قدر:
                        أكد(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        إذا (*e == '{') {
                                nest++;
                                اكسر;
                        }

                        إذا (*e != '}')
                                اكسر;

                        nest--;
                        إذا (nest == 0) {
                                _cleanup_strv_free_ حرف **u = عدم, **b = عدم;
                                _cleanup_free_ حرف *v = عدم;
                                حرف *t = عدم;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (t && state == ALTERNATE_قدر) {
                                        r = replace_env_full(test_قدر, e-test_قدر, env, flags, &v, pu ? &u : عدم, pb ? &b : عدم);
                                        إذا (r < 0)
                                                أرجع r;

                                        t = v;
                                } وإلا إذا (!t && state == DEFAULT_قدر) {
                                        r = replace_env_full(test_قدر, e-test_قدر, env, flags, &v, pu ? &u : عدم, pb ? &b : عدم);
                                        إذا (r < 0)
                                                أرجع r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                إذا (r < 0)
                                        أرجع r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!strextend(&s, t))
                                        أرجع -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        اكسر;

                حالة VARIABLE_RAW:
                        أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        إذا (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                حرف *t = عدم;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                إذا (r < 0)
                                        أرجع r;

                                إذا (!strextend(&s, t))
                                        أرجع -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        اكسر;
                }

        إذا (state == VARIABLE_RAW) {
                حرف *t;

                أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                إذا (r < 0)
                        أرجع r;

                إذا (!strextend(&s, t))
                        أرجع -ENOMEM;

        } وإلا إذا (!strextendn(&s, word, e-word))
                أرجع -ENOMEM;

        إذا (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        إذا (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        إذا (ret)
                *ret = TAKE_PTR(s);

        أرجع 0;
}

صحيح replace_env_argv(
                حرف **argv,
                حرف **env,
                حرف ***ret,
                حرف ***ret_unset_variables,
                حرف ***ret_bad_variables) {

        _cleanup_strv_free_ حرف **n = عدم, **unset_variables = عدم, **bad_variables = عدم;
        size_t k = 0, l;
        صحيح r;

        أكد(!strv_isempty(argv));
        أكد(ret);

        l = strv_length(argv);

        n = new(حرف*, l+1);
        إذا (!n)
                أرجع -ENOMEM;

        STRV_FOREACH(i, argv) {
                ثابت حرف *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                إذا (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ حرف **m = عدم;
                        ثابت حرف *name = word + 1;
                        حرف *e, **w;
                        size_t q;

                        إذا (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                إذا (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                وإلا إذا (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                وإلا
                                        r = 0;
                        } وإلا إذا (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        وإلا
                                r = 0;
                        إذا (r < 0)
                                أرجع r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, حجم(حرف*));
                        إذا (!w)
                                أرجع -ENOMEM;

                        n = w;
                        إذا (m) {
                                memcpy(n + k, m, (q + 1) * حجم(حرف*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = عدم;
                        استمر;
                }

                _cleanup_strv_free_ حرف **u = عدم, **b = عدم;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : عدم,
                                ret_bad_variables ? &b : عدم);
                إذا (r < 0)
                        أرجع r;
                n[++k] = عدم;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                إذا (r < 0)
                        أرجع r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        إذا (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        أرجع 0;
}

صحيح getenv_bool(ثابت حرف *p) {
        ثابت حرف *e;

        e = getenv(p);
        إذا (!e)
                أرجع -ENXIO;

        أرجع parse_boolean(e);
}

صحيح secure_getenv_bool(ثابت حرف *p) {
        ثابت حرف *e;

        e = secure_getenv(p);
        إذا (!e)
                أرجع -ENXIO;

        أرجع parse_boolean(e);
}

صحيح secure_getenv_uint64(ثابت حرف *p, uint64_t *ret) {
        ثابت حرف *e;

        أكد(p);

        e = secure_getenv(p);
        إذا (!e)
                أرجع -ENXIO;

        أرجع safe_atou64(e, ret);
}

صحيح set_unset_env(ثابت حرف *name, ثابت حرف *قدر, bool overwrite) {
        أكد(name);

        إذا (قدر)
                أرجع RET_NERRNO(setenv(name, قدر, overwrite));

        أرجع RET_NERRNO(unsetenv(name));
}

صحيح putenv_dup(ثابت حرف *assignment, bool override) {
        ثابت حرف *e, *n;

        e = strchr(assignment, '=');
        إذا (!e)
                أرجع -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        أرجع RET_NERRNO(setenv(n, e + 1, override));
}

صحيح setenv_systemd_exec_pid(bool update_only) {
        ثابت حرف *e;
        صحيح r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        إذا (!e && update_only)
                أرجع 0;

        إذا (streq_ptr(e, "*"))
                أرجع 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

صحيح setenv_systemd_log_level(فراغ) {
        _cleanup_free_ حرف *val = عدم;
        صحيح r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        إذا (r < 0)
                أرجع r;

        أرجع RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

صحيح getenv_path_list(ثابت حرف *name, حرف ***ret_paths) {
        _cleanup_strv_free_ حرف **l = عدم;
        ثابت حرف *e;
        صحيح r;

        أكد(name);
        أكد(ret_paths);

        e = secure_getenv(name);
        إذا (!e)
                أرجع -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                إذا (!path_is_absolute(*p))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                إذا (!path_is_normalized(*p))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                إذا (path_equal(*p, "/"))
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        إذا (strv_isempty(l))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        أرجع 1;
}

صحيح getenv_steal_erase(ثابت حرف *name, حرف **ret) {
        _cleanup_(erase_and_freep) حرف *a = عدم;
        حرف *e;

        أكد(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        إذا (!e) {
                إذا (ret)
                        *ret = عدم;
                أرجع 0;
        }

        إذا (ret) {
                a = strdup(e);
                إذا (!a)
                        أرجع -ENOMEM;
        }

        string_erase(e);

        إذا (unsetenv(name) < 0)
                أرجع -errno;

        إذا (ret)
                *ret = TAKE_PTR(a);

        أرجع 1;
}

صحيح setenvf(ثابت حرف *name, bool overwrite, ثابت حرف *valuef, ...) {
        _cleanup_free_ حرف *قدر = عدم;
        va_list ap;
        صحيح r;

        أكد(name);

        إذا (!valuef)
                أرجع RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&قدر, valuef, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        إذا (streq_ptr(getenv(name), قدر))
                أرجع 0;

        أرجع RET_NERRNO(setenv(name, قدر, overwrite));
}
