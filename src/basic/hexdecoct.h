/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

ৱর্ণ octchar(পূর্ণাঙ্ক x) _const_;
পূর্ণাঙ্ক unoctchar(ৱর্ণ c) _const_;

ৱর্ণ decchar(পূর্ণাঙ্ক x) _const_;
পূর্ণাঙ্ক undecchar(ৱর্ণ c) _const_;

ৱর্ণ hexchar(পূর্ণাঙ্ক x) _const_;
পূর্ণাঙ্ক unhexchar(ৱর্ণ c) _const_;

ৱর্ণ* hexmem(ধ্রুৱ শূন্য *p, size_t l) _nonnull_if_nonzero_(1, 2);
পূর্ণাঙ্ক unhexmem_full(ধ্রুৱ ৱর্ণ *p, size_t l, bool secure, শূন্য **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃস্থ পূর্ণাঙ্ক unhexmem(ধ্রুৱ ৱর্ণ *p, শূন্য **ret_data, size_t *ret_size) {
        ঘূৰাই unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ৱর্ণ base32hexchar(পূর্ণাঙ্ক x) _const_;
পূর্ণাঙ্ক unbase32hexchar(ৱর্ণ c) _const_;

ৱর্ণ base64char(পূর্ণাঙ্ক x) _const_;
ৱর্ণ urlsafe_base64char(পূর্ণাঙ্ক x) _const_;
পূর্ণাঙ্ক unbase64char(ৱর্ণ c) _const_;

ৱর্ণ* base32hexmem(ধ্রুৱ শূন্য *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
পূর্ণাঙ্ক unbase32hexmem(ধ্রুৱ ৱর্ণ *p, size_t l, bool padding, শূন্য **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ধ্রুৱ শূন্য *p, size_t l, size_t সাৰি_break, ৱর্ণ **ret) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃস্থ ssize_t base64mem(ধ্রুৱ শূন্য *p, size_t l, ৱর্ণ **ret) {
        ঘূৰাই base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                ৱর্ণ **prefix,
                size_t plen,
                ধ্রুৱ শূন্য *p,
                size_t l,
                size_t indent,
                size_t width);
পূর্ণাঙ্ক unbase64mem_full(ধ্রুৱ ৱর্ণ *p, size_t l, bool secure, শূন্য **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃস্থ পূর্ণাঙ্ক unbase64mem(ধ্রুৱ ৱর্ণ *p, শূন্য **ret_data, size_t *ret_size) {
        ঘূৰাই unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

শূন্য hexdump(FILE *f, ধ্রুৱ শূন্য *p, size_t s) _nonnull_if_nonzero_(2, 3);
