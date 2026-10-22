/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

ഘടന iovec_wrapper {
        ഘടന iovec *iovec;
        size_t എണ്ണം;
};

ശൂന്യം iovw_done_free(ഘടന iovec_wrapper *iovw);
ശൂന്യം iovw_done(ഘടന iovec_wrapper *iovw);

ഘടന iovec_wrapper* iovw_free(ഘടന iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ഘടന iovec_wrapper*, iovw_free);

ഘടന iovec_wrapper* iovw_free_free(ഘടന iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ഘടന iovec_wrapper*, iovw_free_free);

പൂർണ്ണസംഖ്യ iovw_compare(സ്ഥിരം ഘടന iovec_wrapper *a, സ്ഥിരം ഘടന iovec_wrapper *b) _pure_;
സ്ഥിതം വരിയിൽ bool iovw_equal(സ്ഥിരം ഘടന iovec_wrapper *a, സ്ഥിരം ഘടന iovec_wrapper *b) {
        തിരികെ iovw_compare(a, b) == 0;
}

പൂർണ്ണസംഖ്യ iovw_put_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ശൂന്യം *data, size_t len);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_put(ഘടന iovec_wrapper *iovw, ശൂന്യം *data, size_t len) {
        തിരികെ iovw_put_full(iovw, false, data, len);
}
പൂർണ്ണസംഖ്യ iovw_put_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec *iov);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_put_iov(ഘടന iovec_wrapper *iovw, സ്ഥിരം ഘടന iovec *iov) {
        തിരികെ iovw_put_iov_full(iovw, false, iov);
}
പൂർണ്ണസംഖ്യ iovw_put_iovw_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec_wrapper *source);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_put_iovw(ഘടന iovec_wrapper *iovw, സ്ഥിരം ഘടന iovec_wrapper *source) {
        തിരികെ iovw_put_iovw_full(iovw, false, source);
}
പൂർണ്ണസംഖ്യ iovw_consume_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ശൂന്യം *data, size_t len);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_consume(ഘടന iovec_wrapper *iovw, ശൂന്യം *data, size_t len) {
        തിരികെ iovw_consume_full(iovw, false, data, len);
}
പൂർണ്ണസംഖ്യ iovw_consume_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ഘടന iovec *iov);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_consume_iov(ഘടന iovec_wrapper *iovw, ഘടന iovec *iov) {
        തിരികെ iovw_consume_iov_full(iovw, false, iov);
}
പൂർണ്ണസംഖ്യ iovw_extend_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ശൂന്യം *data, size_t len);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_extend(ഘടന iovec_wrapper *iovw, സ്ഥിരം ശൂന്യം *data, size_t len) {
        തിരികെ iovw_extend_full(iovw, false, data, len);
}
പൂർണ്ണസംഖ്യ iovw_extend_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec *iov);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_extend_iov(ഘടന iovec_wrapper *iovw, സ്ഥിരം ഘടന iovec *iov) {
        തിരികെ iovw_extend_iov_full(iovw, false, iov);
}
പൂർണ്ണസംഖ്യ iovw_extend_iovw_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec_wrapper *source);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_extend_iovw(ഘടന iovec_wrapper *iovw, സ്ഥിരം ഘടന iovec_wrapper *source) {
        തിരികെ iovw_extend_iovw_full(iovw, false, source);
}

സ്ഥിതം വരിയിൽ bool iovw_isempty(സ്ഥിരം ഘടന iovec_wrapper *iovw) {
        തിരികെ !iovw || iovw->എണ്ണം == 0;
}

പൂർണ്ണസംഖ്യ iovw_put_string_field_full(ഘടന iovec_wrapper *iovw, bool replace, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_put_string_field(ഘടന iovec_wrapper *iovw, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        തിരികെ iovw_put_string_field_full(iovw, false, field, മൂല്യം);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ iovw_replace_string_field(ഘടന iovec_wrapper *iovw, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        തിരികെ iovw_put_string_field_full(iovw, true, field, മൂല്യം);
}
പൂർണ്ണസംഖ്യ iovw_put_string_fieldf_full(ഘടന iovec_wrapper *iovw, bool replace, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
പൂർണ്ണസംഖ്യ iovw_put_string_field_free(ഘടന iovec_wrapper *iovw, സ്ഥിരം അക്ഷരം *field, അക്ഷരം *മൂല്യം);
ശൂന്യം iovw_rebase(ഘടന iovec_wrapper *iovw, ശൂന്യം *old, ശൂന്യം *new);
size_t iovw_size(സ്ഥിരം ഘടന iovec_wrapper *iovw);
പൂർണ്ണസംഖ്യ iovw_concat(സ്ഥിരം ഘടന iovec_wrapper *iovw, ഘടന iovec *ret);
അക്ഷരം* iovw_to_cstring(സ്ഥിരം ഘടന iovec_wrapper *iovw);

പൂർണ്ണസംഖ്യ iovec_split(സ്ഥിരം ഘടന iovec *iov, size_t length_size, ഘടന iovec_wrapper *ret);
പൂർണ്ണസംഖ്യ iovw_merge(സ്ഥിരം ഘടന iovec_wrapper *iovw, size_t length_size, ഘടന iovec *ret);
