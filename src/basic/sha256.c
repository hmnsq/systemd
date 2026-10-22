/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "hexdecoct.h"
#ضمن "sha256.h"
#ضمن "string-util.h"

صحيح sha256_fd(صحيح fd, uint64_t max_اندازه, uint8_t ret[ساكن SHA256_DIGEST_اندازه]) {
        بنية sha256_ctx ctx;
        uint64_t مجموع_اندازه = 0;

        أكد(fd >= 0);

        sha256_init_ctx(&ctx);

        لكل (;;) {
                uint8_t میانگیر[64 * 1024];
                ssize_t n;

                n = اقرأ(fd, میانگیر, حجم(میانگیر));
                إذا (n < 0)
                        أرجع -errno;
                إذا (n == 0)
                        اكسر;

                إذا (!INC_امن(&مجموع_اندازه, n) || مجموع_اندازه > max_اندازه)
                        أرجع -EFBIG;

                sha256_process_بایتها(میانگیر, n, &ctx);
        }

        sha256_پایان_ctx(&ctx, ret);
        أرجع 0;
}

صحيح parse_sha256(ثابت حرف *s, uint8_t ret[ساكن SHA256_DIGEST_اندازه]) {
        _cleanup_free_ uint8_t *داده = عدم;
        size_t اندازه = 0;
        صحيح r;

        إذا (!sha256_is_valid(s))
                أرجع -EINVAL;

        r = unhexmem_پر(s, SHA256_DIGEST_اندازه * 2, false, (فراغ**) &داده, &اندازه);
        إذا (r < 0)
                أرجع r;
        أكد(اندازه == SHA256_DIGEST_اندازه);

        کپی_حافظه(ret, داده, اندازه);
        أرجع 0;
}

bool sha256_is_valid(ثابت حرف *s) {
        أرجع s && درون_charset(s, HEXDIGITS) && (طول_رشته(s) == SHA256_DIGEST_اندازه * 2);
}

حرف* sha256_مستقیم_hex(ثابت فراغ *میانگیر, size_t sz) {
        أكد(میانگیر || sz == 0);

        إذا (sz == SIZE_MAX)
                sz = طول_رشته(میانگیر);

        أرجع hexmem(SHA256_مستقیم(میانگیر, sz), SHA256_DIGEST_اندازه);
}
