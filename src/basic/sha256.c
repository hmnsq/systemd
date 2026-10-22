/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "sha256.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা sha256_fd(পূর্ণসংখ্যা fd, uint64_t max_আকার, uint8_t ret[স্থির SHA256_DIGEST_আকার]) {
        কাঠামো sha256_ctx ctx;
        uint64_t যোগ_আকার = 0;

        প্রতিজ্ঞা(fd >= 0);

        sha256_init_ctx(&ctx);

        জন্য (;;) {
                uint8_t বাফার[64 * 1024];
                ssize_t n;

                n = পঠ(fd, বাফার, আকার(বাফার));
                যদি (n < 0)
                        ফেরত -errno;
                যদি (n == 0)
                        বিরতি;

                যদি (!INC_SAFE(&যোগ_আকার, n) || যোগ_আকার > max_আকার)
                        ফেরত -EFBIG;

                sha256_process_bytes(বাফার, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ফেরত 0;
}

পূর্ণসংখ্যা parse_sha256(ধ্রুবক অক্ষর *s, uint8_t ret[স্থির SHA256_DIGEST_আকার]) {
        _cleanup_free_ uint8_t *তথ্য = ফাঁকা;
        size_t আকার_2 = 0;
        পূর্ণসংখ্যা r;

        যদি (!sha256_is_valid(s))
                ফেরত -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_আকার * 2, false, (শূন্য**) &তথ্য, &আকার_2);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(আকার_2 == SHA256_DIGEST_আকার);

        memcpy(ret, তথ্য, আকার_2);
        ফেরত 0;
}

bool sha256_is_valid(ধ্রুবক অক্ষর *s) {
        ফেরত s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_আকার * 2);
}

অক্ষর* sha256_direct_hex(ধ্রুবক শূন্য *বাফার, size_t sz) {
        প্রতিজ্ঞা(বাফার || sz == 0);

        যদি (sz == SIZE_MAX)
                sz = strlen(বাফার);

        ফেরত hexmem(SHA256_DIRECT(বাফার, sz), SHA256_DIGEST_আকার);
}
