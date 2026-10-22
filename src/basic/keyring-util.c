/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "keyring-util.h"
#അന്തര്ഭാവ "log.h"

പൂർണ്ണസംഖ്യ keyring_read(താക്കോൽ_serial_t serial, ശൂന്യം **ret, size_t *ret_size) {
        size_t bufsize = 100;

        വേണ്ടി (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ശൂന്യ;
                long n;

                buf = new(uint8_t, bufsize + 1);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                n = keyctl(KEYCTL_READ, (unsigned long) serial, (unsigned long) buf, (unsigned long) bufsize, 0);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;

                എങ്കിൽ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        എങ്കിൽ (ret)
                                *ret = TAKE_PTR(buf);
                        എങ്കിൽ (ret_size)
                                *ret_size = n;

                        തിരികെ 0;
                }

                bufsize = (size_t) n;
        }
}

പൂർണ്ണസംഖ്യ keyring_describe(താക്കോൽ_serial_t serial, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *tuple = ശൂന്യ;
        size_t sz = 64;
        പൂർണ്ണസംഖ്യ c;

        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (;;) {
                tuple = new(അക്ഷരം, sz);
                എങ്കിൽ (!tuple)
                        തിരികെ log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (unsigned long) tuple, sz, 0);
                എങ്കിൽ (c < 0)
                        തിരികെ log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                എങ്കിൽ ((size_t) c <= sz)
                        നിർത്തുക;

                sz = c;
                സ്വതന്ത്രം(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        പ്രതിജ്ഞാ(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        തിരികെ 0;
}
