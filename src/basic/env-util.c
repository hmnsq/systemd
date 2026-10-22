/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "syslog-util.h"
#അന്തര്ഭാവ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(ശൂന്യം) {
        long l = sysconf(_SC_ARG_MAX);
        പ്രതിജ്ഞാ(l > 0);
        തിരികെ (size_t) l;
}

സ്ഥിതം bool env_name_is_valid_n(സ്ഥിരം അക്ഷരം *e, size_t n) {

        എങ്കിൽ (n == SIZE_MAX)
                n = strlen_ptr(e);

        എങ്കിൽ (n == 0)
                തിരികെ false;

        പ്രതിജ്ഞാ(e);

        എങ്കിൽ (ascii_isdigit(e[0]))
                തിരികെ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        എങ്കിൽ (_unlikely_(n > sc_arg_max() - 2))
                തിരികെ false;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = e; p < e + n; p++)
                എങ്കിൽ (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        തിരികെ false;

        തിരികെ true;
}

bool env_name_is_valid(സ്ഥിരം അക്ഷരം *e) {
        തിരികെ env_name_is_valid_n(e, SIZE_MAX);
}

bool env_മൂല്യം_is_valid(സ്ഥിരം അക്ഷരം *e) {
        എങ്കിൽ (!e)
                തിരികെ false;

        എങ്കിൽ (!utf8_is_valid(e))
                തിരികെ false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        എങ്കിൽ (_unlikely_(strlen(e) > sc_arg_max() - 3))
                തിരികെ false;

        തിരികെ true;
}

bool env_assignment_is_valid(സ്ഥിരം അക്ഷരം *e) {
        സ്ഥിരം അക്ഷരം *eq;

        പ്രതിജ്ഞാ(e);

        eq = strchr(e, '=');
        എങ്കിൽ (!eq)
                തിരികെ false;

        എങ്കിൽ (!env_name_is_valid_n(e, eq - e))
                തിരികെ false;

        എങ്കിൽ (!env_മൂല്യം_is_valid(eq + 1))
                തിരികെ false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        എങ്കിൽ (_unlikely_(strlen(e) > sc_arg_max() - 1))
                തിരികെ false;

        തിരികെ true;
}

bool strv_env_is_valid(അക്ഷരം * സ്ഥിരം *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                എങ്കിൽ (!env_assignment_is_valid(*p))
                        തിരികെ false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        എങ്കിൽ (strneq(*p, *q, k) && (*q)[k] == '=')
                                തിരികെ false;
        }

        തിരികെ true;
}

bool strv_env_name_is_valid(അക്ഷരം * സ്ഥിരം *l) {
        STRV_FOREACH(p, l) {
                എങ്കിൽ (!env_name_is_valid(*p))
                        തിരികെ false;

                എങ്കിൽ (strv_contains(p + 1, *p))
                        തിരികെ false;
        }

        തിരികെ true;
}

