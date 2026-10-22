/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
تهی random_bytes(تهی *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحیح random_bytes_allocate_iovec(size_t n, ساختار iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
صحیح crypto_random_bytes(تهی *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحیح crypto_random_bytes_allocate_iovec(size_t n, ساختار iovec *ret);

ایستا درونخطی uint64_t random_u64(تهی) {
        uint64_t u;
        random_bytes(&u, اندازه(u));
        بازگشت u;
}

ایستا درونخطی uint32_t random_u32(تهی) {
        uint32_t u;
        random_bytes(&u, اندازه(u));
        بازگشت u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(تهی);

صحیح random_write_entropy(صحیح fd, ثابت تهی *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
