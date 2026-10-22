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

ចំនួនគត់ parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        ចំនួនគត់ r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        ប្រសិនបើ (n == 0)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (n > DECIMAL_STR_MAX(dev_t))
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (s[n] != ':')
                ត្រឡប់ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = safe_atou(s + n + 1, &y);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ត្រឡប់ -ERANGE;

        *ret = makedev(x, y);
        ត្រឡប់ 0;
}

ចំនួនគត់ device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ប្រសិនបើ (S_ISCHR(mode))
                t = "char";
        បើមិនដូច្នេះ ប្រសិនបើ (S_ISBLK(mode))
                t = "block";
        បើមិនដូច្នេះ
                ត្រឡប់ -ENODEV;

        ប្រសិនបើ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ 0;
}

ចំនួនគត់ device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        ប្រសិនបើ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        បើមិនដូច្នេះ ប្រសិនបើ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        បើមិនដូច្នេះ
                ត្រឡប់ -ENODEV;

        ត្រឡប់ strdup_to(ret, s);
}

ចំនួនគត់ device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ចំនួនគត់ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        ប្រសិនបើ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ត្រឡប់ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ chase(p, NULL, 0, ret, NULL);
}

ចំនួនគត់ device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ចំនួនគត់ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ប្រសិនបើ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } បើមិនដូច្នេះ ប្រសិនបើ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } បើមិនដូច្នេះ {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                ប្រសិនបើ (w)
                        mode = S_IFBLK;
                បើមិនដូច្នេះ {
                        w = path_startswith(path, "/dev/char/");
                        ប្រសិនបើ (!w)
                                ត្រឡប់ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        ប្រសិនបើ (ret_mode)
                *ret_mode = mode;
        ប្រសិនបើ (ret_devnum)
                *ret_devnum = devnum;

        ត្រឡប់ 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ត្រឡប់ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
