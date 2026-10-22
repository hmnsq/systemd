/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

var_na octchar(จำนวนเต็ม x) _const_;
จำนวนเต็ม unoctchar(var_na c) _const_;

var_na decchar(จำนวนเต็ม x) _const_;
จำนวนเต็ม undecchar(var_na c) _const_;

var_na hexchar(จำนวนเต็ม x) _const_;
จำนวนเต็ม unhexchar(var_na c) _const_;

var_na* hexmem(const void *p, size_t l) _nonnull_if_nonzero_(1, 2);
จำนวนเต็ม unhexmem_full(const var_na *p, size_t l, bool secure, void **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
static anta_hstha จำนวนเต็ม unhexmem(const var_na *p, void **ret_data, size_t *ret_size) {
        คืน unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

var_na base32hexchar(จำนวนเต็ม x) _const_;
จำนวนเต็ม unbase32hexchar(var_na c) _const_;

var_na base64char(จำนวนเต็ม x) _const_;
var_na urlsafe_base64char(จำนวนเต็ม x) _const_;
จำนวนเต็ม unbase64char(var_na c) _const_;

var_na* base32hexmem(const void *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
จำนวนเต็ม unbase32hexmem(const var_na *p, size_t l, bool padding, void **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(const void *p, size_t l, size_t line_break, var_na **ret) _nonnull_if_nonzero_(1, 2);
static anta_hstha ssize_t base64mem(const void *p, size_t l, var_na **ret) {
        คืน base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                var_na **prefix,
                size_t plen,
                const void *p,
                size_t l,
                size_t indent,
                size_t width);
จำนวนเต็ม unbase64mem_full(const var_na *p, size_t l, bool secure, void **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
static anta_hstha จำนวนเต็ม unbase64mem(const var_na *p, void **ret_data, size_t *ret_size) {
        คืน unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

void hexdump(FILE *f, const void *p, size_t s) _nonnull_if_nonzero_(2, 3);
