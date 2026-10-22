/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

بنية iovec_wrapper {
        بنية iovec *iovec;
        size_t شمار;
};

فراغ iovw_انجامشده_free(بنية iovec_wrapper *iovw);
فراغ iovw_انجامشده(بنية iovec_wrapper *iovw);

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free);

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free_free);

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) خالص;
ساكن مضمن bool iovw_equal(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        أرجع iovw_compare(a, b) == 0;
}

صحيح iovw_put_پر(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *داده, size_t len);
ساكن مضمن صحيح iovw_put(بنية iovec_wrapper *iovw, فراغ *داده, size_t len) {
        أرجع iovw_put_پر(iovw, false, داده, len);
}
صحيح iovw_put_iov_پر(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_put_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_put_iov_پر(iovw, false, iov);
}
صحيح iovw_put_iovw_پر(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *منبع);
ساكن مضمن صحيح iovw_put_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *منبع) {
        أرجع iovw_put_iovw_پر(iovw, false, منبع);
}
صحيح iovw_consume_پر(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *داده, size_t len);
ساكن مضمن صحيح iovw_consume(بنية iovec_wrapper *iovw, فراغ *داده, size_t len) {
        أرجع iovw_consume_پر(iovw, false, داده, len);
}
صحيح iovw_consume_iov_پر(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov);
ساكن مضمن صحيح iovw_consume_iov(بنية iovec_wrapper *iovw, بنية iovec *iov) {
        أرجع iovw_consume_iov_پر(iovw, false, iov);
}
صحيح iovw_extend_پر(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *داده, size_t len);
ساكن مضمن صحيح iovw_extend(بنية iovec_wrapper *iovw, ثابت فراغ *داده, size_t len) {
        أرجع iovw_extend_پر(iovw, false, داده, len);
}
صحيح iovw_extend_iov_پر(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_extend_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_extend_iov_پر(iovw, false, iov);
}
صحيح iovw_extend_iovw_پر(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *منبع);
ساكن مضمن صحيح iovw_extend_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *منبع) {
        أرجع iovw_extend_iovw_پر(iovw, false, منبع);
}

ساكن مضمن bool iovw_isempty(ثابت بنية iovec_wrapper *iovw) {
        أرجع !iovw || iovw->شمار == 0;
}

صحيح iovw_put_string_فیلد_پر(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *فیلد, ثابت حرف *مقدار);
ساكن مضمن صحيح iovw_put_string_فیلد(بنية iovec_wrapper *iovw, ثابت حرف *فیلد, ثابت حرف *مقدار) {
        أرجع iovw_put_string_فیلد_پر(iovw, false, فیلد, مقدار);
}
ساكن مضمن صحيح iovw_replace_string_فیلد(بنية iovec_wrapper *iovw, ثابت حرف *فیلد, ثابت حرف *مقدار) {
        أرجع iovw_put_string_فیلد_پر(iovw, true, فیلد, مقدار);
}
صحيح iovw_put_string_fieldf_پر(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *فیلد, ثابت حرف *format, ...) _printf_(4, 5);
#تعریف_کن iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_پر(iovw, false, __VA_ARGS__)
#تعریف_کن iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_پر(iovw, true, __VA_ARGS__)
صحيح iovw_put_string_فیلد_free(بنية iovec_wrapper *iovw, ثابت حرف *فیلد, حرف *مقدار);
فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new);
size_t iovw_اندازه(ثابت بنية iovec_wrapper *iovw);
صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret);
حرف* iovw_مقصد_cstring(ثابت بنية iovec_wrapper *iovw);

صحيح iovec_تقسیم(ثابت بنية iovec *iov, size_t درازی_اندازه, بنية iovec_wrapper *ret);
صحيح iovw_ادغام(ثابت بنية iovec_wrapper *iovw, size_t درازی_اندازه, بنية iovec *ret);
