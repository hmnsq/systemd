/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "syslog-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(ਖਾਲੀ) {
        ਲੰਮਾ l = sysconf(_SC_ARG_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(l > 0);
        ਵਾਪਸ (size_t) l;
}

ਸਥਿਰ bool env_name_is_valid_n(ਅਚਲ ਅੱਖਰ *e, size_t n) {

        ਜੇ (n == SIZE_MAX)
                n = strlen_ptr(e);

        ਜੇ (n == 0)
                ਵਾਪਸ false;

        ਪ੍ਰਤਿਜ੍ਞਾ(e);

        ਜੇ (ascii_isdigit(e[0]))
                ਵਾਪਸ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ਜੇ (_unlikely_(n > sc_arg_max() - 2))
                ਵਾਪਸ false;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = e; p < e + n; p++)
                ਜੇ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool env_name_is_valid(ਅਚਲ ਅੱਖਰ *e) {
        ਵਾਪਸ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_ਮੁੱਲ_is_valid(ਅਚਲ ਅੱਖਰ *e) {
        ਜੇ (!e)
                ਵਾਪਸ false;

        ਜੇ (!utf8_is_valid(e))
                ਵਾਪਸ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ਜੇ (_unlikely_(ਲੜੀ_ਲੰਬਾਈ(e) > sc_arg_max() - 3))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool env_assignment_is_valid(ਅਚਲ ਅੱਖਰ *e) {
        ਅਚਲ ਅੱਖਰ *eq;

        ਪ੍ਰਤਿਜ੍ਞਾ(e);

        eq = strchr(e, '=');
        ਜੇ (!eq)
                ਵਾਪਸ false;

        ਜੇ (!env_name_is_valid_n(e, eq - e))
                ਵਾਪਸ false;

        ਜੇ (!env_ਮੁੱਲ_is_valid(eq + 1))
                ਵਾਪਸ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ਜੇ (_unlikely_(ਲੜੀ_ਲੰਬਾਈ(e) > sc_arg_max() - 1))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool strv_env_is_valid(ਅੱਖਰ * ਅਚਲ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ਜੇ (!env_assignment_is_valid(*p))
                        ਵਾਪਸ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ਜੇ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

bool strv_env_name_is_valid(ਅੱਖਰ * ਅਚਲ *l) {
        STRV_FOREACH(p, l) {
                ਜੇ (!env_name_is_valid(*p))
                        ਵਾਪਸ false;

                ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(p + 1, *p))
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

bool strv_env_name_or_assignment_is_valid(ਅੱਖਰ * ਅਚਲ *l) {
        STRV_FOREACH(p, l) {
                ਜੇ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ਵਾਪਸ false;

                ਜੇ (strv_ਸ਼ਾਮਲ_ਹੈ(p + 1, *p))
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

ਸਥਿਰ ਪੂਰਨ env_append(ਅੱਖਰ **e, ਅੱਖਰ ***k, ਅੱਖਰ **a) {
        ਪ੍ਰਤਿਜ੍ਞਾ(e);
        ਪ੍ਰਤਿਜ੍ਞਾ(k);
        ਪ੍ਰਤਿਜ੍ਞਾ(*k >= e);

        ਜੇ (!a)
                ਵਾਪਸ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        ਲਈ (; *a; a++) {
                ਅੱਖਰ **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ਜੇ ((*a)[n] == '=')
                        n++;

                ਲਈ (j = e; j < *k; j++)
                        ਜੇ (strneq(*j, *a, n))
                                ਰੋਕੋ;

                c = strdup(*a);
                ਜੇ (!c)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ਨਿਰਾ;
                        (*k)++;
                } ਨਹੀਂਤਾਂ
                        free_and_replace(*j, c); /* Override existing item */
        }

        ਵਾਪਸ 0;
}

ਅੱਖਰ** strv_env_ਮਿਲਾਓ(ਅੱਖਰ **ਪਹਿਲਾ, ...) {
        _cleanup_strv_free_ ਅੱਖਰ **merged = ਨਿਰਾ;
        ਅੱਖਰ **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_ਲੰਮਾਈ(ਪਹਿਲਾ);

        va_start(ap, ਪਹਿਲਾ);
        ਲਈ (;;) {
                ਅੱਖਰ **l;

                l = va_arg(ap, ਅੱਖਰ**);
                ਜੇ (l == ਸੂਚਕ_MAX)
                        ਰੋਕੋ;

                n += strv_ਲੰਮਾਈ(l);
        }
        va_end(ap);

        k = merged = new(ਅੱਖਰ*, n + 1);
        ਜੇ (!merged)
                ਵਾਪਸ ਨਿਰਾ;
        merged[0] = ਨਿਰਾ;

        ਜੇ (env_append(merged, &k, ਪਹਿਲਾ) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        va_start(ap, ਪਹਿਲਾ);
        ਲਈ (;;) {
                ਅੱਖਰ **l;

                l = va_arg(ap, ਅੱਖਰ**);
                ਜੇ (l == ਸੂਚਕ_MAX)
                        ਰੋਕੋ;

                ਜੇ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ਵਾਪਸ ਨਿਰਾ;
                }
        }
        va_end(ap);

        ਵਾਪਸ TAKE_PTR(merged);
}

ਸਥਿਰ bool env_ਮੇਲ(ਅਚਲ ਅੱਖਰ *t, ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਮੂਨਾ);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ਜੇ (streq(t, ਨਮੂਨਾ))
                ਵਾਪਸ true;

        ਜੇ (!strchr(ਨਮੂਨਾ, '=')) {
                t = startswith(t, ਨਮੂਨਾ);

                ਵਾਪਸ t && *t == '=';
        }

        ਵਾਪਸ false;
}

ਸਥਿਰ bool env_ਇੰਦਰਾਜ_has_name(ਅਚਲ ਅੱਖਰ *ਇੰਦਰਾਜ, ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਅੱਖਰ *t;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਇੰਦਰਾਜ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        t = startswith(ਇੰਦਰਾਜ, name);
        ਜੇ (!t)
                ਵਾਪਸ false;

        ਵਾਪਸ *t == '=';
}

ਅੱਖਰ** strv_env_delete(ਅੱਖਰ **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ ਅੱਖਰ **t = ਨਿਰਾ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_ਲੰਮਾਈ(x);

        t = new(ਅੱਖਰ*, n+1);
        ਜੇ (!t)
                ਵਾਪਸ ਨਿਰਾ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                ਲਈ (size_t v = 0; v < n_lists; v++) {
                        ਅੱਖਰ **l;

                        l = va_arg(ap, ਅੱਖਰ**);
                        STRV_FOREACH(j, l)
                                ਜੇ (env_ਮੇਲ(*k, *j))
                                        ਜਾਓ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ਜੇ (!t[i])
                        ਵਾਪਸ ਨਿਰਾ;

                i++;
                ਜਾਰੀ;

        skip:
                va_end(ap);
        }

        t[i] = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(i <= n);

        ਵਾਪਸ TAKE_PTR(t);
}

ਅੱਖਰ** strv_env_unset(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (!l)
                ਵਾਪਸ ਨਿਰਾ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        ਅੱਖਰ **f, **t;
        ਲਈ (f = t = l; *f; f++) {
                ਜੇ (env_ਮੇਲ(*f, p)) {
                        ਮੁਕਤ(*f);
                        ਜਾਰੀ;
                }

                *(t++) = *f;
        }

        *t = ਨਿਰਾ;
        ਵਾਪਸ l;
}

ਅੱਖਰ** strv_env_unset_many_ਅੰਦਰੂਨੀ(ਅੱਖਰ **l, ...) {
        ਜੇ (!l)
                ਵਾਪਸ ਨਿਰਾ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        ਅੱਖਰ **f, **t;
        ਲਈ (f = t = l; *f; f++) {
                bool ਮਿਲਿਆ = false;
                ਅਚਲ ਅੱਖਰ *p;
                va_list ap;

                va_start(ap, l);

                ਜਦੋਂ ((p = va_arg(ap, ਅਚਲ ਅੱਖਰ*)))
                        ਜੇ (env_ਮੇਲ(*f, p)) {
                                ਮਿਲਿਆ = true;
                                ਰੋਕੋ;
                        }

                va_end(ap);

                ਜੇ (ਮਿਲਿਆ) {
                        ਮੁਕਤ(*f);
                        ਜਾਰੀ;
                }

                *(t++) = *f;
        }

        *t = ਨਿਰਾ;
        ਵਾਪਸ l;
}

ਪੂਰਨ strv_env_replace_consume(ਅੱਖਰ ***l, ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *t, *name;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ਜੇ (!t) {
                ਮੁਕਤ(p);
                ਵਾਪਸ -EINVAL;
        }

        name = strndupa_ਸੁਰੱਖਿਅਤ(p, t - p);

        STRV_FOREACH(f, *l)
                ਜੇ (env_ਇੰਦਰਾਜ_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ਵਾਪਸ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਪੂਰਨ strv_env_replace_strdup(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਪ੍ਰਤਿਜ੍ਞਾ(assignment);

        ਅੱਖਰ *p = strdup(assignment);
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_env_replace_consume(l, p);
}

ਪੂਰਨ strv_env_replace_strdup_passthrough(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *assignment) {
        ਅੱਖਰ *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਪ੍ਰਤਿਜ੍ਞਾ(assignment);

        ਜੇ (strchr(assignment, '=')) {
                ਜੇ (!env_assignment_is_valid(assignment))
                        ਵਾਪਸ -EINVAL;

                p = strdup(assignment);
        } ਨਹੀਂਤਾਂ {
                ਜੇ (!env_name_is_valid(assignment))
                        ਵਾਪਸ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_env_replace_consume(l, p);
}

ਪੂਰਨ strv_env_ਸੌਂਪੋ(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        ਜੇ (!env_name_is_valid(ਕੁੰਜੀ))
                ਵਾਪਸ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ਜੇ (!ਮੁੱਲ) {
                strv_env_unset(*l, ਕੁੰਜੀ);
                ਵਾਪਸ 0;
        }

        ਅੱਖਰ *p = strjoin(ਕੁੰਜੀ, "=", ਮੁੱਲ);
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_env_replace_consume(l, p);
}

ਪੂਰਨ strv_env_assignf(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *valuef, ...) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        ਜੇ (!env_name_is_valid(ਕੁੰਜੀ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!valuef) {
                strv_env_unset(*l, ਕੁੰਜੀ);
                ਵਾਪਸ 0;
        }

        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&ਮੁੱਲ, valuef, ap);
        va_end(ap);
        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਅੱਖਰ *p = strjoin(ਕੁੰਜੀ, "=", ਮੁੱਲ);
        ਜੇ (!p)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_env_replace_consume(l, p);
}

ਪੂਰਨ strv_env_ਸੌਂਪੋ_many(ਅੱਖਰ ***l, ...) {
        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        va_start(ap, l);
        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, *ਮੁੱਲ;

                ਕੁੰਜੀ = va_arg(ap, ਅਚਲ ਅੱਖਰ *);
                ਜੇ (!ਕੁੰਜੀ)
                        ਰੋਕੋ;

                ਜੇ (!env_name_is_valid(ਕੁੰਜੀ)) {
                        va_end(ap);
                        ਵਾਪਸ -EINVAL;
                }

                ਮੁੱਲ = va_arg(ap, ਅਚਲ ਅੱਖਰ *);
                ਜੇ (!ਮੁੱਲ) {
                        strv_env_unset(*l, ਕੁੰਜੀ);
                        ਜਾਰੀ;
                }

                ਅੱਖਰ *p = strjoin(ਕੁੰਜੀ, "=", ਮੁੱਲ);
                ਜੇ (!p) {
                        va_end(ap);
                        ਵਾਪਸ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ਜੇ (r < 0) {
                        va_end(ap);
                        ਵਾਪਸ r;
                }
        }
        va_end(ap);

        ਵਾਪਸ 0;
}

ਅੱਖਰ* strv_env_get_n(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *name, size_t k, ReplaceEnvFlags flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਜੇ (k == SIZE_MAX)
                k = ਲੜੀ_ਲੰਬਾਈ(name);
        ਜੇ (k <= 0)
                ਵਾਪਸ ਨਿਰਾ;

        STRV_FOREACH_BACKWARDS(i, l)
                ਜੇ (strneq(*i, name, k) && (*i)[k] == '=')
                        ਵਾਪਸ (ਅੱਖਰ*) *i + k + 1;

        ਜੇ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ਅਚਲ ਅੱਖਰ *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ਜੇ (k > sc_arg_max() - 2)
                        ਵਾਪਸ ਨਿਰਾ;

                t = strndupa_ਸੁਰੱਖਿਅਤ(name, k);
                ਵਾਪਸ secure_getenv(t);
        };

        ਵਾਪਸ ਨਿਰਾ;
}

ਅੱਖਰ* strv_env_ਜੋੜੇ_get(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *name) {
        ਅੱਖਰ *ਨਤੀਜਾ = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        STRV_FOREACH_PAIR(ਕੁੰਜੀ, ਮੁੱਲ, l)
                ਜੇ (streq(*ਕੁੰਜੀ, name))
                        ਨਤੀਜਾ = *ਮੁੱਲ;

        ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ strv_env_get_merged(ਅੱਖਰ **l, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **v = ਨਿਰਾ;
        size_t n = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(ਕੁੰਜੀ, ਮੁੱਲ, l) {
                ਅੱਖਰ *s;

                s = strjoin(*ਕੁੰਜੀ, "=", *ਮੁੱਲ);
                ਜੇ (!s)
                        ਵਾਪਸ -ENOMEM;

                r = strv_consume_with_ਮਾਪ(&v, &n, s);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(v);
        ਵਾਪਸ 0;
}

ਅੱਖਰ** strv_env_clean_with_callback(ਅੱਖਰ **l, ਖਾਲੀ (*invalid_callback)(ਅਚਲ ਅੱਖਰ *p, ਖਾਲੀ *userdata), ਖਾਲੀ *userdata) {
        ਪੂਰਨ k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ਜੇ (!env_assignment_is_valid(*p)) {
                        ਜੇ (invalid_callback)
                                invalid_callback(*p, userdata);
                        ਮੁਕਤ(*p);
                        ਜਾਰੀ;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ਜੇ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ਰੋਕੋ;
                        }

                ਜੇ (duplicate) {
                        ਮੁਕਤ(*p);
                        ਜਾਰੀ;
                }

                l[k++] = *p;
        }

        ਜੇ (l)
                l[k] = ਨਿਰਾ;

        ਵਾਪਸ l;
}

ਸਥਿਰ ਪੂਰਨ strv_extend_with_ਲੰਮਾਈ(ਅੱਖਰ ***l, ਅਚਲ ਅੱਖਰ *s, size_t n) {
        ਅੱਖਰ *c;

        c = strndup(s, n);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ strv_consume(l, c);
}

ਸਥਿਰ ਪੂਰਨ strv_env_get_n_validated(
                ਅੱਖਰ **env,
                ਅਚਲ ਅੱਖਰ *name,
                size_t l,
                ReplaceEnvFlags flags,
                ਅੱਖਰ **ret,              /* points into the env block! do not free! */
                ਅੱਖਰ ***unset_variables, /* updated in place */
                ਅੱਖਰ ***bad_variables) { /* ditto */

        ਅੱਖਰ *e;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(l == 0 || name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ਜੇ (!e && unset_variables) {
                        r = strv_extend_with_ਲੰਮਾਈ(unset_variables, name, l);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        } ਨਹੀਂਤਾਂ {
                e = ਨਿਰਾ; /* Resolve invalid variable names the same way as unset ones */

                ਜੇ (bad_variables) {
                        r = strv_extend_with_ਲੰਮਾਈ(bad_variables, name, l);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        }

        *ret = e;
        ਵਾਪਸ !!e;
}

ਪੂਰਨ replace_env_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *format,
                size_t length,
                ਅੱਖਰ **env,
                ReplaceEnvFlags flags,
                ਅੱਖਰ **ret,
                ਅੱਖਰ ***ret_unset_variables,
                ਅੱਖਰ ***ret_bad_variables) {

        ਗਿਣਤੀ {
                ਸ਼ਬਦ_2,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_ਮੁੱਲ,
                ALTERNATE_ਮੁੱਲ,
        } state = ਸ਼ਬਦ_2;

        _cleanup_strv_free_ ਅੱਖਰ **unset_variables = ਨਿਰਾ, **bad_variables = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e, *ਸ਼ਬਦ = format, *test_ਮੁੱਲ = ਨਿਰਾ; /* test_value is initialized to appease gcc */
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਅੱਖਰ ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        ਪੂਰਨ nest = 0, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(format);

        ਜੇ (length == SIZE_MAX)
                length = ਲੜੀ_ਲੰਬਾਈ(format);

        pu = ret_unset_variables ? &unset_variables : ਨਿਰਾ;
        pb = ret_bad_variables ? &bad_variables : ਨਿਰਾ;

        ਲਈ (e = format, i = 0; *e && i < length; e++, i++)
                ਚੋਣ (state) {

                ਹਾਲਤ ਸ਼ਬਦ_2:
                        ਜੇ (*e == '$')
                                state = CURLY;
                        ਰੋਕੋ;

                ਹਾਲਤ CURLY:
                        ਜੇ (*e == '{') {
                                ਜੇ (!strextendn(&s, ਸ਼ਬਦ, e-ਸ਼ਬਦ-1))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e-1;
                                state = VARIABLE;
                                nest++;

                        } ਨਹੀਂਤਾਂ ਜੇ (*e == '$') {
                                ਜੇ (!strextendn(&s, ਸ਼ਬਦ, e-ਸ਼ਬਦ))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e+1;
                                state = ਸ਼ਬਦ_2;

                        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ਜੇ (!strextendn(&s, ਸ਼ਬਦ, e-ਸ਼ਬਦ-1))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e-1;
                                state = VARIABLE_RAW;

                        } ਨਹੀਂਤਾਂ
                                state = ਸ਼ਬਦ_2;
                        ਰੋਕੋ;

                ਹਾਲਤ VARIABLE:
                        ਜੇ (*e == '}') {
                                ਅੱਖਰ *t;

                                r = strv_env_get_n_validated(env, ਸ਼ਬਦ+2, e-ਸ਼ਬਦ-2, flags, &t, pu, pb);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (!strextend(&s, t))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e+1;
                                state = ਸ਼ਬਦ_2;
                                nest--;
                        } ਨਹੀਂਤਾਂ ਜੇ (*e == ':') {
                                ਜੇ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - ਸ਼ਬਦ - 2;
                                        state = TEST;
                                } ਨਹੀਂਤਾਂ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = ਸ਼ਬਦ_2;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ TEST:
                        ਜੇ (*e == '-')
                                state = DEFAULT_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ ਜੇ (*e == '+')
                                state = ALTERNATE_ਮੁੱਲ;
                        ਨਹੀਂਤਾਂ {
                                state = ਸ਼ਬਦ_2;
                                ਰੋਕੋ;
                        }

                        test_ਮੁੱਲ = e+1;
                        ਰੋਕੋ;

                ਹਾਲਤ DEFAULT_ਮੁੱਲ: /* fall through */
                ਹਾਲਤ ALTERNATE_ਮੁੱਲ:
                        ਪ੍ਰਤਿਜ੍ਞਾ(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ਜੇ (*e == '{') {
                                nest++;
                                ਰੋਕੋ;
                        }

                        ਜੇ (*e != '}')
                                ਰੋਕੋ;

                        nest--;
                        ਜੇ (nest == 0) {
                                _cleanup_strv_free_ ਅੱਖਰ **u = ਨਿਰਾ, **b = ਨਿਰਾ;
                                _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
                                ਅੱਖਰ *t = ਨਿਰਾ;

                                r = strv_env_get_n_validated(env, ਸ਼ਬਦ+2, len, flags, &t, pu, pb);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (t && state == ALTERNATE_ਮੁੱਲ) {
                                        r = replace_env_ਭਰਿਆ(test_ਮੁੱਲ, e-test_ਮੁੱਲ, env, flags, &v, pu ? &u : ਨਿਰਾ, pb ? &b : ਨਿਰਾ);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ r;

                                        t = v;
                                } ਨਹੀਂਤਾਂ ਜੇ (!t && state == DEFAULT_ਮੁੱਲ) {
                                        r = replace_env_ਭਰਿਆ(test_ਮੁੱਲ, e-test_ਮੁੱਲ, env, flags, &v, pu ? &u : ਨਿਰਾ, pb ? &b : ਨਿਰਾ);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (!strextend(&s, t))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e+1;
                                state = ਸ਼ਬਦ_2;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ VARIABLE_RAW:
                        ਪ੍ਰਤਿਜ੍ਞਾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ਜੇ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ਅੱਖਰ *t = ਨਿਰਾ;

                                r = strv_env_get_n_validated(env, ਸ਼ਬਦ+1, e-ਸ਼ਬਦ-1, flags, &t, &unset_variables, &bad_variables);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (!strextend(&s, t))
                                        ਵਾਪਸ -ENOMEM;

                                ਸ਼ਬਦ = e--;
                                i--;
                                state = ਸ਼ਬਦ_2;
                        }
                        ਰੋਕੋ;
                }

        ਜੇ (state == VARIABLE_RAW) {
                ਅੱਖਰ *t;

                ਪ੍ਰਤਿਜ੍ਞਾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, ਸ਼ਬਦ+1, e-ਸ਼ਬਦ-1, flags, &t, &unset_variables, &bad_variables);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (!strextend(&s, t))
                        ਵਾਪਸ -ENOMEM;

        } ਨਹੀਂਤਾਂ ਜੇ (!strextendn(&s, ਸ਼ਬਦ, e-ਸ਼ਬਦ))
                ਵਾਪਸ -ENOMEM;

        ਜੇ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ਜੇ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ਜੇ (ret)
                *ret = TAKE_PTR(s);

        ਵਾਪਸ 0;
}

ਪੂਰਨ replace_env_argv(
                ਅੱਖਰ **argv,
                ਅੱਖਰ **env,
                ਅੱਖਰ ***ret,
                ਅੱਖਰ ***ret_unset_variables,
                ਅੱਖਰ ***ret_bad_variables) {

        _cleanup_strv_free_ ਅੱਖਰ **n = ਨਿਰਾ, **unset_variables = ਨਿਰਾ, **bad_variables = ਨਿਰਾ;
        size_t k = 0, l;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(!strv_isempty(argv));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        l = strv_ਲੰਮਾਈ(argv);

        n = new(ਅੱਖਰ*, l+1);
        ਜੇ (!n)
                ਵਾਪਸ -ENOMEM;

        STRV_FOREACH(i, argv) {
                ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ਜੇ (ਸ਼ਬਦ[0] == '$' && !ਵਿੱਚ_ਸੈੱਟ(ਸ਼ਬਦ[1], '{', '$')) {
                        _cleanup_strv_free_ ਅੱਖਰ **m = ਨਿਰਾ;
                        ਅਚਲ ਅੱਖਰ *name = ਸ਼ਬਦ + 1;
                        ਅੱਖਰ *e, **w;
                        size_t q;

                        ਜੇ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ਜੇ (e)
                                        r = strv_ਵੰਡੋ_ਭਰਿਆ(&m, e, WHITESPACE, ਕੱਢੋ_RELAX|ਕੱਢੋ_UNQUOTE);
                                ਨਹੀਂਤਾਂ ਜੇ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ਨਹੀਂਤਾਂ
                                        r = 0;
                        } ਨਹੀਂਤਾਂ ਜੇ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ਨਹੀਂਤਾਂ
                                r = 0;
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        q = strv_ਲੰਮਾਈ(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, ਆਕਾਰ(ਅੱਖਰ*));
                        ਜੇ (!w)
                                ਵਾਪਸ -ENOMEM;

                        n = w;
                        ਜੇ (m) {
                                ਸਮ੍ਰਿਤੀ_ਨਕਲ(n + k, m, (q + 1) * ਆਕਾਰ(ਅੱਖਰ*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ਨਿਰਾ;
                        ਜਾਰੀ;
                }

                _cleanup_strv_free_ ਅੱਖਰ **u = ਨਿਰਾ, **b = ਨਿਰਾ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_ਭਰਿਆ(
                                ਸ਼ਬਦ,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ਨਿਰਾ,
                                ret_bad_variables ? &b : ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                n[++k] = ਨਿਰਾ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_unset_variables) {
                strv_ਛਾਂਟੋ_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ਜੇ (ret_bad_variables) {
                strv_ਛਾਂਟੋ_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ਵਾਪਸ 0;
}

ਪੂਰਨ getenv_bool(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *e;

        e = getenv(p);
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        ਵਾਪਸ parse_boolean(e);
}

ਪੂਰਨ secure_getenv_bool(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *e;

        e = secure_getenv(p);
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        ਵਾਪਸ parse_boolean(e);
}

ਪੂਰਨ secure_getenv_uint64(ਅਚਲ ਅੱਖਰ *p, uint64_t *ret) {
        ਅਚਲ ਅੱਖਰ *e;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        e = secure_getenv(p);
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou64(e, ret);
}

ਪੂਰਨ ਸੈੱਟ_unset_env(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, bool overwrite) {
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਜੇ (ਮੁੱਲ)
                ਵਾਪਸ RET_NERRNO(setenv(name, ਮੁੱਲ, overwrite));

        ਵਾਪਸ RET_NERRNO(unsetenv(name));
}

ਪੂਰਨ putenv_dup(ਅਚਲ ਅੱਖਰ *assignment, bool override) {
        ਅਚਲ ਅੱਖਰ *e, *n;

        e = strchr(assignment, '=');
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;

        n = strndupa_ਸੁਰੱਖਿਅਤ(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ਵਾਪਸ RET_NERRNO(setenv(n, e + 1, override));
}

ਪੂਰਨ setenv_systemd_exec_pid(bool update_only) {
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ਜੇ (!e && update_only)
                ਵਾਪਸ 0;

        ਜੇ (streq_ptr(e, "*"))
                ਵਾਪਸ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਪੂਰਨ setenv_systemd_log_level(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *val = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = log_level_ਮੰਜ਼ਿਲ_string_alloc(log_get_max_level(), &val);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ਪੂਰਨ getenv_ਰਸਤਾ_list(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ ***ret_paths) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_paths);

        e = secure_getenv(name);
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        r = strv_ਵੰਡੋ_ਭਰਿਆ(&l, e, ":", ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ਜੇ (!ਰਸਤਾ_is_absolute(*p))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ਜੇ (!ਰਸਤਾ_is_normalized(*p))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ਜੇ (ਰਸਤਾ_equal(*p, "/"))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ਜੇ (strv_isempty(l))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ਵਾਪਸ 1;
}

ਪੂਰਨ getenv_steal_erase(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret) {
        _cleanup_(erase_and_freep) ਅੱਖਰ *a = ਨਿਰਾ;
        ਅੱਖਰ *e;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ਜੇ (!e) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਜੇ (ret) {
                a = strdup(e);
                ਜੇ (!a)
                        ਵਾਪਸ -ENOMEM;
        }

        string_erase(e);

        ਜੇ (unsetenv(name) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret)
                *ret = TAKE_PTR(a);

        ਵਾਪਸ 1;
}

ਪੂਰਨ setenvf(ਅਚਲ ਅੱਖਰ *name, bool overwrite, ਅਚਲ ਅੱਖਰ *valuef, ...) {
        _cleanup_free_ ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਜੇ (!valuef)
                ਵਾਪਸ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&ਮੁੱਲ, valuef, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ਜੇ (streq_ptr(getenv(name), ਮੁੱਲ))
                ਵਾਪਸ 0;

        ਵਾਪਸ RET_NERRNO(setenv(name, ਮੁੱਲ, overwrite));
}
