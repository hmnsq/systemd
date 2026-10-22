/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "keyring-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಪೂರ್ಣಾಙ್ಕ keyring_read(ಕೀಲಿ_serial_t serial, ಶೂನ್ಯ **ret, size_t *ret_size) {
        size_t bufsize = 100;

        ಪ್ರತಿ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ರಿಕ್ತ;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ಯದಿ (ret)
                                *ret = TAKE_PTR(buf);
                        ಯದಿ (ret_size)
                                *ret_size = n;

                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                bufsize = (size_t) n;
        }
}

ಪೂರ್ಣಾಙ್ಕ keyring_describe(ಕೀಲಿ_serial_t serial, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *tuple = ರಿಕ್ತ;
        size_t sz = 64;
        ಪೂರ್ಣಾಙ್ಕ c;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (;;) {
                tuple = new(ವರ್ಣ, sz);
                ಯದಿ (!tuple)
                        ಪ್ರತ್ಯಾಗಮ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ಯದಿ (c < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ಯದಿ ((size_t) c <= sz)
                        ಭಙ್ಗ;

                sz = c;
                ಮುಕ್ತ(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ಪ್ರತಿಜ್ಞಾ(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ಪ್ರತ್ಯಾಗಮ 0;
}
