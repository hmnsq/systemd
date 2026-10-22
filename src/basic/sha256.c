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

        为 (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, sizeof(buffer));
                如果 (n < 0)
                        返回 -errno;
                如果 (n == 0)
                        break;

                如果 (!INC_SAFE(&total_size, n) || total_size > max_size)
                        返回 -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        返回 0;
}

整数 parse_sha256(const char *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        整数 r;

        如果 (!sha256_is_valid(s))
                返回 -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        如果 (r < 0)
                返回 r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        返回 0;
}

bool sha256_is_valid(const char *s) {
        返回 s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

char* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        如果 (sz == SIZE_MAX)
                sz = strlen(buffer);

        返回 hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
