/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* Returns random bytes suitable for most uses, but may be insecure sometimes. */
शून्य random_अष्टकानि(शून्य *p, size_t n) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क random_अष्टकानि_allocate_iovec(size_t n, संरचना iovec *ret);

/* Returns secure random bytes after waiting for the RNG to initialize. */
पूर्णाङ्क crypto_random_अष्टकानि(शून्य *p, size_t n) _nonnull_if_nonzero_(1, 2);
पूर्णाङ्क crypto_random_अष्टकानि_allocate_iovec(size_t n, संरचना iovec *ret);

स्थिर अन्तःस्थ uint64_t random_u64(शून्य) {
        uint64_t u;
        random_अष्टकानि(&u, परिमाण(u));
        प्रत्यागम u;
}

स्थिर अन्तःस्थ uint32_t random_u32(शून्य) {
        uint32_t u;
        random_अष्टकानि(&u, परिमाण(u));
        प्रत्यागम u;
}

/* Some limits on the pool sizes when we deal with the kernel random pool */
#परिभाषय RANDOM_POOL_आकार_MIN 32U
#परिभाषय RANDOM_POOL_आकार_MAX (10U*1024U*1024U)
#परिभाषय RANDOM_EFI_बीजम्_आकार 32U

size_t random_pool_आकार(शून्य);

पूर्णाङ्क random_write_entropy(पूर्णाङ्क fd, ध्रुव शून्य *बीजम्, size_t आकार, bool credit) _nonnull_if_nonzero_(2, 3);

uint64_t random_u64_range(uint64_t max);
