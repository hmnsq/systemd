/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

எழுத்து octchar(முழு x) _const_;
முழு unoctchar(எழுத்து c) _const_;

எழுத்து decchar(முழு x) _const_;
முழு undecchar(எழுத்து c) _const_;

எழுத்து hexchar(முழு x) _const_;
முழு unhexchar(எழுத்து c) _const_;

எழுத்து* hexmem(மாறிலி வெறுமை *p, size_t l) _nonnull_if_nonzero_(1, 2);
முழு unhexmem_full(மாறிலி எழுத்து *p, size_t l, bool secure, வெறுமை **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
நிலையான உள்வரி முழு unhexmem(மாறிலி எழுத்து *p, வெறுமை **ret_data, size_t *ret_size) {
        திரும்பு unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

எழுத்து base32hexchar(முழு x) _const_;
முழு unbase32hexchar(எழுத்து c) _const_;

எழுத்து base64char(முழு x) _const_;
எழுத்து urlsafe_base64char(முழு x) _const_;
முழு unbase64char(எழுத்து c) _const_;

எழுத்து* base32hexmem(மாறிலி வெறுமை *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
முழு unbase32hexmem(மாறிலி எழுத்து *p, size_t l, bool padding, வெறுமை **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(மாறிலி வெறுமை *p, size_t l, size_t வரி_break, எழுத்து **ret) _nonnull_if_nonzero_(1, 2);
நிலையான உள்வரி ssize_t base64mem(மாறிலி வெறுமை *p, size_t l, எழுத்து **ret) {
        திரும்பு base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                எழுத்து **prefix,
                size_t plen,
                மாறிலி வெறுமை *p,
                size_t l,
                size_t indent,
                size_t width);
முழு unbase64mem_full(மாறிலி எழுத்து *p, size_t l, bool secure, வெறுமை **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
நிலையான உள்வரி முழு unbase64mem(மாறிலி எழுத்து *p, வெறுமை **ret_data, size_t *ret_size) {
        திரும்பு unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

வெறுமை hexdump(FILE *f, மாறிலி வெறுமை *p, size_t s) _nonnull_if_nonzero_(2, 3);
