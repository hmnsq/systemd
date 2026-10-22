/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

struct iovec_wrapper {
        struct iovec *iovec;
        size_t count;
};

void iovw_done_free(struct iovec_wrapper *iovw);
void iovw_done(struct iovec_wrapper *iovw);

struct iovec_wrapper* iovw_free(struct iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct iovec_wrapper*, iovw_free);

struct iovec_wrapper* iovw_free_free(struct iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(struct iovec_wrapper*, iovw_free_free);

цяло iovw_compare(const struct iovec_wrapper *a, const struct iovec_wrapper *b) _pure_;
static inline bool iovw_equal(const struct iovec_wrapper *a, const struct iovec_wrapper *b) {
        върни iovw_compare(a, b) == 0;
}

цяло iovw_put_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static inline цяло iovw_put(struct iovec_wrapper *iovw, void *data, size_t len) {
        върни iovw_put_full(iovw, false, data, len);
}
цяло iovw_put_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov);
static inline цяло iovw_put_iov(struct iovec_wrapper *iovw, const struct iovec *iov) {
        върни iovw_put_iov_full(iovw, false, iov);
}
цяло iovw_put_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source);
static inline цяло iovw_put_iovw(struct iovec_wrapper *iovw, const struct iovec_wrapper *source) {
        върни iovw_put_iovw_full(iovw, false, source);
}
цяло iovw_consume_full(struct iovec_wrapper *iovw, bool accept_zero, void *data, size_t len);
static inline цяло iovw_consume(struct iovec_wrapper *iovw, void *data, size_t len) {
        върни iovw_consume_full(iovw, false, data, len);
}
цяло iovw_consume_iov_full(struct iovec_wrapper *iovw, bool accept_zero, struct iovec *iov);
static inline цяло iovw_consume_iov(struct iovec_wrapper *iovw, struct iovec *iov) {
        върни iovw_consume_iov_full(iovw, false, iov);
}
цяло iovw_extend_full(struct iovec_wrapper *iovw, bool accept_zero, const void *data, size_t len);
static inline цяло iovw_extend(struct iovec_wrapper *iovw, const void *data, size_t len) {
        върни iovw_extend_full(iovw, false, data, len);
}
цяло iovw_extend_iov_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec *iov);
static inline цяло iovw_extend_iov(struct iovec_wrapper *iovw, const struct iovec *iov) {
        върни iovw_extend_iov_full(iovw, false, iov);
}
цяло iovw_extend_iovw_full(struct iovec_wrapper *iovw, bool accept_zero, const struct iovec_wrapper *source);
static inline цяло iovw_extend_iovw(struct iovec_wrapper *iovw, const struct iovec_wrapper *source) {
        върни iovw_extend_iovw_full(iovw, false, source);
}

static inline bool iovw_isempty(const struct iovec_wrapper *iovw) {
        върни !iovw || iovw->count == 0;
}

цяло iovw_put_string_field_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *value);
static inline цяло iovw_put_string_field(struct iovec_wrapper *iovw, const char *field, const char *value) {
        върни iovw_put_string_field_full(iovw, false, field, value);
}
static inline цяло iovw_replace_string_field(struct iovec_wrapper *iovw, const char *field, const char *value) {
        върни iovw_put_string_field_full(iovw, true, field, value);
}
цяло iovw_put_string_fieldf_full(struct iovec_wrapper *iovw, bool replace, const char *field, const char *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
цяло iovw_put_string_field_free(struct iovec_wrapper *iovw, const char *field, char *value);
void iovw_rebase(struct iovec_wrapper *iovw, void *old, void *new);
size_t iovw_size(const struct iovec_wrapper *iovw);
цяло iovw_concat(const struct iovec_wrapper *iovw, struct iovec *ret);
char* iovw_to_cstring(const struct iovec_wrapper *iovw);

цяло iovec_split(const struct iovec *iov, size_t length_size, struct iovec_wrapper *ret);
цяло iovw_merge(const struct iovec_wrapper *iovw, size_t length_size, struct iovec *ret);
