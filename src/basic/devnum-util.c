/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "devnum-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ parse_devnum(ಧ್ರುವ ವರ್ಣ *s, dev_t *ret) {
        ಧ್ರುವ ವರ್ಣ *major;
        unsigned x, y;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        n = strspn(s, DIGITS);
        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (n > DECIMAL_STR_MAX(dev_t))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (s[n] != ':')
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atou(s + n + 1, &y);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = makedev(x, y);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ device_path_make_major_minor(mode_t mode, dev_t devnum, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ಯದಿ (S_ISCHR(mode))
                t = "char";
        ಅನ್ಯಥಾ ಯದಿ (S_ISBLK(mode))
                t = "block";
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -ENODEV;

        ಯದಿ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ device_path_make_inaccessible(mode_t mode, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        ಅನ್ಯಥಾ ಯದಿ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -ENODEV;

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, s);
}

ಪೂರ್ಣಾಙ್ಕ device_path_make_canonical(mode_t mode, dev_t devnum, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ಪ್ರತ್ಯಾಗಮ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ chase(p, ರಿಕ್ತ, 0, ret, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ device_path_parse_major_minor(ಧ್ರುವ ವರ್ಣ *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ಯದಿ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } ಅನ್ಯಥಾ ಯದಿ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } ಅನ್ಯಥಾ {
                ಧ್ರುವ ವರ್ಣ *w;

                w = path_startswith(path, "/dev/block/");
                ಯದಿ (w)
                        mode = S_IFBLK;
                ಅನ್ಯಥಾ {
                        w = path_startswith(path, "/dev/char/");
                        ಯದಿ (!w)
                                ಪ್ರತ್ಯಾಗಮ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_mode)
                *ret_mode = mode;
        ಯದಿ (ret_devnum)
                *ret_devnum = devnum;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* format_devnum(dev_t d, ವರ್ಣ buf[ಸ್ಥಿರ DEVNUM_STR_MAX]) {
        ಪ್ರತ್ಯಾಗಮ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
