/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "keyring-util.h"
#شامل "log.h"

صحیح keyring_read(کلید_serial_t serial, خالی **ret, size_t *ret_سائز) {
        size_t bufsize = 100;

        برائے (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = تہی;
                طویل n;

                buf = new(uint8_t, bufsize + 1);
                اگر (!buf)
                        واپس -ENOMEM;

                n = keyctl(KEYCTL_READ, (غیرعلامتی طویل) serial, (غیرعلامتی طویل) buf, (غیرعلامتی طویل) bufsize, 0);
                اگر (n < 0)
                        واپس -errno;

                اگر ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        اگر (ret)
                                *ret = TAKE_PTR(buf);
                        اگر (ret_سائز)
                                *ret_سائز = n;

                        واپس 0;
                }

                bufsize = (size_t) n;
        }
}

صحیح keyring_describe(کلید_serial_t serial, حرف **ret) {
        _cleanup_free_ حرف *tuple = تہی;
        size_t sz = 64;
        صحیح c;

        أكد(ret);

        برائے (;;) {
                tuple = new(حرف, sz);
                اگر (!tuple)
                        واپس log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (غیرعلامتی طویل) tuple, sz, 0);
                اگر (c < 0)
                        واپس log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                اگر ((size_t) c <= sz)
                        توڑو;

                sz = c;
                آزاد(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        أكد(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        واپس 0;
}
