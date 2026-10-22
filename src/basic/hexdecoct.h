/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਅੱਖਰ octchar(ਪੂਰਨ x) _const_;
ਪੂਰਨ unoctchar(ਅੱਖਰ c) _const_;

ਅੱਖਰ decchar(ਪੂਰਨ x) _const_;
ਪੂਰਨ undecchar(ਅੱਖਰ c) _const_;

ਅੱਖਰ hexchar(ਪੂਰਨ x) _const_;
ਪੂਰਨ unhexchar(ਅੱਖਰ c) _const_;

ਅੱਖਰ* hexmem(ਅਚਲ ਖਾਲੀ *p, size_t l) _nonnull_if_nonzero_(1, 2);
ਪੂਰਨ unhexmem_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, size_t l, bool secure, ਖਾਲੀ **ret_ਅੰਕੜੇ, size_t *ret_ਮਾਪ) _nonnull_if_nonzero_(1, 2);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ unhexmem(ਅਚਲ ਅੱਖਰ *p, ਖਾਲੀ **ret_ਅੰਕੜੇ, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ unhexmem_ਭਰਿਆ(p, SIZE_MAX, false, ret_ਅੰਕੜੇ, ret_ਮਾਪ);
}

ਅੱਖਰ base32hexchar(ਪੂਰਨ x) _const_;
ਪੂਰਨ unbase32hexchar(ਅੱਖਰ c) _const_;

ਅੱਖਰ base64char(ਪੂਰਨ x) _const_;
ਅੱਖਰ urlsafe_base64char(ਪੂਰਨ x) _const_;
ਪੂਰਨ unbase64char(ਅੱਖਰ c) _const_;

ਅੱਖਰ* base32hexmem(ਅਚਲ ਖਾਲੀ *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
ਪੂਰਨ unbase32hexmem(ਅਚਲ ਅੱਖਰ *p, size_t l, bool padding, ਖਾਲੀ **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_ਭਰਿਆ(ਅਚਲ ਖਾਲੀ *p, size_t l, size_t ਸਤਰ_break, ਅੱਖਰ **ret) _nonnull_if_nonzero_(1, 2);
ਸਥਿਰ ਸਤਰਅੰਦਰ ssize_t base64mem(ਅਚਲ ਖਾਲੀ *p, size_t l, ਅੱਖਰ **ret) {
        ਵਾਪਸ base64mem_ਭਰਿਆ(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                ਅੱਖਰ **ਅਗੇਤਰ,
                size_t plen,
                ਅਚਲ ਖਾਲੀ *p,
                size_t l,
                size_t indent,
                size_t width);
ਪੂਰਨ unbase64mem_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, size_t l, bool secure, ਖਾਲੀ **ret_ਅੰਕੜੇ, size_t *ret_ਮਾਪ) _nonnull_if_nonzero_(1, 2);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ unbase64mem(ਅਚਲ ਅੱਖਰ *p, ਖਾਲੀ **ret_ਅੰਕੜੇ, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ unbase64mem_ਭਰਿਆ(p, SIZE_MAX, false, ret_ਅੰਕੜੇ, ret_ਮਾਪ);
}

ਖਾਲੀ hexdump(FILE *f, ਅਚਲ ਖਾਲੀ *p, size_t s) _nonnull_if_nonzero_(2, 3);
