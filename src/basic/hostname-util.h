/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#ವ್ಯಾಖ್ಯಾನಿಸು LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

ಅಕ್ಷರ* get_default_hostname_raw(ಶೂನ್ಯ);

bool valid_ldh_char(ಅಕ್ಷರ c) _const_;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_ಶಬ್ದ_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ValidHostnameFlags flags) ಶುದ್ಧ;
ಅಕ್ಷರ* hostname_cleanup(ಅಕ್ಷರ *s);

bool is_localhost(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_gateway_hostname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ಹಿಂತಿರುಗಿ STRCASE_ಒಳ_ಗಣ(hostname, "_gateway", "_gateway.");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_outbound_hostname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ಹಿಂತಿರುಗಿ STRCASE_ಒಳ_ಗಣ(hostname, "_outbound", "_outbound.");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_dns_stub_hostname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname) {
        ಹಿಂತಿರುಗಿ STRCASE_ಒಳ_ಗಣ(hostname, "_localdnsstub", "_localdnsstub.");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_dns_proxy_stub_hostname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname) {
        ಹಿಂತಿರುಗಿ STRCASE_ಒಳ_ಗಣ(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_hostname(ಶೂನ್ಯ);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_machine_info(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ get_pretty_hostname(ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ machine_spec_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ ವಿಭಜಿಸು_user_at_host(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ **ret_user, ಅಕ್ಷರ **ret_host);

#ವ್ಯಾಖ್ಯಾನಿಸು MACHINE_ಟ್ಯಾಗುಗಳು_MAX 1024U

bool machine_ಟ್ಯಾಗ್_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗ್_list_is_valid(ಅಕ್ಷರ **l);
ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗುಗಳು_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool graceful, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ machine_ಟ್ಯಾಗುಗಳು_from_strv(ಅಕ್ಷರ **l, bool graceful, ಅಕ್ಷರ ***ret);
