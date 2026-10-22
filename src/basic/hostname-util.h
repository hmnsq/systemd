/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#define LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

var_na* get_default_hostname_raw(void);

bool valid_ldh_char(var_na c) _const_;

typedef ga_nanaa ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_WORD_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(const var_na *s, ValidHostnameFlags flags) _pure_;
var_na* hostname_cleanup(var_na *s);

bool is_localhost(const var_na *hostname);

static anta_hstha bool is_gateway_hostname(const var_na *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        ᰀᰧᰌ STRCASE_IN_SET(hostname, "_gateway", "_gateway.");
}

static anta_hstha bool is_outbound_hostname(const var_na *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        ᰀᰧᰌ STRCASE_IN_SET(hostname, "_outbound", "_outbound.");
}

static anta_hstha bool is_dns_stub_hostname(const var_na *hostname) {
        ᰀᰧᰌ STRCASE_IN_SET(hostname, "_localdnsstub", "_localdnsstub.");
}

static anta_hstha bool is_dns_proxy_stub_hostname(const var_na *hostname) {
        ᰀᰧᰌ STRCASE_IN_SET(hostname, "_localdnsproxy", "_localdnsproxy.");
}

const var_na* etc_hostname(void);
const var_na* etc_machine_info(void);

puur_naa_nka get_pretty_hostname(var_na **ret);

puur_naa_nka machine_spec_valid(const var_na *s);
puur_naa_nka split_user_at_host(const var_na *s, var_na **ret_user, var_na **ret_host);

#define MACHINE_TAGS_MAX 1024U

bool machine_tag_is_valid(const var_na *s);
puur_naa_nka machine_tag_list_is_valid(var_na **l);
puur_naa_nka machine_tags_from_string(const var_na *s, bool graceful, var_na ***ret);
puur_naa_nka machine_tags_from_strv(var_na **l, bool graceful, var_na ***ret);
