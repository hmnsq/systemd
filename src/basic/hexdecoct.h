/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

वर्ण octchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unoctchar(वर्ण c) _const_;

वर्ण decchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क undecchar(वर्ण c) _const_;

वर्ण hexchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unhexchar(वर्ण c) _const_;

वर्ण* hexmem(ध्रुव शून्य *p, size_t l) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क unhexmem_full(ध्रुव वर्ण *p, size_t l, bool secure, शून्य **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ पूर्णाङ्क unhexmem(ध्रुव वर्ण *p, शून्य **ret_data, size_t *ret_size) {
        प्रत्यागम unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

वर्ण base32hexchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unbase32hexchar(वर्ण c) _const_;

वर्ण base64char(पूर्णाङ्क x) _const_;
वर्ण urlsafe_base64char(पूर्णाङ्क x) _const_;
पूर्णाङ्क unbase64char(वर्ण c) _const_;

वर्ण* base32hexmem(ध्रुव शून्य *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क unbase32hexmem(ध्रुव वर्ण *p, size_t l, bool padding, शून्य **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ध्रुव शून्य *p, size_t l, size_t पंक्ति_break, वर्ण **ret) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ ssize_t base64mem(ध्रुव शून्य *p, size_t l, वर्ण **ret) {
        प्रत्यागम base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                वर्ण **prefix,
                size_t plen,
                ध्रुव शून्य *p,
                size_t l,
                size_t indent,
                size_t width);
पूर्णाङ्क unbase64mem_full(ध्रुव वर्ण *p, size_t l, bool secure, शून्य **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ पूर्णाङ्क unbase64mem(ध्रुव वर्ण *p, शून्य **ret_data, size_t *ret_size) {
        प्रत्यागम unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

शून्य hexdump(FILE *f, ध्रुव शून्य *p, size_t s) _nonnull_if_nonzero_(2, 3);
