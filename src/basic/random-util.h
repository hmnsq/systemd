/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
فراغ random_بائيٽون(فراغ *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحيح random_بائيٽون_allocate_iovec(size_t n, بنية iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
صحيح crypto_random_بائيٽون(فراغ *p, size_t n) _nonnull_if_nonzero_(1, 2);
صحيح crypto_random_بائيٽون_allocate_iovec(size_t n, بنية iovec *ret);

ساكن مضمن uint64_t random_u64(فراغ) {
        uint64_t u;
        random_بائيٽون(&u, حجم(u));
        أرجع u;
}

ساكن مضمن uint32_t random_u32(فراغ) {
        uint32_t u;
        random_بائيٽون(&u, حجم(u));
        أرجع u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#وصف_ڪريو RANDOM_POOL_سائيز_MIN 32U
#وصف_ڪريو RANDOM_POOL_سائيز_MAX (10U*1024U*1024U)
#وصف_ڪريو RANDOM_EFI_ٻج_سائيز 32U

size_t random_pool_سائيز(فراغ);

صحيح random_write_entropy(صحيح fd, ثابت فراغ *ٻج, size_t سائيز, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
