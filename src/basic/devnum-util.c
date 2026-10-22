/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <sys/sysmacros.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "devnum-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা parse_devnum(ধ্রুবক অক্ষর *s, dev_t *ret) {
        ধ্রুবক অক্ষর *major;
        unsigned x, y;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        n = strspn(s, DIGITS);
        যদি (n == 0)
                ফেরত -EINVAL;
        যদি (n > DECIMAL_STR_MAX(dev_t))
                ফেরত -EINVAL;
        যদি (s[n] != ':')
                ফেরত -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        যদি (r < 0)
                ফেরত r;

        r = safe_atou(s + n + 1, &y);
        যদি (r < 0)
                ফেরত r;

        যদি (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ফেরত -ERANGE;

        *ret = makedev(x, y);
        ফেরত 0;
}

পূর্ণসংখ্যা device_path_make_major_minor(mode_t mode, dev_t devnum, অক্ষর **ret) {
        ধ্রুবক অক্ষর *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        যদি (S_ISCHR(mode))
                t = "char";
        অন্যথা যদি (S_ISBLK(mode))
                t = "block";
        অন্যথা
                ফেরত -ENODEV;

        যদি (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ফেরত -ENOMEM;

        ফেরত 0;
}

পূর্ণসংখ্যা device_path_make_inaccessible(mode_t mode, অক্ষর **ret) {
        ধ্রুবক অক্ষর *s;

        প্রতিজ্ঞা(ret);

        যদি (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        অন্যথা যদি (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        অন্যথা
                ফেরত -ENODEV;

        ফেরত strdup_to(ret, s);
}

পূর্ণসংখ্যা device_path_make_canonical(mode_t mode, dev_t devnum, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        প্রতিজ্ঞা(ret);

        যদি (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ফেরত device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        যদি (r < 0)
                ফেরত r;

        ফেরত chase(p, ফাঁকা, 0, ret, ফাঁকা);
}

পূর্ণসংখ্যা device_path_parse_major_minor(ধ্রুবক অক্ষর *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        পূর্ণসংখ্যা r;

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
                ধ্রুবক অক্ষর *w;

                w = path_startswith(path, "/dev/block/");
                যদি (w)
                        mode = S_IFBLK;
                অন্যথা {
                        w = path_startswith(path, "/dev/char/");
                        যদি (!w)
                                ফেরত -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_mode)
                *ret_mode = mode;
        যদি (ret_devnum)
                *ret_devnum = devnum;

        ফেরত 0;
}

অক্ষর* format_devnum(dev_t d, অক্ষর buf[স্থির DEVNUM_STR_MAX]) {
        ফেরত ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
