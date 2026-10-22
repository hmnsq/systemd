/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "keyring-util.h"
#అన్తర్భావ "log.h"

పూర్ణాంకం keyring_read(కీలకం_serial_t serial, శూన్య **ret, size_t *ret_కొలత) {
        size_t bufsize = 100;

        కొరకు (;;) {
                _cleanup_(erase_and_freep) uint8_t *buf = ఖాళీ;
                దీర్ఘ n;

                buf = new(uint8_t, bufsize + 1);
                ఒకవేళ (!buf)
                        తిరిగి -ENOMEM;

                n = keyctl(KEYCTL_READ, (గుర్తులేని దీర్ఘ) serial, (గుర్తులేని దీర్ఘ) buf, (గుర్తులేని దీర్ఘ) bufsize, 0);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;

                ఒకవేళ ((size_t) n <= bufsize) {
                        buf[n] = 0; /* NUL terminate, just in case */

                        ఒకవేళ (ret)
                                *ret = TAKE_PTR(buf);
                        ఒకవేళ (ret_కొలత)
                                *ret_కొలత = n;

                        తిరిగి 0;
                }

                bufsize = (size_t) n;
        }
}

పూర్ణాంకం keyring_describe(కీలకం_serial_t serial, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *tuple = ఖాళీ;
        size_t sz = 64;
        పూర్ణాంకం c;

        ప్రతిజ్ఞా(ret);

        కొరకు (;;) {
                tuple = new(అక్షరం, sz);
                ఒకవేళ (!tuple)
                        తిరిగి log_oom_debug();

                c = keyctl(KEYCTL_DESCRIBE, serial, (గుర్తులేని దీర్ఘ) tuple, sz, 0);
                ఒకవేళ (c < 0)
                        తిరిగి log_debug_errno(errno, "Failed to describe key id %d: %m", serial);

                ఒకవేళ ((size_t) c <= sz)
                        ఆపు;

                sz = c;
                విడుదల(tuple);
        }

        /* The kernel returns a final NUL in the string, verify that. */
        ప్రతిజ్ఞా(tuple[c-1] == 0);

        *ret = TAKE_PTR(tuple);

        తిరిగి 0;
}
