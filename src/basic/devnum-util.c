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

จำนวนเต็ม parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        จำนวนเต็ม r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        ถ้า (n == 0)
                คืน -EINVAL;
        ถ้า (n > DECIMAL_STR_MAX(dev_t))
                คืน -EINVAL;
        ถ้า (s[n] != ':')
                คืน -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ถ้า (r < 0)
                คืน r;

        r = safe_atou(s + n + 1, &y);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                คืน -ERANGE;

        *ret = makedev(x, y);
        คืน 0;
}

จำนวนเต็ม device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ถ้า (S_ISCHR(mode))
                t = "char";
        มิฉะนั้น ถ้า (S_ISBLK(mode))
                t = "block";
        มิฉะนั้น
                คืน -ENODEV;

        ถ้า (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                คืน -ENOMEM;

        คืน 0;
}

จำนวนเต็ม device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        ถ้า (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        มิฉะนั้น ถ้า (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        มิฉะนั้น
                คืน -ENODEV;

        คืน strdup_to(ret, s);
}

จำนวนเต็ม device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        ถ้า (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                คืน device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ถ้า (r < 0)
                คืน r;

        คืน chase(p, NULL, 0, ret, NULL);
}

จำนวนเต็ม device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        จำนวนเต็ม r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ถ้า (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } มิฉะนั้น ถ้า (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } มิฉะนั้น {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                ถ้า (w)
                        mode = S_IFBLK;
                มิฉะนั้น {
                        w = path_startswith(path, "/dev/char/");
                        ถ้า (!w)
                                คืน -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (ret_mode)
                *ret_mode = mode;
        ถ้า (ret_devnum)
                *ret_devnum = devnum;

        คืน 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        คืน ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
