/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

size_t strnpcpy_full(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len, bool *ret_truncated);
স্থির অন্তঃরেখ size_t strnpcpy(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len) {
        ফেরত strnpcpy_full(dest, আকার_2, src, len, ফাঁকা);
}
size_t strpcpy_full(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src, bool *ret_truncated);
স্থির অন্তঃরেখ size_t strpcpy(অক্ষর **dest, size_t আকার_2, ধ্রুবক অক্ষর *src) {
        ফেরত strpcpy_full(dest, আকার_2, src, ফাঁকা);
}
size_t strpcpyf_full(অক্ষর **dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) _printf_(4, 5);
#define strpcpyf(dest, আকার_2, src, ...) \
        strpcpyf_full((dest), (আকার_2), ফাঁকা, (src), ##__VA_ARGS__)
size_t strpcpyl_full(অক্ষর **dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) _sentinel_;
#define strpcpyl(dest, আকার_2, src, ...) \
        strpcpyl_full((dest), (আকার_2), ফাঁকা, (src), ##__VA_ARGS__)
size_t strnscpy_full(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len, bool *ret_truncated);
স্থির অন্তঃরেখ size_t strnscpy(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src, size_t len) {
        ফেরত strnscpy_full(dest, আকার_2, src, len, ফাঁকা);
}
size_t strscpy_full(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src, bool *ret_truncated);
স্থির অন্তঃরেখ size_t strscpy(অক্ষর *dest, size_t আকার_2, ধ্রুবক অক্ষর *src) {
        ফেরত strscpy_full(dest, আকার_2, src, ফাঁকা);
}
size_t strscpyl_full(অক্ষর *dest, size_t আকার_2, bool *ret_truncated, ধ্রুবক অক্ষর *src, ...) _sentinel_;
#define strscpyl(dest, আকার_2, src, ...) \
        strscpyl_full(dest, আকার_2, ফাঁকা, src, ##__VA_ARGS__)
