/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

संरचना iovec_wrapper {
        संरचना iovec *iovec;
        size_t गणनम्;
};

शून्य iovw_समाप्तम्_free(संरचना iovec_wrapper *iovw);
शून्य iovw_समाप्तम्(संरचना iovec_wrapper *iovw);

संरचना iovec_wrapper* iovw_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free);

संरचना iovec_wrapper* iovw_free_free(संरचना iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(संरचना iovec_wrapper*, iovw_free_free);

पूर्णाङ्क iovw_compare(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) विशुद्धम्;
स्थिर अन्तःस्थ bool iovw_equal(ध्रुव संरचना iovec_wrapper *a, ध्रुव संरचना iovec_wrapper *b) {
        प्रत्यागम iovw_compare(a, b) == 0;
}

पूर्णाङ्क iovw_put_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *दत्त, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put(संरचना iovec_wrapper *iovw, शून्य *दत्त, size_t len) {
        प्रत्यागम iovw_put_पूर्णम्(iovw, false, दत्त, len);
}
पूर्णाङ्क iovw_put_iov_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iov(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec *iov) {
        प्रत्यागम iovw_put_iov_पूर्णम्(iovw, false, iov);
}
पूर्णाङ्क iovw_put_iovw_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_iovw(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec_wrapper *source) {
        प्रत्यागम iovw_put_iovw_पूर्णम्(iovw, false, source);
}
पूर्णाङ्क iovw_consume_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, शून्य *दत्त, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume(संरचना iovec_wrapper *iovw, शून्य *दत्त, size_t len) {
        प्रत्यागम iovw_consume_पूर्णम्(iovw, false, दत्त, len);
}
पूर्णाङ्क iovw_consume_iov_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_consume_iov(संरचना iovec_wrapper *iovw, संरचना iovec *iov) {
        प्रत्यागम iovw_consume_iov_पूर्णम्(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव शून्य *दत्त, size_t len);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend(संरचना iovec_wrapper *iovw, ध्रुव शून्य *दत्त, size_t len) {
        प्रत्यागम iovw_extend_पूर्णम्(iovw, false, दत्त, len);
}
पूर्णाङ्क iovw_extend_iov_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec *iov);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iov(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec *iov) {
        प्रत्यागम iovw_extend_iov_पूर्णम्(iovw, false, iov);
}
पूर्णाङ्क iovw_extend_iovw_पूर्णम्(संरचना iovec_wrapper *iovw, bool accept_zero, ध्रुव संरचना iovec_wrapper *source);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_extend_iovw(संरचना iovec_wrapper *iovw, ध्रुव संरचना iovec_wrapper *source) {
        प्रत्यागम iovw_extend_iovw_पूर्णम्(iovw, false, source);
}

स्थिर अन्तःस्थ bool iovw_isempty(ध्रुव संरचना iovec_wrapper *iovw) {
        प्रत्यागम !iovw || iovw->गणनम् == 0;
}

पूर्णाङ्क iovw_put_string_क्षेत्रम्_पूर्णम्(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *क्षेत्रम्, ध्रुव वर्ण *मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क iovw_put_string_क्षेत्रम्(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *क्षेत्रम्, ध्रुव वर्ण *मानम्) {
        प्रत्यागम iovw_put_string_क्षेत्रम्_पूर्णम्(iovw, false, क्षेत्रम्, मानम्);
}
स्थिर अन्तःस्थ पूर्णाङ्क iovw_replace_string_क्षेत्रम्(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *क्षेत्रम्, ध्रुव वर्ण *मानम्) {
        प्रत्यागम iovw_put_string_क्षेत्रम्_पूर्णम्(iovw, true, क्षेत्रम्, मानम्);
}
पूर्णाङ्क iovw_put_string_fieldf_पूर्णम्(संरचना iovec_wrapper *iovw, bool replace, ध्रुव वर्ण *क्षेत्रम्, ध्रुव वर्ण *format, ...) _printf_(4, 5);
#परिभाषय iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_पूर्णम्(iovw, false, __VA_ARGS__)
#परिभाषय iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_पूर्णम्(iovw, true, __VA_ARGS__)
पूर्णाङ्क iovw_put_string_क्षेत्रम्_free(संरचना iovec_wrapper *iovw, ध्रुव वर्ण *क्षेत्रम्, वर्ण *मानम्);
शून्य iovw_rebase(संरचना iovec_wrapper *iovw, शून्य *old, शून्य *new);
size_t iovw_आकार(ध्रुव संरचना iovec_wrapper *iovw);
पूर्णाङ्क iovw_concat(ध्रुव संरचना iovec_wrapper *iovw, संरचना iovec *ret);
वर्ण* iovw_गन्तव्यम्_cstring(ध्रुव संरचना iovec_wrapper *iovw);

पूर्णाङ्क iovec_विभज(ध्रुव संरचना iovec *iov, size_t दीर्घता_आकार, संरचना iovec_wrapper *ret);
पूर्णाङ्क iovw_merge(ध्रुव संरचना iovec_wrapper *iovw, size_t दीर्घता_आकार, संरचना iovec *ret);
