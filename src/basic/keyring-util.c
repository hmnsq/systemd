/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

цео keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        за (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ако (!buf)
                        врати -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ако (n < 0)
                        врати -errno;

                ако ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ако (ret)
                                *ret = TAKE_PTR(buf);
                        ако (ret_size)
                                *ret_size = n;

                        врати 0;
                }

                bufsize = (size_t) n;
        }
}

цео keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        цео c;

        assert(ret);

        за (;;) {
                tuple = new(char, sz);
                ако (!tuple)
                        врати log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ако (c < 0)
                        врати log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ако ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        врати 0;
}
