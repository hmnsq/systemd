/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

ವರ್ಣ* get_default_hostname_raw(ಶೂನ್ಯ);

bool valid_ldh_char(ವರ್ಣ c) _const_;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ಧ್ರುವ ವರ್ಣ *s, ValidHostnameFlags flags) _pure_;
ವರ್ಣ* hostname_cleanup(ವರ್ಣ *s);

bool is_localhost(ಧ್ರುವ ವರ್ಣ *hostname);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool is_gateway_hostname(ಧ್ರುವ ವರ್ಣ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ಪ್ರತ್ಯಾಗಮ STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool is_outbound_hostname(ಧ್ರುವ ವರ್ಣ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ಪ್ರತ್ಯಾಗಮ STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool is_dns_stub_hostname(ಧ್ರುವ ವರ್ಣ *hostname) {
        ಪ್ರತ್ಯಾಗಮ STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool is_dns_proxy_stub_hostname(ಧ್ರುವ ವರ್ಣ *hostname) {
        ಪ್ರತ್ಯಾಗಮ STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ಧ್ರುವ ವರ್ಣ* etc_hostname(ಶೂನ್ಯ);
ಧ್ರುವ ವರ್ಣ* etc_machine_info(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ get_pretty_hostname(ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ machine_spec_valid(ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ split_user_at_host(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ **ret_user, ವರ್ಣ **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ machine_tag_list_is_valid(ವರ್ಣ **l);
ಪೂರ್ಣಾಙ್ಕ machine_tags_from_string(ಧ್ರುವ ವರ್ಣ *s, bool graceful, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ machine_tags_from_strv(ವರ್ಣ **l, bool graceful, ವರ್ಣ ***ret);
