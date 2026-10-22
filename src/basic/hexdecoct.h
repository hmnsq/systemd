/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

वर्ण octchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unoctchar(वर्ण c) _const_;

वर्ण decchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क undecchar(वर्ण c) _const_;

वर्ण hexchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unhexchar(वर्ण c) _const_;

वर्ण* hexmem(अचर शून्य *p, माप_प्रकार l) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क unhexmem_full(अचर वर्ण *p, माप_प्रकार l, bool secure, शून्य **ret_data, माप_प्रकार *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ पूर्णाङ्क unhexmem(अचर वर्ण *p, शून्य **ret_data, माप_प्रकार *ret_size) {
        फर्काऊ unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

वर्ण base32hexchar(पूर्णाङ्क x) _const_;
पूर्णाङ्क unbase32hexchar(वर्ण c) _const_;

वर्ण base64char(पूर्णाङ्क x) _const_;
वर्ण urlsafe_base64char(पूर्णाङ्क x) _const_;
पूर्णाङ्क unbase64char(वर्ण c) _const_;

वर्ण* base32hexmem(अचर शून्य *p, माप_प्रकार l, bool padding) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क unbase32hexmem(अचर वर्ण *p, माप_प्रकार l, bool padding, शून्य **mem, माप_प्रकार *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(अचर शून्य *p, माप_प्रकार l, माप_प्रकार पङ्क्ति_break, वर्ण **ret) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ ssize_t base64mem(अचर शून्य *p, माप_प्रकार l, वर्ण **ret) {
        फर्काऊ base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                वर्ण **prefix,
                माप_प्रकार plen,
                अचर शून्य *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width);
पूर्णाङ्क unbase64mem_full(अचर वर्ण *p, माप_प्रकार l, bool secure, शून्य **ret_data, माप_प्रकार *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर अन्तःस्थ पूर्णाङ्क unbase64mem(अचर वर्ण *p, शून्य **ret_data, माप_प्रकार *ret_size) {
        फर्काऊ unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

शून्य hexdump(खाता *f, अचर शून्य *p, माप_प्रकार s) _nonnull_if_nonzero_(2, 3);
