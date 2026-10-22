/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

கட்டமைப்பு iovec_wrapper {
        கட்டமைப்பு iovec *iovec;
        size_t மொத்தம்;
};

வெறுமை iovw_done_free(கட்டமைப்பு iovec_wrapper *iovw);
வெறுமை iovw_done(கட்டமைப்பு iovec_wrapper *iovw);

கட்டமைப்பு iovec_wrapper* iovw_free(கட்டமைப்பு iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(கட்டமைப்பு iovec_wrapper*, iovw_free);

கட்டமைப்பு iovec_wrapper* iovw_free_free(கட்டமைப்பு iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(கட்டமைப்பு iovec_wrapper*, iovw_free_free);

முழு iovw_compare(மாறிலி கட்டமைப்பு iovec_wrapper *a, மாறிலி கட்டமைப்பு iovec_wrapper *b) _pure_;
நிலையான உள்வரி bool iovw_equal(மாறிலி கட்டமைப்பு iovec_wrapper *a, மாறிலி கட்டமைப்பு iovec_wrapper *b) {
        திரும்பு iovw_compare(a, b) == 0;
}

முழு iovw_put_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, வெறுமை *data, size_t len);
நிலையான உள்வரி முழு iovw_put(கட்டமைப்பு iovec_wrapper *iovw, வெறுமை *data, size_t len) {
        திரும்பு iovw_put_full(iovw, false, data, len);
}
முழு iovw_put_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec *iov);
நிலையான உள்வரி முழு iovw_put_iov(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி கட்டமைப்பு iovec *iov) {
        திரும்பு iovw_put_iov_full(iovw, false, iov);
}
முழு iovw_put_iovw_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec_wrapper *source);
நிலையான உள்வரி முழு iovw_put_iovw(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி கட்டமைப்பு iovec_wrapper *source) {
        திரும்பு iovw_put_iovw_full(iovw, false, source);
}
முழு iovw_consume_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, வெறுமை *data, size_t len);
நிலையான உள்வரி முழு iovw_consume(கட்டமைப்பு iovec_wrapper *iovw, வெறுமை *data, size_t len) {
        திரும்பு iovw_consume_full(iovw, false, data, len);
}
முழு iovw_consume_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, கட்டமைப்பு iovec *iov);
நிலையான உள்வரி முழு iovw_consume_iov(கட்டமைப்பு iovec_wrapper *iovw, கட்டமைப்பு iovec *iov) {
        திரும்பு iovw_consume_iov_full(iovw, false, iov);
}
முழு iovw_extend_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி வெறுமை *data, size_t len);
நிலையான உள்வரி முழு iovw_extend(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி வெறுமை *data, size_t len) {
        திரும்பு iovw_extend_full(iovw, false, data, len);
}
முழு iovw_extend_iov_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec *iov);
நிலையான உள்வரி முழு iovw_extend_iov(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி கட்டமைப்பு iovec *iov) {
        திரும்பு iovw_extend_iov_full(iovw, false, iov);
}
முழு iovw_extend_iovw_full(கட்டமைப்பு iovec_wrapper *iovw, bool accept_zero, மாறிலி கட்டமைப்பு iovec_wrapper *source);
நிலையான உள்வரி முழு iovw_extend_iovw(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி கட்டமைப்பு iovec_wrapper *source) {
        திரும்பு iovw_extend_iovw_full(iovw, false, source);
}

நிலையான உள்வரி bool iovw_isempty(மாறிலி கட்டமைப்பு iovec_wrapper *iovw) {
        திரும்பு !iovw || iovw->மொத்தம் == 0;
}

முழு iovw_put_string_field_full(கட்டமைப்பு iovec_wrapper *iovw, bool replace, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு);
நிலையான உள்வரி முழு iovw_put_string_field(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு) {
        திரும்பு iovw_put_string_field_full(iovw, false, field, மதிப்பு);
}
நிலையான உள்வரி முழு iovw_replace_string_field(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி எழுத்து *field, மாறிலி எழுத்து *மதிப்பு) {
        திரும்பு iovw_put_string_field_full(iovw, true, field, மதிப்பு);
}
முழு iovw_put_string_fieldf_full(கட்டமைப்பு iovec_wrapper *iovw, bool replace, மாறிலி எழுத்து *field, மாறிலி எழுத்து *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
முழு iovw_put_string_field_free(கட்டமைப்பு iovec_wrapper *iovw, மாறிலி எழுத்து *field, எழுத்து *மதிப்பு);
வெறுமை iovw_rebase(கட்டமைப்பு iovec_wrapper *iovw, வெறுமை *old, வெறுமை *new);
size_t iovw_size(மாறிலி கட்டமைப்பு iovec_wrapper *iovw);
முழு iovw_concat(மாறிலி கட்டமைப்பு iovec_wrapper *iovw, கட்டமைப்பு iovec *ret);
எழுத்து* iovw_to_cstring(மாறிலி கட்டமைப்பு iovec_wrapper *iovw);

முழு iovec_split(மாறிலி கட்டமைப்பு iovec *iov, size_t length_size, கட்டமைப்பு iovec_wrapper *ret);
முழு iovw_merge(மாறிலி கட்டமைப்பு iovec_wrapper *iovw, size_t length_size, கட்டமைப்பு iovec *ret);
