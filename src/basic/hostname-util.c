/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "env-file.h"
#شامل "hashmap.h"
#شامل "hostname-util.h"
#شامل "log.h"
#شامل "os-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "user-util.h"

نویسه* get_default_hostname_raw(تهی) {
        صحیح r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ثابت نویسه *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        اگر (e) {
                اگر (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        بازگشت strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ نویسه *f = پوچ;
        r = parse_os_release(پوچ, "DEFAULT_HOSTNAME", &f);
        اگر (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        وگرنه اگر (f) {
                اگر (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        بازگشت TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        بازگشت strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(نویسه c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        بازگشت ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ثابت نویسه *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ثابت نویسه *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        اگر (isempty(s))
                بازگشت false;

        اگر (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                بازگشت FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        برای (p = s, dot = hyphen = true; *p; p++)
                اگر (*p == '.') {
                        اگر (dot || hyphen)
                                بازگشت false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } وگرنه اگر (*p == '-') {
                        اگر (dot)
                                بازگشت false;

                        dot = false;
                        hyphen = true;

                } وگرنه {
                        اگر (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                بازگشت false;

                        dot = false;
                        hyphen = false;
                }

        اگر (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                بازگشت false;
        اگر (hyphen)
                بازگشت false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        اگر (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                بازگشت false;

        بازگشت true;
}

نویسه* hostname_cleanup(نویسه *s) {
        نویسه *p, *d;
        bool dot, hyphen;

        أكد(s);

        برای (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                اگر (*p == '.') {
                        اگر (dot || hyphen)
                                ادامه;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } وگرنه اگر (*p == '-') {
                        اگر (dot)
                                ادامه;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } وگرنه اگر (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        وقتی (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        بازگشت s;
}

bool is_localhost(ثابت نویسه *hostname) {
        أكد(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        بازگشت STRCASE_IN_SET(
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

ثابت نویسه* etc_hostname(تهی) {
        ایستا ثابت نویسه *cached = پوچ;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        بازگشت cached;
}

ثابت نویسه* etc_machine_info(تهی) {
        ایستا ثابت نویسه *cached = پوچ;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        بازگشت cached;
}

صحیح get_pretty_hostname(نویسه **ret) {
        _cleanup_free_ نویسه *n = پوچ;
        صحیح r;

        أكد(ret);

        r = parse_env_file(پوچ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        اگر (r < 0)
                بازگشت r;

        اگر (isempty(n))
                بازگشت -ENXIO;

        *ret = TAKE_PTR(n);
        بازگشت 0;
}

صحیح split_user_at_host(ثابت نویسه *s, نویسه **ret_user, نویسه **ret_host) {
        _cleanup_free_ نویسه *u = پوچ, *h = پوچ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        أكد(s);

        ثابت نویسه *rhs = strchr(s, '@');
        اگر (rhs) {
                اگر (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        اگر (!u)
                                بازگشت -ENOMEM;
                }

                اگر (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        اگر (!h)
                                بازگشت -ENOMEM;
                }

        } وگرنه {
                اگر (isempty(s))
                        بازگشت -EINVAL;

                اگر (ret_host) {
                        h = strdup(s);
                        اگر (!h)
                                بازگشت -ENOMEM;
                }
        }

        اگر (ret_user)
                *ret_user = TAKE_PTR(u);
        اگر (ret_host)
                *ret_host = TAKE_PTR(h);

        بازگشت !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

صحیح machine_spec_valid(ثابت نویسه *s) {
        _cleanup_free_ نویسه *u = پوچ, *h = پوچ;
        صحیح r;

        أكد(s);

        r = split_user_at_host(s, &u, &h);
        اگر (r == -EINVAL)
                بازگشت false;
        اگر (r < 0)
                بازگشت r;

        اگر (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                بازگشت false;

        اگر (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                بازگشت false;

        بازگشت true;
}

bool machine_tag_is_valid(ثابت نویسه *s) {
        size_t n = strlen_ptr(s);
        اگر (n <= 0 || n >= 256)
                بازگشت false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        اگر (strchr("-.=", s[0]))
                بازگشت false;

        /* We allow parameterization of tags, with a "=" as separator */
        ثابت نویسه *eq = strchr(s, '=');
        اگر (eq) {
                أكد(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                اگر (strchr("-.", eq[-1]))
                        بازگشت false;
        } وگرنه {
                /* If there's no '=', then make the restriction on the very last character */
                اگر (strchr("-.", s[n-1]))
                        بازگشت false;
        }

        بازگشت in_charset(s, ALPHANUMERICAL "-.=");
}

صحیح machine_tag_list_is_valid(نویسه **l) {
        صحیح r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ پوچ);
        انتخاب (r) {
        حالت -EINVAL:
        حالت -E2BIG:
                بازگشت false;
        حالت 0:
                بازگشت true;
        پیشفرض:
                بازگشت r;
        }
}

صحیح machine_tags_from_string(ثابت نویسه *s, bool graceful, نویسه ***ret) {
        أكد(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        اگر (isempty(s)) {
                *ret = پوچ;
                بازگشت 0;
        }

        _cleanup_strv_free_ نویسه **l = strv_split(s, ":");
        اگر (!l)
                بازگشت -ENOMEM;

        بازگشت machine_tags_from_strv(l, graceful, ret);
}

صحیح machine_tags_from_strv(نویسه **l, bool graceful, نویسه ***ret) {
        صحیح r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = پوچ;
        STRV_FOREACH(i, l) {
                اگر (!machine_tag_is_valid(*i)) {
                        اگر (graceful)
                                ادامه;

                        بازگشت -EINVAL;
                }

                اگر (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        بازگشت -E2BIG;

                ثابت نویسه *eq = strchrnul(*i, '=');
                _cleanup_free_ نویسه *k = strndup(*i, (eq - *i) + (*eq == '='));
                اگر (!k)
                        بازگشت -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                اگر (r == -EEXIST) {
                        اگر (streq(hashmap_get(h, k), *i))
                                ادامه;
                        اگر (!graceful)
                                بازگشت -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } وگرنه اگر (r < 0)
                        بازگشت r;
                وگرنه اگر (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        اگر (ret) {
                _cleanup_strv_free_ نویسه **cleaned = پوچ;
                size_t n = 0;
                نویسه *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        اگر (r < 0)
                                بازگشت r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        بازگشت 0;
}
