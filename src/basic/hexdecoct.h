/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

حرف octchar(صحيح x) _const_;
صحيح unoctchar(حرف c) _const_;

حرف decchar(صحيح x) _const_;
صحيح undecchar(حرف c) _const_;

حرف hexchar(صحيح x) _const_;
صحيح unhexchar(حرف c) _const_;

حرف* hexmem(ثابت فراغ *p, size_t l) _nonnull_if_nonzero_(1, 2);
صحيح unhexmem_پر(ثابت حرف *p, size_t l, bool secure, فراغ **ret_داده, size_t *ret_اندازه) _nonnull_if_nonzero_(1, 2);
ساكن مضمن صحيح unhexmem(ثابت حرف *p, فراغ **ret_داده, size_t *ret_اندازه) {
        أرجع unhexmem_پر(p, SIZE_MAX, false, ret_داده, ret_اندازه);
}

حرف base32hexchar(صحيح x) _const_;
صحيح unbase32hexchar(حرف c) _const_;

حرف base64char(صحيح x) _const_;
حرف urlsafe_base64char(صحيح x) _const_;
صحيح unbase64char(حرف c) _const_;

حرف* base32hexmem(ثابت فراغ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
صحيح unbase32hexmem(ثابت حرف *p, size_t l, bool padding, فراغ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_پر(ثابت فراغ *p, size_t l, size_t سطر_break, حرف **ret) _nonnull_if_nonzero_(1, 2);
ساكن مضمن ssize_t base64mem(ثابت فراغ *p, size_t l, حرف **ret) {
        أرجع base64mem_پر(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                حرف **پیشوند,
                size_t plen,
                ثابت فراغ *p,
                size_t l,
                size_t indent,
                size_t width);
صحيح unbase64mem_پر(ثابت حرف *p, size_t l, bool secure, فراغ **ret_داده, size_t *ret_اندازه) _nonnull_if_nonzero_(1, 2);
ساكن مضمن صحيح unbase64mem(ثابت حرف *p, فراغ **ret_داده, size_t *ret_اندازه) {
        أرجع unbase64mem_پر(p, SIZE_MAX, false, ret_داده, ret_اندازه);
}

فراغ hexdump(FILE *f, ثابت فراغ *p, size_t s) _nonnull_if_nonzero_(2, 3);
