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
                إذا (n < 0)
                        إرجاع -errno;
                إذا (n == 0)
                        اكسر;

                إذا (!INC_SAFE(&total_size, n) || total_size > max_size)
                        إرجاع -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        إرجاع 0;
}

صحيح parse_sha256(ثابت حرف *s, uint8_t ret[ساكن SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = عدم;
        size_t size = 0;
        صحيح r;

        إذا (!sha256_is_valid(s))
                إرجاع -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (فراغ**) &data, &size);
        إذا (r < 0)
                إرجاع r;
        أكد(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        إرجاع 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        إرجاع s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

حرف* sha256_direct_hex(ثابت فراغ *buffer, size_t sz) {
        أكد(buffer || sz == 0);

        إذا (sz == SIZE_MAX)
                sz = strlen(buffer);

        إرجاع hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
