/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

بنية iovec_wrapper {
        بنية iovec *iovec;
        size_t سانى;
};

فراغ iovw_done_free(بنية iovec_wrapper *iovw);
فراغ iovw_done(بنية iovec_wrapper *iovw);

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free);

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free_free);

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) _pure_;
ساكن مضمن bool iovw_equal(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        أرجع iovw_compare(a, b) == 0;
}

صحيح iovw_put_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len);
ساكن مضمن صحيح iovw_put(بنية iovec_wrapper *iovw, فراغ *data, size_t len) {
        أرجع iovw_put_full(iovw, false, data, len);
}
صحيح iovw_put_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_put_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_put_iov_full(iovw, false, iov);
}
صحيح iovw_put_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source);
ساكن مضمن صحيح iovw_put_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *source) {
        أرجع iovw_put_iovw_full(iovw, false, source);
}
صحيح iovw_consume_full(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *data, size_t len);
ساكن مضمن صحيح iovw_consume(بنية iovec_wrapper *iovw, فراغ *data, size_t len) {
        أرجع iovw_consume_full(iovw, false, data, len);
}
صحيح iovw_consume_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov);
ساكن مضمن صحيح iovw_consume_iov(بنية iovec_wrapper *iovw, بنية iovec *iov) {
        أرجع iovw_consume_iov_full(iovw, false, iov);
}
صحيح iovw_extend_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *data, size_t len);
ساكن مضمن صحيح iovw_extend(بنية iovec_wrapper *iovw, ثابت فراغ *data, size_t len) {
        أرجع iovw_extend_full(iovw, false, data, len);
}
صحيح iovw_extend_iov_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_extend_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_extend_iov_full(iovw, false, iov);
}
صحيح iovw_extend_iovw_full(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *source);
ساكن مضمن صحيح iovw_extend_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *source) {
        أرجع iovw_extend_iovw_full(iovw, false, source);
}

ساكن مضمن bool iovw_isempty(ثابت بنية iovec_wrapper *iovw) {
        أرجع !iovw || iovw->سانى == 0;
}

صحيح iovw_put_string_field_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *قىممەت);
ساكن مضمن صحيح iovw_put_string_field(بنية iovec_wrapper *iovw, ثابت حرف *field, ثابت حرف *قىممەت) {
        أرجع iovw_put_string_field_full(iovw, false, field, قىممەت);
}
ساكن مضمن صحيح iovw_replace_string_field(بنية iovec_wrapper *iovw, ثابت حرف *field, ثابت حرف *قىممەت) {
        أرجع iovw_put_string_field_full(iovw, true, field, قىممەت);
}
صحيح iovw_put_string_fieldf_full(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *field, ثابت حرف *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
صحيح iovw_put_string_field_free(بنية iovec_wrapper *iovw, ثابت حرف *field, حرف *قىممەت);
فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new);
size_t iovw_size(ثابت بنية iovec_wrapper *iovw);
صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret);
حرف* iovw_to_cstring(ثابت بنية iovec_wrapper *iovw);

صحيح iovec_split(ثابت بنية iovec *iov, size_t length_size, بنية iovec_wrapper *ret);
صحيح iovw_merge(ثابت بنية iovec_wrapper *iovw, size_t length_size, بنية iovec *ret);
