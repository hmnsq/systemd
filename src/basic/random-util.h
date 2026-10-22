/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
শূন্য random_bytes(শূন্য *p, size_t n) _nonnull_if_nonzero_(1, 2);
পূর্ণাঙ্ক random_bytes_allocate_iovec(size_t n, সংরচনা iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
পূর্ণাঙ্ক crypto_random_bytes(শূন্য *p, size_t n) _nonnull_if_nonzero_(1, 2);
পূর্ণাঙ্ক crypto_random_bytes_allocate_iovec(size_t n, সংরচনা iovec *ret);

স্থির অন্তঃস্থ uint64_t random_u64(শূন্য) {
        uint64_t u;
        random_bytes(&u, পরিমাণ(u));
        প্রত্যাগম u;
}

স্থির অন্তঃস্থ uint32_t random_u32(শূন্য) {
        uint32_t u;
        random_bytes(&u, পরিমাণ(u));
        প্রত্যাগম u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(শূন্য);

পূর্ণাঙ্ক random_write_entropy(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
