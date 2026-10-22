/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

size_t strnpcpy_full(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strnpcpy(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len) {
        ঘূৰাই strnpcpy_full(dest, size, src, len, রিক্ত);
}
size_t strpcpy_full(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strpcpy(ৱর্ণ **dest, size_t size, ধ্রুৱ ৱর্ণ *src) {
        ঘূৰাই strpcpy_full(dest, size, src, রিক্ত);
}
size_t strpcpyf_full(ৱর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), রিক্ত, (src), ##__VA_ARGS__)
size_t strpcpyl_full(ৱর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), রিক্ত, (src), ##__VA_ARGS__)
size_t strnscpy_full(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strnscpy(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src, size_t len) {
        ঘূৰাই strnscpy_full(dest, size, src, len, রিক্ত);
}
size_t strscpy_full(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strscpy(ৱর্ণ *dest, size_t size, ধ্রুৱ ৱর্ণ *src) {
        ঘূৰাই strscpy_full(dest, size, src, রিক্ত);
}
size_t strscpyl_full(ৱর্ণ *dest, size_t size, bool *ret_truncated, ধ্রুৱ ৱর্ণ *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, রিক্ত, src, ##__VA_ARGS__)
