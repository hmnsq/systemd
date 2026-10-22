/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

অক্ষর* get_default_hostname_raw(শূন্য);

bool valid_ldh_char(অক্ষর c) _const_;

প্রকারলেখ গণনা ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ধ্রুবক অক্ষর *s, ValidHostnameFlags flags) _pure_;
অক্ষর* hostname_cleanup(অক্ষর *s);

bool is_localhost(ধ্রুবক অক্ষর *hostname);

স্থির অন্তঃরেখ bool is_gateway_hostname(ধ্রুবক অক্ষর *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ফেরত STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

স্থির অন্তঃরেখ bool is_outbound_hostname(ধ্রুবক অক্ষর *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ফেরত STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

স্থির অন্তঃরেখ bool is_dns_stub_hostname(ধ্রুবক অক্ষর *hostname) {
        ফেরত STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

স্থির অন্তঃরেখ bool is_dns_proxy_stub_hostname(ধ্রুবক অক্ষর *hostname) {
        ফেরত STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ধ্রুবক অক্ষর* etc_hostname(শূন্য);
ধ্রুবক অক্ষর* etc_machine_info(শূন্য);

পূর্ণসংখ্যা get_pretty_hostname(অক্ষর **ret);

পূর্ণসংখ্যা machine_spec_valid(ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা split_user_at_host(ধ্রুবক অক্ষর *s, অক্ষর **ret_user, অক্ষর **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা machine_tag_list_is_valid(অক্ষর **l);
পূর্ণসংখ্যা machine_tags_from_string(ধ্রুবক অক্ষর *s, bool graceful, অক্ষর ***ret);
পূর্ণসংখ্যা machine_tags_from_strv(অক্ষর **l, bool graceful, অক্ষর ***ret);
