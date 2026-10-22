/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-file.h"
#অন্তর্ভাব "hashmap.h"
#অন্তর্ভাব "hostname-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "os-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "user-util.h"

বর্ণ* get_default_hostname_raw(শূন্য) {
        পূর্ণাঙ্ক r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ধ্রুব বর্ণ *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        যদি (e) {
                যদি (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        প্রত্যাগম strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ বর্ণ *f = রিক্ত;
        r = parse_os_release(রিক্ত, "DEFAULT_HOSTNAME", &f);
        যদি (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        অন্যথা যদি (f) {
                যদি (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        প্রত্যাগম TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        প্রত্যাগম strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(বর্ণ c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        প্রত্যাগম ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ধ্রুব বর্ণ *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ধ্রুব বর্ণ *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        যদি (isempty(s))
                প্রত্যাগম false;

        যদি (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                প্রত্যাগম FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        প্রতি (p = s, dot = hyphen = true; *p; p++)
                যদি (*p == '.') {
                        যদি (dot || hyphen)
                                প্রত্যাগম false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } অন্যথা যদি (*p == '-') {
                        যদি (dot)
                                প্রত্যাগম false;

                        dot = false;
                        hyphen = true;

                } অন্যথা {
                        যদি (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                প্রত্যাগম false;

                        dot = false;
                        hyphen = false;
                }

        যদি (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                প্রত্যাগম false;
        যদি (hyphen)
                প্রত্যাগম false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        যদি (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                প্রত্যাগম false;

        প্রত্যাগম true;
}

বর্ণ* hostname_cleanup(বর্ণ *s) {
        বর্ণ *p, *d;
        bool dot, hyphen;

        প্রতিজ্ঞা(s);

        প্রতি (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                যদি (*p == '.') {
                        যদি (dot || hyphen)
                                অনুবর্ত;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } অন্যথা যদি (*p == '-') {
                        যদি (dot)
                                অনুবর্ত;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } অন্যথা যদি (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        যাবত্ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        প্রত্যাগম s;
}

bool is_localhost(ধ্রুব বর্ণ *hostname) {
        প্রতিজ্ঞা(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        প্রত্যাগম STRCASE_IN_SET(
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

ধ্রুব বর্ণ* etc_hostname(শূন্য) {
        স্থির ধ্রুব বর্ণ *cached = রিক্ত;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        প্রত্যাগম cached;
}

ধ্রুব বর্ণ* etc_machine_info(শূন্য) {
        স্থির ধ্রুব বর্ণ *cached = রিক্ত;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        প্রত্যাগম cached;
}

পূর্ণাঙ্ক get_pretty_hostname(বর্ণ **ret) {
        _cleanup_free_ বর্ণ *n = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = parse_env_file(রিক্ত, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (isempty(n))
                প্রত্যাগম -ENXIO;

        *ret = TAKE_PTR(n);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক split_user_at_host(ধ্রুব বর্ণ *s, বর্ণ **ret_user, বর্ণ **ret_host) {
        _cleanup_free_ বর্ণ *u = রিক্ত, *h = রিক্ত;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        প্রতিজ্ঞা(s);

        ধ্রুব বর্ণ *rhs = strchr(s, '@');
        যদি (rhs) {
                যদি (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        যদি (!u)
                                প্রত্যাগম -ENOMEM;
                }

                যদি (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        যদি (!h)
                                প্রত্যাগম -ENOMEM;
                }

        } অন্যথা {
                যদি (isempty(s))
                        প্রত্যাগম -EINVAL;

                যদি (ret_host) {
                        h = strdup(s);
                        যদি (!h)
                                প্রত্যাগম -ENOMEM;
                }
        }

        যদি (ret_user)
                *ret_user = TAKE_PTR(u);
        যদি (ret_host)
                *ret_host = TAKE_PTR(h);

        প্রত্যাগম !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

পূর্ণাঙ্ক machine_spec_valid(ধ্রুব বর্ণ *s) {
        _cleanup_free_ বর্ণ *u = রিক্ত, *h = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = split_user_at_host(s, &u, &h);
        যদি (r == -EINVAL)
                প্রত্যাগম false;
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                প্রত্যাগম false;

        যদি (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool machine_tag_is_valid(ধ্রুব বর্ণ *s) {
        size_t n = strlen_ptr(s);
        যদি (n <= 0 || n >= 256)
                প্রত্যাগম false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        যদি (strchr("-.=", s[0]))
                প্রত্যাগম false;

        /* We allow parameterization of tags, with a "=" as separator */
        ধ্রুব বর্ণ *eq = strchr(s, '=');
        যদি (eq) {
                প্রতিজ্ঞা(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                যদি (strchr("-.", eq[-1]))
                        প্রত্যাগম false;
        } অন্যথা {
                /* If there's no '=', then make the restriction on the very last character */
                যদি (strchr("-.", s[n-1]))
                        প্রত্যাগম false;
        }

        প্রত্যাগম in_charset(s, ALPHANUMERICAL "-.=");
}

পূর্ণাঙ্ক machine_tag_list_is_valid(বর্ণ **l) {
        পূর্ণাঙ্ক r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ রিক্ত);
        বিকল্প (r) {
        পক্ষ -EINVAL:
        পক্ষ -E2BIG:
                প্রত্যাগম false;
        পক্ষ 0:
                প্রত্যাগম true;
        স্বভাব:
                প্রত্যাগম r;
        }
}

পূর্ণাঙ্ক machine_tags_from_string(ধ্রুব বর্ণ *s, bool graceful, বর্ণ ***ret) {
        প্রতিজ্ঞা(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        যদি (isempty(s)) {
                *ret = রিক্ত;
                প্রত্যাগম 0;
        }

        _cleanup_strv_free_ বর্ণ **l = strv_split(s, ":");
        যদি (!l)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম machine_tags_from_strv(l, graceful, ret);
}

পূর্ণাঙ্ক machine_tags_from_strv(বর্ণ **l, bool graceful, বর্ণ ***ret) {
        পূর্ণাঙ্ক r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = রিক্ত;
        STRV_FOREACH(i, l) {
                যদি (!machine_tag_is_valid(*i)) {
                        যদি (graceful)
                                অনুবর্ত;

                        প্রত্যাগম -EINVAL;
                }

                যদি (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        প্রত্যাগম -E2BIG;

                ধ্রুব বর্ণ *eq = strchrnul(*i, '=');
                _cleanup_free_ বর্ণ *k = strndup(*i, (eq - *i) + (*eq == '='));
                যদি (!k)
                        প্রত্যাগম -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                যদি (r == -EEXIST) {
                        যদি (streq(hashmap_get(h, k), *i))
                                অনুবর্ত;
                        যদি (!graceful)
                                প্রত্যাগম -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } অন্যথা যদি (r < 0)
                        প্রত্যাগম r;
                অন্যথা যদি (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        যদি (ret) {
                _cleanup_strv_free_ বর্ণ **cleaned = রিক্ত;
                size_t n = 0;
                বর্ণ *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        প্রত্যাগম 0;
}
