/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "keyring-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyring_read(ଚାବି_serial_t serial, ଶୂନ୍ୟ **ret, size_t *ret_size) {
        size_t bufsize = 100;

        ପାଇଁ (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ଖାଲି;
                long n;

                buf = new(uint8_t, bufsize + 1);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ଯଦି (ret)
                                *ret = TAKE_PTR(buf);
                        ଯଦି (ret_size)
                                *ret_size = n;

                        ଫେରନ୍ତୁ 0;
                }

                bufsize = (size_t) n;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyring_describe(ଚାବି_serial_t serial, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *tuple = ଖାଲି;
        size_t sz = 64;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (;;) {
                tuple = new(ଅକ୍ଷର, sz);
                ଯଦି (!tuple)
                        ଫେରନ୍ତୁ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                ଯଦି (c < 0)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ଯଦି ((size_t) c <= sz)
                        ଭାଙ୍ଗ;

                sz = c;
                ମୁକ୍ତ(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ପ୍ରତିଜ୍ଞା(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ଫେରନ୍ତୁ 0;
}
