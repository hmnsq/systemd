/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-file.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "hostname-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "os-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "user-util.h"

वर्ण* get_default_hostname_raw(शून्य) {
        पूर्णाङ्क r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        यदि (e) {
                यदि (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        प्रत्यागम strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ वर्ण *f = रिक्त;
        r = parse_os_release(रिक्त, "DEFAULT_HOSTNAME", &f);
        यदि (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        अन्यथा यदि (f) {
                यदि (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        प्रत्यागम TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        प्रत्यागम strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(वर्ण c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        प्रत्यागम ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ध्रुव वर्ण *s, ValidHostnameFlags flags) {
        अचिह्नित n_dots = 0;
        ध्रुव वर्ण *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        यदि (isempty(s))
                प्रत्यागम false;

        यदि (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                प्रत्यागम FLAGS_समुच्चयः(flags, VALID_HOSTNAME_DOT_HOST);

        प्रति (p = s, dot = hyphen = true; *p; p++)
                यदि (*p == '.') {
                        यदि (dot || hyphen)
                                प्रत्यागम false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } अन्यथा यदि (*p == '-') {
                        यदि (dot)
                                प्रत्यागम false;

                        dot = false;
                        hyphen = true;

                } अन्यथा {
                        यदि (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_समुच्चयः(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_समुच्चयः(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                प्रत्यागम false;

                        dot = false;
                        hyphen = false;
                }

        यदि (dot && (n_dots < 2 || !FLAGS_समुच्चयः(flags, VALID_HOSTNAME_TRAILING_DOT)))
                प्रत्यागम false;
        यदि (hyphen)
                प्रत्यागम false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        यदि (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                प्रत्यागम false;

        प्रत्यागम true;
}

वर्ण* hostname_cleanup(वर्ण *s) {
        वर्ण *p, *d;
        bool dot, hyphen;

        प्रतिज्ञा(s);

        प्रति (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                यदि (*p == '.') {
                        यदि (dot || hyphen)
                                अनुवर्त;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } अन्यथा यदि (*p == '-') {
                        यदि (dot)
                                अनुवर्त;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } अन्यथा यदि (valid_ldh_char(*p) || अन्तर्_समुच्चयः(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        यावत् (d > s && अन्तर्_समुच्चयः(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        प्रत्यागम s;
}

bool is_localhost(ध्रुव वर्ण *hostname) {
        प्रतिज्ञा(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        प्रत्यागम STRCASE_अन्तर्_समुच्चयः(
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

ध्रुव वर्ण* etc_hostname(शून्य) {
        स्थिर ध्रुव वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        प्रत्यागम cached;
}

ध्रुव वर्ण* etc_machine_info(शून्य) {
        स्थिर ध्रुव वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        प्रत्यागम cached;
}

पूर्णाङ्क get_pretty_hostname(वर्ण **ret) {
        _cleanup_free_ वर्ण *n = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = parse_env_file(रिक्त, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (isempty(n))
                प्रत्यागम -ENXIO;

        *ret = TAKE_PTR(n);
        प्रत्यागम 0;
}

पूर्णाङ्क विभज_user_at_host(ध्रुव वर्ण *s, वर्ण **ret_user, वर्ण **ret_host) {
        _cleanup_free_ वर्ण *u = रिक्त, *h = रिक्त;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        प्रतिज्ञा(s);

        ध्रुव वर्ण *rhs = strchr(s, '@');
        यदि (rhs) {
                यदि (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        यदि (!u)
                                प्रत्यागम -ENOMEM;
                }

                यदि (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        यदि (!h)
                                प्रत्यागम -ENOMEM;
                }

        } अन्यथा {
                यदि (isempty(s))
                        प्रत्यागम -EINVAL;

                यदि (ret_host) {
                        h = strdup(s);
                        यदि (!h)
                                प्रत्यागम -ENOMEM;
                }
        }

        यदि (ret_user)
                *ret_user = TAKE_PTR(u);
        यदि (ret_host)
                *ret_host = TAKE_PTR(h);

        प्रत्यागम !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

पूर्णाङ्क machine_spec_valid(ध्रुव वर्ण *s) {
        _cleanup_free_ वर्ण *u = रिक्त, *h = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = विभज_user_at_host(s, &u, &h);
        यदि (r == -EINVAL)
                प्रत्यागम false;
        यदि (r < 0)
                प्रत्यागम r;

        यदि (u && !valid_user_गणः_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                प्रत्यागम false;

        यदि (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                प्रत्यागम false;

        प्रत्यागम true;
}

bool machine_चिह्नकम्_is_valid(ध्रुव वर्ण *s) {
        size_t n = strlen_ptr(s);
        यदि (n <= 0 || n >= 256)
                प्रत्यागम false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        यदि (strchr("-.=", s[0]))
                प्रत्यागम false;

        /* We allow parameterization of tags, with a "=" as separator */
        ध्रुव वर्ण *eq = strchr(s, '=');
        यदि (eq) {
                प्रतिज्ञा(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                यदि (strchr("-.", eq[-1]))
                        प्रत्यागम false;
        } अन्यथा {
                /* If there's no '=', then make the restriction on the very last character */
                यदि (strchr("-.", s[n-1]))
                        प्रत्यागम false;
        }

        प्रत्यागम अन्तर्_charset(s, ALPHANUMERICAL "-.=");
}

पूर्णाङ्क machine_चिह्नकम्_list_is_valid(वर्ण **l) {
        पूर्णाङ्क r;

        r = machine_चिह्नकानि_from_strv(l, /* graceful= */ false, /* ret= */ रिक्त);
        विकल्प (r) {
        पक्ष -EINVAL:
        पक्ष -E2BIG:
                प्रत्यागम false;
        पक्ष 0:
                प्रत्यागम true;
        स्वभाव:
                प्रत्यागम r;
        }
}

पूर्णाङ्क machine_चिह्नकानि_from_string(ध्रुव वर्ण *s, bool graceful, वर्ण ***ret) {
        प्रतिज्ञा(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        यदि (isempty(s)) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        _cleanup_strv_free_ वर्ण **l = strv_विभज(s, ":");
        यदि (!l)
                प्रत्यागम -ENOMEM;

        प्रत्यागम machine_चिह्नकानि_from_strv(l, graceful, ret);
}

पूर्णाङ्क machine_चिह्नकानि_from_strv(वर्ण **l, bool graceful, वर्ण ***ret) {
        पूर्णाङ्क r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = रिक्त;
        STRV_FOREACH(i, l) {
                यदि (!machine_चिह्नकम्_is_valid(*i)) {
                        यदि (graceful)
                                अनुवर्त;

                        प्रत्यागम -EINVAL;
                }

                यदि (hashmap_आकार_2(h) >= MACHINE_चिह्नकानि_MAX)
                        प्रत्यागम -E2BIG;

                ध्रुव वर्ण *eq = strchrnul(*i, '=');
                _cleanup_free_ वर्ण *k = strndup(*i, (eq - *i) + (*eq == '='));
                यदि (!k)
                        प्रत्यागम -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                यदि (r == -EEXIST) {
                        यदि (streq(hashmap_get(h, k), *i))
                                अनुवर्त;
                        यदि (!graceful)
                                प्रत्यागम -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } अन्यथा यदि (r < 0)
                        प्रत्यागम r;
                अन्यथा यदि (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        यदि (ret) {
                _cleanup_strv_free_ वर्ण **cleaned = रिक्त;
                size_t n = 0;
                वर्ण *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_आकार(&cleaned, &n, v);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                strv_क्रमय(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        प्रत्यागम 0;
}
