/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "keyring-util.h"
#ضمن "log.h"

صحيح keyring_read(ڪنجي_serial_t serial, فراغ **ret, size_t *ret_سائيز) {
        size_t bufsize = 100;

        لكل (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = عدم;
                ڊگهو n;

                buf = new(uint8_t, bufsize + 1);
                إذا (!buf)
                        أرجع -ENOMEM;

                n = keyctl(KEYCTL_READ, (بنانشان ڊگهو) serial, (بنانشان ڊگهو) buf, (بنانشان ڊگهو) bufsize, 0);
                إذا (n < 0)
                        أرجع -errno;

                إذا ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        إذا (ret)
                                *ret = TAKE_PTR(buf);
                        إذا (ret_سائيز)
                                *ret_سائيز = n;

                        أرجع 0;
                }

                bufsize = (size_t) n;
        }
}

صحيح keyring_describe(ڪنجي_serial_t serial, حرف **ret) {
        _cleanup_free_ حرف *tuple = عدم;
        size_t sz = 64;
        صحيح c;

        أكد(ret);

        لكل (;;) {
                tuple = new(حرف, sz);
                إذا (!tuple)
                        أرجع log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (بنانشان ڊگهو) tuple, sz, 0);
                إذا (c < 0)
                        أرجع log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                إذا ((size_t) c <= sz)
                        اكسر;

                sz = c;
                حرر(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        أكد(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        أرجع 0;
}
