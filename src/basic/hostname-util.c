/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-file.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "hostname-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "os-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ଅକ୍ଷର* get_default_hostname_raw(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ଯଦି (e) {
                ଯଦି (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ଫେରନ୍ତୁ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ ଅକ୍ଷର *f = ଖାଲି;
        r = parse_os_release(ଖାଲି, "DEFAULT_HOSTNAME", &f);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ଅନ୍ୟଥା ଯଦି (f) {
                ଯଦି (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ଫେରନ୍ତୁ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ଫେରନ୍ତୁ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(ଅକ୍ଷର c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ଫେରନ୍ତୁ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ଅଚଳ ଅକ୍ଷର *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ଅଚଳ ଅକ୍ଷର *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ଯଦି (isempty(s))
                ଫେରନ୍ତୁ false;

        ଯଦି (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ଫେରନ୍ତୁ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        ପାଇଁ (p = s, dot = hyphen = true; *p; p++)
                ଯଦି (*p == '.') {
                        ଯଦି (dot || hyphen)
                                ଫେରନ୍ତୁ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ଅନ୍ୟଥା ଯଦି (*p == '-') {
                        ଯଦି (dot)
                                ଫେରନ୍ତୁ false;

                        dot = false;
                        hyphen = true;

                } ଅନ୍ୟଥା {
                        ଯଦି (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ଫେରନ୍ତୁ false;

                        dot = false;
                        hyphen = false;
                }

        ଯଦି (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ଫେରନ୍ତୁ false;
        ଯଦି (hyphen)
                ଫେରନ୍ତୁ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ଯଦି (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ଅକ୍ଷର* hostname_cleanup(ଅକ୍ଷର *s) {
        ଅକ୍ଷର *p, *d;
        bool dot, hyphen;

        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ଯଦି (*p == '.') {
                        ଯଦି (dot || hyphen)
                                ଜାରି;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ଅନ୍ୟଥା ଯଦି (*p == '-') {
                        ଯଦି (dot)
                                ଜାରି;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ଅନ୍ୟଥା ଯଦି (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ଯେତେବେଳେ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ଫେରନ୍ତୁ s;
}

bool is_localhost(ଅଚଳ ଅକ୍ଷର *hostname) {
        ପ୍ରତିଜ୍ଞା(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ଫେରନ୍ତୁ STRCASE_IN_SET(
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

ଅଚଳ ଅକ୍ଷର* etc_hostname(ଶୂନ୍ୟ) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *cached = ଖାଲି;

        ଯଦି (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ଫେରନ୍ତୁ cached;
}

ଅଚଳ ଅକ୍ଷର* etc_machine_info(ଶୂନ୍ୟ) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *cached = ଖାଲି;

        ଯଦି (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ଫେରନ୍ତୁ cached;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_pretty_hostname(ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_env_file(ଖାଲି, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (isempty(n))
                ଫେରନ୍ତୁ -ENXIO;

        *ret = TAKE_PTR(n);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା split_user_at_host(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର **ret_user, ଅକ୍ଷର **ret_host) {
        _cleanup_free_ ଅକ୍ଷର *u = ଖାଲି, *h = ଖାଲି;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ପ୍ରତିଜ୍ଞା(s);

        ଅଚଳ ଅକ୍ଷର *rhs = strchr(s, '@');
        ଯଦି (rhs) {
                ଯଦି (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ଯଦି (!u)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

                ଯଦି (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ଯଦି (!h)
                                ଫେରନ୍ତୁ -ENOMEM;
                }

        } ଅନ୍ୟଥା {
                ଯଦି (isempty(s))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (ret_host) {
                        h = strdup(s);
                        ଯଦି (!h)
                                ଫେରନ୍ତୁ -ENOMEM;
                }
        }

        ଯଦି (ret_user)
                *ret_user = TAKE_PTR(u);
        ଯଦି (ret_host)
                *ret_host = TAKE_PTR(h);

        ଫେରନ୍ତୁ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_spec_valid(ଅଚଳ ଅକ୍ଷର *s) {
        _cleanup_free_ ଅକ୍ଷର *u = ଖାଲି, *h = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = split_user_at_host(s, &u, &h);
        ଯଦି (r == -EINVAL)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ଫେରନ୍ତୁ false;

        ଯଦି (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool machine_tag_is_valid(ଅଚଳ ଅକ୍ଷର *s) {
        size_t n = strlen_ptr(s);
        ଯଦି (n <= 0 || n >= 256)
                ଫେରନ୍ତୁ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ଯଦି (strchr("-.=", s[0]))
                ଫେରନ୍ତୁ false;

        /* We allow parameterization of tags, with a "=" as separator */
        ଅଚଳ ଅକ୍ଷର *eq = strchr(s, '=');
        ଯଦି (eq) {
                ପ୍ରତିଜ୍ଞା(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ଯଦି (strchr("-.", eq[-1]))
                        ଫେରନ୍ତୁ false;
        } ଅନ୍ୟଥା {
                /* If there's no '=', then make the restriction on the very last character */
                ଯଦି (strchr("-.", s[n-1]))
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ in_charset(s, ALPHANUMERICAL "-.=");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tag_list_is_valid(ଅକ୍ଷର **l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ ଖାଲି);
        ବାଛ (r) {
        ସ୍ଥିତି -EINVAL:
        ସ୍ଥିତି -E2BIG:
                ଫେରନ୍ତୁ false;
        ସ୍ଥିତି 0:
                ଫେରନ୍ତୁ true;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ r;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tags_from_string(ଅଚଳ ଅକ୍ଷର *s, bool graceful, ଅକ୍ଷର ***ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ଯଦି (isempty(s)) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_strv_free_ ଅକ୍ଷର **l = strv_split(s, ":");
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ machine_tags_from_strv(l, graceful, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tags_from_strv(ଅକ୍ଷର **l, bool graceful, ଅକ୍ଷର ***ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ଖାଲି;
        STRV_FOREACH(i, l) {
                ଯଦି (!machine_tag_is_valid(*i)) {
                        ଯଦି (graceful)
                                ଜାରି;

                        ଫେରନ୍ତୁ -EINVAL;
                }

                ଯଦି (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ଫେରନ୍ତୁ -E2BIG;

                ଅଚଳ ଅକ୍ଷର *eq = strchrnul(*i, '=');
                _cleanup_free_ ଅକ୍ଷର *k = strndup(*i, (eq - *i) + (*eq == '='));
                ଯଦି (!k)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ଯଦି (r == -EEXIST) {
                        ଯଦି (streq(hashmap_get(h, k), *i))
                                ଜାରି;
                        ଯଦି (!graceful)
                                ଫେରନ୍ତୁ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ଅନ୍ୟଥା ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଅନ୍ୟଥା ଯଦି (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ଯଦି (ret) {
                _cleanup_strv_free_ ଅକ୍ଷର **cleaned = ଖାଲି;
                size_t n = 0;
                ଅକ୍ଷର *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ଫେରନ୍ତୁ 0;
}
