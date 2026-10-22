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
        ᰀᰧ (e) {
                ᰀᰧ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ᰀᰧᰌ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ var_na *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        ᰀᰧ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ᰀᰧᰀ ᰀᰧ (f) {
                ᰀᰧ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ᰀᰧᰌ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ᰀᰧᰌ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(var_na c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ᰀᰧᰌ ascii_isalpha(c) ||
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

        ᰀᰧ (isempty(s))
                ᰀᰧᰌ false;

        ᰀᰧ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ᰀᰧᰌ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        for (p = s, dot = hyphen = true; *p; p++)
                ᰀᰧ (*p == '.') {
                        ᰀᰧ (dot || hyphen)
                                ᰀᰧᰌ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ᰀᰧᰀ ᰀᰧ (*p == '-') {
                        ᰀᰧ (dot)
                                ᰀᰧᰌ false;

                        dot = false;
                        hyphen = true;

                } ᰀᰧᰀ {
                        ᰀᰧ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ᰀᰧᰌ false;

                        dot = false;
                        hyphen = false;
                }

        ᰀᰧ (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ᰀᰧᰌ false;
        ᰀᰧ (hyphen)
                ᰀᰧᰌ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ᰀᰧ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

var_na* hostname_cleanup(var_na *s) {
        var_na *p, *d;
        bool dot, hyphen;

        assert(s);

        for (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ᰀᰧ (*p == '.') {
                        ᰀᰧ (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ᰀᰧᰀ ᰀᰧ (*p == '-') {
                        ᰀᰧ (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ᰀᰧᰀ ᰀᰧ (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ᰀᰬ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ᰀᰧᰌ s;
}

bool is_localhost(const var_na *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ᰀᰧᰌ STRCASE_IN_SET(
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

        ᰀᰧ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ᰀᰧᰌ cached;
}

const var_na* etc_machine_info(void) {
        static const var_na *cached = NULL;

        ᰀᰧ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ᰀᰧᰌ cached;
}

puur_naa_nka get_pretty_hostname(var_na **ret) {
        _cleanup_free_ var_na *n = NULL;
        puur_naa_nka r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (isempty(n))
                ᰀᰧᰌ -ENXIO;

        *ret = TAKE_PTR(n);
        ᰀᰧᰌ 0;
}

puur_naa_nka split_user_at_host(const var_na *s, var_na **ret_user, var_na **ret_host) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const var_na *rhs = strchr(s, '@');
        ᰀᰧ (rhs) {
                ᰀᰧ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ᰀᰧ (!u)
                                ᰀᰧᰌ -ENOMEM;
                }

                ᰀᰧ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ᰀᰧ (!h)
                                ᰀᰧᰌ -ENOMEM;
                }

        } ᰀᰧᰀ {
                ᰀᰧ (isempty(s))
                        ᰀᰧᰌ -EINVAL;

                ᰀᰧ (ret_host) {
                        h = strdup(s);
                        ᰀᰧ (!h)
                                ᰀᰧᰌ -ENOMEM;
                }
        }

        ᰀᰧ (ret_user)
                *ret_user = TAKE_PTR(u);
        ᰀᰧ (ret_host)
                *ret_host = TAKE_PTR(h);

        ᰀᰧᰌ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

puur_naa_nka machine_spec_valid(const var_na *s) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;
        puur_naa_nka r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        ᰀᰧ (r == -EINVAL)
                ᰀᰧᰌ false;
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ᰀᰧᰌ false;

        ᰀᰧ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

bool machine_tag_is_valid(const var_na *s) {
        size_t n = strlen_ptr(s);
        ᰀᰧ (n <= 0 || n >= 256)
                ᰀᰧᰌ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ᰀᰧ (strchr("-.=", s[0]))
                ᰀᰧᰌ false;

        /* We allow parameterization of tags, with a "=" as separator */
        const var_na *eq = strchr(s, '=');
        ᰀᰧ (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ᰀᰧ (strchr("-.", eq[-1]))
                        ᰀᰧᰌ false;
        } ᰀᰧᰀ {
                /* If there's no '=', then make the restriction on the very last character */
                ᰀᰧ (strchr("-.", s[n-1]))
                        ᰀᰧᰌ false;
        }

        ᰀᰧᰌ in_charset(s, ALPHANUMERICAL "-.=");
}

puur_naa_nka machine_tag_list_is_valid(var_na **l) {
        puur_naa_nka r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        pak_sa -EINVAL:
        pak_sa -E2BIG:
                ᰀᰧᰌ false;
        pak_sa 0:
                ᰀᰧᰌ true;
        default:
                ᰀᰧᰌ r;
        }
}

puur_naa_nka machine_tags_from_string(const var_na *s, bool graceful, var_na ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ᰀᰧ (isempty(s)) {
                *ret = NULL;
                ᰀᰧᰌ 0;
        }

        _cleanup_strv_free_ var_na **l = strv_split(s, ":");
        ᰀᰧ (!l)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧᰌ machine_tags_from_strv(l, graceful, ret);
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
                ᰀᰧ (!machine_tag_is_valid(*i)) {
                        ᰀᰧ (graceful)
                                continue;

                        ᰀᰧᰌ -EINVAL;
                }

                ᰀᰧ (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ᰀᰧᰌ -E2BIG;

                const var_na *eq = strchrnul(*i, '=');
                _cleanup_free_ var_na *k = strndup(*i, (eq - *i) + (*eq == '='));
                ᰀᰧ (!k)
                        ᰀᰧᰌ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ᰀᰧ (r == -EEXIST) {
                        ᰀᰧ (streq(hashmap_get(h, k), *i))
                                continue;
                        ᰀᰧ (!graceful)
                                ᰀᰧᰌ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ᰀᰧᰀ ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;
                ᰀᰧᰀ ᰀᰧ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ᰀᰧ (ret) {
                _cleanup_strv_free_ var_na **cleaned = NULL;
                size_t n = 0;
                var_na *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        ᰀᰧ (r < 0)
                                ᰀᰧᰌ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ᰀᰧᰌ 0;
}
