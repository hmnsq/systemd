/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "sha256.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क sha256_fd(पूर्णाङ्क fd, uint64_t max_आकार, uint8_t ret[स्थिर SHA256_DIGEST_आकार]) {
        संरचना sha256_ctx ctx;
        uint64_t योग_आकार = 0;

        प्रतिज्ञा(fd >= 0);

        sha256_init_ctx(&ctx);

        प्रति (;;) {
                uint8_t बफर[64 * 1024];
                ssize_t n;

                n = पठ(fd, बफर, परिमाण(बफर));
                यदि (n < 0)
                        प्रत्यागम -errno;
                यदि (n == 0)
                        भङ्ग;

                यदि (!INC_SAFE(&योग_आकार, n) || योग_आकार > max_आकार)
                        प्रत्यागम -EFBIG;

                sha256_process_अष्टकानि(बफर, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        प्रत्यागम 0;
}

पूर्णाङ्क parse_sha256(ध्रुव वर्ण *s, uint8_t ret[स्थिर SHA256_DIGEST_आकार]) {
        _cleanup_free_ uint8_t *दत्त = रिक्त;
        size_t आकार = 0;
        पूर्णाङ्क r;

        यदि (!sha256_is_valid(s))
                प्रत्यागम -EINVAL;

        r = unhexmem_पूर्णम्(s, SHA256_DIGEST_आकार * 2, false, (शून्य**) &दत्त, &आकार);
        यदि (r < 0)
                प्रत्यागम r;
        प्रतिज्ञा(आकार == SHA256_DIGEST_आकार);

        स्मृतिं_प्रतिलिख(ret, दत्त, आकार);
        प्रत्यागम 0;
}

bool sha256_is_valid(ध्रुव वर्ण *s) {
        प्रत्यागम s && अन्तर्_charset(s, HEXDIGITS) && (वाक्यदैर्घ्यम्(s) == SHA256_DIGEST_आकार * 2);
}

वर्ण* sha256_प्रत्यक्षम्_hex(ध्रुव शून्य *बफर, size_t sz) {
        प्रतिज्ञा(बफर || sz == 0);

        यदि (sz == SIZE_MAX)
                sz = वाक्यदैर्घ्यम्(बफर);

        प्रत्यागम hexmem(SHA256_प्रत्यक्षम्(बफर, sz), SHA256_DIGEST_आकार);
}
