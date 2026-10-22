/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "keyring-util.h"
#अन्तर्भाव "log.h"

पूर्णाङ्क keyring_read(कुञ्चिका_serial_t serial, शून्य **ret, size_t *ret_size) {
        size_t bufsize = 100;

        अत्थाय (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = रिक्त;
                long n;

                buf = new(uint8_t, bufsize + 1);
                सचे (!buf)
                        पटिनिवत्त -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                सचे (n < 0)
                        पटिनिवत्त -errno;

                सचे ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        सचे (ret)
                                *ret = TAKE_PTR(buf);
                        सचे (ret_size)
                                *ret_size = n;

                        पटिनिवत्त 0;
                }

                bufsize = (size_t) n;
        }
}

पूर्णाङ्क keyring_describe(कुञ्चिका_serial_t serial, वर्ण **ret) {
        _cleanup_free_ वर्ण *tuple = रिक्त;
        size_t sz = 64;
        पूर्णाङ्क c;

        प्रतिज्ञा(ret);

        अत्थाय (;;) {
                tuple = new(वर्ण, sz);
                सचे (!tuple)
                        पटिनिवत्त log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                सचे (c < 0)
                        पटिनिवत्त log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                सचे ((size_t) c <= sz)
                        भङ्ग;

                sz = c;
                मुक्त(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        प्रतिज्ञा(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        पटिनिवत्त 0;
}
