/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ sha256_fd(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t max_size, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_SIZE]) {
        ಸಂರಚನಾ sha256_ctx ctx;
        uint64_t total_size = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        sha256_init_ctx(&ctx);

        ಪ್ರತಿ (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = ಪಠ(fd, buffer, ಪರಿಮಾಣ(buffer));
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (n == 0)
                        ಭಙ್ಗ;

                ಯದಿ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_sha256(ಧ್ರುವ ವರ್ಣ *s, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = ರಿಕ್ತ;
        size_t size = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!sha256_is_valid(s))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (ಶೂನ್ಯ**) &data, &size);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತಿಜ್ಞಾ(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool sha256_is_valid(ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

ವರ್ಣ* sha256_direct_hex(ಧ್ರುವ ಶೂನ್ಯ *buffer, size_t sz) {
        ಪ್ರತಿಜ್ಞಾ(buffer || sz == 0);

        ಯದಿ (sz == SIZE_MAX)
                sz = strlen(buffer);

        ಪ್ರತ್ಯಾಗಮ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
