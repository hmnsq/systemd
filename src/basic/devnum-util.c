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

ཧྲིལ parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        ཧྲིལ r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        གལ་ཏེ (n == 0)
                ཕྱིར -EINVAL;
        གལ་ཏེ (n > DECIMAL_STR_MAX(dev_t))
                ཕྱིར -EINVAL;
        གལ་ཏེ (s[n] != ':')
                ཕྱིར -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = safe_atou(s + n + 1, &y);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ཕྱིར -ERANGE;

        *ret = makedev(x, y);
        ཕྱིར 0;
}

ཧྲིལ device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        གལ་ཏེ (S_ISCHR(mode))
                t = "char";
        དེ་མིན གལ་ཏེ (S_ISBLK(mode))
                t = "block";
        དེ་མིན
                ཕྱིར -ENODEV;

        གལ་ཏེ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ཕྱིར -ENOMEM;

        ཕྱིར 0;
}

ཧྲིལ device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        གལ་ཏེ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        དེ་མིན གལ་ཏེ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        དེ་མིན
                ཕྱིར -ENODEV;

        ཕྱིར strdup_to(ret, s);
}

ཧྲིལ device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        གལ་ཏེ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ཕྱིར device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར chase(p, NULL, 0, ret, NULL);
}

ཧྲིལ device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ཧྲིལ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        གལ་ཏེ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } དེ་མིན གལ་ཏེ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } དེ་མིན {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                གལ་ཏེ (w)
                        mode = S_IFBLK;
                དེ་མིན {
                        w = path_startswith(path, "/dev/char/");
                        གལ་ཏེ (!w)
                                ཕྱིར -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        གལ་ཏེ (ret_mode)
                *ret_mode = mode;
        གལ་ཏེ (ret_devnum)
                *ret_devnum = devnum;

        ཕྱིར 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ཕྱིར ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
