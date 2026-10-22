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
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(تهی) {
        long l = sysconf(_SC_ARG_MAX);
        أكد(l > 0);
        بازگشت (size_t) l;
}

ایستا bool env_name_is_valid_n(ثابت نویسه *e, size_t n) {

        اگر (n == SIZE_MAX)
                n = strlen_ptr(e);

        اگر (n == 0)
                بازگشت false;

        أكد(e);

        اگر (ascii_isdigit(e[0]))
                بازگشت false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        اگر (_unlikely_(n > sc_arg_max() - 2))
                بازگشت false;

        برای (ثابت نویسه *p = e; p < e + n; p++)
                اگر (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        بازگشت false;

        بازگشت true;
}

bool env_name_is_valid(ثابت نویسه *e) {
        بازگشت env_name_is_valid_n(e, SIZE_MAX);
}

bool env_مقدار_is_valid(ثابت نویسه *e) {
        اگر (!e)
                بازگشت false;

        اگر (!utf8_is_valid(e))
                بازگشت false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        اگر (_unlikely_(strlen(e) > sc_arg_max() - 3))
                بازگشت false;

        بازگشت true;
}

bool env_assignment_is_valid(ثابت نویسه *e) {
        ثابت نویسه *eq;

        أكد(e);

        eq = strchr(e, '=');
        اگر (!eq)
                بازگشت false;

        اگر (!env_name_is_valid_n(e, eq - e))
                بازگشت false;

        اگر (!env_مقدار_is_valid(eq + 1))
                بازگشت false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        اگر (_unlikely_(strlen(e) > sc_arg_max() - 1))
                بازگشت false;

        بازگشت true;
}

bool strv_env_is_valid(نویسه * ثابت *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                اگر (!env_assignment_is_valid(*p))
                        بازگشت false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        اگر (strneq(*p, *q, k) && (*q)[k] == '=')
                                بازگشت false;
        }

        بازگشت true;
}

bool strv_env_name_is_valid(نویسه * ثابت *l) {
        STRV_FOREACH(p, l) {
                اگر (!env_name_is_valid(*p))
                        بازگشت false;

                اگر (strv_contains(p + 1, *p))
                        بازگشت false;
        }

        بازگشت true;
}

