/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "keyring-util.h"
#अन्तर्भाव "log.h"

पूर्णाङ्क keyring_read(कुञ्चिका_serial_t serial, शून्य **ret, size_t *ret_आकार) {
        size_t bufsize = 100;

        प्रति (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = रिक्त;
                दीर्घ n;

                buf = new(uint8_t, bufsize + 1);
                यदि (!buf)
                        प्रत्यागम -ENOMEM;

                n = keyctl(KEYCTL_READ, (अचिह्नित दीर्घ) serial, (अचिह्नित दीर्घ) buf, (अचिह्नित दीर्घ) bufsize, 0);
                यदि (n < 0)
                        प्रत्यागम -errno;

                यदि ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        यदि (ret)
                                *ret = TAKE_PTR(buf);
                        यदि (ret_आकार)
                                *ret_आकार = n;

                        प्रत्यागम 0;
                }

                bufsize = (size_t) n;
        }
}

पूर्णाङ्क keyring_describe(कुञ्चिका_serial_t serial, वर्ण **ret) {
        _cleanup_free_ वर्ण *tuple = रिक्त;
        size_t sz = 64;
        पूर्णाङ्क c;

        प्रतिज्ञा(ret);

        प्रति (;;) {
                tuple = new(वर्ण, sz);
                यदि (!tuple)
                        प्रत्यागम log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (अचिह्नित दीर्घ) tuple, sz, 0);
                यदि (c < 0)
                        प्रत्यागम log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                यदि ((size_t) c <= sz)
                        भङ्ग;

                sz = c;
                मुक्त(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        प्रतिज्ञा(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        प्रत्यागम 0;
}
