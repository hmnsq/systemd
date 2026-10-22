/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

ଅକ୍ଷର* get_default_hostname_raw(ଶୂନ୍ୟ);

bool valid_ldh_char(ଅକ୍ଷର c) _const_;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ଅଚଳ ଅକ୍ଷର *s, ValidHostnameFlags flags) _pure_;
ଅକ୍ଷର* hostname_cleanup(ଅକ୍ଷର *s);

bool is_localhost(ଅଚଳ ଅକ୍ଷର *hostname);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool is_gateway_hostname(ଅଚଳ ଅକ୍ଷର *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ଫେରନ୍ତୁ STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool is_outbound_hostname(ଅଚଳ ଅକ୍ଷର *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ଫେରନ୍ତୁ STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool is_dns_stub_hostname(ଅଚଳ ଅକ୍ଷର *hostname) {
        ଫେରନ୍ତୁ STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool is_dns_proxy_stub_hostname(ଅଚଳ ଅକ୍ଷର *hostname) {
        ଫେରନ୍ତୁ STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ଅଚଳ ଅକ୍ଷର* etc_hostname(ଶୂନ୍ୟ);
ଅଚଳ ଅକ୍ଷର* etc_machine_info(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_pretty_hostname(ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_spec_valid(ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା split_user_at_host(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର **ret_user, ଅକ୍ଷର **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tag_list_is_valid(ଅକ୍ଷର **l);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tags_from_string(ଅଚଳ ଅକ୍ଷର *s, bool graceful, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା machine_tags_from_strv(ଅକ୍ଷର **l, bool graceful, ଅକ୍ଷର ***ret);
