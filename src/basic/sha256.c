/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ sha256_fd(ਪੂਰਨ fd, uint64_t max_ਮਾਪ, uint8_t ret[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]) {
        ਬਣਤਰ sha256_ctx ctx;
        uint64_t ਕੁੱਲ_ਮਾਪ = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        sha256_init_ctx(&ctx);

        ਲਈ (;;) {
                uint8_t ਬਫਰ[64 * 1024];
                ssize_t n;

                n = ਪਠ(fd, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (n == 0)
                        ਰੋਕੋ;

                ਜੇ (!INC_ਸੁਰੱਖਿਅਤ(&ਕੁੱਲ_ਮਾਪ, n) || ਕੁੱਲ_ਮਾਪ > max_ਮਾਪ)
                        ਵਾਪਸ -EFBIG;

                sha256_process_ਬਾਈਟਾਂ(ਬਫਰ, n, &ctx);
        }

        sha256_ਅੰਤ_ctx(&ctx, ret);
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_sha256(ਅਚਲ ਅੱਖਰ *s, uint8_t ret[ਸਥਿਰ SHA256_DIGEST_ਮਾਪ]) {
        _cleanup_free_ uint8_t *ਅੰਕੜੇ = ਨਿਰਾ;
        size_t ਮਾਪ = 0;
        ਪੂਰਨ r;

        ਜੇ (!sha256_is_valid(s))
                ਵਾਪਸ -EINVAL;

        r = unhexmem_ਭਰਿਆ(s, SHA256_DIGEST_ਮਾਪ * 2, false, (ਖਾਲੀ**) &ਅੰਕੜੇ, &ਮਾਪ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ == SHA256_DIGEST_ਮਾਪ);

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ret, ਅੰਕੜੇ, ਮਾਪ);
        ਵਾਪਸ 0;
}

bool sha256_is_valid(ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ s && ਵਿੱਚ_charset(s, HEXDIGITS) && (ਲੜੀ_ਲੰਬਾਈ(s) == SHA256_DIGEST_ਮਾਪ * 2);
}

ਅੱਖਰ* sha256_ਸਿੱਧਾ_hex(ਅਚਲ ਖਾਲੀ *ਬਫਰ, size_t sz) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ || sz == 0);

        ਜੇ (sz == SIZE_MAX)
                sz = ਲੜੀ_ਲੰਬਾਈ(ਬਫਰ);

        ਵਾਪਸ hexmem(SHA256_ਸਿੱਧਾ(ਬਫਰ, sz), SHA256_DIGEST_ਮਾਪ);
}
