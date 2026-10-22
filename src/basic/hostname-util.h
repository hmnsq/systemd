/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#تعریف_کرو LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

حرف* get_default_hostname_raw(خالی);

bool valid_ldh_char(حرف c) _const_;

قسمتعریف شمار ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_لفظ_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ثابت حرف *s, ValidHostnameFlags flags) خالص;
حرف* hostname_cleanup(حرف *s);

bool is_localhost(ثابت حرف *hostname);

ساکن اندرونی bool is_gateway_hostname(ثابت حرف *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        واپس STRCASE_اندر_سیٹ(hostname, "_gateway", "_gateway.");
}

ساکن اندرونی bool is_outbound_hostname(ثابت حرف *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        واپس STRCASE_اندر_سیٹ(hostname, "_outbound", "_outbound.");
}

ساکن اندرونی bool is_dns_stub_hostname(ثابت حرف *hostname) {
        واپس STRCASE_اندر_سیٹ(hostname, "_localdnsstub", "_localdnsstub.");
}

ساکن اندرونی bool is_dns_proxy_stub_hostname(ثابت حرف *hostname) {
        واپس STRCASE_اندر_سیٹ(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ثابت حرف* etc_hostname(خالی);
ثابت حرف* etc_machine_info(خالی);

صحیح get_pretty_hostname(حرف **ret);

صحیح machine_spec_valid(ثابت حرف *s);
صحیح بانٹو_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host);

#تعریف_کرو MACHINE_لیبل_فہرست_MAX 1024U

bool machine_لیبل_is_valid(ثابت حرف *s);
صحیح machine_لیبل_list_is_valid(حرف **l);
صحیح machine_لیبل_فہرست_from_string(ثابت حرف *s, bool graceful, حرف ***ret);
صحیح machine_لیبل_فہرست_from_strv(حرف **l, bool graceful, حرف ***ret);
