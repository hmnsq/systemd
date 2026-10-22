/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Concatenates/copies strings. In any case, terminates in all cases
 * with '\0' and moves the @dest pointer forward to the added '\0'.
 * Returns the remaining size, and 0 if the string was truncated.
 *
 * Due to the intended usage, these helpers silently noop invocations
 * having zero size.  This is technically an exception to the above
 * statement "terminates in all cases".  It's unexpected for such calls to
 * occur outside of a loop where this is the preferred behavior.
 */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "strxcpyx.h"

size_t strnpcpy_full(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        എങ്കിൽ (size == 0) {
                എങ്കിൽ (ret_truncated)
                        *ret_truncated = len > 0;
                തിരികെ 0;
        }

        എങ്കിൽ (len >= size) {
                എങ്കിൽ (size > 1)
                        *dest = mempcpy(*dest, src, size-1);
                size = 0;
                truncated = true;
        } അല്ലെങ്കിൽ എങ്കിൽ (len > 0) {
                *dest = mempcpy(*dest, src, len);
                size -= len;
        }

        എങ്കിൽ (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        തിരികെ size;
}

size_t strpcpy_full(അക്ഷരം **dest, size_t size, സ്ഥിരം അക്ഷരം *src, bool *ret_truncated) {
        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        തിരികെ strnpcpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strpcpyf_full(അക്ഷരം **dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) {
        bool truncated = false;
        va_list va;
        പൂർണ്ണസംഖ്യ i;

        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        va_start(va, src);
        i = vsnprintf(*dest, size, src, va);
        va_end(va);

        പ്രതിജ്ഞാ(i >= 0);

        എങ്കിൽ (i < (പൂർണ്ണസംഖ്യ) size) {
                *dest += i;
                size -= i;
        } അല്ലെങ്കിൽ {
                size = 0;
                truncated = i > 0;
        }

        എങ്കിൽ (ret_truncated)
                *ret_truncated = truncated;

        തിരികെ size;
}

size_t strpcpyl_full(അക്ഷരം **dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) {
        bool truncated = false;
        va_list va;

        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        va_start(va, src);
        ചെയ്യുക {
                bool t;

                size = strpcpy_full(dest, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, അക്ഷരം *);
        } ആകുമ്പോൾ (src);
        va_end(va);

        എങ്കിൽ (ret_truncated)
                *ret_truncated = truncated;
        തിരികെ size;
}

size_t strnscpy_full(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src, size_t len, bool *ret_truncated) {
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        s = dest;
        തിരികെ strnpcpy_full(&s, size, src, len, ret_truncated);
}

size_t strscpy_full(അക്ഷരം *dest, size_t size, സ്ഥിരം അക്ഷരം *src, bool *ret_truncated) {
        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        തിരികെ strnscpy_full(dest, size, src, strlen(src), ret_truncated);
}

size_t strscpyl_full(അക്ഷരം *dest, size_t size, bool *ret_truncated, സ്ഥിരം അക്ഷരം *src, ...) {
        bool truncated = false;
        va_list va;
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(dest);
        പ്രതിജ്ഞാ(src);

        va_start(va, src);
        s = dest;
        ചെയ്യുക {
                bool t;

                size = strpcpy_full(&s, size, src, &t);
                truncated = truncated || t;
                src = va_arg(va, അക്ഷരം *);
        } ആകുമ്പോൾ (src);
        va_end(va);

        എങ്കിൽ (ret_truncated)
                *ret_truncated = truncated;

        തിരികെ size;
}
