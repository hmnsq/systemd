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

ціле parse_devnum(const char *s, dev_t *ret) {
        const char *major;
        unsigned x, y;
        size_t n;
        ціле r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        якщо (n == 0)
                повернути -EINVAL;
        якщо (n > DECIMAL_STR_MAX(dev_t))
                повернути -EINVAL;
        якщо (s[n] != ':')
                повернути -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        якщо (r < 0)
                повернути r;

        r = safe_atou(s + n + 1, &y);
        якщо (r < 0)
                повернути r;

        якщо (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                повернути -ERANGE;

        *ret = makedev(x, y);
        повернути 0;
}

ціле device_path_make_major_minor(mode_t mode, dev_t devnum, char **ret) {
        const char *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        якщо (S_ISCHR(mode))
                t = "char";
        інакше якщо (S_ISBLK(mode))
                t = "block";
        інакше
                повернути -ENODEV;

        якщо (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                повернути -ENOMEM;

        повернути 0;
}

ціле device_path_make_inaccessible(mode_t mode, char **ret) {
        const char *s;

        assert(ret);

        якщо (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        інакше якщо (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        інакше
                повернути -ENODEV;

        повернути strdup_to(ret, s);
}

ціле device_path_make_canonical(mode_t mode, dev_t devnum, char **ret) {
        _cleanup_free_ char *p = NULL;
        ціле r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        якщо (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                повернути device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        якщо (r < 0)
                повернути r;

        повернути chase(p, NULL, 0, ret, NULL);
}

ціле device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ціле r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        якщо (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } інакше якщо (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } інакше {
                const char *w;

                w = path_startswith(path, "/dev/block/");
                якщо (w)
                        mode = S_IFBLK;
                інакше {
                        w = path_startswith(path, "/dev/char/");
                        якщо (!w)
                                повернути -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_mode)
                *ret_mode = mode;
        якщо (ret_devnum)
                *ret_devnum = devnum;

        повернути 0;
}

char* format_devnum(dev_t d, char buf[static DEVNUM_STR_MAX]) {
        повернути ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
