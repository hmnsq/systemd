/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "syslog-util.h"
#அந்தர்பாவ "utf8.h"

/* We follow bash for the character set. Different shells have different rules. */
#define VALID_BASH_ENV_NAME_CHARS               \
        ALPHANUMERICAL                          \
        "_"

size_t sc_arg_max(வெறுமை) {
        long l = sysconf(_SC_ARG_MAX);
        ப்ரதிஜ்ஞா(l > 0);
        திரும்பு (size_t) l;
}

நிலையான bool env_name_is_valid_n(மாறிலி எழுத்து *e, size_t n) {

        எனில் (n == SIZE_MAX)
                n = strlen_ptr(e);

        எனில் (n == 0)
                திரும்பு false;

        ப்ரதிஜ்ஞா(e);

        எனில் (ascii_isdigit(e[0]))
                திரும்பு false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the equal sign and trailing NUL this hence leaves ARG_MAX-2 as
         * longest possible variable name. */
        எனில் (_unlikely_(n > sc_arg_max() - 2))
                திரும்பு false;

        க்கு (மாறிலி எழுத்து *p = e; p < e + n; p++)
                எனில் (!strchr(VALID_BASH_ENV_NAME_CHARS, *p))
                        திரும்பு false;

        திரும்பு true;
}

bool env_name_is_valid(மாறிலி எழுத்து *e) {
        திரும்பு env_name_is_valid_n(e, SIZE_MAX);
}

bool env_மதிப்பு_is_valid(மாறிலி எழுத்து *e) {
        எனில் (!e)
                திரும்பு false;

        எனில் (!utf8_is_valid(e))
                திரும்பு false;

        /* Note that variable *values* may contain control characters, in particular NL, TAB, BS, DEL, ESC…
         * When printing those variables with show-environment, we'll escape them. Make sure to print
         * environment variables carefully! */

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, an individual assignment
         * hence cannot be either. Discounting the shortest possible variable name of length 1, the equal
         * sign and trailing NUL this hence leaves ARG_MAX-3 as longest possible variable value. */
        எனில் (_unlikely_(strlen(e) > sc_arg_max() - 3))
                திரும்பு false;

        திரும்பு true;
}

bool env_assignment_is_valid(மாறிலி எழுத்து *e) {
        மாறிலி எழுத்து *eq;

        ப்ரதிஜ்ஞா(e);

        eq = strchr(e, '=');
        எனில் (!eq)
                திரும்பு false;

        எனில் (!env_name_is_valid_n(e, eq - e))
                திரும்பு false;

        எனில் (!env_மதிப்பு_is_valid(eq + 1))
                திரும்பு false;

        /* POSIX says the overall size of the environment block cannot be > ARG_MAX, hence the individual
         * variable assignments cannot be either, but let's leave room for one trailing NUL byte. */
        எனில் (_unlikely_(strlen(e) > sc_arg_max() - 1))
                திரும்பு false;

        திரும்பு true;
}

bool strv_env_is_valid(எழுத்து * மாறிலி *e) {
        STRV_FOREACH(p, e) {
                size_t k;

                எனில் (!env_assignment_is_valid(*p))
                        திரும்பு false;

                /* Check if there are duplicate assignments */
                k = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        எனில் (strneq(*p, *q, k) && (*q)[k] == '=')
                                திரும்பு false;
        }

        திரும்பு true;
}

bool strv_env_name_is_valid(எழுத்து * மாறிலி *l) {
        STRV_FOREACH(p, l) {
                எனில் (!env_name_is_valid(*p))
                        திரும்பு false;

                எனில் (strv_contains(p + 1, *p))
                        திரும்பு false;
        }

        திரும்பு true;
}

bool strv_env_name_or_assignment_is_valid(எழுத்து * மாறிலி *l) {
        STRV_FOREACH(p, l) {
                எனில் (!env_assignment_is_valid(*p) && !env_name_is_valid(*p))
                        திரும்பு false;

                எனில் (strv_contains(p + 1, *p))
                        திரும்பு false;
        }

        திரும்பு true;
}

