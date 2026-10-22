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

පූර්ණ_සංඛ්‍යා parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        නම් (n == 0)
                ආපසු -EINVAL;
        නම් (n > DECIMAL_STR_MAX(dev_t))
                ආපසු -EINVAL;
        නම් (s[n] != ':')
                ආපසු -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        නම් (r < 0)
                ආපසු r;

        r = safe_atou(s + n + 1, &y);
        නම් (r < 0)
                ආපසු r;

        නම් (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ආපසු -ERANGE;

        *ret = makedev(x, y);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        නම් (S_ISCHR(mode))
                t = "char";
        නැතහොත් නම් (S_ISBLK(mode))
                t = "block";
        නැතහොත්
                ආපසු -ENODEV;

        නම් (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ආපසු -ENOMEM;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        නම් (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        නැතහොත් නම් (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        නැතහොත්
                ආපසු -ENODEV;

        ආපසු strdup_to(ret, s);
}

පූර්ණ_සංඛ්‍යා device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        පූර්ණ_සංඛ්‍යා r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        නම් (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ආපසු device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        නම් (r < 0)
                ආපසු r;

        ආපසු chase(p, NULL, 0, ret, NULL);
}

පූර්ණ_සංඛ්‍යා device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        පූර්ණ_සංඛ්‍යා r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        නම් (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } නැතහොත් නම් (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } නැතහොත් {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                නම් (w)
                        mode = S_IFBLK;
                නැතහොත් {
                        w = path_startswith(path, "/dev/char/");
                        නම් (!w)
                                ආපසු -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                නම් (r < 0)
                        ආපසු r;
        }

        නම් (ret_mode)
                *ret_mode = mode;
        නම් (ret_devnum)
                *ret_devnum = devnum;

        ආපසු 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ආපසු ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
