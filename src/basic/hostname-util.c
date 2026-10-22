/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "env-file.h"
#ضمن "hashmap.h"
#ضمن "hostname-util.h"
#ضمن "log.h"
#ضمن "os-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "user-util.h"

حرف* get_default_hostname_raw(فراغ) {
        صحيح r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ثابت حرف *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        ܐܢ (e) {
                ܐܢ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ܦܢܐ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ حرف *f = عدم;
        r = parse_os_release(عدم, "DEFAULT_HOSTNAME", &f);
        ܐܢ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ܐܚܪܢܐ ܐܢ (f) {
                ܐܢ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        ܦܢܐ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        ܦܢܐ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(حرف c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        ܦܢܐ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ثابت حرف *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        ثابت حرف *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        ܐܢ (isempty(s))
                ܦܢܐ false;

        ܐܢ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                ܦܢܐ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        لكل (p = s, dot = hyphen = true; *p; p++)
                ܐܢ (*p == '.') {
                        ܐܢ (dot || hyphen)
                                ܦܢܐ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ܐܚܪܢܐ ܐܢ (*p == '-') {
                        ܐܢ (dot)
                                ܦܢܐ false;

                        dot = false;
                        hyphen = true;

                } ܐܚܪܢܐ {
                        ܐܢ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                ܦܢܐ false;

                        dot = false;
                        hyphen = false;
                }

        ܐܢ (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                ܦܢܐ false;
        ܐܢ (hyphen)
                ܦܢܐ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        ܐܢ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                ܦܢܐ false;

        ܦܢܐ true;
}

حرف* hostname_cleanup(حرف *s) {
        حرف *p, *d;
        bool dot, hyphen;

        أكد(s);

        لكل (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                ܐܢ (*p == '.') {
                        ܐܢ (dot || hyphen)
                                استمر;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ܐܚܪܢܐ ܐܢ (*p == '-') {
                        ܐܢ (dot)
                                استمر;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ܐܚܪܢܐ ܐܢ (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ܟܡܐ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        ܦܢܐ s;
}

bool is_localhost(ثابت حرف *hostname) {
        أكد(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        ܦܢܐ STRCASE_IN_SET(
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

ثابت حرف* etc_hostname(فراغ) {
        ساكن ثابت حرف *cached = عدم;

        ܐܢ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        ܦܢܐ cached;
}

ثابت حرف* etc_machine_info(فراغ) {
        ساكن ثابت حرف *cached = عدم;

        ܐܢ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        ܦܢܐ cached;
}

صحيح get_pretty_hostname(حرف **ret) {
        _cleanup_free_ حرف *n = عدم;
        صحيح r;

        أكد(ret);

        r = parse_env_file(عدم, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (isempty(n))
                ܦܢܐ -ENXIO;

        *ret = TAKE_PTR(n);
        ܦܢܐ 0;
}

صحيح split_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        أكد(s);

        ثابت حرف *rhs = strchr(s, '@');
        ܐܢ (rhs) {
                ܐܢ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        ܐܢ (!u)
                                ܦܢܐ -ENOMEM;
                }

                ܐܢ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        ܐܢ (!h)
                                ܦܢܐ -ENOMEM;
                }

        } ܐܚܪܢܐ {
                ܐܢ (isempty(s))
                        ܦܢܐ -EINVAL;

                ܐܢ (ret_host) {
                        h = strdup(s);
                        ܐܢ (!h)
                                ܦܢܐ -ENOMEM;
                }
        }

        ܐܢ (ret_user)
                *ret_user = TAKE_PTR(u);
        ܐܢ (ret_host)
                *ret_host = TAKE_PTR(h);

        ܦܢܐ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

صحيح machine_spec_valid(ثابت حرف *s) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;
        صحيح r;

        أكد(s);

        r = split_user_at_host(s, &u, &h);
        ܐܢ (r == -EINVAL)
                ܦܢܐ false;
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                ܦܢܐ false;

        ܐܢ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                ܦܢܐ false;

        ܦܢܐ true;
}

bool machine_tag_is_valid(ثابت حرف *s) {
        size_t n = strlen_ptr(s);
        ܐܢ (n <= 0 || n >= 256)
                ܦܢܐ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        ܐܢ (strchr("-.=", s[0]))
                ܦܢܐ false;

        /* We allow parameterization of tags, with a "=" as separator */
        ثابت حرف *eq = strchr(s, '=');
        ܐܢ (eq) {
                أكد(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                ܐܢ (strchr("-.", eq[-1]))
                        ܦܢܐ false;
        } ܐܚܪܢܐ {
                /* If there's no '=', then make the restriction on the very last character */
                ܐܢ (strchr("-.", s[n-1]))
                        ܦܢܐ false;
        }

        ܦܢܐ in_charset(s, ALPHANUMERICAL "-.=");
}

صحيح machine_tag_list_is_valid(حرف **l) {
        صحيح r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ عدم);
        بدل (r) {
        حالة -EINVAL:
        حالة -E2BIG:
                ܦܢܐ false;
        حالة 0:
                ܦܢܐ true;
        افتراضي:
                ܦܢܐ r;
        }
}

صحيح machine_tags_from_string(ثابت حرف *s, bool graceful, حرف ***ret) {
        أكد(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        ܐܢ (isempty(s)) {
                *ret = عدم;
                ܦܢܐ 0;
        }

        _cleanup_strv_free_ حرف **l = strv_split(s, ":");
        ܐܢ (!l)
                ܦܢܐ -ENOMEM;

        ܦܢܐ machine_tags_from_strv(l, graceful, ret);
}

صحيح machine_tags_from_strv(حرف **l, bool graceful, حرف ***ret) {
        صحيح r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = عدم;
        STRV_FOREACH(i, l) {
                ܐܢ (!machine_tag_is_valid(*i)) {
                        ܐܢ (graceful)
                                استمر;

                        ܦܢܐ -EINVAL;
                }

                ܐܢ (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        ܦܢܐ -E2BIG;

                ثابت حرف *eq = strchrnul(*i, '=');
                _cleanup_free_ حرف *k = strndup(*i, (eq - *i) + (*eq == '='));
                ܐܢ (!k)
                        ܦܢܐ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                ܐܢ (r == -EEXIST) {
                        ܐܢ (streq(hashmap_get(h, k), *i))
                                استمر;
                        ܐܢ (!graceful)
                                ܦܢܐ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ܐܚܪܢܐ ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܚܪܢܐ ܐܢ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        ܐܢ (ret) {
                _cleanup_strv_free_ حرف **cleaned = عدم;
                size_t n = 0;
                حرف *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        ܦܢܐ 0;
}
