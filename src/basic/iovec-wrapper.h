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

ကိန်းပြည့် iovw_compare(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) _pure_;
static anta_hstha bool iovw_equal(const sa_nracanaa iovec_wrapper *a, const sa_nracanaa iovec_wrapper *b) {
        ပြန်ပေး iovw_compare(a, b) == 0;
}

ကိန်းပြည့် iovw_put_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static anta_hstha ကိန်းပြည့် iovw_put(sa_nracanaa iovec_wrapper *iovw, void *data, size_t len) {
        ပြန်ပေး iovw_put_full(iovw, false, data, len);
}
ကိန်းပြည့် iovw_put_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov);
static anta_hstha ကိန်းပြည့် iovw_put_iov(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec *iov) {
        ပြန်ပေး iovw_put_iov_full(iovw, false, iov);
}
ကိန်းပြည့် iovw_put_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source);
static anta_hstha ကိန်းပြည့် iovw_put_iovw(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec_wrapper *source) {
        ပြန်ပေး iovw_put_iovw_full(iovw, false, source);
}
ကိန်းပြည့် iovw_consume_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static anta_hstha ကိန်းပြည့် iovw_consume(sa_nracanaa iovec_wrapper *iovw, void *data, size_t len) {
        ပြန်ပေး iovw_consume_full(iovw, false, data, len);
}
ကိန်းပြည့် iovw_consume_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, sa_nracanaa iovec *iov);
static anta_hstha ကိန်းပြည့် iovw_consume_iov(sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *iov) {
        ပြန်ပေး iovw_consume_iov_full(iovw, false, iov);
}
ကိန်းပြည့် iovw_extend_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len);
static anta_hstha ကိန်းပြည့် iovw_extend(sa_nracanaa iovec_wrapper *iovw, const void *data, size_t len) {
        ပြန်ပေး iovw_extend_full(iovw, false, data, len);
}
ကိန်းပြည့် iovw_extend_iov_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec *iov);
static anta_hstha ကိန်းပြည့် iovw_extend_iov(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec *iov) {
        ပြန်ပေး iovw_extend_iov_full(iovw, false, iov);
}
ကိန်းပြည့် iovw_extend_iovw_full(sa_nracanaa iovec_wrapper *iovw, bool accept_zero, const sa_nracanaa iovec_wrapper *source);
static anta_hstha ကိန်းပြည့် iovw_extend_iovw(sa_nracanaa iovec_wrapper *iovw, const sa_nracanaa iovec_wrapper *source) {
        ပြန်ပေး iovw_extend_iovw_full(iovw, false, source);
}

static anta_hstha bool iovw_isempty(const sa_nracanaa iovec_wrapper *iovw) {
        ပြန်ပေး !iovw || iovw->count == 0;
}

ကိန်းပြည့် iovw_put_string_field_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *value);
static anta_hstha ကိန်းပြည့် iovw_put_string_field(sa_nracanaa iovec_wrapper *iovw, const var_na *field, const var_na *value) {
        ပြန်ပေး iovw_put_string_field_full(iovw, false, field, value);
}
static anta_hstha ကိန်းပြည့် iovw_replace_string_field(sa_nracanaa iovec_wrapper *iovw, const var_na *field, const var_na *value) {
        ပြန်ပေး iovw_put_string_field_full(iovw, true, field, value);
}
ကိန်းပြည့် iovw_put_string_fieldf_full(sa_nracanaa iovec_wrapper *iovw, bool replace, const var_na *field, const var_na *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
ကိန်းပြည့် iovw_put_string_field_free(sa_nracanaa iovec_wrapper *iovw, const var_na *field, var_na *value);
void iovw_rebase(sa_nracanaa iovec_wrapper *iovw, void *old, void *new);
size_t iovw_size(const sa_nracanaa iovec_wrapper *iovw);
ကိန်းပြည့် iovw_concat(const sa_nracanaa iovec_wrapper *iovw, sa_nracanaa iovec *ret);
var_na* iovw_to_cstring(const sa_nracanaa iovec_wrapper *iovw);

ကိန်းပြည့် iovec_split(const sa_nracanaa iovec *iov, size_t length_size, sa_nracanaa iovec_wrapper *ret);
ကိန်းပြည့် iovw_merge(const sa_nracanaa iovec_wrapper *iovw, size_t length_size, sa_nracanaa iovec *ret);
