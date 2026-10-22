/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "keyring-util.h"
#அந்தர்பாவ "log.h"

முழு keyring_read(சாவி_serial_t serial, வெறுமை **ret, size_t *ret_size) {
        size_t bufsize = 100;

        க்கு (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = வெற்று;
                long n;

                buf = new(uint8_t, bufsize + 1);
                எனில் (!buf)
                        திரும்பு -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                எனில் (n < 0)
                        திரும்பு -errno;

                எனில் ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        எனில் (ret)
                                *ret = TAKE_PTR(buf);
                        எனில் (ret_size)
                                *ret_size = n;

                        திரும்பு 0;
                }

                bufsize = (size_t) n;
        }
}

முழு keyring_describe(சாவி_serial_t serial, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *tuple = வெற்று;
        size_t sz = 64;
        முழு c;

        ப்ரதிஜ்ஞா(ret);

        க்கு (;;) {
                tuple = new(எழுத்து, sz);
                எனில் (!tuple)
                        திரும்பு log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                எனில் (c < 0)
                        திரும்பு log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                எனில் ((size_t) c <= sz)
                        நிறுத்து;

                sz = c;
                விடுவி(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ப்ரதிஜ்ஞா(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        திரும்பு 0;
}
