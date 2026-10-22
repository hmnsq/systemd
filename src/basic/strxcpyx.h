/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

size_t strnpcpy_full(वर्ण **dest, size_t size, ध्रुव वर्ण *src, size_t len, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strnpcpy(वर्ण **dest, size_t size, ध्रुव वर्ण *src, size_t len) {
        प्रत्यागम strnpcpy_full(dest, size, src, len, रिक्त);
}
size_t strpcpy_full(वर्ण **dest, size_t size, ध्रुव वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strpcpy(वर्ण **dest, size_t size, ध्रुव वर्ण *src) {
        प्रत्यागम strpcpy_full(dest, size, src, रिक्त);
}
size_t strpcpyf_full(वर्ण **dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
size_t strpcpyl_full(वर्ण **dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
size_t strnscpy_full(वर्ण *dest, size_t size, ध्रुव वर्ण *src, size_t len, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strnscpy(वर्ण *dest, size_t size, ध्रुव वर्ण *src, size_t len) {
        प्रत्यागम strnscpy_full(dest, size, src, len, रिक्त);
}
size_t strscpy_full(वर्ण *dest, size_t size, ध्रुव वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ size_t strscpy(वर्ण *dest, size_t size, ध्रुव वर्ण *src) {
        प्रत्यागम strscpy_full(dest, size, src, रिक्त);
}
size_t strscpyl_full(वर्ण *dest, size_t size, bool *ret_truncated, ध्रुव वर्ण *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, रिक्त, src, ##__VA_ARGS__)
