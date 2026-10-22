/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <sys/sysmacros.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "devnum-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ parse_devnum(സ്ഥിരം അക്ഷരം *s, dev_t *ret) {
        സ്ഥിരം അക്ഷരം *major;
        unsigned x, y;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        n = strspn(s, DIGITS);
        എങ്കിൽ (n == 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (n > DECIMAL_STR_MAX(dev_t))
                തിരികെ -EINVAL;
        എങ്കിൽ (s[n] != ':')
                തിരികെ -EINVAL;

        major = strndupa_safe(s, n);
        r = safe_atou(major, &x);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = safe_atou(s + n + 1, &y);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!DEVICE_MAJOR_VALID(x) || !DEVICE_MINOR_VALID(y))
                തിരികെ -ERANGE;

        *ret = makedev(x, y);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ device_path_make_major_minor(mode_t mode, dev_t devnum, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *t;

        /* Generates the /dev/{char|block}/MAJOR:MINOR path for a dev_t */

        എങ്കിൽ (S_ISCHR(mode))
                t = "char";
        അല്ലെങ്കിൽ എങ്കിൽ (S_ISBLK(mode))
                t = "block";
        അല്ലെങ്കിൽ
                തിരികെ -ENODEV;

        എങ്കിൽ (asprintf(ret, "/dev/%s/" DEVNUM_FORMAT_STR, t, DEVNUM_FORMAT_VAL(devnum)) < 0)
                തിരികെ -ENOMEM;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ device_path_make_inaccessible(mode_t mode, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *s;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (S_ISCHR(mode))
                s = "/run/systemd/inaccessible/chr";
        അല്ലെങ്കിൽ എങ്കിൽ (S_ISBLK(mode))
                s = "/run/systemd/inaccessible/blk";
        അല്ലെങ്കിൽ
                തിരികെ -ENODEV;

        തിരികെ strdup_to(ret, s);
}

പൂർണ്ണസംഖ്യ device_path_make_canonical(mode_t mode, dev_t devnum, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Finds the canonical path for a device, i.e. resolves the /dev/{char|block}/MAJOR:MINOR path to the end. */

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (devnum_is_zero(devnum))
                /* A special hack to make sure our 'inaccessible' device nodes work. They won't have symlinks in
                 * /dev/block/ and /dev/char/, hence we handle them specially here. */
                തിരികെ device_path_make_inaccessible(mode, ret);

        r = device_path_make_major_minor(mode, devnum, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ chase(p, ശൂന്യ, 0, ret, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ device_path_parse_major_minor(സ്ഥിരം അക്ഷരം *path, mode_t *ret_mode, dev_t *ret_devnum) {
        mode_t mode;
        dev_t devnum;
        പൂർണ്ണസംഖ്യ r;

        /* Tries to extract the major/minor directly from the device path if we can. Handles /dev/block/ and /dev/char/
         * paths, as well out synthetic inaccessible device nodes. Never goes to disk. Returns -ENODEV if the device
         * path cannot be parsed like this.  */

        എങ്കിൽ (path_equal(path, "/run/systemd/inaccessible/chr")) {
                mode = S_IFCHR;
                devnum = makedev(0, 0);
        } അല്ലെങ്കിൽ എങ്കിൽ (path_equal(path, "/run/systemd/inaccessible/blk")) {
                mode = S_IFBLK;
                devnum = makedev(0, 0);
        } അല്ലെങ്കിൽ {
                സ്ഥിരം അക്ഷരം *w;

                w = path_startswith(path, "/dev/block/");
                എങ്കിൽ (w)
                        mode = S_IFBLK;
                അല്ലെങ്കിൽ {
                        w = path_startswith(path, "/dev/char/");
                        എങ്കിൽ (!w)
                                തിരികെ -ENODEV;

                        mode = S_IFCHR;
                }

                r = parse_devnum(w, &devnum);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_mode)
                *ret_mode = mode;
        എങ്കിൽ (ret_devnum)
                *ret_devnum = devnum;

        തിരികെ 0;
}

അക്ഷരം* format_devnum(dev_t d, അക്ഷരം buf[സ്ഥിതം DEVNUM_STR_MAX]) {
        തിരികെ ASSERT_PTR(snprintf_ok(buf, DEVNUM_STR_MAX, DEVNUM_FORMAT_STR, DEVNUM_FORMAT_VAL(d)));
}
