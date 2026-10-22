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

अक्षर* get_default_hostname_raw(शून्य) {
        पूर्णांक r;

        /* Returns the default hostname, and leaves any ??? in place. */

        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        जर (e) {
                जर (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        परत strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ अक्षर *f = रिक्त;
        r = parse_os_release(रिक्त, "DEFAULT_HOSTNAME", &f);
        जर (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        अन्यथा जर (f) {
                जर (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        परत TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        परत strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(अक्षर c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        परत ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(स्थिरांक अक्षर *s, ValidHostnameFlags flags) {
        अचिन्हित n_dots = 0;
        स्थिरांक अक्षर *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        जर (isempty(s))
                परत false;

        जर (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                परत FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        साठी (p = s, dot = hyphen = true; *p; p++)
                जर (*p == '.') {
                        जर (dot || hyphen)
                                परत false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } अन्यथा जर (*p == '-') {
                        जर (dot)
                                परत false;

                        dot = false;
                        hyphen = true;

                } अन्यथा {
                        जर (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                परत false;

                        dot = false;
                        hyphen = false;
                }

        जर (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                परत false;
        जर (hyphen)
                परत false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        जर (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                परत false;

        परत true;
}

अक्षर* hostname_cleanup(अक्षर *s) {
        अक्षर *p, *d;
        bool dot, hyphen;

        प्रतिज्ञा(s);

        साठी (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                जर (*p == '.') {
                        जर (dot || hyphen)
                                सुरूठेवा;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } अन्यथा जर (*p == '-') {
                        जर (dot)
                                सुरूठेवा;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } अन्यथा जर (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        पर्यंत (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        परत s;
}

bool is_localhost(स्थिरांक अक्षर *hostname) {
        प्रतिज्ञा(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        परत STRCASE_IN_SET(
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

स्थिरांक अक्षर* etc_hostname(शून्य) {
        स्थिर स्थिरांक अक्षर *cached = रिक्त;

        जर (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        परत cached;
}

स्थिरांक अक्षर* etc_machine_info(शून्य) {
        स्थिर स्थिरांक अक्षर *cached = रिक्त;

        जर (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        परत cached;
}

पूर्णांक get_pretty_hostname(अक्षर **ret) {
        _cleanup_free_ अक्षर *n = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = parse_env_file(रिक्त, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        जर (r < 0)
                परत r;

        जर (isempty(n))
                परत -ENXIO;

        *ret = TAKE_PTR(n);
        परत 0;
}

पूर्णांक split_user_at_host(स्थिरांक अक्षर *s, अक्षर **ret_user, अक्षर **ret_host) {
        _cleanup_free_ अक्षर *u = रिक्त, *h = रिक्त;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        प्रतिज्ञा(s);

        स्थिरांक अक्षर *rhs = म_अक्षर(s, '@');
        जर (rhs) {
                जर (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        जर (!u)
                                परत -ENOMEM;
                }

                जर (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        जर (!h)
                                परत -ENOMEM;
                }

        } अन्यथा {
                जर (isempty(s))
                        परत -EINVAL;

                जर (ret_host) {
                        h = strdup(s);
                        जर (!h)
                                परत -ENOMEM;
                }
        }

        जर (ret_user)
                *ret_user = TAKE_PTR(u);
        जर (ret_host)
                *ret_host = TAKE_PTR(h);

        परत !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

पूर्णांक machine_spec_valid(स्थिरांक अक्षर *s) {
        _cleanup_free_ अक्षर *u = रिक्त, *h = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = split_user_at_host(s, &u, &h);
        जर (r == -EINVAL)
                परत false;
        जर (r < 0)
                परत r;

        जर (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                परत false;

        जर (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                परत false;

        परत true;
}

bool machine_tag_is_valid(स्थिरांक अक्षर *s) {
        माप_प्रकार n = strlen_ptr(s);
        जर (n <= 0 || n >= 256)
                परत false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        जर (म_अक्षर("-.=", s[0]))
                परत false;

        /* We allow parameterization of tags, with a "=" as separator */
        स्थिरांक अक्षर *eq = म_अक्षर(s, '=');
        जर (eq) {
                प्रतिज्ञा(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                जर (म_अक्षर("-.", eq[-1]))
                        परत false;
        } अन्यथा {
                /* If there's no '=', then make the restriction on the very last character */
                जर (म_अक्षर("-.", s[n-1]))
                        परत false;
        }

        परत in_charset(s, ALPHANUMERICAL "-.=");
}

पूर्णांक machine_tag_list_is_valid(अक्षर **l) {
        पूर्णांक r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ रिक्त);
        निवड (r) {
        स्थिती -EINVAL:
        स्थिती -E2BIG:
                परत false;
        स्थिती 0:
                परत true;
        पूर्वनिर्धारित:
                परत r;
        }
}

पूर्णांक machine_tags_from_string(स्थिरांक अक्षर *s, bool graceful, अक्षर ***ret) {
        प्रतिज्ञा(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        जर (isempty(s)) {
                *ret = रिक्त;
                परत 0;
        }

        _cleanup_strv_free_ अक्षर **l = strv_split(s, ":");
        जर (!l)
                परत -ENOMEM;

        परत machine_tags_from_strv(l, graceful, ret);
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
        _cleanup_hashmap_free_ Hashmap *h = रिक्त;
        STRV_FOREACH(i, l) {
                जर (!machine_tag_is_valid(*i)) {
                        जर (graceful)
                                सुरूठेवा;

                        परत -EINVAL;
                }

                जर (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        परत -E2BIG;

                स्थिरांक अक्षर *eq = strchrnul(*i, '=');
                _cleanup_free_ अक्षर *k = strndup(*i, (eq - *i) + (*eq == '='));
                जर (!k)
                        परत -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                जर (r == -EEXIST) {
                        जर (streq(hashmap_get(h, k), *i))
                                सुरूठेवा;
                        जर (!graceful)
                                परत -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } अन्यथा जर (r < 0)
                        परत r;
                अन्यथा जर (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        जर (ret) {
                _cleanup_strv_free_ अक्षर **cleaned = रिक्त;
                माप_प्रकार n = 0;
                अक्षर *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        जर (r < 0)
                                परत r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        परत 0;
}
