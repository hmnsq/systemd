/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

ចំនួនគត់ keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        សម្រាប់ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ប្រសិនបើ (!buf)
                        ត្រឡប់ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ប្រសិនបើ (n < 0)
                        ត្រឡប់ -errno;

                ប្រសិនបើ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ប្រសិនបើ (ret)
                                *ret = TAKE_PTR(buf);
                        ប្រសិនបើ (ret_size)
                                *ret_size = n;

                        ត្រឡប់ 0;
                }

                bufsize = (size_t) n;
        }
}

ចំនួនគត់ keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        ចំនួនគត់ c;

        assert(ret);

        សម្រាប់ (;;) {
                tuple = new(var_na, sz);
                ប្រសិនបើ (!tuple)
                        ត្រឡប់ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ប្រសិនបើ (c < 0)
                        ត្រឡប់ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ប្រសិនបើ ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ត្រឡប់ 0;
}
