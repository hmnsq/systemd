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

        अचर वर्ण *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        यदि (e) {
                यदि (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        फर्काऊ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ वर्ण *f = रिक्त;
        r = parse_os_release(रिक्त, "DEFAULT_HOSTNAME", &f);
        यदि (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        अन्यथा यदि (f) {
                यदि (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        फर्काऊ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        फर्काऊ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(वर्ण c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        फर्काऊ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(अचर वर्ण *s, ValidHostnameFlags flags) {
        अचिन्हित n_dots = 0;
        अचर वर्ण *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        यदि (isempty(s))
                फर्काऊ false;

        यदि (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                फर्काऊ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        लागि (p = s, dot = hyphen = true; *p; p++)
                यदि (*p == '.') {
                        यदि (dot || hyphen)
                                फर्काऊ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } अन्यथा यदि (*p == '-') {
                        यदि (dot)
                                फर्काऊ false;

                        dot = false;
                        hyphen = true;

                } अन्यथा {
                        यदि (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                फर्काऊ false;

                        dot = false;
                        hyphen = false;
                }

        यदि (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                फर्काऊ false;
        यदि (hyphen)
                फर्काऊ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        यदि (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                फर्काऊ false;

        फर्काऊ true;
}

वर्ण* hostname_cleanup(वर्ण *s) {
        वर्ण *p, *d;
        bool dot, hyphen;

        प्रतिज्ञा(s);

        लागि (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                यदि (*p == '.') {
                        यदि (dot || hyphen)
                                जारी;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } अन्यथा यदि (*p == '-') {
                        यदि (dot)
                                जारी;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } अन्यथा यदि (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        जबसम्म (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        फर्काऊ s;
}

bool is_localhost(अचर वर्ण *hostname) {
        प्रतिज्ञा(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        फर्काऊ STRCASE_IN_SET(
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

अचर वर्ण* etc_hostname(शून्य) {
        स्थिर अचर वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        फर्काऊ cached;
}

अचर वर्ण* etc_machine_info(शून्य) {
        स्थिर अचर वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        फर्काऊ cached;
}

पूर्णाङ्क get_pretty_hostname(वर्ण **ret) {
        _cleanup_free_ वर्ण *n = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = parse_env_file(रिक्त, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        यदि (r < 0)
                फर्काऊ r;

        यदि (isempty(n))
                फर्काऊ -ENXIO;

        *ret = TAKE_PTR(n);
        फर्काऊ 0;
}

पूर्णाङ्क split_user_at_host(अचर वर्ण *s, वर्ण **ret_user, वर्ण **ret_host) {
        _cleanup_free_ वर्ण *u = रिक्त, *h = रिक्त;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        प्रतिज्ञा(s);

        अचर वर्ण *rhs = म_अक्षर(s, '@');
        यदि (rhs) {
                यदि (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        यदि (!u)
                                फर्काऊ -ENOMEM;
                }

                यदि (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        यदि (!h)
                                फर्काऊ -ENOMEM;
                }

        } अन्यथा {
                यदि (isempty(s))
                        फर्काऊ -EINVAL;

                यदि (ret_host) {
                        h = strdup(s);
                        यदि (!h)
                                फर्काऊ -ENOMEM;
                }
        }

        यदि (ret_user)
                *ret_user = TAKE_PTR(u);
        यदि (ret_host)
                *ret_host = TAKE_PTR(h);

        फर्काऊ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

पूर्णाङ्क machine_spec_valid(अचर वर्ण *s) {
        _cleanup_free_ वर्ण *u = रिक्त, *h = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = split_user_at_host(s, &u, &h);
        यदि (r == -EINVAL)
                फर्काऊ false;
        यदि (r < 0)
                फर्काऊ r;

        यदि (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                फर्काऊ false;

        यदि (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                फर्काऊ false;

        फर्काऊ true;
}

bool machine_tag_is_valid(अचर वर्ण *s) {
        माप_प्रकार n = strlen_ptr(s);
        यदि (n <= 0 || n >= 256)
                फर्काऊ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        यदि (म_अक्षर("-.=", s[0]))
                फर्काऊ false;

        /* We allow parameterization of tags, with a "=" as separator */
        अचर वर्ण *eq = म_अक्षर(s, '=');
        यदि (eq) {
                प्रतिज्ञा(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                यदि (म_अक्षर("-.", eq[-1]))
                        फर्काऊ false;
        } अन्यथा {
                /* If there's no '=', then make the restriction on the very last character */
                यदि (म_अक्षर("-.", s[n-1]))
                        फर्काऊ false;
        }

        फर्काऊ in_charset(s, ALPHANUMERICAL "-.=");
}

पूर्णाङ्क machine_tag_list_is_valid(वर्ण **l) {
        पूर्णाङ्क r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ रिक्त);
        विकल्प (r) {
        पक्ष -EINVAL:
        पक्ष -E2BIG:
                फर्काऊ false;
        पक्ष 0:
                फर्काऊ true;
        स्वभाव:
                फर्काऊ r;
        }
}

पूर्णाङ्क machine_tags_from_string(अचर वर्ण *s, bool graceful, वर्ण ***ret) {
        प्रतिज्ञा(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        यदि (isempty(s)) {
                *ret = रिक्त;
                फर्काऊ 0;
        }

        _cleanup_strv_free_ वर्ण **l = strv_split(s, ":");
        यदि (!l)
                फर्काऊ -ENOMEM;

        फर्काऊ machine_tags_from_strv(l, graceful, ret);
}

पूर्णाङ्क machine_tags_from_strv(वर्ण **l, bool graceful, वर्ण ***ret) {
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
                यदि (!machine_tag_is_valid(*i)) {
                        यदि (graceful)
                                जारी;

                        फर्काऊ -EINVAL;
                }

                यदि (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        फर्काऊ -E2BIG;

                अचर वर्ण *eq = strchrnul(*i, '=');
                _cleanup_free_ वर्ण *k = strndup(*i, (eq - *i) + (*eq == '='));
                यदि (!k)
                        फर्काऊ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                यदि (r == -EEXIST) {
                        यदि (streq(hashmap_get(h, k), *i))
                                जारी;
                        यदि (!graceful)
                                फर्काऊ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } अन्यथा यदि (r < 0)
                        फर्काऊ r;
                अन्यथा यदि (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        यदि (ret) {
                _cleanup_strv_free_ वर्ण **cleaned = रिक्त;
                माप_प्रकार n = 0;
                वर्ण *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        यदि (r < 0)
                                फर्काऊ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        फर्काऊ 0;
}
