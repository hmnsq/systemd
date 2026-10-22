/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <sys/sysmacros.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "devnum-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం parse_devnum(స్థిరరాశి అక్షరం *s, dev_t *ret) {
        స్థిరరాశి అక్షరం *major;
        గుర్తులేని x, y;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        n = strspn(s, DIGITS);
        ఒకవేళ (n == 0)
                తిరిగి -EINVAL;
        ఒకవేళ (n > DECIMAL_STR_MAX(dev_t))
                తిరిగి -EINVAL;
        ఒకవేళ (s[n] != ':')
                తిరిగి -EINVAL;

        major = strndupa_సురక్షిత(s, n);
        r = సురక్షిత_atou(major, &x);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = సురక్షిత_atou(s + n + 1, &y);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                తిరిగి -ERANGE;

        *ret = makedev(x, y);
        తిరిగి 0;
}

పూర్ణాంకం device_మార్గం_make_major_minor(mode_t బహుళకం, dev_t devnum, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        ఒకవేళ (S_ISCHR(బహుళకం))
                t = "char";
        లేకపోతే ఒకవేళ (S_ISBLK(బహుళకం))
                t = "block";
        లేకపోతే
                తిరిగి -ENODEV;

        ఒకవేళ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                తిరిగి -ENOMEM;

        తిరిగి 0;
}

పూర్ణాంకం device_మార్గం_make_inaccessible(mode_t బహుళకం, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *s;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (S_ISCHR(బహుళకం))
                s = "/run/systemd/inaccessible/chr";
        లేకపోతే ఒకవేళ (S_ISBLK(బహుళకం))
                s = "/run/systemd/inaccessible/blk";
        లేకపోతే
                తిరిగి -ENODEV;

        తిరిగి strdup_గమ్యం(ret, s);
}

పూర్ణాంకం device_మార్గం_make_canonical(mode_t బహుళకం, dev_t devnum, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                తిరిగి device_మార్గం_make_inaccessible(బహుళకం, ret);

        r = device_మార్గం_make_major_minor(బహుళకం, devnum, &p);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి chase(p, ఖాళీ, 0, ret, ఖాళీ);
}

పూర్ణాంకం device_మార్గం_parse_major_minor(స్థిరరాశి అక్షరం *మార్గం, mode_t *ret_బహుళకం, dev_t *ret_devnum) {
        mode_t బహుళకం;
        dev_t devnum;
        పూర్ణాంకం r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        ఒకవేళ (మార్గం_equal(మార్గం, "/run/systemd/inaccessible/chr")) {
                బహుళకం = S_IFCHR;
                devnum = makedev(0, 0);
        } లేకపోతే ఒకవేళ (మార్గం_equal(మార్గం, "/run/systemd/inaccessible/blk")) {
                బహుళకం = S_IFBLK;
                devnum = makedev(0, 0);
        } లేకపోతే {
                స్థిరరాశి అక్షరం *w;

                w = మార్గం_startswith(మార్గం, "/dev/block/");
                ఒకవేళ (w)
                        బహుళకం = S_IFBLK;
                లేకపోతే {
                        w = మార్గం_startswith(మార్గం, "/dev/char/");
                        ఒకవేళ (!w)
                                తిరిగి -ENODEV;

                        బహుళకం = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_బహుళకం)
                *ret_బహుళకం = బహుళకం;
        ఒకవేళ (ret_devnum)
                *ret_devnum = devnum;

        తిరిగి 0;
}

అక్షరం* format_devnum(dev_t d, అక్షరం buf[స్థిర DEVNUM_STR_MAX]) {
        తిరిగి ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
