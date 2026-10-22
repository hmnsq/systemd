/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "sha256.h"
#ضمن "string-util.h"

صحيح sha256_fd(صحيح fd, uint64_t max_size, uint8_t ret[ساكن SHA256_DIGEST_SIZE]) {
        بنية sha256_ctx ctx;
        uint64_t total_size = 0;

        أكد(fd >= 0);

        sha256_init_ctx(&ctx);

        لكل (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = اقرأ(fd, buffer, حجم(buffer));
                ܐܢ (n < 0)
                        ܦܢܐ -errno;
                ܐܢ (n == 0)
                        اكسر;

                ܐܢ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ܦܢܐ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ܦܢܐ 0;
}

صحيح parse_sha256(ثابت حرف *s, uint8_t ret[ساكن SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = عدم;
        size_t size = 0;
        صحيح r;

        ܐܢ (!sha256_is_valid(s))
                ܦܢܐ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (فراغ**) &data, &size);
        ܐܢ (r < 0)
                ܦܢܐ r;
        أكد(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ܦܢܐ 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        ܦܢܐ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

حرف* sha256_direct_hex(ثابت فراغ *buffer, size_t sz) {
        أكد(buffer || sz == 0);

        ܐܢ (sz == SIZE_MAX)
                sz = strlen(buffer);

        ܦܢܐ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
