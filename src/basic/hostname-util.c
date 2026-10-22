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
        số_nguyên r;

        /* Returns the default hostname, and leaves any ??? in place. */

        const char *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        nếu (e) {
                nếu (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        trả_về strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ char *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        nếu (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        không_thì nếu (f) {
                nếu (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        trả_về TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        trả_về strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(char c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        trả_về ascii_isalpha(c) ||
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

        nếu (isempty(s))
                trả_về false;

        nếu (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                trả_về FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        cho (p = s, dot = hyphen = true; *p; p++)
                nếu (*p == '.') {
                        nếu (dot || hyphen)
                                trả_về false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } không_thì nếu (*p == '-') {
                        nếu (dot)
                                trả_về false;

                        dot = false;
                        hyphen = true;

                } không_thì {
                        nếu (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                trả_về false;

                        dot = false;
                        hyphen = false;
                }

        nếu (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                trả_về false;
        nếu (hyphen)
                trả_về false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        nếu (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                trả_về false;

        trả_về true;
}

char* hostname_cleanup(char *s) {
        char *p, *d;
        bool dot, hyphen;

        assert(s);

        cho (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                nếu (*p == '.') {
                        nếu (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } không_thì nếu (*p == '-') {
                        nếu (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } không_thì nếu (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        trong_khi (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        trả_về s;
}

bool is_localhost(const char *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        trả_về STRCASE_IN_SET(
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

        nếu (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        trả_về cached;
}

const char* etc_machine_info(void) {
        static const char *cached = NULL;

        nếu (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        trả_về cached;
}

số_nguyên get_pretty_hostname(char **ret) {
        _cleanup_free_ char *n = NULL;
        số_nguyên r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        nếu (r < 0)
                trả_về r;

        nếu (isempty(n))
                trả_về -ENXIO;

        *ret = TAKE_PTR(n);
        trả_về 0;
}

số_nguyên split_user_at_host(const char *s, char **ret_user, char **ret_host) {
        _cleanup_free_ char *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const char *rhs = strchr(s, '@');
        nếu (rhs) {
                nếu (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        nếu (!u)
                                trả_về -ENOMEM;
                }

                nếu (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        nếu (!h)
                                trả_về -ENOMEM;
                }

        } không_thì {
                nếu (isempty(s))
                        trả_về -EINVAL;

                nếu (ret_host) {
                        h = strdup(s);
                        nếu (!h)
                                trả_về -ENOMEM;
                }
        }

        nếu (ret_user)
                *ret_user = TAKE_PTR(u);
        nếu (ret_host)
                *ret_host = TAKE_PTR(h);

        trả_về !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

số_nguyên machine_spec_valid(const char *s) {
        _cleanup_free_ char *u = NULL, *h = NULL;
        số_nguyên r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        nếu (r == -EINVAL)
                trả_về false;
        nếu (r < 0)
                trả_về r;

        nếu (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                trả_về false;

        nếu (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                trả_về false;

        trả_về true;
}

bool machine_tag_is_valid(const char *s) {
        size_t n = strlen_ptr(s);
        nếu (n <= 0 || n >= 256)
                trả_về false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        nếu (strchr("-.=", s[0]))
                trả_về false;

        /* We allow parameterization of tags, with a "=" as separator */
        const char *eq = strchr(s, '=');
        nếu (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                nếu (strchr("-.", eq[-1]))
                        trả_về false;
        } không_thì {
                /* If there's no '=', then make the restriction on the very last character */
                nếu (strchr("-.", s[n-1]))
                        trả_về false;
        }

        trả_về in_charset(s, ALPHANUMERICAL "-.=");
}

số_nguyên machine_tag_list_is_valid(char **l) {
        số_nguyên r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        case -EINVAL:
        case -E2BIG:
                trả_về false;
        case 0:
                trả_về true;
        default:
                trả_về r;
        }
}

số_nguyên machine_tags_from_string(const char *s, bool graceful, char ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        nếu (isempty(s)) {
                *ret = NULL;
                trả_về 0;
        }

        _cleanup_strv_free_ char **l = strv_split(s, ":");
        nếu (!l)
                trả_về -ENOMEM;

        trả_về machine_tags_from_strv(l, graceful, ret);
}

số_nguyên machine_tags_from_strv(char **l, bool graceful, char ***ret) {
        số_nguyên r;

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
                nếu (!machine_tag_is_valid(*i)) {
                        nếu (graceful)
                                continue;

                        trả_về -EINVAL;
                }

                nếu (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        trả_về -E2BIG;

                const char *eq = strchrnul(*i, '=');
                _cleanup_free_ char *k = strndup(*i, (eq - *i) + (*eq == '='));
                nếu (!k)
                        trả_về -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                nếu (r == -EEXIST) {
                        nếu (streq(hashmap_get(h, k), *i))
                                continue;
                        nếu (!graceful)
                                trả_về -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } không_thì nếu (r < 0)
                        trả_về r;
                không_thì nếu (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        nếu (ret) {
                _cleanup_strv_free_ char **cleaned = NULL;
                size_t n = 0;
                char *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        nếu (r < 0)
                                trả_về r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        trả_về 0;
}
