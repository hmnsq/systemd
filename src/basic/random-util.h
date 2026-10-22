/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
ശൂന്യം random_bytes(ശൂന്യം *p, size_t n) _nonnull_if_nonzero_(1, 2);
പൂർണ്ണസംഖ്യ random_bytes_allocate_iovec(size_t n, ഘടന iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
പൂർണ്ണസംഖ്യ crypto_random_bytes(ശൂന്യം *p, size_t n) _nonnull_if_nonzero_(1, 2);
പൂർണ്ണസംഖ്യ crypto_random_bytes_allocate_iovec(size_t n, ഘടന iovec *ret);

സ്ഥിതം വരിയിൽ uint64_t random_u64(ശൂന്യം) {
        uint64_t u;
        random_bytes(&u, വലുപ്പം(u));
        തിരികെ u;
}

സ്ഥിതം വരിയിൽ uint32_t random_u32(ശൂന്യം) {
        uint32_t u;
        random_bytes(&u, വലുപ്പം(u));
        തിരികെ u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(ശൂന്യം);

പൂർണ്ണസംഖ്യ random_write_entropy(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
