/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "env-file.h"
#include "hashmap.h"
#include "hostname-util.h"
#include "log.h"
#include "os-util.h"
#include "string-util.h"
#include "strv.h"
#include "user-util.h"

char* get_default_hostname_raw(void) {
        ціле r;

        /* Returns the default hostname, and leaves any ??? in place. */

        const char *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        якщо (e) {
                якщо (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        повернути strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ char *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        якщо (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        інакше якщо (f) {
                якщо (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        повернути TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        повернути strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(char c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        повернути ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(const char *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        const char *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        якщо (isempty(s))
                повернути false;

        якщо (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                повернути FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        для (p = s, dot = hyphen = true; *p; p++)
                якщо (*p == '.') {
                        якщо (dot || hyphen)
                                повернути false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } інакше якщо (*p == '-') {
                        якщо (dot)
                                повернути false;

                        dot = false;
                        hyphen = true;

                } інакше {
                        якщо (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                повернути false;

                        dot = false;
                        hyphen = false;
                }

        якщо (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                повернути false;
        якщо (hyphen)
                повернути false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        якщо (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                повернути false;

        повернути true;
}

char* hostname_cleanup(char *s) {
        char *p, *d;
        bool dot, hyphen;

        assert(s);

        для (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                якщо (*p == '.') {
                        якщо (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } інакше якщо (*p == '-') {
                        якщо (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } інакше якщо (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        поки (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        повернути s;
}

bool is_localhost(const char *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        повернути STRCASE_IN_SET(
                        hostname,
                        "localhost",
                        "localhost.",
                        "localhost.localdomain",
                        "localhost.localdomain.") ||
                endswith_no_case(hostname, ".localhost") ||
                endswith_no_case(hostname, ".localhost.") ||
                endswith_no_case(hostname, ".localhost.localdomain") ||
                endswith_no_case(hostname, ".localhost.localdomain.");
}

const char* etc_hostname(void) {
        static const char *cached = NULL;

        якщо (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        повернути cached;
}

const char* etc_machine_info(void) {
        static const char *cached = NULL;

        якщо (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        повернути cached;
}

ціле get_pretty_hostname(char **ret) {
        _cleanup_free_ char *n = NULL;
        ціле r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        якщо (r < 0)
                повернути r;

        якщо (isempty(n))
                повернути -ENXIO;

        *ret = TAKE_PTR(n);
        повернути 0;
}

ціле split_user_at_host(const char *s, char **ret_user, char **ret_host) {
        _cleanup_free_ char *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const char *rhs = strchr(s, '@');
        якщо (rhs) {
                якщо (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        якщо (!u)
                                повернути -ENOMEM;
                }

                якщо (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        якщо (!h)
                                повернути -ENOMEM;
                }

        } інакше {
                якщо (isempty(s))
                        повернути -EINVAL;

                якщо (ret_host) {
                        h = strdup(s);
                        якщо (!h)
                                повернути -ENOMEM;
                }
        }

        якщо (ret_user)
                *ret_user = TAKE_PTR(u);
        якщо (ret_host)
                *ret_host = TAKE_PTR(h);

        повернути !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

ціле machine_spec_valid(const char *s) {
        _cleanup_free_ char *u = NULL, *h = NULL;
        ціле r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        якщо (r == -EINVAL)
                повернути false;
        якщо (r < 0)
                повернути r;

        якщо (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                повернути false;

        якщо (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                повернути false;

        повернути true;
}

bool machine_tag_is_valid(const char *s) {
        size_t n = strlen_ptr(s);
        якщо (n <= 0 || n >= 256)
                повернути false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        якщо (strchr("-.=", s[0]))
                повернути false;

        /* We allow parameterization of tags, with a "=" as separator */
        const char *eq = strchr(s, '=');
        якщо (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                якщо (strchr("-.", eq[-1]))
                        повернути false;
        } інакше {
                /* If there's no '=', then make the restriction on the very last character */
                якщо (strchr("-.", s[n-1]))
                        повернути false;
        }

        повернути in_charset(s, ALPHANUMERICAL "-.=");
}

ціле machine_tag_list_is_valid(char **l) {
        ціле r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        case -EINVAL:
        case -E2BIG:
                повернути false;
        case 0:
                повернути true;
        default:
                повернути r;
        }
}

ціле machine_tags_from_string(const char *s, bool graceful, char ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        якщо (isempty(s)) {
                *ret = NULL;
                повернути 0;
        }

        _cleanup_strv_free_ char **l = strv_split(s, ":");
        якщо (!l)
                повернути -ENOMEM;

        повернути machine_tags_from_strv(l, graceful, ret);
}

ціле machine_tags_from_strv(char **l, bool graceful, char ***ret) {
        ціле r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = NULL;
        STRV_FOREACH(i, l) {
                якщо (!machine_tag_is_valid(*i)) {
                        якщо (graceful)
                                continue;

                        повернути -EINVAL;
                }

                якщо (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        повернути -E2BIG;

                const char *eq = strchrnul(*i, '=');
                _cleanup_free_ char *k = strndup(*i, (eq - *i) + (*eq == '='));
                якщо (!k)
                        повернути -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                якщо (r == -EEXIST) {
                        якщо (streq(hashmap_get(h, k), *i))
                                continue;
                        якщо (!graceful)
                                повернути -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } інакше якщо (r < 0)
                        повернути r;
                інакше якщо (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        якщо (ret) {
                _cleanup_strv_free_ char **cleaned = NULL;
                size_t n = 0;
                char *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        якщо (r < 0)
                                повернути r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        повернути 0;
}
