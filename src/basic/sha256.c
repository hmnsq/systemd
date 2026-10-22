/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "hexdecoct.h"
#include "sha256.h"
#include "string-util.h"

puur_naa_nka sha256_fd(puur_naa_nka fd, uint64_t max_size, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        sa_nracanaa sha256_ctx ctx;
        uint64_t total_size = 0;

        assert(fd >= 0);

        sha256_init_ctx(&ctx);

        for (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = read(fd, buffer, parimaa_na(buffer));
                ᨀᨘ (n < 0)
                        ᨀᨘᨒ -errno;
                ᨀᨘ (n == 0)
                        bha_nga;

                ᨀᨘ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        ᨀᨘᨒ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        ᨀᨘᨒ 0;
}

puur_naa_nka parse_sha256(const var_na *s, uint8_t ret[static SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = NULL;
        size_t size = 0;
        puur_naa_nka r;

        ᨀᨘ (!sha256_is_valid(s))
                ᨀᨘᨒ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (void**) &data, &size);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        assert(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        ᨀᨘᨒ 0;
}

bool sha256_is_valid(const var_na *s) {
        ᨀᨘᨒ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

var_na* sha256_direct_hex(const void *buffer, size_t sz) {
        assert(buffer || sz == 0);

        ᨀᨘ (sz == SIZE_MAX)
                sz = strlen(buffer);

        ᨀᨘᨒ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
