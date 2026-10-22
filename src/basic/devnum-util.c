/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <sys/sysmacros.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "devnum-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક parse_devnum(અચલ અક્ષર *s, dev_t *ret) {
        અચલ અક્ષર *major;
        unsigned x, y;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        n = strspn(s, DIGITS);
        જો (n == 0)
                પાછા -EINVAL;
        જો (n > DECIMAL_STR_MAX(dev_t))
                પાછા -EINVAL;
        જો (s[n] != ':')
                પાછા -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        જો (r < 0)
                પાછા r;

        r = safe_atou(s + n + 1, &y);
        જો (r < 0)
                પાછા r;

        જો (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                પાછા -ERANGE;

        *ret = makedev(x, y);
        પાછા 0;
}

પૂર્ણાંક device_path_make_major_minor(mode_t mode, dev_t devnum, અક્ષર **ret) {
        અચલ અક્ષર *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        જો (S_ISCHR(mode))
                t = "char";
        નહીંતર જો (S_ISBLK(mode))
                t = "block";
        નહીંતર
                પાછા -ENODEV;

        જો (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                પાછા -ENOMEM;

        પાછા 0;
}

પૂર્ણાંક device_path_make_inaccessible(mode_t mode, અક્ષર **ret) {
        અચલ અક્ષર *s;

        પ્રતિજ્ઞા(ret);

        જો (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        નહીંતર જો (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        નહીંતર
                પાછા -ENODEV;

        પાછા strdup_to(ret, s);
}

પૂર્ણાંક device_path_make_canonical(mode_t mode, dev_t devnum, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        પ્રતિજ્ઞા(ret);

        જો (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                પાછા device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        જો (r < 0)
                પાછા r;

        પાછા chase(p, ખાલી, 0, ret, ખાલી);
}

પૂર્ણાંક device_path_parse_major_minor(અચલ અક્ષર *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        પૂર્ણાંક r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        જો (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } નહીંતર જો (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } નહીંતર {
                અચલ અક્ષર *w;

                w = path_startswith(path, "/dev/block/");
                જો (w)
                        mode = S_IFBLK;
                નહીંતર {
                        w = path_startswith(path, "/dev/char/");
                        જો (!w)
                                પાછા -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_mode)
                *ret_mode = mode;
        જો (ret_devnum)
                *ret_devnum = devnum;

        પાછા 0;
}

અક્ષર* format_devnum(dev_t d, અક્ષર buf[સ્થિર DEVNUM_STR_MAX]) {
        પાછા ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
