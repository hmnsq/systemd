/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <string.h>
#समावेश <sys/stat.h>
#समावेश <sys/sysmacros.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "devnum-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"

पूर्णांक parse_devnum(स्थिर अक्षर *s, dev_t *ret) {
        स्थिर अक्षर *major;
        अचिन्हित x, y;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret);

        n = म_अखोज(s, DIGITS);
        अगर (n == 0)
                वापस -EINVAL;
        अगर (n > DECIMAL_STR_MAX(dev_t))
                वापस -EINVAL;
        अगर (s[n] != ':')
                वापस -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        अगर (r < 0)
                वापस r;

        r = safe_atou(s + n + 1, &y);
        अगर (r < 0)
                वापस r;

        अगर (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                वापस -दुस्फल;

        *ret = makedev(x, y);
        वापस 0;
}

पूर्णांक device_path_make_major_minor(mode_t mode, dev_t devnum, अक्षर **ret) {
        स्थिर अक्षर *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        अगर (S_ISCHR(mode))
                t = "char";
        अन्यथा अगर (S_ISBLK(mode))
                t = "block";
        अन्यथा
                वापस -ENODEV;

        अगर (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                वापस -ENOMEM;

        वापस 0;
}

पूर्णांक device_path_make_inaccessible(mode_t mode, अक्षर **ret) {
        स्थिर अक्षर *s;

        निश्चित(ret);

        अगर (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        अन्यथा अगर (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        अन्यथा
                वापस -ENODEV;

        वापस strdup_to(ret, s);
}

पूर्णांक device_path_make_canonical(mode_t mode, dev_t devnum, अक्षर **ret) {
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        निश्चित(ret);

        अगर (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                वापस device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        अगर (r < 0)
                वापस r;

        वापस chase(p, शून्य, 0, ret, शून्य);
}

पूर्णांक device_path_parse_major_minor(स्थिर अक्षर *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        पूर्णांक r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        अगर (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } अन्यथा अगर (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } अन्यथा {
                स्थिर अक्षर *w;

                w = path_startswith(path, "/dev/block/");
                अगर (w)
                        mode = S_IFBLK;
                अन्यथा {
                        w = path_startswith(path, "/dev/char/");
                        अगर (!w)
                                वापस -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_mode)
                *ret_mode = mode;
        अगर (ret_devnum)
                *ret_devnum = devnum;

        वापस 0;
}

अक्षर* format_devnum(dev_t d, अक्षर buf[अटल DEVNUM_STR_MAX]) {
        वापस ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
