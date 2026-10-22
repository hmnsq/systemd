/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "keyring-util.h"
#अन्तर्भाव "log.h"

पूर्णांक keyring_read(किल्ली_serial_t serial, शून्य **ret, माप_प्रकार *ret_size) {
        माप_प्रकार bufsize = 100;

        साठी (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = रिक्त;
                दीर्घ n;

                buf = new(uint8_t, bufsize + 1);
                जर (!buf)
                        परत -ENOMEM;

                n = keyctl(KEYCTL_READ, (अचिन्हित दीर्घ) serial, (अचिन्हित दीर्घ) buf, (अचिन्हित दीर्घ) bufsize, 0);
                जर (n < 0)
                        परत -त्रुटि_सं;

                जर ((माप_प्रकार) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        जर (ret)
                                *ret = TAKE_PTR(buf);
                        जर (ret_size)
                                *ret_size = n;

                        परत 0;
                }

                bufsize = (माप_प्रकार) n;
        }
}

पूर्णांक keyring_describe(किल्ली_serial_t serial, अक्षर **ret) {
        _cleanup_free_ अक्षर *tuple = रिक्त;
        माप_प्रकार sz = 64;
        पूर्णांक c;

        प्रतिज्ञा(ret);

        साठी (;;) {
                tuple = new(अक्षर, sz);
                जर (!tuple)
                        परत log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (अचिन्हित दीर्घ) tuple, sz, 0);
                जर (c < 0)
                        परत log_debug_errno(त्रुटि_सं, "Failed to describe key id %d: %m", serial);

                जर ((माप_प्रकार) c <= sz)
                        थांबा;

                sz = c;
                मुक्त(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        प्रतिज्ञा(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        परत 0;
}
