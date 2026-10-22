/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "syslog-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#ವ್ಯಾಖ್ಯಾನಿಸು VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(ಶೂನ್ಯ) {
        ದೀರ್ಘ l = sysconf(_SC_ARG_MAX);
        ಪ್ರತಿಜ್ಞಾ(l > 0);
        ಹಿಂತಿರುಗಿ (size_t) l;
}

ಸ್ಥಿರ bool env_name_is_valid_n(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, size_t n) {

        ಆದರೆ (n == SIZE_MAX)
                n = strlen_ptr(e);

        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ false;

        ಪ್ರತಿಜ್ಞಾ(e);

        ಆದರೆ (ascii_isdigit(e[0]))
                ಹಿಂತಿರುಗಿ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ಆದರೆ (_unlikely_(n > sc_arg_max() - 2))
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = e; p < e + n; p++)
                ಆದರೆ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool env_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಹಿಂತಿರುಗಿ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_ಮೌಲ್ಯ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!utf8_is_valid(e))
                ಹಿಂತಿರುಗಿ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ಆದರೆ (_unlikely_(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e) > sc_arg_max() - 3))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool env_assignment_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *eq;

        ಪ್ರತಿಜ್ಞಾ(e);

        eq = strchr(e, '=');
        ಆದರೆ (!eq)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!env_name_is_valid_n(e, eq - e))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!env_ಮೌಲ್ಯ_is_valid(eq + 1))
                ಹಿಂತಿರುಗಿ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ಆದರೆ (_unlikely_(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(e) > sc_arg_max() - 1))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool strv_env_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ಆದರೆ (!env_assignment_is_valid(*p))
                        ಹಿಂತಿರುಗಿ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ಆದರೆ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

bool strv_env_name_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        STRV_FOREACH(p, l) {
                ಆದರೆ (!env_name_is_valid(*p))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(p + 1, *p))
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

bool strv_env_name_or_assignment_is_valid(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        STRV_FOREACH(p, l) {
                ಆದರೆ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (strv_ಒಳಗೊಂಡಿದೆ(p + 1, *p))
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ env_append(ಅಕ್ಷರ **e, ಅಕ್ಷರ ***k, ಅಕ್ಷರ **a) {
        ಪ್ರತಿಜ್ಞಾ(e);
        ಪ್ರತಿಜ್ಞಾ(k);
        ಪ್ರತಿಜ್ಞಾ(*k >= e);

        ಆದರೆ (!a)
                ಹಿಂತಿರುಗಿ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        ಗಾಗಿ (; *a; a++) {
                ಅಕ್ಷರ **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ಆದರೆ ((*a)[n] == '=')
                        n++;

                ಗಾಗಿ (j = e; j < *k; j++)
                        ಆದರೆ (strneq(*j, *a, n))
                                ನಿಲ್ಲಿಸು;

                c = strdup(*a);
                ಆದರೆ (!c)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ಖಾಲಿ;
                        (*k)++;
                } ಇಲ್ಲದಿದ್ದರೆ
                        free_and_replace(*j, c); /* Override existing item */
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ** strv_env_ವಿಲೀನ(ಅಕ್ಷರ **ಮೊದಲ, ...) {
        _cleanup_strv_free_ ಅಕ್ಷರ **merged = ಖಾಲಿ;
        ಅಕ್ಷರ **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_ಉದ್ದಳತೆ(ಮೊದಲ);

        va_start(ap, ಮೊದಲ);
        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ **l;

                l = va_arg(ap, ಅಕ್ಷರ**);
                ಆದರೆ (l == ಸೂಚಕ_MAX)
                        ನಿಲ್ಲಿಸು;

                n += strv_ಉದ್ದಳತೆ(l);
        }
        va_end(ap);

        k = merged = new(ಅಕ್ಷರ*, n + 1);
        ಆದರೆ (!merged)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        merged[0] = ಖಾಲಿ;

        ಆದರೆ (env_append(merged, &k, ಮೊದಲ) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        va_start(ap, ಮೊದಲ);
        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ **l;

                l = va_arg(ap, ಅಕ್ಷರ**);
                ಆದರೆ (l == ಸೂಚಕ_MAX)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
                }
        }
        va_end(ap);

        ಹಿಂತಿರುಗಿ TAKE_PTR(merged);
}

ಸ್ಥಿರ bool env_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ) {
        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ನಮೂನೆ);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ಆದರೆ (streq(t, ನಮೂನೆ))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (!strchr(ನಮೂನೆ, '=')) {
                t = startswith(t, ನಮೂನೆ);

                ಹಿಂತಿರುಗಿ t && *t == '=';
        }

        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ bool env_ನಮೂದು_has_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂದು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;

        ಪ್ರತಿಜ್ಞಾ(ನಮೂದು);
        ಪ್ರತಿಜ್ಞಾ(name);

        t = startswith(ನಮೂದು, name);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ *t == '=';
}

