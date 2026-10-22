/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

size_t strnpcpy_full(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src, size_t len, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strnpcpy(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src, size_t len) {
        প্রত্যাগম strnpcpy_full(dest, size, src, len, রিক্ত);
}
size_t strpcpy_full(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strpcpy(বর্ণ **dest, size_t size, ধ্রুব বর্ণ *src) {
        প্রত্যাগম strpcpy_full(dest, size, src, রিক্ত);
}
size_t strpcpyf_full(বর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), রিক্ত, (src), ##__VA_ARGS__)
size_t strpcpyl_full(বর্ণ **dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), রিক্ত, (src), ##__VA_ARGS__)
size_t strnscpy_full(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src, size_t len, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strnscpy(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src, size_t len) {
        প্রত্যাগম strnscpy_full(dest, size, src, len, রিক্ত);
}
size_t strscpy_full(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src, bool *ret_truncated);
স্থির অন্তঃস্থ size_t strscpy(বর্ণ *dest, size_t size, ধ্রুব বর্ণ *src) {
        প্রত্যাগম strscpy_full(dest, size, src, রিক্ত);
}
size_t strscpyl_full(বর্ণ *dest, size_t size, bool *ret_truncated, ধ্রুব বর্ণ *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, রিক্ত, src, ##__VA_ARGS__)
