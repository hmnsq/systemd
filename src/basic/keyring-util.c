/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

số_nguyên keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        cho (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                nếu (!buf)
                        trả_về -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                nếu (n < 0)
                        trả_về -errno;

                nếu ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        nếu (ret)
                                *ret = TAKE_PTR(buf);
                        nếu (ret_size)
                                *ret_size = n;

                        trả_về 0;
                }

                bufsize = (size_t) n;
        }
}

số_nguyên keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        số_nguyên c;

        assert(ret);

        cho (;;) {
                tuple = new(char, sz);
                nếu (!tuple)
                        trả_về log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                nếu (c < 0)
                        trả_về log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                nếu ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        trả_về 0;
}
