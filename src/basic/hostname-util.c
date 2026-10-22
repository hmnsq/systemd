/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-file.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "hostname-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "os-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "user-util.h"

અક્ષર* get_default_hostname_raw(શૂન્ય) {
        પૂર્ણાંક r;

        /* Returns the default hostname, and leaves any ??? in place. */

        અચલ અક્ષર *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        જો (e) {
                જો (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        પાછા strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ અક્ષર *f = ખાલી;
        r = parse_os_release(ખાલી, "DEFAULT_HOSTNAME", &f);
        જો (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        નહીંતર જો (f) {
                જો (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        પાછા TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        પાછા strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(અક્ષર c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        પાછા ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(અચલ અક્ષર *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        અચલ અક્ષર *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        જો (isempty(s))
                પાછા false;

        જો (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                પાછા FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        માટે (p = s, dot = hyphen = true; *p; p++)
                જો (*p == '.') {
                        જો (dot || hyphen)
                                પાછા false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } નહીંતર જો (*p == '-') {
                        જો (dot)
                                પાછા false;

                        dot = false;
                        hyphen = true;

                } નહીંતર {
                        જો (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                પાછા false;

                        dot = false;
                        hyphen = false;
                }

        જો (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                પાછા false;
        જો (hyphen)
                પાછા false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        જો (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                પાછા false;

        પાછા true;
}

અક્ષર* hostname_cleanup(અક્ષર *s) {
        અક્ષર *p, *d;
        bool dot, hyphen;

        પ્રતિજ્ઞા(s);

        માટે (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                જો (*p == '.') {
                        જો (dot || hyphen)
                                ચાલુ;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } નહીંતર જો (*p == '-') {
                        જો (dot)
                                ચાલુ;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } નહીંતર જો (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        જ્યારે (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        પાછા s;
}

bool is_localhost(અચલ અક્ષર *hostname) {
        પ્રતિજ્ઞા(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        પાછા STRCASE_IN_SET(
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

અચલ અક્ષર* etc_hostname(શૂન્ય) {
        સ્થિર અચલ અક્ષર *cached = ખાલી;

        જો (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        પાછા cached;
}

અચલ અક્ષર* etc_machine_info(શૂન્ય) {
        સ્થિર અચલ અક્ષર *cached = ખાલી;

        જો (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        પાછા cached;
}

પૂર્ણાંક get_pretty_hostname(અક્ષર **ret) {
        _cleanup_free_ અક્ષર *n = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = parse_env_file(ખાલી, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        જો (r < 0)
                પાછા r;

        જો (isempty(n))
                પાછા -ENXIO;

        *ret = TAKE_PTR(n);
        પાછા 0;
}

પૂર્ણાંક split_user_at_host(અચલ અક્ષર *s, અક્ષર **ret_user, અક્ષર **ret_host) {
        _cleanup_free_ અક્ષર *u = ખાલી, *h = ખાલી;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        પ્રતિજ્ઞા(s);

        અચલ અક્ષર *rhs = strchr(s, '@');
        જો (rhs) {
                જો (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        જો (!u)
                                પાછા -ENOMEM;
                }

                જો (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        જો (!h)
                                પાછા -ENOMEM;
                }

        } નહીંતર {
                જો (isempty(s))
                        પાછા -EINVAL;

                જો (ret_host) {
                        h = strdup(s);
                        જો (!h)
                                પાછા -ENOMEM;
                }
        }

        જો (ret_user)
                *ret_user = TAKE_PTR(u);
        જો (ret_host)
                *ret_host = TAKE_PTR(h);

        પાછા !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

પૂર્ણાંક machine_spec_valid(અચલ અક્ષર *s) {
        _cleanup_free_ અક્ષર *u = ખાલી, *h = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = split_user_at_host(s, &u, &h);
        જો (r == -EINVAL)
                પાછા false;
        જો (r < 0)
                પાછા r;

        જો (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                પાછા false;

        જો (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                પાછા false;

        પાછા true;
}

bool machine_tag_is_valid(અચલ અક્ષર *s) {
        size_t n = strlen_ptr(s);
        જો (n <= 0 || n >= 256)
                પાછા false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        જો (strchr("-.=", s[0]))
                પાછા false;

        /* We allow parameterization of tags, with a "=" as separator */
        અચલ અક્ષર *eq = strchr(s, '=');
        જો (eq) {
                પ્રતિજ્ઞા(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                જો (strchr("-.", eq[-1]))
                        પાછા false;
        } નહીંતર {
                /* If there's no '=', then make the restriction on the very last character */
                જો (strchr("-.", s[n-1]))
                        પાછા false;
        }

        પાછા in_charset(s, ALPHANUMERICAL "-.=");
}

પૂર્ણાંક machine_tag_list_is_valid(અક્ષર **l) {
        પૂર્ણાંક r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ ખાલી);
        પસંદગી (r) {
        સ્થિતિ -EINVAL:
        સ્થિતિ -E2BIG:
                પાછા false;
        સ્થિતિ 0:
                પાછા true;
        મૂળભૂત:
                પાછા r;
        }
}

પૂર્ણાંક machine_tags_from_string(અચલ અક્ષર *s, bool graceful, અક્ષર ***ret) {
        પ્રતિજ્ઞા(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        જો (isempty(s)) {
                *ret = ખાલી;
                પાછા 0;
        }

        _cleanup_strv_free_ અક્ષર **l = strv_split(s, ":");
        જો (!l)
                પાછા -ENOMEM;

        પાછા machine_tags_from_strv(l, graceful, ret);
}

પૂર્ણાંક machine_tags_from_strv(અક્ષર **l, bool graceful, અક્ષર ***ret) {
        પૂર્ણાંક r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ખાલી;
        STRV_FOREACH(i, l) {
                જો (!machine_tag_is_valid(*i)) {
                        જો (graceful)
                                ચાલુ;

                        પાછા -EINVAL;
                }

                જો (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        પાછા -E2BIG;

                અચલ અક્ષર *eq = strchrnul(*i, '=');
                _cleanup_free_ અક્ષર *k = strndup(*i, (eq - *i) + (*eq == '='));
                જો (!k)
                        પાછા -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                જો (r == -EEXIST) {
                        જો (streq(hashmap_get(h, k), *i))
                                ચાલુ;
                        જો (!graceful)
                                પાછા -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } નહીંતર જો (r < 0)
                        પાછા r;
                નહીંતર જો (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        જો (ret) {
                _cleanup_strv_free_ અક્ષર **cleaned = ખાલી;
                size_t n = 0;
                અક્ષર *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        જો (r < 0)
                                પાછા r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        પાછા 0;
}
