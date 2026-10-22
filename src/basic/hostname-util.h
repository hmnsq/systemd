/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((माप_प्रकार) HOST_NAME_MAX, (माप_प्रकार) 64)

अक्षर* get_default_hostname_raw(शून्य);

bool valid_ldh_char(अक्षर c) _const_;

प्रकारव्याख्या गणना ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(स्थिरांक अक्षर *s, ValidHostnameFlags flags) _pure_;
अक्षर* hostname_cleanup(अक्षर *s);

bool is_localhost(स्थिरांक अक्षर *hostname);

स्थिर ओळीत bool is_gateway_hostname(स्थिरांक अक्षर *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        परत STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

स्थिर ओळीत bool is_outbound_hostname(स्थिरांक अक्षर *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        परत STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

स्थिर ओळीत bool is_dns_stub_hostname(स्थिरांक अक्षर *hostname) {
        परत STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

स्थिर ओळीत bool is_dns_proxy_stub_hostname(स्थिरांक अक्षर *hostname) {
        परत STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

स्थिरांक अक्षर* etc_hostname(शून्य);
स्थिरांक अक्षर* etc_machine_info(शून्य);

पूर्णांक get_pretty_hostname(अक्षर **ret);

पूर्णांक machine_spec_valid(स्थिरांक अक्षर *s);
पूर्णांक split_user_at_host(स्थिरांक अक्षर *s, अक्षर **ret_user, अक्षर **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(स्थिरांक अक्षर *s);
पूर्णांक machine_tag_list_is_valid(अक्षर **l);
पूर्णांक machine_tags_from_string(स्थिरांक अक्षर *s, bool graceful, अक्षर ***ret);
पूर्णांक machine_tags_from_strv(अक्षर **l, bool graceful, अक्षर ***ret);
