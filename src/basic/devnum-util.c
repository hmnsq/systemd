/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "alloc-util.h"
#include "chase.h"
#include "devnum-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "stdio-util.h"
#include "string-util.h"

整数 parse_devnum(const char *s, dev_t *ret) {
        const char *major;
        unsigned x, y;
        size_t n;
        整数 r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        如果 (n == 0)
                返回 -EINVAL;
        如果 (n > DECIMAL_STR_MAX(dev_t))
                返回 -EINVAL;
        如果 (s[n] != ':')
                返回 -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        如果 (r < 0)
                返回 r;

        r = safe_atou(s + n + 1, &y);
        如果 (r < 0)
                返回 r;

        如果 (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                返回 -ERANGE;

        *ret = makedev(x, y);
        返回 0;
}

整数 device_path_make_major_minor(mode_t mode, dev_t devnum, char **ret) {
        const char *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        如果 (S_ISCHR(mode))
                t = "char";
        否则 如果 (S_ISBLK(mode))
                t = "block";
        否则
                返回 -ENODEV;

        如果 (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                返回 -ENOMEM;

        返回 0;
}

整数 device_path_make_inaccessible(mode_t mode, char **ret) {
        const char *s;

        assert(ret);

        如果 (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        否则 如果 (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        否则
                返回 -ENODEV;

        返回 strdup_to(ret, s);
}

整数 device_path_make_canonical(mode_t mode, dev_t devnum, char **ret) {
        _cleanup_free_ char *p = NULL;
        整数 r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        如果 (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                返回 device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        如果 (r < 0)
                返回 r;

        返回 chase(p, NULL, 0, ret, NULL);
}

整数 device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        整数 r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        如果 (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } 否则 如果 (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } 否则 {
                const char *w;

                w = path_startswith(path, "/dev/block/");
                如果 (w)
                        mode = S_IFBLK;
                否则 {
                        w = path_startswith(path, "/dev/char/");
                        如果 (!w)
                                返回 -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                如果 (r < 0)
                        返回 r;
        }

        如果 (ret_mode)
                *ret_mode = mode;
        如果 (ret_devnum)
                *ret_devnum = devnum;

        返回 0;
}

char* format_devnum(dev_t d, char buf[static DEVNUM_STR_MAX]) {
        返回 ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
