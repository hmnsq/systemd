/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#घोषणा LINUX_HOST_NAME_MAX CONST_MIN((माप_प्रकार) HOST_NAME_MAX, (माप_प्रकार) 64)

अक्षर* get_default_hostname_raw(व्योम);

bool valid_ldh_char(अक्षर c) _const_;

प्रकार क्रमागत ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(स्थिर अक्षर *s, ValidHostnameFlags flags) _pure_;
अक्षर* hostname_cleanup(अक्षर *s);

bool is_localhost(स्थिर अक्षर *hostname);

अटल अंतरभूत bool is_gateway_hostname(स्थिर अक्षर *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        वापस STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

अटल अंतरभूत bool is_outbound_hostname(स्थिर अक्षर *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        वापस STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

अटल अंतरभूत bool is_dns_stub_hostname(स्थिर अक्षर *hostname) {
        वापस STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

अटल अंतरभूत bool is_dns_proxy_stub_hostname(स्थिर अक्षर *hostname) {
        वापस STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

स्थिर अक्षर* etc_hostname(व्योम);
स्थिर अक्षर* etc_machine_info(व्योम);

पूर्णांक get_pretty_hostname(अक्षर **ret);

पूर्णांक machine_spec_valid(स्थिर अक्षर *s);
पूर्णांक split_user_at_host(स्थिर अक्षर *s, अक्षर **ret_user, अक्षर **ret_host);

#घोषणा MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(स्थिर अक्षर *s);
पूर्णांक machine_tag_list_is_valid(अक्षर **l);
पूर्णांक machine_tags_from_string(स्थिर अक्षर *s, bool graceful, अक्षर ***ret);
पूर्णांक machine_tags_from_strv(अक्षर **l, bool graceful, अक्षर ***ret);
