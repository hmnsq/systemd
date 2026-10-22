/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "devnum-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ parse_devnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, dev_t *ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *major;
        ಚಿಹ್ನೆರಹಿತ x, y;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        n = strspn(s, DIGITS);
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (n > DECIMAL_STR_MAX(dev_t))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (s[n] != ':')
                ಹಿಂತಿರುಗಿ -EINVAL;

        major = strndupa_ಸುರಕ್ಷಿತ(s, n);
        r = ಸುರಕ್ಷಿತ_atou(major, &x);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atou(s + n + 1, &y);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = makedev(x, y);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_major_minor(mode_t ಬಹುಲಕ, dev_t devnum, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ಆದರೆ (S_ISCHR(ಬಹುಲಕ))
                t = "char";
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISBLK(ಬಹುಲಕ))
                t = "block";
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -ENODEV;

        ಆದರೆ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_inaccessible(mode_t ಬಹುಲಕ, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (S_ISCHR(ಬಹುಲಕ))
                s = "/run/systemd/inaccessible/chr";
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISBLK(ಬಹುಲಕ))
                s = "/run/systemd/inaccessible/blk";
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -ENODEV;

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, s);
}

ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_canonical(mode_t ಬಹುಲಕ, dev_t devnum, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                ಹಿಂತಿರುಗಿ device_ಮಾರ್ಗ_make_inaccessible(ಬಹುಲಕ, ret);

        r = device_ಮಾರ್ಗ_make_major_minor(ಬಹುಲಕ, devnum, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ chase(p, ಖಾಲಿ, 0, ret, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_parse_major_minor(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t *ret_ಬಹುಲಕ, dev_t *ret_devnum) {
        mode_t ಬಹುಲಕ;
        dev_t devnum;
        ಪೂರ್ಣಾಂಕ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ಆದರೆ (ಮಾರ್ಗ_equal(ಮಾರ್ಗ, "/run/systemd/inaccessible/chr")) {
                ಬಹುಲಕ = S_IFCHR;
                devnum = makedev(0, 0);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮಾರ್ಗ_equal(ಮಾರ್ಗ, "/run/systemd/inaccessible/blk")) {
                ಬಹುಲಕ = S_IFBLK;
                devnum = makedev(0, 0);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *w;

                w = ಮಾರ್ಗ_startswith(ಮಾರ್ಗ, "/dev/block/");
                ಆದರೆ (w)
                        ಬಹುಲಕ = S_IFBLK;
                ಇಲ್ಲದಿದ್ದರೆ {
                        w = ಮಾರ್ಗ_startswith(ಮಾರ್ಗ, "/dev/char/");
                        ಆದರೆ (!w)
                                ಹಿಂತಿರುಗಿ -ENODEV;

                        ಬಹುಲಕ = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_ಬಹುಲಕ)
                *ret_ಬಹುಲಕ = ಬಹುಲಕ;
        ಆದರೆ (ret_devnum)
                *ret_devnum = devnum;

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* format_devnum(dev_t d, ಅಕ್ಷರ buf[ಸ್ಥಿರ DEVNUM_STR_MAX]) {
        ಹಿಂತಿರುಗಿ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
