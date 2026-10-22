/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

حرف octchar(صحيح x) _const_;
صحيح unoctchar(حرف c) _const_;

حرف decchar(صحيح x) _const_;
صحيح undecchar(حرف c) _const_;

حرف hexchar(صحيح x) _const_;
صحيح unhexchar(حرف c) _const_;

حرف* hexmem(ثابت فراغ *p, size_t l) _nonnull_if_nonzero_(1, 2);
صحيح unhexmem_full(ثابت حرف *p, size_t l, bool secure, فراغ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ساكن مضمن صحيح unhexmem(ثابت حرف *p, فراغ **ret_data, size_t *ret_size) {
        أرجع unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

حرف base32hexchar(صحيح x) _const_;
صحيح unbase32hexchar(حرف c) _const_;

حرف base64char(صحيح x) _const_;
حرف urlsafe_base64char(صحيح x) _const_;
صحيح unbase64char(حرف c) _const_;

حرف* base32hexmem(ثابت فراغ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
صحيح unbase32hexmem(ثابت حرف *p, size_t l, bool padding, فراغ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ثابت فراغ *p, size_t l, size_t لي_break, حرف **ret) _nonnull_if_nonzero_(1, 2);
ساكن مضمن ssize_t base64mem(ثابت فراغ *p, size_t l, حرف **ret) {
        أرجع base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                حرف **prefix,
                size_t plen,
                ثابت فراغ *p,
                size_t l,
                size_t indent,
                size_t width);
صحيح unbase64mem_full(ثابت حرف *p, size_t l, bool secure, فراغ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ساكن مضمن صحيح unbase64mem(ثابت حرف *p, فراغ **ret_data, size_t *ret_size) {
        أرجع unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

فراغ hexdump(FILE *f, ثابت فراغ *p, size_t s) _nonnull_if_nonzero_(2, 3);
