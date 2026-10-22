/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

संरचना iovec_wrapper {
        संरचना iovec *iovec;
        size_t गणनम्;
};

शून्य iovw_done_free(संरचना iovec_wrapper *iovw);
शून्य iovw_done(संरचना iovec_wrapper *iovw);

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free);

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free_free);

पूर्णाङ्क iovw_compare(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) _pure_;
स्थिर अन्तःस्थ bool iovw_equal(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) {
        प्रत्यागम iovw_compare(a, b) == 0;
}

पूर्णाङ्क iovw_put_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put(संरचना iovec_wrapper *iovw, शून्य *data, size_t len) {
        प्रत्यागम iovw_put_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_put_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iov(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec *iov) {
        प्रत्यागम iovw_put_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_put_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iovw(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec_wrapper *source) {
        प्रत्यागम iovw_put_iovw_full(iovw, false, source);
}
पूर्णाङ्क iovw_consume_full(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *data, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume(संरचना iovec_wrapper *iovw, शून्य *data, size_t len) {
        प्रत्यागम iovw_consume_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_consume_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume_iov(संरचना iovec_wrapper *iovw, संरचना iovec *iov) {
        प्रत्यागम iovw_consume_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव शून्य *data, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend(संरचना iovec_wrapper *iovw, ध्रुव शून्य *data, size_t len) {
        प्रत्यागम iovw_extend_full(iovw, false, data, len);
}
पूर्णाङ्क iovw_extend_iov_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iov(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec *iov) {
        प्रत्यागम iovw_extend_iov_full(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_iovw_full(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iovw(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec_wrapper *source) {
        प्रत्यागम iovw_extend_iovw_full(iovw, false, source);
}

स्थिर अन्तःस्थ bool iovw_isempty(ध्रुव संरचना iovec_wrapper *iovw) {
        प्रत्यागम !iovw || iovw->गणनम् == 0;
}

पूर्णाङ्क iovw_put_string_field_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_string_field(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *field, ध्रुव वर्ण *मानम्) {
        प्रत्यागम iovw_put_string_field_full(iovw, false, field, मानम्);
}
स्थिर अन्तःस्थ पूर्णाङ्क iovw_replace_string_field(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *field, ध्रुव वर्ण *मानम्) {
        प्रत्यागम iovw_put_string_field_full(iovw, true, field, मानम्);
}
पूर्णाङ्क iovw_put_string_fieldf_full(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *field, ध्रुव वर्ण *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
पूर्णाङ्क iovw_put_string_field_free(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *field, वर्ण *मानम्);
शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new);
size_t iovw_size(ध्रुव संरचना iovec_wrapper *iovw);
पूर्णाङ्क iovw_concat(ध्रुव संरचना iovec_wrapper *iovw, संरचना iovec *ret);
वर्ण* iovw_to_cstring(ध्रुव संरचना iovec_wrapper *iovw);

पूर्णाङ्क iovec_split(ध्रुव संरचना iovec *iov, size_t length_size, संरचना iovec_wrapper *ret);
पूर्णाङ्क iovw_merge(ध्रुव संरचना iovec_wrapper *iovw, size_t length_size, संरचना iovec *ret);
