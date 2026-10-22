/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

整数 keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        ために (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                もし (!buf)
                        戻る -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                もし (n < 0)
                        戻る -errno;

                もし ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        もし (ret)
                                *ret = TAKE_PTR(buf);
                        もし (ret_size)
                                *ret_size = n;

                        戻る 0;
                }

                bufsize = (size_t) n;
        }
}

整数 keyring_describe(key_serial_t serial, char **ret) {
        _cleanup_free_ char *tuple = NULL;
        size_t sz = 64;
        整数 c;

        assert(ret);

        ために (;;) {
                tuple = new(char, sz);
                もし (!tuple)
                        戻る log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                もし (c < 0)
                        戻る log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                もし ((size_t) c <= sz)
                        break;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        戻る 0;
}
