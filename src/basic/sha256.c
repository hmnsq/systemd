/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "sha256.h"
#அந்தர்பாவ "string-util.h"

முழு sha256_fd(முழு fd, uint64_t max_size, uint8_t ret[நிலையான SHA256_DIGEST_SIZE]) {
        கட்டமைப்பு sha256_ctx ctx;
        uint64_t total_size = 0;

        ப்ரதிஜ்ஞா(fd >= 0);

        sha256_init_ctx(&ctx);

        க்கு (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = பட(fd, buffer, அளவு(buffer));
                எனில் (n < 0)
                        திரும்பு -errno;
                எனில் (n == 0)
                        நிறுத்து;

                எனில் (!INC_SAFE(&total_size, n) || total_size > max_size)
                        திரும்பு -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        திரும்பு 0;
}

முழு parse_sha256(மாறிலி எழுத்து *s, uint8_t ret[நிலையான SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = வெற்று;
        size_t size = 0;
        முழு r;

        எனில் (!sha256_is_valid(s))
                திரும்பு -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (வெறுமை**) &data, &size);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        திரும்பு 0;
}

bool sha256_is_valid(மாறிலி எழுத்து *s) {
        திரும்பு s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

எழுத்து* sha256_direct_hex(மாறிலி வெறுமை *buffer, size_t sz) {
        ப்ரதிஜ்ஞா(buffer || sz == 0);

        எனில் (sz == SIZE_MAX)
                sz = strlen(buffer);

        திரும்பு hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
