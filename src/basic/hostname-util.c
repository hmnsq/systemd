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
        אם (e) {
                אם (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        החזר strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ حرف *f = عدم;
        r = parse_os_release(عدم, "DEFAULT_HOSTNAME", &f);
        אם (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        אחרת אם (f) {
                אם (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        החזר TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        החזר strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(حرف c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        החזר ascii_isalpha(c) ||
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

        אם (isempty(s))
                החזר false;

        אם (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                החזר FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        עבור (p = s, dot = hyphen = true; *p; p++)
                אם (*p == '.') {
                        אם (dot || hyphen)
                                החזר false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } אחרת אם (*p == '-') {
                        אם (dot)
                                החזר false;

                        dot = false;
                        hyphen = true;

                } אחרת {
                        אם (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                החזר false;

                        dot = false;
                        hyphen = false;
                }

        אם (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                החזר false;
        אם (hyphen)
                החזר false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        אם (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                החזר false;

        החזר true;
}

حرف* hostname_cleanup(حرف *s) {
        حرف *p, *d;
        bool dot, hyphen;

        أكد(s);

        עבור (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                אם (*p == '.') {
                        אם (dot || hyphen)
                                استمر;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } אחרת אם (*p == '-') {
                        אם (dot)
                                استمر;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } אחרת אם (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        כלעוד (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        החזר s;
}

bool is_localhost(ثابت حرف *hostname) {
        أكد(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        החזר STRCASE_IN_SET(
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

        אם (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        החזר cached;
}

ثابت حرف* etc_machine_info(فراغ) {
        ساكن ثابت حرف *cached = عدم;

        אם (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        החזר cached;
}

صحيح get_pretty_hostname(حرف **ret) {
        _cleanup_free_ حرف *n = عدم;
        صحيح r;

        أكد(ret);

        r = parse_env_file(عدم, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        אם (r < 0)
                החזר r;

        אם (isempty(n))
                החזר -ENXIO;

        *ret = TAKE_PTR(n);
        החזר 0;
}

صحيح split_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        أكد(s);

        ثابت حرف *rhs = strchr(s, '@');
        אם (rhs) {
                אם (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        אם (!u)
                                החזר -ENOMEM;
                }

                אם (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        אם (!h)
                                החזר -ENOMEM;
                }

        } אחרת {
                אם (isempty(s))
                        החזר -EINVAL;

                אם (ret_host) {
                        h = strdup(s);
                        אם (!h)
                                החזר -ENOMEM;
                }
        }

        אם (ret_user)
                *ret_user = TAKE_PTR(u);
        אם (ret_host)
                *ret_host = TAKE_PTR(h);

        החזר !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

صحيح machine_spec_valid(ثابت حرف *s) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;
        صحيح r;

        أكد(s);

        r = split_user_at_host(s, &u, &h);
        אם (r == -EINVAL)
                החזר false;
        אם (r < 0)
                החזר r;

        אם (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                החזר false;

        אם (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                החזר false;

        החזר true;
}

bool machine_tag_is_valid(ثابت حرف *s) {
        size_t n = strlen_ptr(s);
        אם (n <= 0 || n >= 256)
                החזר false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        אם (strchr("-.=", s[0]))
                החזר false;

        /* We allow parameterization of tags, with a "=" as separator */
        ثابت حرف *eq = strchr(s, '=');
        אם (eq) {
                أكد(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                אם (strchr("-.", eq[-1]))
                        החזר false;
        } אחרת {
                /* If there's no '=', then make the restriction on the very last character */
                אם (strchr("-.", s[n-1]))
                        החזר false;
        }

        החזר in_charset(s, ALPHANUMERICAL "-.=");
}

صحيح machine_tag_list_is_valid(حرف **l) {
        صحيح r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ عدم);
        بدل (r) {
        حالة -EINVAL:
        حالة -E2BIG:
                החזר false;
        حالة 0:
                החזר true;
        افتراضي:
                החזר r;
        }
}

صحيح machine_tags_from_string(ثابت حرف *s, bool graceful, حرف ***ret) {
        أكد(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        אם (isempty(s)) {
                *ret = عدم;
                החזר 0;
        }

        _cleanup_strv_free_ حرف **l = strv_split(s, ":");
        אם (!l)
                החזר -ENOMEM;

        החזר machine_tags_from_strv(l, graceful, ret);
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
                אם (!machine_tag_is_valid(*i)) {
                        אם (graceful)
                                استمر;

                        החזר -EINVAL;
                }

                אם (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        החזר -E2BIG;

                ثابت حرف *eq = strchrnul(*i, '=');
                _cleanup_free_ حرف *k = strndup(*i, (eq - *i) + (*eq == '='));
                אם (!k)
                        החזר -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                אם (r == -EEXIST) {
                        אם (streq(hashmap_get(h, k), *i))
                                استمر;
                        אם (!graceful)
                                החזר -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } אחרת אם (r < 0)
                        החזר r;
                אחרת אם (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        אם (ret) {
                _cleanup_strv_free_ حرف **cleaned = عدم;
                size_t n = 0;
                حرف *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        אם (r < 0)
                                החזר r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        החזר 0;
}
