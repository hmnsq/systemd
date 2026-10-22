/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "keyring-util.h"
#અન્તર્ભાવ "log.h"

પૂર્ણાંક keyring_read(ચાવી_serial_t serial, શૂન્ય **ret, size_t *ret_size) {
        size_t bufsize = 100;

        માટે (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ખાલી;
                long n;

                buf = new(uint8_t, bufsize + 1);
                જો (!buf)
                        પાછા -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                જો (n < 0)
                        પાછા -errno;

                જો ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        જો (ret)
                                *ret = TAKE_PTR(buf);
                        જો (ret_size)
                                *ret_size = n;

                        પાછા 0;
                }

                bufsize = (size_t) n;
        }
}

પૂર્ણાંક keyring_describe(ચાવી_serial_t serial, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *tuple = ખાલી;
        size_t sz = 64;
        પૂર્ણાંક c;

        પ્રતિજ્ઞા(ret);

        માટે (;;) {
                tuple = new(અક્ષર, sz);
                જો (!tuple)
                        પાછા log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                જો (c < 0)
                        પાછા log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                જો ((size_t) c <= sz)
                        તોડો;

                sz = c;
                મુક્ત(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        પ્રતિજ્ઞા(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        પાછા 0;
}
