/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
خالی random_بائٹیں(خالی *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحیح random_بائٹیں_allocate_iovec(size_t n, ساخت iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
صحیح crypto_random_بائٹیں(خالی *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحیح crypto_random_بائٹیں_allocate_iovec(size_t n, ساخت iovec *ret);

ساکن اندرونی uint64_t random_u64(خالی) {
        uint64_t u;
        random_بائٹیں(&u, حجم(u));
        واپس u;
}

ساکن اندرونی uint32_t random_u32(خالی) {
        uint32_t u;
        random_بائٹیں(&u, حجم(u));
        واپس u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#تعریف_کرو RANDOM_POOL_سائز_MIN 32U
#تعریف_کرو RANDOM_POOL_سائز_MAX (10U*1024U*1024U)
#تعریف_کرو RANDOM_EFI_بیج_سائز 32U

size_t random_pool_سائز(خالی);

صحیح random_write_entropy(صحیح fd, ثابت خالی *بیج, size_t سائز, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
