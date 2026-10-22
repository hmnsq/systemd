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

অক্ষর* get_default_hostname_raw(শূন্য) {
        পূর্ণসংখ্যা r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        যদি (e) {
                যদি (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ফেরত strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ অক্ষর *f = ফাঁকা;
        r = parse_os_release(ফাঁকা, "DEFAULT_HOSTNAME", &f);
        যদি (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        অন্যথা যদি (f) {
                যদি (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ফেরত TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ফেরত strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(অক্ষর c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ফেরত ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ধ্রুবক অক্ষর *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ধ্রুবক অক্ষর *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        যদি (isempty(s))
                ফেরত false;

        যদি (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ফেরত FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        জন্য (p = s, dot = hyphen = true; *p; p++)
                যদি (*p == '.') {
                        যদি (dot || hyphen)
                                ফেরত false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } অন্যথা যদি (*p == '-') {
                        যদি (dot)
                                ফেরত false;

                        dot = false;
                        hyphen = true;

                } অন্যথা {
                        যদি (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ফেরত false;

                        dot = false;
                        hyphen = false;
                }

        যদি (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ফেরত false;
        যদি (hyphen)
                ফেরত false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        যদি (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ফেরত false;

        ফেরত true;
}

অক্ষর* hostname_cleanup(অক্ষর *s) {
        অক্ষর *p, *d;
        bool dot, hyphen;

        প্রতিজ্ঞা(s);

        জন্য (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                যদি (*p == '.') {
                        যদি (dot || hyphen)
                                চালু;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } অন্যথা যদি (*p == '-') {
                        যদি (dot)
                                চালু;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } অন্যথা যদি (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        যখন (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ফেরত s;
}

bool is_localhost(ধ্রুবক অক্ষর *hostname) {
        প্রতিজ্ঞা(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ফেরত STRCASE_IN_SET(
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

ধ্রুবক অক্ষর* etc_hostname(শূন্য) {
        স্থির ধ্রুবক অক্ষর *cached = ফাঁকা;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ফেরত cached;
}

ধ্রুবক অক্ষর* etc_machine_info(শূন্য) {
        স্থির ধ্রুবক অক্ষর *cached = ফাঁকা;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ফেরত cached;
}

পূর্ণসংখ্যা get_pretty_hostname(অক্ষর **ret) {
        _cleanup_free_ অক্ষর *n = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = parse_env_file(ফাঁকা, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        যদি (r < 0)
                ফেরত r;

        যদি (isempty(n))
                ফেরত -ENXIO;

        *ret = TAKE_PTR(n);
        ফেরত 0;
}

পূর্ণসংখ্যা split_user_at_host(ধ্রুবক অক্ষর *s, অক্ষর **ret_user, অক্ষর **ret_host) {
        _cleanup_free_ অক্ষর *u = ফাঁকা, *h = ফাঁকা;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        প্রতিজ্ঞা(s);

        ধ্রুবক অক্ষর *rhs = strchr(s, '@');
        যদি (rhs) {
                যদি (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        যদি (!u)
                                ফেরত -ENOMEM;
                }

                যদি (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        যদি (!h)
                                ফেরত -ENOMEM;
                }

        } অন্যথা {
                যদি (isempty(s))
                        ফেরত -EINVAL;

                যদি (ret_host) {
                        h = strdup(s);
                        যদি (!h)
                                ফেরত -ENOMEM;
                }
        }

        যদি (ret_user)
                *ret_user = TAKE_PTR(u);
        যদি (ret_host)
                *ret_host = TAKE_PTR(h);

        ফেরত !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

পূর্ণসংখ্যা machine_spec_valid(ধ্রুবক অক্ষর *s) {
        _cleanup_free_ অক্ষর *u = ফাঁকা, *h = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = split_user_at_host(s, &u, &h);
        যদি (r == -EINVAL)
                ফেরত false;
        যদি (r < 0)
                ফেরত r;

        যদি (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ফেরত false;

        যদি (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ফেরত false;

        ফেরত true;
}

bool machine_tag_is_valid(ধ্রুবক অক্ষর *s) {
        size_t n = strlen_ptr(s);
        যদি (n <= 0 || n >= 256)
                ফেরত false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        যদি (strchr("-.=", s[0]))
                ফেরত false;

        /* We allow parameterization of tags, with a "=" as separator */
        ধ্রুবক অক্ষর *eq = strchr(s, '=');
        যদি (eq) {
                প্রতিজ্ঞা(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                যদি (strchr("-.", eq[-1]))
                        ফেরত false;
        } অন্যথা {
                /* If there's no '=', then make the restriction on the very last character */
                যদি (strchr("-.", s[n-1]))
                        ফেরত false;
        }

        ফেরত in_charset(s, ALPHANUMERICAL "-.=");
}

পূর্ণসংখ্যা machine_tag_list_is_valid(অক্ষর **l) {
        পূর্ণসংখ্যা r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ ফাঁকা);
        নির্বাচন (r) {
        ক্ষেত্র -EINVAL:
        ক্ষেত্র -E2BIG:
                ফেরত false;
        ক্ষেত্র 0:
                ফেরত true;
        পূর্বনির্ধারিত:
                ফেরত r;
        }
}

পূর্ণসংখ্যা machine_tags_from_string(ধ্রুবক অক্ষর *s, bool graceful, অক্ষর ***ret) {
        প্রতিজ্ঞা(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        যদি (isempty(s)) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        _cleanup_strv_free_ অক্ষর **l = strv_split(s, ":");
        যদি (!l)
                ফেরত -ENOMEM;

        ফেরত machine_tags_from_strv(l, graceful, ret);
}

পূর্ণসংখ্যা machine_tags_from_strv(অক্ষর **l, bool graceful, অক্ষর ***ret) {
        পূর্ণসংখ্যা r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ফাঁকা;
        STRV_FOREACH(i, l) {
                যদি (!machine_tag_is_valid(*i)) {
                        যদি (graceful)
                                চালু;

                        ফেরত -EINVAL;
                }

                যদি (hashmap_আকার_2(h) >= MACHINE_TAGS_MAX)
                        ফেরত -E2BIG;

                ধ্রুবক অক্ষর *eq = strchrnul(*i, '=');
                _cleanup_free_ অক্ষর *k = strndup(*i, (eq - *i) + (*eq == '='));
                যদি (!k)
                        ফেরত -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                যদি (r == -EEXIST) {
                        যদি (streq(hashmap_get(h, k), *i))
                                চালু;
                        যদি (!graceful)
                                ফেরত -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } অন্যথা যদি (r < 0)
                        ফেরত r;
                অন্যথা যদি (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        যদি (ret) {
                _cleanup_strv_free_ অক্ষর **cleaned = ফাঁকা;
                size_t n = 0;
                অক্ষর *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_আকার(&cleaned, &n, v);
                        যদি (r < 0)
                                ফেরত r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ফেরত 0;
}
