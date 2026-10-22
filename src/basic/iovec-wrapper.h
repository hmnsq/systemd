/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

بنية iovec_wrapper {
        بنية iovec *iovec;
        size_t گنتی;
};

فراغ iovw_پورا_free(بنية iovec_wrapper *iovw);
فراغ iovw_پورا(بنية iovec_wrapper *iovw);

بنية iovec_wrapper* iovw_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free);

بنية iovec_wrapper* iovw_free_free(بنية iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(بنية iovec_wrapper*, iovw_free_free);

صحيح iovw_compare(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) خالص;
ساكن مضمن bool iovw_equal(ثابت بنية iovec_wrapper *a, ثابت بنية iovec_wrapper *b) {
        أرجع iovw_compare(a, b) == 0;
}

صحيح iovw_put_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *انکڑے, size_t len);
ساكن مضمن صحيح iovw_put(بنية iovec_wrapper *iovw, فراغ *انکڑے, size_t len) {
        أرجع iovw_put_بھریا(iovw, false, انکڑے, len);
}
صحيح iovw_put_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_put_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_put_iov_بھریا(iovw, false, iov);
}
صحيح iovw_put_iovw_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سروت);
ساكن مضمن صحيح iovw_put_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *سروت) {
        أرجع iovw_put_iovw_بھریا(iovw, false, سروت);
}
صحيح iovw_consume_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, فراغ *انکڑے, size_t len);
ساكن مضمن صحيح iovw_consume(بنية iovec_wrapper *iovw, فراغ *انکڑے, size_t len) {
        أرجع iovw_consume_بھریا(iovw, false, انکڑے, len);
}
صحيح iovw_consume_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, بنية iovec *iov);
ساكن مضمن صحيح iovw_consume_iov(بنية iovec_wrapper *iovw, بنية iovec *iov) {
        أرجع iovw_consume_iov_بھریا(iovw, false, iov);
}
صحيح iovw_extend_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت فراغ *انکڑے, size_t len);
ساكن مضمن صحيح iovw_extend(بنية iovec_wrapper *iovw, ثابت فراغ *انکڑے, size_t len) {
        أرجع iovw_extend_بھریا(iovw, false, انکڑے, len);
}
صحيح iovw_extend_iov_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec *iov);
ساكن مضمن صحيح iovw_extend_iov(بنية iovec_wrapper *iovw, ثابت بنية iovec *iov) {
        أرجع iovw_extend_iov_بھریا(iovw, false, iov);
}
صحيح iovw_extend_iovw_بھریا(بنية iovec_wrapper *iovw, bool accept_zero, ثابت بنية iovec_wrapper *سروت);
ساكن مضمن صحيح iovw_extend_iovw(بنية iovec_wrapper *iovw, ثابت بنية iovec_wrapper *سروت) {
        أرجع iovw_extend_iovw_بھریا(iovw, false, سروت);
}

ساكن مضمن bool iovw_isempty(ثابت بنية iovec_wrapper *iovw) {
        أرجع !iovw || iovw->گنتی == 0;
}

صحيح iovw_put_string_میدان_بھریا(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *قدر);
ساكن مضمن صحيح iovw_put_string_میدان(بنية iovec_wrapper *iovw, ثابت حرف *میدان, ثابت حرف *قدر) {
        أرجع iovw_put_string_میدان_بھریا(iovw, false, میدان, قدر);
}
ساكن مضمن صحيح iovw_replace_string_میدان(بنية iovec_wrapper *iovw, ثابت حرف *میدان, ثابت حرف *قدر) {
        أرجع iovw_put_string_میدان_بھریا(iovw, true, میدان, قدر);
}
صحيح iovw_put_string_fieldf_بھریا(بنية iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *format, ...) _printf_(4, 5);
#تعریف_کرو iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_بھریا(iovw, false, __VA_ARGS__)
#تعریف_کرو iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_بھریا(iovw, true, __VA_ARGS__)
صحيح iovw_put_string_میدان_free(بنية iovec_wrapper *iovw, ثابت حرف *میدان, حرف *قدر);
فراغ iovw_rebase(بنية iovec_wrapper *iovw, فراغ *old, فراغ *new);
size_t iovw_ناپ(ثابت بنية iovec_wrapper *iovw);
صحيح iovw_concat(ثابت بنية iovec_wrapper *iovw, بنية iovec *ret);
حرف* iovw_منزل_cstring(ثابت بنية iovec_wrapper *iovw);

صحيح iovec_ونڈو(ثابت بنية iovec *iov, size_t لمائی_ناپ, بنية iovec_wrapper *ret);
صحيح iovw_ملاؤ(ثابت بنية iovec_wrapper *iovw, size_t لمائی_ناپ, بنية iovec *ret);
