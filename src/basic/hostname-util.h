/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

અક્ષર* get_default_hostname_raw(શૂન્ય);

bool valid_ldh_char(અક્ષર c) _const_;

પ્રકારવ્યાખ્યા ગણના ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(અચલ અક્ષર *s, ValidHostnameFlags flags) _pure_;
અક્ષર* hostname_cleanup(અક્ષર *s);

bool is_localhost(અચલ અક્ષર *hostname);

સ્થિર અંતઃરેખ bool is_gateway_hostname(અચલ અક્ષર *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        પાછા STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

સ્થિર અંતઃરેખ bool is_outbound_hostname(અચલ અક્ષર *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        પાછા STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

સ્થિર અંતઃરેખ bool is_dns_stub_hostname(અચલ અક્ષર *hostname) {
        પાછા STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

સ્થિર અંતઃરેખ bool is_dns_proxy_stub_hostname(અચલ અક્ષર *hostname) {
        પાછા STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

અચલ અક્ષર* etc_hostname(શૂન્ય);
અચલ અક્ષર* etc_machine_info(શૂન્ય);

પૂર્ણાંક get_pretty_hostname(અક્ષર **ret);

પૂર્ણાંક machine_spec_valid(અચલ અક્ષર *s);
પૂર્ણાંક split_user_at_host(અચલ અક્ષર *s, અક્ષર **ret_user, અક્ષર **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(અચલ અક્ષર *s);
પૂર્ણાંક machine_tag_list_is_valid(અક્ષર **l);
પૂર્ણાંક machine_tags_from_string(અચલ અક્ષર *s, bool graceful, અક્ષર ***ret);
પૂર્ણાંક machine_tags_from_strv(અક્ષર **l, bool graceful, અક્ષર ***ret);
