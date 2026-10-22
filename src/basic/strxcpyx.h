/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

size_t strnpcpy_full(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src, size_t len, bool *ret_truncated);
நிலையான உள்வரி size_t strnpcpy(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src, size_t len) {
        திரும்பு strnpcpy_full(dest, size, src, len, வெற்று);
}
size_t strpcpy_full(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src, bool *ret_truncated);
நிலையான உள்வரி size_t strpcpy(எழுத்து **dest, size_t size, மாறிலி எழுத்து *src) {
        திரும்பு strpcpy_full(dest, size, src, வெற்று);
}
size_t strpcpyf_full(எழுத்து **dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), வெற்று, (src), ##__VA_ARGS__)
size_t strpcpyl_full(எழுத்து **dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), வெற்று, (src), ##__VA_ARGS__)
size_t strnscpy_full(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src, size_t len, bool *ret_truncated);
நிலையான உள்வரி size_t strnscpy(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src, size_t len) {
        திரும்பு strnscpy_full(dest, size, src, len, வெற்று);
}
size_t strscpy_full(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src, bool *ret_truncated);
நிலையான உள்வரி size_t strscpy(எழுத்து *dest, size_t size, மாறிலி எழுத்து *src) {
        திரும்பு strscpy_full(dest, size, src, வெற்று);
}
size_t strscpyl_full(எழுத்து *dest, size_t size, bool *ret_truncated, மாறிலி எழுத்து *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, வெற்று, src, ##__VA_ARGS__)
