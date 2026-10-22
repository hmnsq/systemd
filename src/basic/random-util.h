/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
శూన్య random_బైట్లు(శూన్య *p, size_t n) _nonnull_if_nonzero_(1, 2);
పూర్ణాంకం random_బైట్లు_allocate_iovec(size_t n, నిర్మాణం iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
పూర్ణాంకం crypto_random_బైట్లు(శూన్య *p, size_t n) _nonnull_if_nonzero_(1, 2);
పూర్ణాంకం crypto_random_బైట్లు_allocate_iovec(size_t n, నిర్మాణం iovec *ret);

స్థిర అంతఃవరుస uint64_t random_u64(శూన్య) {
        uint64_t u;
        random_బైట్లు(&u, పరిమాణం(u));
        తిరిగి u;
}

స్థిర అంతఃవరుస uint32_t random_u32(శూన్య) {
        uint32_t u;
        random_బైట్లు(&u, పరిమాణం(u));
        తిరిగి u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#నిర్వచించు RANDOM_POOL_కొలత_MIN 32U
#నిర్వచించు RANDOM_POOL_కొలత_MAX (10U*1024U*1024U)
#నిర్వచించు RANDOM_EFI_విత్తనం_కొలత 32U

size_t random_pool_కొలత(శూన్య);

పూర్ణాంకం random_write_entropy(పూర్ణాంకం fd, స్థిరరాశి శూన్య *విత్తనం, size_t కొలత, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
