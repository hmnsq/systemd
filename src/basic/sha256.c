/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

ចំនួនគត់ sha256_fd(ចំនួនគត់ fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        sa_nracanaa sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        សម្រាប់ (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, parimaa_na(buffer));
                ប្រសិនបើ (n < 0)
                        ត្រឡប់ -errno;
                ប្រសិនបើ (n == 0)
                        bha_nga;

                ប្រសិនបើ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ត្រឡប់ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_sha256(const var_na *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        ចំនួនគត់ r;

        ប្រសិនបើ (!sha256_is_valid(s))
                ត្រឡប់ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ត្រឡប់ 0;
}

bool sha256_is_valid(const var_na *s) {
        ត្រឡប់ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

var_na* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        ប្រសិនបើ (sz == SIZE_MAX)
                sz = strlen(buffer);

        ត្រឡប់ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
