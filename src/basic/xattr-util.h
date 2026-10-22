/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ getxattr_at_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ at_flags, ವರ್ಣ **ret, size_t *ret_size);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ getxattr_malloc(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ lgetxattr_malloc(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fgetxattr_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ getxattr_at_malloc(fd, ರಿಕ್ತ, name, AT_EMPTY_PATH, ret, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ getxattr_at_bool(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ at_flags);
ಪೂರ್ಣಾಙ್ಕ getxattr_at_strv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ at_flags, ವರ್ಣ ***ret_strv);

ಪೂರ್ಣಾಙ್ಕ listxattr_at_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ listxattr_malloc(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ llistxattr_malloc(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ flistxattr_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ listxattr_at_malloc(fd, ರಿಕ್ತ, AT_EMPTY_PATH, ret);
}

ಪೂರ್ಣಾಙ್ಕ xsetxattr_full(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                size_t size,
                ಪೂರ್ಣಾಙ್ಕ xattr_flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xsetxattr(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ xsetxattr_full(fd, path, at_flags, name, ಮೌಲ್ಯ, SIZE_MAX, 0);
}

ಪೂರ್ಣಾಙ್ಕ xsetxattr_strv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ * ಧ್ರುವ *l);

ಪೂರ್ಣಾಙ್ಕ xremovexattr(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ಧ್ರುವ ವರ್ಣ *name);

ಪೂರ್ಣಾಙ್ಕ fd_setcrtime(ಪೂರ್ಣಾಙ್ಕ fd, usec_t usec);
ಪೂರ್ಣಾಙ್ಕ getcrtime_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, usec_t *ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_getcrtime(ಪೂರ್ಣಾಙ್ಕ fd, usec_t *ret) {
        ಪ್ರತ್ಯಾಗಮ getcrtime_at(fd, ರಿಕ್ತ, 0, ret);
}

bool xattr_is_acl(ಧ್ರುವ ವರ್ಣ *name);
bool xattr_is_selinux(ಧ್ರುವ ವರ್ಣ *name);
