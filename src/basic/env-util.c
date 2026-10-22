/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "syslog-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(ଶୂନ୍ୟ) {
        long l = sysconf(_SC_ARG_MAX);
        ପ୍ରତିଜ୍ଞା(l > 0);
        ଫେରନ୍ତୁ (size_t) l;
}

ସ୍ଥିର bool env_name_is_valid_n(ଅଚଳ ଅକ୍ଷର *e, size_t n) {

        ଯଦି (n == SIZE_MAX)
                n = strlen_ptr(e);

        ଯଦି (n == 0)
                ଫେରନ୍ତୁ false;

        ପ୍ରତିଜ୍ଞା(e);

        ଯଦି (ascii_isdigit(e[0]))
                ଫେରନ୍ତୁ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        ଯଦି (_unlikely_(n > sc_arg_max() - 2))
                ଫେରନ୍ତୁ false;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = e; p < e + n; p++)
                ଯଦି (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool env_name_is_valid(ଅଚଳ ଅକ୍ଷର *e) {
        ଫେରନ୍ତୁ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_ମୂଲ୍ୟ_is_valid(ଅଚଳ ଅକ୍ଷର *e) {
        ଯଦି (!e)
                ଫେରନ୍ତୁ false;

        ଯଦି (!utf8_is_valid(e))
                ଫେରନ୍ତୁ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        ଯଦି (_unlikely_(strlen(e) > sc_arg_max() - 3))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool env_assignment_is_valid(ଅଚଳ ଅକ୍ଷର *e) {
        ଅଚଳ ଅକ୍ଷର *eq;

        ପ୍ରତିଜ୍ଞା(e);

        eq = strchr(e, '=');
        ଯଦି (!eq)
                ଫେରନ୍ତୁ false;

        ଯଦି (!env_name_is_valid_n(e, eq - e))
                ଫେରନ୍ତୁ false;

        ଯଦି (!env_ମୂଲ୍ୟ_is_valid(eq + 1))
                ଫେରନ୍ତୁ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        ଯଦି (_unlikely_(strlen(e) > sc_arg_max() - 1))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool strv_env_is_valid(ଅକ୍ଷର * ଅଚଳ *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                ଯଦି (!env_assignment_is_valid(*p))
                        ଫେରନ୍ତୁ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ଯଦି (strneq(*p, *q, k) && (*q)[k] == '=')
                                ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

bool strv_env_name_is_valid(ଅକ୍ଷର * ଅଚଳ *l) {
        STRV_FOREACH(p, l) {
                ଯଦି (!env_name_is_valid(*p))
                        ଫେରନ୍ତୁ false;

                ଯଦି (strv_contains(p + 1, *p))
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

bool strv_env_name_or_assignment_is_valid(ଅକ୍ଷର * ଅଚଳ *l) {
        STRV_FOREACH(p, l) {
                ଯଦି (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        ଫେରନ୍ତୁ false;

                ଯଦି (strv_contains(p + 1, *p))
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା env_append(ଅକ୍ଷର **e, ଅକ୍ଷର ***k, ଅକ୍ଷର **a) {
        ପ୍ରତିଜ୍ଞା(e);
        ପ୍ରତିଜ୍ଞା(k);
        ପ୍ରତିଜ୍ଞା(*k >= e);

        ଯଦି (!a)
                ଫେରନ୍ତୁ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        ପାଇଁ (; *a; a++) {
                ଅକ୍ଷର **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                ଯଦି ((*a)[n] == '=')
                        n++;

                ପାଇଁ (j = e; j < *k; j++)
                        ଯଦି (strneq(*j, *a, n))
                                ଭାଙ୍ଗ;

                c = strdup(*a);
                ଯଦି (!c)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ଖାଲି;
                        (*k)++;
                } ଅନ୍ୟଥା
                        free_and_replace(*j, c); /* Override existing item */
        }

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର** _strv_env_merge(ଅକ୍ଷର **first, ...) {
        _cleanup_strv_free_ ଅକ୍ଷର **merged = ଖାଲି;
        ଅକ୍ଷର **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        ପାଇଁ (;;) {
                ଅକ୍ଷର **l;

                l = va_arg(ap, ଅକ୍ଷର**);
                ଯଦି (l == POINTER_MAX)
                        ଭାଙ୍ଗ;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(ଅକ୍ଷର*, n + 1);
        ଯଦି (!merged)
                ଫେରନ୍ତୁ ଖାଲି;
        merged[0] = ଖାଲି;

        ଯଦି (env_append(merged, &k, first) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        va_start(ap, first);
        ପାଇଁ (;;) {
                ଅକ୍ଷର **l;

                l = va_arg(ap, ଅକ୍ଷର**);
                ଯଦି (l == POINTER_MAX)
                        ଭାଙ୍ଗ;

                ଯଦି (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        ଫେରନ୍ତୁ ଖାଲି;
                }
        }
        va_end(ap);

        ଫେରନ୍ତୁ TAKE_PTR(merged);
}

ସ୍ଥିର bool env_match(ଅଚଳ ଅକ୍ଷର *t, ଅଚଳ ଅକ୍ଷର *pattern) {
        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        ଯଦି (streq(t, pattern))
                ଫେରନ୍ତୁ true;

        ଯଦି (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                ଫେରନ୍ତୁ t && *t == '=';
        }

        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର bool env_entry_has_name(ଅଚଳ ଅକ୍ଷର *entry, ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(entry);
        ପ୍ରତିଜ୍ଞା(name);

        t = startswith(entry, name);
        ଯଦି (!t)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ *t == '=';
}

ଅକ୍ଷର** strv_env_delete(ଅକ୍ଷର **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ ଅକ୍ଷର **t = ଖାଲି;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(ଅକ୍ଷର*, n+1);
        ଯଦି (!t)
                ଫେରନ୍ତୁ ଖାଲି;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                ପାଇଁ (size_t v = 0; v < n_lists; v++) {
                        ଅକ୍ଷର **l;

                        l = va_arg(ap, ଅକ୍ଷର**);
                        STRV_FOREACH(j, l)
                                ଯଦି (env_match(*k, *j))
                                        ଯାଅ skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                ଯଦି (!t[i])
                        ଫେରନ୍ତୁ ଖାଲି;

                i++;
                ଜାରି;

        skip:
                va_end(ap);
        }

        t[i] = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(i <= n);

        ଫେରନ୍ତୁ TAKE_PTR(t);
}

ଅକ୍ଷର** strv_env_unset(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *p) {
        ପ୍ରତିଜ୍ଞା(p);

        ଯଦି (!l)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        ଅକ୍ଷର **f, **t;
        ପାଇଁ (f = t = l; *f; f++) {
                ଯଦି (env_match(*f, p)) {
                        ମୁକ୍ତ(*f);
                        ଜାରି;
                }

                *(t++) = *f;
        }

        *t = ଖାଲି;
        ଫେରନ୍ତୁ l;
}

ଅକ୍ଷର** strv_env_unset_many_internal(ଅକ୍ଷର **l, ...) {
        ଯଦି (!l)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        ଅକ୍ଷର **f, **t;
        ପାଇଁ (f = t = l; *f; f++) {
                bool found = false;
                ଅଚଳ ଅକ୍ଷର *p;
                va_list ap;

                va_start(ap, l);

                ଯେତେବେଳେ ((p = va_arg(ap, ଅଚଳ ଅକ୍ଷର*)))
                        ଯଦି (env_match(*f, p)) {
                                found = true;
                                ଭାଙ୍ଗ;
                        }

                va_end(ap);

                ଯଦି (found) {
                        ମୁକ୍ତ(*f);
                        ଜାରି;
                }

                *(t++) = *f;
        }

        *t = ଖାଲି;
        ଫେରନ୍ତୁ l;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_consume(ଅକ୍ଷର ***l, ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *t, *name;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(l);
        ପ୍ରତିଜ୍ଞା(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        ଯଦି (!t) {
                ମୁକ୍ତ(p);
                ଫେରନ୍ତୁ -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                ଯଦି (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        ଫେରନ୍ତୁ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_strdup(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ପ୍ରତିଜ୍ଞା(l);
        ପ୍ରତିଜ୍ଞା(assignment);

        ଅକ୍ଷର *p = strdup(assignment);
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_env_replace_consume(l, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_replace_strdup_passthrough(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *assignment) {
        ଅକ୍ଷର *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ପ୍ରତିଜ୍ଞା(l);
        ପ୍ରତିଜ୍ଞା(assignment);

        ଯଦି (strchr(assignment, '=')) {
                ଯଦି (!env_assignment_is_valid(assignment))
                        ଫେରନ୍ତୁ -EINVAL;

                p = strdup(assignment);
        } ଅନ୍ୟଥା {
                ଯଦି (!env_name_is_valid(assignment))
                        ଫେରନ୍ତୁ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_env_replace_consume(l, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_assign(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପ୍ରତିଜ୍ଞା(l);

        ଯଦି (!env_name_is_valid(ଚାବି))
                ଫେରନ୍ତୁ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        ଯଦି (!ମୂଲ୍ୟ) {
                strv_env_unset(*l, ଚାବି);
                ଫେରନ୍ତୁ 0;
        }

        ଅକ୍ଷର *p = strjoin(ଚାବି, "=", ମୂଲ୍ୟ);
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_env_replace_consume(l, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_assignf(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *valuef, ...) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(l);
        ପ୍ରତିଜ୍ଞା(ଚାବି);

        ଯଦି (!env_name_is_valid(ଚାବି))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!valuef) {
                strv_env_unset(*l, ଚାବି);
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&ମୂଲ୍ୟ, valuef, ap);
        va_end(ap);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର *p = strjoin(ଚାବି, "=", ମୂଲ୍ୟ);
        ଯଦି (!p)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_env_replace_consume(l, p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _strv_env_assign_many(ଅକ୍ଷର ***l, ...) {
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(l);

        va_start(ap, l);
        ପାଇଁ (;;) {
                ଅଚଳ ଅକ୍ଷର *ଚାବି, *ମୂଲ୍ୟ;

                ଚାବି = va_arg(ap, ଅଚଳ ଅକ୍ଷର *);
                ଯଦି (!ଚାବି)
                        ଭାଙ୍ଗ;

                ଯଦି (!env_name_is_valid(ଚାବି)) {
                        va_end(ap);
                        ଫେରନ୍ତୁ -EINVAL;
                }

                ମୂଲ୍ୟ = va_arg(ap, ଅଚଳ ଅକ୍ଷର *);
                ଯଦି (!ମୂଲ୍ୟ) {
                        strv_env_unset(*l, ଚାବି);
                        ଜାରି;
                }

                ଅକ୍ଷର *p = strjoin(ଚାବି, "=", ମୂଲ୍ୟ);
                ଯଦି (!p) {
                        va_end(ap);
                        ଫେରନ୍ତୁ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                ଯଦି (r < 0) {
                        va_end(ap);
                        ଫେରନ୍ତୁ r;
                }
        }
        va_end(ap);

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* strv_env_get_n(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *name, size_t k, ReplaceEnvFlags flags) {
        ପ୍ରତିଜ୍ଞା(name);

        ଯଦି (k == SIZE_MAX)
                k = strlen(name);
        ଯଦି (k <= 0)
                ଫେରନ୍ତୁ ଖାଲି;

        STRV_FOREACH_BACKWARDS(i, l)
                ଯଦି (strneq(*i, name, k) && (*i)[k] == '=')
                        ଫେରନ୍ତୁ (ଅକ୍ଷର*) *i + k + 1;

        ଯଦି (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                ଅଚଳ ଅକ୍ଷର *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                ଯଦି (k > sc_arg_max() - 2)
                        ଫେରନ୍ତୁ ଖାଲି;

                t = strndupa_safe(name, k);
                ଫେରନ୍ତୁ secure_getenv(t);
        };

        ଫେରନ୍ତୁ ଖାଲି;
}

ଅକ୍ଷର* strv_env_pairs_get(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *name) {
        ଅକ୍ଷର *result = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(name);

        STRV_FOREACH_PAIR(ଚାବି, ମୂଲ୍ୟ, l)
                ଯଦି (streq(*ଚାବି, name))
                        result = *ମୂଲ୍ୟ;

        ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_get_merged(ଅକ୍ଷର **l, ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **v = ଖାଲି;
        size_t n = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(ଚାବି, ମୂଲ୍ୟ, l) {
                ଅକ୍ଷର *s;

                s = strjoin(*ଚାବି, "=", *ମୂଲ୍ୟ);
                ଯଦି (!s)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(v);
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର** strv_env_clean_with_callback(ଅକ୍ଷର **l, ଶୂନ୍ୟ (*invalid_callback)(ଅଚଳ ଅକ୍ଷର *p, ଶୂନ୍ୟ *userdata), ଶୂନ୍ୟ *userdata) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                ଯଦି (!env_assignment_is_valid(*p)) {
                        ଯଦି (invalid_callback)
                                invalid_callback(*p, userdata);
                        ମୁକ୍ତ(*p);
                        ଜାରି;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        ଯଦି (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                ଭାଙ୍ଗ;
                        }

                ଯଦି (duplicate) {
                        ମୁକ୍ତ(*p);
                        ଜାରି;
                }

                l[k++] = *p;
        }

        ଯଦି (l)
                l[k] = ଖାଲି;

        ଫେରନ୍ତୁ l;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_extend_with_length(ଅକ୍ଷର ***l, ଅଚଳ ଅକ୍ଷର *s, size_t n) {
        ଅକ୍ଷର *c;

        c = strndup(s, n);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ strv_consume(l, c);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା strv_env_get_n_validated(
                ଅକ୍ଷର **env,
                ଅଚଳ ଅକ୍ଷର *name,
                size_t l,
                ReplaceEnvFlags flags,
                ଅକ୍ଷର **ret,              /* points into the env block! do not free! */
                ଅକ୍ଷର ***unset_variables, /* updated in place */
                ଅକ୍ଷର ***bad_variables) { /* ditto */

        ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(l == 0 || name);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                ଯଦି (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        } ଅନ୍ୟଥା {
                e = ଖାଲି; /* Resolve invalid variable names the same way as unset ones */

                ଯଦି (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        }

        *ret = e;
        ଫେରନ୍ତୁ !!e;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା replace_env_full(
                ଅଚଳ ଅକ୍ଷର *format,
                size_t length,
                ଅକ୍ଷର **env,
                ReplaceEnvFlags flags,
                ଅକ୍ଷର **ret,
                ଅକ୍ଷର ***ret_unset_variables,
                ଅକ୍ଷର ***ret_bad_variables) {

        ଗଣନା {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_ମୂଲ୍ୟ,
                ALTERNATE_ମୂଲ୍ୟ,
        } state = WORD;

        _cleanup_strv_free_ ଅକ୍ଷର **unset_variables = ଖାଲି, **bad_variables = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e, *word = format, *test_ମୂଲ୍ୟ = ଖାଲି; /* test_value is initialized to appease gcc */
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ଅକ୍ଷର ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା nest = 0, r;

        ପ୍ରତିଜ୍ଞା(format);

        ଯଦି (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : ଖାଲି;
        pb = ret_bad_variables ? &bad_variables : ଖାଲି;

        ପାଇଁ (e = format, i = 0; *e && i < length; e++, i++)
                ବାଛ (state) {

                ସ୍ଥିତି WORD:
                        ଯଦି (*e == '$')
                                state = CURLY;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି CURLY:
                        ଯଦି (*e == '{') {
                                ଯଦି (!strextendn(&s, word, e-word-1))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } ଅନ୍ୟଥା ଯଦି (*e == '$') {
                                ଯଦି (!strextendn(&s, word, e-word))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ଯଦି (!strextendn(&s, word, e-word-1))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } ଅନ୍ୟଥା
                                state = WORD;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି VARIABLE:
                        ଯଦି (*e == '}') {
                                ଅକ୍ଷର *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (!strextend(&s, t))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } ଅନ୍ୟଥା ଯଦି (*e == ':') {
                                ଯଦି (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } ଅନ୍ୟଥା
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି TEST:
                        ଯଦି (*e == '-')
                                state = DEFAULT_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା ଯଦି (*e == '+')
                                state = ALTERNATE_ମୂଲ୍ୟ;
                        ଅନ୍ୟଥା {
                                state = WORD;
                                ଭାଙ୍ଗ;
                        }

                        test_ମୂଲ୍ୟ = e+1;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DEFAULT_ମୂଲ୍ୟ: /* fall through */
                ସ୍ଥିତି ALTERNATE_ମୂଲ୍ୟ:
                        ପ୍ରତିଜ୍ଞା(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        ଯଦି (*e == '{') {
                                nest++;
                                ଭାଙ୍ଗ;
                        }

                        ଯଦି (*e != '}')
                                ଭାଙ୍ଗ;

                        nest--;
                        ଯଦି (nest == 0) {
                                _cleanup_strv_free_ ଅକ୍ଷର **u = ଖାଲି, **b = ଖାଲି;
                                _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
                                ଅକ୍ଷର *t = ଖାଲି;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (t && state == ALTERNATE_ମୂଲ୍ୟ) {
                                        r = replace_env_full(test_ମୂଲ୍ୟ, e-test_ମୂଲ୍ୟ, env, flags, &v, pu ? &u : ଖାଲି, pb ? &b : ଖାଲି);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ r;

                                        t = v;
                                } ଅନ୍ୟଥା ଯଦି (!t && state == DEFAULT_ମୂଲ୍ୟ) {
                                        r = replace_env_full(test_ମୂଲ୍ୟ, e-test_ମୂଲ୍ୟ, env, flags, &v, pu ? &u : ଖାଲି, pb ? &b : ଖାଲି);
                                        ଯଦି (r < 0)
                                                ଫେରନ୍ତୁ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (!strextend(&s, t))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି VARIABLE_RAW:
                        ପ୍ରତିଜ୍ଞା(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        ଯଦି (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                ଅକ୍ଷର *t = ଖାଲି;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ଯଦି (!strextend(&s, t))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        ଭାଙ୍ଗ;
                }

        ଯଦି (state == VARIABLE_RAW) {
                ଅକ୍ଷର *t;

                ପ୍ରତିଜ୍ଞା(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (!strextend(&s, t))
                        ଫେରନ୍ତୁ -ENOMEM;

        } ଅନ୍ୟଥା ଯଦି (!strextendn(&s, word, e-word))
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        ଯଦି (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        ଯଦି (ret)
                *ret = TAKE_PTR(s);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା replace_env_argv(
                ଅକ୍ଷର **argv,
                ଅକ୍ଷର **env,
                ଅକ୍ଷର ***ret,
                ଅକ୍ଷର ***ret_unset_variables,
                ଅକ୍ଷର ***ret_bad_variables) {

        _cleanup_strv_free_ ଅକ୍ଷର **n = ଖାଲି, **unset_variables = ଖାଲି, **bad_variables = ଖାଲି;
        size_t k = 0, l;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(!strv_isempty(argv));
        ପ୍ରତିଜ୍ଞା(ret);

        l = strv_length(argv);

        n = new(ଅକ୍ଷର*, l+1);
        ଯଦି (!n)
                ଫେରନ୍ତୁ -ENOMEM;

        STRV_FOREACH(i, argv) {
                ଅଚଳ ଅକ୍ଷର *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                ଯଦି (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ ଅକ୍ଷର **m = ଖାଲି;
                        ଅଚଳ ଅକ୍ଷର *name = word + 1;
                        ଅକ୍ଷର *e, **w;
                        size_t q;

                        ଯଦି (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                ଯଦି (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                ଅନ୍ୟଥା ଯଦି (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                ଅନ୍ୟଥା
                                        r = 0;
                        } ଅନ୍ୟଥା ଯଦି (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        ଅନ୍ୟଥା
                                r = 0;
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, ଆକାର(ଅକ୍ଷର*));
                        ଯଦି (!w)
                                ଫେରନ୍ତୁ -ENOMEM;

                        n = w;
                        ଯଦି (m) {
                                memcpy(n + k, m, (q + 1) * ଆକାର(ଅକ୍ଷର*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ଖାଲି;
                        ଜାରି;
                }

                _cleanup_strv_free_ ଅକ୍ଷର **u = ଖାଲି, **b = ଖାଲି;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ଖାଲି,
                                ret_bad_variables ? &b : ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                n[++k] = ଖାଲି;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        ଯଦି (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_bool(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *e;

        e = getenv(p);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        ଫେରନ୍ତୁ parse_boolean(e);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure_getenv_bool(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *e;

        e = secure_getenv(p);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        ଫେରନ୍ତୁ parse_boolean(e);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure_getenv_uint64(ଅଚଳ ଅକ୍ଷର *p, uint64_t *ret) {
        ଅଚଳ ଅକ୍ଷର *e;

        ପ୍ରତିଜ୍ଞା(p);

        e = secure_getenv(p);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        ଫେରନ୍ତୁ safe_atou64(e, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_unset_env(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, bool overwrite) {
        ପ୍ରତିଜ୍ଞା(name);

        ଯଦି (ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ RET_NERRNO(setenv(name, ମୂଲ୍ୟ, overwrite));

        ଫେରନ୍ତୁ RET_NERRNO(unsetenv(name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା putenv_dup(ଅଚଳ ଅକ୍ଷର *assignment, bool override) {
        ଅଚଳ ଅକ୍ଷର *e, *n;

        e = strchr(assignment, '=');
        ଯଦି (!e)
                ଫେରନ୍ତୁ -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        ଫେରନ୍ତୁ RET_NERRNO(setenv(n, e + 1, override));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenv_systemd_exec_pid(bool update_only) {
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        ଯଦି (!e && update_only)
                ଫେରନ୍ତୁ 0;

        ଯଦି (streq_ptr(e, "*"))
                ଫେରନ୍ତୁ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenv_systemd_log_level(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *val = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_path_list(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର ***ret_paths) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(ret_paths);

        e = secure_getenv(name);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                ଯଦି (!path_is_absolute(*p))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                ଯଦି (!path_is_normalized(*p))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                ଯଦି (path_equal(*p, "/"))
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        ଯଦି (strv_isempty(l))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_steal_erase(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret) {
        _cleanup_(erase_and_freep) ଅକ୍ଷର *a = ଖାଲି;
        ଅକ୍ଷର *e;

        ପ୍ରତିଜ୍ଞା(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        ଯଦି (!e) {
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (ret) {
                a = strdup(e);
                ଯଦି (!a)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        string_erase(e);

        ଯଦି (unsetenv(name) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret)
                *ret = TAKE_PTR(a);

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setenvf(ଅଚଳ ଅକ୍ଷର *name, bool overwrite, ଅଚଳ ଅକ୍ଷର *valuef, ...) {
        _cleanup_free_ ଅକ୍ଷର *ମୂଲ୍ୟ = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);

        ଯଦି (!valuef)
                ଫେରନ୍ତୁ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&ମୂଲ୍ୟ, valuef, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        ଯଦି (streq_ptr(getenv(name), ମୂଲ୍ୟ))
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ RET_NERRNO(setenv(name, ମୂଲ୍ୟ, overwrite));
}
