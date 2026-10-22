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

पूर्णाङ्क parse_devnum(ध्रुव वर्ण *s, dev_t *ret) {
        ध्रुव वर्ण *major;
        unsigned x, y;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        n = strspn(s, DIGITS);
        सचे (n == 0)
                पटिनिवत्त -EINVAL;
        सचे (n > DECIMAL_STR_MAX(dev_t))
                पटिनिवत्त -EINVAL;
        सचे (s[n] != ':')
                पटिनिवत्त -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        सचे (r < 0)
                पटिनिवत्त r;

        r = safe_atou(s + n + 1, &y);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                पटिनिवत्त -ERANGE;

        *ret = makedev(x, y);
        पटिनिवत्त 0;
}

पूर्णाङ्क device_path_make_major_minor(mode_t mode, dev_t devnum, वर्ण **ret) {
        ध्रुव वर्ण *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        सचे (S_ISCHR(mode))
                t = "char";
        अञ्ञथा सचे (S_ISBLK(mode))
                t = "block";
        अञ्ञथा
                पटिनिवत्त -ENODEV;

        सचे (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त 0;
}

पूर्णाङ्क device_path_make_inaccessible(mode_t mode, वर्ण **ret) {
        ध्रुव वर्ण *s;

        प्रतिज्ञा(ret);

        सचे (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        अञ्ञथा सचे (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        अञ्ञथा
                पटिनिवत्त -ENODEV;

        पटिनिवत्त strdup_to(ret, s);
}

पूर्णाङ्क device_path_make_canonical(mode_t mode, dev_t devnum, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        प्रतिज्ञा(ret);

        सचे (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                पटिनिवत्त device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त chase(p, रिक्त, 0, ret, रिक्त);
}

पूर्णाङ्क device_path_parse_major_minor(ध्रुव वर्ण *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        पूर्णाङ्क r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        सचे (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } अञ्ञथा सचे (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } अञ्ञथा {
                ध्रुव वर्ण *w;

                w = path_startswith(path, "/dev/block/");
                सचे (w)
                        mode = S_IFBLK;
                अञ्ञथा {
                        w = path_startswith(path, "/dev/char/");
                        सचे (!w)
                                पटिनिवत्त -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (ret_mode)
                *ret_mode = mode;
        सचे (ret_devnum)
                *ret_devnum = devnum;

        पटिनिवत्त 0;
}

वर्ण* format_devnum(dev_t d, वर्ण buf[स्थिर DEVNUM_STR_MAX]) {
        पटिनिवत्त ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
