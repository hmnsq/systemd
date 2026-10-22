/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
શૂન્ય random_bytes(શૂન્ય *p, size_t n) _nonnull_if_nonzero_(1, 2);
પૂર્ણાંક random_bytes_allocate_iovec(size_t n, સંરચના iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
પૂર્ણાંક crypto_random_bytes(શૂન્ય *p, size_t n) _nonnull_if_nonzero_(1, 2);
પૂર્ણાંક crypto_random_bytes_allocate_iovec(size_t n, સંરચના iovec *ret);

સ્થિર અંતઃરેખ uint64_t random_u64(શૂન્ય) {
        uint64_t u;
        random_bytes(&u, કદ(u));
        પાછા u;
}

સ્થિર અંતઃરેખ uint32_t random_u32(શૂન્ય) {
        uint32_t u;
        random_bytes(&u, કદ(u));
        પાછા u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(શૂન્ય);

પૂર્ણાંક random_write_entropy(પૂર્ણાંક fd, અચલ શૂન્ય *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
