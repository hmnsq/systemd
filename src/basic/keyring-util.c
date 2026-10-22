/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "keyring-util.h"
#অন্তর্ভাব "log.h"

পূর্ণসংখ্যা keyring_read(চাবি_serial_t serial, শূন্য **ret, size_t *ret_আকার) {
        size_t bufsize = 100;

        জন্য (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ফাঁকা;
                long n;

                buf = new(uint8_t, bufsize + 1);
                যদি (!buf)
                        ফেরত -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                যদি (n < 0)
                        ফেরত -errno;

                যদি ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        যদি (ret)
                                *ret = TAKE_PTR(buf);
                        যদি (ret_আকার)
                                *ret_আকার = n;

                        ফেরত 0;
                }

                bufsize = (size_t) n;
        }
}

পূর্ণসংখ্যা keyring_describe(চাবি_serial_t serial, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *tuple = ফাঁকা;
        size_t sz = 64;
        পূর্ণসংখ্যা c;

        প্রতিজ্ঞা(ret);

        জন্য (;;) {
                tuple = new(অক্ষর, sz);
                যদি (!tuple)
                        ফেরত log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                যদি (c < 0)
                        ফেরত log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                যদি ((size_t) c <= sz)
                        বিরতি;

                sz = c;
                মুক্ত(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        প্রতিজ্ঞা(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        ফেরত 0;
}
