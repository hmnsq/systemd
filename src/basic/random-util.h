/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
ಶೂನ್ಯ random_bytes(ಶೂನ್ಯ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಙ್ಕ random_bytes_allocate_iovec(size_t n, ಸಂರಚನಾ iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
ಪೂರ್ಣಾಙ್ಕ crypto_random_bytes(ಶೂನ್ಯ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಙ್ಕ crypto_random_bytes_allocate_iovec(size_t n, ಸಂರಚನಾ iovec *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint64_t random_u64(ಶೂನ್ಯ) {
        uint64_t u;
        random_bytes(&u, ಪರಿಮಾಣ(u));
        ಪ್ರತ್ಯಾಗಮ u;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint32_t random_u32(ಶೂನ್ಯ) {
        uint32_t u;
        random_bytes(&u, ಪರಿಮಾಣ(u));
        ಪ್ರತ್ಯಾಗಮ u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ random_write_entropy(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
