/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "sha256.h"
#ضمن "string-util.h"

صحيح sha256_fd(صحيح fd, uint64_t max_سائيز, uint8_t ret[ساكن SHA256_DIGEST_سائيز]) {
        بنية sha256_ctx ctx;
        uint64_t ڪل_سائيز = 0;

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

                إذا (!INC_محفوظ(&ڪل_سائيز, n) || ڪل_سائيز > max_سائيز)
                        أرجع -EFBIG;

                sha256_process_بائيٽون(بفر, n, &ctx);
        }

        sha256_پڄاڻي_ctx(&ctx, ret);
        أرجع 0;
}

صحيح parse_sha256(ثابت حرف *s, uint8_t ret[ساكن SHA256_DIGEST_سائيز]) {
        _cleanup_free_ uint8_t *معلومات = عدم;
        size_t سائيز = 0;
        صحيح r;

        إذا (!sha256_is_valid(s))
                أرجع -EINVAL;

        r = unhexmem_ڀريل(s, SHA256_DIGEST_سائيز * 2, false, (فراغ**) &معلومات, &سائيز);
        إذا (r < 0)
                أرجع r;
        أكد(سائيز == SHA256_DIGEST_سائيز);

        يادگيري_نقل(ret, معلومات, سائيز);
        أرجع 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        أرجع s && اندريون_charset(s, HEXDIGITS) && (لڙي_ڊيگهه(s) == SHA256_DIGEST_سائيز * 2);
}

حرف* sha256_سڌو_hex(ثابت فراغ *بفر, size_t sz) {
        أكد(بفر || sz == 0);

        إذا (sz == SIZE_MAX)
                sz = لڙي_ڊيگهه(بفر);

        أرجع hexmem(SHA256_سڌو(بفر, sz), SHA256_DIGEST_سائيز);
}
