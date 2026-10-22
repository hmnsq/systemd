/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "sha256.h"
#ضمن "string-util.h"

صحيح sha256_fd(صحيح fd, uint64_t max_ناپ, uint8_t ret[ساكن SHA256_DIGEST_ناپ]) {
        بنية sha256_ctx ctx;
        uint64_t کل_ناپ = 0;

        أكد(fd >= 0);

        sha256_init_ctx(&ctx);

        لكل (;;) {
                uint8_t بفر[64 * 1024];
                ssize_t n;

                n = اقرأ(fd, بفر, حجم(بفر));
                إذا (n < 0)
                        أرجع -errno;
                إذا (n == 0)
                        اكسر;

                إذا (!INC_محفوظ(&کل_ناپ, n) || کل_ناپ > max_ناپ)
                        أرجع -EFBIG;

                sha256_process_بائٹاں(بفر, n, &ctx);
        }

        sha256_انت_ctx(&ctx, ret);
        أرجع 0;
}

صحيح parse_sha256(ثابت حرف *s, uint8_t ret[ساكن SHA256_DIGEST_ناپ]) {
        _cleanup_free_ uint8_t *انکڑے = عدم;
        size_t ناپ = 0;
        صحيح r;

        إذا (!sha256_is_valid(s))
                أرجع -EINVAL;

        r = unhexmem_بھریا(s, SHA256_DIGEST_ناپ * 2, false, (فراغ**) &انکڑے, &ناپ);
        إذا (r < 0)
                أرجع r;
        أكد(ناپ == SHA256_DIGEST_ناپ);

        حافظہ_نقل(ret, انکڑے, ناپ);
        أرجع 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        أرجع s && وچ_charset(s, HEXDIGITS) && (لڑی_لمبائی(s) == SHA256_DIGEST_ناپ * 2);
}

حرف* sha256_سدھا_hex(ثابت فراغ *بفر, size_t sz) {
        أكد(بفر || sz == 0);

        إذا (sz == SIZE_MAX)
                sz = لڑی_لمبائی(بفر);

        أرجع hexmem(SHA256_سدھا(بفر, sz), SHA256_DIGEST_ناپ);
}
