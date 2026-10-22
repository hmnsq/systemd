/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "sha256.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ sha256_fd(ಪೂರ್ಣಾಂಕ fd, uint64_t max_ಗಾತ್ರ, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_ಗಾತ್ರ]) {
        ರಚನೆ sha256_ctx ctx;
        uint64_t ಒಟ್ಟು_ಗಾತ್ರ = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        sha256_init_ctx(&ctx);

        ಗಾಗಿ (;;) {
                uint8_t ಬಫರ್[64 * 1024];
                ssize_t n;

                n = ಪಠ(fd, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (n == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (!INC_ಸುರಕ್ಷಿತ(&ಒಟ್ಟು_ಗಾತ್ರ, n) || ಒಟ್ಟು_ಗಾತ್ರ > max_ಗಾತ್ರ)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                sha256_process_ಬೈಟುಗಳು(ಬಫರ್, n, &ctx);
        }

        sha256_ಮುಕ್ತಾಯ_ctx(&ctx, ret);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_sha256(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint8_t ret[ಸ್ಥಿರ SHA256_DIGEST_ಗಾತ್ರ]) {
        _cleanup_free_ uint8_t *ದತ್ತಾಂಶ = ಖಾಲಿ;
        size_t ಗಾತ್ರ_2 = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!sha256_is_valid(s))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = unhexmem_ತುಂಬಿದ(s, SHA256_DIGEST_ಗಾತ್ರ * 2, false, (ಶೂನ್ಯ**) &ದತ್ತಾಂಶ, &ಗಾತ್ರ_2);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 == SHA256_DIGEST_ಗಾತ್ರ);

        ಸ್ಮರಣೆ_ನಕಲು(ret, ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
        ಹಿಂತಿರುಗಿ 0;
}

bool sha256_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ s && ಒಳ_charset(s, HEXDIGITS) && (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) == SHA256_DIGEST_ಗಾತ್ರ * 2);
}

ಅಕ್ಷರ* sha256_ನೇರ_hex(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್, size_t sz) {
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್ || sz == 0);

        ಆದರೆ (sz == SIZE_MAX)
                sz = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಬಫರ್);

        ಹಿಂತಿರುಗಿ hexmem(SHA256_ನೇರ(ಬಫರ್, sz), SHA256_DIGEST_ಗಾತ್ರ);
}
