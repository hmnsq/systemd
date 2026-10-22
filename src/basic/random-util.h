/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
வெறுமை random_bytes(வெறுமை *p, size_t n) _nonnull_if_nonzero_(1, 2);
முழு random_bytes_allocate_iovec(size_t n, கட்டமைப்பு iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
முழு crypto_random_bytes(வெறுமை *p, size_t n) _nonnull_if_nonzero_(1, 2);
முழு crypto_random_bytes_allocate_iovec(size_t n, கட்டமைப்பு iovec *ret);

நிலையான உள்வரி uint64_t random_u64(வெறுமை) {
        uint64_t u;
        random_bytes(&u, அளவு(u));
        திரும்பு u;
}

நிலையான உள்வரி uint32_t random_u32(வெறுமை) {
        uint32_t u;
        random_bytes(&u, அளவு(u));
        திரும்பு u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(வெறுமை);

முழு random_write_entropy(முழு fd, மாறிலி வெறுமை *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
