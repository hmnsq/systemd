/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "keyring-util.h"
#ضمن "log.h"

صحيح keyring_read(key_serial_t serial, فراغ **ret, size_t *ret_size) {
        size_t bufsize = 100;

        עבור (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = عدم;
                long n;

                buf = new(uint8_t, bufsize + 1);
                אם (!buf)
                        החזר -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                אם (n < 0)
                        החזר -errno;

                אם ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        אם (ret)
                                *ret = TAKE_PTR(buf);
                        אם (ret_size)
                                *ret_size = n;

                        החזר 0;
                }

                bufsize = (size_t) n;
        }
}

صحيح keyring_describe(key_serial_t serial, حرف **ret) {
        _cleanup_free_ حرف *tuple = عدم;
        size_t sz = 64;
        صحيح c;

        أكد(ret);

        עבור (;;) {
                tuple = new(حرف, sz);
                אם (!tuple)
                        החזר log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                אם (c < 0)
                        החזר log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                אם ((size_t) c <= sz)
                        اكسر;

                sz = c;
                حرر(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        أكد(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        החזר 0;
}
