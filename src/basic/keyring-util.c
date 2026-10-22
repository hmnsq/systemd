/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

整数 keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        为 (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                如果 (!buf)
                        返回 -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                如果 (n < 0)
                        返回 -errno;

                如果 ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        如果 (ret)
                                *ret = TAKE_PTR(buf);
                        如果 (ret_size)
                                *ret_size = n;

                        返回 0;
                }

                bufsize = (size_t) n;
        }
}

整数 keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        整数 c;

        assert(ret);

        为 (;;) {
                tuple = new(char, sz);
                如果 (!tuple)
                        返回 log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                如果 (c < 0)
                        返回 log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                如果 ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        返回 0;
}
