/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "keyring-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಪೂರ್ಣಾಂಕ keyring_read(ಕೀಲಿ_serial_t serial, ಶೂನ್ಯ **ret, size_t *ret_ಗಾತ್ರ) {
        size_t bufsize = 100;

        ಗಾಗಿ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ಖಾಲಿ;
                ದೀರ್ಘ n;

                buf = new(uint8_t, bufsize + 1);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                n = keyctl(KEYCTL_READ, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) serial, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) buf, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) bufsize, 0);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ಆದರೆ (ret)
                                *ret = TAKE_PTR(buf);
                        ಆದರೆ (ret_ಗಾತ್ರ)
                                *ret_ಗಾತ್ರ = n;

                        ಹಿಂತಿರುಗಿ 0;
                }

                bufsize = (size_t) n;
        }
}

ಪೂರ್ಣಾಂಕ keyring_describe(ಕೀಲಿ_serial_t serial, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *tuple = ಖಾಲಿ;
        size_t sz = 64;
        ಪೂರ್ಣಾಂಕ c;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (;;) {
                tuple = new(ಅಕ್ಷರ, sz);
                ಆದರೆ (!tuple)
                        ಹಿಂತಿರುಗಿ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) tuple, sz, 0);
                ಆದರೆ (c < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ಆದರೆ ((size_t) c <= sz)
                        ನಿಲ್ಲಿಸು;

                sz = c;
                ಬಿಡುಗಡೆ(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ಪ್ರತಿಜ್ಞಾ(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ಹಿಂತಿರುಗಿ 0;
}
