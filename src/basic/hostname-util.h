/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

അക്ഷരം* get_default_hostname_raw(ശൂന്യം);

bool valid_ldh_char(അക്ഷരം c) _const_;

തരനിർവചനം എണ്ണൽ ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(സ്ഥിരം അക്ഷരം *s, ValidHostnameFlags flags) _pure_;
അക്ഷരം* hostname_cleanup(അക്ഷരം *s);

bool is_localhost(സ്ഥിരം അക്ഷരം *hostname);

സ്ഥിതം വരിയിൽ bool is_gateway_hostname(സ്ഥിരം അക്ഷരം *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        തിരികെ STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

സ്ഥിതം വരിയിൽ bool is_outbound_hostname(സ്ഥിരം അക്ഷരം *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        തിരികെ STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

സ്ഥിതം വരിയിൽ bool is_dns_stub_hostname(സ്ഥിരം അക്ഷരം *hostname) {
        തിരികെ STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

സ്ഥിതം വരിയിൽ bool is_dns_proxy_stub_hostname(സ്ഥിരം അക്ഷരം *hostname) {
        തിരികെ STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

സ്ഥിരം അക്ഷരം* etc_hostname(ശൂന്യം);
സ്ഥിരം അക്ഷരം* etc_machine_info(ശൂന്യം);

പൂർണ്ണസംഖ്യ get_pretty_hostname(അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ machine_spec_valid(സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ split_user_at_host(സ്ഥിരം അക്ഷരം *s, അക്ഷരം **ret_user, അക്ഷരം **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ machine_tag_list_is_valid(അക്ഷരം **l);
പൂർണ്ണസംഖ്യ machine_tags_from_string(സ്ഥിരം അക്ഷരം *s, bool graceful, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ machine_tags_from_strv(അക്ഷരം **l, bool graceful, അക്ഷരം ***ret);
