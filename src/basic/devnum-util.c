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

صحیح parse_devnum(ثابت حرف *s, dev_t *ret) {
        ثابت حرف *major;
        غیرعلامتی x, y;
        size_t n;
        صحیح r;

        أكد(s);
        أكد(ret);

        n = strspn(s, DIGITS);
        اگر (n == 0)
                واپس -EINVAL;
        اگر (n > DECIMAL_STR_MAX(dev_t))
                واپس -EINVAL;
        اگر (s[n] != ':')
                واپس -EINVAL;

        major = strndupa_محفوظ(s, n);
        r = محفوظ_atou(major, &x);
        اگر (r < 0)
                واپس r;

        r = محفوظ_atou(s + n + 1, &y);
        اگر (r < 0)
                واپس r;

        اگر (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                واپس -ERANGE;

        *ret = makedev(x, y);
        واپس 0;
}

صحیح device_راستہ_make_major_minor(mode_t منوال, dev_t devnum, حرف **ret) {
        ثابت حرف *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        اگر (S_ISCHR(منوال))
                t = "char";
        ورنہ اگر (S_ISBLK(منوال))
                t = "block";
        ورنہ
                واپس -ENODEV;

        اگر (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                واپس -ENOMEM;

        واپس 0;
}

صحیح device_راستہ_make_inaccessible(mode_t منوال, حرف **ret) {
        ثابت حرف *s;

        أكد(ret);

        اگر (S_ISCHR(منوال))
                s = "/run/systemd/inaccessible/chr";
        ورنہ اگر (S_ISBLK(منوال))
                s = "/run/systemd/inaccessible/blk";
        ورنہ
                واپس -ENODEV;

        واپس strdup_مقصد(ret, s);
}

صحیح device_راستہ_make_canonical(mode_t منوال, dev_t devnum, حرف **ret) {
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        أكد(ret);

        اگر (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                واپس device_راستہ_make_inaccessible(منوال, ret);

        r = device_راستہ_make_major_minor(منوال, devnum, &p);
        اگر (r < 0)
                واپس r;

        واپس chase(p, تہی, 0, ret, تہی);
}

صحیح device_راستہ_parse_major_minor(ثابت حرف *راستہ, mode_t *ret_منوال, dev_t *ret_devnum) {
        mode_t منوال;
        dev_t devnum;
        صحیح r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        اگر (راستہ_equal(راستہ, "/run/systemd/inaccessible/chr")) {
                منوال = S_IFCHR;
                devnum = makedev(0, 0);
        } ورنہ اگر (راستہ_equal(راستہ, "/run/systemd/inaccessible/blk")) {
                منوال = S_IFBLK;
                devnum = makedev(0, 0);
        } ورنہ {
                ثابت حرف *w;

                w = راستہ_startswith(راستہ, "/dev/block/");
                اگر (w)
                        منوال = S_IFBLK;
                ورنہ {
                        w = راستہ_startswith(راستہ, "/dev/char/");
                        اگر (!w)
                                واپس -ENODEV;

                        منوال = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_منوال)
                *ret_منوال = منوال;
        اگر (ret_devnum)
                *ret_devnum = devnum;

        واپس 0;
}

حرف* format_devnum(dev_t d, حرف buf[ساکن DEVNUM_STR_MAX]) {
        واپس ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
