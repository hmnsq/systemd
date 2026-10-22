/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <sys/sysmacros.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "devnum-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_devnum(ଅଚଳ ଅକ୍ଷର *s, dev_t *ret) {
        ଅଚଳ ଅକ୍ଷର *major;
        unsigned x, y;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        n = strspn(s, DIGITS);
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (n > DECIMAL_STR_MAX(dev_t))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (s[n] != ':')
                ଫେରନ୍ତୁ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = safe_atou(s + n + 1, &y);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ଫେରନ୍ତୁ -ERANGE;

        *ret = makedev(x, y);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା device_path_make_major_minor(mode_t mode, dev_t devnum, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ଯଦି (S_ISCHR(mode))
                t = "char";
        ଅନ୍ୟଥା ଯଦି (S_ISBLK(mode))
                t = "block";
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -ENODEV;

        ଯଦି (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା device_path_make_inaccessible(mode_t mode, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        ଅନ୍ୟଥା ଯଦି (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -ENODEV;

        ଫେରନ୍ତୁ strdup_to(ret, s);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା device_path_make_canonical(mode_t mode, dev_t devnum, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ଫେରନ୍ତୁ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ chase(p, ଖାଲି, 0, ret, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା device_path_parse_major_minor(ଅଚଳ ଅକ୍ଷର *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ଯଦି (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } ଅନ୍ୟଥା ଯଦି (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } ଅନ୍ୟଥା {
                ଅଚଳ ଅକ୍ଷର *w;

                w = path_startswith(path, "/dev/block/");
                ଯଦି (w)
                        mode = S_IFBLK;
                ଅନ୍ୟଥା {
                        w = path_startswith(path, "/dev/char/");
                        ଯଦି (!w)
                                ଫେରନ୍ତୁ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_mode)
                *ret_mode = mode;
        ଯଦି (ret_devnum)
                *ret_devnum = devnum;

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* format_devnum(dev_t d, ଅକ୍ଷର buf[ସ୍ଥିର DEVNUM_STR_MAX]) {
        ଫେରନ୍ତୁ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
