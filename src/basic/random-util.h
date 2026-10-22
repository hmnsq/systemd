/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
শূন্য random_bytes(শূন্য *p, size_t n) _nonnull_if_nonzero_(1, 2);
পূর্ণসংখ্যা random_bytes_allocate_iovec(size_t n, কাঠামো iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
পূর্ণসংখ্যা crypto_random_bytes(শূন্য *p, size_t n) _nonnull_if_nonzero_(1, 2);
পূর্ণসংখ্যা crypto_random_bytes_allocate_iovec(size_t n, কাঠামো iovec *ret);

স্থির অন্তঃরেখ uint64_t random_u64(শূন্য) {
        uint64_t u;
        random_bytes(&u, আকার(u));
        ফেরত u;
}

স্থির অন্তঃরেখ uint32_t random_u32(শূন্য) {
        uint32_t u;
        random_bytes(&u, আকার(u));
        ফেরত u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_আকার_MIN 32U
#define RANDOM_POOL_আকার_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_আকার 32U

size_t random_pool_আকার(শূন্য);

পূর্ণসংখ্যা random_write_entropy(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *seed, size_t আকার_2, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
