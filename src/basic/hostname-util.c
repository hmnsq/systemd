/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-file.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "hostname-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "os-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "user-util.h"

అక్షరం* get_default_hostname_raw(శూన్య) {
        పూర్ణాంకం r;

        /* Returns the default hostname, and leaves any ??? in place. */

        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ఒకవేళ (e) {
                ఒకవేళ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_మాట_TOKEN))
                        తిరిగి strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ అక్షరం *f = ఖాళీ;
        r = parse_os_release(ఖాళీ, "DEFAULT_HOSTNAME", &f);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        లేకపోతే ఒకవేళ (f) {
                ఒకవేళ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_మాట_TOKEN))
                        తిరిగి TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        తిరిగి strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(అక్షరం c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        తిరిగి ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(స్థిరరాశి అక్షరం *s, ValidHostnameFlags flags) {
        గుర్తులేని n_dots = 0;
        స్థిరరాశి అక్షరం *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ఒకవేళ (isempty(s))
                తిరిగి false;

        ఒకవేళ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                తిరిగి FLAGS_సమితి(flags, VALID_HOSTNAME_DOT_HOST);

        కొరకు (p = s, dot = hyphen = true; *p; p++)
                ఒకవేళ (*p == '.') {
                        ఒకవేళ (dot || hyphen)
                                తిరిగి false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } లేకపోతే ఒకవేళ (*p == '-') {
                        ఒకవేళ (dot)
                                తిరిగి false;

                        dot = false;
                        hyphen = true;

                } లేకపోతే {
                        ఒకవేళ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_సమితి(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_సమితి(flags, VALID_HOSTNAME_మాట_TOKEN)))
                                తిరిగి false;

                        dot = false;
                        hyphen = false;
                }

        ఒకవేళ (dot && (n_dots < 2 || !FLAGS_సమితి(flags, VALID_HOSTNAME_TRAILING_DOT)))
                తిరిగి false;
        ఒకవేళ (hyphen)
                తిరిగి false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ఒకవేళ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                తిరిగి false;

        తిరిగి true;
}

