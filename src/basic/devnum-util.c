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

정수 parse_devnum(const char *s, dev_t *ret) {
        const char *major;
        unsigned x, y;
        size_t n;
        정수 r;

        assert(s);
        assert(ret);

        n = strspn(s, DIGITS);
        만약 (n == 0)
                반환 -EINVAL;
        만약 (n > DECIMAL_STR_MAX(dev_t))
                반환 -EINVAL;
        만약 (s[n] != ':')
                반환 -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        만약 (r < 0)
                반환 r;

        r = safe_atou(s + n + 1, &y);
        만약 (r < 0)
                반환 r;

        만약 (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                반환 -ERANGE;

        *ret = makedev(x, y);
        반환 0;
}

정수 device_path_make_major_minor(mode_t mode, dev_t devnum, char **ret) {
        const char *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        만약 (S_ISCHR(mode))
                t = "char";
        아니면 만약 (S_ISBLK(mode))
                t = "block";
        아니면
                반환 -ENODEV;

        만약 (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                반환 -ENOMEM;

        반환 0;
}

정수 device_path_make_inaccessible(mode_t mode, char **ret) {
        const char *s;

        assert(ret);

        만약 (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        아니면 만약 (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        아니면
                반환 -ENODEV;

        반환 strdup_to(ret, s);
}

정수 device_path_make_canonical(mode_t mode, dev_t devnum, char **ret) {
        _cleanup_free_ char *p = NULL;
        정수 r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        assert(ret);

        만약 (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                반환 device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        만약 (r < 0)
                반환 r;

        반환 chase(p, NULL, 0, ret, NULL);
}

정수 device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        정수 r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        만약 (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } 아니면 만약 (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } 아니면 {
                const char *w;

                w = path_startswith(path, "/dev/block/");
                만약 (w)
                        mode = S_IFBLK;
                아니면 {
                        w = path_startswith(path, "/dev/char/");
                        만약 (!w)
                                반환 -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                만약 (r < 0)
                        반환 r;
        }

        만약 (ret_mode)
                *ret_mode = mode;
        만약 (ret_devnum)
                *ret_devnum = devnum;

        반환 0;
}

char* format_devnum(dev_t d, char buf[static DEVNUM_STR_MAX]) {
        반환 ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
