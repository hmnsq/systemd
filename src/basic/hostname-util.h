/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

نویسه* get_default_hostname_raw(تهی);

bool valid_ldh_char(نویسه c) _const_;

تعریفنوع شمارش ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ثابت نویسه *s, ValidHostnameFlags flags) _pure_;
نویسه* hostname_cleanup(نویسه *s);

bool is_localhost(ثابت نویسه *hostname);

ایستا درونخطی bool is_gateway_hostname(ثابت نویسه *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        بازگشت STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

ایستا درونخطی bool is_outbound_hostname(ثابت نویسه *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        بازگشت STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

ایستا درونخطی bool is_dns_stub_hostname(ثابت نویسه *hostname) {
        بازگشت STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

ایستا درونخطی bool is_dns_proxy_stub_hostname(ثابت نویسه *hostname) {
        بازگشت STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ثابت نویسه* etc_hostname(تهی);
ثابت نویسه* etc_machine_info(تهی);

صحیح get_pretty_hostname(نویسه **ret);

صحیح machine_spec_valid(ثابت نویسه *s);
صحیح split_user_at_host(ثابت نویسه *s, نویسه **ret_user, نویسه **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ثابت نویسه *s);
صحیح machine_tag_list_is_valid(نویسه **l);
صحیح machine_tags_from_string(ثابت نویسه *s, bool graceful, نویسه ***ret);
صحیح machine_tags_from_strv(نویسه **l, bool graceful, نویسه ***ret);
