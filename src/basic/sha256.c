/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

整数 sha256_fd(整数 fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        struct sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        ために (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, sizeof(buffer));
                もし (n < 0)
                        戻る -errno;
                もし (n == 0)
                        break;

                もし (!INC_SAFE(&total_size, n) || total_size > max_size)
                        戻る -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        戻る 0;
}

整数 parse_sha256(const char *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        整数 r;

        もし (!sha256_is_valid(s))
                戻る -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        もし (r < 0)
                戻る r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        戻る 0;
}

bool sha256_is_valid(const char *s) {
        戻る s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

char* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        もし (sz == SIZE_MAX)
                sz = strlen(buffer);

        戻る hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
