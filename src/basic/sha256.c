/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "hexdecoct.h"
#شامل "sha256.h"
#شامل "string-util.h"

صحیح sha256_fd(صحیح fd, uint64_t max_size, uint8_t ret[ایستا SHA256_DIGEST_SIZE]) {
        ساختار sha256_ctx ctx;
        uint64_t total_size = 0;

        أكد(fd >= 0);

        sha256_init_ctx(&ctx);

        برای (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = اقرأ(fd, buffer, اندازه(buffer));
                اگر (n < 0)
                        بازگشت -errno;
                اگر (n == 0)
                        بشکن;

                اگر (!INC_SAFE(&total_size, n) || total_size > max_size)
                        بازگشت -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        بازگشت 0;
}

صحیح parse_sha256(ثابت نویسه *s, uint8_t ret[ایستا SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = پوچ;
        size_t size = 0;
        صحیح r;

        اگر (!sha256_is_valid(s))
                بازگشت -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (تهی**) &data, &size);
        اگر (r < 0)
                بازگشت r;
        أكد(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        بازگشت 0;
}

bool sha256_is_valid(ثابت نویسه *s) {
        بازگشت s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

نویسه* sha256_direct_hex(ثابت تهی *buffer, size_t sz) {
        أكد(buffer || sz == 0);

        اگر (sz == SIZE_MAX)
                sz = strlen(buffer);

        بازگشت hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
