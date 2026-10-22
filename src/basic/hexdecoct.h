/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

अक्षर octchar(पूर्णांक x) _const_;
पूर्णांक unoctchar(अक्षर c) _const_;

अक्षर decchar(पूर्णांक x) _const_;
पूर्णांक undecchar(अक्षर c) _const_;

अक्षर hexchar(पूर्णांक x) _const_;
पूर्णांक unhexchar(अक्षर c) _const_;

अक्षर* hexmem(स्थिर व्योम *p, माप_प्रकार l) _nonnull_if_nonzero_(1, 2);
पूर्णांक unhexmem_full(स्थिर अक्षर *p, माप_प्रकार l, bool secure, व्योम **ret_तथ्य, माप_प्रकार *ret_आकार) _nonnull_if_nonzero_(1, 2);
अटल अंतरभूत पूर्णांक unhexmem(स्थिर अक्षर *p, व्योम **ret_तथ्य, माप_प्रकार *ret_आकार) {
        वापस unhexmem_full(p, SIZE_MAX, false, ret_तथ्य, ret_आकार);
}

अक्षर base32hexchar(पूर्णांक x) _const_;
पूर्णांक unbase32hexchar(अक्षर c) _const_;

अक्षर base64char(पूर्णांक x) _const_;
अक्षर urlsafe_base64char(पूर्णांक x) _const_;
पूर्णांक unbase64char(अक्षर c) _const_;

अक्षर* base32hexmem(स्थिर व्योम *p, माप_प्रकार l, bool padding) _nonnull_if_nonzero_(1, 2);
पूर्णांक unbase32hexmem(स्थिर अक्षर *p, माप_प्रकार l, bool padding, व्योम **mem, माप_प्रकार *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(स्थिर व्योम *p, माप_प्रकार l, माप_प्रकार पंक्ति_break, अक्षर **ret) _nonnull_if_nonzero_(1, 2);
अटल अंतरभूत ssize_t base64mem(स्थिर व्योम *p, माप_प्रकार l, अक्षर **ret) {
        वापस base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                अक्षर **prefix,
                माप_प्रकार plen,
                स्थिर व्योम *p,
                माप_प्रकार l,
                माप_प्रकार indent,
                माप_प्रकार width);
पूर्णांक unbase64mem_full(स्थिर अक्षर *p, माप_प्रकार l, bool secure, व्योम **ret_तथ्य, माप_प्रकार *ret_आकार) _nonnull_if_nonzero_(1, 2);
अटल अंतरभूत पूर्णांक unbase64mem(स्थिर अक्षर *p, व्योम **ret_तथ्य, माप_प्रकार *ret_आकार) {
        वापस unbase64mem_full(p, SIZE_MAX, false, ret_तथ्य, ret_आकार);
}

व्योम hexdump(खाता *f, स्थिर व्योम *p, माप_प्रकार s) _nonnull_if_nonzero_(2, 3);