நிலையான முழு env_append(எழுத்து **e, எழுத்து ***k, எழுத்து **a) {
        ப்ரதிஜ்ஞா(e);
        ப்ரதிஜ்ஞா(k);
        ப்ரதிஜ்ஞா(*k >= e);

        எனில் (!a)
                திரும்பு 0;

        /* Expects the following arguments: 'e' shall point to the beginning of an strv we are going to append to, 'k'
         * to a pointer pointing to the NULL entry at the end of the same array. 'a' shall point to another strv.
         *
         * This call adds every entry of 'a' to 'e', either overriding an existing matching entry, or appending to it.
         *
         * This call assumes 'e' has enough pre-allocated space to grow by all of 'a''s items. */

        க்கு (; *a; a++) {
                எழுத்து **j, *c;
                size_t n;

                n = strcspn(*a, "=");
                எனில் ((*a)[n] == '=')
                        n++;

                க்கு (j = e; j < *k; j++)
                        எனில் (strneq(*j, *a, n))
                                நிறுத்து;

                c = strdup(*a);
                எனில் (!c)
                        திரும்பு -ENOMEM;

                எனில் (j >= *k) { /* Append to the end? */
                        (*k)[0] = c;
                        (*k)[1] = வெற்று;
                        (*k)++;
                } இல்லையெனில்
                        free_and_replace(*j, c); /* Override existing item */
        }

        திரும்பு 0;
}

எழுத்து** _strv_env_merge(எழுத்து **first, ...) {
        _cleanup_strv_free_ எழுத்து **merged = வெற்று;
        எழுத்து **k;
        va_list ap;

        /* Merges an arbitrary number of environment sets */

        size_t n = strv_length(first);

        va_start(ap, first);
        க்கு (;;) {
                எழுத்து **l;

                l = va_arg(ap, எழுத்து**);
                எனில் (l == POINTER_MAX)
                        நிறுத்து;

                n += strv_length(l);
        }
        va_end(ap);

        k = merged = new(எழுத்து*, n + 1);
        எனில் (!merged)
                திரும்பு வெற்று;
        merged[0] = வெற்று;

        எனில் (env_append(merged, &k, first) < 0)
                திரும்பு வெற்று;

        va_start(ap, first);
        க்கு (;;) {
                எழுத்து **l;

                l = va_arg(ap, எழுத்து**);
                எனில் (l == POINTER_MAX)
                        நிறுத்து;

                எனில் (env_append(merged, &k, l) < 0) {
                        va_end(ap);
                        திரும்பு வெற்று;
                }
        }
        va_end(ap);

        திரும்பு TAKE_PTR(merged);
}

நிலையான bool env_match(மாறிலி எழுத்து *t, மாறிலி எழுத்து *pattern) {
        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(pattern);

        /* pattern a matches string a
         *         a matches a=
         *         a matches a=b
         *         a= matches a=
         *         a=b matches a=b
         *         a= does not match a
         *         a=b does not match a=
         *         a=b does not match a
         *         a=b does not match a=c */

        எனில் (streq(t, pattern))
                திரும்பு true;

        எனில் (!strchr(pattern, '=')) {
                t = startswith(t, pattern);

                திரும்பு t && *t == '=';
        }

        திரும்பு false;
}

நிலையான bool env_entry_has_name(மாறிலி எழுத்து *entry, மாறிலி எழுத்து *name) {
        மாறிலி எழுத்து *t;

        ப்ரதிஜ்ஞா(entry);
        ப்ரதிஜ்ஞா(name);

        t = startswith(entry, name);
        எனில் (!t)
                திரும்பு false;

        திரும்பு *t == '=';
}

எழுத்து** strv_env_delete(எழுத்து **x, size_t n_lists, ...) {
        size_t n, i = 0;
        _cleanup_strv_free_ எழுத்து **t = வெற்று;
        va_list ap;

        /* Deletes every entry from x that is mentioned in the other
         * string lists */

        n = strv_length(x);

        t = new(எழுத்து*, n+1);
        எனில் (!t)
                திரும்பு வெற்று;

        STRV_FOREACH(k, x) {
                va_start(ap, n_lists);
                க்கு (size_t v = 0; v < n_lists; v++) {
                        எழுத்து **l;

                        l = va_arg(ap, எழுத்து**);
                        STRV_FOREACH(j, l)
                                எனில் (env_match(*k, *j))
                                        செல் skip;
                }
                va_end(ap);

                t[i] = strdup(*k);
                எனில் (!t[i])
                        திரும்பு வெற்று;

                i++;
                தொடர்;

        skip:
                va_end(ap);
        }

        t[i] = வெற்று;

        ப்ரதிஜ்ஞா(i <= n);

        திரும்பு TAKE_PTR(t);
}

