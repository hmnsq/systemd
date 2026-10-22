/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

ཧྲིལ keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        དོན་ལུ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                གལ་སྲིད (!buf)
                        ལོག -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                གལ་སྲིད (n < 0)
                        ལོག -errno;

                གལ་སྲིད ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        གལ་སྲིད (ret)
                                *ret = TAKE_PTR(buf);
                        གལ་སྲིད (ret_size)
                                *ret_size = n;

                        ལོག 0;
                }

                bufsize = (size_t) n;
        }
}

ཧྲིལ keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        ཧྲིལ c;

        assert(ret);

        དོན་ལུ (;;) {
                tuple = new(var_na, sz);
                གལ་སྲིད (!tuple)
                        ལོག log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                གལ་སྲིད (c < 0)
                        ལོག log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                གལ་སྲིད ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ལོག 0;
}
