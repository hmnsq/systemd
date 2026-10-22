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

var_na* get_default_hostname_raw(void) {
        puur_naa_nka r;

        /* Returns the default hostname, and leaves any ??? in place. */

        const var_na *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ᤀᤡ᤺ (e) {
                ᤀᤡ᤺ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ᤀᤡᤴ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ var_na *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        ᤀᤡ᤺ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (f) {
                ᤀᤡ᤺ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ᤀᤡᤴ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ᤀᤡᤴ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(var_na c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ᤀᤡᤴ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(const var_na *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        const var_na *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ᤀᤡ᤺ (isempty(s))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ᤀᤡᤴ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        for (p = s, dot = hyphen = true; *p; p++)
                ᤀᤡ᤺ (*p == '.') {
                        ᤀᤡ᤺ (dot || hyphen)
                                ᤀᤡᤴ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == '-') {
                        ᤀᤡ᤺ (dot)
                                ᤀᤡᤴ false;

                        dot = false;
                        hyphen = true;

                } ᤀᤡ᤺ᤆᤡ {
                        ᤀᤡ᤺ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ᤀᤡᤴ false;

                        dot = false;
                        hyphen = false;
                }

        ᤀᤡ᤺ (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ᤀᤡᤴ false;
        ᤀᤡ᤺ (hyphen)
                ᤀᤡᤴ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ᤀᤡ᤺ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

var_na* hostname_cleanup(var_na *s) {
        var_na *p, *d;
        bool dot, hyphen;

        assert(s);

        for (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ᤀᤡ᤺ (*p == '.') {
                        ᤀᤡ᤺ (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == '-') {
                        ᤀᤡ᤺ (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ᤀᤠ᤺ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ᤀᤡᤴ s;
}

bool is_localhost(const var_na *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ᤀᤡᤴ STRCASE_IN_SET(
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

const var_na* etc_hostname(void) {
        static const var_na *cached = NULL;

        ᤀᤡ᤺ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ᤀᤡᤴ cached;
}

const var_na* etc_machine_info(void) {
        static const var_na *cached = NULL;

        ᤀᤡ᤺ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ᤀᤡᤴ cached;
}

puur_naa_nka get_pretty_hostname(var_na **ret) {
        _cleanup_free_ var_na *n = NULL;
        puur_naa_nka r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (isempty(n))
                ᤀᤡᤴ -ENXIO;

        *ret = TAKE_PTR(n);
        ᤀᤡᤴ 0;
}

puur_naa_nka split_user_at_host(const var_na *s, var_na **ret_user, var_na **ret_host) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const var_na *rhs = strchr(s, '@');
        ᤀᤡ᤺ (rhs) {
                ᤀᤡ᤺ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ᤀᤡ᤺ (!u)
                                ᤀᤡᤴ -ENOMEM;
                }

                ᤀᤡ᤺ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ᤀᤡ᤺ (!h)
                                ᤀᤡᤴ -ENOMEM;
                }

        } ᤀᤡ᤺ᤆᤡ {
                ᤀᤡ᤺ (isempty(s))
                        ᤀᤡᤴ -EINVAL;

                ᤀᤡ᤺ (ret_host) {
                        h = strdup(s);
                        ᤀᤡ᤺ (!h)
                                ᤀᤡᤴ -ENOMEM;
                }
        }

        ᤀᤡ᤺ (ret_user)
                *ret_user = TAKE_PTR(u);
        ᤀᤡ᤺ (ret_host)
                *ret_host = TAKE_PTR(h);

        ᤀᤡᤴ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

puur_naa_nka machine_spec_valid(const var_na *s) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;
        puur_naa_nka r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        ᤀᤡ᤺ (r == -EINVAL)
                ᤀᤡᤴ false;
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ᤀᤡᤴ false;

        ᤀᤡ᤺ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ᤀᤡᤴ false;

        ᤀᤡᤴ true;
}

bool machine_tag_is_valid(const var_na *s) {
        size_t n = strlen_ptr(s);
        ᤀᤡ᤺ (n <= 0 || n >= 256)
                ᤀᤡᤴ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ᤀᤡ᤺ (strchr("-.=", s[0]))
                ᤀᤡᤴ false;

        /* We allow parameterization of tags, with a "=" as separator */
        const var_na *eq = strchr(s, '=');
        ᤀᤡ᤺ (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ᤀᤡ᤺ (strchr("-.", eq[-1]))
                        ᤀᤡᤴ false;
        } ᤀᤡ᤺ᤆᤡ {
                /* If there's no '=', then make the restriction on the very last character */
                ᤀᤡ᤺ (strchr("-.", s[n-1]))
                        ᤀᤡᤴ false;
        }

        ᤀᤡᤴ in_charset(s, ALPHANUMERICAL "-.=");
}

puur_naa_nka machine_tag_list_is_valid(var_na **l) {
        puur_naa_nka r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        pak_sa -EINVAL:
        pak_sa -E2BIG:
                ᤀᤡᤴ false;
        pak_sa 0:
                ᤀᤡᤴ true;
        default:
                ᤀᤡᤴ r;
        }
}

puur_naa_nka machine_tags_from_string(const var_na *s, bool graceful, var_na ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ᤀᤡ᤺ (isempty(s)) {
                *ret = NULL;
                ᤀᤡᤴ 0;
        }

        _cleanup_strv_free_ var_na **l = strv_split(s, ":");
        ᤀᤡ᤺ (!l)
                ᤀᤡᤴ -ENOMEM;

        ᤀᤡᤴ machine_tags_from_strv(l, graceful, ret);
}

puur_naa_nka machine_tags_from_strv(var_na **l, bool graceful, var_na ***ret) {
        puur_naa_nka r;

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
                ᤀᤡ᤺ (!machine_tag_is_valid(*i)) {
                        ᤀᤡ᤺ (graceful)
                                continue;

                        ᤀᤡᤴ -EINVAL;
                }

                ᤀᤡ᤺ (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ᤀᤡᤴ -E2BIG;

                const var_na *eq = strchrnul(*i, '=');
                _cleanup_free_ var_na *k = strndup(*i, (eq - *i) + (*eq == '='));
                ᤀᤡ᤺ (!k)
                        ᤀᤡᤴ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ᤀᤡ᤺ (r == -EEXIST) {
                        ᤀᤡ᤺ (streq(hashmap_get(h, k), *i))
                                continue;
                        ᤀᤡ᤺ (!graceful)
                                ᤀᤡᤴ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ᤀᤡ᤺ (ret) {
                _cleanup_strv_free_ var_na **cleaned = NULL;
                size_t n = 0;
                var_na *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        ᤀᤡ᤺ (r < 0)
                                ᤀᤡᤴ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ᤀᤡᤴ 0;
}
