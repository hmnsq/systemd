/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

size_t strnpcpy_full(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len, bool *ret_truncated);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t strnpcpy(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len) {
        ಪ್ರತ್ಯಾಗಮ strnpcpy_full(dest, size, src, len, ರಿಕ್ತ);
}
size_t strpcpy_full(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, bool *ret_truncated);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t strpcpy(ವರ್ಣ **dest, size_t size, ಧ್ರುವ ವರ್ಣ *src) {
        ಪ್ರತ್ಯಾಗಮ strpcpy_full(dest, size, src, ರಿಕ್ತ);
}
size_t strpcpyf_full(ವರ್ಣ **dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), ರಿಕ್ತ, (src), ##__VA_ARGS__)
size_t strpcpyl_full(ವರ್ಣ **dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), ರಿಕ್ತ, (src), ##__VA_ARGS__)
size_t strnscpy_full(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len, bool *ret_truncated);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t strnscpy(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, size_t len) {
        ಪ್ರತ್ಯಾಗಮ strnscpy_full(dest, size, src, len, ರಿಕ್ತ);
}
size_t strscpy_full(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src, bool *ret_truncated);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t strscpy(ವರ್ಣ *dest, size_t size, ಧ್ರುವ ವರ್ಣ *src) {
        ಪ್ರತ್ಯಾಗಮ strscpy_full(dest, size, src, ರಿಕ್ತ);
}
size_t strscpyl_full(ವರ್ಣ *dest, size_t size, bool *ret_truncated, ಧ್ರುವ ವರ್ಣ *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, ರಿಕ್ತ, src, ##__VA_ARGS__)
