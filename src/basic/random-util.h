/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
व्योम random_bytes(व्योम *p, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
पूर्णांक random_bytes_allocate_iovec(माप_प्रकार n, काष्ठा iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
पूर्णांक crypto_random_bytes(व्योम *p, माप_प्रकार n) _nonnull_if_nonzero_(1, 2);
पूर्णांक crypto_random_bytes_allocate_iovec(माप_प्रकार n, काष्ठा iovec *ret);

अटल अंतरभूत uint64_t random_u64(व्योम) {
        uint64_t u;
        random_bytes(&u, माप(u));
        वापस u;
}

अटल अंतरभूत uint32_t random_u32(व्योम) {
        uint32_t u;
        random_bytes(&u, माप(u));
        वापस u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#घोषणा RANDOM_POOL_आकार_MIN 32U
#घोषणा RANDOM_POOL_आकार_MAX (10U*1024U*1024U)
#घोषणा RANDOM_EFI_SEED_आकार 32U

माप_प्रकार random_pool_आकार(व्योम);

पूर्णांक random_write_entropy(पूर्णांक fd, स्थिर व्योम *seed, माप_प्रकार आकार, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
