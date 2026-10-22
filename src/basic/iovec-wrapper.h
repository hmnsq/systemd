/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸಂರಚನಾ iovec_wrapper {
        ಸಂರಚನಾ iovec *iovec;
        size_t ಎಣಿಕೆ;
};

ಶೂನ್ಯ iovw_done_free(ಸಂರಚನಾ iovec_wrapper *iovw);
ಶೂನ್ಯ iovw_done(ಸಂರಚನಾ iovec_wrapper *iovw);

ಸಂರಚನಾ iovec_wrapper* iovw_free(ಸಂರಚನಾ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಸಂರಚನಾ iovec_wrapper*, iovw_free);

ಸಂರಚನಾ iovec_wrapper* iovw_free_free(ಸಂರಚನಾ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಸಂರಚನಾ iovec_wrapper*, iovw_free_free);

ಪೂರ್ಣಾಙ್ಕ iovw_compare(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *a, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *b) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool iovw_equal(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *a, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *b) {
        ಪ್ರತ್ಯಾಗಮ iovw_compare(a, b) == 0;
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *data, size_t len);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_put(ಸಂರಚನಾ iovec_wrapper *iovw, ಶೂನ್ಯ *data, size_t len) {
        ಪ್ರತ್ಯಾಗಮ iovw_put_full(iovw, false, data, len);
}
ಪೂರ್ಣಾಙ್ಕ iovw_put_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec *iov);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_put_iov(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ಸಂರಚನಾ iovec *iov) {
        ಪ್ರತ್ಯಾಗಮ iovw_put_iov_full(iovw, false, iov);
}
ಪೂರ್ಣಾಙ್ಕ iovw_put_iovw_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_put_iovw(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source) {
        ಪ್ರತ್ಯಾಗಮ iovw_put_iovw_full(iovw, false, source);
}
ಪೂರ್ಣಾಙ್ಕ iovw_consume_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *data, size_t len);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_consume(ಸಂರಚನಾ iovec_wrapper *iovw, ಶೂನ್ಯ *data, size_t len) {
        ಪ್ರತ್ಯಾಗಮ iovw_consume_full(iovw, false, data, len);
}
ಪೂರ್ಣಾಙ್ಕ iovw_consume_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಸಂರಚನಾ iovec *iov);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_consume_iov(ಸಂರಚನಾ iovec_wrapper *iovw, ಸಂರಚನಾ iovec *iov) {
        ಪ್ರತ್ಯಾಗಮ iovw_consume_iov_full(iovw, false, iov);
}
ಪೂರ್ಣಾಙ್ಕ iovw_extend_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಶೂನ್ಯ *data, size_t len);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_extend(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ಶೂನ್ಯ *data, size_t len) {
        ಪ್ರತ್ಯಾಗಮ iovw_extend_full(iovw, false, data, len);
}
ಪೂರ್ಣಾಙ್ಕ iovw_extend_iov_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec *iov);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_extend_iov(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ಸಂರಚನಾ iovec *iov) {
        ಪ್ರತ್ಯಾಗಮ iovw_extend_iov_full(iovw, false, iov);
}
ಪೂರ್ಣಾಙ್ಕ iovw_extend_iovw_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool accept_zero, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_extend_iovw(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *source) {
        ಪ್ರತ್ಯಾಗಮ iovw_extend_iovw_full(iovw, false, source);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool iovw_isempty(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw) {
        ಪ್ರತ್ಯಾಗಮ !iovw || iovw->ಎಣಿಕೆ == 0;
}

ಪೂರ್ಣಾಙ್ಕ iovw_put_string_field_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool replace, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_put_string_field(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ iovw_put_string_field_full(iovw, false, field, ಮೌಲ್ಯ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ iovw_replace_string_field(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ iovw_put_string_field_full(iovw, true, field, ಮೌಲ್ಯ);
}
ಪೂರ್ಣಾಙ್ಕ iovw_put_string_fieldf_full(ಸಂರಚನಾ iovec_wrapper *iovw, bool replace, ಧ್ರುವ ವರ್ಣ *field, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(4, 5);
#define iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_full(iovw, false, __VA_ARGS__)
#define iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_full(iovw, true, __VA_ARGS__)
ಪೂರ್ಣಾಙ್ಕ iovw_put_string_field_free(ಸಂರಚನಾ iovec_wrapper *iovw, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ *ಮೌಲ್ಯ);
ಶೂನ್ಯ iovw_rebase(ಸಂರಚನಾ iovec_wrapper *iovw, ಶೂನ್ಯ *old, ಶೂನ್ಯ *new);
size_t iovw_size(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw);
ಪೂರ್ಣಾಙ್ಕ iovw_concat(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw, ಸಂರಚನಾ iovec *ret);
ವರ್ಣ* iovw_to_cstring(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw);

ಪೂರ್ಣಾಙ್ಕ iovec_split(ಧ್ರುವ ಸಂರಚನಾ iovec *iov, size_t length_size, ಸಂರಚನಾ iovec_wrapper *ret);
ಪೂರ್ಣಾಙ್ಕ iovw_merge(ಧ್ರುವ ಸಂರಚನಾ iovec_wrapper *iovw, size_t length_size, ಸಂರಚನಾ iovec *ret);
