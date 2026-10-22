/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

ຈຳນວນເຕັມ keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        ສຳລັບ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ຖ້າ (!buf)
                        ສົ່ງຄືນ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ຖ້າ (n < 0)
                        ສົ່ງຄືນ -errno;

                ຖ້າ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ຖ້າ (ret)
                                *ret = TAKE_PTR(buf);
                        ຖ້າ (ret_size)
                                *ret_size = n;

                        ສົ່ງຄືນ 0;
                }

                bufsize = (size_t) n;
        }
}

ຈຳນວນເຕັມ keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        ຈຳນວນເຕັມ c;

        assert(ret);

        ສຳລັບ (;;) {
                tuple = new(var_na, sz);
                ຖ້າ (!tuple)
                        ສົ່ງຄືນ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ຖ້າ (c < 0)
                        ສົ່ງຄືນ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ຖ້າ ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ສົ່ງຄືນ 0;
}
