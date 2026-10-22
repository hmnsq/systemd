/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
ਖਾਲੀ random_ਬਾਈਟਾਂ(ਖਾਲੀ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ਪੂਰਨ random_ਬਾਈਟਾਂ_allocate_iovec(size_t n, ਬਣਤਰ iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
ਪੂਰਨ crypto_random_ਬਾਈਟਾਂ(ਖਾਲੀ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ਪੂਰਨ crypto_random_ਬਾਈਟਾਂ_allocate_iovec(size_t n, ਬਣਤਰ iovec *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t random_u64(ਖਾਲੀ) {
        uint64_t u;
        random_ਬਾਈਟਾਂ(&u, ਆਕਾਰ(u));
        ਵਾਪਸ u;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t random_u32(ਖਾਲੀ) {
        uint32_t u;
        random_ਬਾਈਟਾਂ(&u, ਆਕਾਰ(u));
        ਵਾਪਸ u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RANDOM_POOL_ਮਾਪ_MIN 32U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RANDOM_POOL_ਮਾਪ_MAX (10U*1024U*1024U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RANDOM_EFI_ਬੀਜ_ਮਾਪ 32U

size_t random_pool_ਮਾਪ(ਖਾਲੀ);

ਪੂਰਨ random_write_entropy(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *ਬੀਜ, size_t ਮਾਪ, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
