/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಅಕ್ಷರ octchar(ಪೂರ್ಣಾಂಕ x) _const_;
ಪೂರ್ಣಾಂಕ unoctchar(ಅಕ್ಷರ c) _const_;

ಅಕ್ಷರ decchar(ಪೂರ್ಣಾಂಕ x) _const_;
ಪೂರ್ಣಾಂಕ undecchar(ಅಕ್ಷರ c) _const_;

ಅಕ್ಷರ hexchar(ಪೂರ್ಣಾಂಕ x) _const_;
ಪೂರ್ಣಾಂಕ unhexchar(ಅಕ್ಷರ c) _const_;

ಅಕ್ಷರ* hexmem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಂಕ unhexmem_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t l, bool secure, ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ, size_t *ret_ಗಾತ್ರ) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ unhexmem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ unhexmem_ತುಂಬಿದ(p, SIZE_MAX, false, ret_ದತ್ತಾಂಶ, ret_ಗಾತ್ರ);
}

ಅಕ್ಷರ base32hexchar(ಪೂರ್ಣಾಂಕ x) _const_;
ಪೂರ್ಣಾಂಕ unbase32hexchar(ಅಕ್ಷರ c) _const_;

ಅಕ್ಷರ base64char(ಪೂರ್ಣಾಂಕ x) _const_;
ಅಕ್ಷರ urlsafe_base64char(ಪೂರ್ಣಾಂಕ x) _const_;
ಪೂರ್ಣಾಂಕ unbase64char(ಅಕ್ಷರ c) _const_;

ಅಕ್ಷರ* base32hexmem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಂಕ unbase32hexmem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t l, bool padding, ಶೂನ್ಯ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l, size_t ಸಾಲು_break, ಅಕ್ಷರ **ret) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ssize_t base64mem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t l, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ base64mem_ತುಂಬಿದ(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                ಅಕ್ಷರ **ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t plen,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p,
                size_t l,
                size_t indent,
                size_t width);
ಪೂರ್ಣಾಂಕ unbase64mem_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t l, bool secure, ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ, size_t *ret_ಗಾತ್ರ) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ unbase64mem(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಶೂನ್ಯ **ret_ದತ್ತಾಂಶ, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ unbase64mem_ತುಂಬಿದ(p, SIZE_MAX, false, ret_ದತ್ತಾಂಶ, ret_ಗಾತ್ರ);
}

ಶೂನ್ಯ hexdump(FILE *f, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t s) _nonnull_if_nonzero_(2, 3);
