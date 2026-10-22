/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <sys/sysmacros.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "chase.h"
#অন্তর্ভাৱ "devnum-util.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক parse_devnum(ধ্রুৱ ৱর্ণ *s, dev_t *ret) {
        ধ্রুৱ ৱর্ণ *major;
        unsigned x, y;
        size_t n;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        n = strspn(s, DIGITS);
        যদি (n == 0)
                ঘূৰাই -EINVAL;
        যদি (n > DECIMAL_STR_MAX(dev_t))
                ঘূৰাই -EINVAL;
        যদি (s[n] != ':')
                ঘূৰাই -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        যদি (r < 0)
                ঘূৰাই r;

        r = safe_atou(s + n + 1, &y);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ঘূৰাই -ERANGE;

        *ret = makedev(x, y);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক device_path_make_major_minor(mode_t mode, dev_t devnum, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        যদি (S_ISCHR(mode))
                t = "char";
        অন্যথা যদি (S_ISBLK(mode))
                t = "block";
        অন্যথা
                ঘূৰাই -ENODEV;

        যদি (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ঘূৰাই -ENOMEM;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক device_path_make_inaccessible(mode_t mode, ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *s;

        প্রতিজ্ঞা(ret);

        যদি (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        অন্যথা যদি (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        অন্যথা
                ঘূৰাই -ENODEV;

        ঘূৰাই strdup_to(ret, s);
}

পূর্ণাঙ্ক device_path_make_canonical(mode_t mode, dev_t devnum, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        প্রতিজ্ঞা(ret);

        যদি (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ঘূৰাই device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই chase(p, রিক্ত, 0, ret, রিক্ত);
}

পূর্ণাঙ্ক device_path_parse_major_minor(ধ্রুৱ ৱর্ণ *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        পূর্ণাঙ্ক r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        যদি (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } অন্যথা যদি (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } অন্যথা {
                ধ্রুৱ ৱর্ণ *w;

                w = path_startswith(path, "/dev/block/");
                যদি (w)
                        mode = S_IFBLK;
                অন্যথা {
                        w = path_startswith(path, "/dev/char/");
                        যদি (!w)
                                ঘূৰাই -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (ret_mode)
                *ret_mode = mode;
        যদি (ret_devnum)
                *ret_devnum = devnum;

        ঘূৰাই 0;
}

ৱর্ণ* format_devnum(dev_t d, ৱর্ণ buf[স্থির DEVNUM_STR_MAX]) {
        ঘূৰাই ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
