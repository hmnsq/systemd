/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना iovec_wrapper {
        संरचना iovec *iovec;
        माप_प्रकार मोजणी;
};

शून्य iovw_done_free(संरचना iovec_wrapper *iovw);
शून्य iovw_done(संरचना iovec_wrapper *iovw);

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free);

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free_free);

पूर्णांक iovw_compare(स्थिरांक संरचना iovec_wrapper *a, स्थिरांक संरचना iovec_wrapper *b) _pure_;
स्थिर ओळीत bool iovw_equal(स्थिरांक संरचना iovec_wrapper *a, स्थिरांक संरचना iovec_wrapper *b) {
        परत iovw_compare(a, b) == 0;
}

पूर्णांक iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len);
स्थिर ओळीत पूर्णांक iovw_put(संरचना iovec_wrapper *iovw, शून्य *data, माप_प्रकार len) {
        परत iovw_put_full(iovw, false, data, len);
}
पूर्णांक iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec *iov);
स्थिर ओळीत पूर्णांक iovw_put_iov(संरचना iovec_wrapper *iovw, स्थिरांक संरचना iovec *iov) {
        परत iovw_put_iov_full(iovw, false, iov);
}
पूर्णांक iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec_wrapper *source);
स्थिर ओळीत पूर्णांक iovw_put_iovw(संरचना iovec_wrapper *iovw, स्थिरांक संरचना iovec_wrapper *source) {
        परत iovw_put_iovw_full(iovw, false, source);
}
पूर्णांक iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len);
स्थिर ओळीत पूर्णांक iovw_consume(संरचना iovec_wrapper *iovw, शून्य *data, माप_प्रकार len) {
        परत iovw_consume_full(iovw, false, data, len);
}
पूर्णांक iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov);
स्थिर ओळीत पूर्णांक iovw_consume_iov(संरचना iovec_wrapper *iovw, संरचना iovec *iov) {
        परत iovw_consume_iov_full(iovw, false, iov);
}
पूर्णांक iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक शून्य *data, माप_प्रकार len);
स्थिर ओळीत पूर्णांक iovw_extend(संरचना iovec_wrapper *iovw, स्थिरांक शून्य *data, माप_प्रकार len) {
        परत iovw_extend_full(iovw, false, data, len);
}
पूर्णांक iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec *iov);
स्थिर ओळीत पूर्णांक iovw_extend_iov(संरचना iovec_wrapper *iovw, स्थिरांक संरचना iovec *iov) {
        परत iovw_extend_iov_full(iovw, false, iov);
}
पूर्णांक iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, स्थिरांक संरचना iovec_wrapper *source);
स्थिर ओळीत पूर्णांक iovw_extend_iovw(संरचना iovec_wrapper *iovw, स्थिरांक संरचना iovec_wrapper *source) {
        परत iovw_extend_iovw_full(iovw, false, source);
}

स्थिर ओळीत bool iovw_isempty(स्थिरांक संरचना iovec_wrapper *iovw) {
        परत !iovw || iovw->मोजणी == 0;
}

पूर्णांक iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, स्थिरांक अक्षर *field, स्थिरांक अक्षर *मूल्य);
स्थिर ओळीत पूर्णांक iovw_put_string_field(संरचना iovec_wrapper *iovw, स्थिरांक अक्षर *field, स्थिरांक अक्षर *मूल्य) {
        परत iovw_put_string_field_full(iovw, false, field, मूल्य);
}
स्थिर ओळीत पूर्णांक iovw_replace_string_field(संरचना iovec_wrapper *iovw, स्थिरांक अक्षर *field, स्थिरांक अक्षर *मूल्य) {
        परत iovw_put_string_field_full(iovw, true, field, मूल्य);
}
पूर्णांक iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, स्थिरांक अक्षर *field, स्थिरांक अक्षर *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
पूर्णांक iovw_put_string_field_free(संरचना iovec_wrapper *iovw, स्थिरांक अक्षर *field, अक्षर *मूल्य);
शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new);
माप_प्रकार iovw_size(स्थिरांक संरचना iovec_wrapper *iovw);
पूर्णांक iovw_concat(स्थिरांक संरचना iovec_wrapper *iovw, संरचना iovec *ret);
अक्षर* iovw_to_cstring(स्थिरांक संरचना iovec_wrapper *iovw);

पूर्णांक iovec_split(स्थिरांक संरचना iovec *iov, माप_प्रकार length_size, संरचना iovec_wrapper *ret);
पूर्णांक iovw_merge(स्थिरांक संरचना iovec_wrapper *iovw, माप_प्रकार length_size, संरचना iovec *ret);
