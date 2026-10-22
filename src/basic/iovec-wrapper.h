/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

সংরচনা iovec_wrapper {
        সংরচনা iovec *iovec;
        size_t গুনতি;
};

শূন্য iovw_done_free(সংরচনা iovec_wrapper *iovw);
শূন্য iovw_done(সংরচনা iovec_wrapper *iovw);

সংরচনা iovec_wrapper* iovw_free(সংরচনা iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(সংরচনা iovec_wrapper*, iovw_free);

সংরচনা iovec_wrapper* iovw_free_free(সংরচনা iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(সংরচনা iovec_wrapper*, iovw_free_free);

পূর্ণাঙ্ক iovw_compare(ধ্রুব সংরচনা iovec_wrapper *a, ধ্রুব সংরচনা iovec_wrapper *b) _pure_;
স্থির অন্তঃস্থ bool iovw_equal(ধ্রুব সংরচনা iovec_wrapper *a, ধ্রুব সংরচনা iovec_wrapper *b) {
        প্রত্যাগম iovw_compare(a, b) == 0;
}

পূর্ণাঙ্ক iovw_put_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, শূন্য *data, size_t len);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_put(সংরচনা iovec_wrapper *iovw, শূন্য *data, size_t len) {
        প্রত্যাগম iovw_put_full(iovw, false, data, len);
}
পূর্ণাঙ্ক iovw_put_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুব সংরচনা iovec *iov);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_put_iov(সংরচনা iovec_wrapper *iovw, ধ্রুব সংরচনা iovec *iov) {
        প্রত্যাগম iovw_put_iov_full(iovw, false, iov);
}
পূর্ণাঙ্ক iovw_put_iovw_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুব সংরচনা iovec_wrapper *source);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_put_iovw(সংরচনা iovec_wrapper *iovw, ধ্রুব সংরচনা iovec_wrapper *source) {
        প্রত্যাগম iovw_put_iovw_full(iovw, false, source);
}
পূর্ণাঙ্ক iovw_consume_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, শূন্য *data, size_t len);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_consume(সংরচনা iovec_wrapper *iovw, শূন্য *data, size_t len) {
        প্রত্যাগম iovw_consume_full(iovw, false, data, len);
}
পূর্ণাঙ্ক iovw_consume_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, সংরচনা iovec *iov);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_consume_iov(সংরচনা iovec_wrapper *iovw, সংরচনা iovec *iov) {
        প্রত্যাগম iovw_consume_iov_full(iovw, false, iov);
}
পূর্ণাঙ্ক iovw_extend_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুব শূন্য *data, size_t len);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_extend(সংরচনা iovec_wrapper *iovw, ধ্রুব শূন্য *data, size_t len) {
        প্রত্যাগম iovw_extend_full(iovw, false, data, len);
}
পূর্ণাঙ্ক iovw_extend_iov_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুব সংরচনা iovec *iov);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_extend_iov(সংরচনা iovec_wrapper *iovw, ধ্রুব সংরচনা iovec *iov) {
        প্রত্যাগম iovw_extend_iov_full(iovw, false, iov);
}
পূর্ণাঙ্ক iovw_extend_iovw_full(সংরচনা iovec_wrapper *iovw, bool accept_zero, ধ্রুব সংরচনা iovec_wrapper *source);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_extend_iovw(সংরচনা iovec_wrapper *iovw, ধ্রুব সংরচনা iovec_wrapper *source) {
        প্রত্যাগম iovw_extend_iovw_full(iovw, false, source);
}

স্থির অন্তঃস্থ bool iovw_isempty(ধ্রুব সংরচনা iovec_wrapper *iovw) {
        প্রত্যাগম !iovw || iovw->গুনতি == 0;
}

পূর্ণাঙ্ক iovw_put_string_field_full(সংরচনা iovec_wrapper *iovw, bool replace, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_put_string_field(সংরচনা iovec_wrapper *iovw, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *মান) {
        প্রত্যাগম iovw_put_string_field_full(iovw, false, field, মান);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক iovw_replace_string_field(সংরচনা iovec_wrapper *iovw, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *মান) {
        প্রত্যাগম iovw_put_string_field_full(iovw, true, field, মান);
}
পূর্ণাঙ্ক iovw_put_string_fieldf_full(সংরচনা iovec_wrapper *iovw, bool replace, ধ্রুব বর্ণ *field, ধ্রুব বর্ণ *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
পূর্ণাঙ্ক iovw_put_string_field_free(সংরচনা iovec_wrapper *iovw, ধ্রুব বর্ণ *field, বর্ণ *মান);
শূন্য iovw_rebase(সংরচনা iovec_wrapper *iovw, শূন্য *old, শূন্য *new);
size_t iovw_size(ধ্রুব সংরচনা iovec_wrapper *iovw);
পূর্ণাঙ্ক iovw_concat(ধ্রুব সংরচনা iovec_wrapper *iovw, সংরচনা iovec *ret);
বর্ণ* iovw_to_cstring(ধ্রুব সংরচনা iovec_wrapper *iovw);

পূর্ণাঙ্ক iovec_split(ধ্রুব সংরচনা iovec *iov, size_t length_size, সংরচনা iovec_wrapper *ret);
পূর্ণাঙ্ক iovw_merge(ধ্রুব সংরচনা iovec_wrapper *iovw, size_t length_size, সংরচনা iovec *ret);
