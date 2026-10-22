/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

size_t strnpcpy_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len, bool *ret_truncated);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t strnpcpy(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len) {
        ಹಿಂತಿರುಗಿ strnpcpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, len, ಖಾಲಿ);
}
size_t strpcpy_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, bool *ret_truncated);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t strpcpy(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src) {
        ಹಿಂತಿರುಗಿ strpcpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, ಖಾಲಿ);
}
size_t strpcpyf_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) _printf_(4, 5);
#ವ್ಯಾಖ್ಯಾನಿಸು strpcpyf(dest, ಗಾತ್ರ_2, src, ...) \
        strpcpyf_ತುಂಬಿದ((dest), (ಗಾತ್ರ_2), ಖಾಲಿ, (src), ##__VA_ARGS__)
size_t strpcpyl_ತುಂಬಿದ(ಅಕ್ಷರ **dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strpcpyl(dest, ಗಾತ್ರ_2, src, ...) \
        strpcpyl_ತುಂಬಿದ((dest), (ಗಾತ್ರ_2), ಖಾಲಿ, (src), ##__VA_ARGS__)
size_t strnscpy_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len, bool *ret_truncated);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t strnscpy(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t len) {
        ಹಿಂತಿರುಗಿ strnscpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, len, ಖಾಲಿ);
}
size_t strscpy_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, bool *ret_truncated);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t strscpy(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src) {
        ಹಿಂತಿರುಗಿ strscpy_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, src, ಖಾಲಿ);
}
size_t strscpyl_ತುಂಬಿದ(ಅಕ್ಷರ *dest, size_t ಗಾತ್ರ_2, bool *ret_truncated, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ...) _sentinel_;
#ವ್ಯಾಖ್ಯಾನಿಸು strscpyl(dest, ಗಾತ್ರ_2, src, ...) \
        strscpyl_ತುಂಬಿದ(dest, ಗಾತ್ರ_2, ಖಾಲಿ, src, ##__VA_ARGS__)
