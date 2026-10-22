/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "alloc-util.h"
#include "chase.h"
#include "devnum-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "stdio-util.h"
#include "string-util.h"

số_nguyên parse_devnum(const char *s, dev_t *ret) {
        const char *major;
        unsigned x, y;
        size_t n;
        số_nguyên r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        nếu (n == 0)
                trả_về -EINVAL;
        nếu (n > DECIMAL_STR_MAX(dev_t))
                trả_về -EINVAL;
        nếu (s[n] != ':')
                trả_về -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        nếu (r < 0)
                trả_về r;

        r = safe_atou(s + n + 1, &y);
        nếu (r < 0)
                trả_về r;

        nếu (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                trả_về -ERANGE;

        *ret = makedev(x, y);
        trả_về 0;
}

số_nguyên device_path_make_major_minor(mode_t mode, dev_t devnum, char **ret) {
        const char *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        nếu (S_ISCHR(mode))
                t = "char";
        không_thì nếu (S_ISBLK(mode))
                t = "block";
        không_thì
                trả_về -ENODEV;

        nếu (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                trả_về -ENOMEM;

        trả_về 0;
}

số_nguyên device_path_make_inaccessible(mode_t mode, char **ret) {
        const char *s;

        assert(ret);

        nếu (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        không_thì nếu (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        không_thì
                trả_về -ENODEV;

        trả_về strdup_to(ret, s);
}

số_nguyên device_path_make_canonical(mode_t mode, dev_t devnum, char **ret) {
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        nếu (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                trả_về device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        nếu (r < 0)
                trả_về r;

        trả_về chase(p, NULL, 0, ret, NULL);
}

số_nguyên device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        số_nguyên r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        nếu (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } không_thì nếu (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } không_thì {
                const char *w;

                w = path_startswith(path, "/dev/block/");
                nếu (w)
                        mode = S_IFBLK;
                không_thì {
                        w = path_startswith(path, "/dev/char/");
                        nếu (!w)
                                trả_về -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_mode)
                *ret_mode = mode;
        nếu (ret_devnum)
                *ret_devnum = devnum;

        trả_về 0;
}

char* format_devnum(dev_t d, char buf[static DEVNUM_STR_MAX]) {
        trả_về ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
