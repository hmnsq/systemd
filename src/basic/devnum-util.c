/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sysmacros.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "devnum-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ parse_devnum(ਅਚਲ ਅੱਖਰ *s, dev_t *ret) {
        ਅਚਲ ਅੱਖਰ *major;
        ਬਿਨਾਨਿਸ਼ਾਨੀ x, y;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        n = strspn(s, DIGITS);
        ਜੇ (n == 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (n > DECIMAL_STR_MAX(dev_t))
                ਵਾਪਸ -EINVAL;
        ਜੇ (s[n] != ':')
                ਵਾਪਸ -EINVAL;

        major = strndupa_ਸੁਰੱਖਿਅਤ(s, n);
        r = ਸੁਰੱਖਿਅਤ_atou(major, &x);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atou(s + n + 1, &y);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ਵਾਪਸ -ERANGE;

        *ret = makedev(x, y);
        ਵਾਪਸ 0;
}

ਪੂਰਨ device_ਰਸਤਾ_make_major_minor(mode_t ਬਹੁਲਕ, dev_t devnum, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ਜੇ (S_ISCHR(ਬਹੁਲਕ))
                t = "char";
        ਨਹੀਂਤਾਂ ਜੇ (S_ISBLK(ਬਹੁਲਕ))
                t = "block";
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -ENODEV;

        ਜੇ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ 0;
}

ਪੂਰਨ device_ਰਸਤਾ_make_inaccessible(mode_t ਬਹੁਲਕ, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (S_ISCHR(ਬਹੁਲਕ))
                s = "/run/systemd/inaccessible/chr";
        ਨਹੀਂਤਾਂ ਜੇ (S_ISBLK(ਬਹੁਲਕ))
                s = "/run/systemd/inaccessible/blk";
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -ENODEV;

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, s);
}

ਪੂਰਨ device_ਰਸਤਾ_make_canonical(mode_t ਬਹੁਲਕ, dev_t devnum, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ਵਾਪਸ device_ਰਸਤਾ_make_inaccessible(ਬਹੁਲਕ, ret);

        r = device_ਰਸਤਾ_make_major_minor(ਬਹੁਲਕ, devnum, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ chase(p, ਨਿਰਾ, 0, ret, ਨਿਰਾ);
}

ਪੂਰਨ device_ਰਸਤਾ_parse_major_minor(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t *ret_ਬਹੁਲਕ, dev_t *ret_devnum) {
        mode_t ਬਹੁਲਕ;
        dev_t devnum;
        ਪੂਰਨ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ਜੇ (ਰਸਤਾ_equal(ਰਸਤਾ, "/run/systemd/inaccessible/chr")) {
                ਬਹੁਲਕ = S_IFCHR;
                devnum = makedev(0, 0);
        } ਨਹੀਂਤਾਂ ਜੇ (ਰਸਤਾ_equal(ਰਸਤਾ, "/run/systemd/inaccessible/blk")) {
                ਬਹੁਲਕ = S_IFBLK;
                devnum = makedev(0, 0);
        } ਨਹੀਂਤਾਂ {
                ਅਚਲ ਅੱਖਰ *w;

                w = ਰਸਤਾ_startswith(ਰਸਤਾ, "/dev/block/");
                ਜੇ (w)
                        ਬਹੁਲਕ = S_IFBLK;
                ਨਹੀਂਤਾਂ {
                        w = ਰਸਤਾ_startswith(ਰਸਤਾ, "/dev/char/");
                        ਜੇ (!w)
                                ਵਾਪਸ -ENODEV;

                        ਬਹੁਲਕ = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_ਬਹੁਲਕ)
                *ret_ਬਹੁਲਕ = ਬਹੁਲਕ;
        ਜੇ (ret_devnum)
                *ret_devnum = devnum;

        ਵਾਪਸ 0;
}

ਅੱਖਰ* format_devnum(dev_t d, ਅੱਖਰ buf[ਸਥਿਰ DEVNUM_STR_MAX]) {
        ਵਾਪਸ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
