/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#నిర్వచించు LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

అక్షరం* get_default_hostname_raw(శూన్య);

bool valid_ldh_char(అక్షరం c) _const_;

రకనిర్వచనం గణన ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_మాట_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(స్థిరరాశి అక్షరం *s, ValidHostnameFlags flags) శుద్ధ;
అక్షరం* hostname_cleanup(అక్షరం *s);

bool is_localhost(స్థిరరాశి అక్షరం *hostname);

స్థిర అంతఃవరుస bool is_gateway_hostname(స్థిరరాశి అక్షరం *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        తిరిగి STRCASE_లోపల_సమితి(hostname, "_gateway", "_gateway.");
}

స్థిర అంతఃవరుస bool is_outbound_hostname(స్థిరరాశి అక్షరం *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        తిరిగి STRCASE_లోపల_సమితి(hostname, "_outbound", "_outbound.");
}

స్థిర అంతఃవరుస bool is_dns_stub_hostname(స్థిరరాశి అక్షరం *hostname) {
        తిరిగి STRCASE_లోపల_సమితి(hostname, "_localdnsstub", "_localdnsstub.");
}

స్థిర అంతఃవరుస bool is_dns_proxy_stub_hostname(స్థిరరాశి అక్షరం *hostname) {
        తిరిగి STRCASE_లోపల_సమితి(hostname, "_localdnsproxy", "_localdnsproxy.");
}

స్థిరరాశి అక్షరం* etc_hostname(శూన్య);
స్థిరరాశి అక్షరం* etc_machine_info(శూన్య);

పూర్ణాంకం get_pretty_hostname(అక్షరం **ret);

పూర్ణాంకం machine_spec_valid(స్థిరరాశి అక్షరం *s);
పూర్ణాంకం విభజించు_user_at_host(స్థిరరాశి అక్షరం *s, అక్షరం **ret_user, అక్షరం **ret_host);

#నిర్వచించు MACHINE_ట్యాగులు_MAX 1024U

bool machine_ట్యాగ్_is_valid(స్థిరరాశి అక్షరం *s);
పూర్ణాంకం machine_ట్యాగ్_list_is_valid(అక్షరం **l);
పూర్ణాంకం machine_ట్యాగులు_from_string(స్థిరరాశి అక్షరం *s, bool graceful, అక్షరం ***ret);
పూర్ణాంకం machine_ట్యాగులు_from_strv(అక్షరం **l, bool graceful, అక్షరం ***ret);
