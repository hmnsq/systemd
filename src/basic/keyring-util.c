/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "keyring-util.h"
#অন্তর্ভাব "log.h"

পূর্ণাঙ্ক keyring_read(চাবি_serial_t serial, শূন্য **ret, size_t *ret_size) {
        size_t bufsize = 100;

        প্রতি (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = রিক্ত;
                long n;

                buf = new(uint8_t, bufsize + 1);
                যদি (!buf)
                        প্রত্যাগম -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                যদি (n < 0)
                        প্রত্যাগম -errno;

                যদি ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        যদি (ret)
                                *ret = TAKE_PTR(buf);
                        যদি (ret_size)
                                *ret_size = n;

                        প্রত্যাগম 0;
                }

                bufsize = (size_t) n;
        }
}

পূর্ণাঙ্ক keyring_describe(চাবি_serial_t serial, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *tuple = রিক্ত;
        size_t sz = 64;
        পূর্ণাঙ্ক c;

        প্রতিজ্ঞা(ret);

        প্রতি (;;) {
                tuple = new(বর্ণ, sz);
                যদি (!tuple)
                        প্রত্যাগম log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                যদি (c < 0)
                        প্রত্যাগম log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                যদি ((size_t) c <= sz)
                        ভঙ্গ;

                sz = c;
                মুক্ত(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        প্রতিজ্ঞা(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        প্রত্যাগম 0;
}
