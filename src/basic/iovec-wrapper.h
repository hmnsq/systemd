/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ರಚನೆ iovec_wrapper {
        ರಚನೆ iovec *iovec;
        size_t ಎಣಿಕೆ_2;
};

ಶೂನ್ಯ iovw_ಮುಗಿದ_free(ರಚನೆ iovec_wrapper *iovw);
ಶೂನ್ಯ iovw_ಮುಗಿದ(ರಚನೆ iovec_wrapper *iovw);

ರಚನೆ iovec_wrapper* iovw_free(ರಚನೆ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ರಚನೆ iovec_wrapper*, iovw_free);

ರಚನೆ iovec_wrapper* iovw_free_free(ರಚನೆ iovec_wrapper *iovw);
DEFINE_TRIVIAL_CLEANUP_FUNC(ರಚನೆ iovec_wrapper*, iovw_free_free);

ಪೂರ್ಣಾಂಕ iovw_compare(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *b) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool iovw_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *a, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *b) {
        ಹಿಂತಿರುಗಿ iovw_compare(a, b) == 0;
}

ಪೂರ್ಣಾಂಕ iovw_put_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_put(ರಚನೆ iovec_wrapper *iovw, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        ಹಿಂತಿರುಗಿ iovw_put_ತುಂಬಿದ(iovw, false, ದತ್ತಾಂಶ, len);
}
ಪೂರ್ಣಾಂಕ iovw_put_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_put_iov(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov) {
        ಹಿಂತಿರುಗಿ iovw_put_iov_ತುಂಬಿದ(iovw, false, iov);
}
ಪೂರ್ಣಾಂಕ iovw_put_iovw_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_put_iovw(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ) {
        ಹಿಂತಿರುಗಿ iovw_put_iovw_ತುಂಬಿದ(iovw, false, ಮೂಲ);
}
ಪೂರ್ಣಾಂಕ iovw_consume_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_consume(ರಚನೆ iovec_wrapper *iovw, ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        ಹಿಂತಿರುಗಿ iovw_consume_ತುಂಬಿದ(iovw, false, ದತ್ತಾಂಶ, len);
}
ಪೂರ್ಣಾಂಕ iovw_consume_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ರಚನೆ iovec *iov);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_consume_iov(ರಚನೆ iovec_wrapper *iovw, ರಚನೆ iovec *iov) {
        ಹಿಂತಿರುಗಿ iovw_consume_iov_ತುಂಬಿದ(iovw, false, iov);
}
ಪೂರ್ಣಾಂಕ iovw_extend_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_extend(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t len) {
        ಹಿಂತಿರುಗಿ iovw_extend_ತುಂಬಿದ(iovw, false, ದತ್ತಾಂಶ, len);
}
ಪೂರ್ಣಾಂಕ iovw_extend_iov_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_extend_iov(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov) {
        ಹಿಂತಿರುಗಿ iovw_extend_iov_ತುಂಬಿದ(iovw, false, iov);
}
ಪೂರ್ಣಾಂಕ iovw_extend_iovw_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool accept_zero, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_extend_iovw(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *ಮೂಲ) {
        ಹಿಂತಿರುಗಿ iovw_extend_iovw_ತುಂಬಿದ(iovw, false, ಮೂಲ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool iovw_isempty(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw) {
        ಹಿಂತಿರುಗಿ !iovw || iovw->ಎಣಿಕೆ_2 == 0;
}

ಪೂರ್ಣಾಂಕ iovw_put_string_ಕ್ಷೇತ್ರ_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool replace, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_put_string_ಕ್ಷೇತ್ರ(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ iovw_put_string_ಕ್ಷೇತ್ರ_ತುಂಬಿದ(iovw, false, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ iovw_replace_string_ಕ್ಷೇತ್ರ(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ iovw_put_string_ಕ್ಷೇತ್ರ_ತುಂಬಿದ(iovw, true, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}
ಪೂರ್ಣಾಂಕ iovw_put_string_fieldf_ತುಂಬಿದ(ರಚನೆ iovec_wrapper *iovw, bool replace, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(4, 5);
#ವ್ಯಾಖ್ಯಾನಿಸು iovw_put_string_fieldf(iovw, ...)     iovw_put_string_fieldf_ತುಂಬಿದ(iovw, false, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು iovw_replace_string_fieldf(iovw, ...) iovw_put_string_fieldf_ತುಂಬಿದ(iovw, true, __VA_ARGS__)
ಪೂರ್ಣಾಂಕ iovw_put_string_ಕ್ಷೇತ್ರ_free(ರಚನೆ iovec_wrapper *iovw, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ *ಮೌಲ್ಯ);
ಶೂನ್ಯ iovw_rebase(ರಚನೆ iovec_wrapper *iovw, ಶೂನ್ಯ *old, ಶೂನ್ಯ *new);
size_t iovw_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw);
ಪೂರ್ಣಾಂಕ iovw_concat(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw, ರಚನೆ iovec *ret);
ಅಕ್ಷರ* iovw_ಗಮ್ಯ_cstring(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw);

ಪೂರ್ಣಾಂಕ iovec_ವಿಭಜಿಸು(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iov, size_t ಉದ್ದಳತೆ_ಗಾತ್ರ, ರಚನೆ iovec_wrapper *ret);
ಪೂರ್ಣಾಂಕ iovw_ವಿಲೀನ(ಸ್ಥಿರಾಂಕ ರಚನೆ iovec_wrapper *iovw, size_t ಉದ್ದಳತೆ_ಗಾತ್ರ, ರಚನೆ iovec *ret);
