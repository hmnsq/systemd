/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "env-file.h"
#समावेश "hashmap.h"
#समावेश "hostname-util.h"
#समावेश "log.h"
#समावेश "os-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "user-util.h"

अक्षर* get_default_hostname_raw(व्योम) {
        पूर्णांक r;

        /* Returns the default hostname, and leaves any ??? in place. */

        स्थिर अक्षर *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        अगर (e) {
                अगर (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        वापस strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ अक्षर *f = शून्य;
        r = parse_os_release(शून्य, "DEFAULT_HOSTNAME", &f);
        अगर (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        अन्यथा अगर (f) {
                अगर (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        वापस TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        वापस strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(अक्षर c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        वापस ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(स्थिर अक्षर *s, ValidHostnameFlags flags) {
        अचिन्हित n_dots = 0;
        स्थिर अक्षर *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        अगर (isempty(s))
                वापस false;

        अगर (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                वापस FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        क्रम (p = s, dot = hyphen = true; *p; p++)
                अगर (*p == '.') {
                        अगर (dot || hyphen)
                                वापस false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } अन्यथा अगर (*p == '-') {
                        अगर (dot)
                                वापस false;

                        dot = false;
                        hyphen = true;

                } अन्यथा {
                        अगर (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                वापस false;

                        dot = false;
                        hyphen = false;
                }

        अगर (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                वापस false;
        अगर (hyphen)
                वापस false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        अगर (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                वापस false;

        वापस true;
}

अक्षर* hostname_cleanup(अक्षर *s) {
        अक्षर *p, *d;
        bool dot, hyphen;

        निश्चित(s);

        क्रम (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                अगर (*p == '.') {
                        अगर (dot || hyphen)
                                जारी;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } अन्यथा अगर (*p == '-') {
                        अगर (dot)
                                जारी;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } अन्यथा अगर (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        जबतक (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        वापस s;
}

bool is_localhost(स्थिर अक्षर *hostname) {
        निश्चित(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        वापस STRCASE_IN_SET(
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

स्थिर अक्षर* etc_hostname(व्योम) {
        अटल स्थिर अक्षर *cached = शून्य;

        अगर (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        वापस cached;
}

स्थिर अक्षर* etc_machine_info(व्योम) {
        अटल स्थिर अक्षर *cached = शून्य;

        अगर (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        वापस cached;
}

पूर्णांक get_pretty_hostname(अक्षर **ret) {
        _cleanup_free_ अक्षर *n = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = parse_env_file(शून्य, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        अगर (r < 0)
                वापस r;

        अगर (isempty(n))
                वापस -ENXIO;

        *ret = TAKE_PTR(n);
        वापस 0;
}

पूर्णांक split_user_at_host(स्थिर अक्षर *s, अक्षर **ret_user, अक्षर **ret_host) {
        _cleanup_free_ अक्षर *u = शून्य, *h = शून्य;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        निश्चित(s);

        स्थिर अक्षर *rhs = म_अक्षर(s, '@');
        अगर (rhs) {
                अगर (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        अगर (!u)
                                वापस -ENOMEM;
                }

                अगर (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        अगर (!h)
                                वापस -ENOMEM;
                }

        } अन्यथा {
                अगर (isempty(s))
                        वापस -EINVAL;

                अगर (ret_host) {
                        h = strdup(s);
                        अगर (!h)
                                वापस -ENOMEM;
                }
        }

        अगर (ret_user)
                *ret_user = TAKE_PTR(u);
        अगर (ret_host)
                *ret_host = TAKE_PTR(h);

        वापस !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

पूर्णांक machine_spec_valid(स्थिर अक्षर *s) {
        _cleanup_free_ अक्षर *u = शून्य, *h = शून्य;
        पूर्णांक r;

        निश्चित(s);

        r = split_user_at_host(s, &u, &h);
        अगर (r == -EINVAL)
                वापस false;
        अगर (r < 0)
                वापस r;

        अगर (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                वापस false;

        अगर (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                वापस false;

        वापस true;
}

bool machine_tag_is_valid(स्थिर अक्षर *s) {
        माप_प्रकार n = strlen_ptr(s);
        अगर (n <= 0 || n >= 256)
                वापस false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        अगर (म_अक्षर("-.=", s[0]))
                वापस false;

        /* We allow parameterization of tags, with a "=" as separator */
        स्थिर अक्षर *eq = म_अक्षर(s, '=');
        अगर (eq) {
                निश्चित(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                अगर (म_अक्षर("-.", eq[-1]))
                        वापस false;
        } अन्यथा {
                /* If there's no '=', then make the restriction on the very last character */
                अगर (म_अक्षर("-.", s[n-1]))
                        वापस false;
        }

        वापस in_charset(s, ALPHANUMERICAL "-.=");
}

पूर्णांक machine_tag_list_is_valid(अक्षर **l) {
        पूर्णांक r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ शून्य);
        चयन (r) {
        हाल -EINVAL:
        हाल -E2BIG:
                वापस false;
        हाल 0:
                वापस true;
        शेष:
                वापस r;
        }
}

पूर्णांक machine_tags_from_string(स्थिर अक्षर *s, bool graceful, अक्षर ***ret) {
        निश्चित(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        अगर (isempty(s)) {
                *ret = शून्य;
                वापस 0;
        }

        _cleanup_strv_free_ अक्षर **l = strv_split(s, ":");
        अगर (!l)
                वापस -ENOMEM;

        वापस machine_tags_from_strv(l, graceful, ret);
}

पूर्णांक machine_tags_from_strv(अक्षर **l, bool graceful, अक्षर ***ret) {
        पूर्णांक r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = शून्य;
        STRV_FOREACH(i, l) {
                अगर (!machine_tag_is_valid(*i)) {
                        अगर (graceful)
                                जारी;

                        वापस -EINVAL;
                }

                अगर (hashmap_आकार_2(h) >= MACHINE_TAGS_MAX)
                        वापस -E2BIG;

                स्थिर अक्षर *eq = strchrnul(*i, '=');
                _cleanup_free_ अक्षर *k = strndup(*i, (eq - *i) + (*eq == '='));
                अगर (!k)
                        वापस -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                अगर (r == -EEXIST) {
                        अगर (streq(hashmap_get(h, k), *i))
                                जारी;
                        अगर (!graceful)
                                वापस -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } अन्यथा अगर (r < 0)
                        वापस r;
                अन्यथा अगर (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        अगर (ret) {
                _cleanup_strv_free_ अक्षर **cleaned = शून्य;
                माप_प्रकार n = 0;
                अक्षर *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_आकार(&cleaned, &n, v);
                        अगर (r < 0)
                                वापस r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        वापस 0;
}
