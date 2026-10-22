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

ಅಕ್ಷರ* get_default_hostname_raw(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ಆದರೆ (e) {
                ಆದರೆ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_ಶಬ್ದ_TOKEN))
                        ಹಿಂತಿರುಗಿ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ ಅಕ್ಷರ *f = ಖಾಲಿ;
        r = parse_os_release(ಖಾಲಿ, "DEFAULT_HOSTNAME", &f);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f) {
                ಆದರೆ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_ಶಬ್ದ_TOKEN))
                        ಹಿಂತಿರುಗಿ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ಹಿಂತಿರುಗಿ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(ಅಕ್ಷರ c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ಹಿಂತಿರುಗಿ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ValidHostnameFlags flags) {
        ಚಿಹ್ನೆರಹಿತ n_dots = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ಆದರೆ (isempty(s))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, VALID_HOSTNAME_DOT_HOST);

        ಗಾಗಿ (p = s, dot = hyphen = true; *p; p++)
                ಆದರೆ (*p == '.') {
                        ಆದರೆ (dot || hyphen)
                                ಹಿಂತಿರುಗಿ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '-') {
                        ಆದರೆ (dot)
                                ಹಿಂತಿರುಗಿ false;

                        dot = false;
                        hyphen = true;

                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_ಗಣ(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_ಗಣ(flags, VALID_HOSTNAME_ಶಬ್ದ_TOKEN)))
                                ಹಿಂತಿರುಗಿ false;

                        dot = false;
                        hyphen = false;
                }

        ಆದರೆ (dot && (n_dots < 2 || !FLAGS_ಗಣ(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (hyphen)
                ಹಿಂತಿರುಗಿ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ಆದರೆ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಅಕ್ಷರ* hostname_cleanup(ಅಕ್ಷರ *s) {
        ಅಕ್ಷರ *p, *d;
        bool dot, hyphen;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ಆದರೆ (*p == '.') {
                        ಆದರೆ (dot || hyphen)
                                ಮುಂದುವರಿ;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '-') {
                        ಆದರೆ (dot)
                                ಮುಂದುವರಿ;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (valid_ldh_char(*p) || ಒಳ_ಗಣ(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ಆಗ (d > s && ಒಳ_ಗಣ(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ಹಿಂತಿರುಗಿ s;
}

bool is_localhost(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname) {
        ಪ್ರತಿಜ್ಞಾ(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ಹಿಂತಿರುಗಿ STRCASE_ಒಳ_ಗಣ(
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

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_hostname(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached = ಖಾಲಿ;

        ಆದರೆ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ಹಿಂತಿರುಗಿ cached;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_machine_info(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached = ಖಾಲಿ;

        ಆದರೆ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ಹಿಂತಿರುಗಿ cached;
}

ಪೂರ್ಣಾಂಕ get_pretty_hostname(ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *n = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file(ಖಾಲಿ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (isempty(n))
                ಹಿಂತಿರುಗಿ -ENXIO;

        *ret = TAKE_PTR(n);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು_user_at_host(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ **ret_user, ಅಕ್ಷರ **ret_host) {
        _cleanup_free_ ಅಕ್ಷರ *u = ಖಾಲಿ, *h = ಖಾಲಿ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ಪ್ರತಿಜ್ಞಾ(s);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rhs = strchr(s, '@');
        ಆದರೆ (rhs) {
                ಆದರೆ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ಆದರೆ (!u)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

                ಆದರೆ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ಆದರೆ (!h)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }

        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (isempty(s))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (ret_host) {
                        h = strdup(s);
                        ಆದರೆ (!h)
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                }
        }

        ಆದರೆ (ret_user)
                *ret_user = TAKE_PTR(u);
        ಆದರೆ (ret_host)
                *ret_host = TAKE_PTR(h);

        ಹಿಂತಿರುಗಿ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

ಪೂರ್ಣಾಂಕ machine_spec_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        _cleanup_free_ ಅಕ್ಷರ *u = ಖಾಲಿ, *h = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ವಿಭಜಿಸು_user_at_host(s, &u, &h);
        ಆದರೆ (r == -EINVAL)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (u && !valid_user_ಗುಂಪು_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool machine_ಟ್ಯಾಗ್_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        size_t n = strlen_ptr(s);
        ಆದರೆ (n <= 0 || n >= 256)
                ಹಿಂತಿರುಗಿ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ಆದರೆ (strchr("-.=", s[0]))
                ಹಿಂತಿರುಗಿ false;

        /* We allow parameterization of tags, with a "=" as separator */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *eq = strchr(s, '=');
        ಆದರೆ (eq) {
                ಪ್ರತಿಜ್ಞಾ(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ಆದರೆ (strchr("-.", eq[-1]))
                        ಹಿಂತಿರುಗಿ false;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* If there's no '=', then make the restriction on the very last character */
                ಆದರೆ (strchr("-.", s[n-1]))
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ ಒಳ_charset(s, ALPHANUMERICAL "-.=");
}

ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗ್_list_is_valid(ಅಕ್ಷರ **l) {
        ಪೂರ್ಣಾಂಕ r;

        r = machine_ಟ್ಯಾಗುಗಳು_from_strv(l, /* graceful= */ false, /* ret= */ ಖಾಲಿ);
        ಆಯ್ಕೆ (r) {
        ಸ್ಥಿತಿ -EINVAL:
        ಸ್ಥಿತಿ -E2BIG:
                ಹಿಂತಿರುಗಿ false;
        ಸ್ಥಿತಿ 0:
                ಹಿಂತಿರುಗಿ true;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ r;
        }
}

ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗುಗಳು_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool graceful, ಅಕ್ಷರ ***ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ಆದರೆ (isempty(s)) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_strv_free_ ಅಕ್ಷರ **l = strv_ವಿಭಜಿಸು(s, ":");
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ machine_ಟ್ಯಾಗುಗಳು_from_strv(l, graceful, ret);
}

ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗುಗಳು_from_strv(ಅಕ್ಷರ **l, bool graceful, ಅಕ್ಷರ ***ret) {
        ಪೂರ್ಣಾಂಕ r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ಖಾಲಿ;
        STRV_FOREACH(i, l) {
                ಆದರೆ (!machine_ಟ್ಯಾಗ್_is_valid(*i)) {
                        ಆದರೆ (graceful)
                                ಮುಂದುವರಿ;

                        ಹಿಂತಿರುಗಿ -EINVAL;
                }

                ಆದರೆ (hashmap_ಗಾತ್ರ_2(h) >= MACHINE_ಟ್ಯಾಗುಗಳು_MAX)
                        ಹಿಂತಿರುಗಿ -E2BIG;

                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *eq = strchrnul(*i, '=');
                _cleanup_free_ ಅಕ್ಷರ *k = strndup(*i, (eq - *i) + (*eq == '='));
                ಆದರೆ (!k)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ಆದರೆ (r == -EEXIST) {
                        ಆದರೆ (streq(hashmap_get(h, k), *i))
                                ಮುಂದುವರಿ;
                        ಆದರೆ (!graceful)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ಆದರೆ (ret) {
                _cleanup_strv_free_ ಅಕ್ಷರ **cleaned = ಖಾಲಿ;
                size_t n = 0;
                ಅಕ್ಷರ *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_ಗಾತ್ರ(&cleaned, &n, v);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                strv_ವಿಂಗಡಿಸು(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ಹಿಂತಿರುಗಿ 0;
}
