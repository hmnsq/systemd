/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

অক্ষর octchar(পূর্ণসংখ্যা x) _const_;
পূর্ণসংখ্যা unoctchar(অক্ষর c) _const_;

অক্ষর decchar(পূর্ণসংখ্যা x) _const_;
পূর্ণসংখ্যা undecchar(অক্ষর c) _const_;

অক্ষর hexchar(পূর্ণসংখ্যা x) _const_;
পূর্ণসংখ্যা unhexchar(অক্ষর c) _const_;

অক্ষর* hexmem(ধ্রুবক শূন্য *p, size_t l) _nonnull_if_nonzero_(1, 2);
পূর্ণসংখ্যা unhexmem_full(ধ্রুবক অক্ষর *p, size_t l, bool secure, শূন্য **ret_তথ্য, size_t *ret_আকার) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃরেখ পূর্ণসংখ্যা unhexmem(ধ্রুবক অক্ষর *p, শূন্য **ret_তথ্য, size_t *ret_আকার) {
        ফেরত unhexmem_full(p, SIZE_MAX, false, ret_তথ্য, ret_আকার);
}

অক্ষর base32hexchar(পূর্ণসংখ্যা x) _const_;
পূর্ণসংখ্যা unbase32hexchar(অক্ষর c) _const_;

অক্ষর base64char(পূর্ণসংখ্যা x) _const_;
অক্ষর urlsafe_base64char(পূর্ণসংখ্যা x) _const_;
পূর্ণসংখ্যা unbase64char(অক্ষর c) _const_;

অক্ষর* base32hexmem(ধ্রুবক শূন্য *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
পূর্ণসংখ্যা unbase32hexmem(ধ্রুবক অক্ষর *p, size_t l, bool padding, শূন্য **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ধ্রুবক শূন্য *p, size_t l, size_t সারি_break, অক্ষর **ret) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃরেখ ssize_t base64mem(ধ্রুবক শূন্য *p, size_t l, অক্ষর **ret) {
        ফেরত base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                অক্ষর **prefix,
                size_t plen,
                ধ্রুবক শূন্য *p,
                size_t l,
                size_t indent,
                size_t width);
পূর্ণসংখ্যা unbase64mem_full(ধ্রুবক অক্ষর *p, size_t l, bool secure, শূন্য **ret_তথ্য, size_t *ret_আকার) _nonnull_if_nonzero_(1, 2);
স্থির অন্তঃরেখ পূর্ণসংখ্যা unbase64mem(ধ্রুবক অক্ষর *p, শূন্য **ret_তথ্য, size_t *ret_আকার) {
        ফেরত unbase64mem_full(p, SIZE_MAX, false, ret_তথ্য, ret_আকার);
}

শূন্য hexdump(FILE *f, ধ্রুবক শূন্য *p, size_t s) _nonnull_if_nonzero_(2, 3);
