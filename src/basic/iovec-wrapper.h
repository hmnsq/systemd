/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਬਣਤਰ iovec_wrapper {
        ਬਣਤਰ iovec *iovec;
        size_t ਗਿਣਤੀ_2;
};

ਖਾਲੀ iovw_ਪੂਰਾ_free(ਬਣਤਰ iovec_wrapper *iovw);
ਖਾਲੀ iovw_ਪੂਰਾ(ਬਣਤਰ iovec_wrapper *iovw);

ਬਣਤਰ iovec_wrapper* iovw_free(ਬਣਤਰ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਬਣਤਰ iovec_wrapper*, iovw_free);

ਬਣਤਰ iovec_wrapper* iovw_free_free(ਬਣਤਰ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਬਣਤਰ iovec_wrapper*, iovw_free_free);

ਪੂਰਨ iovw_compare(ਅਚਲ ਬਣਤਰ iovec_wrapper *a, ਅਚਲ ਬਣਤਰ iovec_wrapper *b) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool iovw_equal(ਅਚਲ ਬਣਤਰ iovec_wrapper *a, ਅਚਲ ਬਣਤਰ iovec_wrapper *b) {
        ਵਾਪਸ iovw_compare(a, b) == 0;
}

ਪੂਰਨ iovw_put_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਖਾਲੀ *ਅੰਕੜੇ, size_t len);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_put(ਬਣਤਰ iovec_wrapper *iovw, ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        ਵਾਪਸ iovw_put_ਭਰਿਆ(iovw, false, ਅੰਕੜੇ, len);
}
ਪੂਰਨ iovw_put_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec *iov);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_put_iov(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਬਣਤਰ iovec *iov) {
        ਵਾਪਸ iovw_put_iov_ਭਰਿਆ(iovw, false, iov);
}
ਪੂਰਨ iovw_put_iovw_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_put_iovw(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ) {
        ਵਾਪਸ iovw_put_iovw_ਭਰਿਆ(iovw, false, ਸਰੋਤ);
}
ਪੂਰਨ iovw_consume_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਖਾਲੀ *ਅੰਕੜੇ, size_t len);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_consume(ਬਣਤਰ iovec_wrapper *iovw, ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        ਵਾਪਸ iovw_consume_ਭਰਿਆ(iovw, false, ਅੰਕੜੇ, len);
}
ਪੂਰਨ iovw_consume_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਬਣਤਰ iovec *iov);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_consume_iov(ਬਣਤਰ iovec_wrapper *iovw, ਬਣਤਰ iovec *iov) {
        ਵਾਪਸ iovw_consume_iov_ਭਰਿਆ(iovw, false, iov);
}
ਪੂਰਨ iovw_extend_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t len);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_extend(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t len) {
        ਵਾਪਸ iovw_extend_ਭਰਿਆ(iovw, false, ਅੰਕੜੇ, len);
}
ਪੂਰਨ iovw_extend_iov_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec *iov);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_extend_iov(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਬਣਤਰ iovec *iov) {
        ਵਾਪਸ iovw_extend_iov_ਭਰਿਆ(iovw, false, iov);
}
ਪੂਰਨ iovw_extend_iovw_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool accept_zero, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_extend_iovw(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਬਣਤਰ iovec_wrapper *ਸਰੋਤ) {
        ਵਾਪਸ iovw_extend_iovw_ਭਰਿਆ(iovw, false, ਸਰੋਤ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool iovw_isempty(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw) {
        ਵਾਪਸ !iovw || iovw->ਗਿਣਤੀ_2 == 0;
}

ਪੂਰਨ iovw_put_string_ਖੇਤਰ_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool replace, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_put_string_ਖੇਤਰ(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ iovw_put_string_ਖੇਤਰ_ਭਰਿਆ(iovw, false, ਖੇਤਰ, ਮੁੱਲ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ iovw_replace_string_ਖੇਤਰ(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ iovw_put_string_ਖੇਤਰ_ਭਰਿਆ(iovw, true, ਖੇਤਰ, ਮੁੱਲ);
}
ਪੂਰਨ iovw_put_string_fieldf_ਭਰਿਆ(ਬਣਤਰ iovec_wrapper *iovw, bool replace, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(4, 5);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_ਭਰਿਆ(iovw, false, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_ਭਰਿਆ(iovw, true, __VA_ARGS__)
ਪੂਰਨ iovw_put_string_ਖੇਤਰ_free(ਬਣਤਰ iovec_wrapper *iovw, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ *ਮੁੱਲ);
ਖਾਲੀ iovw_rebase(ਬਣਤਰ iovec_wrapper *iovw, ਖਾਲੀ *old, ਖਾਲੀ *new);
size_t iovw_ਮਾਪ(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw);
ਪੂਰਨ iovw_concat(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw, ਬਣਤਰ iovec *ret);
ਅੱਖਰ* iovw_ਮੰਜ਼ਿਲ_cstring(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw);

ਪੂਰਨ iovec_ਵੰਡੋ(ਅਚਲ ਬਣਤਰ iovec *iov, size_t ਲੰਮਾਈ_ਮਾਪ, ਬਣਤਰ iovec_wrapper *ret);
ਪੂਰਨ iovw_ਮਿਲਾਓ(ਅਚਲ ਬਣਤਰ iovec_wrapper *iovw, size_t ਲੰਮਾਈ_ਮਾਪ, ਬਣਤਰ iovec *ret);
