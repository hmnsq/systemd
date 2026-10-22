/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

अक्षर octchar(पूर्णांक x) _const_;
पूर्णांक unoctchar(अक्षर c) _const_;

अक्षर decchar(पूर्णांक x) _const_;
पूर्णांक undecchar(अक्षर c) _const_;

अक्षर hexchar(पूर्णांक x) _const_;
पूर्णांक unhexchar(अक्षर c) _const_;

अक्षर* hexmem(स्थिरांक शून्य *p, माप_प्रकार l) _nonnull_if_nonzero_(1, 2);
पूर्णांक unhexmem_full(स्थिरांक अक्षर *p, माप_प्रकार l, bool secure, शून्य **ret_data, माप_प्रकार *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर ओळीत पूर्णांक unhexmem(स्थिरांक अक्षर *p, शून्य **ret_data, माप_प्रकार *ret_size) {
        परत unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

अक्षर base32hexchar(पूर्णांक x) _const_;
पूर्णांक unbase32hexchar(अक्षर c) _const_;

अक्षर base64char(पूर्णांक x) _const_;
अक्षर urlsafe_base64char(पूर्णांक x) _const_;
पूर्णांक unbase64char(अक्षर c) _const_;

अक्षर* base32hexmem(स्थिरांक शून्य *p, माप_प्रकार l, bool padding) _nonnull_if_nonzero_(1, 2);
पूर्णांक unbase32hexmem(स्थिरांक अक्षर *p, माप_प्रकार l, bool padding, शून्य **mem, माप_प्रकार *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(स्थिरांक शून्य *p, माप_प्रकार l, माप_प्रकार ओळ_break, अक्षर **ret) _nonnull_if_nonzero_(1, 2);
स्थिर ओळीत ssize_t base64mem(स्थिरांक शून्य *p, माप_प्रकार l, अक्षर **ret) {
        परत base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                अक्षर **prefix,
                माप_प्रकार plen,
                स्थिरांक शून्य *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width);
पूर्णांक unbase64mem_full(स्थिरांक अक्षर *p, माप_प्रकार l, bool secure, शून्य **ret_data, माप_प्रकार *ret_size) _nonnull_if_nonzero_(1, 2);
स्थिर ओळीत पूर्णांक unbase64mem(स्थिरांक अक्षर *p, शून्य **ret_data, माप_प्रकार *ret_size) {
        परत unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

शून्य hexdump(खाता *f, स्थिरांक शून्य *p, माप_प्रकार s) _nonnull_if_nonzero_(2, 3);
