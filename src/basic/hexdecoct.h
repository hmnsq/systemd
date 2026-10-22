/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ವರ್ಣ octchar(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ಪೂರ್ಣಾಙ್ಕ unoctchar(ವರ್ಣ c) _const_;

ವರ್ಣ decchar(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ಪೂರ್ಣಾಙ್ಕ undecchar(ವರ್ಣ c) _const_;

ವರ್ಣ hexchar(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ಪೂರ್ಣಾಙ್ಕ unhexchar(ವರ್ಣ c) _const_;

ವರ್ಣ* hexmem(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಙ್ಕ unhexmem_full(ಧ್ರುವ ವರ್ಣ *p, size_t l, bool secure, ಶೂನ್ಯ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ unhexmem(ಧ್ರುವ ವರ್ಣ *p, ಶೂನ್ಯ **ret_data, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ವರ್ಣ base32hexchar(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ಪೂರ್ಣಾಙ್ಕ unbase32hexchar(ವರ್ಣ c) _const_;

ವರ್ಣ base64char(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ವರ್ಣ urlsafe_base64char(ಪೂರ್ಣಾಙ್ಕ x) _const_;
ಪೂರ್ಣಾಙ್ಕ unbase64char(ವರ್ಣ c) _const_;

ವರ್ಣ* base32hexmem(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಙ್ಕ unbase32hexmem(ಧ್ರುವ ವರ್ಣ *p, size_t l, bool padding, ಶೂನ್ಯ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l, size_t ಸಾಲು_break, ವರ್ಣ **ret) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ssize_t base64mem(ಧ್ರುವ ಶೂನ್ಯ *p, size_t l, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                ವರ್ಣ **prefix,
                size_t plen,
                ಧ್ರುವ ಶೂನ್ಯ *p,
                size_t l,
                size_t indent,
                size_t width);
ಪೂರ್ಣಾಙ್ಕ unbase64mem_full(ಧ್ರುವ ವರ್ಣ *p, size_t l, bool secure, ಶೂನ್ಯ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ unbase64mem(ಧ್ರುವ ವರ್ಣ *p, ಶೂನ್ಯ **ret_data, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ಶೂನ್ಯ hexdump(FILE *f, ಧ್ರುವ ಶೂನ್ಯ *p, size_t s) _nonnull_if_nonzero_(2, 3);
