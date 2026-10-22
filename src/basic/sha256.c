/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "hexdecoct.h"
#অন্তর্ভাৱ "sha256.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক sha256_fd(পূর্ণাঙ্ক fd, uint64_t max_size, uint8_t ret[স্থির SHA256_DIGEST_SIZE]) {
        সংরচনা sha256_ctx ctx;
        uint64_t total_size = 0;

        প্রতিজ্ঞা(fd >= 0);

        sha256_init_ctx(&ctx);

        বাবে (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = পঠ(fd, buffer, পরিমাণ(buffer));
                যদি (n < 0)
                        ঘূৰাই -errno;
                যদি (n == 0)
                        ভঙ্গ;

                যদি (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ঘূৰাই -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_sha256(ধ্রুৱ ৱর্ণ *s, uint8_t ret[স্থির SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = রিক্ত;
        size_t size = 0;
        পূর্ণাঙ্ক r;

        যদি (!sha256_is_valid(s))
                ঘূৰাই -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (শূন্য**) &data, &size);
        যদি (r < 0)
                ঘূৰাই r;
        প্রতিজ্ঞা(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ঘূৰাই 0;
}

bool sha256_is_valid(ধ্রুৱ ৱর্ণ *s) {
        ঘূৰাই s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

ৱর্ণ* sha256_direct_hex(ধ্রুৱ শূন্য *buffer, size_t sz) {
        প্রতিজ্ঞা(buffer || sz == 0);

        যদি (sz == SIZE_MAX)
                sz = strlen(buffer);

        ঘূৰাই hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