எழுத்து** strv_env_unset(எழுத்து **l, மாறிலி எழுத்து *p) {
        ப்ரதிஜ்ஞா(p);

        எனில் (!l)
                திரும்பு வெற்று;

        /* Drops every occurrence of the env var setting p in the
         * string list. Edits in-place. */

        எழுத்து **f, **t;
        க்கு (f = t = l; *f; f++) {
                எனில் (env_match(*f, p)) {
                        விடுவி(*f);
                        தொடர்;
                }

                *(t++) = *f;
        }

        *t = வெற்று;
        திரும்பு l;
}

எழுத்து** strv_env_unset_many_internal(எழுத்து **l, ...) {
        எனில் (!l)
                திரும்பு வெற்று;

        /* Like strv_env_unset() but applies many at once. Edits in-place. */

        எழுத்து **f, **t;
        க்கு (f = t = l; *f; f++) {
                bool found = false;
                மாறிலி எழுத்து *p;
                va_list ap;

                va_start(ap, l);

                போது ((p = va_arg(ap, மாறிலி எழுத்து*)))
                        எனில் (env_match(*f, p)) {
                                found = true;
                                நிறுத்து;
                        }

                va_end(ap);

                எனில் (found) {
                        விடுவி(*f);
                        தொடர்;
                }

                *(t++) = *f;
        }

        *t = வெற்று;
        திரும்பு l;
}