ಅಕ್ಷರ** strv_env_delete(ಅಕ್ಷರ **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ ಅಕ್ಷರ **t = ಖಾಲಿ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_ಉದ್ದಳತೆ(x);

        t = new(ಅಕ್ಷರ*, n+1);
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                ಗಾಗಿ (size_t v = 0; v < n_lists; v++) {
                        ಅಕ್ಷರ **l;

                        l = va_arg(ap, ಅಕ್ಷರ**);
                        STRV_FOREACH(j, l)
                                ಆದರೆ (env_ಹೊಂದಿಕೆ(*k, *j))
                                        ಹೋಗು skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ಆದರೆ (!t[i])
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                i++;
                ಮುಂದುವರಿ;

        skip:
                va_end(ap);
        }

        t[i] = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(i <= n);

        ಹಿಂತಿರುಗಿ TAKE_PTR(t);
}

ಅಕ್ಷರ** strv_env_unset(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        ಅಕ್ಷರ **f, **t;
        ಗಾಗಿ (f = t = l; *f; f++) {
                ಆದರೆ (env_ಹೊಂದಿಕೆ(*f, p)) {
                        ಬಿಡುಗಡೆ(*f);
                        ಮುಂದುವರಿ;
                }

                *(t++) = *f;
        }

        *t = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ l;
}

ಅಕ್ಷರ** strv_env_unset_many_ಆಂತರಿಕ(ಅಕ್ಷರ **l, ...) {
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        ಅಕ್ಷರ **f, **t;
        ಗಾಗಿ (f = t = l; *f; f++) {
                bool ಸಿಕ್ಕಿತು = false;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
                va_list ap;

                va_start(ap, l);

                ಆಗ ((p = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)))
                        ಆದರೆ (env_ಹೊಂದಿಕೆ(*f, p)) {
                                ಸಿಕ್ಕಿತು = true;
                                ನಿಲ್ಲಿಸು;
                        }

                va_end(ap);

                ಆದರೆ (ಸಿಕ್ಕಿತು) {
                        ಬಿಡುಗಡೆ(*f);
                        ಮುಂದುವರಿ;
                }

                *(t++) = *f;
        }

        *t = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ l;
}

