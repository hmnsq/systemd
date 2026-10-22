/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "format-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "syslog-util.h"
#شامل "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#تعریف_کرو VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(خالی) {
        طویل l = sysconf(_SC_ARG_MAX);
        أكد(l > 0);
        واپس (size_t) l;
}

ساکن bool env_name_is_valid_n(ثابت حرف *e, size_t n) {

        اگر (n == SIZE_MAX)
                n = strlen_ptr(e);

        اگر (n == 0)
                واپس false;

        أكد(e);

        اگر (ascii_isdigit(e[0]))
                واپس false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        اگر (_unlikely_(n > sc_arg_max() - 2))
                واپس false;

        برائے (ثابت حرف *p = e; p < e + n; p++)
                اگر (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        واپس false;

        واپس true;
}

bool env_name_is_valid(ثابت حرف *e) {
        واپس env_name_is_valid_n(e, SIZE_MAX);
}

bool env_قدر_is_valid(ثابت حرف *e) {
        اگر (!e)
                واپس false;

        اگر (!utf8_is_valid(e))
                واپس false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        اگر (_unlikely_(لڑی_لمبائی(e) > sc_arg_max() - 3))
                واپس false;

        واپس true;
}

bool env_assignment_is_valid(ثابت حرف *e) {
        ثابت حرف *eq;

        أكد(e);

        eq = strchr(e, '=');
        اگر (!eq)
                واپس false;

        اگر (!env_name_is_valid_n(e, eq - e))
                واپس false;

        اگر (!env_قدر_is_valid(eq + 1))
                واپس false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        اگر (_unlikely_(لڑی_لمبائی(e) > sc_arg_max() - 1))
                واپس false;

        واپس true;
}

bool strv_env_is_valid(حرف * ثابت *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                اگر (!env_assignment_is_valid(*p))
                        واپس false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        اگر (strneq(*p, *q, k) && (*q)[k] == '=')
                                واپس false;
        }

        واپس true;
}

bool strv_env_name_is_valid(حرف * ثابت *l) {
        STRV_FOREACH(p, l) {
                اگر (!env_name_is_valid(*p))
                        واپس false;

                اگر (strv_شامل_ہے(p + 1, *p))
                        واپس false;
        }

        واپس true;
}

