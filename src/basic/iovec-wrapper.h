/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

ساختار iovec_wrapper {
        ساختار iovec *iovec;
        size_t شمار;
};

تهی iovw_done_free(ساختار iovec_wrapper *iovw);
تهی iovw_done(ساختار iovec_wrapper *iovw);

ساختار iovec_wrapper* iovw_free(ساختار iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساختار iovec_wrapper*, iovw_free);

ساختار iovec_wrapper* iovw_free_free(ساختار iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساختار iovec_wrapper*, iovw_free_free);

صحیح iovw_compare(ثابت ساختار iovec_wrapper *a, ثابت ساختار iovec_wrapper *b) _pure_;
ایستا درونخطی bool iovw_equal(ثابت ساختار iovec_wrapper *a, ثابت ساختار iovec_wrapper *b) {
        بازگشت iovw_compare(a, b) == 0;
}

صحیح iovw_put_full(ساختار iovec_wrapper *iovw, bool accept_zero, تهی *data, size_t len);
ایستا درونخطی صحیح iovw_put(ساختار iovec_wrapper *iovw, تهی *data, size_t len) {
        بازگشت iovw_put_full(iovw, false, data, len);
}
صحیح iovw_put_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec *iov);
ایستا درونخطی صحیح iovw_put_iov(ساختار iovec_wrapper *iovw, ثابت ساختار iovec *iov) {
        بازگشت iovw_put_iov_full(iovw, false, iov);
}
صحیح iovw_put_iovw_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec_wrapper *source);
ایستا درونخطی صحیح iovw_put_iovw(ساختار iovec_wrapper *iovw, ثابت ساختار iovec_wrapper *source) {
        بازگشت iovw_put_iovw_full(iovw, false, source);
}
صحیح iovw_consume_full(ساختار iovec_wrapper *iovw, bool accept_zero, تهی *data, size_t len);
ایستا درونخطی صحیح iovw_consume(ساختار iovec_wrapper *iovw, تهی *data, size_t len) {
        بازگشت iovw_consume_full(iovw, false, data, len);
}
صحیح iovw_consume_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ساختار iovec *iov);
ایستا درونخطی صحیح iovw_consume_iov(ساختار iovec_wrapper *iovw, ساختار iovec *iov) {
        بازگشت iovw_consume_iov_full(iovw, false, iov);
}
صحیح iovw_extend_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت تهی *data, size_t len);
ایستا درونخطی صحیح iovw_extend(ساختار iovec_wrapper *iovw, ثابت تهی *data, size_t len) {
        بازگشت iovw_extend_full(iovw, false, data, len);
}
صحیح iovw_extend_iov_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec *iov);
ایستا درونخطی صحیح iovw_extend_iov(ساختار iovec_wrapper *iovw, ثابت ساختار iovec *iov) {
        بازگشت iovw_extend_iov_full(iovw, false, iov);
}
صحیح iovw_extend_iovw_full(ساختار iovec_wrapper *iovw, bool accept_zero, ثابت ساختار iovec_wrapper *source);
ایستا درونخطی صحیح iovw_extend_iovw(ساختار iovec_wrapper *iovw, ثابت ساختار iovec_wrapper *source) {
        بازگشت iovw_extend_iovw_full(iovw, false, source);
}

ایستا درونخطی bool iovw_isempty(ثابت ساختار iovec_wrapper *iovw) {
        بازگشت !iovw || iovw->شمار == 0;
}

صحیح iovw_put_string_field_full(ساختار iovec_wrapper *iovw, bool replace, ثابت نویسه *field, ثابت نویسه *مقدار);
ایستا درونخطی صحیح iovw_put_string_field(ساختار iovec_wrapper *iovw, ثابت نویسه *field, ثابت نویسه *مقدار) {
        بازگشت iovw_put_string_field_full(iovw, false, field, مقدار);
}
ایستا درونخطی صحیح iovw_replace_string_field(ساختار iovec_wrapper *iovw, ثابت نویسه *field, ثابت نویسه *مقدار) {
        بازگشت iovw_put_string_field_full(iovw, true, field, مقدار);
}
صحیح iovw_put_string_fieldf_full(ساختار iovec_wrapper *iovw, bool replace, ثابت نویسه *field, ثابت نویسه *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
صحیح iovw_put_string_field_free(ساختار iovec_wrapper *iovw, ثابت نویسه *field, نویسه *مقدار);
تهی iovw_rebase(ساختار iovec_wrapper *iovw, تهی *old, تهی *new);
size_t iovw_size(ثابت ساختار iovec_wrapper *iovw);
صحیح iovw_concat(ثابت ساختار iovec_wrapper *iovw, ساختار iovec *ret);
نویسه* iovw_to_cstring(ثابت ساختار iovec_wrapper *iovw);

صحیح iovec_split(ثابت ساختار iovec *iov, size_t length_size, ساختار iovec_wrapper *ret);
صحیح iovw_merge(ثابت ساختار iovec_wrapper *iovw, size_t length_size, ساختار iovec *ret);
