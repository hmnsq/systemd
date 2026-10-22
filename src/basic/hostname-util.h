/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

ৱর্ণ* get_default_hostname_raw(শূন্য);

bool valid_ldh_char(ৱর্ণ c) _const_;

প্রকারনির্দেশ গণনা ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ধ্রুৱ ৱর্ণ *s, ValidHostnameFlags flags) _pure_;
ৱর্ণ* hostname_cleanup(ৱর্ণ *s);

bool is_localhost(ধ্রুৱ ৱর্ণ *hostname);

স্থির অন্তঃস্থ bool is_gateway_hostname(ধ্রুৱ ৱর্ণ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ঘূৰাই STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

স্থির অন্তঃস্থ bool is_outbound_hostname(ধ্রুৱ ৱর্ণ *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ঘূৰাই STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

স্থির অন্তঃস্থ bool is_dns_stub_hostname(ধ্রুৱ ৱর্ণ *hostname) {
        ঘূৰাই STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

স্থির অন্তঃস্থ bool is_dns_proxy_stub_hostname(ধ্রুৱ ৱর্ণ *hostname) {
        ঘূৰাই STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ধ্রুৱ ৱর্ণ* etc_hostname(শূন্য);
ধ্রুৱ ৱর্ণ* etc_machine_info(শূন্য);

পূর্ণাঙ্ক get_pretty_hostname(ৱর্ণ **ret);

পূর্ণাঙ্ক machine_spec_valid(ধ্রুৱ ৱর্ণ *s);
পূর্ণাঙ্ক split_user_at_host(ধ্রুৱ ৱর্ণ *s, ৱর্ণ **ret_user, ৱর্ণ **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ধ্রুৱ ৱর্ণ *s);
পূর্ণাঙ্ক machine_tag_list_is_valid(ৱর্ণ **l);
পূর্ণাঙ্ক machine_tags_from_string(ধ্রুৱ ৱর্ণ *s, bool graceful, ৱর্ণ ***ret);
পূর্ণাঙ্ক machine_tags_from_strv(ৱর্ণ **l, bool graceful, ৱর্ণ ***ret);
