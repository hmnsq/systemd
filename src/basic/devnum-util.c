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

puur_naa_nka parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        puur_naa_nka r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        ᬬᬾᬦ᭄ (n == 0)
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (n > DECIMAL_STR_MAX(dev_t))
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (s[n] != ':')
                ᬯᬮᬶ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        r = safe_atou(s + n + 1, &y);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ᬯᬮᬶ -ERANGE;

        *ret = makedev(x, y);
        ᬯᬮᬶ 0;
}

puur_naa_nka device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ᬬᬾᬦ᭄ (S_ISCHR(mode))
                t = "char";
        ᬮᬶᬬ ᬬᬾᬦ᭄ (S_ISBLK(mode))
                t = "block";
        ᬮᬶᬬ
                ᬯᬮᬶ -ENODEV;

        ᬬᬾᬦ᭄ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ᬯᬮᬶ -ENOMEM;

        ᬯᬮᬶ 0;
}

puur_naa_nka device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        ᬬᬾᬦ᭄ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        ᬮᬶᬬ ᬬᬾᬦ᭄ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        ᬮᬶᬬ
                ᬯᬮᬶ -ENODEV;

        ᬯᬮᬶ strdup_to(ret, s);
}

puur_naa_nka device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        ᬬᬾᬦ᭄ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ᬯᬮᬶ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ chase(p, NULL, 0, ret, NULL);
}

puur_naa_nka device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        puur_naa_nka r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ᬬᬾᬦ᭄ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } ᬮᬶᬬ {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                ᬬᬾᬦ᭄ (w)
                        mode = S_IFBLK;
                ᬮᬶᬬ {
                        w = path_startswith(path, "/dev/char/");
                        ᬬᬾᬦ᭄ (!w)
                                ᬯᬮᬶ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
        }

        ᬬᬾᬦ᭄ (ret_mode)
                *ret_mode = mode;
        ᬬᬾᬦ᭄ (ret_devnum)
                *ret_devnum = devnum;

        ᬯᬮᬶ 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ᬯᬮᬶ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
