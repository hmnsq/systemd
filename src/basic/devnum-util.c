/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <string.h>
#ضمن <sys/stat.h>
#ضمن <sys/sysmacros.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "devnum-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

صحيح parse_devnum(ثابت حرف *s, dev_t *ret) {
        ثابت حرف *major;
        بیعلامت x, y;
        size_t n;
        صحيح r;

        أكد(s);
        أكد(ret);

        n = strspn(s, DIGITS);
        إذا (n == 0)
                أرجع -EINVAL;
        إذا (n > DECIMAL_STR_MAX(dev_t))
                أرجع -EINVAL;
        إذا (s[n] != ':')
                أرجع -EINVAL;

        major = strndupa_امن(s, n);
        r = امن_atou(major, &x);
        إذا (r < 0)
                أرجع r;

        r = امن_atou(s + n + 1, &y);
        إذا (r < 0)
                أرجع r;

        إذا (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                أرجع -ERANGE;

        *ret = makedev(x, y);
        أرجع 0;
}

صحيح device_مسیر_make_major_minor(mode_t نما, dev_t devnum, حرف **ret) {
        ثابت حرف *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        إذا (S_ISCHR(نما))
                t = "char";
        وإلا إذا (S_ISBLK(نما))
                t = "block";
        وإلا
                أرجع -ENODEV;

        إذا (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح device_مسیر_make_inaccessible(mode_t نما, حرف **ret) {
        ثابت حرف *s;

        أكد(ret);

        إذا (S_ISCHR(نما))
                s = "/run/systemd/inaccessible/chr";
        وإلا إذا (S_ISBLK(نما))
                s = "/run/systemd/inaccessible/blk";
        وإلا
                أرجع -ENODEV;

        أرجع strdup_مقصد(ret, s);
}

صحيح device_مسیر_make_canonical(mode_t نما, dev_t devnum, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        أكد(ret);

        إذا (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                أرجع device_مسیر_make_inaccessible(نما, ret);

        r = device_مسیر_make_major_minor(نما, devnum, &p);
        إذا (r < 0)
                أرجع r;

        أرجع chase(p, عدم, 0, ret, عدم);
}

صحيح device_مسیر_parse_major_minor(ثابت حرف *مسیر, mode_t *ret_نما, dev_t *ret_devnum) {
        mode_t نما;
        dev_t devnum;
        صحيح r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        إذا (مسیر_equal(مسیر, "/run/systemd/inaccessible/chr")) {
                نما = S_IFCHR;
                devnum = makedev(0, 0);
        } وإلا إذا (مسیر_equal(مسیر, "/run/systemd/inaccessible/blk")) {
                نما = S_IFBLK;
                devnum = makedev(0, 0);
        } وإلا {
                ثابت حرف *w;

                w = مسیر_startswith(مسیر, "/dev/block/");
                إذا (w)
                        نما = S_IFBLK;
                وإلا {
                        w = مسیر_startswith(مسیر, "/dev/char/");
                        إذا (!w)
                                أرجع -ENODEV;

                        نما = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_نما)
                *ret_نما = نما;
        إذا (ret_devnum)
                *ret_devnum = devnum;

        أرجع 0;
}

حرف* format_devnum(dev_t d, حرف buf[ساكن DEVNUM_STR_MAX]) {
        أرجع ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
