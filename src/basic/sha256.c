/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "sha256.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क sha256_fd(पूर्णाङ्क fd, uint64_t max_size, uint8_t ret[स्थिर SHA256_DIGEST_SIZE]) {
        संरचना sha256_ctx ctx;
        uint64_t total_size = 0;

        प्रतिज्ञा(fd >= 0);

        sha256_init_ctx(&ctx);

        प्रति (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = पठ(fd, buffer, परिमाण(buffer));
                यदि (n < 0)
                        प्रत्यागम -errno;
                यदि (n == 0)
                        भङ्ग;

                यदि (!INC_SAFE(&total_size, n) || total_size > max_size)
                        प्रत्यागम -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        प्रत्यागम 0;
}

पूर्णाङ्क parse_sha256(ध्रुव वर्ण *s, uint8_t ret[स्थिर SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = रिक्त;
        size_t size = 0;
        पूर्णाङ्क r;

        यदि (!sha256_is_valid(s))
                प्रत्यागम -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (शून्य**) &data, &size);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        प्रत्यागम 0;
}

bool sha256_is_valid(ध्रुव वर्ण *s) {
        प्रत्यागम s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

वर्ण* sha256_direct_hex(ध्रुव शून्य *buffer, size_t sz) {
        प्रतिज्ञा(buffer || sz == 0);

        यदि (sz == SIZE_MAX)
                sz = strlen(buffer);

        प्रत्यागम hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
