/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

size_t strnpcpy_full(نویسه **dest, size_t size, ثابت نویسه *src, size_t len, bool *ret_truncated);
ایستا درونخطی size_t strnpcpy(نویسه **dest, size_t size, ثابت نویسه *src, size_t len) {
        بازگشت strnpcpy_full(dest, size, src, len, پوچ);
}
size_t strpcpy_full(نویسه **dest, size_t size, ثابت نویسه *src, bool *ret_truncated);
ایستا درونخطی size_t strpcpy(نویسه **dest, size_t size, ثابت نویسه *src) {
        بازگشت strpcpy_full(dest, size, src, پوچ);
}
size_t strpcpyf_full(نویسه **dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), پوچ, (src), ##__VA_ARGS__)
size_t strpcpyl_full(نویسه **dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), پوچ, (src), ##__VA_ARGS__)
size_t strnscpy_full(نویسه *dest, size_t size, ثابت نویسه *src, size_t len, bool *ret_truncated);
ایستا درونخطی size_t strnscpy(نویسه *dest, size_t size, ثابت نویسه *src, size_t len) {
        بازگشت strnscpy_full(dest, size, src, len, پوچ);
}
size_t strscpy_full(نویسه *dest, size_t size, ثابت نویسه *src, bool *ret_truncated);
ایستا درونخطی size_t strscpy(نویسه *dest, size_t size, ثابت نویسه *src) {
        بازگشت strscpy_full(dest, size, src, پوچ);
}
size_t strscpyl_full(نویسه *dest, size_t size, bool *ret_truncated, ثابت نویسه *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, پوچ, src, ##__VA_ARGS__)