bool strv_env_name_or_assignment_is_valid(അക്ഷരം * സ്ഥിരം *l) {
        STRV_FOREACH(p, l) {
                എങ്കിൽ (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        തിരികെ false;

                എങ്കിൽ (strv_contains(p + 1, *p))
                        തിരികെ false;
        }

        തിരികെ true;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ env_append(അക്ഷരം **e, അക്ഷരം ***k, അക്ഷരം **a) {
        പ്രതിജ്ഞാ(e);
        പ്രതിജ്ഞാ(k);
        പ്രതിജ്ഞാ(*k >= e);

        എങ്കിൽ (!a)
                തിരികെ 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        വേണ്ടി (; *a; a++) {
                അക്ഷരം **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                എങ്കിൽ ((*a)[n] == '=')
                        n++;

                വേണ്ടി (j = e; j < *k; j++)
                        എങ്കിൽ (strneq(*j, *a, n))
                                നിർത്തുക;

                c = strdup(*a);
                എങ്കിൽ (!c)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = ശൂന്യ;
                        (*k)++;
                } അല്ലെങ്കിൽ
                        free_and_replace(*j, c); /* Override existing item */
        }

        തിരികെ 0;
}

അക്ഷരം** _strv_env_merge(അക്ഷരം **first, ...) {
        _cleanup_strv_free_ അക്ഷരം **merged = ശൂന്യ;
        അക്ഷരം **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        വേണ്ടി (;;) {
                അക്ഷരം **l;

                l = va_arg(ap, അക്ഷരം**);
                എങ്കിൽ (l == POINTER_MAX)
                        നിർത്തുക;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(അക്ഷരം*, n + 1);
        എങ്കിൽ (!merged)
                തിരികെ ശൂന്യ;
        merged[0] = ശൂന്യ;

        എങ്കിൽ (env_append(merged, &k, first) < 0)
                തിരികെ ശൂന്യ;

        va_start(ap, first);
        വേണ്ടി (;;) {
                അക്ഷരം **l;

                l = va_arg(ap, അക്ഷരം**);
                എങ്കിൽ (l == POINTER_MAX)
                        നിർത്തുക;

                എങ്കിൽ (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        തിരികെ ശൂന്യ;
                }
        }
        va_end(ap);

        തിരികെ TAKE_PTR(merged);
}

സ്ഥിതം bool env_match(സ്ഥിരം അക്ഷരം *t, സ്ഥിരം അക്ഷരം *pattern) {
        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        എങ്കിൽ (streq(t, pattern))
                തിരികെ true;

        എങ്കിൽ (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                തിരികെ t && *t == '=';
        }

        തിരികെ false;
}

സ്ഥിതം bool env_entry_has_name(സ്ഥിരം അക്ഷരം *entry, സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം അക്ഷരം *t;

        പ്രതിജ്ഞാ(entry);
        പ്രതിജ്ഞാ(name);

        t = startswith(entry, name);
        എങ്കിൽ (!t)
                തിരികെ false;

        തിരികെ *t == '=';
}

അക്ഷരം** strv_env_delete(അക്ഷരം **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ അക്ഷരം **t = ശൂന്യ;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(അക്ഷരം*, n+1);
        എങ്കിൽ (!t)
                തിരികെ ശൂന്യ;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                വേണ്ടി (size_t v = 0; v < n_lists; v++) {
                        അക്ഷരം **l;

                        l = va_arg(ap, അക്ഷരം**);
                        STRV_FOREACH(j, l)
                                എങ്കിൽ (env_match(*k, *j))
                                        പോകുക skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                എങ്കിൽ (!t[i])
                        തിരികെ ശൂന്യ;

                i++;
                തുടരുക;

        skip:
                va_end(ap);
        }

        t[i] = ശൂന്യ;

        പ്രതിജ്ഞാ(i <= n);

        തിരികെ TAKE_PTR(t);
}

അക്ഷരം** strv_env_unset(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *p) {
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (!l)
                തിരികെ ശൂന്യ;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        അക്ഷരം **f, **t;
        വേണ്ടി (f = t = l; *f; f++) {
                എങ്കിൽ (env_match(*f, p)) {
                        സ്വതന്ത്രം(*f);
                        തുടരുക;
                }

                *(t++) = *f;
        }

        *t = ശൂന്യ;
        തിരികെ l;
}

അക്ഷരം** strv_env_unset_many_internal(അക്ഷരം **l, ...) {
        എങ്കിൽ (!l)
                തിരികെ ശൂന്യ;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        അക്ഷരം **f, **t;
        വേണ്ടി (f = t = l; *f; f++) {
                bool found = false;
                സ്ഥിരം അക്ഷരം *p;
                va_list ap;

                va_start(ap, l);

                ആകുമ്പോൾ ((p = va_arg(ap, സ്ഥിരം അക്ഷരം*)))
                        എങ്കിൽ (env_match(*f, p)) {
                                found = true;
                                നിർത്തുക;
                        }

                va_end(ap);

                എങ്കിൽ (found) {
                        സ്വതന്ത്രം(*f);
                        തുടരുക;
                }

                *(t++) = *f;
        }

        *t = ശൂന്യ;
        തിരികെ l;
}

പൂർണ്ണസംഖ്യ strv_env_replace_consume(അക്ഷരം ***l, അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *t, *name;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(l);
        പ്രതിജ്ഞാ(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        എങ്കിൽ (!t) {
                സ്വതന്ത്രം(p);
                തിരികെ -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                എങ്കിൽ (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        തിരികെ 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ strv_env_replace_strdup(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        പ്രതിജ്ഞാ(l);
        പ്രതിജ്ഞാ(assignment);

        അക്ഷരം *p = strdup(assignment);
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        തിരികെ strv_env_replace_consume(l, p);
}

പൂർണ്ണസംഖ്യ strv_env_replace_strdup_passthrough(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *assignment) {
        അക്ഷരം *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        പ്രതിജ്ഞാ(l);
        പ്രതിജ്ഞാ(assignment);

        എങ്കിൽ (strchr(assignment, '=')) {
                എങ്കിൽ (!env_assignment_is_valid(assignment))
                        തിരികെ -EINVAL;

                p = strdup(assignment);
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (!env_name_is_valid(assignment))
                        തിരികെ -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        തിരികെ strv_env_replace_consume(l, p);
}

പൂർണ്ണസംഖ്യ strv_env_assign(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പ്രതിജ്ഞാ(l);

        എങ്കിൽ (!env_name_is_valid(താക്കോൽ))
                തിരികെ -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        എങ്കിൽ (!മൂല്യം) {
                strv_env_unset(*l, താക്കോൽ);
                തിരികെ 0;
        }

        അക്ഷരം *p = strjoin(താക്കോൽ, "=", മൂല്യം);
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        തിരികെ strv_env_replace_consume(l, p);
}

പൂർണ്ണസംഖ്യ strv_env_assignf(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *valuef, ...) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(l);
        പ്രതിജ്ഞാ(താക്കോൽ);

        എങ്കിൽ (!env_name_is_valid(താക്കോൽ))
                തിരികെ -EINVAL;

        എങ്കിൽ (!valuef) {
                strv_env_unset(*l, താക്കോൽ);
                തിരികെ 0;
        }

        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&മൂല്യം, valuef, ap);
        va_end(ap);
        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        അക്ഷരം *p = strjoin(താക്കോൽ, "=", മൂല്യം);
        എങ്കിൽ (!p)
                തിരികെ -ENOMEM;

        തിരികെ strv_env_replace_consume(l, p);
}

പൂർണ്ണസംഖ്യ _strv_env_assign_many(അക്ഷരം ***l, ...) {
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(l);

        va_start(ap, l);
        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *താക്കോൽ, *മൂല്യം;

                താക്കോൽ = va_arg(ap, സ്ഥിരം അക്ഷരം *);
                എങ്കിൽ (!താക്കോൽ)
                        നിർത്തുക;

                എങ്കിൽ (!env_name_is_valid(താക്കോൽ)) {
                        va_end(ap);
                        തിരികെ -EINVAL;
                }

                മൂല്യം = va_arg(ap, സ്ഥിരം അക്ഷരം *);
                എങ്കിൽ (!മൂല്യം) {
                        strv_env_unset(*l, താക്കോൽ);
                        തുടരുക;
                }

                അക്ഷരം *p = strjoin(താക്കോൽ, "=", മൂല്യം);
                എങ്കിൽ (!p) {
                        va_end(ap);
                        തിരികെ -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                എങ്കിൽ (r < 0) {
                        va_end(ap);
                        തിരികെ r;
                }
        }
        va_end(ap);

        തിരികെ 0;
}

അക്ഷരം* strv_env_get_n(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *name, size_t k, ReplaceEnvFlags flags) {
        പ്രതിജ്ഞാ(name);

        എങ്കിൽ (k == SIZE_MAX)
                k = strlen(name);
        എങ്കിൽ (k <= 0)
                തിരികെ ശൂന്യ;

        STRV_FOREACH_BACKWARDS(i, l)
                എങ്കിൽ (strneq(*i, name, k) && (*i)[k] == '=')
                        തിരികെ (അക്ഷരം*) *i + k + 1;

        എങ്കിൽ (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                സ്ഥിരം അക്ഷരം *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                എങ്കിൽ (k > sc_arg_max() - 2)
                        തിരികെ ശൂന്യ;

                t = strndupa_safe(name, k);
                തിരികെ secure_getenv(t);
        };

        തിരികെ ശൂന്യ;
}

അക്ഷരം* strv_env_pairs_get(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *name) {
        അക്ഷരം *result = ശൂന്യ;

        പ്രതിജ്ഞാ(name);

        STRV_FOREACH_PAIR(താക്കോൽ, മൂല്യം, l)
                എങ്കിൽ (streq(*താക്കോൽ, name))
                        result = *മൂല്യം;

        തിരികെ result;
}

പൂർണ്ണസംഖ്യ strv_env_get_merged(അക്ഷരം **l, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **v = ശൂന്യ;
        size_t n = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(താക്കോൽ, മൂല്യം, l) {
                അക്ഷരം *s;

                s = strjoin(*താക്കോൽ, "=", *മൂല്യം);
                എങ്കിൽ (!s)
                        തിരികെ -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(v);
        തിരികെ 0;
}

അക്ഷരം** strv_env_clean_with_callback(അക്ഷരം **l, ശൂന്യം (*invalid_callback)(സ്ഥിരം അക്ഷരം *p, ശൂന്യം *userdata), ശൂന്യം *userdata) {
        പൂർണ്ണസംഖ്യ k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                എങ്കിൽ (!env_assignment_is_valid(*p)) {
                        എങ്കിൽ (invalid_callback)
                                invalid_callback(*p, userdata);
                        സ്വതന്ത്രം(*p);
                        തുടരുക;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        എങ്കിൽ (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                നിർത്തുക;
                        }

                എങ്കിൽ (duplicate) {
                        സ്വതന്ത്രം(*p);
                        തുടരുക;
                }

                l[k++] = *p;
        }

        എങ്കിൽ (l)
                l[k] = ശൂന്യ;

        തിരികെ l;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ strv_extend_with_length(അക്ഷരം ***l, സ്ഥിരം അക്ഷരം *s, size_t n) {
        അക്ഷരം *c;

        c = strndup(s, n);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        തിരികെ strv_consume(l, c);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ strv_env_get_n_validated(
                അക്ഷരം **env,
                സ്ഥിരം അക്ഷരം *name,
                size_t l,
                ReplaceEnvFlags flags,
                അക്ഷരം **ret,              /* points into the env block! do not free! */
                അക്ഷരം ***unset_variables, /* updated in place */
                അക്ഷരം ***bad_variables) { /* ditto */

        അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(l == 0 || name);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                എങ്കിൽ (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }
        } അല്ലെങ്കിൽ {
                e = ശൂന്യ; /* Resolve invalid variable names the same way as unset ones */

                എങ്കിൽ (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }
        }

        *ret = e;
        തിരികെ !!e;
}

പൂർണ്ണസംഖ്യ replace_env_full(
                സ്ഥിരം അക്ഷരം *format,
                size_t length,
                അക്ഷരം **env,
                ReplaceEnvFlags flags,
                അക്ഷരം **ret,
                അക്ഷരം ***ret_unset_variables,
                അക്ഷരം ***ret_bad_variables) {

        എണ്ണൽ {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_മൂല്യം,
                ALTERNATE_മൂല്യം,
        } state = WORD;

        _cleanup_strv_free_ അക്ഷരം **unset_variables = ശൂന്യ, **bad_variables = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e, *word = format, *test_മൂല്യം = ശൂന്യ; /* test_value is initialized to appease gcc */
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        അക്ഷരം ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        പൂർണ്ണസംഖ്യ nest = 0, r;

        പ്രതിജ്ഞാ(format);

        എങ്കിൽ (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : ശൂന്യ;
        pb = ret_bad_variables ? &bad_variables : ശൂന്യ;

        വേണ്ടി (e = format, i = 0; *e && i < length; e++, i++)
                തിരഞ്ഞെടുപ്പ് (state) {

                അവസ്ഥ WORD:
                        എങ്കിൽ (*e == '$')
                                state = CURLY;
                        നിർത്തുക;

                അവസ്ഥ CURLY:
                        എങ്കിൽ (*e == '{') {
                                എങ്കിൽ (!strextendn(&s, word, e-word-1))
                                        തിരികെ -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } അല്ലെങ്കിൽ എങ്കിൽ (*e == '$') {
                                എങ്കിൽ (!strextendn(&s, word, e-word))
                                        തിരികെ -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                എങ്കിൽ (!strextendn(&s, word, e-word-1))
                                        തിരികെ -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } അല്ലെങ്കിൽ
                                state = WORD;
                        നിർത്തുക;

                അവസ്ഥ VARIABLE:
                        എങ്കിൽ (*e == '}') {
                                അക്ഷരം *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (!strextend(&s, t))
                                        തിരികെ -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } അല്ലെങ്കിൽ എങ്കിൽ (*e == ':') {
                                എങ്കിൽ (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } അല്ലെങ്കിൽ
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        നിർത്തുക;

                അവസ്ഥ TEST:
                        എങ്കിൽ (*e == '-')
                                state = DEFAULT_മൂല്യം;
                        അല്ലെങ്കിൽ എങ്കിൽ (*e == '+')
                                state = ALTERNATE_മൂല്യം;
                        അല്ലെങ്കിൽ {
                                state = WORD;
                                നിർത്തുക;
                        }

                        test_മൂല്യം = e+1;
                        നിർത്തുക;

                അവസ്ഥ DEFAULT_മൂല്യം: /* fall through */
                അവസ്ഥ ALTERNATE_മൂല്യം:
                        പ്രതിജ്ഞാ(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        എങ്കിൽ (*e == '{') {
                                nest++;
                                നിർത്തുക;
                        }

                        എങ്കിൽ (*e != '}')
                                നിർത്തുക;

                        nest--;
                        എങ്കിൽ (nest == 0) {
                                _cleanup_strv_free_ അക്ഷരം **u = ശൂന്യ, **b = ശൂന്യ;
                                _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
                                അക്ഷരം *t = ശൂന്യ;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (t && state == ALTERNATE_മൂല്യം) {
                                        r = replace_env_full(test_മൂല്യം, e-test_മൂല്യം, env, flags, &v, pu ? &u : ശൂന്യ, pb ? &b : ശൂന്യ);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ r;

                                        t = v;
                                } അല്ലെങ്കിൽ എങ്കിൽ (!t && state == DEFAULT_മൂല്യം) {
                                        r = replace_env_full(test_മൂല്യം, e-test_മൂല്യം, env, flags, &v, pu ? &u : ശൂന്യ, pb ? &b : ശൂന്യ);
                                        എങ്കിൽ (r < 0)
                                                തിരികെ r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (!strextend(&s, t))
                                        തിരികെ -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        നിർത്തുക;

                അവസ്ഥ VARIABLE_RAW:
                        പ്രതിജ്ഞാ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        എങ്കിൽ (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                അക്ഷരം *t = ശൂന്യ;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (!strextend(&s, t))
                                        തിരികെ -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        നിർത്തുക;
                }

        എങ്കിൽ (state == VARIABLE_RAW) {
                അക്ഷരം *t;

                പ്രതിജ്ഞാ(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (!strextend(&s, t))
                        തിരികെ -ENOMEM;

        } അല്ലെങ്കിൽ എങ്കിൽ (!strextendn(&s, word, e-word))
                തിരികെ -ENOMEM;

        എങ്കിൽ (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        എങ്കിൽ (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(s);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ replace_env_argv(
                അക്ഷരം **argv,
                അക്ഷരം **env,
                അക്ഷരം ***ret,
                അക്ഷരം ***ret_unset_variables,
                അക്ഷരം ***ret_bad_variables) {

        _cleanup_strv_free_ അക്ഷരം **n = ശൂന്യ, **unset_variables = ശൂന്യ, **bad_variables = ശൂന്യ;
        size_t k = 0, l;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(!strv_isempty(argv));
        പ്രതിജ്ഞാ(ret);

        l = strv_length(argv);

        n = new(അക്ഷരം*, l+1);
        എങ്കിൽ (!n)
                തിരികെ -ENOMEM;

        STRV_FOREACH(i, argv) {
                സ്ഥിരം അക്ഷരം *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                എങ്കിൽ (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ അക്ഷരം **m = ശൂന്യ;
                        സ്ഥിരം അക്ഷരം *name = word + 1;
                        അക്ഷരം *e, **w;
                        size_t q;

                        എങ്കിൽ (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                എങ്കിൽ (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                അല്ലെങ്കിൽ എങ്കിൽ (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                അല്ലെങ്കിൽ
                                        r = 0;
                        } അല്ലെങ്കിൽ എങ്കിൽ (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        അല്ലെങ്കിൽ
                                r = 0;
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, വലുപ്പം(അക്ഷരം*));
                        എങ്കിൽ (!w)
                                തിരികെ -ENOMEM;

                        n = w;
                        എങ്കിൽ (m) {
                                memcpy(n + k, m, (q + 1) * വലുപ്പം(അക്ഷരം*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = ശൂന്യ;
                        തുടരുക;
                }

                _cleanup_strv_free_ അക്ഷരം **u = ശൂന്യ, **b = ശൂന്യ;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : ശൂന്യ,
                                ret_bad_variables ? &b : ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                n[++k] = ശൂന്യ;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        എങ്കിൽ (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getenv_bool(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *e;

        e = getenv(p);
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        തിരികെ parse_boolean(e);
}

പൂർണ്ണസംഖ്യ secure_getenv_bool(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *e;

        e = secure_getenv(p);
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        തിരികെ parse_boolean(e);
}

പൂർണ്ണസംഖ്യ secure_getenv_uint64(സ്ഥിരം അക്ഷരം *p, uint64_t *ret) {
        സ്ഥിരം അക്ഷരം *e;

        പ്രതിജ്ഞാ(p);

        e = secure_getenv(p);
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        തിരികെ safe_atou64(e, ret);
}

പൂർണ്ണസംഖ്യ set_unset_env(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *മൂല്യം, bool overwrite) {
        പ്രതിജ്ഞാ(name);

        എങ്കിൽ (മൂല്യം)
                തിരികെ RET_NERRNO(setenv(name, മൂല്യം, overwrite));

        തിരികെ RET_NERRNO(unsetenv(name));
}

പൂർണ്ണസംഖ്യ putenv_dup(സ്ഥിരം അക്ഷരം *assignment, bool override) {
        സ്ഥിരം അക്ഷരം *e, *n;

        e = strchr(assignment, '=');
        എങ്കിൽ (!e)
                തിരികെ -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        തിരികെ RET_NERRNO(setenv(n, e + 1, override));
}

പൂർണ്ണസംഖ്യ setenv_systemd_exec_pid(bool update_only) {
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        എങ്കിൽ (!e && update_only)
                തിരികെ 0;

        എങ്കിൽ (streq_ptr(e, "*"))
                തിരികെ 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ setenv_systemd_log_level(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *val = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

പൂർണ്ണസംഖ്യ getenv_path_list(സ്ഥിരം അക്ഷരം *name, അക്ഷരം ***ret_paths) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(ret_paths);

        e = secure_getenv(name);
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                എങ്കിൽ (!path_is_absolute(*p))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                എങ്കിൽ (!path_is_normalized(*p))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                എങ്കിൽ (path_equal(*p, "/"))
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        എങ്കിൽ (strv_isempty(l))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ getenv_steal_erase(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret) {
        _cleanup_(erase_and_freep) അക്ഷരം *a = ശൂന്യ;
        അക്ഷരം *e;

        പ്രതിജ്ഞാ(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        എങ്കിൽ (!e) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ 0;
        }

        എങ്കിൽ (ret) {
                a = strdup(e);
                എങ്കിൽ (!a)
                        തിരികെ -ENOMEM;
        }

        string_erase(e);

        എങ്കിൽ (unsetenv(name) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(a);

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ setenvf(സ്ഥിരം അക്ഷരം *name, bool overwrite, സ്ഥിരം അക്ഷരം *valuef, ...) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);

        എങ്കിൽ (!valuef)
                തിരികെ RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&മൂല്യം, valuef, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        എങ്കിൽ (streq_ptr(getenv(name), മൂല്യം))
                തിരികെ 0;

        തിരികെ RET_NERRNO(setenv(name, മൂല്യം, overwrite));
}
