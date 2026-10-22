/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

ساخت iovec_wrapper {
        ساخت iovec *iovec;
        size_t گنتی;
};

خالی iovw_مکمل_free(ساخت iovec_wrapper *iovw);
خالی iovw_مکمل(ساخت iovec_wrapper *iovw);

ساخت iovec_wrapper* iovw_free(ساخت iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساخت iovec_wrapper*, iovw_free);

ساخت iovec_wrapper* iovw_free_free(ساخت iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ساخت iovec_wrapper*, iovw_free_free);

صحیح iovw_compare(ثابت ساخت iovec_wrapper *a, ثابت ساخت iovec_wrapper *b) خالص;
ساکن اندرونی bool iovw_equal(ثابت ساخت iovec_wrapper *a, ثابت ساخت iovec_wrapper *b) {
        واپس iovw_compare(a, b) == 0;
}

صحیح iovw_put_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, خالی *ڈیٹا, size_t len);
ساکن اندرونی صحیح iovw_put(ساخت iovec_wrapper *iovw, خالی *ڈیٹا, size_t len) {
        واپس iovw_put_بھرا(iovw, false, ڈیٹا, len);
}
صحیح iovw_put_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec *iov);
ساکن اندرونی صحیح iovw_put_iov(ساخت iovec_wrapper *iovw, ثابت ساخت iovec *iov) {
        واپس iovw_put_iov_بھرا(iovw, false, iov);
}
صحیح iovw_put_iovw_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec_wrapper *ماخذ);
ساکن اندرونی صحیح iovw_put_iovw(ساخت iovec_wrapper *iovw, ثابت ساخت iovec_wrapper *ماخذ) {
        واپس iovw_put_iovw_بھرا(iovw, false, ماخذ);
}
صحیح iovw_consume_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, خالی *ڈیٹا, size_t len);
ساکن اندرونی صحیح iovw_consume(ساخت iovec_wrapper *iovw, خالی *ڈیٹا, size_t len) {
        واپس iovw_consume_بھرا(iovw, false, ڈیٹا, len);
}
صحیح iovw_consume_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ساخت iovec *iov);
ساکن اندرونی صحیح iovw_consume_iov(ساخت iovec_wrapper *iovw, ساخت iovec *iov) {
        واپس iovw_consume_iov_بھرا(iovw, false, iov);
}
صحیح iovw_extend_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت خالی *ڈیٹا, size_t len);
ساکن اندرونی صحیح iovw_extend(ساخت iovec_wrapper *iovw, ثابت خالی *ڈیٹا, size_t len) {
        واپس iovw_extend_بھرا(iovw, false, ڈیٹا, len);
}
صحیح iovw_extend_iov_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec *iov);
ساکن اندرونی صحیح iovw_extend_iov(ساخت iovec_wrapper *iovw, ثابت ساخت iovec *iov) {
        واپس iovw_extend_iov_بھرا(iovw, false, iov);
}
صحیح iovw_extend_iovw_بھرا(ساخت iovec_wrapper *iovw, bool accept_zero, ثابت ساخت iovec_wrapper *ماخذ);
ساکن اندرونی صحیح iovw_extend_iovw(ساخت iovec_wrapper *iovw, ثابت ساخت iovec_wrapper *ماخذ) {
        واپس iovw_extend_iovw_بھرا(iovw, false, ماخذ);
}

ساکن اندرونی bool iovw_isempty(ثابت ساخت iovec_wrapper *iovw) {
        واپس !iovw || iovw->گنتی == 0;
}

صحیح iovw_put_string_میدان_بھرا(ساخت iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *قدر);
ساکن اندرونی صحیح iovw_put_string_میدان(ساخت iovec_wrapper *iovw, ثابت حرف *میدان, ثابت حرف *قدر) {
        واپس iovw_put_string_میدان_بھرا(iovw, false, میدان, قدر);
}
ساکن اندرونی صحیح iovw_replace_string_میدان(ساخت iovec_wrapper *iovw, ثابت حرف *میدان, ثابت حرف *قدر) {
        واپس iovw_put_string_میدان_بھرا(iovw, true, میدان, قدر);
}
صحیح iovw_put_string_fieldf_بھرا(ساخت iovec_wrapper *iovw, bool replace, ثابت حرف *میدان, ثابت حرف *format, ...) _printf_(4, 5);
#تعریف_کرو iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_بھرا(iovw, false, __VA_ARGS__)
#تعریف_کرو iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_بھرا(iovw, true, __VA_ARGS__)
صحیح iovw_put_string_میدان_free(ساخت iovec_wrapper *iovw, ثابت حرف *میدان, حرف *قدر);
خالی iovw_rebase(ساخت iovec_wrapper *iovw, خالی *old, خالی *new);
size_t iovw_سائز(ثابت ساخت iovec_wrapper *iovw);
صحیح iovw_concat(ثابت ساخت iovec_wrapper *iovw, ساخت iovec *ret);
حرف* iovw_مقصد_cstring(ثابت ساخت iovec_wrapper *iovw);

صحیح iovec_بانٹو(ثابت ساخت iovec *iov, size_t لمبائی_سائز, ساخت iovec_wrapper *ret);
صحیح iovw_ملاؤ(ثابت ساخت iovec_wrapper *iovw, size_t لمبائی_سائز, ساخت iovec *ret);
