/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

char octchar(số_nguyên x) _const_;
số_nguyên unoctchar(char c) _const_;

char decchar(số_nguyên x) _const_;
số_nguyên undecchar(char c) _const_;

char hexchar(số_nguyên x) _const_;
số_nguyên unhexchar(char c) _const_;

char* hexmem(const void *p, size_t l) _nonnull_if_nonzero_(1, 2);
số_nguyên unhexmem_full(const char *p, size_t l, bool secure, void **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
static inline số_nguyên unhexmem(const char *p, void **ret_data, size_t *ret_size) {
        trả_về unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

char base32hexchar(số_nguyên x) _const_;
số_nguyên unbase32hexchar(char c) _const_;

char base64char(số_nguyên x) _const_;
char urlsafe_base64char(số_nguyên x) _const_;
số_nguyên unbase64char(char c) _const_;

char* base32hexmem(const void *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
số_nguyên unbase32hexmem(const char *p, size_t l, bool padding, void **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(const void *p, size_t l, size_t line_break, char **ret) _nonnull_if_nonzero_(1, 2);
static inline ssize_t base64mem(const void *p, size_t l, char **ret) {
        trả_về base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                char **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width);
số_nguyên unbase64mem_full(const char *p, size_t l, bool secure, void **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
static inline số_nguyên unbase64mem(const char *p, void **ret_data, size_t *ret_size) {
        trả_về unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

void hexdump(FILE *f, const void *p, size_t s) _nonnull_if_nonzero_(2, 3);
