/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

माप_प्रकार strnpcpy_full(वर्ण **dest, माप_प्रकार size, अचर वर्ण *src, माप_प्रकार len, bool *ret_truncated);
स्थिर अन्तःस्थ माप_प्रकार strnpcpy(वर्ण **dest, माप_प्रकार size, अचर वर्ण *src, माप_प्रकार len) {
        फर्काऊ strnpcpy_full(dest, size, src, len, रिक्त);
}
माप_प्रकार strpcpy_full(वर्ण **dest, माप_प्रकार size, अचर वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ माप_प्रकार strpcpy(वर्ण **dest, माप_प्रकार size, अचर वर्ण *src) {
        फर्काऊ strpcpy_full(dest, size, src, रिक्त);
}
माप_प्रकार strpcpyf_full(वर्ण **dest, माप_प्रकार size, bool *ret_truncated, अचर वर्ण *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
माप_प्रकार strpcpyl_full(वर्ण **dest, माप_प्रकार size, bool *ret_truncated, अचर वर्ण *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
माप_प्रकार strnscpy_full(वर्ण *dest, माप_प्रकार size, अचर वर्ण *src, माप_प्रकार len, bool *ret_truncated);
स्थिर अन्तःस्थ माप_प्रकार strnscpy(वर्ण *dest, माप_प्रकार size, अचर वर्ण *src, माप_प्रकार len) {
        फर्काऊ strnscpy_full(dest, size, src, len, रिक्त);
}
माप_प्रकार strscpy_full(वर्ण *dest, माप_प्रकार size, अचर वर्ण *src, bool *ret_truncated);
स्थिर अन्तःस्थ माप_प्रकार strscpy(वर्ण *dest, माप_प्रकार size, अचर वर्ण *src) {
        फर्काऊ strscpy_full(dest, size, src, रिक्त);
}
माप_प्रकार strscpyl_full(वर्ण *dest, माप_प्रकार size, bool *ret_truncated, अचर वर्ण *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, रिक्त, src, ##__VA_ARGS__)
