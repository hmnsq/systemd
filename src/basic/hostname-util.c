/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-file.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "hostname-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "os-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "user-util.h"

അക്ഷരം* get_default_hostname_raw(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        /* Returns the default hostname, and leaves any ??? in place. */

        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_DEFAULT_HOSTNAME");
        എങ്കിൽ (e) {
                എങ്കിൽ (hostname_is_valid(e, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        തിരികെ strdup(e);

                log_debug("Invalid hostname in $SYSTEMD_DEFAULT_HOSTNAME, ignoring: %s", e);
        }

        _cleanup_free_ അക്ഷരം *f = ശൂന്യ;
        r = parse_os_release(ശൂന്യ, "DEFAULT_HOSTNAME", &f);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to parse os-release, ignoring: %m");
        അല്ലെങ്കിൽ എങ്കിൽ (f) {
                എങ്കിൽ (hostname_is_valid(f, VALID_HOSTNAME_QUESTION_MARK|VALID_HOSTNAME_WORD_TOKEN))
                        തിരികെ TAKE_PTR(f);

                log_debug("Invalid hostname in os-release, ignoring: %s", f);
        }

        തിരികെ strdup(FALLBACK_HOSTNAME);
}

bool valid_ldh_char(അക്ഷരം c) {
        /* "LDH" → "Letters, digits, hyphens", as per RFC 5890, Section 2.3.1 */

        തിരികെ ascii_isalpha(c) ||
                ascii_isdigit(c) ||
                c == '-';
}

bool hostname_is_valid(സ്ഥിരം അക്ഷരം *s, ValidHostnameFlags flags) {
        unsigned n_dots = 0;
        സ്ഥിരം അക്ഷരം *p;
        bool dot, hyphen;

        /* Check if s looks like a valid hostname or FQDN. This does not do full DNS validation, but only
         * checks if the name is composed of allowed characters and the length is not above the maximum
         * allowed by Linux (c.f. dns_name_is_valid()). A trailing dot is allowed if
         * VALID_HOSTNAME_TRAILING_DOT flag is set and at least two components are present in the name. Note
         * that due to the restricted charset and length this call is substantially more conservative than
         * dns_name_is_valid(). Doesn't accept empty hostnames, hostnames with leading dots, and hostnames
         * with multiple dots in a sequence. Doesn't allow hyphens at the beginning or end of label. */

        എങ്കിൽ (isempty(s))
                തിരികെ false;

        എങ്കിൽ (streq(s, ".host")) /* Used by the container logic to denote the "root container" */
                തിരികെ FLAGS_SET(flags, VALID_HOSTNAME_DOT_HOST);

        വേണ്ടി (p = s, dot = hyphen = true; *p; p++)
                എങ്കിൽ (*p == '.') {
                        എങ്കിൽ (dot || hyphen)
                                തിരികെ false;

                        dot = true;
                        hyphen = false;
                        n_dots++;

                } അല്ലെങ്കിൽ എങ്കിൽ (*p == '-') {
                        എങ്കിൽ (dot)
                                തിരികെ false;

                        dot = false;
                        hyphen = true;

                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (!valid_ldh_char(*p) &&
                            (*p != '?' || !FLAGS_SET(flags, VALID_HOSTNAME_QUESTION_MARK)) &&
                            (*p != '$' || !FLAGS_SET(flags, VALID_HOSTNAME_WORD_TOKEN)))
                                തിരികെ false;

                        dot = false;
                        hyphen = false;
                }

        എങ്കിൽ (dot && (n_dots < 2 || !FLAGS_SET(flags, VALID_HOSTNAME_TRAILING_DOT)))
                തിരികെ false;
        എങ്കിൽ (hyphen)
                തിരികെ false;

        /* Note that host name max is 64 on Linux, but DNS allows domain names up to 255 characters. */
        എങ്കിൽ (p - s > (ssize_t) LINUX_HOST_NAME_MAX)
                തിരികെ false;

        തിരികെ true;
}

അക്ഷരം* hostname_cleanup(അക്ഷരം *s) {
        അക്ഷരം *p, *d;
        bool dot, hyphen;

        പ്രതിജ്ഞാ(s);

        വേണ്ടി (p = s, d = s, dot = hyphen = true; *p && d - s < (ssize_t) LINUX_HOST_NAME_MAX; p++)
                എങ്കിൽ (*p == '.') {
                        എങ്കിൽ (dot || hyphen)
                                തുടരുക;

                        *(d++) = '.';
                        dot = true;
                        hyphen = false;

                } അല്ലെങ്കിൽ എങ്കിൽ (*p == '-') {
                        എങ്കിൽ (dot)
                                തുടരുക;

                        *(d++) = '-';
                        dot = false;
                        hyphen = true;

                } അല്ലെങ്കിൽ എങ്കിൽ (valid_ldh_char(*p) || IN_SET(*p, '?', '$')) {
                        *(d++) = *p;
                        dot = false;
                        hyphen = false;
                }

        ആകുമ്പോൾ (d > s && IN_SET(d[-1], '-', '.'))
                /* The dot can occur at most once, but we might have multiple
                 * hyphens, hence the loop */
                d--;
        *d = 0;

        തിരികെ s;
}

bool is_localhost(സ്ഥിരം അക്ഷരം *hostname) {
        പ്രതിജ്ഞാ(hostname);

        /* This tries to identify local host and domain names
         * described in RFC6761 plus the redhatism of localdomain */

        തിരികെ STRCASE_IN_SET(
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

സ്ഥിരം അക്ഷരം* etc_hostname(ശൂന്യം) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം *cached = ശൂന്യ;

        എങ്കിൽ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_HOSTNAME") ?: "/etc/hostname";

        തിരികെ cached;
}

സ്ഥിരം അക്ഷരം* etc_machine_info(ശൂന്യം) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം *cached = ശൂന്യ;

        എങ്കിൽ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_MACHINE_INFO") ?: "/etc/machine-info";

        തിരികെ cached;
}

പൂർണ്ണസംഖ്യ get_pretty_hostname(അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *n = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = parse_env_file(ശൂന്യ, etc_machine_info(), "PRETTY_HOSTNAME", &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (isempty(n))
                തിരികെ -ENXIO;

        *ret = TAKE_PTR(n);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ split_user_at_host(സ്ഥിരം അക്ഷരം *s, അക്ഷരം **ret_user, അക്ഷരം **ret_host) {
        _cleanup_free_ അക്ഷരം *u = ശൂന്യ, *h = ശൂന്യ;

        /* Splits a user@host expression (one of those we accept on --machine= and similar). Returns NULL in
         * each of the two return parameters if that part was left empty. */

        പ്രതിജ്ഞാ(s);

        സ്ഥിരം അക്ഷരം *rhs = strchr(s, '@');
        എങ്കിൽ (rhs) {
                എങ്കിൽ (ret_user && rhs > s) {
                        u = strndup(s, rhs - s);
                        എങ്കിൽ (!u)
                                തിരികെ -ENOMEM;
                }

                എങ്കിൽ (ret_host && rhs[1] != 0) {
                        h = strdup(rhs + 1);
                        എങ്കിൽ (!h)
                                തിരികെ -ENOMEM;
                }

        } അല്ലെങ്കിൽ {
                എങ്കിൽ (isempty(s))
                        തിരികെ -EINVAL;

                എങ്കിൽ (ret_host) {
                        h = strdup(s);
                        എങ്കിൽ (!h)
                                തിരികെ -ENOMEM;
                }
        }

        എങ്കിൽ (ret_user)
                *ret_user = TAKE_PTR(u);
        എങ്കിൽ (ret_host)
                *ret_host = TAKE_PTR(h);

        തിരികെ !!rhs; /* return > 0 if '@' was specified, 0 otherwise */
}

പൂർണ്ണസംഖ്യ machine_spec_valid(സ്ഥിരം അക്ഷരം *s) {
        _cleanup_free_ അക്ഷരം *u = ശൂന്യ, *h = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = split_user_at_host(s, &u, &h);
        എങ്കിൽ (r == -EINVAL)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (u && !valid_user_group_name(u, VALID_USER_RELAX | VALID_USER_ALLOW_NUMERIC))
                തിരികെ false;

        എങ്കിൽ (h && !hostname_is_valid(h, VALID_HOSTNAME_DOT_HOST))
                തിരികെ false;

        തിരികെ true;
}

bool machine_tag_is_valid(സ്ഥിരം അക്ഷരം *s) {
        size_t n = strlen_ptr(s);
        എങ്കിൽ (n <= 0 || n >= 256)
                തിരികെ false;

        /* Don't allow "-" and "." as first char. (This is load-bearing, we want that "+"/"-" can be used as
         * prefix for adding/removing tags from the list). */
        എങ്കിൽ (strchr("-.=", s[0]))
                തിരികെ false;

        /* We allow parameterization of tags, with a "=" as separator */
        സ്ഥിരം അക്ഷരം *eq = strchr(s, '=');
        എങ്കിൽ (eq) {
                പ്രതിജ്ഞാ(eq > s);

                /* If there is an '=', then make the same restrictions as for the first char on the last char before it */
                എങ്കിൽ (strchr("-.", eq[-1]))
                        തിരികെ false;
        } അല്ലെങ്കിൽ {
                /* If there's no '=', then make the restriction on the very last character */
                എങ്കിൽ (strchr("-.", s[n-1]))
                        തിരികെ false;
        }

        തിരികെ in_charset(s, ALPHANUMERICAL "-.=");
}

പൂർണ്ണസംഖ്യ machine_tag_list_is_valid(അക്ഷരം **l) {
        പൂർണ്ണസംഖ്യ r;

        r = machine_tags_from_strv(l, /* graceful= */ false, /* ret= */ ശൂന്യ);
        തിരഞ്ഞെടുപ്പ് (r) {
        അവസ്ഥ -EINVAL:
        അവസ്ഥ -E2BIG:
                തിരികെ false;
        അവസ്ഥ 0:
                തിരികെ true;
        സ്ഥിരസ്ഥിതി:
                തിരികെ r;
        }
}

പൂർണ്ണസംഖ്യ machine_tags_from_string(സ്ഥിരം അക്ഷരം *s, bool graceful, അക്ഷരം ***ret) {
        പ്രതിജ്ഞാ(ret);

        /* Parse the colon-separated TAGS= machine-info field into a sorted, deduplicated strv. */

        എങ്കിൽ (isempty(s)) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        _cleanup_strv_free_ അക്ഷരം **l = strv_split(s, ":");
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        തിരികെ machine_tags_from_strv(l, graceful, ret);
}

പൂർണ്ണസംഖ്യ machine_tags_from_strv(അക്ഷരം **l, bool graceful, അക്ഷരം ***ret) {
        പൂർണ്ണസംഖ്യ r;

        /* Go through a list of tags and verify each tag. Invalid tags are silently dropped if 'graceful',
         * and -EINVAL is returned otherwise. The same valueless tag may specified more than once. If the
         * same tag with listed with different values, the last value wins if 'graceful', otherwise -EINVAL
         * is returned, too. At most MACHINE_TAGS_MAX valid tags are accepted. The result is sorted only
         * after deduplication, and is NULL if no (valid) tags remain. */

        /* Maps the key of each tag, i.e. everything up to and including the first '=', or the whole tag if
         * it has no '=', to the tag itself (borrowed from 'l'). A bare tag and an assignment of the same
         * name hence get distinct keys and may coexist. */
        _cleanup_hashmap_free_ Hashmap *h = ശൂന്യ;
        STRV_FOREACH(i, l) {
                എങ്കിൽ (!machine_tag_is_valid(*i)) {
                        എങ്കിൽ (graceful)
                                തുടരുക;

                        തിരികെ -EINVAL;
                }

                എങ്കിൽ (hashmap_size(h) >= MACHINE_TAGS_MAX)
                        തിരികെ -E2BIG;

                സ്ഥിരം അക്ഷരം *eq = strchrnul(*i, '=');
                _cleanup_free_ അക്ഷരം *k = strndup(*i, (eq - *i) + (*eq == '='));
                എങ്കിൽ (!k)
                        തിരികെ -ENOMEM;

                r = hashmap_ensure_put(&h, &string_hash_ops_free, k, *i);
                എങ്കിൽ (r == -EEXIST) {
                        എങ്കിൽ (streq(hashmap_get(h, k), *i))
                                തുടരുക;
                        എങ്കിൽ (!graceful)
                                തിരികെ -EINVAL;

                        /* when graceful, update to the later value */
                        assert_se(hashmap_update(h, k, *i) >= 0);
                } അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                        തിരികെ r;
                അല്ലെങ്കിൽ എങ്കിൽ (r > 0)
                        TAKE_PTR(k);   /* successfully added the new value */
        }

        എങ്കിൽ (ret) {
                _cleanup_strv_free_ അക്ഷരം **cleaned = ശൂന്യ;
                size_t n = 0;
                അക്ഷരം *v;
                HASHMAP_FOREACH(v, h) {
                        r = strv_extend_with_size(&cleaned, &n, v);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                strv_sort(cleaned);
                *ret = TAKE_PTR(cleaned);
        }

        തിരികെ 0;
}
