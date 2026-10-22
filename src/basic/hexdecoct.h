/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅକ୍ଷର octchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unoctchar(ଅକ୍ଷର c) _const_;

ଅକ୍ଷର decchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା undecchar(ଅକ୍ଷର c) _const_;

ଅକ୍ଷର hexchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhexchar(ଅକ୍ଷର c) _const_;

ଅକ୍ଷର* hexmem(ଅଚଳ ଶୂନ୍ୟ *p, size_t l) _nonnull_if_nonzero_(1, 2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhexmem_full(ଅଚଳ ଅକ୍ଷର *p, size_t l, bool secure, ଶୂନ୍ୟ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା unhexmem(ଅଚଳ ଅକ୍ଷର *p, ଶୂନ୍ୟ **ret_data, size_t *ret_size) {
        ଫେରନ୍ତୁ unhexmem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ଅକ୍ଷର base32hexchar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase32hexchar(ଅକ୍ଷର c) _const_;

ଅକ୍ଷର base64char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ଅକ୍ଷର urlsafe_base64char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64char(ଅକ୍ଷର c) _const_;

ଅକ୍ଷର* base32hexmem(ଅଚଳ ଶୂନ୍ୟ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase32hexmem(ଅଚଳ ଅକ୍ଷର *p, size_t l, bool padding, ଶୂନ୍ୟ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_full(ଅଚଳ ଶୂନ୍ୟ *p, size_t l, size_t ଧାଡ଼ି_break, ଅକ୍ଷର **ret) _nonnull_if_nonzero_(1, 2);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ssize_t base64mem(ଅଚଳ ଶୂନ୍ୟ *p, size_t l, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ base64mem_full(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                ଅକ୍ଷର **prefix,
                size_t plen,
                ଅଚଳ ଶୂନ୍ୟ *p,
                size_t l,
                size_t indent,
                size_t width);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64mem_full(ଅଚଳ ଅକ୍ଷର *p, size_t l, bool secure, ଶୂନ୍ୟ **ret_data, size_t *ret_size) _nonnull_if_nonzero_(1, 2);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା unbase64mem(ଅଚଳ ଅକ୍ଷର *p, ଶୂନ୍ୟ **ret_data, size_t *ret_size) {
        ଫେରନ୍ତୁ unbase64mem_full(p, SIZE_MAX, false, ret_data, ret_size);
}

ଶୂନ୍ୟ hexdump(FILE *f, ଅଚଳ ଶୂନ୍ୟ *p, size_t s) _nonnull_if_nonzero_(2, 3);
