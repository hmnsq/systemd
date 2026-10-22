/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
ಶೂನ್ಯ random_ಬೈಟುಗಳು(ಶೂನ್ಯ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಂಕ random_ಬೈಟುಗಳು_allocate_iovec(size_t n, ರಚನೆ iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
ಪೂರ್ಣಾಂಕ crypto_random_ಬೈಟುಗಳು(ಶೂನ್ಯ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ಪೂರ್ಣಾಂಕ crypto_random_ಬೈಟುಗಳು_allocate_iovec(size_t n, ರಚನೆ iovec *ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t random_u64(ಶೂನ್ಯ) {
        uint64_t u;
        random_ಬೈಟುಗಳು(&u, ಗಾತ್ರ(u));
        ಹಿಂತಿರುಗಿ u;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t random_u32(ಶೂನ್ಯ) {
        uint32_t u;
        random_ಬೈಟುಗಳು(&u, ಗಾತ್ರ(u));
        ಹಿಂತಿರುಗಿ u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#ವ್ಯಾಖ್ಯಾನಿಸು RANDOM_POOL_ಗಾತ್ರ_MIN 32U
#ವ್ಯಾಖ್ಯಾನಿಸು RANDOM_POOL_ಗಾತ್ರ_MAX (10U*1024U*1024U)
#ವ್ಯಾಖ್ಯಾನಿಸು RANDOM_EFI_ಬೀಜ_ಗಾತ್ರ 32U

size_t random_pool_ಗಾತ್ರ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ random_write_entropy(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬೀಜ, size_t ಗಾತ್ರ_2, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
