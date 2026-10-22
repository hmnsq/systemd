/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-file.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "hostname-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "os-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ವರ್ಣ* get_default_hostname_raw(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ಯದಿ (e) {
                ಯದಿ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ಪ್ರತ್ಯಾಗಮ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ ವರ್ಣ *f = ರಿಕ್ತ;
        r = parse_os_release(ರಿಕ್ತ, "DEFAULT_HOSTNAME", &f);
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ಅನ್ಯಥಾ ಯದಿ (f) {
                ಯದಿ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ಪ್ರತ್ಯಾಗಮ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(ವರ್ಣ c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ಪ್ರತ್ಯಾಗಮ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ಧ್ರುವ ವರ್ಣ *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ಧ್ರುವ ವರ್ಣ *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ಯದಿ (isempty(s))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        ಪ್ರತಿ (p = s, dot = hyphen = true; *p; p++)
                ಯದಿ (*p == '.') {
                        ಯದಿ (dot || hyphen)
                                ಪ್ರತ್ಯಾಗಮ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ಅನ್ಯಥಾ ಯದಿ (*p == '-') {
                        ಯದಿ (dot)
                                ಪ್ರತ್ಯಾಗಮ false;

                        dot = false;
                        hyphen = true;

                } ಅನ್ಯಥಾ {
                        ಯದಿ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ಪ್ರತ್ಯಾಗಮ false;

                        dot = false;
                        hyphen = false;
                }

        ಯದಿ (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (hyphen)
                ಪ್ರತ್ಯಾಗಮ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ಯದಿ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ವರ್ಣ* hostname_cleanup(ವರ್ಣ *s) {
        ವರ್ಣ *p, *d;
        bool dot, hyphen;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ಯದಿ (*p == '.') {
                        ಯದಿ (dot || hyphen)
                                ಅನುವರ್ತ;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ಅನ್ಯಥಾ ಯದಿ (*p == '-') {
                        ಯದಿ (dot)
                                ಅನುವರ್ತ;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ಅನ್ಯಥಾ ಯದಿ (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ಯಾವತ್ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ಪ್ರತ್ಯಾಗಮ s;
}

bool is_localhost(ಧ್ರುವ ವರ್ಣ *hostname) {
        ಪ್ರತಿಜ್ಞಾ(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ಪ್ರತ್ಯಾಗಮ STRCASE_IN_SET(
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

ಧ್ರುವ ವರ್ಣ* etc_hostname(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *cached = ರಿಕ್ತ;

        ಯದಿ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ಪ್ರತ್ಯಾಗಮ cached;
}

ಧ್ರುವ ವರ್ಣ* etc_machine_info(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *cached = ರಿಕ್ತ;

        ಯದಿ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ಪ್ರತ್ಯಾಗಮ cached;
}

ಪೂರ್ಣಾಙ್ಕ get_pretty_hostname(ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *n = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file(ರಿಕ್ತ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (isempty(n))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        *ret = TAKE_PTR(n);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ split_user_at_host(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ **ret_user, ವರ್ಣ **ret_host) {
        _cleanup_free_ ವರ್ಣ *u = ರಿಕ್ತ, *h = ರಿಕ್ತ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ಪ್ರತಿಜ್ಞಾ(s);

        ಧ್ರುವ ವರ್ಣ *rhs = strchr(s, '@');
        ಯದಿ (rhs) {
                ಯದಿ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ಯದಿ (!u)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

                ಯದಿ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ಯದಿ (!h)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }

        } ಅನ್ಯಥಾ {
                ಯದಿ (isempty(s))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (ret_host) {
                        h = strdup(s);
                        ಯದಿ (!h)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }
        }

        ಯದಿ (ret_user)
                *ret_user = TAKE_PTR(u);
        ಯದಿ (ret_host)
                *ret_host = TAKE_PTR(h);

        ಪ್ರತ್ಯಾಗಮ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

ಪೂರ್ಣಾಙ್ಕ machine_spec_valid(ಧ್ರುವ ವರ್ಣ *s) {
        _cleanup_free_ ವರ್ಣ *u = ರಿಕ್ತ, *h = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = split_user_at_host(s, &u, &h);
        ಯದಿ (r == -EINVAL)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool machine_tag_is_valid(ಧ್ರುವ ವರ್ಣ *s) {
        size_t n = strlen_ptr(s);
        ಯದಿ (n <= 0 || n >= 256)
                ಪ್ರತ್ಯಾಗಮ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ಯದಿ (strchr("-.=", s[0]))
                ಪ್ರತ್ಯಾಗಮ false;

        /* We allow parameterization of tags, with a "=" as separator */
        ಧ್ರುವ ವರ್ಣ *eq = strchr(s, '=');
        ಯದಿ (eq) {
                ಪ್ರತಿಜ್ಞಾ(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ಯದಿ (strchr("-.", eq[-1]))
                        ಪ್ರತ್ಯಾಗಮ false;
        } ಅನ್ಯಥಾ {
                /* If there's no '=', then make the restriction on the very last character */
                ಯದಿ (strchr("-.", s[n-1]))
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ in_charset(s, ALPHANUMERICAL "-.=");
}

ಪೂರ್ಣಾಙ್ಕ machine_tag_list_is_valid(ವರ್ಣ **l) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ ರಿಕ್ತ);
        ವಿಕಲ್ಪ (r) {
        ಪಕ್ಷ -EINVAL:
        ಪಕ್ಷ -E2BIG:
                ಪ್ರತ್ಯಾಗಮ false;
        ಪಕ್ಷ 0:
                ಪ್ರತ್ಯಾಗಮ true;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ r;
        }
}

ಪೂರ್ಣಾಙ್ಕ machine_tags_from_string(ಧ್ರುವ ವರ್ಣ *s, bool graceful, ವರ್ಣ ***ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ಯದಿ (isempty(s)) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_strv_free_ ವರ್ಣ **l = strv_split(s, ":");
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ machine_tags_from_strv(l, graceful, ret);
}

ಪೂರ್ಣಾಙ್ಕ machine_tags_from_strv(ವರ್ಣ **l, bool graceful, ವರ್ಣ ***ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ರಿಕ್ತ;
        STRV_FOREACH(i, l) {
                ಯದಿ (!machine_tag_is_valid(*i)) {
                        ಯದಿ (graceful)
                                ಅನುವರ್ತ;

                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                }

                ಯದಿ (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ಪ್ರತ್ಯಾಗಮ -E2BIG;

                ಧ್ರುವ ವರ್ಣ *eq = strchrnul(*i, '=');
                _cleanup_free_ ವರ್ಣ *k = strndup(*i, (eq - *i) + (*eq == '='));
                ಯದಿ (!k)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ಯದಿ (r == -EEXIST) {
                        ಯದಿ (streq(hashmap_get(h, k), *i))
                                ಅನುವರ್ತ;
                        ಯದಿ (!graceful)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ಅನ್ಯಥಾ ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಅನ್ಯಥಾ ಯದಿ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ಯದಿ (ret) {
                _cleanup_strv_free_ ವರ್ಣ **cleaned = ರಿಕ್ತ;
                size_t n = 0;
                ವರ್ಣ *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}
