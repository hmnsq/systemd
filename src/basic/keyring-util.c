/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

정수 keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        위해 (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                만약 (!buf)
                        반환 -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                만약 (n < 0)
                        반환 -errno;

                만약 ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        만약 (ret)
                                *ret = TAKE_PTR(buf);
                        만약 (ret_size)
                                *ret_size = n;

                        반환 0;
                }

                bufsize = (size_t) n;
        }
}

정수 keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        정수 c;

        assert(ret);

        위해 (;;) {
                tuple = new(char, sz);
                만약 (!tuple)
                        반환 log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                만약 (c < 0)
                        반환 log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                만약 ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        반환 0;
}
