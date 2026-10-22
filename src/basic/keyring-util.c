/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

පූර්ණ_සංඛ්‍යා keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        සඳහා (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                නම් (!buf)
                        ආපසු -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                නම් (n < 0)
                        ආපසු -errno;

                නම් ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        නම් (ret)
                                *ret = TAKE_PTR(buf);
                        නම් (ret_size)
                                *ret_size = n;

                        ආපසු 0;
                }

                bufsize = (size_t) n;
        }
}

පූර්ණ_සංඛ්‍යා keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        පූර්ණ_සංඛ්‍යා c;

        assert(ret);

        සඳහා (;;) {
                tuple = new(var_na, sz);
                නම් (!tuple)
                        ආපසු log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                නම් (c < 0)
                        ආපසු log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                නම් ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ආපසු 0;
}
