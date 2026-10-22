/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/sysmacros.h>

#समावेश "forward.h"

पूर्णांक parse_devnum(स्थिर अक्षर *s, dev_t *ret);

#घोषणा DEVNUM_MAJOR_MAX ((UINT32_C(1) << 12) - 1U)
#घोषणा DEVNUM_MINOR_MAX ((UINT32_C(1) << 20) - 1U)

/* glibc and the Linux kernel have different ideas about the major/minor size. These calls will check whether the
 * specified major is valid by the Linux kernel's standards, not by glibc's. Linux has 20bits of minor, and 12 bits of
 * major space. See MINORBITS in linux/kdev_t.h in the kernel sources. (If you wonder why we define _y here, instead of
 * comparing directly >= 0: it's to trick out -Wtype-limits, which would otherwise complain if the type is unsigned, as
 * such a test would be pointless in such a case.) */

#घोषणा DEVICE_MAJOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MAJOR_MAX;                     \
                                                                        \
        })

#घोषणा DEVICE_MINOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x <= DEVNUM_MINOR_MAX;                     \
        })

पूर्णांक device_path_make_major_minor(mode_t mode, dev_t devnum, अक्षर **ret);
पूर्णांक device_path_make_inaccessible(mode_t mode, अक्षर **ret);
पूर्णांक device_path_make_canonical(mode_t mode, dev_t devnum, अक्षर **ret);
पूर्णांक device_path_parse_major_minor(स्थिर अक्षर *path, mode_t *ret_mode, dev_t *ret_devnum);

अटल अंतरभूत bool devnum_set_and_equal(dev_t a, dev_t b) {
        /* Returns true if a and b definitely refer to the same device. If either is zero, this means "don't
         * know" and we'll return false */
        वापस a == b && a != 0;
}

/* Maximum string length for a major:minor string. (Note that DECIMAL_STR_MAX includes space for a trailing NUL) */
#घोषणा DEVNUM_STR_MAX (DECIMAL_STR_MAX(dev_t)-1+1+DECIMAL_STR_MAX(dev_t))

#घोषणा DEVNUM_FORMAT_STR "%u:%u"
#घोषणा DEVNUM_FORMAT_VAL(d) major(d), minor(d)

अक्षर *format_devnum(dev_t d, अक्षर buf[अटल DEVNUM_STR_MAX]);

#घोषणा FORMAT_DEVNUM(d) format_devnum((d), (अक्षर[DEVNUM_STR_MAX]) {})

अटल अंतरभूत bool devnum_is_zero(dev_t d) {
        वापस major(d) == 0 && minor(d) == 0;
}

#घोषणा DEVNUM_TO_PTR(u) ((व्योम*) (uintptr_t) (u))
#घोषणा PTR_TO_DEVNUM(p) ((dev_t) ((uintptr_t) (p)))

/* Convert a userspace dev_t (as returned by stat()) to the kernel's internal dev_t encoding. stat() returns
 * new_encode_dev(s_dev), while various kernel interfaces (e.g. the BPF sb helpers, or unix_diag's
 * udiag_vfs_dev) report s_dev directly, which uses MKDEV(major, minor) = (major << 20) | minor. */
#घोषणा STAT_DEV_TO_KERNEL(dev) \
        ((uint32_t) major(dev) << 20 | (uint32_t) minor(dev))
