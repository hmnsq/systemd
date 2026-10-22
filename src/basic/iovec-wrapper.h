/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

بنية iovec_wrapper {
        بنية iovec *iovec;
        size_t شمېر;
};

فراغ iovw_بشپړ_free(بنية iovec_wrapper *iovw);
فراغ iovw_بشپړ(بنية iovec_wrapper *iovw);

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free);

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free_free);

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) خالص;
ساكن مضمن bool iovw_equal(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        أرجع iovw_compare(a, b) == 0;
}

صحيح iovw_put_ډک(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_put(بنية iovec_wrapper *iovw, فراغ *معلومات, size_t len) {
        أرجع iovw_put_ډک(iovw, false, معلومات, len);
}
صحيح iovw_put_iov_ډک(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_put_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_put_iov_ډک(iovw, false, iov);
}
صحيح iovw_put_iovw_ډک(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سرچینه);
ساكن مضمن صحيح iovw_put_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *سرچینه) {
        أرجع iovw_put_iovw_ډک(iovw, false, سرچینه);
}
صحيح iovw_consume_ډک(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_consume(بنية iovec_wrapper *iovw, فراغ *معلومات, size_t len) {
        أرجع iovw_consume_ډک(iovw, false, معلومات, len);
}
صحيح iovw_consume_iov_ډک(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov);
ساكن مضمن صحيح iovw_consume_iov(بنية iovec_wrapper *iovw, بنية iovec *iov) {
        أرجع iovw_consume_iov_ډک(iovw, false, iov);
}
صحيح iovw_extend_ډک(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_extend(بنية iovec_wrapper *iovw, ثابت فراغ *معلومات, size_t len) {
        أرجع iovw_extend_ډک(iovw, false, معلومات, len);
}
صحيح iovw_extend_iov_ډک(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_extend_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_extend_iov_ډک(iovw, false, iov);
}
صحيح iovw_extend_iovw_ډک(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سرچینه);
ساكن مضمن صحيح iovw_extend_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *سرچینه) {
        أرجع iovw_extend_iovw_ډک(iovw, false, سرچینه);
}

ساكن مضمن bool iovw_isempty(ثابت بنية iovec_wrapper *iovw) {
        أرجع !iovw || iovw->شمېر == 0;
}

صحيح iovw_put_string_ډګر_ډک(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ډګر, ثابت حرف *ارزښت);
ساكن مضمن صحيح iovw_put_string_ډګر(بنية iovec_wrapper *iovw, ثابت حرف *ډګر, ثابت حرف *ارزښت) {
        أرجع iovw_put_string_ډګر_ډک(iovw, false, ډګر, ارزښت);
}
ساكن مضمن صحيح iovw_replace_string_ډګر(بنية iovec_wrapper *iovw, ثابت حرف *ډګر, ثابت حرف *ارزښت) {
        أرجع iovw_put_string_ډګر_ډک(iovw, true, ډګر, ارزښت);
}
صحيح iovw_put_string_fieldf_ډک(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ډګر, ثابت حرف *format, ...) _printf_(4, 5);
#تعریف_کړه iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_ډک(iovw, false, __VA_ARGS__)
#تعریف_کړه iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_ډک(iovw, true, __VA_ARGS__)
صحيح iovw_put_string_ډګر_free(بنية iovec_wrapper *iovw, ثابت حرف *ډګر, حرف *ارزښت);
فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new);
size_t iovw_اندازه(ثابت بنية iovec_wrapper *iovw);
صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret);
حرف* iovw_منزل_cstring(ثابت بنية iovec_wrapper *iovw);

صحيح iovec_وېشل(ثابت بنية iovec *iov, size_t اوږدتیا_اندازه, بنية iovec_wrapper *ret);
صحيح iovw_یوځای(ثابت بنية iovec_wrapper *iovw, size_t اوږدتیا_اندازه, بنية iovec *ret);
