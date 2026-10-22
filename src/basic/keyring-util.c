/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

ціле keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        для (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                якщо (!buf)
                        повернути -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                якщо (n < 0)
                        повернути -errno;

                якщо ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        якщо (ret)
                                *ret = TAKE_PTR(buf);
                        якщо (ret_size)
                                *ret_size = n;

                        повернути 0;
                }

                bufsize = (size_t) n;
        }
}

ціле keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        ціле c;

        assert(ret);

        для (;;) {
                tuple = new(char, sz);
                якщо (!tuple)
                        повернути log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                якщо (c < 0)
                        повернути log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                якщо ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        повернути 0;
}
