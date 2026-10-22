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
        整数 r;

        /* Returns the default hostname, and leaves any ??? in place. */

        const char *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        如果 (e) {
                如果 (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        返回 strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ char *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        如果 (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        否则 如果 (f) {
                如果 (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        返回 TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        返回 strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(char c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        返回 ascii_isalpha(c) ||
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

        如果 (isempty(s))
                返回 false;

        如果 (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                返回 FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        为 (p = s, dot = hyphen = true; *p; p++)
                如果 (*p == '.') {
                        如果 (dot || hyphen)
                                返回 false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } 否则 如果 (*p == '-') {
                        如果 (dot)
                                返回 false;

                        dot = false;
                        hyphen = true;

                } 否则 {
                        如果 (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                返回 false;

                        dot = false;
                        hyphen = false;
                }

        如果 (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                返回 false;
        如果 (hyphen)
                返回 false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        如果 (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                返回 false;

        返回 true;
}

char* hostname_cleanup(char *s) {
        char *p, *d;
        bool dot, hyphen;

        assert(s);

        为 (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                如果 (*p == '.') {
                        如果 (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } 否则 如果 (*p == '-') {
                        如果 (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } 否则 如果 (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        当 (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        返回 s;
}

bool is_localhost(const char *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        返回 STRCASE_IN_SET(
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

        如果 (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        返回 cached;
}

const char* etc_machine_info(void) {
        static const char *cached = NULL;

        如果 (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        返回 cached;
}

整数 get_pretty_hostname(char **ret) {
        _cleanup_free_ char *n = NULL;
        整数 r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        如果 (r < 0)
                返回 r;

        如果 (isempty(n))
                返回 -ENXIO;

        *ret = TAKE_PTR(n);
        返回 0;
}

整数 split_user_at_host(const char *s, char **ret_user, char **ret_host) {
        _cleanup_free_ char *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const char *rhs = strchr(s, '@');
        如果 (rhs) {
                如果 (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        如果 (!u)
                                返回 -ENOMEM;
                }

                如果 (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        如果 (!h)
                                返回 -ENOMEM;
                }

        } 否则 {
                如果 (isempty(s))
                        返回 -EINVAL;

                如果 (ret_host) {
                        h = strdup(s);
                        如果 (!h)
                                返回 -ENOMEM;
                }
        }

        如果 (ret_user)
                *ret_user = TAKE_PTR(u);
        如果 (ret_host)
                *ret_host = TAKE_PTR(h);

        返回 !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

整数 machine_spec_valid(const char *s) {
        _cleanup_free_ char *u = NULL, *h = NULL;
        整数 r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        如果 (r == -EINVAL)
                返回 false;
        如果 (r < 0)
                返回 r;

        如果 (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                返回 false;

        如果 (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                返回 false;

        返回 true;
}

bool machine_tag_is_valid(const char *s) {
        size_t n = strlen_ptr(s);
        如果 (n <= 0 || n >= 256)
                返回 false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        如果 (strchr("-.=", s[0]))
                返回 false;

        /* We allow parameterization of tags, with a "=" as separator */
        const char *eq = strchr(s, '=');
        如果 (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                如果 (strchr("-.", eq[-1]))
                        返回 false;
        } 否则 {
                /* If there's no '=', then make the restriction on the very last character */
                如果 (strchr("-.", s[n-1]))
                        返回 false;
        }

        返回 in_charset(s, ALPHANUMERICAL "-.=");
}

整数 machine_tag_list_is_valid(char **l) {
        整数 r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        case -EINVAL:
        case -E2BIG:
                返回 false;
        case 0:
                返回 true;
        default:
                返回 r;
        }
}

整数 machine_tags_from_string(const char *s, bool graceful, char ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        如果 (isempty(s)) {
                *ret = NULL;
                返回 0;
        }

        _cleanup_strv_free_ char **l = strv_split(s, ":");
        如果 (!l)
                返回 -ENOMEM;

        返回 machine_tags_from_strv(l, graceful, ret);
}

整数 machine_tags_from_strv(char **l, bool graceful, char ***ret) {
        整数 r;

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
                如果 (!machine_tag_is_valid(*i)) {
                        如果 (graceful)
                                continue;

                        返回 -EINVAL;
                }

                如果 (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        返回 -E2BIG;

                const char *eq = strchrnul(*i, '=');
                _cleanup_free_ char *k = strndup(*i, (eq - *i) + (*eq == '='));
                如果 (!k)
                        返回 -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                如果 (r == -EEXIST) {
                        如果 (streq(hashmap_get(h, k), *i))
                                continue;
                        如果 (!graceful)
                                返回 -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } 否则 如果 (r < 0)
                        返回 r;
                否则 如果 (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        如果 (ret) {
                _cleanup_strv_free_ char **cleaned = NULL;
                size_t n = 0;
                char *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        如果 (r < 0)
                                返回 r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        返回 0;
}
