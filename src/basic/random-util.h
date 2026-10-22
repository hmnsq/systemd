/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
فراغ random_bytes(فراغ *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحيح random_bytes_allocate_iovec(size_t n, بنية iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
صحيح crypto_random_bytes(فراغ *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحيح crypto_random_bytes_allocate_iovec(size_t n, بنية iovec *ret);

ساكن مضمن uint64_t random_u64(فراغ) {
        uint64_t u;
        random_bytes(&u, حجم(u));
        إرجاع u;
}

ساكن مضمن uint32_t random_u32(فراغ) {
        uint32_t u;
        random_bytes(&u, حجم(u));
        إرجاع u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(فراغ);

صحيح random_write_entropy(صحيح fd, ثابت فراغ *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
