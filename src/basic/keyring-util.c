/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "keyring-util.h"
#شامل "log.h"

صحیح keyring_read(کلید_serial_t serial, تهی **ret, size_t *ret_size) {
        size_t bufsize = 100;

        برای (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = پوچ;
                long n;

                buf = new(uint8_t, bufsize + 1);
                اگر (!buf)
                        بازگشت -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                اگر (n < 0)
                        بازگشت -errno;

                اگر ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        اگر (ret)
                                *ret = TAKE_PTR(buf);
                        اگر (ret_size)
                                *ret_size = n;

                        بازگشت 0;
                }

                bufsize = (size_t) n;
        }
}

صحیح keyring_describe(کلید_serial_t serial, نویسه **ret) {
        _cleanup_free_ نویسه *tuple = پوچ;
        size_t sz = 64;
        صحیح c;

        أكد(ret);

        برای (;;) {
                tuple = new(نویسه, sz);
                اگر (!tuple)
                        بازگشت log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                اگر (c < 0)
                        بازگشت log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                اگر ((size_t) c <= sz)
                        بشکن;

                sz = c;
                آزاد(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        أكد(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        بازگشت 0;
}
