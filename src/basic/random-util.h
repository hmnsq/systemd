/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
ଶୂନ୍ୟ random_bytes(ଶୂନ୍ୟ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା random_bytes_allocate_iovec(size_t n, ସଂରଚନା iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypto_random_bytes(ଶୂନ୍ୟ *p, size_t n) _nonnull_if_nonzero_(1, 2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା crypto_random_bytes_allocate_iovec(size_t n, ସଂରଚନା iovec *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t random_u64(ଶୂନ୍ୟ) {
        uint64_t u;
        random_bytes(&u, ଆକାର(u));
        ଫେରନ୍ତୁ u;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t random_u32(ଶୂନ୍ୟ) {
        uint32_t u;
        random_bytes(&u, ଆକାର(u));
        ଫେରନ୍ତୁ u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#define RANDOM_POOL_SIZE_MIN 32U
#define RANDOM_POOL_SIZE_MAX (10U*1024U*1024U)
#define RANDOM_EFI_SEED_SIZE 32U

size_t random_pool_size(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା random_write_entropy(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *seed, size_t size, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
