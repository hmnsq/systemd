/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "keyring-util.h"
#ضمن "log.h"

صحيح keyring_read(key_serial_t serial, فراغ **ret, size_t *ret_size) {
        size_t bufsize = 100;

        لكل (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = عدم;
                long n;

                buf = new(uint8_t, bufsize + 1);
                𐤀𐤌 (!buf)
                        أرجع -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                𐤀𐤌 (n < 0)
                        أرجع -errno;

                𐤀𐤌 ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        𐤀𐤌 (ret)
                                *ret = TAKE_PTR(buf);
                        𐤀𐤌 (ret_size)
                                *ret_size = n;

                        أرجع 0;
                }

                bufsize = (size_t) n;
        }
}

صحيح keyring_describe(key_serial_t serial, حرف **ret) {
        _cleanup_free_ حرف *tuple = عدم;
        size_t sz = 64;
        صحيح c;

        أكد(ret);

        لكل (;;) {
                tuple = new(حرف, sz);
                𐤀𐤌 (!tuple)
                        أرجع log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                𐤀𐤌 (c < 0)
                        أرجع log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                𐤀𐤌 ((size_t) c <= sz)
                        اكسر;

                sz = c;
                حرر(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        أكد(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        أرجع 0;
}
