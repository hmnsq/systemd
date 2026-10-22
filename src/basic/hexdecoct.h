/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

حرف octchar(صحیح x) _const_;
صحیح unoctchar(حرف c) _const_;

حرف decchar(صحیح x) _const_;
صحیح undecchar(حرف c) _const_;

حرف hexchar(صحیح x) _const_;
صحیح unhexchar(حرف c) _const_;

حرف* hexmem(ثابت خالی *p, size_t l) _nonnull_if_nonzero_(1, 2);
صحیح unhexmem_بھرا(ثابت حرف *p, size_t l, bool secure, خالی **ret_ڈیٹا, size_t *ret_سائز) _nonnull_if_nonzero_(1, 2);
ساکن اندرونی صحیح unhexmem(ثابت حرف *p, خالی **ret_ڈیٹا, size_t *ret_سائز) {
        واپس unhexmem_بھرا(p, SIZE_MAX, false, ret_ڈیٹا, ret_سائز);
}

حرف base32hexchar(صحیح x) _const_;
صحیح unbase32hexchar(حرف c) _const_;

حرف base64char(صحیح x) _const_;
حرف urlsafe_base64char(صحیح x) _const_;
صحیح unbase64char(حرف c) _const_;

حرف* base32hexmem(ثابت خالی *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
صحیح unbase32hexmem(ثابت حرف *p, size_t l, bool padding, خالی **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_بھرا(ثابت خالی *p, size_t l, size_t سطر_break, حرف **ret) _nonnull_if_nonzero_(1, 2);
ساکن اندرونی ssize_t base64mem(ثابت خالی *p, size_t l, حرف **ret) {
        واپس base64mem_بھرا(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                حرف **سابقہ,
                size_t plen,
                ثابت خالی *p,
                size_t l,
                size_t indent,
                size_t width);
صحیح unbase64mem_بھرا(ثابت حرف *p, size_t l, bool secure, خالی **ret_ڈیٹا, size_t *ret_سائز) _nonnull_if_nonzero_(1, 2);
ساکن اندرونی صحیح unbase64mem(ثابت حرف *p, خالی **ret_ڈیٹا, size_t *ret_سائز) {
        واپس unbase64mem_بھرا(p, SIZE_MAX, false, ret_ڈیٹا, ret_سائز);
}

خالی hexdump(FILE *f, ثابت خالی *p, size_t s) _nonnull_if_nonzero_(2, 3);