முழு strv_env_replace_consume(எழுத்து ***l, எழுத்து *p) {
        மாறிலி எழுத்து *t, *name;
        முழு r;

        ப்ரதிஜ்ஞா(l);
        ப்ரதிஜ்ஞா(p);

        /* Replace first occurrence of the env var or add a new one in the string list. Drop other
         * occurrences. Edits in-place. Does not copy p and CONSUMES p EVEN ON FAILURE.
         *
         * p must be a valid key=value assignment. */

        t = strchr(p, '=');
        எனில் (!t) {
                விடுவி(p);
                திரும்பு -EINVAL;
        }

        name = strndupa_safe(p, t - p);

        STRV_FOREACH(f, *l)
                எனில் (env_entry_has_name(*f, name)) {
                        free_and_replace(*f, p);
                        strv_env_unset(f + 1, *f);
                        திரும்பு 0;
                }

        /* We didn't find a match, we need to append p or create a new strv */
        r = strv_consume(l, p);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

முழு strv_env_replace_strdup(எழுத்து ***l, மாறிலி எழுத்து *assignment) {
        /* Like strv_env_replace_consume(), but copies the argument. */

        ப்ரதிஜ்ஞா(l);
        ப்ரதிஜ்ஞா(assignment);

        எழுத்து *p = strdup(assignment);
        எனில் (!p)
                திரும்பு -ENOMEM;

        திரும்பு strv_env_replace_consume(l, p);
}

முழு strv_env_replace_strdup_passthrough(எழுத்து ***l, மாறிலி எழுத்து *assignment) {
        எழுத்து *p;

        /* Like strv_env_replace_strdup(), but pulls the variable from the environment of
         * the calling program, if a variable name without value is specified. */

        ப்ரதிஜ்ஞா(l);
        ப்ரதிஜ்ஞா(assignment);

        எனில் (strchr(assignment, '=')) {
                எனில் (!env_assignment_is_valid(assignment))
                        திரும்பு -EINVAL;

                p = strdup(assignment);
        } இல்லையெனில் {
                எனில் (!env_name_is_valid(assignment))
                        திரும்பு -EINVAL;

                /* If we can't find the variable in our environment, we will use
                 * the empty string. This way "passthrough" is equivalent to passing
                 * --setenv=FOO=$FOO in the shell. */
                p = strjoin(assignment, "=", secure_getenv(assignment));
        }
        எனில் (!p)
                திரும்பு -ENOMEM;

        திரும்பு strv_env_replace_consume(l, p);
}

முழு strv_env_assign(எழுத்து ***l, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        ப்ரதிஜ்ஞா(l);

        எனில் (!env_name_is_valid(சாவி))
                திரும்பு -EINVAL;

        /* NULL removes assignment, "" creates an empty assignment. */

        எனில் (!மதிப்பு) {
                strv_env_unset(*l, சாவி);
                திரும்பு 0;
        }

        எழுத்து *p = strjoin(சாவி, "=", மதிப்பு);
        எனில் (!p)
                திரும்பு -ENOMEM;

        திரும்பு strv_env_replace_consume(l, p);
}

முழு strv_env_assignf(எழுத்து ***l, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *valuef, ...) {
        முழு r;

        ப்ரதிஜ்ஞா(l);
        ப்ரதிஜ்ஞா(சாவி);

        எனில் (!env_name_is_valid(சாவி))
                திரும்பு -EINVAL;

        எனில் (!valuef) {
                strv_env_unset(*l, சாவி);
                திரும்பு 0;
        }

        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        va_list ap;
        va_start(ap, valuef);
        r = vasprintf(&மதிப்பு, valuef, ap);
        va_end(ap);
        எனில் (r < 0)
                திரும்பு -ENOMEM;

        எழுத்து *p = strjoin(சாவி, "=", மதிப்பு);
        எனில் (!p)
                திரும்பு -ENOMEM;

        திரும்பு strv_env_replace_consume(l, p);
}

முழு _strv_env_assign_many(எழுத்து ***l, ...) {
        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(l);

        va_start(ap, l);
        க்கு (;;) {
                மாறிலி எழுத்து *சாவி, *மதிப்பு;

                சாவி = va_arg(ap, மாறிலி எழுத்து *);
                எனில் (!சாவி)
                        நிறுத்து;

                எனில் (!env_name_is_valid(சாவி)) {
                        va_end(ap);
                        திரும்பு -EINVAL;
                }

                மதிப்பு = va_arg(ap, மாறிலி எழுத்து *);
                எனில் (!மதிப்பு) {
                        strv_env_unset(*l, சாவி);
                        தொடர்;
                }

                எழுத்து *p = strjoin(சாவி, "=", மதிப்பு);
                எனில் (!p) {
                        va_end(ap);
                        திரும்பு -ENOMEM;
                }

                r = strv_env_replace_consume(l, p);
                எனில் (r < 0) {
                        va_end(ap);
                        திரும்பு r;
                }
        }
        va_end(ap);

        திரும்பு 0;
}

எழுத்து* strv_env_get_n(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *name, size_t k, ReplaceEnvFlags flags) {
        ப்ரதிஜ்ஞா(name);

        எனில் (k == SIZE_MAX)
                k = strlen(name);
        எனில் (k <= 0)
                திரும்பு வெற்று;

        STRV_FOREACH_BACKWARDS(i, l)
                எனில் (strneq(*i, name, k) && (*i)[k] == '=')
                        திரும்பு (எழுத்து*) *i + k + 1;

        எனில் (flags & REPLACE_ENV_USE_ENVIRONMENT) {
                மாறிலி எழுத்து *t;

                /* Safety check that the name is not overly long, before we do a stack allocation */
                எனில் (k > sc_arg_max() - 2)
                        திரும்பு வெற்று;

                t = strndupa_safe(name, k);
                திரும்பு secure_getenv(t);
        };

        திரும்பு வெற்று;
}

எழுத்து* strv_env_pairs_get(எழுத்து **l, மாறிலி எழுத்து *name) {
        எழுத்து *result = வெற்று;

        ப்ரதிஜ்ஞா(name);

        STRV_FOREACH_PAIR(சாவி, மதிப்பு, l)
                எனில் (streq(*சாவி, name))
                        result = *மதிப்பு;

        திரும்பு result;
}

முழு strv_env_get_merged(எழுத்து **l, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **v = வெற்று;
        size_t n = 0;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* This converts a strv with pairs of environment variable name + value into a strv of name and
         * value concatenated with a "=" separator. E.g.
         * input  : { "NAME", "value", "FOO", "var" }
         * output : { "NAME=value", "FOO=var" } */

        STRV_FOREACH_PAIR(சாவி, மதிப்பு, l) {
                எழுத்து *s;

                s = strjoin(*சாவி, "=", *மதிப்பு);
                எனில் (!s)
                        திரும்பு -ENOMEM;

                r = strv_consume_with_size(&v, &n, s);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(v);
        திரும்பு 0;
}

எழுத்து** strv_env_clean_with_callback(எழுத்து **l, வெறுமை (*invalid_callback)(மாறிலி எழுத்து *p, வெறுமை *userdata), வெறுமை *userdata) {
        முழு k = 0;

        STRV_FOREACH(p, l) {
                size_t n;
                bool duplicate = false;

                எனில் (!env_assignment_is_valid(*p)) {
                        எனில் (invalid_callback)
                                invalid_callback(*p, userdata);
                        விடுவி(*p);
                        தொடர்;
                }

                n = strcspn(*p, "=");
                STRV_FOREACH(q, p + 1)
                        எனில் (strneq(*p, *q, n) && (*q)[n] == '=') {
                                duplicate = true;
                                நிறுத்து;
                        }

                எனில் (duplicate) {
                        விடுவி(*p);
                        தொடர்;
                }

                l[k++] = *p;
        }

        எனில் (l)
                l[k] = வெற்று;

        திரும்பு l;
}

நிலையான முழு strv_extend_with_length(எழுத்து ***l, மாறிலி எழுத்து *s, size_t n) {
        எழுத்து *c;

        c = strndup(s, n);
        எனில் (!c)
                திரும்பு -ENOMEM;

        திரும்பு strv_consume(l, c);
}

நிலையான முழு strv_env_get_n_validated(
                எழுத்து **env,
                மாறிலி எழுத்து *name,
                size_t l,
                ReplaceEnvFlags flags,
                எழுத்து **ret,              /* points into the env block! do not free! */
                எழுத்து ***unset_variables, /* updated in place */
                எழுத்து ***bad_variables) { /* ditto */

        எழுத்து *e;
        முழு r;

        ப்ரதிஜ்ஞா(l == 0 || name);
        ப்ரதிஜ்ஞா(ret);

        எனில் (env_name_is_valid_n(name, l)) {
                e = strv_env_get_n(env, name, l, flags);
                எனில் (!e && unset_variables) {
                        r = strv_extend_with_length(unset_variables, name, l);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        } இல்லையெனில் {
                e = வெற்று; /* Resolve invalid variable names the same way as unset ones */

                எனில் (bad_variables) {
                        r = strv_extend_with_length(bad_variables, name, l);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        }

        *ret = e;
        திரும்பு !!e;
}

முழு replace_env_full(
                மாறிலி எழுத்து *format,
                size_t length,
                எழுத்து **env,
                ReplaceEnvFlags flags,
                எழுத்து **ret,
                எழுத்து ***ret_unset_variables,
                எழுத்து ***ret_bad_variables) {

        எண்ணிக்கை {
                WORD,
                CURLY,
                VARIABLE,
                VARIABLE_RAW,
                TEST,
                DEFAULT_மதிப்பு,
                ALTERNATE_மதிப்பு,
        } state = WORD;

        _cleanup_strv_free_ எழுத்து **unset_variables = வெற்று, **bad_variables = வெற்று;
        மாறிலி எழுத்து *e, *word = format, *test_மதிப்பு = வெற்று; /* test_value is initialized to appease gcc */
        _cleanup_free_ எழுத்து *s = வெற்று;
        எழுத்து ***pu, ***pb;
        size_t i, len = 0; /* len is initialized to appease gcc */
        முழு nest = 0, r;

        ப்ரதிஜ்ஞா(format);

        எனில் (length == SIZE_MAX)
                length = strlen(format);

        pu = ret_unset_variables ? &unset_variables : வெற்று;
        pb = ret_bad_variables ? &bad_variables : வெற்று;

        க்கு (e = format, i = 0; *e && i < length; e++, i++)
                தேர்வு (state) {

                நிலை WORD:
                        எனில் (*e == '$')
                                state = CURLY;
                        நிறுத்து;

                நிலை CURLY:
                        எனில் (*e == '{') {
                                எனில் (!strextendn(&s, word, e-word-1))
                                        திரும்பு -ENOMEM;

                                word = e-1;
                                state = VARIABLE;
                                nest++;

                        } இல்லையெனில் எனில் (*e == '$') {
                                எனில் (!strextendn(&s, word, e-word))
                                        திரும்பு -ENOMEM;

                                word = e+1;
                                state = WORD;

                        } இல்லையெனில் எனில் (FLAGS_SET(flags, REPLACE_ENV_ALLOW_BRACELESS) && strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                எனில் (!strextendn(&s, word, e-word-1))
                                        திரும்பு -ENOMEM;

                                word = e-1;
                                state = VARIABLE_RAW;

                        } இல்லையெனில்
                                state = WORD;
                        நிறுத்து;

                நிலை VARIABLE:
                        எனில் (*e == '}') {
                                எழுத்து *t;

                                r = strv_env_get_n_validated(env, word+2, e-word-2, flags, &t, pu, pb);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (!strextend(&s, t))
                                        திரும்பு -ENOMEM;

                                word = e+1;
                                state = WORD;
                                nest--;
                        } இல்லையெனில் எனில் (*e == ':') {
                                எனில் (flags & REPLACE_ENV_ALLOW_EXTENDED) {
                                        len = e - word - 2;
                                        state = TEST;
                                } இல்லையெனில்
                                        /* Treat this as unsupported syntax, i.e. do no replacement */
                                        state = WORD;
                        }
                        நிறுத்து;

                நிலை TEST:
                        எனில் (*e == '-')
                                state = DEFAULT_மதிப்பு;
                        இல்லையெனில் எனில் (*e == '+')
                                state = ALTERNATE_மதிப்பு;
                        இல்லையெனில் {
                                state = WORD;
                                நிறுத்து;
                        }

                        test_மதிப்பு = e+1;
                        நிறுத்து;

                நிலை DEFAULT_மதிப்பு: /* fall through */
                நிலை ALTERNATE_மதிப்பு:
                        ப்ரதிஜ்ஞா(flags & REPLACE_ENV_ALLOW_EXTENDED);

                        எனில் (*e == '{') {
                                nest++;
                                நிறுத்து;
                        }

                        எனில் (*e != '}')
                                நிறுத்து;

                        nest--;
                        எனில் (nest == 0) {
                                _cleanup_strv_free_ எழுத்து **u = வெற்று, **b = வெற்று;
                                _cleanup_free_ எழுத்து *v = வெற்று;
                                எழுத்து *t = வெற்று;

                                r = strv_env_get_n_validated(env, word+2, len, flags, &t, pu, pb);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (t && state == ALTERNATE_மதிப்பு) {
                                        r = replace_env_full(test_மதிப்பு, e-test_மதிப்பு, env, flags, &v, pu ? &u : வெற்று, pb ? &b : வெற்று);
                                        எனில் (r < 0)
                                                திரும்பு r;

                                        t = v;
                                } இல்லையெனில் எனில் (!t && state == DEFAULT_மதிப்பு) {
                                        r = replace_env_full(test_மதிப்பு, e-test_மதிப்பு, env, flags, &v, pu ? &u : வெற்று, pb ? &b : வெற்று);
                                        எனில் (r < 0)
                                                திரும்பு r;

                                        t = v;
                                }

                                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                                எனில் (r < 0)
                                        திரும்பு r;
                                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (!strextend(&s, t))
                                        திரும்பு -ENOMEM;

                                word = e+1;
                                state = WORD;
                        }
                        நிறுத்து;

                நிலை VARIABLE_RAW:
                        ப்ரதிஜ்ஞா(flags & REPLACE_ENV_ALLOW_BRACELESS);

                        எனில் (!strchr(VALID_BASH_ENV_NAME_CHARS, *e)) {
                                எழுத்து *t = வெற்று;

                                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (!strextend(&s, t))
                                        திரும்பு -ENOMEM;

                                word = e--;
                                i--;
                                state = WORD;
                        }
                        நிறுத்து;
                }

        எனில் (state == VARIABLE_RAW) {
                எழுத்து *t;

                ப்ரதிஜ்ஞா(flags & REPLACE_ENV_ALLOW_BRACELESS);

                r = strv_env_get_n_validated(env, word+1, e-word-1, flags, &t, &unset_variables, &bad_variables);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (!strextend(&s, t))
                        திரும்பு -ENOMEM;

        } இல்லையெனில் எனில் (!strextendn(&s, word, e-word))
                திரும்பு -ENOMEM;

        எனில் (ret_unset_variables)
                *ret_unset_variables = TAKE_PTR(unset_variables);
        எனில் (ret_bad_variables)
                *ret_bad_variables = TAKE_PTR(bad_variables);

        எனில் (ret)
                *ret = TAKE_PTR(s);

        திரும்பு 0;
}

முழு replace_env_argv(
                எழுத்து **argv,
                எழுத்து **env,
                எழுத்து ***ret,
                எழுத்து ***ret_unset_variables,
                எழுத்து ***ret_bad_variables) {

        _cleanup_strv_free_ எழுத்து **n = வெற்று, **unset_variables = வெற்று, **bad_variables = வெற்று;
        size_t k = 0, l;
        முழு r;

        ப்ரதிஜ்ஞா(!strv_isempty(argv));
        ப்ரதிஜ்ஞா(ret);

        l = strv_length(argv);

        n = new(எழுத்து*, l+1);
        எனில் (!n)
                திரும்பு -ENOMEM;

        STRV_FOREACH(i, argv) {
                மாறிலி எழுத்து *word = *i;

                /* If $FOO appears as single word, replace it by the split up variable */
                எனில் (word[0] == '$' && !IN_SET(word[1], '{', '$')) {
                        _cleanup_strv_free_ எழுத்து **m = வெற்று;
                        மாறிலி எழுத்து *name = word + 1;
                        எழுத்து *e, **w;
                        size_t q;

                        எனில் (env_name_is_valid(name)) {
                                e = strv_env_get(env, name);
                                எனில் (e)
                                        r = strv_split_full(&m, e, WHITESPACE, EXTRACT_RELAX|EXTRACT_UNQUOTE);
                                இல்லையெனில் எனில் (ret_unset_variables)
                                        r = strv_extend(&unset_variables, name);
                                இல்லையெனில்
                                        r = 0;
                        } இல்லையெனில் எனில் (ret_bad_variables)
                                r = strv_extend(&bad_variables, name);
                        இல்லையெனில்
                                r = 0;
                        எனில் (r < 0)
                                திரும்பு r;

                        q = strv_length(m);
                        l = l + q - 1;

                        w = reallocarray(n, l + 1, அளவு(எழுத்து*));
                        எனில் (!w)
                                திரும்பு -ENOMEM;

                        n = w;
                        எனில் (m) {
                                memcpy(n + k, m, (q + 1) * அளவு(எழுத்து*));
                                m = mfree(m);
                        }

                        k += q;
                        /* reallocarray() does not zero-initialize the grown tail, add NUL termination */
                        n[k] = வெற்று;
                        தொடர்;
                }

                _cleanup_strv_free_ எழுத்து **u = வெற்று, **b = வெற்று;

                /* If ${FOO} appears as part of a word, replace it by the variable as-is */
                r = replace_env_full(
                                word,
                                /* length= */ SIZE_MAX,
                                env,
                                /* flags= */ 0,
                                n + k,
                                ret_unset_variables ? &u : வெற்று,
                                ret_bad_variables ? &b : வெற்று);
                எனில் (r < 0)
                        திரும்பு r;
                n[++k] = வெற்று;

                r = strv_extend_strv_consume(&unset_variables, TAKE_PTR(u), /* filter_duplicates= */ true);
                எனில் (r < 0)
                        திரும்பு r;

                r = strv_extend_strv_consume(&bad_variables, TAKE_PTR(b), /* filter_duplicates= */ true);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_unset_variables) {
                strv_sort_uniq(unset_variables);
                *ret_unset_variables = TAKE_PTR(unset_variables);
        }
        எனில் (ret_bad_variables) {
                strv_sort_uniq(bad_variables);
                *ret_bad_variables = TAKE_PTR(bad_variables);
        }

        *ret = TAKE_PTR(n);
        திரும்பு 0;
}

முழு getenv_bool(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *e;

        e = getenv(p);
        எனில் (!e)
                திரும்பு -ENXIO;

        திரும்பு parse_boolean(e);
}

முழு secure_getenv_bool(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *e;

        e = secure_getenv(p);
        எனில் (!e)
                திரும்பு -ENXIO;

        திரும்பு parse_boolean(e);
}

முழு secure_getenv_uint64(மாறிலி எழுத்து *p, uint64_t *ret) {
        மாறிலி எழுத்து *e;

        ப்ரதிஜ்ஞா(p);

        e = secure_getenv(p);
        எனில் (!e)
                திரும்பு -ENXIO;

        திரும்பு safe_atou64(e, ret);
}

முழு set_unset_env(மாறிலி எழுத்து *name, மாறிலி எழுத்து *மதிப்பு, bool overwrite) {
        ப்ரதிஜ்ஞா(name);

        எனில் (மதிப்பு)
                திரும்பு RET_NERRNO(setenv(name, மதிப்பு, overwrite));

        திரும்பு RET_NERRNO(unsetenv(name));
}

முழு putenv_dup(மாறிலி எழுத்து *assignment, bool override) {
        மாறிலி எழுத்து *e, *n;

        e = strchr(assignment, '=');
        எனில் (!e)
                திரும்பு -EINVAL;

        n = strndupa_safe(assignment, e - assignment);

        /* This is like putenv(), but uses setenv() so that our memory doesn't become part of environ[]. */
        திரும்பு RET_NERRNO(setenv(n, e + 1, override));
}

முழு setenv_systemd_exec_pid(bool update_only) {
        மாறிலி எழுத்து *e;
        முழு r;

        /* Update $SYSTEMD_EXEC_PID=pid except when '*' is set for the variable. */

        e = secure_getenv("SYSTEMD_EXEC_PID");
        எனில் (!e && update_only)
                திரும்பு 0;

        எனில் (streq_ptr(e, "*"))
                திரும்பு 0;

        r = setenvf("SYSTEMD_EXEC_PID", /* overwrite= */ 1, PID_FMT, getpid_cached());
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

முழு setenv_systemd_log_level(வெறுமை) {
        _cleanup_free_ எழுத்து *val = வெற்று;
        முழு r;

        r = log_level_to_string_alloc(log_get_max_level(), &val);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு RET_NERRNO(setenv("SYSTEMD_LOG_LEVEL", val, /* overwrite= */ true));
}

முழு getenv_path_list(மாறிலி எழுத்து *name, எழுத்து ***ret_paths) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        மாறிலி எழுத்து *e;
        முழு r;

        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(ret_paths);

        e = secure_getenv(name);
        எனில் (!e)
                திரும்பு -ENXIO;

        r = strv_split_full(&l, e, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to parse $%s: %m", name);

        STRV_FOREACH(p, l) {
                எனில் (!path_is_absolute(*p))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not absolute, refusing.", *p);

                எனில் (!path_is_normalized(*p))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is not normalized, refusing.", *p);

                எனில் (path_equal(*p, "/"))
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                               "Path '%s' is the root fs, refusing.", *p);
        }

        எனில் (strv_isempty(l))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "No paths specified, refusing.");

        *ret_paths = TAKE_PTR(l);
        திரும்பு 1;
}

முழு getenv_steal_erase(மாறிலி எழுத்து *name, எழுத்து **ret) {
        _cleanup_(erase_and_freep) எழுத்து *a = வெற்று;
        எழுத்து *e;

        ப்ரதிஜ்ஞா(name);

        /* Reads an environment variable, makes a copy of it, erases its memory in the environment block and removes
         * it from there. Usecase: reading passwords from the env block (which is a bad idea, but useful for
         * testing, and given that people are likely going to misuse this, be thorough) */

        e = secure_getenv(name);
        எனில் (!e) {
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு 0;
        }

        எனில் (ret) {
                a = strdup(e);
                எனில் (!a)
                        திரும்பு -ENOMEM;
        }

        string_erase(e);

        எனில் (unsetenv(name) < 0)
                திரும்பு -errno;

        எனில் (ret)
                *ret = TAKE_PTR(a);

        திரும்பு 1;
}

முழு setenvf(மாறிலி எழுத்து *name, bool overwrite, மாறிலி எழுத்து *valuef, ...) {
        _cleanup_free_ எழுத்து *மதிப்பு = வெற்று;
        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(name);

        எனில் (!valuef)
                திரும்பு RET_NERRNO(unsetenv(name));

        va_start(ap, valuef);
        r = vasprintf(&மதிப்பு, valuef, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு -ENOMEM;

        /* Try to suppress writes if the value is already set correctly (simply because memory management of
         * environment variables sucks a bit. */
        எனில் (streq_ptr(getenv(name), மதிப்பு))
                திரும்பு 0;

        திரும்பு RET_NERRNO(setenv(name, மதிப்பு, overwrite));
}