bool strv_env_name_or_assignment_is_valid(نویسه * ثابت *l) {
        STRV_FOREACH(p, l) {
                اگر (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        بازگشت false;

                اگر (strv_contains(p + 1, *p))
                        بازگشت false;
        }

        بازگشت true;
}

ایستا صحیح env_append(نویسه **e, نویسه ***k, نویسه **a) {
        أكد(e);
        أكد(k);
        أكد(*k >= e);

        اگر (!a)
                بازگشت 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        برای (; *a; a++) {
                نویسه **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                اگر ((*a)[n] == '=')
                        n++;

                برای (j = e; j < *k; j++)
                        اگر (strneq(*j, *a, n))
                                بشکن;

                c = strdup(*a);
                اگر (!c)
                        بازگشت -ENOMEM;

                اگر (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = پوچ;
                        (*k)++;
                } وگرنه
                        free_and_replace(*j, c); /* Override existing item */
        }

        بازگشت 0;
}

نویسه** _strv_env_merge(نویسه **first, ...) {
        _cleanup_strv_free_ نویسه **merged = پوچ;
        نویسه **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        برای (;;) {
                نویسه **l;

                l = va_arg(ap, نویسه**);
                اگر (l == POINTER_MAX)
                        بشکن;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(نویسه*, n + 1);
        اگر (!merged)
                بازگشت پوچ;
        merged[0] = پوچ;

        اگر (env_append(merged, &k, first) < 0)
                بازگشت پوچ;

        va_start(ap, first);
        برای (;;) {
                نویسه **l;

                l = va_arg(ap, نویسه**);
                اگر (l == POINTER_MAX)
                        بشکن;

                اگر (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        بازگشت پوچ;
                }
        }
        va_end(ap);

        بازگشت TAKE_PTR(merged);
}

ایستا bool env_match(ثابت نویسه *t, ثابت نویسه *pattern) {
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

        اگر (streq(t, pattern))
                بازگشت true;

        اگر (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                بازگشت t && *t == '=';
        }

        بازگشت false;
}

ایستا bool env_entry_has_name(ثابت نویسه *entry, ثابت نویسه *name) {
        ثابت نویسه *t;

        أكد(entry);
        أكد(name);

        t = startswith(entry, name);
        اگر (!t)
                بازگشت false;

        بازگشت *t == '=';
}

نویسه** strv_env_delete(نویسه **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ نویسه **t = پوچ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(نویسه*, n+1);
        اگر (!t)
                بازگشت پوچ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                برای (size_t v = 0; v < n_lists; v++) {
                        نویسه **l;

                        l = va_arg(ap, نویسه**);
                        STRV_FOREACH(j, l)
                                اگر (env_match(*k, *j))
                                        برو skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                اگر (!t[i])
                        بازگشت پوچ;

                i++;
                ادامه;

        skip:
                va_end(ap);
        }

        t[i] = پوچ;

        أكد(i <= n);

        بازگشت TAKE_PTR(t);
}

نویسه** strv_env_unset(نویسه **l, ثابت نویسه *p) {
        أكد(p);

        اگر (!l)
                بازگشت پوچ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        نویسه **f, **t;
        برای (f = t = l; *f; f++) {
                اگر (env_match(*f, p)) {
                        آزاد(*f);
                        ادامه;
                }

                *(t++) = *f;
        }

        *t = پوچ;
        بازگشت l;
}

نویسه** strv_env_unset_many_internal(نویسه **l, ...) {
        اگر (!l)
                بازگشت پوچ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        نویسه **f, **t;
        برای (f = t = l; *f; f++) {
                bool found = false;
                ثابت نویسه *p;
                va_list ap;

                va_start(ap, l);

                وقتی ((p = va_arg(ap, ثابت نویسه*)))
                        اگر (env_match(*f, p)) {
                                found = true;
                                بشکن;
                        }

                va_end(ap);

                اگر (found) {
                        آزاد(*f);
                        ادامه;
                }

                *(t++) = *f;
        }

        *t = پوچ;
        بازگشت l;
}

صحیح strv_env_replace_consume(نویسه ***l, نویسه *p) {
        ثابت نویسه *t, *name;
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
                بازگشت -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                اگر (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        بازگشت 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

صحیح strv_env_replace_strdup(نویسه ***l, ثابت نویسه *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        أكد(l);
        أكد(assignment);

        نویسه *p = strdup(assignment);
        اگر (!p)
                بازگشت -ENOMEM;

        بازگشت strv_env_replace_consume(l, p);
}

صحیح strv_env_replace_strdup_passthrough(نویسه ***l, ثابت نویسه *assignment) {
        نویسه *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        أكد(l);
        أكد(assignment);

        اگر (strchr(assignment, '=')) {
                اگر (!env_assignment_is_valid(assignment))
                        بازگشت -EINVAL;

                p = strdup(assignment);
        } وگرنه {
                اگر (!env_name_is_valid(assignment))
                        بازگشت -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        اگر (!p)
                بازگشت -ENOMEM;

        بازگشت strv_env_replace_consume(l, p);
}

صحیح strv_env_assign(نویسه ***l, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        أكد(l);

        اگر (!env_name_is_valid(کلید))
                بازگشت -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        اگر (!مقدار) {
                strv_env_unset(*l, کلید);
                بازگشت 0;
        }

        نویسه *p = strjoin(کلید, "=", مقدار);
        اگر (!p)
                بازگشت -ENOMEM;

        بازگشت strv_env_replace_consume(l, p);
}

صحیح strv_env_assignf(نویسه ***l, ثابت نویسه *کلید, ثابت نویسه *valuef, ...) {
        صحیح r;

        أكد(l);
        أكد(کلید);

        اگر (!env_name_is_valid(کلید))
                بازگشت -EINVAL;

        اگر (!valuef) {
                strv_env_unset(*l, کلید);
                بازگشت 0;
        }

        _cleanup_free_ نویسه *مقدار = پوچ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&مقدار, valuef, ap);
        va_end(ap);
        اگر (r < 0)
                بازگشت -ENOMEM;

        نویسه *p = strjoin(کلید, "=", مقدار);
        اگر (!p)
                بازگشت -ENOMEM;

        بازگشت strv_env_replace_consume(l, p);
}

صحیح _strv_env_assign_many(نویسه ***l, ...) {
        va_list ap;
        صحیح r;

        أكد(l);

        va_start(ap, l);
        برای (;;) {
                ثابت نویسه *کلید, *مقدار;

                کلید = va_arg(ap, ثابت نویسه *);
                اگر (!کلید)
                        بشکن;

                اگر (!env_name_is_valid(کلید)) {
                        va_end(ap);
                        بازگشت -EINVAL;
                }

                مقدار = va_arg(ap, ثابت نویسه *);
                اگر (!مقدار) {
                        strv_env_unset(*l, کلید);
                        ادامه;
                }

                نویسه *p = strjoin(کلید, "=", مقدار);
                اگر (!p) {
                        va_end(ap);
                        بازگشت -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                اگر (r < 0) {
                        va_end(ap);
                        بازگشت r;
                }
        }
        va_end(ap);

        بازگشت 0;
}

نویسه* strv_env_get_n(نویسه * ثابت *l, ثابت نویسه *name, size_t k, ReplaceEnvFlags flags) {
        أكد(name);

        اگر (k == SIZE_MAX)
                k = strlen(name);
        اگر (k <= 0)
                بازگشت پوچ;

        STRV_FOREACH_BACKWARDS(i, l)
                اگر (strneq(*i, name, k) && (*i)[k] == '=')
                        بازگشت (نویسه*) *i + k + 1;

        اگر (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ثابت نویسه *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                اگر (k > sc_arg_max() - 2)
                        بازگشت پوچ;

                t = strndupa_safe(name, k);
                بازگشت secure_getenv(t);
        };

        بازگشت پوچ;
}

نویسه* strv_env_pairs_get(نویسه **l, ثابت نویسه *name) {
        نویسه *result = پوچ;

        أكد(name);

        STRV_FOREACH_PAIR(کلید, مقدار, l)
                اگر (streq(*کلید, name))
                        result = *مقدار;

        بازگشت result;
}

صحیح strv_env_get_merged(نویسه **l, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **v = پوچ;
        size_t n = 0;
        صحیح r;

        أكد(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(کلید, مقدار, l) {
                نویسه *s;

                s = strjoin(*کلید, "=", *مقدار);
                اگر (!s)
                        بازگشت -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(v);
        بازگشت 0;
}

نویسه** strv_env_clean_with_callback(نویسه **l, تهی (*invalid_callback)(ثابت نویسه *p, تهی *userdata), تهی *userdata) {
        صحیح k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                اگر (!env_assignment_is_valid(*p)) {
                        اگر (invalid_callback)
                                invalid_callback(*p, userdata);
                        آزاد(*p);
                        ادامه;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        اگر (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                بشکن;
                        }

                اگر (duplicate) {
                        آزاد(*p);
                        ادامه;
                }

                l[k++] = *p;
        }

        اگر (l)
                l[k] = پوچ;

        بازگشت l;
}

ایستا صحیح strv_extend_with_length(نویسه ***l, ثابت نویسه *s, size_t n) {
        نویسه *c;

        c = strndup(s, n);
        اگر (!c)
                بازگشت -ENOMEM;

        بازگشت strv_consume(l, c);
}

ایستا صحیح strv_env_get_n_validated(
                نویسه **env,
                ثابت نویسه *name,
                size_t l,
                ReplaceEnvFlags flags,
                نویسه **ret,              /* points into the env block! do not free! */
                نویسه ***unset_variables, /* updated in place */
                نویسه ***bad_variables) { /* ditto */

        نویسه *e;
        صحیح r;

        أكد(l == 0 || name);
        أكد(ret);

        اگر (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                اگر (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        اگر (r < 0)
                                بازگشت r;
                }
        } وگرنه {
                e = پوچ; /* Resolve invalid variable names the same way as unset ones */

                اگر (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        اگر (r < 0)
                                بازگشت r;
                }
        }

        *ret = e;
        بازگشت !!e;
}

صحیح replace_env_full(
                ثابت نویسه *format,
                size_t length,
                نویسه **env,
                ReplaceEnvFlags flags,
                نویسه **ret,
                نویسه ***ret_unset_variables,
                نویسه ***ret_bad_variables) {

        شمارش {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_مقدار,
                ALTERNATE_مقدار,
        } state = WORD;

        _cleanup_strv_free_ نویسه **unset_variables = پوچ, **bad_variables = پوچ;
        ثابت نویسه *e, *word = format, *test_مقدار = پوچ; /* test_value is initialized to appease gcc */
        _cleanup_free_ نویسه *s = پوچ;
        نویسه ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        صحیح nest = 0, r;

        أكد(format);

        اگر (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : پوچ;
        pb = ret_bad_variables ? &bad_variables : پوچ;

        برای (e = format, i = 0; *e && i < length; e++, i++)
                انتخاب (state) {

                حالت WORD:
                        اگر (*e == '$')
                                state = CURLY;
                        بشکن;

                حالت CURLY:
                        اگر (*e == '{') {
                                اگر (!strextendn(&s, word, e-word-1))
                                        بازگشت -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } وگرنه اگر (*e == '$') {
                                اگر (!strextendn(&s, word, e-word))
                                        بازگشت -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } وگرنه اگر (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                اگر (!strextendn(&s, word, e-word-1))
                                        بازگشت -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } وگرنه
                                state = WORD;
                        بشکن;

                حالت VARIABLE:
                        اگر (*e == '}') {
                                نویسه *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (!strextend(&s, t))
                                        بازگشت -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } وگرنه اگر (*e == ':') {
                                اگر (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } وگرنه
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        بشکن;

                حالت TEST:
                        اگر (*e == '-')
                                state = DEFAULT_مقدار;
                        وگرنه اگر (*e == '+')
                                state = ALTERNATE_مقدار;
                        وگرنه {
                                state = WORD;
                                بشکن;
                        }

                        test_مقدار = e+1;
                        بشکن;

                حالت DEFAULT_مقدار: /* fall through */
                حالت ALTERNATE_مقدار:
                        أكد(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        اگر (*e == '{') {
                                nest++;
                                بشکن;
                        }

                        اگر (*e != '}')
                                بشکن;

                        nest--;
                        اگر (nest == 0) {
                                _cleanup_strv_free_ نویسه **u = پوچ, **b = پوچ;
                                _cleanup_free_ نویسه *v = پوچ;
                                نویسه *t = پوچ;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (t && state == ALTERNATE_مقدار) {
                                        r = replace_env_full(test_مقدار, e-test_مقدار, env, flags, &v, pu ? &u : پوچ, pb ? &b : پوچ);
                                        اگر (r < 0)
                                                بازگشت r;

                                        t = v;
                                } وگرنه اگر (!t && state == DEFAULT_مقدار) {
                                        r = replace_env_full(test_مقدار, e-test_مقدار, env, flags, &v, pu ? &u : پوچ, pb ? &b : پوچ);
                                        اگر (r < 0)
                                                بازگشت r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                اگر (r < 0)
                                        بازگشت r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (!strextend(&s, t))
                                        بازگشت -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        بشکن;

                حالت VARIABLE_RAW:
                        أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        اگر (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                نویسه *t = پوچ;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                اگر (r < 0)
                                        بازگشت r;

                                اگر (!strextend(&s, t))
                                        بازگشت -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        بشکن;
                }

        اگر (state == VARIABLE_RAW) {
                نویسه *t;

                أكد(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                اگر (r < 0)
                        بازگشت r;

                اگر (!strextend(&s, t))
                        بازگشت -ENOMEM;

        } وگرنه اگر (!strextendn(&s, word, e-word))
                بازگشت -ENOMEM;

        اگر (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        اگر (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        اگر (ret)
                *ret = TAKE_PTR(s);

        بازگشت 0;
}

صحیح replace_env_argv(
                نویسه **argv,
                نویسه **env,
                نویسه ***ret,
                نویسه ***ret_unset_variables,
                نویسه ***ret_bad_variables) {

        _cleanup_strv_free_ نویسه **n = پوچ, **unset_variables = پوچ, **bad_variables = پوچ;
        size_t k = 0, l;
        صحیح r;

        أكد(!strv_isempty(argv));
        أكد(ret);

        l = strv_length(argv);

        n = new(نویسه*, l+1);
        اگر (!n)
                بازگشت -ENOMEM;

        STRV_FOREACH(i, argv) {
                ثابت نویسه *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                اگر (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ نویسه **m = پوچ;
                        ثابت نویسه *name = word + 1;
                        نویسه *e, **w;
                        size_t q;

                        اگر (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                اگر (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                وگرنه اگر (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                وگرنه
                                        r = 0;
                        } وگرنه اگر (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        وگرنه
                                r = 0;
                        اگر (r < 0)
                                بازگشت r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, اندازه(نویسه*));
                        اگر (!w)
                                بازگشت -ENOMEM;

                        n = w;
                        اگر (m) {
                                memcpy(n + k, m, (q + 1) * اندازه(نویسه*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = پوچ;
                        ادامه;
                }

                _cleanup_strv_free_ نویسه **u = پوچ, **b = پوچ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : پوچ,
                                ret_bad_variables ? &b : پوچ);
                اگر (r < 0)
                        بازگشت r;
                n[++k] = پوچ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                اگر (r < 0)
                        بازگشت r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        اگر (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        بازگشت 0;
}

صحیح getenv_bool(ثابت نویسه *p) {
        ثابت نویسه *e;

        e = getenv(p);
        اگر (!e)
                بازگشت -ENXIO;

        بازگشت parse_boolean(e);
}

صحیح secure_getenv_bool(ثابت نویسه *p) {
        ثابت نویسه *e;

        e = secure_getenv(p);
        اگر (!e)
                بازگشت -ENXIO;

        بازگشت parse_boolean(e);
}

صحیح secure_getenv_uint64(ثابت نویسه *p, uint64_t *ret) {
        ثابت نویسه *e;

        أكد(p);

        e = secure_getenv(p);
        اگر (!e)
                بازگشت -ENXIO;

        بازگشت safe_atou64(e, ret);
}

صحیح set_unset_env(ثابت نویسه *name, ثابت نویسه *مقدار, bool overwrite) {
        أكد(name);

        اگر (مقدار)
                بازگشت RET_NERRNO(setenv(name, مقدار, overwrite));

        بازگشت RET_NERRNO(unsetenv(name));
}

صحیح putenv_dup(ثابت نویسه *assignment, bool override) {
        ثابت نویسه *e, *n;

        e = strchr(assignment, '=');
        اگر (!e)
                بازگشت -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        بازگشت RET_NERRNO(setenv(n, e + 1, override));
}

صحیح setenv_systemd_exec_pid(bool update_only) {
        ثابت نویسه *e;
        صحیح r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        اگر (!e && update_only)
                بازگشت 0;

        اگر (streq_ptr(e, "*"))
                بازگشت 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

صحیح setenv_systemd_log_level(تهی) {
        _cleanup_free_ نویسه *val = پوچ;
        صحیح r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        اگر (r < 0)
                بازگشت r;

        بازگشت RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

صحیح getenv_path_list(ثابت نویسه *name, نویسه ***ret_paths) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        ثابت نویسه *e;
        صحیح r;

        أكد(name);
        أكد(ret_paths);

        e = secure_getenv(name);
        اگر (!e)
                بازگشت -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                اگر (!path_is_absolute(*p))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                اگر (!path_is_normalized(*p))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                اگر (path_equal(*p, "/"))
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        اگر (strv_isempty(l))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        بازگشت 1;
}

صحیح getenv_steal_erase(ثابت نویسه *name, نویسه **ret) {
        _cleanup_(erase_and_freep) نویسه *a = پوچ;
        نویسه *e;

        أكد(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        اگر (!e) {
                اگر (ret)
                        *ret = پوچ;
                بازگشت 0;
        }

        اگر (ret) {
                a = strdup(e);
                اگر (!a)
                        بازگشت -ENOMEM;
        }

        string_erase(e);

        اگر (unsetenv(name) < 0)
                بازگشت -errno;

        اگر (ret)
                *ret = TAKE_PTR(a);

        بازگشت 1;
}

صحیح setenvf(ثابت نویسه *name, bool overwrite, ثابت نویسه *valuef, ...) {
        _cleanup_free_ نویسه *مقدار = پوچ;
        va_list ap;
        صحیح r;

        أكد(name);

        اگر (!valuef)
                بازگشت RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&مقدار, valuef, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        اگر (streq_ptr(getenv(name), مقدار))
                بازگشت 0;

        بازگشت RET_NERRNO(setenv(name, مقدار, overwrite));
}
