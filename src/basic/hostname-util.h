/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

বর্ণ* get_default_hostname_raw(শূন্য);

bool valid_ldh_char(বর্ণ c) _const_;

প্রকারনির্দেশ গণনা ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ধ্রুব বর্ণ *s, ValidHostnameFlags flags) _pure_;
বর্ণ* hostname_cleanup(বর্ণ *s);

bool is_localhost(ধ্রুব বর্ণ *hostname);

স্থির অন্তঃস্থ bool is_gateway_hostname(ধ্রুব বর্ণ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        প্রত্যাগম STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

স্থির অন্তঃস্থ bool is_outbound_hostname(ধ্রুব বর্ণ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        প্রত্যাগম STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

স্থির অন্তঃস্থ bool is_dns_stub_hostname(ধ্রুব বর্ণ *hostname) {
        প্রত্যাগম STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

স্থির অন্তঃস্থ bool is_dns_proxy_stub_hostname(ধ্রুব বর্ণ *hostname) {
        প্রত্যাগম STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ধ্রুব বর্ণ* etc_hostname(শূন্য);
ধ্রুব বর্ণ* etc_machine_info(শূন্য);

পূর্ণাঙ্ক get_pretty_hostname(বর্ণ **ret);

পূর্ণাঙ্ক machine_spec_valid(ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক split_user_at_host(ধ্রুব বর্ণ *s, বর্ণ **ret_user, বর্ণ **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ধ্রুব বর্ণ *s);
পূর্ণাঙ্ক machine_tag_list_is_valid(বর্ণ **l);
পূর্ণাঙ্ক machine_tags_from_string(ধ্রুব বর্ণ *s, bool graceful, বর্ণ ***ret);
পূর্ণাঙ্ক machine_tags_from_strv(বর্ণ **l, bool graceful, বর্ণ ***ret);
