/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

ਅੱਖਰ* get_default_hostname_raw(ਖਾਲੀ);

bool valid_ldh_char(ਅੱਖਰ c) _const_;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_ਸ਼ਬਦ_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ਅਚਲ ਅੱਖਰ *s, ValidHostnameFlags flags) ਸ਼ੁੱਧ;
ਅੱਖਰ* hostname_cleanup(ਅੱਖਰ *s);

bool is_localhost(ਅਚਲ ਅੱਖਰ *hostname);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_gateway_hostname(ਅਚਲ ਅੱਖਰ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ਵਾਪਸ STRCASE_ਵਿੱਚ_ਸੈੱਟ(hostname, "_gateway", "_gateway.");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_outbound_hostname(ਅਚਲ ਅੱਖਰ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ਵਾਪਸ STRCASE_ਵਿੱਚ_ਸੈੱਟ(hostname, "_outbound", "_outbound.");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_dns_stub_hostname(ਅਚਲ ਅੱਖਰ *hostname) {
        ਵਾਪਸ STRCASE_ਵਿੱਚ_ਸੈੱਟ(hostname, "_localdnsstub", "_localdnsstub.");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool is_dns_proxy_stub_hostname(ਅਚਲ ਅੱਖਰ *hostname) {
        ਵਾਪਸ STRCASE_ਵਿੱਚ_ਸੈੱਟ(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ਅਚਲ ਅੱਖਰ* etc_hostname(ਖਾਲੀ);
ਅਚਲ ਅੱਖਰ* etc_machine_info(ਖਾਲੀ);

ਪੂਰਨ get_pretty_hostname(ਅੱਖਰ **ret);

ਪੂਰਨ machine_spec_valid(ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ ਵੰਡੋ_user_at_host(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ **ret_user, ਅੱਖਰ **ret_host);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MACHINE_ਟੈਗ_ਸੂਚੀ_MAX 1024U

bool machine_ਟੈਗ_is_valid(ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ machine_ਟੈਗ_list_is_valid(ਅੱਖਰ **l);
ਪੂਰਨ machine_ਟੈਗ_ਸੂਚੀ_from_string(ਅਚਲ ਅੱਖਰ *s, bool graceful, ਅੱਖਰ ***ret);
ਪੂਰਨ machine_ਟੈਗ_ਸੂਚੀ_from_strv(ਅੱਖਰ **l, bool graceful, ਅੱਖਰ ***ret);
