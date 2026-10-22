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

حرف* get_default_hostname_raw(خالی) {
        صحیح r;

        /* Returns the default hostname, and leaves any ??? in place. */

        ثابت حرف *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        اگر (e) {
                اگر (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_لفظ_TOKEN))
                        واپس strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ حرف *f = تہی;
        r = parse_os_release(تہی, "DEFAULT_HOSTNAME", &f);
        اگر (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        ورنہ اگر (f) {
                اگر (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_لفظ_TOKEN))
                        واپس TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        واپس strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(حرف c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        واپس ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ثابت حرف *s, ValidHostnameFlags flags) {
        غیرعلامتی n_dots = 0;
        ثابت حرف *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        اگر (isempty(s))
                واپس false;

        اگر (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                واپس FLAGS_سیٹ(flags, VALID_HOSTNAME_DOT_HOST);

        برائے (p = s, dot = hyphen = true; *p; p++)
                اگر (*p == '.') {
                        اگر (dot || hyphen)
                                واپس false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } ورنہ اگر (*p == '-') {
                        اگر (dot)
                                واپس false;

                        dot = false;
                        hyphen = true;

                } ورنہ {
                        اگر (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_سیٹ(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_سیٹ(flags, VALID_HOSTNAME_لفظ_TOKEN)))
                                واپس false;

                        dot = false;
                        hyphen = false;
                }

        اگر (dot && (n_dots < 2 || !FLAGS_سیٹ(flags, VALID_HOSTNAME_TRAILING_DOT)))
                واپس false;
        اگر (hyphen)
                واپس false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        اگر (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                واپس false;

        واپس true;
}

حرف* hostname_cleanup(حرف *s) {
        حرف *p, *d;
        bool dot, hyphen;

        أكد(s);

        برائے (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                اگر (*p == '.') {
                        اگر (dot || hyphen)
                                جاری;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } ورنہ اگر (*p == '-') {
                        اگر (dot)
                                جاری;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } ورنہ اگر (valid_ldh_char(*p) || اندر_سیٹ(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        جب (d > s && اندر_سیٹ(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        واپس s;
}

bool is_localhost(ثابت حرف *hostname) {
        أكد(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        واپس STRCASE_اندر_سیٹ(
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

ثابت حرف* etc_hostname(خالی) {
        ساکن ثابت حرف *cached = تہی;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        واپس cached;
}

ثابت حرف* etc_machine_info(خالی) {
        ساکن ثابت حرف *cached = تہی;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        واپس cached;
}

صحیح get_pretty_hostname(حرف **ret) {
        _cleanup_free_ حرف *n = تہی;
        صحیح r;

        أكد(ret);

        r = parse_env_file(تہی, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        اگر (r < 0)
                واپس r;

        اگر (isempty(n))
                واپس -ENXIO;

        *ret = TAKE_PTR(n);
        واپس 0;
}

صحیح بانٹو_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host) {
        _cleanup_free_ حرف *u = تہی, *h = تہی;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        أكد(s);

        ثابت حرف *rhs = strchr(s, '@');
        اگر (rhs) {
                اگر (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        اگر (!u)
                                واپس -ENOMEM;
                }

                اگر (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        اگر (!h)
                                واپس -ENOMEM;
                }

        } ورنہ {
                اگر (isempty(s))
                        واپس -EINVAL;

                اگر (ret_host) {
                        h = strdup(s);
                        اگر (!h)
                                واپس -ENOMEM;
                }
        }

        اگر (ret_user)
                *ret_user = TAKE_PTR(u);
        اگر (ret_host)
                *ret_host = TAKE_PTR(h);

        واپس !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

صحیح machine_spec_valid(ثابت حرف *s) {
        _cleanup_free_ حرف *u = تہی, *h = تہی;
        صحیح r;

        أكد(s);

        r = بانٹو_user_at_host(s, &u, &h);
        اگر (r == -EINVAL)
                واپس false;
        اگر (r < 0)
                واپس r;

        اگر (u && !valid_user_گروہ_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                واپس false;

        اگر (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                واپس false;

        واپس true;
}

bool machine_لیبل_is_valid(ثابت حرف *s) {
        size_t n = strlen_ptr(s);
        اگر (n <= 0 || n >= 256)
                واپس false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        اگر (strchr("-.=", s[0]))
                واپس false;

        /* We allow parameterization of tags, with a "=" as separator */
        ثابت حرف *eq = strchr(s, '=');
        اگر (eq) {
                أكد(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                اگر (strchr("-.", eq[-1]))
                        واپس false;
        } ورنہ {
                /* If there's no '=', then make the restriction on the very last character */
                اگر (strchr("-.", s[n-1]))
                        واپس false;
        }

        واپس اندر_charset(s, ALPHANUMERICAL "-.=");
}

صحیح machine_لیبل_list_is_valid(حرف **l) {
        صحیح r;

        r = machine_لیبل_فہرست_from_strv(l, /* graceful= */ false, /* ret= */ تہی);
        انتخاب (r) {
        حالت -EINVAL:
        حالت -E2BIG:
                واپس false;
        حالت 0:
                واپس true;
        طےشدہ:
                واپس r;
        }
}

صحیح machine_لیبل_فہرست_from_string(ثابت حرف *s, bool graceful, حرف ***ret) {
        أكد(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        اگر (isempty(s)) {
                *ret = تہی;
                واپس 0;
        }

        _cleanup_strv_free_ حرف **l = strv_بانٹو(s, ":");
        اگر (!l)
                واپس -ENOMEM;

        واپس machine_لیبل_فہرست_from_strv(l, graceful, ret);
}

صحیح machine_لیبل_فہرست_from_strv(حرف **l, bool graceful, حرف ***ret) {
        صحیح r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = تہی;
        STRV_FOREACH(i, l) {
                اگر (!machine_لیبل_is_valid(*i)) {
                        اگر (graceful)
                                جاری;

                        واپس -EINVAL;
                }

                اگر (hashmap_سائز_2(h) >= MACHINE_لیبل_فہرست_MAX)
                        واپس -E2BIG;

                ثابت حرف *eq = strchrnul(*i, '=');
                _cleanup_free_ حرف *k = strndup(*i, (eq - *i) + (*eq == '='));
                اگر (!k)
                        واپس -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                اگر (r == -EEXIST) {
                        اگر (streq(hashmap_get(h, k), *i))
                                جاری;
                        اگر (!graceful)
                                واپس -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } ورنہ اگر (r < 0)
                        واپس r;
                ورنہ اگر (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        اگر (ret) {
                _cleanup_strv_free_ حرف **cleaned = تہی;
                size_t n = 0;
                حرف *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_سائز(&cleaned, &n, v);
                        اگر (r < 0)
                                واپس r;
                }

                strv_ترتیب_دو(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        واپس 0;
}
