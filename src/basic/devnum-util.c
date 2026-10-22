/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <sys/sysmacros.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "devnum-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क parse_devnum(अचर वर्ण *s, dev_t *ret) {
        अचर वर्ण *major;
        अचिन्हित x, y;
        माप_प्रकार n;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        n = म_अखोज(s, DIGITS);
        यदि (n == 0)
                फर्काऊ -EINVAL;
        यदि (n > DECIMAL_STR_MAX(dev_t))
                फर्काऊ -EINVAL;
        यदि (s[n] != ':')
                फर्काऊ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        यदि (r < 0)
                फर्काऊ r;

        r = safe_atou(s + n + 1, &y);
        यदि (r < 0)
                फर्काऊ r;

        यदि (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                फर्काऊ -दुस्फल;

        *ret = makedev(x, y);
        फर्काऊ 0;
}

पूर्णाङ्क device_path_make_major_minor(mode_t mode, dev_t devnum, वर्ण **ret) {
        अचर वर्ण *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        यदि (S_ISCHR(mode))
                t = "char";
        अन्यथा यदि (S_ISBLK(mode))
                t = "block";
        अन्यथा
                फर्काऊ -ENODEV;

        यदि (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ 0;
}

पूर्णाङ्क device_path_make_inaccessible(mode_t mode, वर्ण **ret) {
        अचर वर्ण *s;

        प्रतिज्ञा(ret);

        यदि (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        अन्यथा यदि (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        अन्यथा
                फर्काऊ -ENODEV;

        फर्काऊ strdup_to(ret, s);
}

पूर्णाङ्क device_path_make_canonical(mode_t mode, dev_t devnum, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        प्रतिज्ञा(ret);

        यदि (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                फर्काऊ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ chase(p, रिक्त, 0, ret, रिक्त);
}

पूर्णाङ्क device_path_parse_major_minor(अचर वर्ण *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        पूर्णाङ्क r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        यदि (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } अन्यथा यदि (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } अन्यथा {
                अचर वर्ण *w;

                w = path_startswith(path, "/dev/block/");
                यदि (w)
                        mode = S_IFBLK;
                अन्यथा {
                        w = path_startswith(path, "/dev/char/");
                        यदि (!w)
                                फर्काऊ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                यदि (r < 0)
                        फर्काऊ r;
        }

        यदि (ret_mode)
                *ret_mode = mode;
        यदि (ret_devnum)
                *ret_devnum = devnum;

        फर्काऊ 0;
}

वर्ण* format_devnum(dev_t d, वर्ण buf[स्थिर DEVNUM_STR_MAX]) {
        फर्काऊ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
