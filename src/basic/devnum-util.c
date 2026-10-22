/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <sys/sysmacros.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "devnum-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"

முழு parse_devnum(மாறிலி எழுத்து *s, dev_t *ret) {
        மாறிலி எழுத்து *major;
        unsigned x, y;
        size_t n;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        n = strspn(s, DIGITS);
        எனில் (n == 0)
                திரும்பு -EINVAL;
        எனில் (n > DECIMAL_STR_MAX(dev_t))
                திரும்பு -EINVAL;
        எனில் (s[n] != ':')
                திரும்பு -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        எனில் (r < 0)
                திரும்பு r;

        r = safe_atou(s + n + 1, &y);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                திரும்பு -ERANGE;

        *ret = makedev(x, y);
        திரும்பு 0;
}

முழு device_path_make_major_minor(mode_t mode, dev_t devnum, எழுத்து **ret) {
        மாறிலி எழுத்து *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        எனில் (S_ISCHR(mode))
                t = "char";
        இல்லையெனில் எனில் (S_ISBLK(mode))
                t = "block";
        இல்லையெனில்
                திரும்பு -ENODEV;

        எனில் (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                திரும்பு -ENOMEM;

        திரும்பு 0;
}

முழு device_path_make_inaccessible(mode_t mode, எழுத்து **ret) {
        மாறிலி எழுத்து *s;

        ப்ரதிஜ்ஞா(ret);

        எனில் (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        இல்லையெனில் எனில் (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        இல்லையெனில்
                திரும்பு -ENODEV;

        திரும்பு strdup_to(ret, s);
}

முழு device_path_make_canonical(mode_t mode, dev_t devnum, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ப்ரதிஜ்ஞா(ret);

        எனில் (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                திரும்பு device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு chase(p, வெற்று, 0, ret, வெற்று);
}

முழு device_path_parse_major_minor(மாறிலி எழுத்து *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        முழு r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        எனில் (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } இல்லையெனில் எனில் (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } இல்லையெனில் {
                மாறிலி எழுத்து *w;

                w = path_startswith(path, "/dev/block/");
                எனில் (w)
                        mode = S_IFBLK;
                இல்லையெனில் {
                        w = path_startswith(path, "/dev/char/");
                        எனில் (!w)
                                திரும்பு -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_mode)
                *ret_mode = mode;
        எனில் (ret_devnum)
                *ret_devnum = devnum;

        திரும்பு 0;
}

எழுத்து* format_devnum(dev_t d, எழுத்து buf[நிலையான DEVNUM_STR_MAX]) {
        திரும்பு ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
