/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
शून्य random_bytes(शून्य *p, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
पूर्णांक random_bytes_allocate_iovec(माप_प्रकार n, संरचना iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
पूर्णांक crypto_random_bytes(शून्य *p, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
पूर्णांक crypto_random_bytes_allocate_iovec(माप_प्रकार n, संरचना iovec *ret);

स्थिर ओळीत uint64_t random_u64(शून्य) {
        uint64_t u;
        random_bytes(&u, आकार(u));
        परत u;
}

स्थिर ओळीत uint32_t random_u32(शून्य) {
        uint32_t u;
        random_bytes(&u, आकार(u));
        परत u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

माप_प्रकार random_pool_size(शून्य);

पूर्णांक random_write_entropy(पूर्णांक fd, स्थिरांक शून्य *seed, माप_प्रकार size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
