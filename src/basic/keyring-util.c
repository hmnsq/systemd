/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

จำนวนเต็ม keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        สำหรับ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ถ้า (!buf)
                        คืน -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ถ้า (n < 0)
                        คืน -errno;

                ถ้า ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ถ้า (ret)
                                *ret = TAKE_PTR(buf);
                        ถ้า (ret_size)
                                *ret_size = n;

                        คืน 0;
                }

                bufsize = (size_t) n;
        }
}

จำนวนเต็ม keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        จำนวนเต็ม c;

        assert(ret);

        สำหรับ (;;) {
                tuple = new(var_na, sz);
                ถ้า (!tuple)
                        คืน log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ถ้า (c < 0)
                        คืน log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ถ้า ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        คืน 0;
}
