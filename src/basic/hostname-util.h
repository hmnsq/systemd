/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "strv.h"

/* HOST_NAME_MAX should be 64 on linux, but musl uses the one by POSIX (255). */
#تعریف_کن LINUX_HOST_NAME_MAX CONST_MIN((size_t) HOST_NAME_MAX, (size_t) 64)

حرف* get_default_hostname_raw(فراغ);

bool valid_ldh_char(حرف c) _const_;

تعريفنوع تعداد ValidHostnameFlags {
        VALID_HOSTNAME_TRAILING_DOT  = 1 << 0,   /* Accept trailing dot on multi-label names */
        VALID_HOSTNAME_DOT_HOST      = 1 << 1,   /* Accept ".host" as valid hostname */
        VALID_HOSTNAME_QUESTION_MARK = 1 << 2,   /* Accept "?" as place holder for hashed machine ID value */
        VALID_HOSTNAME_کلمه_TOKEN    = 1 << 3,   /* Accept "$" as place holder for a word list substitution */
} ValidHostnameFlags;

bool hostname_is_valid(ثابت حرف *s, ValidHostnameFlags flags) خالص;
حرف* hostname_cleanup(حرف *s);

bool is_localhost(ثابت حرف *hostname);

ساكن مضمن bool is_gateway_hostname(ثابت حرف *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "gateway" host. */
        أرجع STRCASE_درون_مجموعه(hostname, "_gateway", "_gateway.");
}

ساكن مضمن bool is_outbound_hostname(ثابت حرف *hostname) {
        /* This tries to identify the valid syntaxes for the our synthetic "outbound" host. */
        أرجع STRCASE_درون_مجموعه(hostname, "_outbound", "_outbound.");
}

ساكن مضمن bool is_dns_stub_hostname(ثابت حرف *hostname) {
        أرجع STRCASE_درون_مجموعه(hostname, "_localdnsstub", "_localdnsstub.");
}

ساكن مضمن bool is_dns_proxy_stub_hostname(ثابت حرف *hostname) {
        أرجع STRCASE_درون_مجموعه(hostname, "_localdnsproxy", "_localdnsproxy.");
}

ثابت حرف* etc_hostname(فراغ);
ثابت حرف* etc_machine_info(فراغ);

صحيح get_pretty_hostname(حرف **ret);

صحيح machine_spec_valid(ثابت حرف *s);
صحيح تقسیم_user_at_host(ثابت حرف *s, حرف **ret_user, حرف **ret_host);

#تعریف_کن MACHINE_برچسبها_MAX 1024U

bool machine_برچسب_is_valid(ثابت حرف *s);
صحيح machine_برچسب_list_is_valid(حرف **l);
صحيح machine_برچسبها_from_string(ثابت حرف *s, bool graceful, حرف ***ret);
صحيح machine_برچسبها_from_strv(حرف **l, bool graceful, حرف ***ret);
