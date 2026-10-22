/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hostname-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "os-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਅੱਖਰ* get_default_hostname_raw(ਖਾਲੀ) {
        ਪੂਰਨ r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ਜੇ (e) {
                ਜੇ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_ਸ਼ਬਦ_TOKEN))
                        ਵਾਪਸ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ ਅੱਖਰ *f = ਨਿਰਾ;
        r = parse_os_release(ਨਿਰਾ, "DEFAULT_HOSTNAME", &f);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ਨਹੀਂਤਾਂ ਜੇ (f) {
                ਜੇ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_ਸ਼ਬਦ_TOKEN))
                        ਵਾਪਸ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ਵਾਪਸ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(ਅੱਖਰ c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ਵਾਪਸ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ਅਚਲ ਅੱਖਰ *s, ValidHostnameFlags flags) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_dots = 0;
        ਅਚਲ ਅੱਖਰ *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ਜੇ (isempty(s))
                ਵਾਪਸ false;

        ਜੇ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, VALID_HOSTNAME_DOT_HOST);

        ਲਈ (p = s, dot = hyphen = true; *p; p++)
                ਜੇ (*p == '.') {
                        ਜੇ (dot || hyphen)
                                ਵਾਪਸ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ਨਹੀਂਤਾਂ ਜੇ (*p == '-') {
                        ਜੇ (dot)
                                ਵਾਪਸ false;

                        dot = false;
                        hyphen = true;

                } ਨਹੀਂਤਾਂ {
                        ਜੇ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_ਸੈੱਟ(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_ਸੈੱਟ(flags, VALID_HOSTNAME_ਸ਼ਬਦ_TOKEN)))
                                ਵਾਪਸ false;

                        dot = false;
                        hyphen = false;
                }

        ਜੇ (dot && (n_dots < 2 || !FLAGS_ਸੈੱਟ(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ਵਾਪਸ false;
        ਜੇ (hyphen)
                ਵਾਪਸ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ਜੇ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਅੱਖਰ* hostname_cleanup(ਅੱਖਰ *s) {
        ਅੱਖਰ *p, *d;
        bool dot, hyphen;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ਜੇ (*p == '.') {
                        ਜੇ (dot || hyphen)
                                ਜਾਰੀ;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ਨਹੀਂਤਾਂ ਜੇ (*p == '-') {
                        ਜੇ (dot)
                                ਜਾਰੀ;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ਨਹੀਂਤਾਂ ਜੇ (valid_ldh_char(*p) || ਵਿੱਚ_ਸੈੱਟ(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ਜਦੋਂ (d > s && ਵਿੱਚ_ਸੈੱਟ(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ਵਾਪਸ s;
}

bool is_localhost(ਅਚਲ ਅੱਖਰ *hostname) {
        ਪ੍ਰਤਿਜ੍ਞਾ(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ਵਾਪਸ STRCASE_ਵਿੱਚ_ਸੈੱਟ(
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

ਅਚਲ ਅੱਖਰ* etc_hostname(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ *cached = ਨਿਰਾ;

        ਜੇ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ਵਾਪਸ cached;
}

ਅਚਲ ਅੱਖਰ* etc_machine_info(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ *cached = ਨਿਰਾ;

        ਜੇ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ਵਾਪਸ cached;
}

ਪੂਰਨ get_pretty_hostname(ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *n = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_env_file(ਨਿਰਾ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (isempty(n))
                ਵਾਪਸ -ENXIO;

        *ret = TAKE_PTR(n);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਵੰਡੋ_user_at_host(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ **ret_user, ਅੱਖਰ **ret_host) {
        _cleanup_free_ ਅੱਖਰ *u = ਨਿਰਾ, *h = ਨਿਰਾ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਅਚਲ ਅੱਖਰ *rhs = strchr(s, '@');
        ਜੇ (rhs) {
                ਜੇ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ਜੇ (!u)
                                ਵਾਪਸ -ENOMEM;
                }

                ਜੇ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ਜੇ (!h)
                                ਵਾਪਸ -ENOMEM;
                }

        } ਨਹੀਂਤਾਂ {
                ਜੇ (isempty(s))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (ret_host) {
                        h = strdup(s);
                        ਜੇ (!h)
                                ਵਾਪਸ -ENOMEM;
                }
        }

        ਜੇ (ret_user)
                *ret_user = TAKE_PTR(u);
        ਜੇ (ret_host)
                *ret_host = TAKE_PTR(h);

        ਵਾਪਸ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

ਪੂਰਨ machine_spec_valid(ਅਚਲ ਅੱਖਰ *s) {
        _cleanup_free_ ਅੱਖਰ *u = ਨਿਰਾ, *h = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਵੰਡੋ_user_at_host(s, &u, &h);
        ਜੇ (r == -EINVAL)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (u && !valid_user_ਸਮੂਹ_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ਵਾਪਸ false;

        ਜੇ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool machine_ਟੈਗ_is_valid(ਅਚਲ ਅੱਖਰ *s) {
        size_t n = strlen_ptr(s);
        ਜੇ (n <= 0 || n >= 256)
                ਵਾਪਸ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ਜੇ (strchr("-.=", s[0]))
                ਵਾਪਸ false;

        /* We allow parameterization of tags, with a "=" as separator */
        ਅਚਲ ਅੱਖਰ *eq = strchr(s, '=');
        ਜੇ (eq) {
                ਪ੍ਰਤਿਜ੍ਞਾ(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ਜੇ (strchr("-.", eq[-1]))
                        ਵਾਪਸ false;
        } ਨਹੀਂਤਾਂ {
                /* If there's no '=', then make the restriction on the very last character */
                ਜੇ (strchr("-.", s[n-1]))
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ ਵਿੱਚ_charset(s, ALPHANUMERICAL "-.=");
}

ਪੂਰਨ machine_ਟੈਗ_list_is_valid(ਅੱਖਰ **l) {
        ਪੂਰਨ r;

        r = machine_ਟੈਗ_ਸੂਚੀ_from_strv(l, /* graceful= */ false, /* ret= */ ਨਿਰਾ);
        ਚੋਣ (r) {
        ਹਾਲਤ -EINVAL:
        ਹਾਲਤ -E2BIG:
                ਵਾਪਸ false;
        ਹਾਲਤ 0:
                ਵਾਪਸ true;
        ਮੂਲ:
                ਵਾਪਸ r;
        }
}

ਪੂਰਨ machine_ਟੈਗ_ਸੂਚੀ_from_string(ਅਚਲ ਅੱਖਰ *s, bool graceful, ਅੱਖਰ ***ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ਜੇ (isempty(s)) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        _cleanup_strv_free_ ਅੱਖਰ **l = strv_ਵੰਡੋ(s, ":");
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ machine_ਟੈਗ_ਸੂਚੀ_from_strv(l, graceful, ret);
}

ਪੂਰਨ machine_ਟੈਗ_ਸੂਚੀ_from_strv(ਅੱਖਰ **l, bool graceful, ਅੱਖਰ ***ret) {
        ਪੂਰਨ r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ਨਿਰਾ;
        STRV_FOREACH(i, l) {
                ਜੇ (!machine_ਟੈਗ_is_valid(*i)) {
                        ਜੇ (graceful)
                                ਜਾਰੀ;

                        ਵਾਪਸ -EINVAL;
                }

                ਜੇ (hashmap_ਮਾਪ_2(h) >= MACHINE_ਟੈਗ_ਸੂਚੀ_MAX)
                        ਵਾਪਸ -E2BIG;

                ਅਚਲ ਅੱਖਰ *eq = strchrnul(*i, '=');
                _cleanup_free_ ਅੱਖਰ *k = strndup(*i, (eq - *i) + (*eq == '='));
                ਜੇ (!k)
                        ਵਾਪਸ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ਜੇ (r == -EEXIST) {
                        ਜੇ (streq(hashmap_get(h, k), *i))
                                ਜਾਰੀ;
                        ਜੇ (!graceful)
                                ਵਾਪਸ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਨਹੀਂਤਾਂ ਜੇ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ਜੇ (ret) {
                _cleanup_strv_free_ ਅੱਖਰ **cleaned = ਨਿਰਾ;
                size_t n = 0;
                ਅੱਖਰ *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_ਮਾਪ(&cleaned, &n, v);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                strv_ਛਾਂਟੋ(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ਵਾਪਸ 0;
}