ಪೂರ್ಣಾಂಕ strv_env_replace_consume(ಅಕ್ಷರ ***l, ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, *name;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ಆದರೆ (!t) {
                ಬಿಡುಗಡೆ(p);
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        name = strndupa_ಸುರಕ್ಷಿತ(p, t - p);

        STRV_FOREACH(f, *l)
                ಆದರೆ (env_ನಮೂದು_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ಹಿಂತಿರುಗಿ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ strv_env_replace_strdup(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(assignment);

        ಅಕ್ಷರ *p = strdup(assignment);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಂಕ strv_env_replace_strdup_passthrough(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment) {
        ಅಕ್ಷರ *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(assignment);

        ಆದರೆ (strchr(assignment, '=')) {
                ಆದರೆ (!env_assignment_is_valid(assignment))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = strdup(assignment);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (!env_name_is_valid(assignment))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಂಕ strv_env_ನಿಯೋಜಿಸು(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(l);

        ಆದರೆ (!env_name_is_valid(ಕೀಲಿ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ಆದರೆ (!ಮೌಲ್ಯ) {
                strv_env_unset(*l, ಕೀಲಿ);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಅಕ್ಷರ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಂಕ strv_env_assignf(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *valuef, ...) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಆದರೆ (!env_name_is_valid(ಕೀಲಿ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!valuef) {
                strv_env_unset(*l, ಕೀಲಿ);
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&ಮೌಲ್ಯ, valuef, ap);
        va_end(ap);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಅಕ್ಷರ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಂಕ strv_env_ನಿಯೋಜಿಸು_many(ಅಕ್ಷರ ***l, ...) {
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);

        va_start(ap, l);
        ಗಾಗಿ (;;) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, *ಮೌಲ್ಯ;

                ಕೀಲಿ = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
                ಆದರೆ (!ಕೀಲಿ)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (!env_name_is_valid(ಕೀಲಿ)) {
                        va_end(ap);
                        ಹಿಂತಿರುಗಿ -EINVAL;
                }

                ಮೌಲ್ಯ = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
                ಆದರೆ (!ಮೌಲ್ಯ) {
                        strv_env_unset(*l, ಕೀಲಿ);
                        ಮುಂದುವರಿ;
                }

                ಅಕ್ಷರ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
                ಆದರೆ (!p) {
                        va_end(ap);
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ಆದರೆ (r < 0) {
                        va_end(ap);
                        ಹಿಂತಿರುಗಿ r;
                }
        }
        va_end(ap);

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* strv_env_get_n(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t k, ReplaceEnvFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(name);

        ಆದರೆ (k == SIZE_MAX)
                k = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
        ಆದರೆ (k <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        STRV_FOREACH_BACKWARDS(i, l)
                ಆದರೆ (strneq(*i, name, k) && (*i)[k] == '=')
                        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) *i + k + 1;

        ಆದರೆ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ಆದರೆ (k > sc_arg_max() - 2)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                t = strndupa_ಸುರಕ್ಷಿತ(name, k);
                ಹಿಂತಿರುಗಿ secure_getenv(t);
        };

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಅಕ್ಷರ* strv_env_ಜೋಡಿಗಳು_get(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಅಕ್ಷರ *ಫಲಿತಾಂಶ = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH_PAIR(ಕೀಲಿ, ಮೌಲ್ಯ, l)
                ಆದರೆ (streq(*ಕೀಲಿ, name))
                        ಫಲಿತಾಂಶ = *ಮೌಲ್ಯ;

        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ strv_env_get_merged(ಅಕ್ಷರ **l, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **v = ಖಾಲಿ;
        size_t n = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(ಕೀಲಿ, ಮೌಲ್ಯ, l) {
                ಅಕ್ಷರ *s;

                s = strjoin(*ಕೀಲಿ, "=", *ಮೌಲ್ಯ);
                ಆದರೆ (!s)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = strv_consume_with_ಗಾತ್ರ(&v, &n, s);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(v);
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ** strv_env_clean_with_callback(ಅಕ್ಷರ **l, ಶೂನ್ಯ (*invalid_callback)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata) {
        ಪೂರ್ಣಾಂಕ k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ಆದರೆ (!env_assignment_is_valid(*p)) {
                        ಆದರೆ (invalid_callback)
                                invalid_callback(*p, userdata);
                        ಬಿಡುಗಡೆ(*p);
                        ಮುಂದುವರಿ;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ಆದರೆ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ನಿಲ್ಲಿಸು;
                        }

                ಆದರೆ (duplicate) {
                        ಬಿಡುಗಡೆ(*p);
                        ಮುಂದುವರಿ;
                }

                l[k++] = *p;
        }

        ಆದರೆ (l)
                l[k] = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ l;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ strv_extend_with_ಉದ್ದಳತೆ(ಅಕ್ಷರ ***l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t n) {
        ಅಕ್ಷರ *c;

        c = strndup(s, n);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ strv_consume(l, c);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ strv_env_get_n_validated(
                ಅಕ್ಷರ **env,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                size_t l,
                ReplaceEnvFlags flags,
                ಅಕ್ಷರ **ret,              /* points into the env block! do not free! */
                ಅಕ್ಷರ ***unset_variables, /* updated in place */
                ಅಕ್ಷರ ***bad_variables) { /* ditto */

        ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ಆದರೆ (!e && unset_variables) {
                        r = strv_extend_with_ಉದ್ದಳತೆ(unset_variables, name, l);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        } ಇಲ್ಲದಿದ್ದರೆ {
                e = ಖಾಲಿ; /* Resolve invalid variable names the same way as unset ones */

                ಆದರೆ (bad_variables) {
                        r = strv_extend_with_ಉದ್ದಳತೆ(bad_variables, name, l);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        }

        *ret = e;
        ಹಿಂತಿರುಗಿ !!e;
}

ಪೂರ್ಣಾಂಕ replace_env_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                size_t length,
                ಅಕ್ಷರ **env,
                ReplaceEnvFlags flags,
                ಅಕ್ಷರ **ret,
                ಅಕ್ಷರ ***ret_unset_variables,
                ಅಕ್ಷರ ***ret_bad_variables) {

        ಎಣಿಕೆ {
                ಶಬ್ದ_2,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_ಮೌಲ್ಯ,
                ALTERNATE_ಮೌಲ್ಯ,
        } state = ಶಬ್ದ_2;

        _cleanup_strv_free_ ಅಕ್ಷರ **unset_variables = ಖಾಲಿ, **bad_variables = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *ಶಬ್ದ = format, *test_ಮೌಲ್ಯ = ಖಾಲಿ; /* test_value is initialized to appease gcc */
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಅಕ್ಷರ ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        ಪೂರ್ಣಾಂಕ nest = 0, r;

        ಪ್ರತಿಜ್ಞಾ(format);

        ಆದರೆ (length == SIZE_MAX)
                length = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(format);

        pu = ret_unset_variables ? &unset_variables : ಖಾಲಿ;
        pb = ret_bad_variables ? &bad_variables : ಖಾಲಿ;

        ಗಾಗಿ (e = format, i = 0; *e && i < length; e++, i++)
                ಆಯ್ಕೆ (state) {

                ಸ್ಥಿತಿ ಶಬ್ದ_2:
                        ಆದರೆ (*e == '$')
                                state = CURLY;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ CURLY:
                        ಆದರೆ (*e == '{') {
                                ಆದರೆ (!strextendn(&s, ಶಬ್ದ, e-ಶಬ್ದ-1))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e-1;
                                state = VARIABLE;
                                nest++;

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*e == '$') {
                                ಆದರೆ (!strextendn(&s, ಶಬ್ದ, e-ಶಬ್ದ))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e+1;
                                state = ಶಬ್ದ_2;

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ಆದರೆ (!strextendn(&s, ಶಬ್ದ, e-ಶಬ್ದ-1))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e-1;
                                state = VARIABLE_RAW;

                        } ಇಲ್ಲದಿದ್ದರೆ
                                state = ಶಬ್ದ_2;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ VARIABLE:
                        ಆದರೆ (*e == '}') {
                                ಅಕ್ಷರ *t;

                                r = strv_env_get_n_validated(env, ಶಬ್ದ+2, e-ಶಬ್ದ-2, flags, &t, pu, pb);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (!strextend(&s, t))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e+1;
                                state = ಶಬ್ದ_2;
                                nest--;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*e == ':') {
                                ಆದರೆ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - ಶಬ್ದ - 2;
                                        state = TEST;
                                } ಇಲ್ಲದಿದ್ದರೆ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = ಶಬ್ದ_2;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ TEST:
                        ಆದರೆ (*e == '-')
                                state = DEFAULT_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*e == '+')
                                state = ALTERNATE_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                state = ಶಬ್ದ_2;
                                ನಿಲ್ಲಿಸು;
                        }

                        test_ಮೌಲ್ಯ = e+1;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DEFAULT_ಮೌಲ್ಯ: /* fall through */
                ಸ್ಥಿತಿ ALTERNATE_ಮೌಲ್ಯ:
                        ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ಆದರೆ (*e == '{') {
                                nest++;
                                ನಿಲ್ಲಿಸು;
                        }

                        ಆದರೆ (*e != '}')
                                ನಿಲ್ಲಿಸು;

                        nest--;
                        ಆದರೆ (nest == 0) {
                                _cleanup_strv_free_ ಅಕ್ಷರ **u = ಖಾಲಿ, **b = ಖಾಲಿ;
                                _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
                                ಅಕ್ಷರ *t = ಖಾಲಿ;

                                r = strv_env_get_n_validated(env, ಶಬ್ದ+2, len, flags, &t, pu, pb);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (t && state == ALTERNATE_ಮೌಲ್ಯ) {
                                        r = replace_env_ತುಂಬಿದ(test_ಮೌಲ್ಯ, e-test_ಮೌಲ್ಯ, env, flags, &v, pu ? &u : ಖಾಲಿ, pb ? &b : ಖಾಲಿ);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ r;

                                        t = v;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!t && state == DEFAULT_ಮೌಲ್ಯ) {
                                        r = replace_env_ತುಂಬಿದ(test_ಮೌಲ್ಯ, e-test_ಮೌಲ್ಯ, env, flags, &v, pu ? &u : ಖಾಲಿ, pb ? &b : ಖಾಲಿ);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (!strextend(&s, t))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e+1;
                                state = ಶಬ್ದ_2;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ VARIABLE_RAW:
                        ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ಆದರೆ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ಅಕ್ಷರ *t = ಖಾಲಿ;

                                r = strv_env_get_n_validated(env, ಶಬ್ದ+1, e-ಶಬ್ದ-1, flags, &t, &unset_variables, &bad_variables);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಆದರೆ (!strextend(&s, t))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಶಬ್ದ = e--;
                                i--;
                                state = ಶಬ್ದ_2;
                        }
                        ನಿಲ್ಲಿಸು;
                }

        ಆದರೆ (state == VARIABLE_RAW) {
                ಅಕ್ಷರ *t;

                ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, ಶಬ್ದ+1, e-ಶಬ್ದ-1, flags, &t, &unset_variables, &bad_variables);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (!strextend(&s, t))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strextendn(&s, ಶಬ್ದ, e-ಶಬ್ದ))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ಆದರೆ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ಆದರೆ (ret)
                *ret = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ replace_env_argv(
                ಅಕ್ಷರ **argv,
                ಅಕ್ಷರ **env,
                ಅಕ್ಷರ ***ret,
                ಅಕ್ಷರ ***ret_unset_variables,
                ಅಕ್ಷರ ***ret_bad_variables) {

        _cleanup_strv_free_ ಅಕ್ಷರ **n = ಖಾಲಿ, **unset_variables = ಖಾಲಿ, **bad_variables = ಖಾಲಿ;
        size_t k = 0, l;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(!strv_isempty(argv));
        ಪ್ರತಿಜ್ಞಾ(ret);

        l = strv_ಉದ್ದಳತೆ(argv);

        n = new(ಅಕ್ಷರ*, l+1);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        STRV_FOREACH(i, argv) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ಆದರೆ (ಶಬ್ದ[0] == '$' && !ಒಳ_ಗಣ(ಶಬ್ದ[1], '{', '$')) {
                        _cleanup_strv_free_ ಅಕ್ಷರ **m = ಖಾಲಿ;
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = ಶಬ್ದ + 1;
                        ಅಕ್ಷರ *e, **w;
                        size_t q;

                        ಆದರೆ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ಆದರೆ (e)
                                        r = strv_ವಿಭಜಿಸು_ತುಂಬಿದ(&m, e, WHITESPACE, ಹೊರತೆಗೆ_RELAX|ಹೊರತೆಗೆ_UNQUOTE);
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ಇಲ್ಲದಿದ್ದರೆ
                                        r = 0;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ಇಲ್ಲದಿದ್ದರೆ
                                r = 0;
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        q = strv_ಉದ್ದಳತೆ(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, ಗಾತ್ರ(ಅಕ್ಷರ*));
                        ಆದರೆ (!w)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        n = w;
                        ಆದರೆ (m) {
                                ಸ್ಮರಣೆ_ನಕಲು(n + k, m, (q + 1) * ಗಾತ್ರ(ಅಕ್ಷರ*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ಖಾಲಿ;
                        ಮುಂದುವರಿ;
                }

                _cleanup_strv_free_ ಅಕ್ಷರ **u = ಖಾಲಿ, **b = ಖಾಲಿ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_ತುಂಬಿದ(
                                ಶಬ್ದ,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ಖಾಲಿ,
                                ret_bad_variables ? &b : ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                n[++k] = ಖಾಲಿ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_unset_variables) {
                strv_ವಿಂಗಡಿಸು_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ಆದರೆ (ret_bad_variables) {
                strv_ವಿಂಗಡಿಸು_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getenv_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        e = getenv(p);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಹಿಂತಿರುಗಿ parse_boolean(e);
}

ಪೂರ್ಣಾಂಕ secure_getenv_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        e = secure_getenv(p);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಹಿಂತಿರುಗಿ parse_boolean(e);
}

ಪೂರ್ಣಾಂಕ secure_getenv_uint64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, uint64_t *ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        ಪ್ರತಿಜ್ಞಾ(p);

        e = secure_getenv(p);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou64(e, ret);
}

ಪೂರ್ಣಾಂಕ ಗಣ_unset_env(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, bool overwrite) {
        ಪ್ರತಿಜ್ಞಾ(name);

        ಆದರೆ (ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ RET_NERRNO(setenv(name, ಮೌಲ್ಯ, overwrite));

        ಹಿಂತಿರುಗಿ RET_NERRNO(unsetenv(name));
}

ಪೂರ್ಣಾಂಕ putenv_dup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *assignment, bool override) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e, *n;

        e = strchr(assignment, '=');
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;

        n = strndupa_ಸುರಕ್ಷಿತ(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ಹಿಂತಿರುಗಿ RET_NERRNO(setenv(n, e + 1, override));
}

ಪೂರ್ಣಾಂಕ setenv_systemd_exec_pid(bool update_only) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ಆದರೆ (!e && update_only)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (streq_ptr(e, "*"))
                ಹಿಂತಿರುಗಿ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ setenv_systemd_log_level(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *val = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = log_level_ಗಮ್ಯ_string_alloc(log_get_max_level(), &val);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ಪೂರ್ಣಾಂಕ getenv_ಮಾರ್ಗ_list(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ ***ret_paths) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret_paths);

        e = secure_getenv(name);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        r = strv_ವಿಭಜಿಸು_ತುಂಬಿದ(&l, e, ":", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ಆದರೆ (!ಮಾರ್ಗ_is_absolute(*p))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ಆದರೆ (!ಮಾರ್ಗ_is_normalized(*p))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ಆದರೆ (ಮಾರ್ಗ_equal(*p, "/"))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ಆದರೆ (strv_isempty(l))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ getenv_steal_erase(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret) {
        _cleanup_(erase_and_freep) ಅಕ್ಷರ *a = ಖಾಲಿ;
        ಅಕ್ಷರ *e;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ಆದರೆ (!e) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (ret) {
                a = strdup(e);
                ಆದರೆ (!a)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        string_erase(e);

        ಆದರೆ (unsetenv(name) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret)
                *ret = TAKE_PTR(a);

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ setenvf(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, bool overwrite, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *valuef, ...) {
        _cleanup_free_ ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        ಆದರೆ (!valuef)
                ಹಿಂತಿರುಗಿ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&ಮೌಲ್ಯ, valuef, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ಆದರೆ (streq_ptr(getenv(name), ಮೌಲ್ಯ))
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ RET_NERRNO(setenv(name, ಮೌಲ್ಯ, overwrite));
}
