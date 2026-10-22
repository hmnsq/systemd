/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "keyring-util.h"
#include "log.h"

puur_naa_nka keyring_read(key_serial_t serial, void **ret, size_t *ret_size) {
        size_t bufsize = 100;

        for (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = NULL;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ᤀᤡ᤺ (!buf)
                        ᤀᤡᤴ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ᤀᤡ᤺ (n < 0)
                        ᤀᤡᤴ -errno;

                ᤀᤡ᤺ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ᤀᤡ᤺ (ret)
                                *ret = TAKE_PTR(buf);
                        ᤀᤡ᤺ (ret_size)
                                *ret_size = n;

                        ᤀᤡᤴ 0;
                }

                bufsize = (size_t) n;
        }
}

puur_naa_nka keyring_describe(key_serial_t serial, var_na **ret) {
        _cleanup_free_ var_na *tuple = NULL;
        size_t sz = 64;
        puur_naa_nka c;

        assert(ret);

        for (;;) {
                tuple = new(var_na, sz);
                ᤀᤡ᤺ (!tuple)
                        ᤀᤡᤴ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ᤀᤡ᤺ (c < 0)
                        ᤀᤡᤴ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ᤀᤡ᤺ ((size_t) c <= sz)
                        bha_nga;

                sz = c;
                free(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        assert(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ᤀᤡᤴ 0;
}
