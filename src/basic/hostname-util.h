/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

वर्ण* get_default_hostname_raw(शून्य);

bool valid_ldh_char(वर्ण c) _const_;

प्रकारनिर्देश गणना ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ध्रुव वर्ण *s, ValidHostnameFlags flags) _pure_;
वर्ण* hostname_cleanup(वर्ण *s);

bool is_localhost(ध्रुव वर्ण *hostname);

स्थिर अन्तःस्थ bool is_gateway_hostname(ध्रुव वर्ण *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        पटिनिवत्त STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

स्थिर अन्तःस्थ bool is_outbound_hostname(ध्रुव वर्ण *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        पटिनिवत्त STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

स्थिर अन्तःस्थ bool is_dns_stub_hostname(ध्रुव वर्ण *hostname) {
        पटिनिवत्त STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

स्थिर अन्तःस्थ bool is_dns_proxy_stub_hostname(ध्रुव वर्ण *hostname) {
        पटिनिवत्त STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ध्रुव वर्ण* etc_hostname(शून्य);
ध्रुव वर्ण* etc_machine_info(शून्य);

पूर्णाङ्क get_pretty_hostname(वर्ण **ret);

पूर्णाङ्क machine_spec_valid(ध्रुव वर्ण *s);
पूर्णाङ्क split_user_at_host(ध्रुव वर्ण *s, वर्ण **ret_user, वर्ण **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ध्रुव वर्ण *s);
पूर्णाङ्क machine_tag_list_is_valid(वर्ण **l);
पूर्णाङ्क machine_tags_from_string(ध्रुव वर्ण *s, bool graceful, वर्ण ***ret);
पूर्णाङ्क machine_tags_from_strv(वर्ण **l, bool graceful, वर्ण ***ret);
