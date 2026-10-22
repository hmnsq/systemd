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
        بینښه x, y;
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

        major = strndupa_خوندي(s, n);
        r = خوندي_atou(major, &x);
        إذا (r < 0)
                أرجع r;

        r = خوندي_atou(s + n + 1, &y);
        إذا (r < 0)
                أرجع r;

        إذا (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                أرجع -ERANGE;

        *ret = makedev(x, y);
        أرجع 0;
}

صحيح device_لار_make_major_minor(mode_t ډېرپېښ, dev_t devnum, حرف **ret) {
        ثابت حرف *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        إذا (S_ISCHR(ډېرپېښ))
                t = "char";
        وإلا إذا (S_ISBLK(ډېرپېښ))
                t = "block";
        وإلا
                أرجع -ENODEV;

        إذا (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح device_لار_make_inaccessible(mode_t ډېرپېښ, حرف **ret) {
        ثابت حرف *s;

        أكد(ret);

        إذا (S_ISCHR(ډېرپېښ))
                s = "/run/systemd/inaccessible/chr";
        وإلا إذا (S_ISBLK(ډېرپېښ))
                s = "/run/systemd/inaccessible/blk";
        وإلا
                أرجع -ENODEV;

        أرجع strdup_منزل(ret, s);
}

صحيح device_لار_make_canonical(mode_t ډېرپېښ, dev_t devnum, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        أكد(ret);

        إذا (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                أرجع device_لار_make_inaccessible(ډېرپېښ, ret);

        r = device_لار_make_major_minor(ډېرپېښ, devnum, &p);
        إذا (r < 0)
                أرجع r;

        أرجع chase(p, عدم, 0, ret, عدم);
}

صحيح device_لار_parse_major_minor(ثابت حرف *لار, mode_t *ret_ډېرپېښ, dev_t *ret_devnum) {
        mode_t ډېرپېښ;
        dev_t devnum;
        صحيح r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        إذا (لار_equal(لار, "/run/systemd/inaccessible/chr")) {
                ډېرپېښ = S_IFCHR;
                devnum = makedev(0, 0);
        } وإلا إذا (لار_equal(لار, "/run/systemd/inaccessible/blk")) {
                ډېرپېښ = S_IFBLK;
                devnum = makedev(0, 0);
        } وإلا {
                ثابت حرف *w;

                w = لار_startswith(لار, "/dev/block/");
                إذا (w)
                        ډېرپېښ = S_IFBLK;
                وإلا {
                        w = لار_startswith(لار, "/dev/char/");
                        إذا (!w)
                                أرجع -ENODEV;

                        ډېرپېښ = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_ډېرپېښ)
                *ret_ډېرپېښ = ډېرپېښ;
        إذا (ret_devnum)
                *ret_devnum = devnum;

        أرجع 0;
}

حرف* format_devnum(dev_t d, حرف buf[ساكن DEVNUM_STR_MAX]) {
        أرجع ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
