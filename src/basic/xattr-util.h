/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ getxattr_at_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ at_flags, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ getxattr_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ getxattr_at_malloc(AT_FDCWD, ಮಾರ್ಗ, name, AT_SYMLINK_FOLLOW, ret, ret_ಗಾತ್ರ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ lgetxattr_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ getxattr_at_malloc(AT_FDCWD, ಮಾರ್ಗ, name, 0, ret, ret_ಗಾತ್ರ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fgetxattr_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ getxattr_at_malloc(fd, ಖಾಲಿ, name, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, ret, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ getxattr_at_bool(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ at_flags);
ಪೂರ್ಣಾಂಕ getxattr_at_strv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ at_flags, ಅಕ್ಷರ ***ret_strv);

ಪೂರ್ಣಾಂಕ listxattr_at_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ listxattr_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ listxattr_at_malloc(AT_FDCWD, ಮಾರ್ಗ, AT_SYMLINK_FOLLOW, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ llistxattr_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ listxattr_at_malloc(AT_FDCWD, ಮಾರ್ಗ, 0, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ flistxattr_malloc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ listxattr_at_malloc(fd, ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, ret);
}

ಪೂರ್ಣಾಂಕ xsetxattr_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                size_t ಗಾತ್ರ_2,
                ಪೂರ್ಣಾಂಕ xattr_flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xsetxattr(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ xsetxattr_ತುಂಬಿದ(fd, ಮಾರ್ಗ, at_flags, name, ಮೌಲ್ಯ, SIZE_MAX, 0);
}

ಪೂರ್ಣಾಂಕ xsetxattr_strv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l);

ಪೂರ್ಣಾಂಕ xremovexattr(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಪೂರ್ಣಾಂಕ fd_setcrtime(ಪೂರ್ಣಾಂಕ fd, usec_t usec);
ಪೂರ್ಣಾಂಕ getcrtime_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, usec_t *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_getcrtime(ಪೂರ್ಣಾಂಕ fd, usec_t *ret) {
        ಹಿಂತಿರುಗಿ getcrtime_at(fd, ಖಾಲಿ, 0, ret);
}

bool xattr_is_acl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
bool xattr_is_selinux(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
