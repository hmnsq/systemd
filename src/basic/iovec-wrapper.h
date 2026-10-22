/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

بنية iovec_wrapper {
        بنية iovec *iovec;
        size_t ڳڻپ;
};

فراغ iovw_پورو_free(بنية iovec_wrapper *iovw);
فراغ iovw_پورو(بنية iovec_wrapper *iovw);

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free);

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free_free);

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) خالص;
ساكن مضمن bool iovw_equal(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        أرجع iovw_compare(a, b) == 0;
}

صحيح iovw_put_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_put(بنية iovec_wrapper *iovw, فراغ *معلومات, size_t len) {
        أرجع iovw_put_ڀريل(iovw, false, معلومات, len);
}
صحيح iovw_put_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_put_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_put_iov_ڀريل(iovw, false, iov);
}
صحيح iovw_put_iovw_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *ذريعو);
ساكن مضمن صحيح iovw_put_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *ذريعو) {
        أرجع iovw_put_iovw_ڀريل(iovw, false, ذريعو);
}
صحيح iovw_consume_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_consume(بنية iovec_wrapper *iovw, فراغ *معلومات, size_t len) {
        أرجع iovw_consume_ڀريل(iovw, false, معلومات, len);
}
صحيح iovw_consume_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov);
ساكن مضمن صحيح iovw_consume_iov(بنية iovec_wrapper *iovw, بنية iovec *iov) {
        أرجع iovw_consume_iov_ڀريل(iovw, false, iov);
}
صحيح iovw_extend_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *معلومات, size_t len);
ساكن مضمن صحيح iovw_extend(بنية iovec_wrapper *iovw, ثابت فراغ *معلومات, size_t len) {
        أرجع iovw_extend_ڀريل(iovw, false, معلومات, len);
}
صحيح iovw_extend_iov_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_extend_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_extend_iov_ڀريل(iovw, false, iov);
}
صحيح iovw_extend_iovw_ڀريل(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *ذريعو);
ساكن مضمن صحيح iovw_extend_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *ذريعو) {
        أرجع iovw_extend_iovw_ڀريل(iovw, false, ذريعو);
}

ساكن مضمن bool iovw_isempty(ثابت بنية iovec_wrapper *iovw) {
        أرجع !iovw || iovw->ڳڻپ == 0;
}

صحيح iovw_put_string_ميدان_ڀريل(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ميدان, ثابت حرف *قدر);
ساكن مضمن صحيح iovw_put_string_ميدان(بنية iovec_wrapper *iovw, ثابت حرف *ميدان, ثابت حرف *قدر) {
        أرجع iovw_put_string_ميدان_ڀريل(iovw, false, ميدان, قدر);
}
ساكن مضمن صحيح iovw_replace_string_ميدان(بنية iovec_wrapper *iovw, ثابت حرف *ميدان, ثابت حرف *قدر) {
        أرجع iovw_put_string_ميدان_ڀريل(iovw, true, ميدان, قدر);
}
صحيح iovw_put_string_fieldf_ڀريل(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *ميدان, ثابت حرف *format, ...) _printf_(4, 5);
#وصف_ڪريو iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_ڀريل(iovw, false, __VA_ARGS__)
#وصف_ڪريو iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_ڀريل(iovw, true, __VA_ARGS__)
صحيح iovw_put_string_ميدان_free(بنية iovec_wrapper *iovw, ثابت حرف *ميدان, حرف *قدر);
فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new);
size_t iovw_سائيز(ثابت بنية iovec_wrapper *iovw);
صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret);
حرف* iovw_منزل_cstring(ثابت بنية iovec_wrapper *iovw);

صحيح iovec_ورهايو(ثابت بنية iovec *iov, size_t ڊيگهائي_سائيز, بنية iovec_wrapper *ret);
صحيح iovw_ملايو(ثابت بنية iovec_wrapper *iovw, size_t ڊيگهائي_سائيز, بنية iovec *ret);
