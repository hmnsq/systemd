/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

نویسه octchar(صحیح x) _const_;
صحیح unoctchar(نویسه c) _const_;

نویسه decchar(صحیح x) _const_;
صحیح undecchar(نویسه c) _const_;

نویسه hexchar(صحیح x) _const_;
صحیح unhexchar(نویسه c) _const_;

نویسه* hexmem(ثابت تهی *p, size_t l) _nonnull_if_nonzero_(1, 2);
صحیح unhexmem_full(ثابت نویسه *p, size_t l, bool secure, تهی **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ایستا درونخطی صحیح unhexmem(ثابت نویسه *p, تهی **ret_data, size_t *ret_size) {
        بازگشت unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

نویسه base32hexchar(صحیح x) _const_;
صحیح unbase32hexchar(نویسه c) _const_;

نویسه base64char(صحیح x) _const_;
نویسه urlsafe_base64char(صحیح x) _const_;
صحیح unbase64char(نویسه c) _const_;

نویسه* base32hexmem(ثابت تهی *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
صحیح unbase32hexmem(ثابت نویسه *p, size_t l, bool padding, تهی **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ثابت تهی *p, size_t l, size_t سطر_break, نویسه **ret) _nonnull_if_nonzero_(1, 2);
ایستا درونخطی ssize_t base64mem(ثابت تهی *p, size_t l, نویسه **ret) {
        بازگشت base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                نویسه **prefix,
                size_t plen,
                ثابت تهی *p,
                size_t l,
                size_t indent,
                size_t width);
صحیح unbase64mem_full(ثابت نویسه *p, size_t l, bool secure, تهی **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ایستا درونخطی صحیح unbase64mem(ثابت نویسه *p, تهی **ret_data, size_t *ret_size) {
        بازگشت unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

تهی hexdump(FILE *f, ثابت تهی *p, size_t s) _nonnull_if_nonzero_(2, 3);
