/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "keyring-util.h"
#समावेश "log.h"

पूर्णांक keyring_read(कुंजी_serial_t serial, व्योम **ret, माप_प्रकार *ret_आकार) {
        माप_प्रकार bufsize = 100;

        क्रम (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = शून्य;
                दीर्घ n;

                buf = new(uint8_t, bufsize + 1);
                अगर (!buf)
                        वापस -ENOMEM;

                n = keyctl(KEYCTL_READ, (अचिन्हित दीर्घ) serial, (अचिन्हित दीर्घ) buf, (अचिन्हित दीर्घ) bufsize, 0);
                अगर (n < 0)
                        वापस -त्रुटि_सं;

                अगर ((माप_प्रकार) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        अगर (ret)
                                *ret = TAKE_PTR(buf);
                        अगर (ret_आकार)
                                *ret_आकार = n;

                        वापस 0;
                }

                bufsize = (माप_प्रकार) n;
        }
}

पूर्णांक keyring_describe(कुंजी_serial_t serial, अक्षर **ret) {
        _cleanup_free_ अक्षर *tuple = शून्य;
        माप_प्रकार sz = 64;
        पूर्णांक c;

        निश्चित(ret);

        क्रम (;;) {
                tuple = new(अक्षर, sz);
                अगर (!tuple)
                        वापस log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (अचिन्हित दीर्घ) tuple, sz, 0);
                अगर (c < 0)
                        वापस log_debug_errno(त्रुटि_सं, "Failed to describe key id %d: %m", serial);

                अगर ((माप_प्रकार) c <= sz)
                        अवरोध;

                sz = c;
                मुक्त(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        निश्चित(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        वापस 0;
}
