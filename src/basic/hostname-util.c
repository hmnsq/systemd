/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-file.h"
#அந்தர்பாவ "hashmap.h"
#அந்தர்பாவ "hostname-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "os-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "user-util.h"

எழுத்து* get_default_hostname_raw(வெறுமை) {
        முழு r;

        /* Returns the default hostname, and leaves any ??? in place. */

        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        எனில் (e) {
                எனில் (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        திரும்பு strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ எழுத்து *f = வெற்று;
        r = parse_os_release(வெற்று, "DEFAULT_HOSTNAME", &f);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        இல்லையெனில் எனில் (f) {
                எனில் (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        திரும்பு TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        திரும்பு strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(எழுத்து c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        திரும்பு ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(மாறிலி எழுத்து *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        மாறிலி எழுத்து *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        எனில் (isempty(s))
                திரும்பு false;

        எனில் (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                திரும்பு FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        க்கு (p = s, dot = hyphen = true; *p; p++)
                எனில் (*p == '.') {
                        எனில் (dot || hyphen)
                                திரும்பு false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } இல்லையெனில் எனில் (*p == '-') {
                        எனில் (dot)
                                திரும்பு false;

                        dot = false;
                        hyphen = true;

                } இல்லையெனில் {
                        எனில் (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                திரும்பு false;

                        dot = false;
                        hyphen = false;
                }

        எனில் (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                திரும்பு false;
        எனில் (hyphen)
                திரும்பு false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        எனில் (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                திரும்பு false;

        திரும்பு true;
}

எழுத்து* hostname_cleanup(எழுத்து *s) {
        எழுத்து *p, *d;
        bool dot, hyphen;

        ப்ரதிஜ்ஞா(s);

        க்கு (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                எனில் (*p == '.') {
                        எனில் (dot || hyphen)
                                தொடர்;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } இல்லையெனில் எனில் (*p == '-') {
                        எனில் (dot)
                                தொடர்;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } இல்லையெனில் எனில் (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        போது (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        திரும்பு s;
}

bool is_localhost(மாறிலி எழுத்து *hostname) {
        ப்ரதிஜ்ஞா(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        திரும்பு STRCASE_IN_SET(
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

மாறிலி எழுத்து* etc_hostname(வெறுமை) {
        நிலையான மாறிலி எழுத்து *cached = வெற்று;

        எனில் (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        திரும்பு cached;
}

மாறிலி எழுத்து* etc_machine_info(வெறுமை) {
        நிலையான மாறிலி எழுத்து *cached = வெற்று;

        எனில் (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        திரும்பு cached;
}

முழு get_pretty_hostname(எழுத்து **ret) {
        _cleanup_free_ எழுத்து *n = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = parse_env_file(வெற்று, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (isempty(n))
                திரும்பு -ENXIO;

        *ret = TAKE_PTR(n);
        திரும்பு 0;
}

முழு split_user_at_host(மாறிலி எழுத்து *s, எழுத்து **ret_user, எழுத்து **ret_host) {
        _cleanup_free_ எழுத்து *u = வெற்று, *h = வெற்று;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        ப்ரதிஜ்ஞா(s);

        மாறிலி எழுத்து *rhs = strchr(s, '@');
        எனில் (rhs) {
                எனில் (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        எனில் (!u)
                                திரும்பு -ENOMEM;
                }

                எனில் (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        எனில் (!h)
                                திரும்பு -ENOMEM;
                }

        } இல்லையெனில் {
                எனில் (isempty(s))
                        திரும்பு -EINVAL;

                எனில் (ret_host) {
                        h = strdup(s);
                        எனில் (!h)
                                திரும்பு -ENOMEM;
                }
        }

        எனில் (ret_user)
                *ret_user = TAKE_PTR(u);
        எனில் (ret_host)
                *ret_host = TAKE_PTR(h);

        திரும்பு !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

முழு machine_spec_valid(மாறிலி எழுத்து *s) {
        _cleanup_free_ எழுத்து *u = வெற்று, *h = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(s);

        r = split_user_at_host(s, &u, &h);
        எனில் (r == -EINVAL)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;

        எனில் (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                திரும்பு false;

        எனில் (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                திரும்பு false;

        திரும்பு true;
}

bool machine_tag_is_valid(மாறிலி எழுத்து *s) {
        size_t n = strlen_ptr(s);
        எனில் (n <= 0 || n >= 256)
                திரும்பு false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        எனில் (strchr("-.=", s[0]))
                திரும்பு false;

        /* We allow parameterization of tags, with a "=" as separator */
        மாறிலி எழுத்து *eq = strchr(s, '=');
        எனில் (eq) {
                ப்ரதிஜ்ஞா(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                எனில் (strchr("-.", eq[-1]))
                        திரும்பு false;
        } இல்லையெனில் {
                /* If there's no '=', then make the restriction on the very last character */
                எனில் (strchr("-.", s[n-1]))
                        திரும்பு false;
        }

        திரும்பு in_charset(s, ALPHANUMERICAL "-.=");
}

முழு machine_tag_list_is_valid(எழுத்து **l) {
        முழு r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ வெற்று);
        தேர்வு (r) {
        நிலை -EINVAL:
        நிலை -E2BIG:
                திரும்பு false;
        நிலை 0:
                திரும்பு true;
        இயல்பு:
                திரும்பு r;
        }
}

முழு machine_tags_from_string(மாறிலி எழுத்து *s, bool graceful, எழுத்து ***ret) {
        ப்ரதிஜ்ஞா(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        எனில் (isempty(s)) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        _cleanup_strv_free_ எழுத்து **l = strv_split(s, ":");
        எனில் (!l)
                திரும்பு -ENOMEM;

        திரும்பு machine_tags_from_strv(l, graceful, ret);
}

முழு machine_tags_from_strv(எழுத்து **l, bool graceful, எழுத்து ***ret) {
        முழு r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = வெற்று;
        STRV_FOREACH(i, l) {
                எனில் (!machine_tag_is_valid(*i)) {
                        எனில் (graceful)
                                தொடர்;

                        திரும்பு -EINVAL;
                }

                எனில் (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        திரும்பு -E2BIG;

                மாறிலி எழுத்து *eq = strchrnul(*i, '=');
                _cleanup_free_ எழுத்து *k = strndup(*i, (eq - *i) + (*eq == '='));
                எனில் (!k)
                        திரும்பு -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                எனில் (r == -EEXIST) {
                        எனில் (streq(hashmap_get(h, k), *i))
                                தொடர்;
                        எனில் (!graceful)
                                திரும்பு -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } இல்லையெனில் எனில் (r < 0)
                        திரும்பு r;
                இல்லையெனில் எனில் (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        எனில் (ret) {
                _cleanup_strv_free_ எழுத்து **cleaned = வெற்று;
                size_t n = 0;
                எழுத்து *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        திரும்பு 0;
}
