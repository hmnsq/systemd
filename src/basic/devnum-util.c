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

पूर्णांक parse_devnum(स्थिरांक अक्षर *s, dev_t *ret) {
        स्थिरांक अक्षर *major;
        अचिन्हित x, y;
        माप_प्रकार n;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        n = म_अखोज(s, DIGITS);
        जर (n == 0)
                परत -EINVAL;
        जर (n > DECIMAL_STR_MAX(dev_t))
                परत -EINVAL;
        जर (s[n] != ':')
                परत -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        जर (r < 0)
                परत r;

        r = safe_atou(s + n + 1, &y);
        जर (r < 0)
                परत r;

        जर (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                परत -दुस्फल;

        *ret = makedev(x, y);
        परत 0;
}

पूर्णांक device_path_make_major_minor(mode_t mode, dev_t devnum, अक्षर **ret) {
        स्थिरांक अक्षर *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        जर (S_ISCHR(mode))
                t = "char";
        अन्यथा जर (S_ISBLK(mode))
                t = "block";
        अन्यथा
                परत -ENODEV;

        जर (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                परत -ENOMEM;

        परत 0;
}

पूर्णांक device_path_make_inaccessible(mode_t mode, अक्षर **ret) {
        स्थिरांक अक्षर *s;

        प्रतिज्ञा(ret);

        जर (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        अन्यथा जर (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        अन्यथा
                परत -ENODEV;

        परत strdup_to(ret, s);
}

पूर्णांक device_path_make_canonical(mode_t mode, dev_t devnum, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = रिक्त;
        पूर्णांक r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        प्रतिज्ञा(ret);

        जर (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                परत device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        जर (r < 0)
                परत r;

        परत chase(p, रिक्त, 0, ret, रिक्त);
}

पूर्णांक device_path_parse_major_minor(स्थिरांक अक्षर *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        पूर्णांक r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        जर (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } अन्यथा जर (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } अन्यथा {
                स्थिरांक अक्षर *w;

                w = path_startswith(path, "/dev/block/");
                जर (w)
                        mode = S_IFBLK;
                अन्यथा {
                        w = path_startswith(path, "/dev/char/");
                        जर (!w)
                                परत -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                जर (r < 0)
                        परत r;
        }

        जर (ret_mode)
                *ret_mode = mode;
        जर (ret_devnum)
                *ret_devnum = devnum;

        परत 0;
}

अक्षर* format_devnum(dev_t d, अक्षर buf[स्थिर DEVNUM_STR_MAX]) {
        परत ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
