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
        إذا (e) {
                إذا (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_لفظ_TOKEN))
                        أرجع strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ حرف *f = عدم;
        r = parse_os_release(عدم, "DEFAULT_HOSTNAME", &f);
        إذا (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        وإلا إذا (f) {
                إذا (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_لفظ_TOKEN))
                        أرجع TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        أرجع strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(حرف c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        أرجع ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(ثابت حرف *s, ValidHostnameFlags flags) {
        بنانشانی n_dots = 0;
        ثابت حرف *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        إذا (isempty(s))
                أرجع false;

        إذا (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                أرجع FLAGS_سیٹ(flags, VALID_HOSTNAME_DOT_HOST);

        لكل (p = s, dot = hyphen = true; *p; p++)
                إذا (*p == '.') {
                        إذا (dot || hyphen)
                                أرجع false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } وإلا إذا (*p == '-') {
                        إذا (dot)
                                أرجع false;

                        dot = false;
                        hyphen = true;

                } وإلا {
                        إذا (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_سیٹ(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_سیٹ(flags, VALID_HOSTNAME_لفظ_TOKEN)))
                                أرجع false;

                        dot = false;
                        hyphen = false;
                }

        إذا (dot && (n_dots < 2 || !FLAGS_سیٹ(flags, VALID_HOSTNAME_TRAILING_DOT)))
                أرجع false;
        إذا (hyphen)
                أرجع false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        إذا (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                أرجع false;

        أرجع true;
}

حرف* hostname_cleanup(حرف *s) {
        حرف *p, *d;
        bool dot, hyphen;

        أكد(s);

        لكل (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                إذا (*p == '.') {
                        إذا (dot || hyphen)
                                استمر;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } وإلا إذا (*p == '-') {
                        إذا (dot)
                                استمر;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } وإلا إذا (valid_ldh_char(*p) || وچ_سیٹ(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        طالما (d > s && وچ_سیٹ(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        أرجع s;
}

bool is_localhost(ثابت حرف *hostname) {
        أكد(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        أرجع STRCASE_وچ_سیٹ(
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

        إذا (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        أرجع cached;
}

ثابت حرف* etc_machine_info(فراغ) {
        ساكن ثابت حرف *cached = عدم;

        إذا (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        أرجع cached;
}

صحيح get_pretty_hostname(حرف **ret) {
        _cleanup_free_ حرف *n = عدم;
        صحيح r;

        أكد(ret);

        r = parse_env_file(عدم, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        إذا (r < 0)
                أرجع r;

        إذا (isempty(n))
                أرجع -ENXIO;

        *ret = TAKE_PTR(n);
        أرجع 0;
}

صحيح ونڈو_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        أكد(s);

        ثابت حرف *rhs = strchr(s, '@');
        إذا (rhs) {
                إذا (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        إذا (!u)
                                أرجع -ENOMEM;
                }

                إذا (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        إذا (!h)
                                أرجع -ENOMEM;
                }

        } وإلا {
                إذا (isempty(s))
                        أرجع -EINVAL;

                إذا (ret_host) {
                        h = strdup(s);
                        إذا (!h)
                                أرجع -ENOMEM;
                }
        }

        إذا (ret_user)
                *ret_user = TAKE_PTR(u);
        إذا (ret_host)
                *ret_host = TAKE_PTR(h);

        أرجع !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

صحيح machine_spec_valid(ثابت حرف *s) {
        _cleanup_free_ حرف *u = عدم, *h = عدم;
        صحيح r;

        أكد(s);

        r = ونڈو_user_at_host(s, &u, &h);
        إذا (r == -EINVAL)
                أرجع false;
        إذا (r < 0)
                أرجع r;

        إذا (u && !valid_user_گروہ_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                أرجع false;

        إذا (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                أرجع false;

        أرجع true;
}

bool machine_ٹیگ_is_valid(ثابت حرف *s) {
        size_t n = strlen_ptr(s);
        إذا (n <= 0 || n >= 256)
                أرجع false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        إذا (strchr("-.=", s[0]))
                أرجع false;

        /* We allow parameterization of tags, with a "=" as separator */
        ثابت حرف *eq = strchr(s, '=');
        إذا (eq) {
                أكد(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                إذا (strchr("-.", eq[-1]))
                        أرجع false;
        } وإلا {
                /* If there's no '=', then make the restriction on the very last character */
                إذا (strchr("-.", s[n-1]))
                        أرجع false;
        }

        أرجع وچ_charset(s, ALPHANUMERICAL "-.=");
}

صحيح machine_ٹیگ_list_is_valid(حرف **l) {
        صحيح r;

        r = machine_ٹیگ_سوچی_from_strv(l, /* graceful= */ false, /* ret= */ عدم);
        بدل (r) {
        حالة -EINVAL:
        حالة -E2BIG:
                أرجع false;
        حالة 0:
                أرجع true;
        افتراضي:
                أرجع r;
        }
}

صحيح machine_ٹیگ_سوچی_from_string(ثابت حرف *s, bool graceful, حرف ***ret) {
        أكد(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        إذا (isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        _cleanup_strv_free_ حرف **l = strv_ونڈو(s, ":");
        إذا (!l)
                أرجع -ENOMEM;

        أرجع machine_ٹیگ_سوچی_from_strv(l, graceful, ret);
}

صحيح machine_ٹیگ_سوچی_from_strv(حرف **l, bool graceful, حرف ***ret) {
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
                إذا (!machine_ٹیگ_is_valid(*i)) {
                        إذا (graceful)
                                استمر;

                        أرجع -EINVAL;
                }

                إذا (hashmap_ناپ_2(h) >= MACHINE_ٹیگ_سوچی_MAX)
                        أرجع -E2BIG;

                ثابت حرف *eq = strchrnul(*i, '=');
                _cleanup_free_ حرف *k = strndup(*i, (eq - *i) + (*eq == '='));
                إذا (!k)
                        أرجع -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                إذا (r == -EEXIST) {
                        إذا (streq(hashmap_get(h, k), *i))
                                استمر;
                        إذا (!graceful)
                                أرجع -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } وإلا إذا (r < 0)
                        أرجع r;
                وإلا إذا (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        إذا (ret) {
                _cleanup_strv_free_ حرف **cleaned = عدم;
                size_t n = 0;
                حرف *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_ناپ(&cleaned, &n, v);
                        إذا (r < 0)
                                أرجع r;
                }

                strv_ترتیب_دیو(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        أرجع 0;
}
