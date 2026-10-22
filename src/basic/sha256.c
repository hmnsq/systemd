/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

целое sha256_fd(целое fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        struct sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        для (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, sizeof(buffer));
                если (n < 0)
                        вернуть -errno;
                если (n == 0)
                        break;

                если (!INC_SAFE(&total_size, n) || total_size > max_size)
                        вернуть -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        вернуть 0;
}

целое parse_sha256(const char *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        целое r;

        если (!sha256_is_valid(s))
                вернуть -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        если (r < 0)
                вернуть r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        вернуть 0;
}

bool sha256_is_valid(const char *s) {
        вернуть s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

char* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        если (sz == SIZE_MAX)
                sz = strlen(buffer);

        вернуть hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
