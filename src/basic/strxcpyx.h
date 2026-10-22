/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

size_t strnpcpy_full(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len, bool *ret_truncated);
സ്ഥിതം വരിയിൽ size_t strnpcpy(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len) {
        തിരികെ strnpcpy_full(dest, size, src, len, ശൂന്യ);
}
size_t strpcpy_full(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src, bool *ret_truncated);
സ്ഥിതം വരിയിൽ size_t strpcpy(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src) {
        തിരികെ strpcpy_full(dest, size, src, ശൂന്യ);
}
size_t strpcpyf_full(അക്ഷരം **dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), ശൂന്യ, (src), ##__VA_ARGS__)
size_t strpcpyl_full(അക്ഷരം **dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), ശൂന്യ, (src), ##__VA_ARGS__)
size_t strnscpy_full(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len, bool *ret_truncated);
സ്ഥിതം വരിയിൽ size_t strnscpy(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len) {
        തിരികെ strnscpy_full(dest, size, src, len, ശൂന്യ);
}
size_t strscpy_full(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src, bool *ret_truncated);
സ്ഥിതം വരിയിൽ size_t strscpy(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src) {
        തിരികെ strscpy_full(dest, size, src, ശൂന്യ);
}
size_t strscpyl_full(അക്ഷരം *dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, ശൂന്യ, src, ##__VA_ARGS__)
