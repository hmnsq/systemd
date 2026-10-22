/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना iovec_wrapper {
        संरचना iovec *iovec;
        माप_प्रकार गन्ती;
};

शून्य iovw_done_free(संरचना iovec_wrapper *iovw);
शून्य iovw_done(संरचना iovec_wrapper *iovw);

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free);

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free_free);

पूर्णाङ्क iovw_compare(अचर संरचना iovec_wrapper *a, अचर संरचना iovec_wrapper *b) _pure_;
स्थिर अन्तःस्थ bool iovw_equal(अचर संरचना iovec_wrapper *a, अचर संरचना iovec_wrapper *b) {
        फर्काऊ iovw_compare(a, b) == 0;
}

पूर्णाङ्क iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put(संरचना iovec_wrapper *iovw, शून्य *data, माप_प्रकार len) {
        फर्काऊ iovw_put_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iov(संरचना iovec_wrapper *iovw, अचर संरचना iovec *iov) {
        फर्काऊ iovw_put_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iovw(संरचना iovec_wrapper *iovw, अचर संरचना iovec_wrapper *source) {
        फर्काऊ iovw_put_iovw_full(iovw, false, source);
}
पूर्णाङ्क iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, माप_प्रकार len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume(संरचना iovec_wrapper *iovw, शून्य *data, माप_प्रकार len) {
        फर्काऊ iovw_consume_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume_iov(संरचना iovec_wrapper *iovw, संरचना iovec *iov) {
        फर्काऊ iovw_consume_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर शून्य *data, माप_प्रकार len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend(संरचना iovec_wrapper *iovw, अचर शून्य *data, माप_प्रकार len) {
        फर्काऊ iovw_extend_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iov(संरचना iovec_wrapper *iovw, अचर संरचना iovec *iov) {
        फर्काऊ iovw_extend_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, अचर संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iovw(संरचना iovec_wrapper *iovw, अचर संरचना iovec_wrapper *source) {
        फर्काऊ iovw_extend_iovw_full(iovw, false, source);
}

स्थिर अन्तःस्थ bool iovw_isempty(अचर संरचना iovec_wrapper *iovw) {
        फर्काऊ !iovw || iovw->गन्ती == 0;
}

पूर्णाङ्क iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, अचर वर्ण *field, अचर वर्ण *मान);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_string_field(संरचना iovec_wrapper *iovw, अचर वर्ण *field, अचर वर्ण *मान) {
        फर्काऊ iovw_put_string_field_full(iovw, false, field, मान);
}
स्थिर अन्तःस्थ पूर्णाङ्क iovw_replace_string_field(संरचना iovec_wrapper *iovw, अचर वर्ण *field, अचर वर्ण *मान) {
        फर्काऊ iovw_put_string_field_full(iovw, true, field, मान);
}
पूर्णाङ्क iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, अचर वर्ण *field, अचर वर्ण *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
पूर्णाङ्क iovw_put_string_field_free(संरचना iovec_wrapper *iovw, अचर वर्ण *field, वर्ण *मान);
शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new);
माप_प्रकार iovw_size(अचर संरचना iovec_wrapper *iovw);
पूर्णाङ्क iovw_concat(अचर संरचना iovec_wrapper *iovw, संरचना iovec *ret);
वर्ण* iovw_to_cstring(अचर संरचना iovec_wrapper *iovw);

पूर्णाङ्क iovec_split(अचर संरचना iovec *iov, माप_प्रकार length_size, संरचना iovec_wrapper *ret);
पूर्णाङ्क iovw_merge(अचर संरचना iovec_wrapper *iovw, माप_प्रकार length_size, संरचना iovec *ret);
