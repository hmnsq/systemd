/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

எழுத்து* get_default_hostname_raw(வெறுமை);

bool valid_ldh_char(எழுத்து c) _const_;

வகைவரை எண்ணிக்கை ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(மாறிலி எழுத்து *s, ValidHostnameFlags flags) _pure_;
எழுத்து* hostname_cleanup(எழுத்து *s);

bool is_localhost(மாறிலி எழுத்து *hostname);

நிலையான உள்வரி bool is_gateway_hostname(மாறிலி எழுத்து *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        திரும்பு STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

நிலையான உள்வரி bool is_outbound_hostname(மாறிலி எழுத்து *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        திரும்பு STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

நிலையான உள்வரி bool is_dns_stub_hostname(மாறிலி எழுத்து *hostname) {
        திரும்பு STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

நிலையான உள்வரி bool is_dns_proxy_stub_hostname(மாறிலி எழுத்து *hostname) {
        திரும்பு STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

மாறிலி எழுத்து* etc_hostname(வெறுமை);
மாறிலி எழுத்து* etc_machine_info(வெறுமை);

முழு get_pretty_hostname(எழுத்து **ret);

முழு machine_spec_valid(மாறிலி எழுத்து *s);
முழு split_user_at_host(மாறிலி எழுத்து *s, எழுத்து **ret_user, எழுத்து **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(மாறிலி எழுத்து *s);
முழு machine_tag_list_is_valid(எழுத்து **l);
முழு machine_tags_from_string(மாறிலி எழுத்து *s, bool graceful, எழுத்து ***ret);
முழு machine_tags_from_strv(எழுத்து **l, bool graceful, எழுத்து ***ret);
