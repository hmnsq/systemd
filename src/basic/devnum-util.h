/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ parse_devnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, dev_t *ret);

#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_MAJOR_MAX ((UINT32_C(1) << 12) - 1U)
#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_MINOR_MAX ((UINT32_C(1) << 20) - 1U)

/* glibc and the Linux kernel have different ideas about the major/minor size. These calls will check whether the
 * specified major is valid by the Linux kernel's standards, not by glibc's. Linux has 20bits of minor, and 12 bits of
 * major space. See MINORBITS in linux/kdev_t.h in the kernel sources. (If you wonder why we define _y here, instead of
 * comparing directly >= 0: it's to trick out -Wtype-limits, which would otherwise complain if the type is unsigned, as
 * such a test would be pointless in such a case.) */

#ವ್ಯಾಖ್ಯಾನಿಸು DEVICE_MAJOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MAJOR_MAX;                     \
                                                                        \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು DEVICE_MINOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MINOR_MAX;                     \
        })

ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_major_minor(mode_t ಬಹುಲಕ, dev_t devnum, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_inaccessible(mode_t ಬಹುಲಕ, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_make_canonical(mode_t ಬಹುಲಕ, dev_t devnum, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ device_ಮಾರ್ಗ_parse_major_minor(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t *ret_ಬಹುಲಕ, dev_t *ret_devnum);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool devnum_ಗಣ_and_equal(dev_t a, dev_t b) {
        /* Returns true if a and b definitely refer to the same device. If either is zero, this means "don't
         * know" and we'll return false */
        ಹಿಂತಿರುಗಿ a == b && a != 0;
}

/* Maximum string length for a major:minor string. (Note that DECIMAL_STR_MAX includes space for a trailing NUL) */
#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_STR_MAX (DECIMAL_STR_MAX(dev_t)-1+1+DECIMAL_STR_MAX(dev_t))

#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_FORMAT_STR "%u:%u"
#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_FORMAT_VAL(d) major(d), minor(d)

ಅಕ್ಷರ *format_devnum(dev_t d, ಅಕ್ಷರ buf[ಸ್ಥಿರ DEVNUM_STR_MAX]);

#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_DEVNUM(d) format_devnum((d), (ಅಕ್ಷರ[DEVNUM_STR_MAX]) {})

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool devnum_is_zero(dev_t d) {
        ಹಿಂತಿರುಗಿ major(d) == 0 && minor(d) == 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು DEVNUM_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ*) (uintptr_t) (u))
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_DEVNUM(p) ((dev_t) ((uintptr_t) (p)))

/* Convert a userspace dev_t (as returned by stat()) to the kernel's internal dev_t encoding. stat() returns
 * new_encode_dev(s_dev), while various kernel interfaces (e.g. the BPF sb helpers, or unix_diag's
 * udiag_vfs_dev) report s_dev directly, which uses MKDEV(major, minor) = (major << 20) | minor. */
#ವ್ಯಾಖ್ಯಾನಿಸು STAT_DEV_ಗಮ್ಯ_KERNEL(dev) \
        ((uint32_t) major(dev) << 20 | (uint32_t) minor(dev))
