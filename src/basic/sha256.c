/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "hexdecoct.h"
#شامل "sha256.h"
#شامل "string-util.h"

صحیح sha256_fd(صحیح fd, uint64_t max_سائز, uint8_t ret[ساکن SHA256_DIGEST_سائز]) {
        ساخت sha256_ctx ctx;
        uint64_t کل_سائز = 0;

        أكد(fd >= 0);

        sha256_init_ctx(&ctx);

        برائے (;;) {
                uint8_t بفر[64 * 1024];
                ssize_t n;

                n = اقرأ(fd, بفر, حجم(بفر));
                اگر (n < 0)
                        واپس -errno;
                اگر (n == 0)
                        توڑو;

                اگر (!INC_محفوظ(&کل_سائز, n) || کل_سائز > max_سائز)
                        واپس -EFBIG;

                sha256_process_بائٹیں(بفر, n, &ctx);
        }

        sha256_اختتام_ctx(&ctx, ret);
        واپس 0;
}

صحیح parse_sha256(ثابت حرف *s, uint8_t ret[ساکن SHA256_DIGEST_سائز]) {
        _cleanup_free_ uint8_t *ڈیٹا = تہی;
        size_t سائز = 0;
        صحیح r;

        اگر (!sha256_is_valid(s))
                واپس -EINVAL;

        r = unhexmem_بھرا(s, SHA256_DIGEST_سائز * 2, false, (خالی**) &ڈیٹا, &سائز);
        اگر (r < 0)
                واپس r;
        أكد(سائز == SHA256_DIGEST_سائز);

        حافظہ_نقل(ret, ڈیٹا, سائز);
        واپس 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        واپس s && اندر_charset(s, HEXDIGITS) && (لڑی_لمبائی(s) == SHA256_DIGEST_سائز * 2);
}

حرف* sha256_براہ_راست_hex(ثابت خالی *بفر, size_t sz) {
        أكد(بفر || sz == 0);

        اگر (sz == SIZE_MAX)
                sz = لڑی_لمبائی(بفر);

        واپس hexmem(SHA256_براہ_راست(بفر, sz), SHA256_DIGEST_سائز);
}
