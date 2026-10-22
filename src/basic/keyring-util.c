/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "keyring-util.h"
#अन्तर्भाव "log.h"

पूर्णाङ्क keyring_read(साँचो_serial_t serial, शून्य **ret, माप_प्रकार *ret_size) {
        माप_प्रकार bufsize = 100;

        लागि (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = रिक्त;
                दीर्घ n;

                buf = new(uint8_t, bufsize + 1);
                यदि (!buf)
                        फर्काऊ -ENOMEM;

                n = keyctl(KEYCTL_READ, (अचिन्हित दीर्घ) serial, (अचिन्हित दीर्घ) buf, (अचिन्हित दीर्घ) bufsize, 0);
                यदि (n < 0)
                        फर्काऊ -त्रुटि_सं;

                यदि ((माप_प्रकार) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        यदि (ret)
                                *ret = TAKE_PTR(buf);
                        यदि (ret_size)
                                *ret_size = n;

                        फर्काऊ 0;
                }

                bufsize = (माप_प्रकार) n;
        }
}

पूर्णाङ्क keyring_describe(साँचो_serial_t serial, वर्ण **ret) {
        _cleanup_free_ वर्ण *tuple = रिक्त;
        माप_प्रकार sz = 64;
        पूर्णाङ्क c;

        प्रतिज्ञा(ret);

        लागि (;;) {
                tuple = new(वर्ण, sz);
                यदि (!tuple)
                        फर्काऊ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (अचिन्हित दीर्घ) tuple, sz, 0);
                यदि (c < 0)
                        फर्काऊ log_debug_errno(त्रुटि_सं, "Failed to describe key id %d: %m", serial);

                यदि ((माप_प्रकार) c <= sz)
                        रोक;

                sz = c;
                मुक्त(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        प्रतिज्ञा(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        फर्काऊ 0;
}
