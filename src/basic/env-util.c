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
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(ಶೂನ್ಯ) {
        long l = sysconf(_SC_ARG_MAX);
        ಪ್ರತಿಜ್ಞಾ(l > 0);
        ಪ್ರತ್ಯಾಗಮ (size_t) l;
}

ಸ್ಥಿರ bool env_name_is_valid_n(ಧ್ರುವ ವರ್ಣ *e, size_t n) {

        ಯದಿ (n == SIZE_MAX)
                n = strlen_ptr(e);

        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿಜ್ಞಾ(e);

        ಯದಿ (ascii_isdigit(e[0]))
                ಪ್ರತ್ಯಾಗಮ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ಯದಿ (_unlikely_(n > sc_arg_max() - 2))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = e; p < e + n; p++)
                ಯದಿ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool env_name_is_valid(ಧ್ರುವ ವರ್ಣ *e) {
        ಪ್ರತ್ಯಾಗಮ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_ಮೌಲ್ಯ_is_valid(ಧ್ರುವ ವರ್ಣ *e) {
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!utf8_is_valid(e))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ಯದಿ (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool env_assignment_is_valid(ಧ್ರುವ ವರ್ಣ *e) {
        ಧ್ರುವ ವರ್ಣ *eq;

        ಪ್ರತಿಜ್ಞಾ(e);

        eq = strchr(e, '=');
        ಯದಿ (!eq)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!env_name_is_valid_n(e, eq - e))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!env_ಮೌಲ್ಯ_is_valid(eq + 1))
                ಪ್ರತ್ಯಾಗಮ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ಯದಿ (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool strv_env_is_valid(ವರ್ಣ * ಧ್ರುವ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ಯದಿ (!env_assignment_is_valid(*p))
                        ಪ್ರತ್ಯಾಗಮ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ಯದಿ (strneq(*p, *q, k) && (*q)[k] == '=')
                                ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

bool strv_env_name_is_valid(ವರ್ಣ * ಧ್ರುವ *l) {
        STRV_FOREACH(p, l) {
                ಯದಿ (!env_name_is_valid(*p))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (strv_contains(p + 1, *p))
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

bool strv_env_name_or_assignment_is_valid(ವರ್ಣ * ಧ್ರುವ *l) {
        STRV_FOREACH(p, l) {
                ಯದಿ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (strv_contains(p + 1, *p))
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ env_append(ವರ್ಣ **e, ವರ್ಣ ***k, ವರ್ಣ **a) {
        ಪ್ರತಿಜ್ಞಾ(e);
        ಪ್ರತಿಜ್ಞಾ(k);
        ಪ್ರತಿಜ್ಞಾ(*k >= e);

        ಯದಿ (!a)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        ಪ್ರತಿ (; *a; a++) {
                ವರ್ಣ **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ಯದಿ ((*a)[n] == '=')
                        n++;

                ಪ್ರತಿ (j = e; j < *k; j++)
                        ಯದಿ (strneq(*j, *a, n))
                                ಭಙ್ಗ;

                c = strdup(*a);
                ಯದಿ (!c)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ರಿಕ್ತ;
                        (*k)++;
                } ಅನ್ಯಥಾ
                        free_and_replace(*j, c); /* Override existing item */
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ** _strv_env_merge(ವರ್ಣ **first, ...) {
        _cleanup_strv_free_ ವರ್ಣ **merged = ರಿಕ್ತ;
        ವರ್ಣ **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        ಪ್ರತಿ (;;) {
                ವರ್ಣ **l;

                l = va_arg(ap, ವರ್ಣ**);
                ಯದಿ (l == POINTER_MAX)
                        ಭಙ್ಗ;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(ವರ್ಣ*, n + 1);
        ಯದಿ (!merged)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        merged[0] = ರಿಕ್ತ;

        ಯದಿ (env_append(merged, &k, first) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        va_start(ap, first);
        ಪ್ರತಿ (;;) {
                ವರ್ಣ **l;

                l = va_arg(ap, ವರ್ಣ**);
                ಯದಿ (l == POINTER_MAX)
                        ಭಙ್ಗ;

                ಯದಿ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
                }
        }
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(merged);
}

ಸ್ಥಿರ bool env_match(ಧ್ರುವ ವರ್ಣ *t, ಧ್ರುವ ವರ್ಣ *pattern) {
        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ಯದಿ (streq(t, pattern))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ಪ್ರತ್ಯಾಗಮ t && *t == '=';
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ bool env_entry_has_name(ಧ್ರುವ ವರ್ಣ *entry, ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ವರ್ಣ *t;

        ಪ್ರತಿಜ್ಞಾ(entry);
        ಪ್ರತಿಜ್ಞಾ(name);

        t = startswith(entry, name);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ *t == '=';
}

ವರ್ಣ** strv_env_delete(ವರ್ಣ **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ ವರ್ಣ **t = ರಿಕ್ತ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(ವರ್ಣ*, n+1);
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                ಪ್ರತಿ (size_t v = 0; v < n_lists; v++) {
                        ವರ್ಣ **l;

                        l = va_arg(ap, ವರ್ಣ**);
                        STRV_FOREACH(j, l)
                                ಯದಿ (env_match(*k, *j))
                                        ಗಚ್ಛ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ಯದಿ (!t[i])
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                i++;
                ಅನುವರ್ತ;

        skip:
                va_end(ap);
        }

        t[i] = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(i <= n);

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(t);
}

ವರ್ಣ** strv_env_unset(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        ವರ್ಣ **f, **t;
        ಪ್ರತಿ (f = t = l; *f; f++) {
                ಯದಿ (env_match(*f, p)) {
                        ಮುಕ್ತ(*f);
                        ಅನುವರ್ತ;
                }

                *(t++) = *f;
        }

        *t = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ l;
}

ವರ್ಣ** strv_env_unset_many_internal(ವರ್ಣ **l, ...) {
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        ವರ್ಣ **f, **t;
        ಪ್ರತಿ (f = t = l; *f; f++) {
                bool found = false;
                ಧ್ರುವ ವರ್ಣ *p;
                va_list ap;

                va_start(ap, l);

                ಯಾವತ್ ((p = va_arg(ap, ಧ್ರುವ ವರ್ಣ*)))
                        ಯದಿ (env_match(*f, p)) {
                                found = true;
                                ಭಙ್ಗ;
                        }

                va_end(ap);

                ಯದಿ (found) {
                        ಮುಕ್ತ(*f);
                        ಅನುವರ್ತ;
                }

                *(t++) = *f;
        }

        *t = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ l;
}

ಪೂರ್ಣಾಙ್ಕ strv_env_replace_consume(ವರ್ಣ ***l, ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *t, *name;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ಯದಿ (!t) {
                ಮುಕ್ತ(p);
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                ಯದಿ (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ strv_env_replace_strdup(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(assignment);

        ವರ್ಣ *p = strdup(assignment);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಙ್ಕ strv_env_replace_strdup_passthrough(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *assignment) {
        ವರ್ಣ *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(assignment);

        ಯದಿ (strchr(assignment, '=')) {
                ಯದಿ (!env_assignment_is_valid(assignment))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                p = strdup(assignment);
        } ಅನ್ಯಥಾ {
                ಯದಿ (!env_name_is_valid(assignment))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಙ್ಕ strv_env_assign(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(l);

        ಯದಿ (!env_name_is_valid(ಕೀಲಿ))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ಯದಿ (!ಮೌಲ್ಯ) {
                strv_env_unset(*l, ಕೀಲಿ);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ವರ್ಣ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಙ್ಕ strv_env_assignf(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *valuef, ...) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಯದಿ (!env_name_is_valid(ಕೀಲಿ))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!valuef) {
                strv_env_unset(*l, ಕೀಲಿ);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&ಮೌಲ್ಯ, valuef, ap);
        va_end(ap);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ವರ್ಣ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_env_replace_consume(l, p);
}

ಪೂರ್ಣಾಙ್ಕ _strv_env_assign_many(ವರ್ಣ ***l, ...) {
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(l);

        va_start(ap, l);
        ಪ್ರತಿ (;;) {
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, *ಮೌಲ್ಯ;

                ಕೀಲಿ = va_arg(ap, ಧ್ರುವ ವರ್ಣ *);
                ಯದಿ (!ಕೀಲಿ)
                        ಭಙ್ಗ;

                ಯದಿ (!env_name_is_valid(ಕೀಲಿ)) {
                        va_end(ap);
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                }

                ಮೌಲ್ಯ = va_arg(ap, ಧ್ರುವ ವರ್ಣ *);
                ಯದಿ (!ಮೌಲ್ಯ) {
                        strv_env_unset(*l, ಕೀಲಿ);
                        ಅನುವರ್ತ;
                }

                ವರ್ಣ *p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
                ಯದಿ (!p) {
                        va_end(ap);
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ಯದಿ (r < 0) {
                        va_end(ap);
                        ಪ್ರತ್ಯಾಗಮ r;
                }
        }
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* strv_env_get_n(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *name, size_t k, ReplaceEnvFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(name);

        ಯದಿ (k == SIZE_MAX)
                k = strlen(name);
        ಯದಿ (k <= 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        STRV_FOREACH_BACKWARDS(i, l)
                ಯದಿ (strneq(*i, name, k) && (*i)[k] == '=')
                        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) *i + k + 1;

        ಯದಿ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ಧ್ರುವ ವರ್ಣ *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ಯದಿ (k > sc_arg_max() - 2)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                t = strndupa_safe(name, k);
                ಪ್ರತ್ಯಾಗಮ secure_getenv(t);
        };

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ವರ್ಣ* strv_env_pairs_get(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *name) {
        ವರ್ಣ *result = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(name);

        STRV_FOREACH_PAIR(ಕೀಲಿ, ಮೌಲ್ಯ, l)
                ಯದಿ (streq(*ಕೀಲಿ, name))
                        result = *ಮೌಲ್ಯ;

        ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ strv_env_get_merged(ವರ್ಣ **l, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **v = ರಿಕ್ತ;
        size_t n = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(ಕೀಲಿ, ಮೌಲ್ಯ, l) {
                ವರ್ಣ *s;

                s = strjoin(*ಕೀಲಿ, "=", *ಮೌಲ್ಯ);
                ಯದಿ (!s)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(v);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ** strv_env_clean_with_callback(ವರ್ಣ **l, ಶೂನ್ಯ (*invalid_callback)(ಧ್ರುವ ವರ್ಣ *p, ಶೂನ್ಯ *userdata), ಶೂನ್ಯ *userdata) {
        ಪೂರ್ಣಾಙ್ಕ k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ಯದಿ (!env_assignment_is_valid(*p)) {
                        ಯದಿ (invalid_callback)
                                invalid_callback(*p, userdata);
                        ಮುಕ್ತ(*p);
                        ಅನುವರ್ತ;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ಯದಿ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ಭಙ್ಗ;
                        }

                ಯದಿ (duplicate) {
                        ಮುಕ್ತ(*p);
                        ಅನುವರ್ತ;
                }

                l[k++] = *p;
        }

        ಯದಿ (l)
                l[k] = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ l;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ strv_extend_with_length(ವರ್ಣ ***l, ಧ್ರುವ ವರ್ಣ *s, size_t n) {
        ವರ್ಣ *c;

        c = strndup(s, n);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ strv_consume(l, c);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ strv_env_get_n_validated(
                ವರ್ಣ **env,
                ಧ್ರುವ ವರ್ಣ *name,
                size_t l,
                ReplaceEnvFlags flags,
                ವರ್ಣ **ret,              /* points into the env block! do not free! */
                ವರ್ಣ ***unset_variables, /* updated in place */
                ವರ್ಣ ***bad_variables) { /* ditto */

        ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(l == 0 || name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ಯದಿ (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }
        } ಅನ್ಯಥಾ {
                e = ರಿಕ್ತ; /* Resolve invalid variable names the same way as unset ones */

                ಯದಿ (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }
        }

        *ret = e;
        ಪ್ರತ್ಯಾಗಮ !!e;
}

ಪೂರ್ಣಾಙ್ಕ replace_env_full(
                ಧ್ರುವ ವರ್ಣ *format,
                size_t length,
                ವರ್ಣ **env,
                ReplaceEnvFlags flags,
                ವರ್ಣ **ret,
                ವರ್ಣ ***ret_unset_variables,
                ವರ್ಣ ***ret_bad_variables) {

        ಗಣನಾ {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_ಮೌಲ್ಯ,
                ALTERNATE_ಮೌಲ್ಯ,
        } state = WORD;

        _cleanup_strv_free_ ವರ್ಣ **unset_variables = ರಿಕ್ತ, **bad_variables = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e, *word = format, *test_ಮೌಲ್ಯ = ರಿಕ್ತ; /* test_value is initialized to appease gcc */
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ವರ್ಣ ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        ಪೂರ್ಣಾಙ್ಕ nest = 0, r;

        ಪ್ರತಿಜ್ಞಾ(format);

        ಯದಿ (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : ರಿಕ್ತ;
        pb = ret_bad_variables ? &bad_variables : ರಿಕ್ತ;

        ಪ್ರತಿ (e = format, i = 0; *e && i < length; e++, i++)
                ವಿಕಲ್ಪ (state) {

                ಪಕ್ಷ WORD:
                        ಯದಿ (*e == '$')
                                state = CURLY;
                        ಭಙ್ಗ;

                ಪಕ್ಷ CURLY:
                        ಯದಿ (*e == '{') {
                                ಯದಿ (!strextendn(&s, word, e-word-1))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } ಅನ್ಯಥಾ ಯದಿ (*e == '$') {
                                ಯದಿ (!strextendn(&s, word, e-word))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ಯದಿ (!strextendn(&s, word, e-word-1))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } ಅನ್ಯಥಾ
                                state = WORD;
                        ಭಙ್ಗ;

                ಪಕ್ಷ VARIABLE:
                        ಯದಿ (*e == '}') {
                                ವರ್ಣ *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (!strextend(&s, t))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } ಅನ್ಯಥಾ ಯದಿ (*e == ':') {
                                ಯದಿ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } ಅನ್ಯಥಾ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ TEST:
                        ಯದಿ (*e == '-')
                                state = DEFAULT_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ ಯದಿ (*e == '+')
                                state = ALTERNATE_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ {
                                state = WORD;
                                ಭಙ್ಗ;
                        }

                        test_ಮೌಲ್ಯ = e+1;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DEFAULT_ಮೌಲ್ಯ: /* fall through */
                ಪಕ್ಷ ALTERNATE_ಮೌಲ್ಯ:
                        ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ಯದಿ (*e == '{') {
                                nest++;
                                ಭಙ್ಗ;
                        }

                        ಯದಿ (*e != '}')
                                ಭಙ್ಗ;

                        nest--;
                        ಯದಿ (nest == 0) {
                                _cleanup_strv_free_ ವರ್ಣ **u = ರಿಕ್ತ, **b = ರಿಕ್ತ;
                                _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
                                ವರ್ಣ *t = ರಿಕ್ತ;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (t && state == ALTERNATE_ಮೌಲ್ಯ) {
                                        r = replace_env_full(test_ಮೌಲ್ಯ, e-test_ಮೌಲ್ಯ, env, flags, &v, pu ? &u : ರಿಕ್ತ, pb ? &b : ರಿಕ್ತ);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        t = v;
                                } ಅನ್ಯಥಾ ಯದಿ (!t && state == DEFAULT_ಮೌಲ್ಯ) {
                                        r = replace_env_full(test_ಮೌಲ್ಯ, e-test_ಮೌಲ್ಯ, env, flags, &v, pu ? &u : ರಿಕ್ತ, pb ? &b : ರಿಕ್ತ);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (!strextend(&s, t))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ VARIABLE_RAW:
                        ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ಯದಿ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ವರ್ಣ *t = ರಿಕ್ತ;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಯದಿ (!strextend(&s, t))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        ಭಙ್ಗ;
                }

        ಯದಿ (state == VARIABLE_RAW) {
                ವರ್ಣ *t;

                ಪ್ರತಿಜ್ಞಾ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (!strextend(&s, t))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        } ಅನ್ಯಥಾ ಯದಿ (!strextendn(&s, word, e-word))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ಯದಿ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ಯದಿ (ret)
                *ret = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ replace_env_argv(
                ವರ್ಣ **argv,
                ವರ್ಣ **env,
                ವರ್ಣ ***ret,
                ವರ್ಣ ***ret_unset_variables,
                ವರ್ಣ ***ret_bad_variables) {

        _cleanup_strv_free_ ವರ್ಣ **n = ರಿಕ್ತ, **unset_variables = ರಿಕ್ತ, **bad_variables = ರಿಕ್ತ;
        size_t k = 0, l;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(!strv_isempty(argv));
        ಪ್ರತಿಜ್ಞಾ(ret);

        l = strv_length(argv);

        n = new(ವರ್ಣ*, l+1);
        ಯದಿ (!n)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        STRV_FOREACH(i, argv) {
                ಧ್ರುವ ವರ್ಣ *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ಯದಿ (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ ವರ್ಣ **m = ರಿಕ್ತ;
                        ಧ್ರುವ ವರ್ಣ *name = word + 1;
                        ವರ್ಣ *e, **w;
                        size_t q;

                        ಯದಿ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ಯದಿ (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                ಅನ್ಯಥಾ ಯದಿ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ಅನ್ಯಥಾ
                                        r = 0;
                        } ಅನ್ಯಥಾ ಯದಿ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ಅನ್ಯಥಾ
                                r = 0;
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, ಪರಿಮಾಣ(ವರ್ಣ*));
                        ಯದಿ (!w)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        n = w;
                        ಯದಿ (m) {
                                memcpy(n + k, m, (q + 1) * ಪರಿಮಾಣ(ವರ್ಣ*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ರಿಕ್ತ;
                        ಅನುವರ್ತ;
                }

                _cleanup_strv_free_ ವರ್ಣ **u = ರಿಕ್ತ, **b = ರಿಕ್ತ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ರಿಕ್ತ,
                                ret_bad_variables ? &b : ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                n[++k] = ರಿಕ್ತ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ಯದಿ (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getenv_bool(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *e;

        e = getenv(p);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಪ್ರತ್ಯಾಗಮ parse_boolean(e);
}

ಪೂರ್ಣಾಙ್ಕ secure_getenv_bool(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *e;

        e = secure_getenv(p);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಪ್ರತ್ಯಾಗಮ parse_boolean(e);
}

ಪೂರ್ಣಾಙ್ಕ secure_getenv_uint64(ಧ್ರುವ ವರ್ಣ *p, uint64_t *ret) {
        ಧ್ರುವ ವರ್ಣ *e;

        ಪ್ರತಿಜ್ಞಾ(p);

        e = secure_getenv(p);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಪ್ರತ್ಯಾಗಮ safe_atou64(e, ret);
}

ಪೂರ್ಣಾಙ್ಕ set_unset_env(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, bool overwrite) {
        ಪ್ರತಿಜ್ಞಾ(name);

        ಯದಿ (ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setenv(name, ಮೌಲ್ಯ, overwrite));

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(unsetenv(name));
}

ಪೂರ್ಣಾಙ್ಕ putenv_dup(ಧ್ರುವ ವರ್ಣ *assignment, bool override) {
        ಧ್ರುವ ವರ್ಣ *e, *n;

        e = strchr(assignment, '=');
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setenv(n, e + 1, override));
}

ಪೂರ್ಣಾಙ್ಕ setenv_systemd_exec_pid(bool update_only) {
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ಯದಿ (!e && update_only)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (streq_ptr(e, "*"))
                ಪ್ರತ್ಯಾಗಮ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ setenv_systemd_log_level(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *val = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ಪೂರ್ಣಾಙ್ಕ getenv_path_list(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ ***ret_paths) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ret_paths);

        e = secure_getenv(name);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ಯದಿ (!path_is_absolute(*p))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ಯದಿ (!path_is_normalized(*p))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ಯದಿ (path_equal(*p, "/"))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ಯದಿ (strv_isempty(l))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ getenv_steal_erase(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret) {
        _cleanup_(erase_and_freep) ವರ್ಣ *a = ರಿಕ್ತ;
        ವರ್ಣ *e;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ಯದಿ (!e) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (ret) {
                a = strdup(e);
                ಯದಿ (!a)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        string_erase(e);

        ಯದಿ (unsetenv(name) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret)
                *ret = TAKE_PTR(a);

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ setenvf(ಧ್ರುವ ವರ್ಣ *name, bool overwrite, ಧ್ರುವ ವರ್ಣ *valuef, ...) {
        _cleanup_free_ ವರ್ಣ *ಮೌಲ್ಯ = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        ಯದಿ (!valuef)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&ಮೌಲ್ಯ, valuef, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ಯದಿ (streq_ptr(getenv(name), ಮೌಲ್ಯ))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setenv(name, ಮೌಲ್ಯ, overwrite));
}
