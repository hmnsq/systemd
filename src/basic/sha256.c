/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "sha256.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ sha256_fd(പൂർണ്ണസംഖ്യ fd, uint64_t max_size, uint8_t ret[സ്ഥിതം SHA256_DIGEST_SIZE]) {
        ഘടന sha256_ctx ctx;
        uint64_t total_size = 0;

        പ്രതിജ്ഞാ(fd >= 0);

        sha256_init_ctx(&ctx);

        വേണ്ടി (;;) {
                uint8_t buffer[64 * 1024];
                ssize_t n;

                n = പഠ(fd, buffer, വലുപ്പം(buffer));
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;
                എങ്കിൽ (n == 0)
                        നിർത്തുക;

                എങ്കിൽ (!INC_SAFE(&total_size, n) || total_size > max_size)
                        തിരികെ -EFBIG;

                sha256_process_bytes(buffer, n, &ctx);
        }

        sha256_finish_ctx(&ctx, ret);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_sha256(സ്ഥിരം അക്ഷരം *s, uint8_t ret[സ്ഥിതം SHA256_DIGEST_SIZE]) {
        _cleanup_free_ uint8_t *data = ശൂന്യ;
        size_t size = 0;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!sha256_is_valid(s))
                തിരികെ -EINVAL;

        r = unhexmem_full(s, SHA256_DIGEST_SIZE * 2, false, (ശൂന്യം**) &data, &size);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(size == SHA256_DIGEST_SIZE);

        memcpy(ret, data, size);
        തിരികെ 0;
}

bool sha256_is_valid(സ്ഥിരം അക്ഷരം *s) {
        തിരികെ s && in_charset(s, HEXDIGITS) && (strlen(s) == SHA256_DIGEST_SIZE * 2);
}

അക്ഷരം* sha256_direct_hex(സ്ഥിരം ശൂന്യം *buffer, size_t sz) {
        പ്രതിജ്ഞാ(buffer || sz == 0);

        എങ്കിൽ (sz == SIZE_MAX)
                sz = strlen(buffer);

        തിരികെ hexmem(SHA256_DIRECT(buffer, sz), SHA256_DIGEST_SIZE);
}
