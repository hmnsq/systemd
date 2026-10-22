/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

sa_nracanaa iovec_wrapper {
        sa_nracanaa iovec *iovec;
        size_t count;
};

void iovw_done_free(sa_nracanaa iovec_wrapper *iovw);
void iovw_done(sa_nracanaa iovec_wrapper *iovw);

sa_nracanaa iovec_wrapper* iovw_free(sa_nracanaa iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(sa_nracanaa iovec_wrapper*, iovw_free);

sa_nracanaa iovec_wrapper* iovw_free_free(sa_nracanaa iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(sa_nracanaa iovec_wrapper*, iovw_free_free);

puur_naa_nka iovw_compare(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) _pure_;
static anta_hstha bool iovw_equal(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) {
        ᯀᯮᯞ iovw_compare(a, b) == 0;
}

puur_naa_nka iovw_put_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static anta_hstha puur_naa_nka iovw_put(sa_nracanaa iovec_wrapper *iovw, void *data, size_t len) {
        ᯀᯮᯞ iovw_put_full(iovw, false, data, len);
}
puur_naa_nka iovw_put_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov);
static anta_hstha puur_naa_nka iovw_put_iov(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec *iov) {
        ᯀᯮᯞ iovw_put_iov_full(iovw, false, iov);
}
puur_naa_nka iovw_put_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source);
static anta_hstha puur_naa_nka iovw_put_iovw(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec_wrapper *source) {
        ᯀᯮᯞ iovw_put_iovw_full(iovw, false, source);
}
puur_naa_nka iovw_consume_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static anta_hstha puur_naa_nka iovw_consume(sa_nracanaa iovec_wrapper *iovw, void *data, size_t len) {
        ᯀᯮᯞ iovw_consume_full(iovw, false, data, len);
}
puur_naa_nka iovw_consume_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, sa_nracanaa iovec *iov);
static anta_hstha puur_naa_nka iovw_consume_iov(sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *iov) {
        ᯀᯮᯞ iovw_consume_iov_full(iovw, false, iov);
}
puur_naa_nka iovw_extend_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len);
static anta_hstha puur_naa_nka iovw_extend(sa_nracanaa iovec_wrapper *iovw, const void *data, size_t len) {
        ᯀᯮᯞ iovw_extend_full(iovw, false, data, len);
}
puur_naa_nka iovw_extend_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov);
static anta_hstha puur_naa_nka iovw_extend_iov(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec *iov) {
        ᯀᯮᯞ iovw_extend_iov_full(iovw, false, iov);
}
puur_naa_nka iovw_extend_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source);
static anta_hstha puur_naa_nka iovw_extend_iovw(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec_wrapper *source) {
        ᯀᯮᯞ iovw_extend_iovw_full(iovw, false, source);
}

static anta_hstha bool iovw_isempty(const sa_nracanaa iovec_wrapper *iovw) {
        ᯀᯮᯞ !iovw || iovw->count == 0;
}

puur_naa_nka iovw_put_string_field_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *value);
static anta_hstha puur_naa_nka iovw_put_string_field(sa_nracanaa iovec_wrapper *iovw, const var_na *field, const var_na *value) {
        ᯀᯮᯞ iovw_put_string_field_full(iovw, false, field, value);
}
static anta_hstha puur_naa_nka iovw_replace_string_field(sa_nracanaa iovec_wrapper *iovw, const var_na *field, const var_na *value) {
        ᯀᯮᯞ iovw_put_string_field_full(iovw, true, field, value);
}
puur_naa_nka iovw_put_string_fieldf_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
puur_naa_nka iovw_put_string_field_free(sa_nracanaa iovec_wrapper *iovw, const var_na *field, var_na *value);
void iovw_rebase(sa_nracanaa iovec_wrapper *iovw, void *old, void *new);
size_t iovw_size(const sa_nracanaa iovec_wrapper *iovw);
puur_naa_nka iovw_concat(const sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *ret);
var_na* iovw_to_cstring(const sa_nracanaa iovec_wrapper *iovw);

puur_naa_nka iovec_split(const sa_nracanaa iovec *iov, size_t length_size, sa_nracanaa iovec_wrapper *ret);
puur_naa_nka iovw_merge(const sa_nracanaa iovec_wrapper *iovw, size_t length_size, sa_nracanaa iovec *ret);
