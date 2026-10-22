/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

цео sha256_fd(цео fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        struct sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        за (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, sizeof(buffer));
                ако (n < 0)
                        врати -errno;
                ако (n == 0)
                        break;

                ако (!INC_SAFE(&total_size, n) || total_size > max_size)
                        врати -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        врати 0;
}

цео parse_sha256(const char *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        цео r;

        ако (!sha256_is_valid(s))
                врати -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        ако (r < 0)
                врати r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        врати 0;
}

bool sha256_is_valid(const char *s) {
        врати s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

char* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        ако (sz == SIZE_MAX)
                sz = strlen(buffer);

        врати hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
