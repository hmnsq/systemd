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
        ᤀᤡ᤺ (n == 0)
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (n > DECIMAL_STR_MAX(dev_t))
                ᤀᤡᤴ -EINVAL;
        ᤀᤡ᤺ (s[n] != ':')
                ᤀᤡᤴ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        r = safe_atou(s + n + 1, &y);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ᤀᤡᤴ -ERANGE;

        *ret = makedev(x, y);
        ᤀᤡᤴ 0;
}

puur_naa_nka device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ᤀᤡ᤺ (S_ISCHR(mode))
                t = "char";
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (S_ISBLK(mode))
                t = "block";
        ᤀᤡ᤺ᤆᤡ
                ᤀᤡᤴ -ENODEV;

        ᤀᤡ᤺ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ᤀᤡᤴ -ENOMEM;

        ᤀᤡᤴ 0;
}

puur_naa_nka device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        ᤀᤡ᤺ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        ᤀᤡ᤺ᤆᤡ
                ᤀᤡᤴ -ENODEV;

        ᤀᤡᤴ strdup_to(ret, s);
}

puur_naa_nka device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        ᤀᤡ᤺ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ᤀᤡᤴ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡᤴ chase(p, NULL, 0, ret, NULL);
}

puur_naa_nka device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        puur_naa_nka r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ᤀᤡ᤺ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } ᤀᤡ᤺ᤆᤡ {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                ᤀᤡ᤺ (w)
                        mode = S_IFBLK;
                ᤀᤡ᤺ᤆᤡ {
                        w = path_startswith(path, "/dev/char/");
                        ᤀᤡ᤺ (!w)
                                ᤀᤡᤴ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
        }

        ᤀᤡ᤺ (ret_mode)
                *ret_mode = mode;
        ᤀᤡ᤺ (ret_devnum)
                *ret_devnum = devnum;

        ᤀᤡᤴ 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ᤀᤡᤴ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
