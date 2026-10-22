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

ကိန်းပြည့် parse_devnum(const var_na *s, dev_t *ret) {
        const var_na *major;
        unsigned x, y;
        size_t n;
        ကိန်းပြည့် r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        အကယ်၍ (n == 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (n > DECIMAL_STR_MAX(dev_t))
                ပြန်ပေး -EINVAL;
        အကယ်၍ (s[n] != ':')
                ပြန်ပေး -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = safe_atou(s + n + 1, &y);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ပြန်ပေး -ERANGE;

        *ret = makedev(x, y);
        ပြန်ပေး 0;
}

ကိန်းပြည့် device_path_make_major_minor(mode_t mode, dev_t devnum, var_na **ret) {
        const var_na *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        အကယ်၍ (S_ISCHR(mode))
                t = "char";
        မဟုတ်ပါက အကယ်၍ (S_ISBLK(mode))
                t = "block";
        မဟုတ်ပါက
                ပြန်ပေး -ENODEV;

        အကယ်၍ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး 0;
}

ကိန်းပြည့် device_path_make_inaccessible(mode_t mode, var_na **ret) {
        const var_na *s;

        assert(ret);

        အကယ်၍ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        မဟုတ်ပါက အကယ်၍ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        မဟုတ်ပါက
                ပြန်ပေး -ENODEV;

        ပြန်ပေး strdup_to(ret, s);
}

ကိန်းပြည့် device_path_make_canonical(mode_t mode, dev_t devnum, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        ကိန်းပြည့် r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        အကယ်၍ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ပြန်ပေး device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး chase(p, NULL, 0, ret, NULL);
}

ကိန်းပြည့် device_path_parse_major_minor(const var_na *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        ကိန်းပြည့် r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        အကယ်၍ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } မဟုတ်ပါက အကယ်၍ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } မဟုတ်ပါက {
                const var_na *w;

                w = path_startswith(path, "/dev/block/");
                အကယ်၍ (w)
                        mode = S_IFBLK;
                မဟုတ်ပါက {
                        w = path_startswith(path, "/dev/char/");
                        အကယ်၍ (!w)
                                ပြန်ပေး -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (ret_mode)
                *ret_mode = mode;
        အကယ်၍ (ret_devnum)
                *ret_devnum = devnum;

        ပြန်ပေး 0;
}

var_na* format_devnum(dev_t d, var_na buf[static DEVNUM_STR_MAX]) {
        ပြန်ပေး ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
