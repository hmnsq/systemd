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
        පූර්ණ_සංඛ්‍යා r;

        /* Returns the default hostname, and leaves any ??? in place. */

        const var_na *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        නම් (e) {
                නම් (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ආපසු strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ var_na *f = NULL;
        r = parse_os_release(NULL, "DEFAULT_HOSTNAME", &f);
        නම් (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        නැතහොත් නම් (f) {
                නම් (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ආපසු TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ආපසු strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(var_na c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ආපසු ascii_isalpha(c) ||
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

        නම් (isempty(s))
                ආපසු false;

        නම් (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ආපසු FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        සඳහා (p = s, dot = hyphen = true; *p; p++)
                නම් (*p == '.') {
                        නම් (dot || hyphen)
                                ආපසු false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } නැතහොත් නම් (*p == '-') {
                        නම් (dot)
                                ආපසු false;

                        dot = false;
                        hyphen = true;

                } නැතහොත් {
                        නම් (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ආපසු false;

                        dot = false;
                        hyphen = false;
                }

        නම් (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ආපසු false;
        නම් (hyphen)
                ආපසු false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        නම් (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ආපසු false;

        ආපසු true;
}

var_na* hostname_cleanup(var_na *s) {
        var_na *p, *d;
        bool dot, hyphen;

        assert(s);

        සඳහා (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                නම් (*p == '.') {
                        නම් (dot || hyphen)
                                continue;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } නැතහොත් නම් (*p == '-') {
                        නම් (dot)
                                continue;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } නැතහොත් නම් (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        අතරතුර (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ආපසු s;
}

bool is_localhost(const var_na *hostname) {
        assert(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ආපසු STRCASE_IN_SET(
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

        නම් (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ආපසු cached;
}

const var_na* etc_machine_info(void) {
        static const var_na *cached = NULL;

        නම් (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ආපසු cached;
}

පූර්ණ_සංඛ්‍යා get_pretty_hostname(var_na **ret) {
        _cleanup_free_ var_na *n = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = parse_env_file(NULL, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        නම් (r < 0)
                ආපසු r;

        නම් (isempty(n))
                ආපසු -ENXIO;

        *ret = TAKE_PTR(n);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා split_user_at_host(const var_na *s, var_na **ret_user, var_na **ret_host) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        assert(s);

        const var_na *rhs = strchr(s, '@');
        නම් (rhs) {
                නම් (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        නම් (!u)
                                ආපසු -ENOMEM;
                }

                නම් (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        නම් (!h)
                                ආපසු -ENOMEM;
                }

        } නැතහොත් {
                නම් (isempty(s))
                        ආපසු -EINVAL;

                නම් (ret_host) {
                        h = strdup(s);
                        නම් (!h)
                                ආපසු -ENOMEM;
                }
        }

        නම් (ret_user)
                *ret_user = TAKE_PTR(u);
        නම් (ret_host)
                *ret_host = TAKE_PTR(h);

        ආපසු !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

පූර්ණ_සංඛ්‍යා machine_spec_valid(const var_na *s) {
        _cleanup_free_ var_na *u = NULL, *h = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);

        r = split_user_at_host(s, &u, &h);
        නම් (r == -EINVAL)
                ආපසු false;
        නම් (r < 0)
                ආපසු r;

        නම් (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ආපසු false;

        නම් (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ආපසු false;

        ආපසු true;
}

bool machine_tag_is_valid(const var_na *s) {
        size_t n = strlen_ptr(s);
        නම් (n <= 0 || n >= 256)
                ආපසු false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        නම් (strchr("-.=", s[0]))
                ආපසු false;

        /* We allow parameterization of tags, with a "=" as separator */
        const var_na *eq = strchr(s, '=');
        නම් (eq) {
                assert(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                නම් (strchr("-.", eq[-1]))
                        ආපසු false;
        } නැතහොත් {
                /* If there's no '=', then make the restriction on the very last character */
                නම් (strchr("-.", s[n-1]))
                        ආපසු false;
        }

        ආපසු in_charset(s, ALPHANUMERICAL "-.=");
}

පූර්ණ_සංඛ්‍යා machine_tag_list_is_valid(var_na **l) {
        පූර්ණ_සංඛ්‍යා r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ NULL);
        switch (r) {
        pak_sa -EINVAL:
        pak_sa -E2BIG:
                ආපසු false;
        pak_sa 0:
                ආපසු true;
        default:
                ආපසු r;
        }
}

පූර්ණ_සංඛ්‍යා machine_tags_from_string(const var_na *s, bool graceful, var_na ***ret) {
        assert(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        නම් (isempty(s)) {
                *ret = NULL;
                ආපසු 0;
        }

        _cleanup_strv_free_ var_na **l = strv_split(s, ":");
        නම් (!l)
                ආපසු -ENOMEM;

        ආපසු machine_tags_from_strv(l, graceful, ret);
}

පූර්ණ_සංඛ්‍යා machine_tags_from_strv(var_na **l, bool graceful, var_na ***ret) {
        පූර්ණ_සංඛ්‍යා r;

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
                නම් (!machine_tag_is_valid(*i)) {
                        නම් (graceful)
                                continue;

                        ආපසු -EINVAL;
                }

                නම් (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ආපසු -E2BIG;

                const var_na *eq = strchrnul(*i, '=');
                _cleanup_free_ var_na *k = strndup(*i, (eq - *i) + (*eq == '='));
                නම් (!k)
                        ආපසු -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                නම් (r == -EEXIST) {
                        නම් (streq(hashmap_get(h, k), *i))
                                continue;
                        නම් (!graceful)
                                ආපසු -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } නැතහොත් නම් (r < 0)
                        ආපසු r;
                නැතහොත් නම් (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        නම් (ret) {
                _cleanup_strv_free_ var_na **cleaned = NULL;
                size_t n = 0;
                var_na *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        නම් (r < 0)
                                ආපසු r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ආපසු 0;
}
