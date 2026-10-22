/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "sha256.h"
#अन्तर्भाव "string-util.h"

पूर्णांक sha256_fd(पूर्णांक fd, uint64_t max_size, uint8_t ret[स्थिर SHA256_DIGEST_SIZE]) {
        संरचना sha256_ctx ctx;
        uint64_t total_size = 0;

        प्रतिज्ञा(fd >= 0);

        sha256_init_ctx(&ctx);

        साठी (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = पठ(fd, buffer, आकार(buffer));
                जर (n < 0)
                        परत -त्रुटि_सं;
                जर (n == 0)
                        थांबा;

                जर (!INC_SAFE(&total_size, n) || total_size > max_size)
                        परत -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        परत 0;
}

पूर्णांक parse_sha256(स्थिरांक अक्षर *s, uint8_t ret[स्थिर SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = रिक्त;
        माप_प्रकार size = 0;
        पूर्णांक r;

        जर (!sha256_is_valid(s))
                परत -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (शून्य**) &data, &size);
        जर (r < 0)
                परत r;
        प्रतिज्ञा(size == SHA256_DIGEST_SIZE);

        स_नकल(ret, data, size);
        परत 0;
}

bool sha256_is_valid(स्थिरांक अक्षर *s) {
        परत s && in_charset(s, HEXDIGITS) && (म_माप(s) == SHA256_DIGEST_SIZE * 2);
}

अक्षर* sha256_direct_hex(स्थिरांक शून्य *buffer, माप_प्रकार sz) {
        प्रतिज्ञा(buffer || sz == 0);

        जर (sz == SIZE_MAX)
                sz = म_माप(buffer);

        परत hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
