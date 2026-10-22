/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "sha256.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક sha256_fd(પૂર્ણાંક fd, uint64_t max_size, uint8_t ret[સ્થિર SHA256_DIGEST_SIZE]) {
        સંરચના sha256_ctx ctx;
        uint64_t total_size = 0;

        પ્રતિજ્ઞા(fd >= 0);

        sha256_init_ctx(&ctx);

        માટે (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = પઠ(fd, buffer, કદ(buffer));
                જો (n < 0)
                        પાછા -errno;
                જો (n == 0)
                        તોડો;

                જો (!INC_SAFE(&total_size, n) || total_size > max_size)
                        પાછા -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        પાછા 0;
}

પૂર્ણાંક parse_sha256(અચલ અક્ષર *s, uint8_t ret[સ્થિર SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = ખાલી;
        size_t size = 0;
        પૂર્ણાંક r;

        જો (!sha256_is_valid(s))
                પાછા -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (શૂન્ય**) &data, &size);
        જો (r < 0)
                પાછા r;
        પ્રતિજ્ઞા(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        પાછા 0;
}

bool sha256_is_valid(અચલ અક્ષર *s) {
        પાછા s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

અક્ષર* sha256_direct_hex(અચલ શૂન્ય *buffer, size_t sz) {
        પ્રતિજ્ઞા(buffer || sz == 0);

        જો (sz == SIZE_MAX)
                sz = strlen(buffer);

        પાછા hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
