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

целое parse_devnum(const char *s, dev_t *ret) {
        const char *major;
        unsigned x, y;
        size_t n;
        целое r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        если (n == 0)
                вернуть -EINVAL;
        если (n > DECIMAL_STR_MAX(dev_t))
                вернуть -EINVAL;
        если (s[n] != ':')
                вернуть -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        если (r < 0)
                вернуть r;

        r = safe_atou(s + n + 1, &y);
        если (r < 0)
                вернуть r;

        если (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                вернуть -ERANGE;

        *ret = makedev(x, y);
        вернуть 0;
}

целое device_path_make_major_minor(mode_t mode, dev_t devnum, char **ret) {
        const char *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        если (S_ISCHR(mode))
                t = "char";
        иначе если (S_ISBLK(mode))
                t = "block";
        иначе
                вернуть -ENODEV;

        если (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                вернуть -ENOMEM;

        вернуть 0;
}

целое device_path_make_inaccessible(mode_t mode, char **ret) {
        const char *s;

        assert(ret);

        если (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        иначе если (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        иначе
                вернуть -ENODEV;

        вернуть strdup_to(ret, s);
}

целое device_path_make_canonical(mode_t mode, dev_t devnum, char **ret) {
        _cleanup_free_ char *p = NULL;
        целое r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        если (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                вернуть device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        если (r < 0)
                вернуть r;

        вернуть chase(p, NULL, 0, ret, NULL);
}

целое device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        целое r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        если (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } иначе если (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } иначе {
                const char *w;

                w = path_startswith(path, "/dev/block/");
                если (w)
                        mode = S_IFBLK;
                иначе {
                        w = path_startswith(path, "/dev/char/");
                        если (!w)
                                вернуть -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                если (r < 0)
                        вернуть r;
        }

        если (ret_mode)
                *ret_mode = mode;
        если (ret_devnum)
                *ret_devnum = devnum;

        вернуть 0;
}

char* format_devnum(dev_t d, char buf[static DEVNUM_STR_MAX]) {
        вернуть ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