అక్షరం* hostname_cleanup(అక్షరం *s) {
        అక్షరం *p, *d;
        bool dot, hyphen;

        ప్రతిజ్ఞా(s);

        కొరకు (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ఒకవేళ (*p == '.') {
                        ఒకవేళ (dot || hyphen)
                                కొనసాగు;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } లేకపోతే ఒకవేళ (*p == '-') {
                        ఒకవేళ (dot)
                                కొనసాగు;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } లేకపోతే ఒకవేళ (valid_ldh_char(*p) || లోపల_సమితి(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        అయితే (d > s && లోపల_సమితి(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        తిరిగి s;
}

bool is_localhost(స్థిరరాశి అక్షరం *hostname) {
        ప్రతిజ్ఞా(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        తిరిగి STRCASE_లోపల_సమితి(
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

స్థిరరాశి అక్షరం* etc_hostname(శూన్య) {
        స్థిర స్థిరరాశి అక్షరం *cached = ఖాళీ;

        ఒకవేళ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        తిరిగి cached;
}

స్థిరరాశి అక్షరం* etc_machine_info(శూన్య) {
        స్థిర స్థిరరాశి అక్షరం *cached = ఖాళీ;

        ఒకవేళ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        తిరిగి cached;
}

పూర్ణాంకం get_pretty_hostname(అక్షరం **ret) {
        _cleanup_free_ అక్షరం *n = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = parse_env_file(ఖాళీ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (isempty(n))
                తిరిగి -ENXIO;

        *ret = TAKE_PTR(n);
        తిరిగి 0;
}

పూర్ణాంకం విభజించు_user_at_host(స్థిరరాశి అక్షరం *s, అక్షరం **ret_user, అక్షరం **ret_host) {
        _cleanup_free_ అక్షరం *u = ఖాళీ, *h = ఖాళీ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ప్రతిజ్ఞా(s);

        స్థిరరాశి అక్షరం *rhs = strchr(s, '@');
        ఒకవేళ (rhs) {
                ఒకవేళ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ఒకవేళ (!u)
                                తిరిగి -ENOMEM;
                }

                ఒకవేళ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ఒకవేళ (!h)
                                తిరిగి -ENOMEM;
                }

        } లేకపోతే {
                ఒకవేళ (isempty(s))
                        తిరిగి -EINVAL;

                ఒకవేళ (ret_host) {
                        h = strdup(s);
                        ఒకవేళ (!h)
                                తిరిగి -ENOMEM;
                }
        }

        ఒకవేళ (ret_user)
                *ret_user = TAKE_PTR(u);
        ఒకవేళ (ret_host)
                *ret_host = TAKE_PTR(h);

        తిరిగి !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

పూర్ణాంకం machine_spec_valid(స్థిరరాశి అక్షరం *s) {
        _cleanup_free_ అక్షరం *u = ఖాళీ, *h = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = విభజించు_user_at_host(s, &u, &h);
        ఒకవేళ (r == -EINVAL)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (u && !valid_user_సమూహం_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                తిరిగి false;

        ఒకవేళ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                తిరిగి false;

        తిరిగి true;
}

bool machine_ట్యాగ్_is_valid(స్థిరరాశి అక్షరం *s) {
        size_t n = strlen_ptr(s);
        ఒకవేళ (n <= 0 || n >= 256)
                తిరిగి false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ఒకవేళ (strchr("-.=", s[0]))
                తిరిగి false;

        /* We allow parameterization of tags, with a "=" as separator */
        స్థిరరాశి అక్షరం *eq = strchr(s, '=');
        ఒకవేళ (eq) {
                ప్రతిజ్ఞా(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ఒకవేళ (strchr("-.", eq[-1]))
                        తిరిగి false;
        } లేకపోతే {
                /* If there's no '=', then make the restriction on the very last character */
                ఒకవేళ (strchr("-.", s[n-1]))
                        తిరిగి false;
        }

        తిరిగి లోపల_charset(s, ALPHANUMERICAL "-.=");
}

పూర్ణాంకం machine_ట్యాగ్_list_is_valid(అక్షరం **l) {
        పూర్ణాంకం r;

        r = machine_ట్యాగులు_from_strv(l, /* graceful= */ false, /* ret= */ ఖాళీ);
        ఎంపిక (r) {
        స్థితి -EINVAL:
        స్థితి -E2BIG:
                తిరిగి false;
        స్థితి 0:
                తిరిగి true;
        అప్రమేయ:
                తిరిగి r;
        }
}

పూర్ణాంకం machine_ట్యాగులు_from_string(స్థిరరాశి అక్షరం *s, bool graceful, అక్షరం ***ret) {
        ప్రతిజ్ఞా(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ఒకవేళ (isempty(s)) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        _cleanup_strv_free_ అక్షరం **l = strv_విభజించు(s, ":");
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        తిరిగి machine_ట్యాగులు_from_strv(l, graceful, ret);
}

పూర్ణాంకం machine_ట్యాగులు_from_strv(అక్షరం **l, bool graceful, అక్షరం ***ret) {
        పూర్ణాంకం r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ఖాళీ;
        STRV_FOREACH(i, l) {
                ఒకవేళ (!machine_ట్యాగ్_is_valid(*i)) {
                        ఒకవేళ (graceful)
                                కొనసాగు;

                        తిరిగి -EINVAL;
                }

                ఒకవేళ (hashmap_కొలత_2(h) >= MACHINE_ట్యాగులు_MAX)
                        తిరిగి -E2BIG;

                స్థిరరాశి అక్షరం *eq = strchrnul(*i, '=');
                _cleanup_free_ అక్షరం *k = strndup(*i, (eq - *i) + (*eq == '='));
                ఒకవేళ (!k)
                        తిరిగి -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ఒకవేళ (r == -EEXIST) {
                        ఒకవేళ (streq(hashmap_get(h, k), *i))
                                కొనసాగు;
                        ఒకవేళ (!graceful)
                                తిరిగి -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } లేకపోతే ఒకవేళ (r < 0)
                        తిరిగి r;
                లేకపోతే ఒకవేళ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ఒకవేళ (ret) {
                _cleanup_strv_free_ అక్షరం **cleaned = ఖాళీ;
                size_t n = 0;
                అక్షరం *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_కొలత(&cleaned, &n, v);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                strv_క్రమీకరించు(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        తిరిగి 0;
}