bool strv_env_name_or_assignment_is_valid(حرف * ثابت *l) {
        STRV_FOREACH(p, l) {
                اگر (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        واپس false;

                اگر (strv_شامل_ہے(p + 1, *p))
                        واپس false;
        }

        واپس true;
}

ساکن صحیح env_append(حرف **e, حرف ***k, حرف **a) {
        أكد(e);
        أكد(k);
        أكد(*k >= e);

        اگر (!a)
                واپس 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        برائے (; *a; a++) {
                حرف **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                اگر ((*a)[n] == '=')
                        n++;

                برائے (j = e; j < *k; j++)
                        اگر (strneq(*j, *a, n))
                                توڑو;

                c = strdup(*a);
                اگر (!c)
                        واپس -ENOMEM;

                اگر (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = تہی;
                        (*k)++;
                } ورنہ
                        free_and_replace(*j, c); /* Override existing item */
        }

        واپس 0;
}

حرف** strv_env_ملاؤ(حرف **پہلا, ...) {
        _cleanup_strv_free_ حرف **merged = تہی;
        حرف **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_لمبائی(پہلا);

        va_start(ap, پہلا);
        برائے (;;) {
                حرف **l;

                l = va_arg(ap, حرف**);
                اگر (l == اشارہ_گر_MAX)
                        توڑو;

                n += strv_لمبائی(l);
        }
        va_end(ap);

        k = merged = new(حرف*, n + 1);
        اگر (!merged)
                واپس تہی;
        merged[0] = تہی;

        اگر (env_append(merged, &k, پہلا) < 0)
                واپس تہی;

        va_start(ap, پہلا);
        برائے (;;) {
                حرف **l;

                l = va_arg(ap, حرف**);
                اگر (l == اشارہ_گر_MAX)
                        توڑو;

                اگر (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        واپس تہی;
                }
        }
        va_end(ap);

        واپس TAKE_PTR(merged);
}

ساکن bool env_مطابقت(ثابت حرف *t, ثابت حرف *نمونہ) {
        أكد(t);
        أكد(نمونہ);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        اگر (streq(t, نمونہ))
                واپس true;

        اگر (!strchr(نمونہ, '=')) {
                t = startswith(t, نمونہ);

                واپس t && *t == '=';
        }

        واپس false;
}

ساکن bool env_اندراج_has_name(ثابت حرف *اندراج_2, ثابت حرف *name) {
        ثابت حرف *t;

        أكد(اندراج_2);
        أكد(name);

        t = startswith(اندراج_2, name);
        اگر (!t)
                واپس false;

        واپس *t == '=';
}

حرف** strv_env_delete(حرف **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ حرف **t = تہی;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_لمبائی(x);

        t = new(حرف*, n+1);
        اگر (!t)
                واپس تہی;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                برائے (size_t v = 0; v < n_lists; v++) {
                        حرف **l;

                        l = va_arg(ap, حرف**);
                        STRV_FOREACH(j, l)
                                اگر (env_مطابقت(*k, *j))
                                        جاؤ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                اگر (!t[i])
                        واپس تہی;

                i++;
                جاری;

        skip:
                va_end(ap);
        }

        t[i] = تہی;

        أكد(i <= n);

        واپس TAKE_PTR(t);
}

حرف** strv_env_unset(حرف **l, ثابت حرف *p) {
        أكد(p);

        اگر (!l)
                واپس تہی;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        حرف **f, **t;
        برائے (f = t = l; *f; f++) {
                اگر (env_مطابقت(*f, p)) {
                        آزاد(*f);
                        جاری;
                }

                *(t++) = *f;
        }

        *t = تہی;
        واپس l;
}

حرف** strv_env_unset_many_اندرونی(حرف **l, ...) {
        اگر (!l)
                واپس تہی;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        حرف **f, **t;
        برائے (f = t = l; *f; f++) {
                bool ملا = false;
                ثابت حرف *p;
                va_list ap;

                va_start(ap, l);

                جب ((p = va_arg(ap, ثابت حرف*)))
                        اگر (env_مطابقت(*f, p)) {
                                ملا = true;
                                توڑو;
                        }

                va_end(ap);

                اگر (ملا) {
                        آزاد(*f);
                        جاری;
                }

                *(t++) = *f;
        }

        *t = تہی;
        واپس l;
}

صحیح strv_env_replace_consume(حرف ***l, حرف *p) {
        ثابت حرف *t, *name;
        صحیح r;

        أكد(l);
        أكد(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        اگر (!t) {
                آزاد(p);
                واپس -EINVAL;
        }

        name = strndupa_محفوظ(p, t - p);

        STRV_FOREACH(f, *l)
                اگر (env_اندراج_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        واپس 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        اگر (r < 0)
                واپس r;

        واپس 1;
}

صحیح strv_env_replace_strdup(حرف ***l, ثابت حرف *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        أكد(l);
        أكد(assignment);

        حرف *p = strdup(assignment);
        اگر (!p)
                واپس -ENOMEM;

        واپس strv_env_replace_consume(l, p);
}

صحیح strv_env_replace_strdup_passthrough(حرف ***l, ثابت حرف *assignment) {
        حرف *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        أكد(l);
        أكد(assignment);

        اگر (strchr(assignment, '=')) {
                اگر (!env_assignment_is_valid(assignment))
                        واپس -EINVAL;

                p = strdup(assignment);
        } ورنہ {
                اگر (!env_name_is_valid(assignment))
                        واپس -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        اگر (!p)
                واپس -ENOMEM;

        واپس strv_env_replace_consume(l, p);
}

صحیح strv_env_تفویض(حرف ***l, ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(l);

        اگر (!env_name_is_valid(کلید))
                واپس -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        اگر (!قدر) {
                strv_env_unset(*l, کلید);
                واپس 0;
        }

        حرف *p = strjoin(کلید, "=", قدر);
        اگر (!p)
                واپس -ENOMEM;

        واپس strv_env_replace_consume(l, p);
}

صحیح strv_env_assignf(حرف ***l, ثابت حرف *کلید, ثابت حرف *valuef, ...) {
        صحیح r;

        أكد(l);
        أكد(کلید);

        اگر (!env_name_is_valid(کلید))
                واپس -EINVAL;

        اگر (!valuef) {
                strv_env_unset(*l, کلید);
                واپس 0;
        }

        _cleanup_free_ حرف *قدر = تہی;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&قدر, valuef, ap);
        va_end(ap);
        اگر (r < 0)
                واپس -ENOMEM;

        حرف *p = strjoin(کلید, "=", قدر);
        اگر (!p)
                واپس -ENOMEM;

        واپس strv_env_replace_consume(l, p);
}

صحیح strv_env_تفویض_many(حرف ***l, ...) {
        va_list ap;
        صحیح r;

        أكد(l);

        va_start(ap, l);
        برائے (;;) {
                ثابت حرف *کلید, *قدر;

                کلید = va_arg(ap, ثابت حرف *);
                اگر (!کلید)
                        توڑو;

                اگر (!env_name_is_valid(کلید)) {
                        va_end(ap);
                        واپس -EINVAL;
                }

                قدر = va_arg(ap, ثابت حرف *);
                اگر (!قدر) {
                        strv_env_unset(*l, کلید);
                        جاری;
                }

                حرف *p = strjoin(کلید, "=", قدر);
                اگر (!p) {
                        va_end(ap);
                        واپس -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                اگر (r < 0) {
                        va_end(ap);
                        واپس r;
                }
        }
        va_end(ap);

        واپس 0;
}

حرف* strv_env_get_n(حرف * ثابت *l, ثابت حرف *name, size_t k, ReplaceEnvFlags flags) {
        أكد(name);

        اگر (k == SIZE_MAX)
                k = لڑی_لمبائی(name);
        اگر (k <= 0)
                واپس تہی;

        STRV_FOREACH_BACKWARDS(i, l)
                اگر (strneq(*i, name, k) && (*i)[k] == '=')
                        واپس (حرف*) *i + k + 1;

        اگر (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ثابت حرف *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                اگر (k > sc_arg_max() - 2)
                        واپس تہی;

                t = strndupa_محفوظ(name, k);
                واپس secure_getenv(t);
        };

        واپس تہی;
}

حرف* strv_env_جوڑے_get(حرف **l, ثابت حرف *name) {
        حرف *نتیجہ = تہی;

        أكد(name);

        STRV_FOREACH_PAIR(کلید, قدر, l)
                اگر (streq(*کلید, name))
                        نتیجہ = *قدر;

        واپس نتیجہ;
}

صحیح strv_env_get_merged(حرف **l, حرف ***ret) {
        _cleanup_strv_free_ حرف **v = تہی;
        size_t n = 0;
        صحیح r;

        أكد(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(کلید, قدر, l) {
                حرف *s;

                s = strjoin(*کلید, "=", *قدر);
                اگر (!s)
                        واپس -ENOMEM;

                r = strv_consume_with_سائز(&v, &n, s);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(v);
        واپس 0;
}

حرف** strv_env_clean_with_callback(حرف **l, خالی (*invalid_callback)(ثابت حرف *p, خالی *userdata), خالی *userdata) {
        صحیح k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                اگر (!env_assignment_is_valid(*p)) {
                        اگر (invalid_callback)
                                invalid_callback(*p, userdata);
                        آزاد(*p);
                        جاری;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        اگر (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                توڑو;
                        }

                اگر (duplicate) {
                        آزاد(*p);
                        جاری;
                }

                l[k++] = *p;
        }

        اگر (l)
                l[k] = تہی;

        واپس l;
}

ساکن صحیح strv_extend_with_لمبائی(حرف ***l, ثابت حرف *s, size_t n) {
        حرف *c;

        c = strndup(s, n);
        اگر (!c)
                واپس -ENOMEM;

        واپس strv_consume(l, c);
}

ساکن صحیح strv_env_get_n_validated(
                حرف **env,
                ثابت حرف *name,
                size_t l,
                ReplaceEnvFlags flags,
                حرف **ret,              /* points into the env block! do not free! */
                حرف ***unset_variables, /* updated in place */
                حرف ***bad_variables) { /* ditto */

        حرف *e;
        صحیح r;

        أكد(l == 0 || name);
        أكد(ret);

        اگر (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                اگر (!e && unset_variables) {
                        r = strv_extend_with_لمبائی(unset_variables, name, l);
                        اگر (r < 0)
                                واپس r;
                }
        } ورنہ {
                e = تہی; /* Resolve invalid variable names the same way as unset ones */

                اگر (bad_variables) {
                        r = strv_extend_with_لمبائی(bad_variables, name, l);
                        اگر (r < 0)
                                واپس r;
                }
        }

        *ret = e;
        واپس !!e;
}

صحیح replace_env_بھرا(
                ثابت حرف *format,
                size_t length,
                حرف **env,
                ReplaceEnvFlags flags,
                حرف **ret,
                حرف ***ret_unset_variables,
                حرف ***ret_bad_variables) {

        شمار {
                لفظ_2,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_قدر,
                ALTERNATE_قدر,
        } state = لفظ_2;

        _cleanup_strv_free_ حرف **unset_variables = تہی, **bad_variables = تہی;
        ثابت حرف *e, *لفظ = format, *test_قدر = تہی; /* test_value is initialized to appease gcc */
        _cleanup_free_ حرف *s = تہی;
        حرف ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        صحیح nest = 0, r;

        أكد(format);

        اگر (length == SIZE_MAX)
                length = لڑی_لمبائی(format);

        pu = ret_unset_variables ? &unset_variables : تہی;
        pb = ret_bad_variables ? &bad_variables : تہی;

        برائے (e = format, i = 0; *e && i < length; e++, i++)
                انتخاب (state) {

                حالت لفظ_2:
                        اگر (*e == '$')
                                state = CURLY;
                        توڑو;

                حالت CURLY:
                        اگر (*e == '{') {
                                اگر (!strextendn(&s, لفظ, e-لفظ-1))
                                        واپس -ENOMEM;

                                لفظ = e-1;
                                state = VARIABLE;
                                nest++;

                        } ورنہ اگر (*e == '$') {
                                اگر (!strextendn(&s, لفظ, e-لفظ))
                                        واپس -ENOMEM;

                                لفظ = e+1;
                                state = لفظ_2;

                        } ورنہ اگر (FLAGS_سیٹ(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                اگر (!strextendn(&s, لفظ, e-لفظ-1))
                                        واپس -ENOMEM;

                                لفظ = e-1;
                                state = VARIABLE_RAW;

                        } ورنہ
                                state = لفظ_2;
                        توڑو;

                حالت VARIABLE:
                        اگر (*e == '}') {
                                حرف *t;

                                r = strv_env_get_n_validated(env, لفظ+2, e-لفظ-2, flags, &t, pu, pb);
                                اگر (r < 0)
                                        واپس r;

                                اگر (!strextend(&s, t))
                                        واپس -ENOMEM;

                                لفظ = e+1;
                                state = لفظ_2;
                                nest--;
                        } ورنہ اگر (*e == ':') {
                                اگر (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - لفظ - 2;
                                        state = TEST;
                                } ورنہ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = لفظ_2;
                        }
                        توڑو;

                حالت TEST:
                        اگر (*e == '-')
                                state = DEFAULT_قدر;
                        ورنہ اگر (*e == '+')
                                state = ALTERNATE_قدر;
                        ورنہ {
                                state = لفظ_2;
                                توڑو;
                        }

                        test_قدر = e+1;
                        توڑو;

                حالت DEFAULT_قدر: /* fall through */
                حالت ALTERNATE_قدر:
                        أكد(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        اگر (*e == '{') {
                                nest++;
                                توڑو;
                        }

                        اگر (*e != '}')
                                توڑو;

                        nest--;
                        اگر (nest == 0) {
                                _cleanup_strv_free_ حرف **u = تہی, **b = تہی;
                                _cleanup_free_ حرف *v = تہی;
                                حرف *t = تہی;

                                r = strv_env_get_n_validated(env, لفظ+2, len, flags, &t, pu, pb);
                                اگر (r < 0)
                                        واپس r;

                                اگر (t && state == ALTERNATE_قدر) {
                                        r = replace_env_بھرا(test_قدر, e-test_قدر, env, flags, &v, pu ? &u : تہی, pb ? &b : تہی);
                                        اگر (r < 0)
                                                واپس r;

                                        t = v;
                                } ورنہ اگر (!t && state == DEFAULT_قدر) {
                                        r = replace_env_بھرا(test_قدر, e-test_قدر, env, flags, &v, pu ? &u : تہی, pb ? &b : تہی);
                                        اگر (r < 0)
                                                واپس r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                اگر (r < 0)
                                        واپس r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                اگر (r < 0)
                                        واپس r;

                                اگر (!strextend(&s, t))
                                        واپس -ENOMEM;

                                لفظ = e+1;
                                state = لفظ_2;
                        }
                        توڑو;

                حالت VARIABLE_RAW:
                        أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        اگر (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                حرف *t = تہی;

                                r = strv_env_get_n_validated(env, لفظ+1, e-لفظ-1, flags, &t, &unset_variables, &bad_variables);
                                اگر (r < 0)
                                        واپس r;

                                اگر (!strextend(&s, t))
                                        واپس -ENOMEM;

                                لفظ = e--;
                                i--;
                                state = لفظ_2;
                        }
                        توڑو;
                }

        اگر (state == VARIABLE_RAW) {
                حرف *t;

                أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, لفظ+1, e-لفظ-1, flags, &t, &unset_variables, &bad_variables);
                اگر (r < 0)
                        واپس r;

                اگر (!strextend(&s, t))
                        واپس -ENOMEM;

        } ورنہ اگر (!strextendn(&s, لفظ, e-لفظ))
                واپس -ENOMEM;

        اگر (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        اگر (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        اگر (ret)
                *ret = TAKE_PTR(s);

        واپس 0;
}

صحیح replace_env_argv(
                حرف **argv,
                حرف **env,
                حرف ***ret,
                حرف ***ret_unset_variables,
                حرف ***ret_bad_variables) {

        _cleanup_strv_free_ حرف **n = تہی, **unset_variables = تہی, **bad_variables = تہی;
        size_t k = 0, l;
        صحیح r;

        أكد(!strv_isempty(argv));
        أكد(ret);

        l = strv_لمبائی(argv);

        n = new(حرف*, l+1);
        اگر (!n)
                واپس -ENOMEM;

        STRV_FOREACH(i, argv) {
                ثابت حرف *لفظ = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                اگر (لفظ[0] == '$' && !اندر_سیٹ(لفظ[1], '{', '$')) {
                        _cleanup_strv_free_ حرف **m = تہی;
                        ثابت حرف *name = لفظ + 1;
                        حرف *e, **w;
                        size_t q;

                        اگر (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                اگر (e)
                                        r = strv_بانٹو_بھرا(&m, e, WHITESPACE, نکالو_RELAX|نکالو_UNQUOTE);
                                ورنہ اگر (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ورنہ
                                        r = 0;
                        } ورنہ اگر (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ورنہ
                                r = 0;
                        اگر (r < 0)
                                واپس r;

                        q = strv_لمبائی(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, حجم(حرف*));
                        اگر (!w)
                                واپس -ENOMEM;

                        n = w;
                        اگر (m) {
                                حافظہ_نقل(n + k, m, (q + 1) * حجم(حرف*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = تہی;
                        جاری;
                }

                _cleanup_strv_free_ حرف **u = تہی, **b = تہی;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_بھرا(
                                لفظ,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : تہی,
                                ret_bad_variables ? &b : تہی);
                اگر (r < 0)
                        واپس r;
                n[++k] = تہی;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                اگر (r < 0)
                        واپس r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_unset_variables) {
                strv_ترتیب_دو_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        اگر (ret_bad_variables) {
                strv_ترتیب_دو_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        واپس 0;
}

صحیح getenv_bool(ثابت حرف *p) {
        ثابت حرف *e;

        e = getenv(p);
        اگر (!e)
                واپس -ENXIO;

        واپس parse_boolean(e);
}

صحیح secure_getenv_bool(ثابت حرف *p) {
        ثابت حرف *e;

        e = secure_getenv(p);
        اگر (!e)
                واپس -ENXIO;

        واپس parse_boolean(e);
}

صحیح secure_getenv_uint64(ثابت حرف *p, uint64_t *ret) {
        ثابت حرف *e;

        أكد(p);

        e = secure_getenv(p);
        اگر (!e)
                واپس -ENXIO;

        واپس محفوظ_atou64(e, ret);
}

صحیح سیٹ_unset_env(ثابت حرف *name, ثابت حرف *قدر, bool overwrite) {
        أكد(name);

        اگر (قدر)
                واپس RET_NERRNO(setenv(name, قدر, overwrite));

        واپس RET_NERRNO(unsetenv(name));
}

صحیح putenv_dup(ثابت حرف *assignment, bool override) {
        ثابت حرف *e, *n;

        e = strchr(assignment, '=');
        اگر (!e)
                واپس -EINVAL;

        n = strndupa_محفوظ(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        واپس RET_NERRNO(setenv(n, e + 1, override));
}

صحیح setenv_systemd_exec_pid(bool update_only) {
        ثابت حرف *e;
        صحیح r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        اگر (!e && update_only)
                واپس 0;

        اگر (streq_ptr(e, "*"))
                واپس 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        اگر (r < 0)
                واپس r;

        واپس 1;
}

صحیح setenv_systemd_log_level(خالی) {
        _cleanup_free_ حرف *val = تہی;
        صحیح r;

        r = log_level_مقصد_string_alloc(log_get_max_level(), &val);
        اگر (r < 0)
                واپس r;

        واپس RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

صحیح getenv_راستہ_list(ثابت حرف *name, حرف ***ret_paths) {
        _cleanup_strv_free_ حرف **l = تہی;
        ثابت حرف *e;
        صحیح r;

        أكد(name);
        أكد(ret_paths);

        e = secure_getenv(name);
        اگر (!e)
                واپس -ENXIO;

        r = strv_بانٹو_بھرا(&l, e, ":", نکالو_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                اگر (!راستہ_is_absolute(*p))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                اگر (!راستہ_is_normalized(*p))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                اگر (راستہ_equal(*p, "/"))
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        اگر (strv_isempty(l))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        واپس 1;
}

صحیح getenv_steal_erase(ثابت حرف *name, حرف **ret) {
        _cleanup_(erase_and_freep) حرف *a = تہی;
        حرف *e;

        أكد(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        اگر (!e) {
                اگر (ret)
                        *ret = تہی;
                واپس 0;
        }

        اگر (ret) {
                a = strdup(e);
                اگر (!a)
                        واپس -ENOMEM;
        }

        string_erase(e);

        اگر (unsetenv(name) < 0)
                واپس -errno;

        اگر (ret)
                *ret = TAKE_PTR(a);

        واپس 1;
}

صحیح setenvf(ثابت حرف *name, bool overwrite, ثابت حرف *valuef, ...) {
        _cleanup_free_ حرف *قدر = تہی;
        va_list ap;
        صحیح r;

        أكد(name);

        اگر (!valuef)
                واپس RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&قدر, valuef, ap);
        va_end(ap);

        اگر (r < 0)
                واپس -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        اگر (streq_ptr(getenv(name), قدر))
                واپس 0;

        واپس RET_NERRNO(setenv(name, قدر, overwrite));
}
