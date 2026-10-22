/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

цяло keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        за (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ако (!buf)
                        върни -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ако (n < 0)
                        върни -errno;

                ако ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ако (ret)
                                *ret = TAKE_PTR(buf);
                        ако (ret_size)
                                *ret_size = n;

                        върни 0;
                }

                bufsize = (size_t) n;
        }
}

цяло keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        цяло c;

        assert(ret);

        за (;;) {
                tuple = new(char, sz);
                ако (!tuple)
                        върни log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ако (c < 0)
                        върни log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ако ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        върни 0;
}
