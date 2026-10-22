/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

ཧྲིལ sha256_fd(ཧྲིལ fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        sa_nracanaa sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        ཆེད་དུ (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, parimaa_na(buffer));
                གལ་ཏེ (n < 0)
                        ཕྱིར -errno;
                གལ་ཏེ (n == 0)
                        bha_nga;

                གལ་ཏེ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ཕྱིར -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ཕྱིར 0;
}

ཧྲིལ parse_sha256(const var_na *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        ཧྲིལ r;

        གལ་ཏེ (!sha256_is_valid(s))
                ཕྱིར -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ཕྱིར 0;
}

bool sha256_is_valid(const var_na *s) {
        ཕྱིར s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

var_na* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        གལ་ཏེ (sz == SIZE_MAX)
                sz = strlen(buffer);

        ཕྱིར hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
