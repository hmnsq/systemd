/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

माप_प्रकार strnpcpy_full(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len, bool *ret_truncated);
स्थिर ओळीत माप_प्रकार strnpcpy(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len) {
        परत strnpcpy_full(dest, size, src, len, रिक्त);
}
माप_प्रकार strpcpy_full(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src, bool *ret_truncated);
स्थिर ओळीत माप_प्रकार strpcpy(अक्षर **dest, माप_प्रकार size, स्थिरांक अक्षर *src) {
        परत strpcpy_full(dest, size, src, रिक्त);
}
माप_प्रकार strpcpyf_full(अक्षर **dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
माप_प्रकार strpcpyl_full(अक्षर **dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), रिक्त, (src), ##__VA_ARGS__)
माप_प्रकार strnscpy_full(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len, bool *ret_truncated);
स्थिर ओळीत माप_प्रकार strnscpy(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src, माप_प्रकार len) {
        परत strnscpy_full(dest, size, src, len, रिक्त);
}
माप_प्रकार strscpy_full(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src, bool *ret_truncated);
स्थिर ओळीत माप_प्रकार strscpy(अक्षर *dest, माप_प्रकार size, स्थिरांक अक्षर *src) {
        परत strscpy_full(dest, size, src, रिक्त);
}
माप_प्रकार strscpyl_full(अक्षर *dest, माप_प्रकार size, bool *ret_truncated, स्थिरांक अक्षर *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, रिक्त, src, ##__VA_ARGS__)
