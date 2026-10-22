/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/sysmacros.h>

#అన్తర్భావ "forward.h"

పూర్ణాంకం parse_devnum(స్థిరరాశి అక్షరం *s, dev_t *ret);

#నిర్వచించు DEVNUM_MAJOR_MAX ((UINT32_C(1) << 12) - 1U)
#నిర్వచించు DEVNUM_MINOR_MAX ((UINT32_C(1) << 20) - 1U)

/* glibc and the Linux kernel have different ideas about the major/minor size. These calls will check whether the
 * specified major is valid by the Linux kernel's standards, not by glibc's. Linux has 20bits of minor, and 12 bits of
 * major space. See MINORBITS in linux/kdev_t.h in the kernel sources. (If you wonder why we define _y here, instead of
 * comparing directly >= 0: it's to trick out -Wtype-limits, which would otherwise complain if the type is unsigned, as
 * such a test would be pointless in such a case.) */

#నిర్వచించు DEVICE_MAJOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MAJOR_MAX;                     \
                                                                        \
        })

#నిర్వచించు DEVICE_MINOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MINOR_MAX;                     \
        })

పూర్ణాంకం device_మార్గం_make_major_minor(mode_t బహుళకం, dev_t devnum, అక్షరం **ret);
పూర్ణాంకం device_మార్గం_make_inaccessible(mode_t బహుళకం, అక్షరం **ret);
పూర్ణాంకం device_మార్గం_make_canonical(mode_t బహుళకం, dev_t devnum, అక్షరం **ret);
పూర్ణాంకం device_మార్గం_parse_major_minor(స్థిరరాశి అక్షరం *మార్గం, mode_t *ret_బహుళకం, dev_t *ret_devnum);

స్థిర అంతఃవరుస bool devnum_సమితి_and_equal(dev_t a, dev_t b) {
        /* Returns true if a and b definitely refer to the same device. If either is zero, this means "don't
         * know" and we'll return false */
        తిరిగి a == b && a != 0;
}

/* Maximum string length for a major:minor string. (Note that DECIMAL_STR_MAX includes space for a trailing NUL) */
#నిర్వచించు DEVNUM_STR_MAX (DECIMAL_STR_MAX(dev_t)-1+1+DECIMAL_STR_MAX(dev_t))

#నిర్వచించు DEVNUM_FORMAT_STR "%u:%u"
#నిర్వచించు DEVNUM_FORMAT_VAL(d) major(d), minor(d)

అక్షరం *format_devnum(dev_t d, అక్షరం buf[స్థిర DEVNUM_STR_MAX]);

#నిర్వచించు FORMAT_DEVNUM(d) format_devnum((d), (అక్షరం[DEVNUM_STR_MAX]) {})

స్థిర అంతఃవరుస bool devnum_is_zero(dev_t d) {
        తిరిగి major(d) == 0 && minor(d) == 0;
}

#నిర్వచించు DEVNUM_గమ్యం_PTR(u) ((శూన్య*) (uintptr_t) (u))
#నిర్వచించు PTR_గమ్యం_DEVNUM(p) ((dev_t) ((uintptr_t) (p)))

/* Convert a userspace dev_t (as returned by stat()) to the kernel's internal dev_t encoding. stat() returns
 * new_encode_dev(s_dev), while various kernel interfaces (e.g. the BPF sb helpers, or unix_diag's
 * udiag_vfs_dev) report s_dev directly, which uses MKDEV(major, minor) = (major << 20) | minor. */
#నిర్వచించు STAT_DEV_గమ్యం_KERNEL(dev) \
        ((uint32_t) major(dev) << 20 | (uint32_t) minor(dev))
