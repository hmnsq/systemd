/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <string.h>
#شامل <sys/stat.h>
#شامل <sys/sysmacros.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "devnum-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

صحیح parse_devnum(ثابت نویسه *s, dev_t *ret) {
        ثابت نویسه *major;
        unsigned x, y;
        size_t n;
        صحیح r;

        أكد(s);
        أكد(ret);

        n = strspn(s, DIGITS);
        اگر (n == 0)
                بازگشت -EINVAL;
        اگر (n > DECIMAL_STR_MAX(dev_t))
                بازگشت -EINVAL;
        اگر (s[n] != ':')
                بازگشت -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        اگر (r < 0)
                بازگشت r;

        r = safe_atou(s + n + 1, &y);
        اگر (r < 0)
                بازگشت r;

        اگر (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                بازگشت -ERANGE;

        *ret = makedev(x, y);
        بازگشت 0;
}

صحیح device_path_make_major_minor(mode_t mode, dev_t devnum, نویسه **ret) {
        ثابت نویسه *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        اگر (S_ISCHR(mode))
                t = "char";
        وگرنه اگر (S_ISBLK(mode))
                t = "block";
        وگرنه
                بازگشت -ENODEV;

        اگر (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                بازگشت -ENOMEM;

        بازگشت 0;
}

صحیح device_path_make_inaccessible(mode_t mode, نویسه **ret) {
        ثابت نویسه *s;

        أكد(ret);

        اگر (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        وگرنه اگر (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        وگرنه
                بازگشت -ENODEV;

        بازگشت strdup_to(ret, s);
}

صحیح device_path_make_canonical(mode_t mode, dev_t devnum, نویسه **ret) {
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        أكد(ret);

        اگر (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                بازگشت device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        اگر (r < 0)
                بازگشت r;

        بازگشت chase(p, پوچ, 0, ret, پوچ);
}

صحیح device_path_parse_major_minor(ثابت نویسه *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        صحیح r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        اگر (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } وگرنه اگر (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } وگرنه {
                ثابت نویسه *w;

                w = path_startswith(path, "/dev/block/");
                اگر (w)
                        mode = S_IFBLK;
                وگرنه {
                        w = path_startswith(path, "/dev/char/");
                        اگر (!w)
                                بازگشت -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_mode)
                *ret_mode = mode;
        اگر (ret_devnum)
                *ret_devnum = devnum;

        بازگشت 0;
}

نویسه* format_devnum(dev_t d, نویسه buf[ایستا DEVNUM_STR_MAX]) {
        بازگشت ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
