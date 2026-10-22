/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h>           /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ memfd_create_wrapper(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਹੁਲਕ);

ਪੂਰਨ memfd_new_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ extra_flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ memfd_new(ਅਚਲ ਅੱਖਰ *name) {
        ਵਾਪਸ memfd_new_ਭਰਿਆ(name, 0);
}

ਪੂਰਨ memfd_new_and_seal(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t sz) _nonnull_if_nonzero_(2, 3);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ memfd_new_and_seal_string(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ memfd_new_and_seal(name, s, SIZE_MAX);
}

ਪੂਰਨ memfd_ਸੈੱਟ_sealed(ਪੂਰਨ fd);
ਪੂਰਨ memfd_get_sealed(ਪੂਰਨ fd);
ਪੂਰਨ memfd_ਸੈੱਟ_sealed_gracefully(ਪੂਰਨ fd);

ਪੂਰਨ memfd_get_ਮਾਪ(ਪੂਰਨ fd, uint64_t *ret);
ਪੂਰਨ memfd_ਸੈੱਟ_ਮਾਪ(ਪੂਰਨ fd, uint64_t sz);
