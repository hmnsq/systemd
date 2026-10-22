/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "sha256.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sha256_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t max_size, uint8_t ret[ସ୍ଥିର SHA256_DIGEST_SIZE]) {
        ସଂରଚନା sha256_ctx ctx;
        uint64_t total_size = 0;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        sha256_init_ctx(&ctx);

        ପାଇଁ (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = ପଠ(fd, buffer, ଆକାର(buffer));
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (n == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ଫେରନ୍ତୁ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sha256(ଅଚଳ ଅକ୍ଷର *s, uint8_t ret[ସ୍ଥିର SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = ଖାଲି;
        size_t size = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!sha256_is_valid(s))
                ଫେରନ୍ତୁ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (ଶୂନ୍ୟ**) &data, &size);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ଫେରନ୍ତୁ 0;
}

bool sha256_is_valid(ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

ଅକ୍ଷର* sha256_direct_hex(ଅଚଳ ଶୂନ୍ୟ *buffer, size_t sz) {
        ପ୍ରତିଜ୍ଞା(buffer || sz == 0);

        ଯଦି (sz == SIZE_MAX)
                sz = strlen(buffer);

        ଫେରନ୍ତୁ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
