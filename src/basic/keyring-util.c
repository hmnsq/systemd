/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

ကိန်းပြည့် keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        အတွက် (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                အကယ်၍ (!buf)
                        ပြန်ပေး -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                အကယ်၍ (n < 0)
                        ပြန်ပေး -errno;

                အကယ်၍ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        အကယ်၍ (ret)
                                *ret = TAKE_PTR(buf);
                        အကယ်၍ (ret_size)
                                *ret_size = n;

                        ပြန်ပေး 0;
                }

                bufsize = (size_t) n;
        }
}

ကိန်းပြည့် keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        ကိန်းပြည့် c;

        assert(ret);

        အတွက် (;;) {
                tuple = new(var_na, sz);
                အကယ်၍ (!tuple)
                        ပြန်ပေး log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                အကယ်၍ (c < 0)
                        ပြန်ပေး log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                အကယ်၍ ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ပြန်ပေး 0;
}
