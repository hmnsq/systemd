/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "keyring-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"

ਪੂਰਨ keyring_read(ਕੁੰਜੀ_serial_t serial, ਖਾਲੀ **ret, size_t *ret_ਮਾਪ) {
        size_t bufsize = 100;

        ਲਈ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ਨਿਰਾ;
                ਲੰਮਾ n;

                buf = new(uint8_t, bufsize + 1);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                n = keyctl(KEYCTL_READ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) serial, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) buf, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) bufsize, 0);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;

                ਜੇ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ਜੇ (ret)
                                *ret = TAKE_PTR(buf);
                        ਜੇ (ret_ਮਾਪ)
                                *ret_ਮਾਪ = n;

                        ਵਾਪਸ 0;
                }

                bufsize = (size_t) n;
        }
}

ਪੂਰਨ keyring_describe(ਕੁੰਜੀ_serial_t serial, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *tuple = ਨਿਰਾ;
        size_t sz = 64;
        ਪੂਰਨ c;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (;;) {
                tuple = new(ਅੱਖਰ, sz);
                ਜੇ (!tuple)
                        ਵਾਪਸ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) tuple, sz, 0);
                ਜੇ (c < 0)
                        ਵਾਪਸ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ਜੇ ((size_t) c <= sz)
                        ਰੋਕੋ;

                sz = c;
                ਮੁਕਤ(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ਪ੍ਰਤਿਜ੍ਞਾ(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ਵਾਪਸ 0;
}
